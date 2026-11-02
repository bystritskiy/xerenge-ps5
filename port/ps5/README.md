# Xerenge on PS5

A native PS5 host for Xerenge, the static recompilation of the Xbox 360 version
of *Burnout Revenge*. The recompiled game code (from your own `default.xex`),
ReXGlue and the plume renderer are the same as on the other platforms; this
folder adds the console's toolchain, libraries and glue, and the PS5 parts of
the SDK and the project are under `REX_PLATFORM_PS5` / `XERENGE_PS5`.

Title ID **PPSA92006**, tile name **Xerenge**. The app contains no game data: it
plays the files of your own disc from the console's storage.

Checks of what has run on a console are in [VALIDATION.md](VALIDATION.md).

## Game files

The USA disc (image SHA-256
`7abf689f52b70666ba92052bd157cfcae9ed6290dae298fbf639baa1d169f92b`). Extract it
and copy the files to `/data/xerenge/game/`:

```sh
./scripts/extract-image --sha256 7abf689f52b70666ba92052bd157cfcae9ed6290dae298fbf639baa1d169f92b \
  <image.iso> game
python3 -I port/ps5/tools/upload-game.py --addr <console> game
```

Saves are in `/data/xerenge/user/`, logs in `/data/xerenge/logs/`.

## Building (macOS host)

Needs Homebrew's `llvm@18`, `lld`, `cmake` and `ninja`. `build/ps5-deps/` holds
the pinned PS5 toolchain, as in SpeedBreaker's port: PS5_Vulkan `f410e67a`,
PS5_Mesa `1ba9874f` (RADV), PS5_PayloadSDK `b83202be`, SDL 3.4.18 `829a65d7` with
the VideoOut WSI and the AudioOut driver, LLVM 18's compiler-rt. Copying it from
another project is fine once the revisions and the compiler-rt hash match.

```sh
scripts/build-tools.sh
cmake --preset mac-arm64 -S rexglue-sdk      # the code generator, for this Mac
cmake --build rexglue-sdk/out/build/mac-arm64 --config Release --target rexglue
scripts/build.sh --game game --generate-only # code and shaders from default.xex
export LLVM_CONFIG=$(port/ps5/tools/llvm-bin.sh build/ps5-deps/llvm18)
cmake -S rexglue -B build/ps5 -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE=$PWD/port/ps5/cmake/ps5.cmake \
  -DCMAKE_C_COMPILER=/opt/homebrew/opt/llvm@18/bin/clang \
  -DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm@18/bin/clang++ \
  -DPS5_PAYLOAD_SDK=$PWD/build/ps5-deps/PS5_Vulkan/.deps/native/ps5-payload-sdk \
  -DREXSDK_DIR=$PWD/rexglue-sdk \
  -DREXGLUE_CODEGEN=$PWD/rexglue-sdk/out/mac-arm64/Release/rexglue \
  -DXERENGE_PS5_SDL3=$PWD/build/ps5-deps/sdl3 \
  -DREXGLUE_ENABLE_TRACY=OFF -DREXGLUE_ENABLE_PERF_COUNTERS=OFF \
  -DCMAKE_CXX_SCAN_FOR_MODULES=OFF
ninja -C build/ps5 burnout xerenge-ps5 rexgpu-plume rexruntime
port/ps5/tools/link.sh
```

`rexglue-sdk`, `plume` and `XenosRecomp` are the upstream forks on a local `ps5`
branch, and the root repository ignores all three: their changes are committed
and pushed from each checkout. Don't run `scripts/setup-sdk.sh` or
`setup-deps.sh` over them, which reset them to upstream.

## Installing

The console needs a homebrew-enabled firmware with ShadowMountPlus and an FTP
server (ftpsrv, port 2121):

```sh
python3 -I port/ps5/tools/deploy-app.py --addr <console>
```

## How it is put together

- **One image.** Nothing of ours is loaded at run time: `rexruntime` and the plume
  plugin are static, `rex::system::LoadGpuPlugin` calls the linked-in plugin, and
  the xenos plugin and the code generator are left out. CMake compiles;
  `tools/link.sh` links (`link-objects.rsp` lists every object), so the hooks
  replace the recompiled functions' weak definitions.
- **Memory.** `Memory::MapViewsPS5`: the guest's 4.5 GB range is reserved and
  filled with views of two direct-memory objects, 2.25 GB for the virtual heaps
  and 512 MB for physical memory. Commit and decommit only change access
  (`rex::memory::AllocFixed`), through libkernel (`src/memory_ps5.cpp`). Host
  pages are 16 KB, the path Apple silicon already takes, so the `0xE0000000`
  window's 4 KB shift is `REX_PHYS_HOST_OFFSET` (`pch_h.inja`).
- **Faults.** FreeBSD's mcontext in `exception_handler_posix.cpp`, SIGBUS handled
  as SIGSEGV. eboot code is execute-only, so the MMIO handler reads the instruction
  from `/app0/eboot.bin` (`src/code_bytes_ps5.cpp`, after reSOT).
- **Threads.** As on macOS: SIGUSR1 and SIGUSR2 for suspend and user callbacks,
  affinity a no-op; guest priorities are not applied.
- **Vulkan.** plume's volk over RADV's `vk_icdGetInstanceProcAddr`; there is no
  loader. Shaders are translated ahead of time, so no glslang at run time.
- **Display, audio, input.** SpeedBreaker's SDL: the offscreen driver with a
  VideoOut WSI, AudioOut, and the DualSense as an SDL virtual gamepad.
- **Folders.** `/app0` is read-only, so `GetExecutablePath` names
  `/data/xerenge/burnout`: the SDK's config and logs live beside it.
