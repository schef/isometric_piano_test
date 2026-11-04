# Hardware progress tracker

Last updated: **2026-11-04**.

Read `HANDOVER.md` first, then this checklist at the start of each hardware
session. Update checkboxes and the session log when work is completed; include
evidence or paths to review results. Mark a task complete only when verified,
not merely proposed. Keep design decisions distinct from prototype validation.

## Approved decisions

**New evidence, 2026-11-04:** the physical Fluxpad reference is strongly
identified as OH49E-S / C85573, despite its DRV5056 schematic label. Earlier
sensor-retention decisions below remain recorded, but require owner
reconfirmation before implementation. See `FLUXPAD_SENSOR_IDENTIFICATION.md`.

- [x] Select Gateron KS-20 White switches.
- [x] Record the earlier decision to retain DRV5056A3QDBZ; this now needs
  reconfirmation following the OH49E-S finding, not silent implementation.
- [x] Retain ADS7953 with REF5025, Range 1 (0–2.5 V).
- [x] Choose Fluxpad V2 local switch/sensor geometry as the reference.
- [x] Confirm Fluxpad PCB thickness: upstream specifies 1.0 mm and owner
  physically measured 1.0 mm. Target Keyboard12 thickness is 1.0 mm.
- [x] Record preliminary Fluxpad measurement: approximately 1.48 V released
  and 0.87 V fully pressed for one key. Preliminary reported readings stayed
  below 2.5 V; these characterize the reference unit, not a qualified worst-case range.

Details and sources: `SWITCH_RANGE_DECISION.md`. The engineering specification
is linked there and in `PCB_DESIGN.md` and `HANDOVER.md`.

## 1. Sensor reconfirmation, then sensing geometry — next task

- [x] Investigate the reference sensor discrepancy: Fluxpad schematic and JLCPCB
  database specify C85573, which LCSC identifies as OH49E-S; catalog photo
  matches the 1320 marking. Evidence: `FLUXPAD_SENSOR_IDENTIFICATION.md`.
- [ ] Reconfirm the main-board sensor with the owner before implementing sensing
  changes. Do not attribute measured Fluxpad voltages to DRV5056A3 or treat
  C85573 as that TI part's sourcing code. No sensor substitution is approved yet.
- [ ] Compare placed Keyboard12 sensor footprints against Fluxpad V2 and resolve
  differing sensor-pad positions relative to the switch centre. Do not rely on
  matching footprint names; the local library and placed PCB assets differ.
- [ ] Verify sensor pin-to-pad mapping, orientation and PCB side against the approved part's
  datasheet and the physical reference.
- [ ] Verify switch seating, mounting height, magnet alignment, full travel and
  sensor/package clearance with a 1.0 mm PCB.
- [ ] Reconcile measured falling voltage with the KS-20 specification's magnetic
  polarity/field information using the actual sensor geometry and verified
  measurement point. Do not treat the illustrative thickness extrapolation as
  a validated voltage prediction.

## 2. Apply approved KiCad changes

**Use Konnect MCP only for KiCad source modifications.** Preserve unrelated
user work. No KiCad changes have been applied for the switch/range decision.

- [ ] Change saved Keyboard12 PCB thickness from 1.6 mm to 1.0 mm and review
  the resulting stackup/fabrication settings.
- [ ] Correct the local switch/sensor arrangement as established by section 1.
- [ ] Update relevant component sourcing fields after verifying identities and
  approval. Retained ADC sourcing choice: ADS7953SDBTR / C701670; recheck stock.
- [x] Reconcile outdated switch, cost and pending-range statements throughout
  project documentation with the approved decision; preserve historical review
  evidence and distinguish selected design from unvalidated performance.
- [ ] Repeat ERC and native netlist review after approved schematic changes.

## 3. Electrical and system review

- [ ] Recalculate five-board power and USB input-current budget for the approved
  sensor, ADC/reference loads, master and losses. The saved DRV5056 design allows
  up to 600 mA for 60 sensors alone; that is not an OH49E-S specification.
- [ ] Validate regulator choice, voltage drop, startup, bulk capacitance and
  distributed decoupling.
- [ ] Finalize connector population, pinout, current rating, ground returns,
  cable lengths and shared-SPI signal integrity.
- [ ] Review five chip-select selection jumpers, local CS pull-up and defined
  deselection during master reset/disconnection.
- [ ] Validate ADS7953 input settling, reference circuitry and RC-filter loading.
- [ ] Establish ADS7953 frame/pipeline/CS timing and five-board scan budget for
  60 keys at the provisional 4 kHz/key target. Do not assume eightfold averaging
  fits the shared bus.
- [ ] Correct and verify Pico 2 hardware SPI pin assignments against datasheets.
- [ ] Establish master-carrier project status; review USB-C/test-pad access,
  ESD, power circuitry and mechanical fit. A separate carrier project has not
  been found in the inspected repository inventory.

## 4. Layout and readiness

- [ ] Confirm board outline, key spacing, mounting holes, keycap clearance and
  inter-board mechanical arrangement.
- [ ] Validate custom symbols/footprints, pin numbering, assembly orientation,
  placement origins and project-library portability.
- [ ] Complete placement, decoupling/reference layout and routing after the
  relevant electrical/mechanical checks are resolved.
- [ ] Run and retain final ERC/DRC results; resolve or justify findings.
- [ ] Recheck BOM identities, live stock, assembly eligibility and pricing.
- [ ] Obtain explicit owner review/approval before generating fabrication and
  assembly outputs. Do not label the board fabrication-ready without evidence.

## 5. Future prototype validation

- [ ] Check several KS-20/sensor pairs over full travel; establish margin inside
  0–2.5 V including supply, mechanical and component variation.
- [ ] Validate ADC settling and cross-channel behaviour at the chosen scan rate.
- [ ] Validate RC-filter response and select calibrated velocity/release thresholds.
- [ ] Confirm actual full-system scan timing during later bring-up.

Firmware development, building and flashing are **not part of the current PCB
handover** unless explicitly requested. The existing Fluxpad example is not the
main 60-key controller implementation.

## Session log

### 2026-11-04

- Recorded KS-20 / 2.5 V Range 1 decision and physical 1.0 mm PCB measurement.
- Linked official KS-20 White engineering specification, including page-5
  mechanical cross-section and magnetic specifications.
- Created this tracker with the placed sensor-footprint comparison as the initial
  next task; later sensor evidence added owner reconfirmation as the preceding gate.
- No KiCad changes or fabrication approval recorded.
- Updated `README.md`, `HANDOVER.md`, `PCB_DESIGN.md`, `AGENTS.md`,
  `SCHEMATIC_REVIEW.md`, `MAX11131_EVALUATION.md` and the example README to
  reflect KS-20 White / REF5025 Range 1 and the current geometry task. Corrected
  documented CS framing, sensor sensitivity/current, connector description and
  proposed SPI pin roles; no hardware validation claimed or KiCad sources changed.
- Investigated Fluxpad upstream history, assembly database, older BOM and
  firmware LUT change. Found C85573 = OH49E-S despite the DRV5056 symbol label;
  recorded catalog marking evidence and owner's +1 mm / removed-switch readings
  in `FLUXPAD_SENSOR_IDENTIFICATION.md`. Sensor decision needs reconfirmation;
  no KiCad changes, fabrication approval or further push performed.
- Removed machine-local evidence paths and temporary artifact references from
  Markdown documentation; replaced sibling Fluxpad paths with public,
  commit-pinned GitHub links. Recorded findings and validation limits are retained.
- Propagated the OH49E-S finding through current guidance and the historical
  reviews: separated saved DRV5056 specifications from reference measurements,
  corrected C85573 sourcing, and made owner sensor reconfirmation the next gate.
  No replacement sensor selected or KiCad/firmware changes made.
