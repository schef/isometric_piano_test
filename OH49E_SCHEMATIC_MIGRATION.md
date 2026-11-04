# OH49E-S schematic migration — 2026-11-04

> **Latest numbering correction:** the owner subsequently selected EasyEDA
> **1 VCC / 2 VOUT / 3 GND**. Active symbol is now `Keyboard12:OH49E-S_EasyEDA`;
> see `OH49E_EASYEDA_NUMBERING.md`. Native-numbering details below are historical.

> **Subsequent footprint assignment:** the combined footprint has now been
> remapped and assigned to all twelve sensors through `Keyboard12_OH49E`.
> The unassigned state below records the earlier schematic-only checkpoint.
> See `OH49E_FOOTPRINT_MIGRATION.md` for current evidence and remaining gates.

## Implemented

All U3–U14 in `ECAD/Keyboard12/Keyboard12/Keyboard12.kicad_sch` now use the
project-local **`Keyboard12:OH49E-S`** symbol, not a renamed DRV5056 symbol.
The symbol is in `libraries/Keyboard12.kicad_sym`, registered through a
`${KIPRJMOD}`-relative entry in the project `sym-lib-table`.

Changes were made through Konnect only:

- Native manufacturer pin numbers: **1 VCC, 2 GND, 3 VOUT**.
- Existing supply, ground and output wire endpoints retained by placing the
  corresponding new functional pins at the original coordinates. Thus old
  schematic pin 2 / OUT becomes new pin 3 / VOUT; old pin 3 / GND becomes new 2.
- Values: `OH49E-S / Gateron KS-20 White`.
- Manufacturer Nanjing Ouzhuo Tech, MPN OH49E-S, JLC Part C85573 and official
  OH49E datasheet recorded. Existing KS-20 switch metadata preserved.
- Removed outdated DRV5056 electrical descriptions from these placed sensors.
- Positioned visible reference/value fields clear of pins and supply wires;
  inspected a 4096-pixel schematic render.

## Verification

- [ERC response](ECAD/reviews/2026-11-04-oh49e/erc.json): **0 errors, 0 warnings,
  0 total violations**, minimum severity `info`, KiCad 10.0.6.
- [Connectivity evidence](ECAD/reviews/2026-11-04-oh49e/connectivity.json): native
  KiCad netlists before/after have identical complete net endpoint sets after
  normalizing the intended sensor pin renumbering. All **46 nets** compared,
  ignoring automatically generated net names/codes. Every sensor has pin 1 to
  +3.3V, pin 2 to GND, pin 3 to its original series resistor; the complete
  comparison also preserves all downstream ADC-channel endpoints.
- A read-only schematic structural comparison confirmed unchanged wires,
  labels, junctions, no-connect markers, symbol UUIDs/placements, hierarchy
  instance references and unrelated components.
- Source hashes are retained in the connectivity evidence. The PCB was not
  synchronized or edited. Existing unrelated project-file changes were preserved.

These checks establish schematic connectivity and identity, not physical sensor
orientation, power/range margin, ADC settling, firmware timing or fabrication readiness.

### Konnect serialization findings — cleanup remains open

Read-only inspection found that the twelve `replace_component` calls embedded
**twelve identical definitions** named `Keyboard12:OH49E-S` in `lib_symbols`.
This is a tool serialization artifact, not twelve different sensor types.
Native KiCad ERC, netlist export and rendering succeeded. A subsequent Konnect
`update_symbols_from_library` dry run reported the symbol unchanged, no moved
pins and no errors, so it did not offer a cleanup plan. Consolidate these
identical embedded definitions using supported Konnect/editor functionality
before final sign-off; do not bypass the source-edit restriction with text edits.
`git diff --check` also reports tab-only blank lines emitted by Konnect in the
schematic. These formatting findings are recorded, not silently fixed by hand.

## Earlier checkpoint: footprint assignment deliberately withheld

At this checkpoint, **all twelve sensor Footprint fields were deliberately empty**, with a hidden
`Footprint Status` property explaining the reason. The existing combined
`Tang:SW_Cherry_MX_PCB_1.00u_hall_effect` uses DRV-style numbers:

| Function | Existing custom pad | Required OH49E-S pad |
| --- | --- | --- |
| Supply | 1 | 1 |
| Ground, isolated land | 3 | 2 |
| Output | 2 | 3 |

Do not assign the old combined footprint unchanged to the native OH49E symbol.
Do not synchronize this transitional schematic to the PCB yet: missing footprint
assignments do not prevent a configured ERC pass and are not a fabrication-ready
state. No production BOM or assembly outputs were generated.

Next create/review a dedicated combined KS-20/OH49E-S footprint through Konnect,
including native numbering, side-aware Fluxpad geometry, switch holes/keepout,
package/assembly orientation and placement origin. Preserve original assets.
Then assign it to U3–U14, repeat ERC/netlist checks and review a PCB-update dry run.
The approved 1.0 mm physical stackup remains a separate pending PCB change.
