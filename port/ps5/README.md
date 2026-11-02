# Xerenge on PS5

<b>Burnout Revenge (Xbox 360), recompiled to run natively on a jailbroken PS5.
Not an emulator. Bring your own disc.</b>

> **No game files are included or distributed.** To play, you need your own
> legally obtained copy of *Burnout Revenge* for Xbox 360 (the USA release) and
> the files of that disc. Don't ask for, or share, game files or disc images.
> See the [legal notice](#legal-notice).

A native PS5 host for [Xerenge](https://github.com/shipa-2/Xerenge), the static
recompilation of the Xbox 360 version of *Burnout Revenge* by Shipa Snake and
contributors. The recompiled game code (from your own `default.xex`), ReXGlue
and the plume renderer are the same as on the other platforms; this folder adds
the console's toolchain, libraries and glue, and the PS5 parts of the SDK and
the project are under `REX_PLATFORM_PS5` / `XERENGE_PS5`.

Title ID **PPSA92006**, tile name **Xerenge**. The app contains no game data: it
plays the files of your own disc from the console's storage.

## What you need

- A PS5 on a homebrew-enabled firmware (tested on 13.40) with
  ShadowMountPlus, which mounts
  folder titles from `/data/homebrew`, and an FTP server (ftpsrv, port 2121) to
  copy files.
- The files of your own **USA** disc of *Burnout Revenge* (Xbox 360): image
  SHA-256 `7abf689f52b70666ba92052bd157cfcae9ed6290dae298fbf639baa1d169f92b`.
  The game code is compiled into the app from that release's `default.xex`, so
  the European disc and other dumps don't work with it.
- About 4 GB free on the console for the disc's files.

## Installing

1. **The app.** Install it from a homebrew store (RecompStore, Prospero Store),
   or download `Xerenge-PS5-<version>.zip` from the
   [Releases](https://github.com/bystritskiy/xerenge-ps5/releases) page and copy its `PPSA92006` folder to
   `/data/homebrew/` on the console, so that `eboot.bin` is at
   `/data/homebrew/PPSA92006/eboot.bin`. ShadowMountPlus shows the tile.
2. **Your disc.** Extract the files of your disc image (the Xbox 360's XDVDFS:
   `scripts/extract-image` in this repository, or `extract-xiso -x`) and copy
   all of them to `/data/xerenge/game/`, so that `default.xex` is at
   `/data/xerenge/game/default.xex` beside `data/`, `fe/`, `language/` and the
   rest. With this repository on a computer, both steps are one command each:

   ```sh
   ./scripts/extract-image --sha256 7abf689f52b70666ba92052bd157cfcae9ed6290dae298fbf639baa1d169f92b \
     <image.iso> game
   python3 -I port/ps5/tools/upload-game.py --addr <console> game
   ```

3. **Start Xerenge** from its tile.

Saves are in `/data/xerenge/user/`, settings in `/data/xerenge/xerenge.conf`,
logs in `/data/xerenge/logs/`. **To update,** copy the new `PPSA92006` folder over
the old one: saves, settings and game files are kept.

### Mods

Mods go in `/data/xerenge/game/mods/`, as on the other platforms: a translation
(this repository has [`rexglue/mods/locale_rus`](../../rexglue/mods/locale_rus))
is chosen in **Driver Details > Settings > LANGUAGE**.
[MODDING.md](../../MODDING.md) has how they work.

## Playing

The DualSense plays as the Xbox 360 pad, with PlayStation button icons in the
game's prompts (Settings > DEBUG > BUTTON ICONS switches back to the game's
own); OPTIONS is the Xbox's START. The game's menus, races, Crash mode and
replays are those of the PC and Mac builds; their settings are under **Driver
Details > Settings** (GRAPHICS, HACKS, DEBUG).

Report a problem with the log from `/data/xerenge/logs/`
([CONTRIBUTING.md](../../CONTRIBUTING.md) says what a report needs).

## Building (macOS host)

Needs Homebrew's `llvm@18`, `lld`, `cmake` and `ninja`. `build/ps5-deps/` holds
the pinned PS5 toolchain: PS5_Vulkan `f410e67a`,
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

`rexglue-sdk`, `plume` and `XenosRecomp` are Xerenge's forks, separate
checkouts the root repository ignores. The port uses them at fixed commits, the
SDK and plume with the PS5 changes in [`patches/`](patches). Fetch them, and the
console's toolchain, before the steps above:

```sh
port/ps5/tools/setup-checkouts.sh    # rexglue-sdk, plume, XenosRecomp, patched
port/ps5/tools/setup-ps5-vulkan.sh   # PS5_Vulkan, PS5_Mesa, PS5_PayloadSDK, compiler-rt
```

After changing the SDK or plume, `setup-checkouts.sh export` writes the patches
again.
SDL is 3.4.18 patched by `tools/prepare-sdl.py`.
Don't run `scripts/setup-sdk.sh` or `setup-deps.sh` over the checkouts, which
reset them to the forks' latest `main`.

## Making a release

Raise `contentVersion` in [`sce_sys/param.json`](sce_sys/param.json), commit,
build and link, then:

```sh
port/ps5/tools/release.sh
```

It writes `build/ps5/release/Xerenge-PS5-<contentVersion>.zip` (the `PPSA92006`
folder at the top, what the homebrew stores install) and the source of the GPL
and LGPL parts ([THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)), and prints
their SHA-256. Both go on a GitHub release tagged with the `contentVersion`.

## Installing a build

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
  from `/app0/eboot.bin` (`src/code_bytes_ps5.cpp`).
- **Threads.** As on macOS: SIGUSR1 and SIGUSR2 for suspend and user callbacks,
  affinity a no-op; guest priorities are not applied.
- **Vulkan.** plume's volk over RADV's `vk_icdGetInstanceProcAddr`; there is no
  loader. Shaders are translated ahead of time, so no glslang at run time.
- **Display, audio, input.** SDL: the offscreen driver with a
  VideoOut WSI, AudioOut, and the DualSense as an SDL virtual gamepad.
- **Folders.** `/app0` is read-only, so `GetExecutablePath` names
  `/data/xerenge/burnout`: the SDK's config and logs live beside it.

## Legal notice

Xerenge is an unofficial, fan-made project by Shipa Snake and contributors; this
PS5 port is not made by them. Neither is **affiliated with, endorsed by or
sponsored by Electronic Arts Inc., Criterion Games or Sony Interactive
Entertainment.**

"Burnout" and "Burnout Revenge" are trademarks of Electronic Arts Inc.
PlayStation, PS5, DualSense and the PlayStation Shapes are trademarks of Sony
Interactive Entertainment Inc.; the button icons in this port are drawings of
the project's own (in Takedawn's style) and Kenney's, not Sony's artwork. It uses
no Sony SDK: only public, community-made tools. Xbox and Xbox 360 are trademarks
of Microsoft Corporation. All of them are used here only to say what Xerenge
works with. No logos, box art or key art of the game are used; game content
visible in screenshots belongs to its owners.

**No game files are included or distributed.** The app contains no game data or
assets: no textures, models, audio, movies, game text or disc files. To play,
you need your own legally obtained copy of *Burnout Revenge* for Xbox 360.

## Credits and license

- [Xerenge](https://github.com/shipa-2/Xerenge): Shipa Snake and contributors.
- [ReXGlue SDK](https://github.com/rexglue/rexglue-sdk): Tom Clay; with code
  from [Xenia](https://xenia.jp).
- plume (renderbag and contributors) and XenosRecomp (hedge-dev and
  contributors), through Xerenge's forks
  [plume-xerenge](https://github.com/shipa-2/plume-xerenge) and
  [XenosRecomp-xerenge](https://github.com/shipa-2/XenosRecomp-xerenge).
- [PS5_Vulkan, PS5_Mesa and PS5_PayloadSDK](https://github.com/mihawk-99):
  BlackBearReloaded and Mihawk-99, over the
  [PS5 payload SDK](https://github.com/ps5-payload-dev/sdk).

Xerenge, with this port, is under the GNU General Public License, version 3 or
later ([LICENSE](../../LICENSE)). Everything else in the app keeps its own license:
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
