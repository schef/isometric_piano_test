# OH49E-S EasyEDA numbering — latest owner decision, 2026-11-04

Owner superseded the earlier instruction to retain PDF/native numbering and
selected **1 VCC / 2 VOUT / 3 GND**, matching the EasyEDA model convention.
Physical supply, ground and output lead locations remain unchanged. This is
not a sensor substitution, imported EasyEDA model, switch change or PCB sync.

## Implemented through Konnect

- Added explicitly named `Keyboard12:OH49E-S_EasyEDA`, and migrated all U3–U14.
  It keeps the existing functional pin coordinates but uses 2 for output and
  3 for ground. KS-20 labels, sourcing, wires, UUIDs and placements are retained.
- Renumbered the assigned combined library footprint's existing output land
  from 3 to 2 and isolated ground land from 2 to 3. No pad positions, sizes,
  graphics, hole or keepout geometry changed.
- Updated sensor descriptions, numbering notes, footprint status and footprint
  description/tags to identify the EasyEDA convention rather than call it the
  manufacturer's native numbering. The manufacturer PDF's 2 GND / 3 VOUT
  labels remain documented; functions, not numeric labels alone, establish mapping.
- Active footprint ID remains
  `Keyboard12_OH49E:SW_Cherry_MX_PCB_1.00u_hall_effect`.

## Verified

- [ERC](ECAD/reviews/2026-11-04-oh49e/erc-easyeda-numbering.json): zero errors,
  warnings and total violations at info severity, KiCad 10.0.6.
- [Readback evidence](ECAD/reviews/2026-11-04-oh49e/easyeda-numbering.json): all
  46 net endpoint sets preserved after the intentional sensor 2/3 renumbering.
  Every sensor has 1 to +3.3V, 2 to its output/filter path, and 3 to GND.
  Physical footprint geometry/keepout/artwork, schematic wires, symbol UUIDs,
  placements, hierarchy instances and unrelated symbols are unchanged.
- PCB hash is unchanged; no synchronization or fabrication output generated.

## Remaining tooling/layout gates

Konnect's library refresh updated only the first duplicate embedded definition,
so the original native symbol was restored consistently and the active EasyEDA
variant was given a distinct name. There are twelve identical embedded native
definitions (unused) and twelve identical active EasyEDA definitions. Checked
that each group is internally consistent; duplicate serialization cleanup still
needs supported tooling. No manual KiCad source edits were used.

**The latest numbering now agrees with legacy PCB pad roles (2 output / 3 GND).**
Do not perform an extra 2/3 net swap based on the superseded native-numbering
migration reports. A reviewed PCB-update plan must use the current schematic
and corrected library geometry, preserving physical functional connections.

PCB Editor transport, library-default property cleanup, inner-layer keepout,
assembly origin/orientation and the 1.0 mm stackup remain open as described in
`OH49E_FOOTPRINT_MIGRATION.md`. The possible separate-footprint approach in
`ONLINE_PARTS_INVESTIGATION.md` remains unimplemented and is not implied by this
numbering decision.
