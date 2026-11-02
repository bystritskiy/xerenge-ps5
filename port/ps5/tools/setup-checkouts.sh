#!/usr/bin/env bash
# Xerenge PS5: the ReXGlue SDK, plume and XenosRecomp as this port builds them -
# Xerenge's forks at the commits below, with the PS5 changes in port/ps5/patches
# applied (staged, not committed).
#
#   port/ps5/tools/setup-checkouts.sh          clone or update rexglue-sdk/,
#                                              plume/, XenosRecomp/ and patch
#   port/ps5/tools/setup-checkouts.sh export   write the patches from the
#                                              checkouts (a changed SDK or plume)
#   port/ps5/tools/setup-checkouts.sh check    fail unless the checkouts are
#                                              what a setup makes (release.sh)
#
# A checkout with changes of its own is left alone. After merging a newer
# upstream into a checkout, set its base here and export.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
PATCHES="$ROOT/port/ps5/patches"
SDK_REMOTE=https://github.com/shipa-2/rexglue-xerenge.git
SDK_BASE=9a10e470e544db3e2799a1fa71ef67f425b598ce
PLUME_REMOTE=https://github.com/shipa-2/plume-xerenge.git
PLUME_BASE=919b921590fa8bed44da50fa9ace2e132de1aa00
XENOS_REMOTE=https://github.com/shipa-2/XenosRecomp-xerenge.git
XENOS_BASE=a65809bc34cae58bc562c6d6e55198b3bf22ea99

# The checkout as base + patch: what link.sh compares with the patch.
patched_diff() {
    git -C "$1" diff --no-color --no-ext-diff --binary --ignore-submodules=dirty "$2"
}

setup() {
    local dir="$ROOT/$1" remote=$2 base=$3 patch=${4:-}
    if [ ! -d "$dir/.git" ]; then
        git clone "$remote" "$dir"
    else
        git -C "$dir" fetch -q origin "$base"
        if [ "$(patched_diff "$dir" "$base")" = "$(cat "${patch:+$PATCHES/$patch}" 2>/dev/null)" ]; then
            echo "$1: already $base${patch:+ with $patch}"
            return
        fi
        if [ -n "$(git -C "$dir" status --porcelain --untracked-files=no --ignore-submodules=dirty)" ]; then
            echo "$1 has changes of its own; left alone" >&2
            exit 1
        fi
    fi
    git -C "$dir" checkout -q --detach "$base"
    git -C "$dir" submodule update --init --recursive
    [ -z "$patch" ] || git -C "$dir" apply --index "$PATCHES/$patch"
    echo "$1: $base${patch:+ with $patch}"
}

if [ "${1:-}" = check ]; then
    [ "$(patched_diff "$ROOT/rexglue-sdk" "$SDK_BASE")" = "$(cat "$PATCHES/rexglue-sdk.patch")" ] &&
    [ "$(patched_diff "$ROOT/plume" "$PLUME_BASE")" = "$(cat "$PATCHES/plume.patch")" ] &&
    [ -z "$(patched_diff "$ROOT/XenosRecomp" "$XENOS_BASE")" ] ||
        { echo "rexglue-sdk, plume or XenosRecomp is not its base with port/ps5/patches: export, or set up again" >&2; exit 1; }
    exit
fi
if [ "${1:-}" = export ]; then
    patched_diff "$ROOT/rexglue-sdk" "$SDK_BASE" > "$PATCHES/rexglue-sdk.patch"
    patched_diff "$ROOT/plume" "$PLUME_BASE" > "$PATCHES/plume.patch"
    wc -l "$PATCHES"/*.patch
    exit
fi
setup rexglue-sdk "$SDK_REMOTE" "$SDK_BASE" rexglue-sdk.patch
setup plume "$PLUME_REMOTE" "$PLUME_BASE" plume.patch
setup XenosRecomp "$XENOS_REMOTE" "$XENOS_BASE"
