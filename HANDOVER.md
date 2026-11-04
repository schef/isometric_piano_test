# PCB project handover

## Current decision — 2026-11-04

Owner selected **Gateron KS-20 White**, the existing **2.5 V reference /
ADS7953 Range 1**, and copying Fluxpad V2's sensing geometry and PCB thickness.
The upstream board specifies **1.0 mm**, confirmed by the owner's physical
measurement, not Keyboard12's saved **1.6 mm**. See
[SWITCH_RANGE_DECISION.md](SWITCH_RANGE_DECISION.md) for measurements, the
footprint discrepancy and remaining validation. This supersedes the older
Lekker selection and pending-range discussion. No KiCad changes applied.

**Later sensor finding:** Fluxpad V2 uses a DRV5056-labelled symbol but specifies
**C85573 = OH49E-S** in both schematic sourcing fields and its assembly database.
The catalog marking and owner's measurements support OH49E-S in the reference
unit. Keyboard12 still specifies DRV5056A3; **reconfirm the sensor with the owner
before implementing sensing changes**. No substitution is approved. See
[FLUXPAD_SENSOR_IDENTIFICATION.md](FLUXPAD_SENSOR_IDENTIFICATION.md).

Switch reference: [official KS-20 White engineering specification (PDF)](https://www.gateron.com/u_file/2311/10/file/GATERONMagneticWhiteSwitch-KS-20TF10B045NW-X72.pdf),
especially page 5 for the cross-section, polarity and mechanical dimensions.

## Scope

The main deliverable is the hardware design for a modular **60-key Hall-effect
MIDI controller**. Prioritize the PCB project and its electrical/mechanical
readiness. The Fluxpad firmware in `examples/fluxpad_midi/` is reference material
only; building or flashing it is not part of this handover.

## Where to start

Read [TODO.md](TODO.md) for the current cross-session checklist and update it
as work is verified. The next task is owner reconfirmation of the intended
sensor, followed by the placed sensor-footprint comparison.

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

- Five identical 12-key boards, each with 12 Hall sensors and an ADS7953 SPI ADC;
  no MCU on the key boards. Saved Keyboard12 specifies DRV5056A3QDBZ, but final
  sensor selection needs reconfirmation; the ADC sourcing choice is ADS7953SDBTR.
- Gateron KS-20 White switches and DSA-profile, MX-stem keycaps.
- A Pico 2 master carrier, USB-C connection, and dedicated 3.3 V sensor-board supply.
- Shared SPI data/clock lines and a separate chip-select for each board.
- JLCPCB assembly for key boards; hand assembly for the master carrier.
- No per-key LEDs.

These are specified decisions, not verified statements about what is currently
placed or wired in the KiCad project. Reconcile the actual design with
`PCB_DESIGN.md` before making changes.

## Next hardware review

**2026-11-02 initial schematic review:** see [SCHEMATIC_REVIEW.md](SCHEMATIC_REVIEW.md).
ERC reports zero violations, but datasheet review confirms an ADC input-range
limitation: with the existing 3.3 V analog supply and 2.5 V reference, Range 1
does not cover the specified DRV5056's full possible output, and Range 2 does not meet
the specified supply condition. Actual switch clipping is not measured. The
reference generator's 3.3 V input is not itself the identified problem. The review
also records a higher sensor worst-case current. The earlier missing-sixth-CS
finding is withdrawn: the owner clarified six boards total means five sensor
boards plus the Pico 2 master, requiring only five CS lines.
The owner has since selected 2.5 V / Range 1; this is a design decision, not
prototype qualification. No electrical changes have been applied. Resolve the
remaining geometry, power, interconnect and protocol checks before routing.

**Sourcing/options update:** for the retained ADS7953, the documented sourcing choice
is **ADS7953SDBTR / C701670**, the tape-and-reel equivalent of ADS7953SDBT with
the same package/pinout. Owner screenshot shows 13 stocked at US$8.5952 each;
recheck before ordering. Saved KiCad fields are unchanged. The owner selected
the existing REF5025 / Range 1, with no additional analog rail or per-key
attenuation. Earlier alternatives remain recorded in `SCHEMATIC_REVIEW.md` as
historical options, not pending selections. Validate actual sensor output versus
travel, range margin and dynamic velocity/release crossings on the selected
KS-20 / Fluxpad geometry; the preliminary one-key measurement does not qualify
worst-case coverage or establish DRV5056A3 behaviour: the measured reference is
strongly identified as OH49E-S. Recalculate sensor-dependent power and range
checks for whichever sensor is approved.

### Owner's current state and blocking concern

- Work stopped **just before routing** after a previous AI raised a possible
  ADS7953SDBT reference-voltage issue. Initial review established an input-range
  limitation, not failed REF5025 supply headroom; actual switch clipping is unmeasured.
- **Current selection:** retain ADS7953 / REF5025, Range 1. Schematic-level
  findings are in `SCHEMATIC_REVIEW.md`; physical decoupling, grounding, input
  settling, range margin and system timing still require validation.
- The functional requirement remains **12 or 16 analog channels per board**,
  readable by the master, with sufficient system throughput for **five sensor
  boards**. No ADC replacement is selected.
- The owner confirmed **60 keys for now: five populated 12-key boards**.
  The sixth board is the Pico 2 master carrier, not an additional sensor board.
  Five chip-select lines suffice; there is no sixth-sensor-board expansion
  requirement. Keep the current 60-key mechanical scope and MIDI map.
- Next milestone: reconfirm sensor selection, resolve the placed sensor-footprint
  discrepancy, apply approved
  geometry/thickness changes through Konnect, and complete remaining electrical
  checks before routing. Firmware development is not the immediate task.

### Switch-reading speed requirement

Use the existing **4 kHz per-key target** as the provisional design requirement
until the owner selects a different performance target. This gives a nominal
250 µs interval between readings of each key; it is not a measured result.

For five sensor boards with 12 active keys each (plus one master carrier):

- 60 keys × 4,000 readings/s = **240,000 channel readings/s** system-wide.
- Each board supplies **48,000 channel readings/s**.
- At one 16-bit SPI frame per reading, useful transfer demand is at least
  **3.840 Mbit/s**, before configuration, pipeline/dummy frames, CS timing, and
  other overhead. At 20 MHz this is an ideal 48 µs of transfer time per full
  60-key scan, not a proven achievable scan time.
- If all 16 channels on each of five sensor boards are scanned, this becomes
  **320,000 readings/s** and **5.120 Mbit/s** at 16 bits per reading.
- Do not copy the example's eight-conversion average without budgeting it:
  eight conversions per key at 4 kHz across 60 keys requires **1.920 MSPS** and
  **30.720 Mbit/s** at 16 bits per conversion, exceeding a single 20 MHz shared
  bus. Filtering/oversampling and bus architecture must be chosen together.

ADC selection must account for conversion rate, actual frame size and protocol,
sequencing/pipeline behavior, input settling with the sensor RC filter, shared-bus
timing, cable/signal integrity, and master scheduling. Verify the whole five-sensor-board
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
