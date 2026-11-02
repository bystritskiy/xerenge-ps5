// Xerenge PS5: what has to be in place before anything else runs.
// GPL-3.0-or-later.
//
// run.sh sets the game's switches on the other platforms; a title on the
// console starts with neither arguments nor an environment, so they are here.
#include "ps5_host.h"

#include <SDL3/SDL.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cxxabi.h>
#include <exception>
#include <fcntl.h>
#include <initializer_list>
#include <sys/stat.h>
#include <typeinfo>
#include <unistd.h>
#include <unwind.h>

extern "C" int sceKernelDebugOutText(int channel, const char* text);

namespace
{
    // A native title's stderr goes nowhere, and libc++abi says why it aborts
    // there and nowhere else. The klog line survives what the file may not.
    int s_crashLog = -1;
}

namespace ps5::startup
{
    void Say(const char* text)
    {
        sceKernelDebugOutText(0, text);
        if (s_crashLog >= 0)
            write(s_crashLog, text, std::strlen(text));
    }
}

using ps5::startup::Say;

namespace
{
    _Unwind_Reason_Code SayFrame(_Unwind_Context* context, void* count)
    {
        int& frames = *static_cast<int*>(count);
        char line[64];
        std::snprintf(line, sizeof(line), "[xerenge]   #%d %#llx\n", frames,
                      (unsigned long long)_Unwind_GetIP(context));
        Say(line);
        return ++frames < 48 ? _URC_NO_REASON : _URC_END_OF_STACK;
    }

    [[noreturn]] void Terminate()
    {
        char line[640];
        const std::type_info* type = abi::__cxa_current_exception_type();
        char what[512] = "";
        try
        {
            if (std::exception_ptr current = std::current_exception())
                std::rethrow_exception(current);
        }
        catch (const std::exception& error)
        {
            std::snprintf(what, sizeof(what), "%s", error.what());
        }
        catch (...)
        {
        }
        std::snprintf(line, sizeof(line), "[xerenge] terminate: %s: %s\n",
                      type ? type->name() : "no exception", what);
        Say(line);
        int frames = 0;
        _Unwind_Backtrace(SayFrame, &frames);
        std::abort();
    }
}

namespace ps5::startup
{
    void Prepare()
    {
        for (const char* path : { "/data/xerenge", "/data/xerenge/logs", "/data/xerenge/user" })
            mkdir(path, 0777);
        s_crashLog = open("/data/xerenge/logs/crash.log", O_WRONLY | O_CREAT | O_TRUNC, 0666);
        std::set_terminate(Terminate);
        if (FILE* file = std::fopen("/data/xerenge/logs/stderr.log", "w"))
        {
            setvbuf(file, nullptr, _IONBF, 0);
            stderr = file;
        }
        // A title starts without a usable working directory: stat of a relative
        // path says EINVAL.
        if (chdir("/data/xerenge") != 0)
            std::fprintf(stderr, "[ps5] chdir(/data/xerenge) failed: %s\n", std::strerror(errno));
        struct stat relative;
        const int relativeResult = stat("logs", &relative);
        std::fprintf(stderr, "[ps5] stat of a relative path: %d (%s)\n", relativeResult,
                     relativeResult ? std::strerror(errno) : "ok");
        FindCode();

        // The offscreen driver with the VideoOut WSI; a hint, not setenv
        // (ps5-knowledge/native-ports.md, NFSU1).
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "offscreen");
        SDL_SetHint(SDL_HINT_AUDIO_DRIVER, "ps5");

        // run.sh's plume switches, and the shaders are translated ahead of time.
        for (const char* name : { "XERENGE_D3D_TARGETS", "XERENGE_D3D_UI", "XERENGE_SKIP_LOGOS",
                                  "XERENGE_REAL_SHADERS", "XERENGE_D3D_DRAWS",
                                  "XERENGE_SECONDARY_TICKS" })
            setenv(name, "1", 0);
        // The console's pad is a DualSense, not an Xbox 360 controller: the
        // menus' and the race's button icons are the PlayStation set
        // (plume_prompts.cpp). A player who chooses XBOX in the settings page
        // writes prompts = xbox, and that wins when it is applied.
        setenv("XERENGE_PROMPTS", "playstation", 0);
    }
}
