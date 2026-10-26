# PCB project handover

## Scope

The main deliverable is the hardware design for a modular **60-key Hall-effect
MIDI controller**. Prioritize the PCB project and its electrical/mechanical
readiness. The Fluxpad firmware in `examples/fluxpad_midi/` is reference material
only; building or flashing it is not part of this handover.

## Where to start

Open **`ECAD/Keyboard12/Keyboard12/Keyboard12.kicad_pro`** in KiCad.

| Path | Role |
| --- | --- |
| `ECAD/Keyboard12/Keyboard12/Keyboard12.kicad_sch` | Existing 12-key schematic |
| `ECAD/Keyboard12/Keyboard12/Keyboard12.kicad_pcb` | Existing 12-key PCB layout |
| `ECAD/Keyboard12/Keyboard12/Keyboard12.kicad_dru` | Project design-rule file |
| `ECAD/Keyboard12/Keyboard12/libraries/` | Local symbols, footprints, and 3D models |
| `ECAD/Keyboard12/Keyboard12/sym-lib-table` | Project symbol-library registration |
| `ECAD/Keyboard12/Keyboard12/fp-lib-table` | Project footprint-library registration |
| `PCB_DESIGN.md` | Intended system architecture and component decisions |

The directory inventory establishes that these files exist, **not that their
contents have passed review**. No ERC/DRC or electrical/mechanical audit was run
during this repository reorganization. A separate master-carrier KiCad project
was not found in the inspected `ECAD/` inventory; confirm its status with the owner.

## Intended architecture

- Five identical 12-key boards, each with 12 DRV5056A3QDBZ Hall sensors and an
  ADS7953SDBT SPI ADC; no MCU on the key boards.
- Wooting Lekker V2 L60 switches and DSA-profile, MX-stem keycaps.
- A Pico 2 master carrier, USB-C connection, and dedicated 3.3 V sensor-board supply.
- Shared SPI data/clock lines and a separate chip-select for each board.
- JLCPCB assembly for key boards; hand assembly for the master carrier.
- No per-key LEDs.

These are specified decisions, not verified statements about what is currently
placed or wired in the KiCad project. Reconcile the actual design with
`PCB_DESIGN.md` before making changes.

## Next hardware review

### Owner's current state and blocking concern

- Work stopped **just before routing** after a previous AI raised a possible
  ADS7953SDBT reference-voltage issue. The concern has not been independently
  verified; do not assume the ADC is either faulty or suitable without review.
- **Inspect this concern before routing.** Compare the actual schematic with the
  ADC datasheet: reference source and voltage, reference supply/headroom, input
  range selection versus Hall-sensor output range, analog/digital supplies,
  grounding, decoupling, and required ADC input-path connections.
- The ADC may be replaced. The functional requirement is **12 or 16 analog
  channels per board**, readable by the master, with sufficient system throughput
  for **six boards**. Other component choices may also change to correct errors.
- The owner confirmed **60 keys for now: five populated 12-key boards**.
  Six-board scan capacity remains a throughput/expansion requirement, not a
  72-key build requirement. Keep the current 60-key mechanical scope and MIDI map.
- Next milestone: review and resolve the electrical blocker, then finish the
  12-key board's placement/routing and readiness checks. Firmware development is
  not the immediate task.

### Switch-reading speed requirement

Use the existing **4 kHz per-key target** as the provisional design requirement
until the owner selects a different performance target. This gives a nominal
250 µs interval between readings of each key; it is not a measured result.

For six boards with 12 active keys each:

- 72 keys × 4,000 readings/s = **288,000 channel readings/s** system-wide.
- Each board supplies **48,000 channel readings/s**.
- At one 16-bit SPI frame per reading, useful transfer demand is at least
  **4.608 Mbit/s**, before configuration, pipeline/dummy frames, CS timing, and
  other overhead. At 20 MHz this is an ideal 57.6 µs of transfer time per full
  72-key scan, not a proven achievable scan time.
- If all 16 channels on each of six boards are scanned, this becomes
  **384,000 readings/s** and **6.144 Mbit/s** at 16 bits per reading.
- Do not copy the example's eight-conversion average without budgeting it:
  eight conversions per key at 4 kHz across 72 keys requires **2.304 MSPS** and
  **36.864 Mbit/s** at 16 bits per conversion, exceeding a single 20 MHz shared
  bus. Filtering/oversampling and bus architecture must be chosen together.

ADC selection must account for conversion rate, actual frame size and protocol,
sequencing/pipeline behavior, input settling with the sensor RC filter, shared-bus
timing, cable/signal integrity, and master scheduling. Verify the whole six-board
scan budget rather than using the ADC's advertised MSPS rating alone.

1. **Establish current state:** export schematic views/netlist and PCB views;
   identify unfinished placement, routing, and connectivity. Run ERC/DRC and
   retain the results. Do not label the board fabrication-ready without evidence.
2. **Validate ADC circuitry:** supply/reference requirements, decoupling,
   MXO/AINP/AINM connections, input range, unused channels, and SPI interface.
3. **Validate the key mechanism:** sensor-to-magnet position, switch footprint,
   polarity, travel/clearance, key spacing, keycaps, mounting, and board outline.
4. **Resolve the interconnect:** connector type/pinout, board ordering and unique
   chip-select distribution, ground returns, power capacity, and cable lengths.
   The specification's connector recommendation is not a finalized wiring design.
5. **Validate the master carrier:** Pico 2 hardware SPI pin mapping, USB test-pad
   access, USB-C/ESD circuit, power budget, regulator choice, and mechanical fit.
   Treat the pin table in the specification as requiring datasheet verification.
6. **Check fabrication/assembly:** local library portability, footprints and pin
   numbering against datasheets, BOM/LCSC identities and live stock, board rules,
   silkscreen, and assembly orientation. Generate production files only after approval.

## Preserve existing work

The working tree already contained user changes to `Keyboard12.kicad_pro`, lock-file
removals, and a new backup archive before this reorganization. Leave these intact.
Do not substitute a backup for the active project without the owner's direction.

Use Konnect MCP tools for all KiCad source modifications. Do not text-edit or
relocate design/library files as part of documentation cleanup; paths and project
references may depend on the existing directory structure.

## Firmware boundary

`examples/fluxpad_midi/` preserves the three-key RP2040 experiment. Its firmware
does not drive the ADS7953 boards or implement the 60-key controller. Root build
and deployment entry points remain compatibility wrappers for that example only.
The example source was relocated without changing its key-scanning/MIDI behavior.
