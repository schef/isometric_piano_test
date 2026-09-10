# Project handoff

## Purpose

This is a Raspberry Pi Pico SDK C firmware prototype for turning the three Hall-effect analog keys on a Fluxpad V2 into velocity-sensitive piano/MIDI keys.

## Current behavior

- `src/main.c` samples GPIO 28, 27, and 26, corresponding to Fluxpad analog keys 2, 3, and 4.
- It scans each key at 4 kHz and averages 8 ADC samples per scan.
- Two-threshold timing: timing starts when ADC falls below 1600 and stops at 600. If ADC rises back above 1600 before hitting 600, the stroke is cancelled.
- Velocity formula: 1,800,000 / travel_time_us, clamped to 1–127, with a 1.5 ms minimum.
- Note Off triggers when ADC rises above 800 (hysteresis above the low threshold).
- It enumerates as a composite TinyUSB CDC serial and USB-MIDI device.
- It sends Note On at the low threshold and Note Off at the release threshold: key 4 -> C4 (60), key 3 -> E4 (64), key 2 -> G4 (67).

## Build and test

```sh
export PICO_SDK_PATH=$HOME/git/pico-sdk
cmake -S . -B build
cmake --build build
```

Flash `build/fluxpad_midi.uf2` through the RP2040 `RPI-RP2` boot drive. Monitor `/dev/ttyACM0` at 115200 baud.

`bash deploy.sh` builds and flashes in one step. It resets the Fluxpad using a 1200-baud serial-port touch and identifies the bootloader storage device through `/dev/disk/by-label/RPI-RP2` before mounting it at `/mnt/usb`.

## Next work

1. Tune velocity thresholds (high, low, scale constant) based on real playing data.
2. Add per-key calibration if needed.
3. Consider a nonlinear velocity curve (square root, lookup table) if linear inverse feels wrong.
4. Add debounce/filtering if false triggers appear.
5. Add a configurable MIDI channel and note mapping.

## Coding notes

- Keep the firmware on the official Pico SDK/CMake stack, not Arduino or PlatformIO.
- Keep time-critical key sampling independent of USB logging or MIDI transmission.
- Preserve the 4 kHz target while implementing later features.
