# Isometric Piano — 60-Key Hall-Effect MIDI Controller

> **Current hardware decision (2026-11-04):** Gateron KS-20 White switches,
> existing 2.5 V ADC reference / Range 1, and Fluxpad V2 sensing geometry and
> PCB thickness. See [SWITCH_RANGE_DECISION.md](SWITCH_RANGE_DECISION.md).
> No KiCad change or fabrication approval has been made.

> **Sensor finding:** Fluxpad V2 assembly fields specify **OH49E-S / C85573**
> despite a DRV5056 symbol label. Keyboard12 still specifies DRV5056A3; sensor
> selection needs owner reconfirmation before sensing changes. No substitution
> is approved. See [the investigation](FLUXPAD_SENSOR_IDENTIFICATION.md).

The main project is the **PCB design** for a modular, velocity-sensitive USB MIDI keyboard using
Hall-effect switches. The goal is **60 keys, built from five identical 12-key
sensor boards and one Raspberry Pi Pico 2 master carrier**.

The original three-key Fluxpad V2 experiment is preserved as
[example firmware](examples/fluxpad_midi/README.md), not the main controller.

## Main controller architecture

- **Keys:** Gateron KS-20 White magnetic switches with DSA-profile keycaps.
- **Sensing:** one Hall sensor per key. Saved Keyboard12 specifies DRV5056A3QDBZ;
  final selection needs reconfirmation following the OH49E-S reference finding.
- **Key boards:** 12 sensors and one ADS7953 SPI ADC per board; no local MCU.
- **Master:** Raspberry Pi Pico 2 on a carrier with USB-C and a dedicated
  3.3 V supply for the sensor boards.
- **Interconnect:** shared SPI clock/data, with a separate chip-select per board.
- **MIDI:** 60 chromatic notes, MIDI 36–95, over USB.
- **Dynamics:** measure travel time between two sensor thresholds and convert
  it to MIDI velocity, with per-key calibration planned.
- **Scan target:** 4 kHz per key, independent of USB logging/transmission.

See [PCB_DESIGN.md](PCB_DESIGN.md) for the detailed hardware specification,
component selections, and open design decisions. It is a design specification,
not a claim that the hardware or SPI implementation has been validated.

## Project status

**Start the hardware handover with [HANDOVER.md](HANDOVER.md).** The current KiCad
project is `ECAD/Keyboard12/Keyboard12/Keyboard12.kicad_pro`.

**Current next task:** reconfirm the intended sensor with the owner, then compare
the placed Keyboard12 sensor geometry with Fluxpad V2, resolve the footprint
discrepancy, and apply approved sensing/thickness changes through Konnect.
The thickness target remains 1.0 mm. The owner selected ADS7953 / REF5025, Range 1 (0–2.5 V);
prototype range margin, ADC settling and other electrical/mechanical checks remain
open (see [TODO.md](TODO.md) and [SWITCH_RANGE_DECISION.md](SWITCH_RANGE_DECISION.md)).
The saved DRV5056 design's full theoretical output exceeds this ADC range, but
actual switch clipping has not been measured. Fluxpad's measured OH49E-S response
does not qualify DRV5056A3. The system has **six boards
total: five populated 12-key sensor boards plus one Pico 2 master carrier**.
Only the five sensor ADCs need chip-select lines. There is no sixth-sensor-board
expansion requirement. See the handover for the 60-key throughput budget.

| Area | Status |
| --- | --- |
| 60-key architecture | Specified in `PCB_DESIGN.md` |
| Hardware design files | Existing work in `ECAD/`; not certified fabrication-ready |
| Pico 2 / ADS7953 controller firmware | Not implemented yet |
| Per-key calibration and flash persistence | Planned |
| Three-key Fluxpad example | Implemented; retained as a reference for sensing, velocity, and USB MIDI |

## Repository layout

```text
ECAD/                       Main controller hardware design work
PCB_DESIGN.md               60-key hardware and firmware specification
examples/fluxpad_midi/      Standalone three-key RP2040 reference firmware
  src/                     Key scanning, velocity curve, and TinyUSB support
  CMakeLists.txt           Pico SDK example build
  deploy.sh                Build and flash the Fluxpad example
CMakeLists.txt              Compatibility build entry point for the example
deploy.sh                   Compatibility deployment wrapper for the example
```

There is no main-controller firmware target yet. Root-level build/deploy commands
still operate on the **Fluxpad example only**, to preserve existing workflows.

## PCB development priorities

1. Validate the 12-key board and master carrier design, including Pico 2 SPI pin
   assignments, ADS7953 framing/sequencing, power, and connector wiring.
2. Establish the current schematic/layout completion state through an electrical
   and mechanical review; do not infer readiness from the design specification.
3. Resolve sensor/switch alignment, board dimensions, mounting, and inter-board connectors.
4. Run ERC/DRC and review component footprints, sourcing, and assembly constraints.
5. Prepare fabrication outputs only after review and approval. Firmware bring-up
   is a later validation step, not the current handover objective.

## Run the reference example

Install the official Raspberry Pi Pico SDK and ARM GCC toolchain, then:

```sh
export PICO_SDK_PATH=$HOME/git/pico-sdk
cmake -S examples/fluxpad_midi -B build/fluxpad_midi -DPICO_BOARD=pico
cmake --build build/fluxpad_midi
```

This produces `build/fluxpad_midi/fluxpad_midi.uf2` for **Fluxpad V2 / RP2040**,
not the Pico 2 master. See the [example README](examples/fluxpad_midi/README.md)
for flashing, serial monitoring, and the current velocity algorithm.
