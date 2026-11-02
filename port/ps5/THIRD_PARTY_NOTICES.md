# Third-party notices: Xerenge for PS5

Xerenge is free software under the GNU General Public License, version 3 or (at
your option) any later version ([`LICENSE`](../../LICENSE)). The PS5 app,
`PPSA92006`, is one statically linked `eboot.bin` beside `sce_module/libc.prx`.
It is built from, and holds, software by others under their own licenses, listed
here. Their texts are in [`LICENSES/`](LICENSES); the app folder carries them in
`licenses/`, with this file, and its `VERSION` names the commit it was built from.

Burnout Revenge is not part of it: the game is Electronic Arts' and Criterion's,
and the app plays the files of your own disc. The game code in `eboot.bin` is
recompiled on the builder's machine from that disc's `default.xex`.

## Compiled into eboot.bin

| Project | What it does here | License | Text |
|---|---|---|---|
| **ReXGlue SDK** (Xerenge's fork [shipa-2/rexglue-xerenge](https://github.com/shipa-2/rexglue-xerenge), with [`patches/rexglue-sdk.patch`](patches/rexglue-sdk.patch)): Copyright (c) 2026 Tom Clay; portions from **Xenia**, Copyright (c) 2022 Ben Vanik and Xenia contributors | The recompiler's runtime: the Xbox 360 kernel, memory, threads, audio and the Xenos GPU front end. | BSD-3-Clause | [`ReXGlue-BSD-3-Clause.txt`](LICENSES/ReXGlue-BSD-3-Clause.txt) |
| **plume** (Xerenge's fork [shipa-2/plume-xerenge](https://github.com/shipa-2/plume-xerenge), with [`patches/plume.patch`](patches/plume.patch)): Copyright (c) 2024 renderbag and contributors | The Vulkan layer the renderer draws through. | MIT | [`plume-MIT.txt`](LICENSES/plume-MIT.txt) |
| **XenosRecomp** ([XenosRecomp-xerenge](https://github.com/shipa-2/XenosRecomp-xerenge)): Copyright (c) 2025 hedge-dev and contributors | Translates the game's shaders ahead of time; its shader cache format. | MIT | [`XenosRecomp-MIT.md`](LICENSES/XenosRecomp-MIT.md) |
| **smol-v**: Copyright (c) 2016-2018 Aras Pranckevicius | The shader cache's SPIR-V compression (through XenosRecomp). | MIT (or public domain) | [`smol-v-MIT.txt`](LICENSES/smol-v-MIT.txt) |
| **Zstandard**: Copyright (c) Meta Platforms, Inc. and affiliates | The shader cache's compression (through XenosRecomp). | BSD-3-Clause (one of the two it offers) | [`zstd-BSD-3-Clause.txt`](LICENSES/zstd-BSD-3-Clause.txt) |
| **FFmpeg**: the FFmpeg developers ([wmarti/FFmpeg](https://github.com/wmarti/FFmpeg) at the SDK's pinned commit) | The XMA audio decoder, linked statically. | LGPL-2.1-or-later | [`FFmpeg-LGPL-2.1.txt`](LICENSES/FFmpeg-LGPL-2.1.txt) |
| **libmspack**: (C) 2003-2023 Stuart Caie | LZX decompression. | LGPL-2.1 | [`libmspack-LGPL-2.1.txt`](LICENSES/libmspack-LGPL-2.1.txt) |
| **aes_128**: Copyright (c) 2017 LuoPeng | The kernel's AES routines. | MIT | [`aes_128-MIT.txt`](LICENSES/aes_128-MIT.txt) |
| **{fmt}**: Copyright (c) 2012-present Victor Zverovich | Text formatting. | MIT | [`fmt-MIT.txt`](LICENSES/fmt-MIT.txt) |
| **spdlog**: Copyright (c) 2016 Gabi Melman | The log. | MIT | [`spdlog-MIT.txt`](LICENSES/spdlog-MIT.txt) |
| **CLI11**: Copyright (c) 2017-2025 University of Cincinnati | Settings given as arguments. | BSD-3-Clause | [`CLI11-BSD-3-Clause.txt`](LICENSES/CLI11-BSD-3-Clause.txt) |
| **toml++**: Copyright (c) Mark Gillard | Reads `xerenge.conf` and the SDK's settings. | MIT | [`tomlplusplus-MIT.txt`](LICENSES/tomlplusplus-MIT.txt) |
| **UTF8-CPP**: Copyright 2006 Nemanja Trifunovic | Text encodings. | BSL-1.0 | [`utfcpp-BSL-1.0.txt`](LICENSES/utfcpp-BSL-1.0.txt) |
| **xxHash**: Copyright (c) 2012-2021 Yann Collet | Hashes of textures and shaders. | BSD-2-Clause | [`xxHash-BSD-2-Clause.txt`](LICENSES/xxHash-BSD-2-Clause.txt) |
| **o1heap**: Copyright (c) 2020 Pavel Kirienko | A heap allocator. | MIT | [`o1heap-MIT.txt`](LICENSES/o1heap-MIT.txt) |
| **SIMDe**: Copyright (c) 2017 Evan Nemerson and contributors | Vector instructions in the recompiled code. | MIT | [`SIMDe-MIT.txt`](LICENSES/SIMDe-MIT.txt) |
| **disruptorplus**: Copyright (c) 2013-2015 Lewis Baker | The timer queue. | MIT | [`disruptorplus-MIT.txt`](LICENSES/disruptorplus-MIT.txt) |
| **Dear ImGui**: Copyright (c) 2014-2025 Omar Cornut | Overlays. | MIT | [`DearImGui-MIT.txt`](LICENSES/DearImGui-MIT.txt) |
| **stb_image, stb_image_write, stb_truetype**: Copyright (c) 2017 Sean Barrett | Pictures and fonts (the button icons, a translation's own font). | MIT (or public domain) | [`stb-MIT.txt`](LICENSES/stb-MIT.txt) |
| **Vulkan Memory Allocator**: Copyright (c) 2017-2025 Advanced Micro Devices, Inc. | GPU memory. | MIT | [`VulkanMemoryAllocator-MIT.txt`](LICENSES/VulkanMemoryAllocator-MIT.txt) |
| **Vulkan-Headers** and **SPIRV-Headers**: The Khronos Group Inc. | Headers the renderer is built with. | Apache-2.0 or MIT; MIT | [`Vulkan-Headers.md`](LICENSES/Vulkan-Headers.md); [`SPIRV-Headers.txt`](LICENSES/SPIRV-Headers.txt) |
| **SMAA**: Copyright (C) 2013 Jorge Jimenez, Jose I. Echevarria, Belen Masia, Fernando Navarro, Diego Gutierrez | Anti-aliasing (`rexglue/thirdparty/smaa`). | MIT | [`SMAA-MIT.txt`](LICENSES/SMAA-MIT.txt) |
| **etcpak**: Copyright (c) 2013-2026 Bartosz Taudul | Texture compression (`rexglue/thirdparty/etcpak`). | BSD-3-Clause | [`etcpak-BSD-3-Clause.txt`](LICENSES/etcpak-BSD-3-Clause.txt) |
| **Takedawn** button style: Copyright (c) 2026 sp00nz, xenophobentx | The PlayStation button icons are drawn in its style (`rexglue/assets/input_prompts`). | MIT | [`takedawn-MIT.txt`](LICENSES/takedawn-MIT.txt) |
| **Input Prompts** by Kenney (www.kenney.nl) | Button and key icons. | CC0-1.0 | [`Kenney-InputPrompts-CC0.txt`](LICENSES/Kenney-InputPrompts-CC0.txt) |

## The PS5 parts

| Project | What it does here | License | Text |
|---|---|---|---|
| **PS5_Vulkan**: Copyright (C) 2026 BlackBearReloaded, Mihawk-99 ([mihawk-99/PS5_Vulkan](https://github.com/mihawk-99/PS5_Vulkan)) | The app's start-up code, the generated `sce_module/libc.prx` (no code of a console module), the RADV link recipe and `ps5-native-tool`, which makes `eboot.bin` (not shipped). | GPL-3.0-or-later | [`GPL-3.0.txt`](LICENSES/GPL-3.0.txt); [`PS5_Vulkan-NOTICE.md`](LICENSES/PS5_Vulkan-NOTICE.md) |
| **PS5 payload SDK**: ps5-payload-dev and contributors, with Mihawk-99's PS5 platform layer ([mihawk-99/PS5_PayloadSDK](https://github.com/mihawk-99/PS5_PayloadSDK)) | Its C library, thread library, module stubs and platform layer. | GPL-3.0-or-later; its `include/freebsd` headers: BSD | [`GPL-3.0.txt`](LICENSES/GPL-3.0.txt) |
| **Mesa** 26.2.0 (RADV, ACO, NIR and Mesa's Vulkan runtime), with Mihawk-99's PS5 winsys ([mihawk-99/PS5_Mesa](https://github.com/mihawk-99/PS5_Mesa)) | The Vulkan driver, linked in whole. | MIT (individual files may name others) | [`Mesa/`](LICENSES/Mesa) |
| **SDL** 3.4.18: Copyright (C) 1997-2026 Sam Lantinga | The window, audio and the DualSense, with this port's VideoOut and AudioOut drivers. | Zlib | [`SDL3-Zlib.txt`](LICENSES/SDL3-Zlib.txt) |
| **LLVM** 18: libc++, libc++abi, libunwind and compiler-rt's builtins | The C++ runtime and the compiler's helper functions. | Apache-2.0 WITH LLVM-exception | [`LLVM-Apache-2.0-WITH-LLVM-exception.txt`](LICENSES/LLVM-Apache-2.0-WITH-LLVM-exception.txt) |

[README.md](README.md#building-macos-host) gives the revisions of the PS5
pieces. No Sony SDK file is used. PlayStation, PS5, DualSense and the
PlayStation Shapes are trademarks of Sony Interactive Entertainment Inc.; the
button icons are drawings of this project's, not Sony's artwork.

## Source code

Xerenge's own source, with this port, is the repository at the commit the
app's `VERSION` names; the ReXGlue SDK, plume and XenosRecomp are Xerenge's
forks at the commits [`tools/setup-checkouts.sh`](tools/setup-checkouts.sh)
pins, the SDK and plume with the PS5 changes in [`patches/`](patches). The complete source of the LGPL
and GPL parts above (FFmpeg and libmspack at the SDK's pinned commits,
PS5_Vulkan, PS5_PayloadSDK and PS5_Mesa at the revisions the build pins) is published next to each release's download as
`Xerenge-<version>-ps5-third-party-source.tar.xz`, which
[`tools/third-party-source.sh`](tools/third-party-source.sh) makes. If it is ever
missing, ask through the repository's issue tracker: for at least three years
after a release, a copy is provided at no charge.
