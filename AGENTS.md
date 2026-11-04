# Project handoff

## Purpose and scope

The main project is the **PCB design and hardware handover** for a modular
60-key Hall-effect, velocity-sensitive USB MIDI controller: five 12-key sensor
boards plus a Raspberry Pi Pico 2 master carrier.
`PCB_DESIGN.md` is the detailed design specification. Hardware work lives in
`ECAD/`. The main Pico 2 / ADS7953 firmware is not implemented yet.

The original Fluxpad V2 three-key RP2040 firmware is a standalone reference in
`examples/fluxpad_midi/`. Do not treat it as the main controller firmware or
claim the main controller is implemented because the example builds.

Read `HANDOVER.md` first. The user's priority is the PCB project, not firmware
development. Do not build, flash, or develop firmware as part of a PCB handover
unless explicitly requested. The example build instructions below are reference only.

Then read `TODO.md` for cross-session hardware progress. Update its checkboxes
and session log as tasks are verified, keeping decisions distinct from measured
validation and retaining references to evidence.

## Main project priorities

**2026-11-04 owner decision supersedes the older pending-range/Lekker text:**
use Gateron KS-20 White, retain ADS7953 / REF5025 2.5 V Range 1, and reproduce
Fluxpad V2 switch/sensor geometry and PCB thickness. Upstream thickness is
1.0 mm, confirmed by the owner's physical measurement; saved Keyboard12 is 1.6 mm.
Read `SWITCH_RANGE_DECISION.md` for measurement limits, the actual PCB footprint
discrepancy and remaining validation. No KiCad changes applied for this decision.

**Later 2026-11-04 sensor finding:** Fluxpad V2's DRV5056-labelled symbols and
assembly database specify **C85573 = OH49E-S**, not DRV5056A3. Catalog marking
1320 and the owner's half-supply baseline support this identification. Read
`FLUXPAD_SENSOR_IDENTIFICATION.md`. Keyboard12 still specifies DRV5056A3;
reconfirm sensor selection with the owner before implementing sensing changes.
No OH49E-S substitution is approved. Do not use the measured Fluxpad voltages
to qualify DRV5056A3 or use C85573 as its sourcing code. Review sensor-dependent
power, polarity, range and geometry after the decision; retain ADS7953 / REF5025
Range 1 as the selected ADC strategy, not as measured prototype qualification.

Owner clarification: work paused just before routing because a previous AI
flagged a possible ADS7953 reference-voltage issue. Initial review confirmed an
input-range limitation, not failed REF5025 supply headroom; actual switch clipping
is unmeasured. See `SCHEMATIC_REVIEW.md` for the 2.0V/0–4.0V extra-rail and
near-3.0V single-rail options and measurement gate for velocity thresholds.
The range decision is now selected; validate geometry, range margin and remaining
electrical checks before routing. For the retained ADS7953, the documented
sourcing choice is ADS7953SDBTR / C701670 (same chip/package as ADS7953SDBT,
tape-and-reel); saved KiCad fields remain unchanged. ADC/components may change;
the ADC must provide 12 or 16 channels and the master must support scanning five
sensor boards. The owner clarified that six boards total means five populated
12-key boards plus the Pico 2 master carrier: 60 keys and five chip-select lines.
There is no sixth-sensor-board expansion requirement; the master needs no CS.
See `HANDOVER.md` for the provisional 4 kHz/key budget,
including the shared-bus limit on copying eight-sample averaging.

1. Validate the hardware design, Pico 2 SPI pin assignment, ADS7953 protocol,
   interconnect, and power budget against the relevant datasheets.
2. Review the current 12-key schematic and PCB; record actual completion state
   and outstanding electrical/mechanical/layout work with evidence.
3. Resolve connector wiring, switch/sensor alignment, mounting, and board dimensions.
4. Validate libraries/footprints, run ERC/DRC, and check assembly sourcing.
5. Prepare fabrication outputs only after explicit review and approval.

The eventual firmware should preserve the 4 kHz per-key scan target and keep
sampling independent of USB logging/MIDI. Calibration and SPI bring-up are future
work, not prerequisites for documenting the current PCB handover.

## Example firmware

- `examples/fluxpad_midi/src/main.c`: GPIO 28, 27, 26 (keys 2, 3, 4).
- 250 µs target scan interval, eight ADC samples averaged per reading.
- Timing thresholds: start below 1600, finish below 1200; cancel at or above 1600.
- Square-root-based velocity curve: 2–12 ms maps from 127 down to 24.
- Release at or above 1500; MIDI notes 67, 64, 60 on channel 1.
- Composite TinyUSB CDC serial and USB MIDI; 1200-baud touch enters bootloader.
- The current example services USB and scans in one loop; it does not guarantee
  hard sampling isolation.

### Build and test the example

```sh
export PICO_SDK_PATH=$HOME/git/pico-sdk
cmake -S examples/fluxpad_midi -B build/fluxpad_midi -DPICO_BOARD=pico
cmake --build build/fluxpad_midi
```

Flash `build/fluxpad_midi/fluxpad_midi.uf2` to a Fluxpad V2, **not** the Pico 2
master. `bash examples/fluxpad_midi/deploy.sh` builds and flashes through the
1200-baud reset and `/dev/disk/by-label/RPI-RP2`, mounted at `/mnt/usb`.
Monitor `/dev/ttyACM0` at 115200 baud.

Root `CMakeLists.txt` and `deploy.sh` remain compatibility entry points for the
example. The root CMake build retains its original `build/fluxpad_midi.uf2` output.

## Coding and hardware notes

- Keep firmware on the official Pico SDK/CMake stack, not Arduino or PlatformIO.
- Keep RP2040 example configuration separate from future Pico 2 firmware.
- Clearly distinguish planned behavior from measured/implemented behavior.
- Modify KiCad design sources only through Konnect MCP tools, never text edits.
- Preserve existing hardware files and unrelated user changes.
