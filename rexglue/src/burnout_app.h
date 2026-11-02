// burnout - ReXGlue Recompiled Project
//
// Customize your app by overriding virtual hooks from rex::ReXApp.

#pragma once

#include <rex/cvar.h>
#include <rex/logging.h>
#include <rex/rex_app.h>

#include "online_lobby.h"
#include "plume_d3d.h"
#include "xerenge_settings.h"

void XerengeAchievementToastStart();

#ifdef XERENGE_PS5
REXCVAR_DECLARE(std::string, game_data_root);
#endif

class BurnoutApp : public rex::ReXApp {
 public:
  using rex::ReXApp::ReXApp;

  static std::unique_ptr<rex::ui::WindowedApp> Create(
      rex::ui::WindowedAppContext& ctx) {
    return std::unique_ptr<BurnoutApp>(new BurnoutApp(ctx, "burnout",
        PPCImageConfig));
  }

  void OnPreSetup(rex::RuntimeConfig& config) override {
    xerenge::settings::ApplyAtStart();
#ifdef XERENGE_PS5
    config.gpu_plugin = "plume";
#else
    (void)config;
#endif
  }

#ifdef XERENGE_PS5
  // No arguments on the console: the disc's files and the saves have one place.
  void OnConfigurePaths(rex::PathConfig& paths) override {
    if (paths.game_data_root.empty()) {
      paths.game_data_root = "/data/xerenge/game";
    }
    // The mods, the language and xerenge.conf find the game by the cvar, as the
    // other platforms' --game_data_root sets it; OnPreSetup comes after this.
    REXCVAR_SET(game_data_root, paths.game_data_root.string());
    paths.user_data_root = "/data/xerenge/user";
    paths.cache_root = "/data/xerenge/cache";
  }
#endif

  // Which build this is, first in the log: a report then says it by itself.
  void OnPostInitLogging() override { REXLOG_INFO("Xerenge {} ({})", XERENGE_VERSION, XERENGE_COMMIT); }

  void OnPostSetup() override {
    XerengeAchievementToastStart();
    xerenge::plume_d3d::InstallPlumeRuntimeHooks();
    // --online: this copy's lobby is up (and heard on the LAN) from the start,
    // not only once its own player signs in.
    xerenge::online::StartLobbyNetwork();
  }
  // void OnCreateDialogs(rex::ui::ImGuiDrawer* drawer) override {}
  // std::unique_ptr<rex::ui::ImGuiDialog> CreateAchievementsOverlay() override;
  // std::unique_ptr<rex::ui::AchievementNotificationDialog>
  // CreateAchievementNotificationDialog() override;
  // void OnShutdown() override {}
  // void OnConfigurePaths(rex::PathConfig& paths) override {}
};
