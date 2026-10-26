# Fluxpad V2 — Three-Key USB MIDI Example

This is the original RP2040 proof of concept for the
[60-key Hall-effect MIDI controller](../../README.md). It uses the three analog
keys on a Fluxpad V2, not the main project's SPI sensor boards or Pico 2.

## Hardware and MIDI mapping

| Fluxpad key | GPIO | MIDI note | Pitch |
| --- | ---: | ---: | --- |
| key_4 | 26 | 60 | C4 |
| key_3 | 27 | 64 | E4 |
| key_2 | 28 | 67 | G4 |

The Fluxpad V2 schematic identifies TI DRV5056A3QDBZ Hall sensors. The sibling
`fluxpad` repository identifies Wooting Lekker analog switches, but does not
confirm the stock L45/L60 variant or switch generation. The main controller's
selection of Lekker V2 L60 switches is a separate design decision.

The device enumerates as composite TinyUSB CDC serial and USB MIDI. MIDI uses
channel 1. Incoming MIDI is drained without further processing.

## Current velocity algorithm

See [`src/main.c`](src/main.c) for the implementation and tuning constants.

- Target scan interval: 250 µs (4 kHz per key).
- Each ADC reading averages eight 12-bit samples.
- Start timing when the reading falls below **1600**.
- Send Note On when it falls below **1200**.
- Cancel an unfinished stroke if the reading returns to **1600 or above**.
- Convert travel time using a square-root-based curve: **2 ms or faster → 127**,
  **12 ms or slower → 24**, with intermediate values decreasing nonlinearly.
- Send Note Off when the reading returns to **1500 or above**.

These are prototype-specific thresholds, not calibrated defaults for the 60-key
controller. Scanning, USB servicing, and event logging currently share the main
loop; hard timing isolation remains work for the main controller.

## Build

From the repository root, with the Pico SDK and ARM GCC toolchain installed:

```sh
export PICO_SDK_PATH=$HOME/git/pico-sdk
cmake -S examples/fluxpad_midi -B build/fluxpad_midi -DPICO_BOARD=pico
cmake --build build/fluxpad_midi
```

Output: `build/fluxpad_midi/fluxpad_midi.uf2`.

The root compatibility build (`cmake -S . -B build` followed by
`cmake --build build`) still produces `build/fluxpad_midi.uf2`. Both entry points
build the same example; neither implements the 60-key controller.

## Flash and monitor

From the repository root:

```sh
bash examples/fluxpad_midi/deploy.sh
# Optional serial device override:
bash examples/fluxpad_midi/deploy.sh /dev/ttyACM1
```

The script configures/builds the example, requests bootloader mode through a
1200-baud serial touch, locates `/dev/disk/by-label/RPI-RP2`, mounts it at
`/mnt/usb`, copies the UF2, and unmounts it. It requires Python `pyserial`, sudo
mount permissions, and an existing `/mnt/usb` mount directory. The root
`bash deploy.sh` command forwards to this script.

Alternatively, enter BOOTSEL mode manually and copy the UF2 to the `RPI-RP2`
drive. After reboot, monitor press/release and velocity diagnostics with:

```sh
screen /dev/ttyACM0 115200
```
