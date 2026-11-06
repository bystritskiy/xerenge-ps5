<h1 align="center">Xerenge for PS5</h1>

<p align="center">
  <b>Burnout Revenge, the Xbox 360 version, statically recompiled<br>
  into a native app for homebrew-enabled PS5 consoles.</b>
</p>

<p align="center">
  <a href="https://prosperostore.net/app/PPSA92006/"><img src="https://prosperostore.net/press/badge-dark.png" alt="Get it on ProsperoStore" height="56"></a>
  <a href="https://recompstore.com/game/PPSA92006"><img src="https://recompstore.com/assets/press/badge-dark.svg" alt="Get it on RecompStore" height="56"></a>
</p>

<p align="center">
  <a href="#install"><b>Install</b></a>
  &nbsp;·&nbsp;
  <a href="#controls">Controls</a>
  &nbsp;·&nbsp;
  <a href="#settings">Settings</a>
  &nbsp;·&nbsp;
  <a href="#reporting-a-bug">Report a bug</a>
</p>

> [!IMPORTANT]
> **Bring your own game.**
> This project does not include or distribute copyrighted game files. Use a dump of your own original disc.
> Please do not request or share game downloads here.

> [!CAUTION]
> **We do not support piracy.**
> Please do not use this project with pirated copies of the game.

## What it is

A PS5 port of [Xerenge](https://github.com/shipa-2/Xerenge) by Shipa Snake and contributors.
The game's PowerPC code is translated into C++ ahead of time, with ReXGlue providing the
runtime and plume handling graphics through Vulkan. Other platforms are maintained upstream.

## Highlights

- **Native PS5 app** with Vulkan rendering through RADV.
- **DualSense controls** and PlayStation button prompts.
- **Community online play**, including players on other platforms.
- **In-game settings** for graphics, gameplay adjustments and debugging.
- **Mod support**, including translations and replacement music.

## Requirements

- A homebrew-enabled PS5 with ShadowMountPlus and an FTP server.
  The documented test firmware is 13.40.
- A dump of your own **USA Xbox 360 disc** of *Burnout Revenge*, with image SHA-256
  `7abf689f52b70666ba92052bd157cfcae9ed6290dae298fbf639baa1d169f92b`.
  The PS5 build requires this release; upstream's European-disc support does not apply here.
- About **4 GB free** for the extracted disc files.

## Install

Install the app from [RecompStore](https://recompstore.com/game/PPSA92006) or
[ProsperoStore](https://prosperostore.net/app/PPSA92006/), or use the manual steps below.
Both methods require your own disc files.

1. **The app.** Download `Xerenge-PS5-<version>.zip` from [Releases](../../releases/latest).
   Extract its `PPSA92006` folder to `/data/homebrew/` on the console.
2. **Your disc.** Extract the disc image with `scripts/extract-image` or `extract-xiso -x`.
   Copy all files to `/data/xerenge/game/`, with `default.xex` beside `data/`, `fe/`,
   `language/` and the remaining disc folders.
3. **Play.** Start the **Xerenge** tile.

**To update,** replace the `PPSA92006` app folder. Saves, settings and game files are kept separately.
See the [PS5 guide](port/ps5/README.md#installing) for extraction and upload commands.

## Controls

The DualSense follows the Xbox 360 controller layout by button position.

| Xbox 360 | DualSense |
|---|---|
| A / B / X / Y | Cross / Circle / Square / Triangle |
| LB / RB | L1 / R1 |
| LT / RT | L2 / R2 |
| Start | Options |

PlayStation button icons are enabled in the game. **Settings → DEBUG → BUTTON ICONS**
switches back to the original icons.

## Settings

Open **Driver Details → Settings** for GRAPHICS, HACKS, DEBUG and LANGUAGE.
Settings are stored in `/data/xerenge/xerenge.conf`.

**Online:** choose ONLINE from the main menu to use the community lobby at `94639.snk.wtf`.
The displayed player name defaults to the console user name. The server and player name can
be changed in the configuration file; see the [online settings](port/ps5/README.md#playing).

**Mods:** put them in `/data/xerenge/game/mods/`. Select a translation under
**Driver Details → Settings → LANGUAGE**. See [MODDING.md](MODDING.md).

## Your files

| Files | Location |
|---|---|
| Extracted disc | `/data/xerenge/game/` |
| Saves | `/data/xerenge/user/` |
| Settings | `/data/xerenge/xerenge.conf` |
| Logs | `/data/xerenge/logs/` |

## Known issues

The plume renderer can occasionally flash a black frame or briefly display an incorrect texture.
See the [PS5 guide](port/ps5/README.md) for platform-specific details.
Upstream platform and performance claims do not establish the same behavior on PS5.

## Reporting a bug

Open an [issue](../../issues) with the app version, firmware, reproduction steps and logs from
`/data/xerenge/logs/`. Describe the expected and actual result. Do not attach game files or disc images.
See [CONTRIBUTING.md](CONTRIBUTING.md) for report details.

## For developers

See the [PS5 build and packaging guide](port/ps5/README.md#building-macos-host).
[SETUP.md](SETUP.md) and the [runtime overview](rexglue/README.md) cover the upstream build.

## Credits

- [Xerenge](https://github.com/shipa-2/Xerenge): Shipa Snake and contributors.
- [ReXGlue SDK](https://github.com/rexglue/rexglue-sdk), plume and XenosRecomp,
  through Xerenge's forks.
- [PS5_Vulkan, PS5_Mesa and PS5_PayloadSDK](https://github.com/mihawk-99):
  BlackBearReloaded, Mihawk-99 and the community projects they build on.
- PS5 port: [bystritskiy](https://github.com/bystritskiy).

## License

Xerenge and this port are GPL-3.0-or-later ([LICENSE](LICENSE)). Third-party components retain
their own terms; see [THIRD_PARTY_NOTICES.md](port/ps5/THIRD_PARTY_NOTICES.md).
The license grants no rights to *Burnout Revenge* or its game data.

## Legal notice

This is an unofficial fan port, independent of the upstream authors, Electronic Arts,
Criterion Games, Microsoft and Sony Interactive Entertainment. Game and platform names
are trademarks of their respective owners. See the [full notice](port/ps5/README.md#legal-notice).
