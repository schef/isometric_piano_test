# Existing Hall/switch CAD assets — investigation, 2026-11-04

> **Latest owner correction:** use **EasyEDA numbering, 1 VCC / 2 VOUT / 3 GND**,
> superseding the preceding instruction to retain native/PDF numbering. Applied
> consistently to current symbols/footprint without changing physical wiring.
> See `OH49E_EASYEDA_NUMBERING.md`. No stock-footprint substitution or split
> architecture was implemented by this decision; comparisons below retain the
> investigation-time native-symbol context.

## Result and recommendation

**A custom combined electrical/mechanical footprint is not necessary for the
sensor lands.** The installed KiCad **`Package_TO_SOT_SMD:SOT-23`** footprint
has precisely the reference sensor pad sizes and relative geometry, after a
rotation and 0.2 mm origin offset. Reusing that footprint unchanged, with a
separate mechanical switch footprint, is a viable alternative worth choosing
before further custom-library edits.

However, **no untouched combined KS-20 White / native-numbered OH49E-S part
matching the selected Fluxpad assembly was verified**. A symbol/footprint mapping
decision and preservation of switch clearance/keepout remain necessary. This
investigation does not approve changing the architecture or undo prior work.
No project KiCad sources were changed or external library assets imported.

## 1. Stock KiCad sensor footprint — best reuse candidate

Inspected installed `Package_TO_SOT_SMD.pretty/SOT-23.kicad_mod` directly:

| Standard pad | X / Y, mm | Native OH49E-S function/number |
| --- | --- | --- |
| 1 | −0.9375 / −0.95 | VCC / 1 |
| 2 | −0.9375 / 0.95 | VOUT / 3 |
| 3, isolated land | 0.9375 / 0 | GND / 2 |

All pads are 1.475 × 0.6 mm, rounded rectangle ratio 0.25. In canonical F.Cu
coordinates, the mathematical transform **(x, y) → (−y, x + 0.2)** gives exactly
the corrected combined footprint's sensor-land coordinates, with the functional
number mapping above. This is a geometric calculation, **not a PCB Editor
rotation command**; confirm KiCad's angle/side conventions when placing.

Important distinctions:

- The installed **`SOT-23-3`** footprint used by HE60/Drachma is a different land
  pattern: X centres ±1.1375 mm, lands 1.325 × 0.6 mm. Do not confuse its name
  with the exact-match installed `SOT-23` footprint.
- An unchanged standard SOT-23 footprint **cannot be assigned directly to the
  current native-numbered OH49E-S symbol**: standard pad 3 is isolated, while the
  OH49E-S manufacturer drawing labels its isolated ground lead 2.
- Existing online models use 1 VCC / 2 OUT / 3 GND as a **package-pad convention**.
  A matched symbol and standard footprint may be physically consistent despite
  different manufacturer numbering. That convention must be explicit, rather
  than calling it the manufacturer's native pinout.
- Options are a clearly documented footprint-compatible OH49E symbol with
  standard pad numbering, or retention of the native symbol with a remapped
  footprint. Only the former leaves the stock footprint unchanged. No selection
  between these options was implemented during this research.

The official [KiCad library license](https://www.kicad.org/libraries/license/)
is CC-BY-SA 4.0 with a design-use exception. Referencing the installed library is
simpler than redistributing a third-party CAD collection; if vendoring library
files, preserve applicable licensing/attribution. No exact OH49E-S symbol was
found in the inspected installed `Sensor_Magnetic` library.

## 2. HE60 — separate switch mechanics and standard sensor footprint

Inspected [HE60 at commit 501871e03edc54cc17a75ea4d55e129fea370f52](https://github.com/peppapighs/HE60/tree/501871e03edc54cc17a75ea4d55e129fea370f52).

- [MX-1U footprint](https://github.com/peppapighs/HE60/blob/501871e03edc54cc17a75ea4d55e129fea370f52/HE60/lib/HE60.pretty/MX-1U.kicad_mod)
  is **mechanical only**: 19.05 mm pitch outline, switch guidance and two 1.75 mm
  NPTH holes at (±5.08, 0). A numerical comparison confirmed those holes match
  our existing combined footprint exactly. It contains no sensor pads, central
  stem hole or keepout zone. Do not discard the current keepout when splitting.
- The current board places sensor HE1 separately on B.Cu, using
  `Package_TO_SOT_SMD:SOT-23-3`, with MT9102ET as its current value. MX1 is on
  F.Cu. This supports the separate-footprint approach, not exact KS-20/Fluxpad
  stack qualification.
- The saved board is 1.6 mm. Its README mentions GEON Raw HE, Gateron Magnetic
  Jade and Duhuk Core HE, not a verified KS-20 White / 1.0 mm reference assembly.
  Do not copy its sensor offset or board thickness as our mechanical target.
- The [repository license](https://github.com/peppapighs/HE60/blob/501871e03edc54cc17a75ea4d55e129fea370f52/LICENSE)
  is GPLv3. Review reuse/redistribution obligations and retain provenance if
  importing its mechanical footprint. No files were imported into the project.
- README lists OH49E-S among previously used sensors and labels it
  “Discontinued.” This is a community-maintainer statement, not independently
  verified manufacturer lifecycle evidence. Recheck current sourcing before
  committing to production; the approved sensor was not changed by this finding.

## 3. Drachma — useful reference, not a clean drop-in library

Inspected [Drachma at commit 9ca1a2e32ae4d44a6899ddd07bda91ad1de45b1f](https://github.com/coincase1/Drachma/tree/9ca1a2e32ae4d44a6899ddd07bda91ad1de45b1f).

- The schematic embeds a symbol explicitly named `HE60:OH49E-S`, with its
  manufacturer PDF link, **1 VCC / 2 OUT / 3 GND**, and a standard SOT-23-3
  footprint. This is the package-pad convention described above, not the native
  manufacturer pin numbering used by our current symbol.
- Board HE1 uses OH49E-S, B.Cu, isolated pad 3 on GND; MX1 is a separate
  `PCM_HE60:MX-1U` mechanical footprint. The board is 1.6 mm, and does not prove
  the approved KS-20/Fluxpad alignment.
- The inspected tree has no standalone sensor library or bundled mechanical
  `.pretty` library. Dependencies are embedded in the design, rather than a
  ready-to-register self-contained library package.
- No LICENSE file was found in the inspected tree; GitHub's license endpoint
  returned 404. Public source availability is not a verified reuse license.
  Prefer this as reference evidence, not copied library content without permission.

## 4. LCSC / EasyEDA C85573 — real model, not a verified KiCad drop-in

The [C85573 component API](https://easyeda.com/api/products/C85573/components)
returned an actual OH49E-S symbol and linked footprint, not just a search page:

- Symbol UUID `8af0a78208404169a1eca40426be495c`, supplier C85573, owner LCSC.
- Footprint UUID `b3b82869fa924bae820e3a6cfb44d689`, titled
  `SOT-23-3_L2.9-W1.6-P1.90-LS2.8-BR`.
- Symbol pins are **1 VCC / 2 OUT / 3 GND**; the footprint's isolated land is 3.
  Consequently, the model pair must be reviewed together and is not an unchanged
  drop-in for our native-numbered symbol.
- This is EasyEDA data, not a verified KiCad export. Conversion and checking
  would still be required, including pad geometry, 3D alignment and clearances.
- Source: **JLCEDA/EasyEDA Official Library**
  ([JLCEDA](https://lceda.cn/), [EasyEDA](https://easyeda.com/)). The retrieved
  metadata includes copyright/attribution terms and commercial-use language for
  use in EasyEDA. Do not assume unrestricted KiCad-library redistribution rights;
  check the applicable terms before importing or committing converted assets.
- Its LCSC/SZLCSC stock fields disagree. Those CAD-service fields are not a
  current JLCPCB assembly-stock guarantee or manufacturer lifecycle confirmation.

## Suggested next decision

Choose whether to use **separate, unmodified stock sensor and mechanical switch
footprints**, with an explicitly mapped OH49E-S symbol, or finish the current
native-numbered combined asset. The separate approach avoids mixed switch/sensor
labels and gives the sensor its own assembly origin. Both still require the
approved Fluxpad placement, 1.0 mm stackup and keepout/assembly review.

Do not silently undo the current Konnect changes, change sensor choice, copy
another keyboard's 1.6 mm geometry or generate production files from this research.
