#!/usr/bin/env bash
# Xerenge PS5: the complete source of the LGPL and GPL parts in the PS5 app, at
# the versions it is built with, for a release's downloads
# (THIRD_PARTY_NOTICES.md, "Source code"):
#   - FFmpeg and libmspack at the SDK's pinned submodule commits, with the
#     SDK's FFmpeg build configuration (thirdparty/ffmpeg-overlay)
#   - PS5_Vulkan, PS5_PayloadSDK and PS5_Mesa at the revisions
#     setup-ps5-vulkan.sh pins (Mesa is MIT, here for completeness)
#
#   port/ps5/tools/third-party-source.sh  -> build/ps5/release/Xerenge-<version>-ps5-third-party-source.tar.xz
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
DEPS="$ROOT/build/ps5-deps"
SDK="$ROOT/rexglue-sdk"
VERSION="$(cat "$ROOT/VERSION")"
NAME="Xerenge-$VERSION-ps5-third-party-source"
OUT="$ROOT/build/ps5/release"
STAGE="$OUT/$NAME"
rev() { sed -n "s/^$1=\([0-9a-f]*\).*/\1/p" "$ROOT/port/ps5/tools/setup-ps5-vulkan.sh"; }
rm -rf "$STAGE"
mkdir -p "$STAGE/FFmpeg" "$STAGE/libmspack" "$STAGE/ffmpeg-overlay"
git -C "$SDK/thirdparty/FFmpeg" archive HEAD | tar -x -C "$STAGE/FFmpeg"
git -C "$SDK/thirdparty/FFmpeg" rev-parse HEAD > "$STAGE/FFmpeg/REVISION"
git -C "$SDK/thirdparty/libmspack" archive HEAD | tar -x -C "$STAGE/libmspack"
git -C "$SDK/thirdparty/libmspack" rev-parse HEAD > "$STAGE/libmspack/REVISION"
# As checked out, with the PS5 patch (staged, not committed, after a setup).
mkdir -p "$STAGE/ffmpeg-overlay/thirdparty"
cp -R "$SDK/thirdparty/ffmpeg-overlay" "$SDK/thirdparty/CMakeLists.txt" "$STAGE/ffmpeg-overlay/thirdparty/"
for pair in PS5_Vulkan:PS5_VULKAN_REV PS5_PayloadSDK:PS5_SDK_REV PS5_Mesa:PS5_MESA_REV; do
    repo=${pair%%:*} revision=$(rev "${pair#*:}")
    [ -n "$revision" ] || { echo "no ${pair#*:} in setup-ps5-vulkan.sh" >&2; exit 1; }
    mkdir -p "$STAGE/$repo"
    git -C "$DEPS/$repo" archive "$revision" | tar -x -C "$STAGE/$repo"
    echo "$revision" > "$STAGE/$repo/REVISION"
done
cat > "$STAGE/README.txt" <<EOF
Source code of the LGPL and GPL parts of Xerenge $VERSION for PS5.

FFmpeg/          github.com/wmarti/FFmpeg at the ReXGlue SDK's pinned commit
                 (LGPL-2.1-or-later)
ffmpeg-overlay/  the SDK's build of it: thirdparty/CMakeLists.txt and the
                 configuration in thirdparty/ffmpeg-overlay
libmspack/       libmspack at the SDK's pinned commit (LGPL-2.1)
PS5_Vulkan/      github.com/mihawk-99/PS5_Vulkan: the app CRT, link recipe,
                 ps5-native-tool and the generated libc.prx (GPL-3.0-or-later)
PS5_PayloadSDK/  github.com/mihawk-99/PS5_PayloadSDK: the payload SDK with the
                 PS5 platform layer (GPL-3.0-or-later; include/freebsd: BSD)
PS5_Mesa/        github.com/mihawk-99/PS5_Mesa: Mesa with RADV's PS5 winsys
                 (MIT and others, its licenses/ folder)

Each REVISION names the commit. Xerenge's own source, with the ReXGlue SDK's
and plume's PS5 patches, is the repository at the commit the app's VERSION
file names.
EOF
tar -C "$OUT" -cJf "$OUT/$NAME.tar.xz" "$NAME"
rm -rf "$STAGE"
echo "$OUT/$NAME.tar.xz"
