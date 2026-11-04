# Hardware progress tracker

Last updated: **2026-11-04**.

Read `HANDOVER.md` first, then this checklist at the start of each hardware
session. Update checkboxes and the session log when work is completed; include
evidence or paths to review results. Mark a task complete only when verified,
not merely proposed. Keep design decisions distinct from prototype validation.

## Approved decisions

**Latest owner decision, 2026-11-04:** select **OH49E-S / C85573** for the main
board following the preliminary evaluation in `OH49E_EVALUATION.md`. This
supersedes earlier DRV5056 retention and pending-reconfirmation guidance.
Electrical/mechanical qualification and fabrication approval remain open.

**Latest implementation:** schematic U3–U14 use `Keyboard12:OH49E-S_EasyEDA`
with **1 VCC / 2 VOUT / 3 GND**, per the latest owner correction,
and the remapped project-local combined footprint via the `Keyboard12_OH49E`
alias. PCB remains legacy/unsynchronized. See `OH49E_FOOTPRINT_MIGRATION.md`
for retained geometry/ERC/netlist evidence and remaining layout/tooling gates.

**Latest investigation:** `ONLINE_PARTS_INVESTIGATION.md` identifies an exact
pad-geometry match in stock KiCad `Package_TO_SOT_SMD:SOT-23`, and a separate
mechanical footprint in HE60 with matching switch-anchor holes. Reuse still
requires an explicit symbol/pad numbering decision, licensing review for external
assets, and retained keepout/geometry validation. No design changes or replacement
architecture approved as part of this research.

**Latest owner correction:** use EasyEDA numbering **1 VCC / 2 VOUT / 3 GND**,
superseding the preceding instruction to retain native/PDF numbering. Symbol and
footprint now agree; physical wiring is unchanged. See
`OH49E_EASYEDA_NUMBERING.md` and its retained ERC/netlist/geometry evidence.
Legacy PCB pad roles already use 2 output / 3 ground: do not apply an additional
swap based on superseded native-numbering guidance.

- [x] Select Gateron KS-20 White switches.
- [x] Select OH49E-S / C85573, superseding the earlier DRV5056A3QDBZ decision.
- [x] Retain ADS7953 with REF5025, Range 1 (0–2.5 V).
- [x] Choose Fluxpad V2 local switch/sensor geometry as the reference.
- [x] Confirm Fluxpad PCB thickness: upstream specifies 1.0 mm and owner
  physically measured 1.0 mm. Target Keyboard12 thickness is 1.0 mm.
- [x] Record preliminary Fluxpad measurement: approximately 1.48 V released
  and 0.87 V fully pressed for one key. Preliminary reported readings stayed
  below 2.5 V; these characterize the reference unit, not a qualified worst-case range.

Details and sources: `SWITCH_RANGE_DECISION.md`. The engineering specification
is linked there and in `PCB_DESIGN.md` and `HANDOVER.md`.

## 1. Sensor mapping and sensing geometry — next task

- [x] Investigate the reference sensor discrepancy: Fluxpad schematic and JLCPCB
  database specify C85573, which LCSC identifies as OH49E-S; catalog photo
  matches the 1320 marking. Evidence: `FLUXPAD_SENSOR_IDENTIFICATION.md`.
- [x] Complete preliminary OH49E-S datasheet/catalog and saved-footprint
  evaluation: `OH49E_EVALUATION.md`. Not production qualification or substitution
  approval; manufacturer pin numbers differ from the existing custom pad numbers.
- [x] Reconfirm the main-board sensor with the owner: OH49E-S / C85573 approved
  after preliminary evaluation. Do not attribute measured Fluxpad voltages to
  DRV5056A3 or treat C85573 as that TI part's sourcing code.
- [ ] Compare placed Keyboard12 sensor footprints against Fluxpad V2 and resolve
  differing sensor-pad positions relative to the switch centre. Do not rely on
  matching footprint names; the local library and placed PCB assets differ.
  Saved-file comparison now covers all twelve instances; live side-aware
  resolution remains open. See `OH49E_EVALUATION.md`.
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
user work. Switch/sensor schematic updates and library pin/geometry mapping are
implemented. PCB geometry, physical stackup and assembly validation remain pending.

- [x] Create/register native OH49E-S symbol and migrate all U3–U14, preserving
  functional wiring and KS-20 metadata. Update sensor identity/datasheet fields.
  Evidence: `OH49E_SCHEMATIC_MIGRATION.md`; ERC zero violations and complete
  46-net native endpoint comparison passed with intended sensor renumbering.
- [x] Remap existing combined KS-20/OH49E-S footprint with native numbering and
  side-aware reference geometry, preserving NPTH holes and existing keepout;
  assign to all twelve sensors through `Keyboard12_OH49E`. Evidence:
  `OH49E_FOOTPRINT_MIGRATION.md`, reference-transform and 46-net checks, clean ERC.
- [ ] Review inner-copper keepout coverage on the saved four-layer board,
  inherited library-default property metadata and physical assembly origin/
  orientation; geometric correspondence is not production qualification.
- [ ] Restore PCB Editor transport, review coordinated net reassignment and
  footprint replacement/refresh, then verify all sensor pads before saving.
  A blind library refresh would preserve the wrong legacy pin-number/net map.
- [ ] Resolve Konnect serialization artifacts before schematic sign-off: twelve
  identical OH49E-S embedded definitions and tab-only blank lines. Native ERC,
  netlist comparison and rendering pass; library-refresh dry run offers no change.
  See `OH49E_SCHEMATIC_MIGRATION.md`. No manual design-source text cleanup allowed.
  Latest capability check: serving Konnect 0.13.0 still has no embedded-symbol
  deduplication operation, and its library metadata writer cannot edit footprint
  Value/Datasheet/Description property fields. Cleanup is blocked pending
  supported Konnect functionality or owner changes through KiCad's native editors;
  source text edits are not an approved workaround. Native editor save may
  normalize duplicate definitions, but this must be verified after save.
- [x] Investigate existing sensor/switch CAD alternatives before more custom
  library edits: stock KiCad, HE60, Drachma and LCSC/EasyEDA. Evidence:
  `ONLINE_PARTS_INVESTIGATION.md`; no external design assets imported.
- [ ] Owner chooses whether to split sensor and switch mechanics using existing
  footprints or finish the combined asset. Do not directly attach standard
  SOT-23 pads to the current native-numbered symbol: ground/output mapping differs.

- [x] Replace obsolete Lekker switch labels on U3–U14 with Gateron KS-20 White
  through Konnect; add switch manufacturer/model/specification fields. Verified
  only Value and switch metadata changed; pins, wires, UUIDs and footprint
  assignments are unchanged. Sensor labels still identify the legacy DRV5056
  symbols pending the separate OH49E-S migration. ERC: zero errors/warnings.

**Implementation blocker, 2026-11-04:** opened the Keyboard12 PCB editor, but
the serving Konnect process targets `ipc:///tmp/kicad/api.sock`, owned by the
KiCad project manager, not PCB Editor. Read-only board queries fail with
`GetOpenDocuments (AS_UNHANDLED)`. The launched PCB Editor exposes a separate
per-process socket; reconnect Konnect to the PCB Editor before live inspection
or mutation. Do not replace socket files or bypass this through source edits.
Konnect's exposed stackup tool is read-only; physical thickness changes require
Board Setup or a supported Konnect writer. Project configuration still records
an older 1.6 mm four-layer candidate; it does not supersede the approved 1.0 mm
target and must be reconciled, not silently treated as the new stackup.

- [ ] Change saved Keyboard12 PCB thickness from 1.6 mm to 1.0 mm and review
  the resulting stackup/fabrication settings.
- [ ] Correct the local switch/sensor arrangement as established by section 1.
- [ ] Update relevant component sourcing fields after verifying identities and
  approval. Retained ADC sourcing choice: ADS7953SDBTR / C701670; recheck stock.
- [x] Reconcile outdated switch, cost and pending-range statements throughout
  project documentation with the approved decision; preserve historical review
  evidence and distinguish selected design from unvalidated performance.
- [x] Repeat ERC and native netlist review after OH49E-S schematic migration.
  Re-run after the pending footprint assignment and any later schematic changes.

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

- Session paused at owner's request for commit/push and later continuation.
  Next session: guide native KiCad library-property cleanup and schematic save,
  verify duplicate counts afterward, then repeat ERC/netlist checks. Current
  EasyEDA numbering is selected and implemented; no PCB synchronization, routing
  or fabrication release. Unrelated project-file normalization changes, debug log
  and editor lock remain outside the checkpoint commit and must be preserved.
- Owner requested library-metadata and duplicate-definition cleanup (step 1).
  Rechecked serving Konnect 0.13.0 and loaded library, schematic, verification
  and editor-navigation capabilities. No supported field-property writer or
  embedded-definition deduplication operation is exposed. No design-source
  changes made for this attempt; cleanup remains blocked, not marked complete.
- Owner corrected the numbering decision to EasyEDA convention, 1 VCC / 2 VOUT /
  3 GND. Applied to active symbols and assigned combined footprint via Konnect,
  preserving physical wiring/geometry. Introduced explicitly named EasyEDA symbol
  to avoid mixing conventions amid Konnect's duplicate-definition issue. ERC
  zero violations and all 46 functional net endpoint sets verified. Evidence:
  `OH49E_EASYEDA_NUMBERING.md`. No PCB synchronization or architecture change.
- Owner directed keeping the current OH49E-S numbering (1 VCC / 2 GND / 3 VOUT)
  and confirmed the physical wiring is OK. No further renumbering, CAD substitution
  or KiCad change made for this clarification; no new continuity test recorded.
- Investigated existing CAD assets at owner request. Found stock KiCad SOT-23
  pads exactly match reference geometry after rotation/0.2 mm offset and explicit
  functional renumbering; HE60 MX-1U has matching NPTH anchors but no keepout.
  Drachma and EasyEDA OH49E models use conventional package-pad numbering, not
  the manufacturer's native labels. Recorded source commits, license caveats,
  differing 1.6 mm assemblies and unverified sourcing/lifecycle claims in
  `ONLINE_PARTS_INVESTIGATION.md`. No KiCad sources changed or approvals inferred.
- Adapted the existing local combined footprint in place through Konnect:
  native OH49E numbering, corrected sensor pad/silk/fab/courtyard Y geometry,
  retained original holes/keepout, detached unqualified model references without
  deleting model files. Registered project-relative `Keyboard12_OH49E` alias
  and assigned all U3–U14. Read-only KiCad transform matches pinned Fluxpad pad
  geometry/artwork. ERC zero violations; complete 46-net native comparison passed.
  Evidence: `OH49E_FOOTPRINT_MIGRATION.md` and review JSON files. PCB-sync dry run
  refused on project-manager transport; no PCB change. Inner-layer keepout,
  library-default metadata, assembly checks, stackup and serialization cleanup
  remain open.
- Migrated U3–U14 to project-local `Keyboard12:OH49E-S` through Konnect, with
  native manufacturer pin numbering and corrected sourcing/datasheet/description.
  Retained KS-20 metadata and functional wiring. Compared all 46 native net
  endpoint sets before/after, normalized for sensor pin renumbering; verified
  preserved UUIDs, hierarchy, placements, wires and unrelated symbols. ERC:
  zero errors/warnings/violations at info severity. Render inspected and fields
  repositioned. Evidence: `OH49E_SCHEMATIC_MIGRATION.md` and
  `ECAD/reviews/2026-11-04-oh49e/`. Old incompatible combined footprint assignments
  cleared deliberately; PCB unchanged, no sync or fabrication release.
- Updated all twelve combined key-assembly schematic Values to
  `DRV5056A3QDBZx / Gateron KS-20 White` and added hidden switch manufacturer,
  model and official specification fields through Konnect. Read-only structural
  comparison confirmed no changes outside these properties. ERC at info severity
  returned zero violations (KiCad 10.0.6). Resulting schematic SHA-256:
  `bcd304ef69e896898eb824005664ab5e5d307bea2dd03f998c2db105dea2dda6`.
  Stale schematic locks were moved aside only after verifying the same host
  and absence of Eeschema. No PCB synchronization or sensor pin migration done.
- Attempted approved implementation: verified U3's saved schematic fields with
  Konnect and launched the actual Keyboard12 PCB in PCB Editor. Diagnosed live
  API failure as Konnect targeting the project-manager socket instead of PCB
  Editor's per-process socket. Stackup writer is not exposed. No KiCad source
  changes applied; mapping, geometry, thickness and ERC tasks remain incomplete.
- Owner explicitly approved OH49E-S / C85573 as the main-board sensor after
  preliminary evaluation. Recorded selection across current project guidance;
  no KiCad changes, prototype qualification or fabrication approval implied.
- Evaluated OH49E-S manufacturer datasheet and LCSC/JLCPCB catalog identity in
  `OH49E_EVALUATION.md`. Confirmed supported supply range includes 3.3 V, but most
  electrical limits are at 5 V and no maximum supply current is provided. Stock
  quantity/price remain unverified. Explicitly distinguished OH49E-S manufacturer
  pin numbers from DRV-style custom pad numbers. Compared all twelve saved
  sensor footprints with pinned Fluxpad assets; local-Y reflection confirmed.
  Konnect live inspection failed with `GetOpenDocuments (AS_UNHANDLED)`;
  no unsaved-state validation, KiCad changes or sensor approval claimed.
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
