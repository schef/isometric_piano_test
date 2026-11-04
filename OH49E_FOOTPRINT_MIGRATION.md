# OH49E-S combined footprint migration — 2026-11-04

> **Superseding numbering decision:** owner subsequently selected EasyEDA
> **1 VCC / 2 VOUT / 3 GND**. Current symbol/footprint use that convention;
> physical geometry from this report is unchanged. See
> `OH49E_EASYEDA_NUMBERING.md`. The native-number tables and required net swaps
> below describe the earlier checkpoint, not the current update instructions.

## Implemented through Konnect

Adapted the existing **project-local** combined footprint in place, rather than
rebuilding it and losing its switch keepout:

`ECAD/Keyboard12/Keyboard12/libraries/Fluxpad/Fluxpad.pretty/SW_Cherry_MX_PCB_1.00u_hall_effect.kicad_mod`

- Renumbered sensor lands to native OH49E-S **1 VCC / 2 GND / 3 VOUT**.
- Corrected sensor pad Y positions and sensor-side silk/fab/courtyard geometry
  for the reference B.Cu placement. Original hole/keepout objects are unchanged.
- Updated description/tags and classified the assembly as SMD (sensor assembly;
  switch anchor holes remain NPTH).
- Detached all three unqualified 3D model references, including the old Lekker
  switch and non-DSA keycap. Model files were not deleted. No qualified 3D model
  or complete mechanical-stack validation is claimed.
- Registered project-relative alias **`Keyboard12_OH49E`** to the existing local
  `.pretty` directory and assigned
  **`Keyboard12_OH49E:SW_Cherry_MX_PCB_1.00u_hall_effect`** to U3–U14.

This is an alias to the modified file, **not a second independent footprint
copy**. The old Tang nickname points at the same directory: do not reuse this
now-native-numbered asset with a legacy DRV5056 symbol. Its generic filename is
retained to avoid introducing an incomplete mechanical reconstruction.

## Geometry and checks

Canonical library F.Cu coordinates, millimetres:

| Native pad | Role | X | Y |
| --- | --- | --- | --- |
| 1 | VCC | 0.95 | −0.7375 |
| 2 | GND | 0 | 1.1375 |
| 3 | VOUT | −0.95 | −0.7375 |

Each sensor land remains 1.475 × 0.6 mm, angle 270°, rounded rectangle ratio
0.25. NPTH switch anchors remain at (±5.08, 0), diameter 1.75 mm.

[Footprint evidence](ECAD/reviews/2026-11-04-oh49e/footprint.json) records a
**read-only** KiCad 10.0.6 transform: load into an unsaved in-memory scratch
board, flip TOP_BOTTOM to B.Cu and match the reference U3 rotation/position.
Sensor pad positions, sizes and layers match the pinned Fluxpad board after
the documented functional pad renumbering. Transformed line geometry matches
the reference; structural comparison proves original holes and keepout unchanged.
No scratch board or transformed design source was saved outside Konnect.

- [ERC](ECAD/reviews/2026-11-04-oh49e/erc-footprint-assigned.json): zero errors,
  warnings or total violations at info severity.
- [Native netlist evidence](ECAD/reviews/2026-11-04-oh49e/connectivity-footprint-assigned.json):
  all 46 complete net endpoint sets preserved after the intended sensor pin
  renumbering; all twelve assigned footprint IDs and supply/GND/output pins checked.
- Project-library resolution through Konnect returned the intended file and
  its three native sensor lands plus two NPTH holes.

The earlier footprint-unassigned state and its evidence remain historical in
`OH49E_SCHEMATIC_MIGRATION.md`.

## Open gates — no PCB synchronization applied

1. **Live PCB transport:** a Konnect PCB-sync dry run was refused with
   `GetOpenDocuments (AS_UNHANDLED)`. The configured default socket belongs to
   the KiCad project manager; reconnect to the actual PCB Editor. The dry run
   returned a conflict and did not modify the PCB.
2. **Coordinate nets and geometry together:** legacy PCB pad 2 is output and pad
   3 is ground; the new footprint uses native OH49E numbering. Do not blindly
   refresh legacy footprints while preserving their old pad-number-to-net map.
   Review both net reassignment and footprint replacement/refresh in the update
   plan, preserving placement, switch holes and unrelated work. Verify readback
   for every sensor before saving a migrated board.
3. **Keepout coverage:** the preserved library keepout lists F.Cu/B.Cu and
   F.SilkS, unlike the upstream all-copper-layer rule. Saved Keyboard12 is four
   copper layers. Inner-layer coverage is not qualified; review the rule's
   purpose and required layer set before planes/routing. No keepout-layer writer
   is exposed in the loaded Konnect library tools.
4. **Library-default metadata:** the footprint's inherited Value/Datasheet/
   Description property fields still contain legacy metadata; Konnect's
   metadata writer updates footprint description/tags/attributes, not those
   property fields. The placed schematic has the correct OH49E-S identity,
   sourcing and datasheet. Resolve library defaults with supported tooling;
   do not generate production BOM/assembly outputs from legacy footprint defaults.
5. **Mechanical/assembly:** validate active Hall-point location, package clearance,
   sensor-side/polarity, placement origin and pick-and-place rotation against the
   actual assembly. Geometric correspondence is not measured qualification.
6. **Thickness and tooling cleanup:** apply the approved 1.0 mm physical stackup,
   reconcile older project configuration, and resolve duplicate embedded symbol
   definitions/formatting recorded in the schematic migration report. Existing
   project-file user changes are preserved. No fabrication release is approved.
