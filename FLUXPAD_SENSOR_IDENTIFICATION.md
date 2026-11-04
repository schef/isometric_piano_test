# Fluxpad V2 fitted Hall sensor investigation — 2026-11-04

## Finding

**Fluxpad V2's schematic label and assembly identity disagree.** The symbol is
named `DRV5056A3QDBZx Lekker`, but its `JLC PN` field is **C85573**, which LCSC
identifies as **Nanjing Ouzhuo OH49E-S**, SOT-23. The repository's JLCPCB assembly
database independently repeats the same association. The catalog photograph has
the **1320** marking observed on the owner's board.

This strongly identifies OH49E-S as the reference unit's sensor and supports
reuse of a DRV5056-labelled symbol/footprint with a different assembly part.
There is no inspected production order or explicit designer statement proving
the exact substitution history of the owner's individual unit. `152D` remains
undecoded; do not assert its date/lot meaning.

**Correction to earlier project documentation:** C85573 is not a verified
DRV5056A3 sourcing code. The owner's measured Fluxpad voltages must not be
attributed to DRV5056A3 or used to qualify that sensor in Keyboard12. The prior
decision to retain DRV5056A3 is recorded history, not approval to substitute
OH49E-S now. Reconfirm the intended sensor before implementing sensing changes.
No KiCad sources were modified during this investigation.

## Repository evidence

Inspected branch `fluxpad-2`, commit
`8b2e752136033a81b9da01566e2e36ccafb966d6`:

- [Schematic](https://github.com/sssata/fluxpad/blob/8b2e752136033a81b9da01566e2e36ccafb966d6/ECAD/FluxpadKicad/Fluxpad.kicad_sch):
  U5 block lines 4792–4821, U2 block lines 5872–5901, and U3 block lines
  6013–6042 use the DRV5056-labelled symbol but specify `JLC PN = C85573`.
- [Assembly database](https://github.com/sssata/fluxpad/blob/8b2e752136033a81b9da01566e2e36ccafb966d6/ECAD/FluxpadKicad/jlcpcb/project.db):
  read-only SQLite query of `part_info` gives U2, U3 and U5 with value
  `DRV5056A3QDBZx Lekker`, footprint `SW_Cherry_MX_PCB_1.00u_hall_effect`,
  and `lcsc = C85573`.
- The same schematic-field discrepancy is present in the initial Fluxpad 2
  commit `e0b1452` (2023-07-30) and hardware update `fa0e326` (2023-09-05).
  It was not merely inferred from a current reseller listing.
- The older `ProductionPackages/FLUXPAD_REV3_PCBWAY_BOM.xlsx` instead lists two
  sensors U2/U3 as DRV5056Z3QDBZR, with DRV5056Z3QDBZT also allowed. This is
  an older two-sensor BOM, not proof of the later three-analog-key V2 population.
- Firmware commit [a42493e](https://github.com/sssata/fluxpad/commit/a42493e1adc66f7afbbb411f3bea7d2637cb65f7),
  "update LUT for fluxpad 2" (2023-09-24), reverses the ADC/distance relationship
  to falling readings on approach. This supports a response change but does not
  identify the sensor by itself; calibration constants are not raw measurements.
- Searched relevant schematic/firmware history and public issue/PR titles and
  bodies; no explicit OH49E substitution explanation was found.

## Catalog and manufacturer evidence

- [LCSC C85573](https://www.lcsc.com/product-detail/C85573.html): manufacturer
  Nanjing Ouzhuo Tech, MPN OH49E-S, SOT-23.
- [LCSC product photograph](https://assets.lcsc.com/images/lcsc/900x900/20250904_Nanjing-Ouzhuo-Tech-OH49E-S_C85573_front.jpg):
  top line **1320**, second line **409D**. The second line differs from the
  owner's **152D**, but the top marking and package match.
- [Manufacturer OH49E/OH49E-S page](https://ohhallsensor.com/product/oh49e-oh49e-s-series-hall-effect-linear-ic-2/)
  and [official datasheet](https://ohhallsensor.com/wp-content/uploads/2020/08/OH49E-OH49E-S-Hall-effect-linear-IC.pdf):
  section 4 identifies OH49E-S as SOT-23; section 8 describes approximately
  half-supply output at zero field and decreasing output for a north pole
  approaching the marked face. Section 9 gives pin 1 VCC, pin 2 GND, pin 3 VOUT.
  No `1320`/`152D` decoding table was found in this datasheet.
- [TI DRV5056 datasheet](https://www.ti.com/lit/ds/symlink/drv5056.pdf):
  package-option table specifies **56A3** for DRV5056A3 SOT-23; nominal no-field
  output is 0.6 V. This differs from the physical unit's marking and baseline.

These parts have the same nominal three-pin supply/ground/output roles, making
a pad-compatible substitution plausible. Matching pins does not establish
identical package clearances, sensitivity, polarity response, bandwidth, noise,
current, acquisition behaviour or calibrated thresholds.

## Owner's additional measurements

Owner confirms measurement at the previously designated R14 point, on the raw
sensor-output side, and vertical lifting of the switch away from the PCB sensor:

| Condition | Released | Fully pressed | Swing |
| --- | --- | --- | --- |
| Normal mounting, earlier measurement | approximately 1.48 V | approximately 0.87 V | 0.61 V |
| Switch lifted approximately 1 mm | 1.56 V | 1.17 V | 0.39 V |
| Switch fully removed | 1.63 V | — | — |

Earlier measured supply was approximately 3.3068 V; simultaneous supply for the
new experiment and displacement uncertainty were not recorded. A 1.63 V
no-switch baseline is consistent with a half-supply sensor. Nearby magnetic
fields and offset are not separately characterized. These are preliminary
single-unit results, not worst-case or dynamic velocity qualification.

## Next gate

Reconfirm sensor selection with the owner. If reproducing the physical Fluxpad
response is the goal, evaluate OH49E-S / C85573 explicitly instead of assuming
DRV5056A3 is equivalent. Keep the ADS7953 / REF5025 Range 1 decision distinct:
the measured reference-unit voltages fit 0–2.5 V, but replacement/prototype
variation, settling and dynamic thresholds still require validation.
