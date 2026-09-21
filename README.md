# Isometric Piano Test

RP2040 firmware for experimenting with velocity-sensitive piano keys on Fluxpad V2 Hall-effect hardware. It enumerates as a composite USB CDC serial and USB-MIDI device.

## Key switches and sensors

- **Analog keys: Wooting Lekker Hall-effect switches.** The sibling `../fluxpad` repository names Wooting Lekker in its README, and its Fluxpad V2 RevB PCB has three Hall-key footprints labeled `DRV5056A3QDBZx Lekker`. The exact stock switch variant (L45 versus L60) and switch generation are not confirmed by those sources.
- **Hall sensors: Texas Instruments DRV5056A3QDBZ**, as identified in the V2 schematic. These are the PCB-mounted sensors, not the physical key switches.
- **Digital keys: Gateron Red**, according to the Fluxpad README. That README describes an older two-analog-key configuration, so this is not definitive confirmation of the digital switches fitted to every V2 unit. This firmware uses the three analog keys only.

Source files in the sibling repository: `../fluxpad/README.md`, `../fluxpad/ECAD/FluxpadKicad/Fluxpad.kicad_pcb`, and `../fluxpad/ECAD/FluxpadKicad/Fluxpad.kicad_sch`.

The proposed 60-key controller in [PCB_DESIGN.md](PCB_DESIGN.md) separately selects **Wooting Lekker V2 L60 Linear switches with 60 g springs**. That design choice does not establish which switch variant shipped in the Fluxpad V2; the board version and switch generation are distinct.

## MIDI mapping

| Fluxpad key | MIDI note | Pitch |
| --- | ---: | --- |
| key_4 | 60 | C4 |
| key_3 | 64 | E4 |
| key_2 | 67 | G4 |

## Velocity algorithm

Two-threshold timing, modeled after the dual-switch approach used by Fatar, Yamaha, and Roland keyboards.

- ADC scan rate: 4 kHz per key (one scan every 250 µs)
- Each reading: average of 8 raw 12-bit ADC samples
- **Timing starts** when ADC falls below 1600 (start of key travel)
- **Timing stops** when ADC falls below 600 (near bottom). Note On is sent with the measured velocity.
- If ADC rises back above 1600 before hitting 600, the stroke is cancelled (no false trigger)
- Velocity is calculated as: `velocity = 1,800,000 / travel_time_us`, clamped to 1–127
- Minimum travel time: 1.5 ms (prevents overflow, fastest realistic strike)
- **Note Off** is sent when ADC rises above 800 (hysteresis band above the low threshold)

Typical velocity values:

| Strike | Travel time | Velocity |
| --- | --- | --- |
| Hard | ~5 ms | 127 |
| Medium | ~15 ms | 120 |
| Soft | ~40 ms | 45 |
| Very soft | ~80 ms | 22 |

## Build

Install the [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk) and an ARM GCC toolchain. Then:

```sh
export PICO_SDK_PATH=$HOME/git/pico-sdk
cmake -S . -B build
cmake --build build
```

The firmware image is `build/fluxpad_midi.uf2`.

## Build and flash in one command

`deploy.sh` rebuilds the firmware, resets the connected Fluxpad into its RP2040 bootloader, detects the boot drive by its `RPI-RP2` filesystem label, mounts it at `/mnt/usb`, copies the UF2, and unmounts it.

```sh
bash deploy.sh
```

Pass a different serial device if needed:

```sh
bash deploy.sh /dev/ttyACM1
```

## Flash and monitor

1. While the stock Fluxpad firmware is installed, open and close its serial port at 1200 baud to enter the RP2040 bootloader:

   ```sh
   python3 -c 'import serial; p = serial.Serial("/dev/ttyACM0", 1200); p.close()'
   ```

2. Copy `build/fluxpad_midi.uf2` to the mounted `RPI-RP2` drive.
3. After reboot, the device appears as a MIDI input/output and as a serial port. View velocity output with:

   ```sh
   screen /dev/ttyACM0 115200
   ```
