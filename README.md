# Isometric Piano Test

RP2040 firmware for experimenting with velocity-sensitive piano keys on Fluxpad V2 Hall-effect hardware. It enumerates as a composite USB CDC serial and USB-MIDI device.

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
