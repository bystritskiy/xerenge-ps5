// Xerenge PS5: the host's own pieces the game and the SDK call (port/ps5/src).
// GPL-3.0-or-later.
#pragma once

namespace ps5::pad
{
    // Attaches the DualSense as an SDL virtual gamepad (after SDL's gamepad
    // subsystem is up: SDLInputDriver::Setup).
    void Attach();
    // Stops the motors and closes the pad.
    void Shutdown();
}

namespace ps5::startup
{
    // Before SDL starts: its video and audio drivers, the switches run.sh sets
    // elsewhere, and the folders. Called first thing in main.
    void Prepare();
    // Where the executable's code is in /app0/eboot.bin (code_bytes_ps5.cpp).
    void FindCode();
    // A line to the kernel log and logs/crash.log.
    void Say(const char* text);
}
