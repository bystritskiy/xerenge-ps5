// --online: which lobby server the title logs in to.
//
// Burnout finds EA's lobby as an Xbox Live title service (XOnlineGetServiceInfo
// for 0x45410004), so the choice is the address the SDK answers that lookup
// with, made the first time the title asks (during its "Signing in to Xbox
// Live"):
//
//   --lobby_server=<host>   a real server of your own (tools/ealobby) - at
//                           home, on a VPS. It is the only lobby then: no
//                           lobby of this copy is started. The title always
//                           uses its ports 31860/1.
//   otherwise               every copy runs a lobby of its own and the title
//                           always logs in to that one. The lobbies find each
//                           other on the LAN (UDP 31859: a broadcast and a
//                           multicast group that also reaches other copies on
//                           this machine): a copy announces the games it
//                           hosts, the others list them in their searches,
//                           and a player who joins one stays connected to its
//                           own lobby, which carries the game's traffic to
//                           the host's (ealobby::Server, "seamless
//                           multiplayer"). No copy is special and none has to
//                           be started first.
//
// Two copies on one machine can do the same with --online_address=127.0.0.2
// and 127.0.0.3; each then serves on its own address.

#include "online_lobby.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <atomic>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <string>

#include <rex/cvar.h>
#include <rex/kernel/xam/online.h>
#include <rex/logging.h>

#include "ealobby/server.h"

#ifdef XERENGE_PS5
#include "xerenge_settings.h"

extern "C" {
int sceNetInit(void);
int sceNetPoolCreate(const char* name, int size, int flags);
int sceNetPoolDestroy(int pool);
int sceNetResolverCreate(const char* name, int pool, int flags);
int sceNetResolverStartNtoa(int resolver, const char* host, uint32_t* address, int timeout,
                            int retries, int flags);
int sceNetResolverDestroy(int resolver);
int sceUserServiceInitialize(const void* params);
int sceUserServiceGetInitialUser(int* user);
int sceUserServiceGetUserName(int user, char* name, size_t size);
}
#endif

REXCVAR_DECLARE(bool, online);
REXCVAR_DECLARE(std::string, online_address);
REXCVAR_DEFINE_STRING(lobby_server, "", "Network",
                      "With --online: the host running a real lobby server (tools/ealobby). Empty: "
                      "run one in this game; the copies on the LAN find each other's games");
#ifdef XERENGE_PS5
REXCVAR_DECLARE(bool, online_fake_lobby);
REXCVAR_DECLARE(std::string, gamertag);
#endif

namespace {

std::string ToString(uint32_t address) {
  char text[INET_ADDRSTRLEN] = "?";
  inet_ntop(AF_INET, &address, text, sizeof(text));
  return text;
}

// --lobby_server: the name or address, as an IPv4 in network order; 0 if it
// cannot be resolved.
uint32_t Resolve(std::string host) {
  if (const size_t colon = host.find(':'); colon != std::string::npos) {
    REXLOG_WARN("--online: the title always uses ports 31860/31861; ignoring '{}'",
                host.substr(colon));
    host.resize(colon);
  }
#ifdef XERENGE_PS5
  // On the console a name is asked of the system's resolver (libSceNet): the
  // SDK's getaddrinfo refuses every lookup in a title.
  if (in_addr literal{}; inet_pton(AF_INET, host.c_str(), &literal) == 1) {
    return literal.s_addr;
  }
  sceNetInit();
  uint32_t found_address = 0;
  int result = sceNetPoolCreate("xerenge_dns", 16 * 1024, 0);
  if (const int pool = result; pool >= 0) {
    result = sceNetResolverCreate("xerenge_dns", pool, 0);
    if (const int resolver = result; resolver >= 0) {
      result = sceNetResolverStartNtoa(resolver, host.c_str(), &found_address, 0, 0, 0);
      sceNetResolverDestroy(resolver);
    }
    sceNetPoolDestroy(pool);
  }
  if (result >= 0 && found_address) {
    return found_address;
  }
  REXLOG_WARN("--online: the console's resolver did not find {} ({:08X})", host, uint32_t(result));
  return 0;
#else
  addrinfo hints{};
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  addrinfo* found = nullptr;
  if (getaddrinfo(host.c_str(), nullptr, &hints, &found) != 0 || !found) {
    return 0;
  }
  const uint32_t address = reinterpret_cast<sockaddr_in*>(found->ai_addr)->sin_addr.s_addr;
  freeaddrinfo(found);
  return address;
#endif
}

struct State {
  std::mutex mutex;
  uint32_t self = 0;  // network order: where this copy's lobby serves
  std::unique_ptr<ealobby::Server> server;
};

State& Lobby() {
  static State state;
  return state;
}

std::atomic<bool> g_started{false};

uint32_t ChooseLobby() {
  const std::string configured = REXCVAR_GET(lobby_server);
  if (!configured.empty()) {
    if (const uint32_t address = Resolve(configured)) {
      REXLOG_INFO("--online: using the lobby server at {} ({})", configured, ToString(address));
      return address;
    }
    REXLOG_ERROR("--online: cannot resolve --lobby_server={}; using this copy's own lobby instead",
                 configured);
  }
  xerenge::online::StartLobbyNetwork();
  std::lock_guard lock(Lobby().mutex);
  if (!Lobby().server) {
    REXLOG_WARN("--online: no lobby of this copy is running; trying 127.0.0.1");
    return htonl(INADDR_LOOPBACK);
  }
  REXLOG_INFO("--online: logging in to this copy's own lobby at {}", ToString(Lobby().self));
  return Lobby().self;
}

uint32_t LobbyAddress() {
  static std::once_flag once;
  static uint32_t address = 0;
  std::call_once(once, [] { address = ChooseLobby(); });
  return address;
}

[[maybe_unused]] const bool g_registered = [] {
  rex::kernel::xam::SetOnlineServiceAddressProvider([] {
    return REXCVAR_GET(online) ? LobbyAddress() : 0u;
  });
  return true;
}();

}  // namespace

namespace xerenge::online {

void StartLobbyNetwork() {
  if (!REXCVAR_GET(online) || !REXCVAR_GET(lobby_server).empty()) {
    return;
  }
  if (g_started.exchange(true)) {
    return;
  }
#ifdef _WIN32
  WSADATA wsa;
  WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
  const uint32_t self = rex::kernel::xam::OnlineLocalAddress();
  // With --online_address the lobby serves on that address alone, so that a
  // second copy on this machine can serve on another.
  ealobby::Options options;
  options.host = REXCVAR_GET(online_address).empty() ? "0.0.0.0" : ToString(self);
  options.self_address = ToString(self);
  auto server = std::make_unique<ealobby::Server>(
      options, [](const std::string& line) { REXLOG_INFO("lobby: {}", line); });
  const bool serving = server->Start();
  if (serving) {
    std::lock_guard lock(Lobby().mutex);
    Lobby().self = self;
    Lobby().server = std::move(server);
    REXLOG_INFO("--online: this copy's lobby is up at {} (ports 31860/31861)", ToString(self));
  } else {
    // Most often a standalone ealobby already on this machine.
    REXLOG_WARN("--online: could not start a lobby here (ports 31860/31861 in use?)");
  }
}

std::string LobbyServerAddress() {
  const std::string configured = REXCVAR_GET(lobby_server);
  const std::string host = configured.substr(0, configured.find(':'));
  if (host.empty()) {
    return {};
  }
  // Resolved once, on the first call.
  static const std::string address = [&host] {
#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
    const uint32_t resolved = Resolve(host);
    return resolved ? ToString(resolved) : host;
  }();
  return address;
}


std::string HttpGet(const std::string& host_port, const std::string& path) {
#ifdef _WIN32
  WSADATA wsa;
  WSAStartup(MAKEWORD(2, 2), &wsa);
  using Socket = SOCKET;
  const Socket kInvalid = INVALID_SOCKET;
  const auto close_socket = [](Socket s) { closesocket(s); };
#else
  using Socket = int;
  const Socket kInvalid = -1;
  const auto close_socket = [](Socket s) { close(s); };
#endif
  const size_t colon = host_port.find(':');
  const std::string host = host_port.substr(0, colon);
  const int port = colon == std::string::npos ? 80 : std::atoi(host_port.c_str() + colon + 1);
  const uint32_t address = Resolve(host);
  if (!address) {
    return {};
  }
  const Socket s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (s == kInvalid) {
    return {};
  }
  // A few seconds at most: the caller is the game's own thread.
#ifdef _WIN32
  DWORD timeout = 3000;
#else
  timeval timeout{3, 0};
#endif
  setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout));
  setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout));
  sockaddr_in to{};
  to.sin_family = AF_INET;
  to.sin_port = htons(uint16_t(port));
  to.sin_addr.s_addr = address;
  std::string response;
  if (connect(s, reinterpret_cast<const sockaddr*>(&to), sizeof(to)) == 0) {
    const std::string request = "GET " + path + " HTTP/1.1\r\nHost: " + host_port + "\r\nConnection: close\r\n\r\n";
    send(s, request.data(), int(request.size()), 0);
    char buffer[4096];
    for (;;) {
      const auto got = recv(s, buffer, sizeof(buffer), 0);
      if (got <= 0) {
        break;
      }
      response.append(buffer, size_t(got));
    }
  }
  close_socket(s);
  const size_t body = response.find("\r\n\r\n");
  if (response.compare(0, 12, "HTTP/1.1 200") != 0 || body == std::string::npos) {
    return {};
  }
  return response.substr(body + 4);
}

#ifdef XERENGE_PS5
void ApplyConf() {
  // A conf without the key is a new install's: the community server, as the
  // installer fills it in. An empty one is no server.
  const std::string server = xerenge::settings::ConfString("lobby_server", "94639.snk.wtf");
  if (server.empty() && xerenge::settings::ConfString("online", "false") != "true") {
    return;
  }
  REXCVAR_SET(online, true);
  REXCVAR_SET(online_fake_lobby, false);
  REXCVAR_SET(lobby_server, server);

  // The gamertag's letters (up to 15 of A-Z, a-z, 0-9, - and _), else those of
  // the console's user (the PSN online ID, for a signed-in one).
  std::string name = xerenge::settings::ConfString("gamertag", "");
  if (name.empty()) {
    sceUserServiceInitialize(nullptr);
    int user = -1;
    char user_name[64] = "";
    if (sceUserServiceGetInitialUser(&user) >= 0 &&
        sceUserServiceGetUserName(user, user_name, sizeof(user_name)) >= 0) {
      name = user_name;
    }
  }
  std::string gamertag;
  for (const char ch : name) {
    if (gamertag.size() < 15 && (std::isalnum(static_cast<unsigned char>(ch)) || ch == '-' || ch == '_')) {
      gamertag += ch;
    }
  }
  REXCVAR_SET(gamertag, gamertag);
  REXLOG_INFO("--online: server {}, gamertag {}", server.empty() ? std::string("none (LAN)") : server,
              gamertag.empty() ? std::string("Player") : gamertag);
}
#endif
}  // namespace xerenge::online
