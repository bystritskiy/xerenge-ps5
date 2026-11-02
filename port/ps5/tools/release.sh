#!/usr/bin/env bash
# Xerenge PS5: a release's downloads, from the app folder link.sh stages:
#   Xerenge-PS5-<contentVersion>.zip   the PPSA92006 folder at the top, the
#                                      artifact the homebrew stores install
#   Xerenge-<version>-ps5-third-party-source.tar.xz   (third-party-source.sh)
# and their SHA-256. The tag is the contentVersion of sce_sys/param.json; raise
# it, commit and link before each release.
#
#   port/ps5/tools/release.sh   -> build/ps5/release/
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
TITLE_ID="${TITLE_ID:-PPSA92006}"
APP="$ROOT/build/ps5/app/$TITLE_ID"
OUT="$ROOT/build/ps5/release"
[ -f "$APP/eboot.bin" ] || { echo "no $APP/eboot.bin: run port/ps5/tools/link.sh" >&2; exit 1; }
cmp -s "$APP/sce_sys/param.json" "$ROOT/port/ps5/sce_sys/param.json" ||
    { echo "the app folder's param.json is not port/ps5/sce_sys's: link again" >&2; exit 1; }
built=$(sed -n '1s/.*(\([0-9a-f]*\)).*/\1/p' "$APP/VERSION")
[ "$built" = "$(git -C "$ROOT" rev-parse --short HEAD)" ] ||
    { echo "the app was linked at $built, not at HEAD: link again" >&2; exit 1; }
"$ROOT/port/ps5/tools/setup-checkouts.sh" check
[ -z "$(git -C "$ROOT" status --porcelain -- port rexglue/src rexglue/mods rexglue/CMakeLists.txt)" ] ||
    echo "warning: uncommitted changes; the download would not match its commit" >&2
VERSION=$(python3 -I -c 'import json,sys; print(json.load(open(sys.argv[1]))["contentVersion"])' \
    "$APP/sce_sys/param.json")
mkdir -p "$OUT"
ZIP="$OUT/Xerenge-PS5-$VERSION.zip"
rm -f "$ZIP"
(cd "$APP/.." && zip -r -X -q "$ZIP" "$TITLE_ID" -x '*.DS_Store')
SOURCE=$("$ROOT/port/ps5/tools/third-party-source.sh")
echo "tag $VERSION"
shasum -a 256 "$ZIP" "$SOURCE"
