# OH49E-S evaluation — 2026-11-04

> **Subsequent implementation:** schematic U3–U14 have now been migrated to
> native-pinout OH49E-S symbols through Konnect, and the remapped combined
> footprint is assigned. PCB remains unchanged. See `OH49E_FOOTPRINT_MIGRATION.md`
> for current evidence and remaining gates. Evaluation-time statements below are historical.

## Conclusion

**Owner approved OH49E-S / C85573 as the main-board sensor on 2026-11-04**,
after reviewing the preliminary findings. This supersedes the earlier
DRV5056A3 retention decision and pending sensor-reconfirmation gate. Retain
ADS7953 / REF5025 Range 1 as selected. Selection is not prototype qualification
or fabrication approval. No KiCad sources were changed to record this decision.

## Manufacturer and sourcing evidence

- [Manufacturer datasheet](https://ohhallsensor.com/wp-content/uploads/2020/08/OH49E-OH49E-S-Hall-effect-linear-IC.pdf),
  sections 4, 7–9, inspected directly, including the package drawing.
- [LCSC C85573](https://www.lcsc.com/product-detail/C85573.html) identifies Nanjing
  Ouzhuo Tech OH49E-S, SOT-23.
- [JLCPCB C85573](https://jlcpcb.com/partdetail/Nanjing_OuzhuoTech-OH49ES/C85573)
  identifies the same device as Extended, SMT assembly, Economic and Standard
  PCBA. The fetched page did not establish orderable quantity or price. Its
  generic pre-order instructions are not evidence of either zero or positive stock.
  Confirm at least 60 sensors plus the chosen spare allowance in the logged-in
  BOM tool before release. No stock or price is claimed here.

## Electrical assessment

| Item | Evidence and implication |
| --- | --- |
| 3.3 V operation | Datasheet section 7 lists 2.3–10 V supply; catalogs list 2.5–10 V. Both include 3.3 V. Most electrical limits are specified at 25°C, 5 V, not 3.3 V. |
| Direction | Section 8 describes approximately half-supply zero-field output; north approaching the marked face lowers output. This is consistent with the owner's reference readings, not proof of the new assembly's field orientation. |
| Selected ADC range | Reference-unit readings of 1.48 V released / 0.87 V pressed fit 0–2.5 V. Their 0.61 V span is approximately 999 ideal ADS7953 codes at a 2.5 V reference. This is arithmetic on one measured reference, not a guaranteed prototype span. |
| Full possible range | Bipolar sensing can also raise output. The selected range does not guarantee coverage of every possible field/output; verify the actual assembly over travel and variation. |
| Supply current | 4.2 mA is **typical at 5 V**, with no maximum supplied. Multiplication gives 50.4 mA per 12 sensors / 252 mA per 60 sensors at that stated typical condition only. Neither is a worst-case or measured 3.3 V system budget. |
| Sensitivity | 1.8 / 2.0 / 2.2 mV/G min/typ/max at 5 V, B = ±100 G. Do not apply these unchanged at 3.3 V or extrapolate a full-travel voltage from the switch's field specification. |
| Output drive | Section 7 lists output resistance 40 Ω typical / 100 Ω maximum at its stated test condition, but gives no complete 3.3 V capacitive-drive/settling specification. |
| Noise and dynamics | Listed noise is 90 µV typical over 10 Hz–10 kHz at the table's test condition. This is not a guaranteed response bandwidth, startup time or dynamic qualification at 3.3 V. |

Keep the existing filter under review rather than removing it based on the
datasheet's low-noise marketing statement. The external 100 Ω / 1 µF has a
100 µs nominal time constant; sensor output impedance adds to the effective
source impedance. Validate sensor drive, ADC acquisition, alternating-channel
settling and dynamic threshold timing separately. No new filter values selected.

## Important pin-number distinction

The manufacturer's OH49E-S drawing labels **1 VCC, 2 GND, 3 VOUT**.
TI's [DRV5056 datasheet](https://www.ti.com/lit/ds/symlink/drv5056.pdf), SOT-23
pin table, instead labels **1 VCC, 2 OUT, 3 GND**. These are not identical
manufacturer pin numbers. Do not copy an OH49E-S symbol onto the existing
footprint without an explicit mapping review.

The saved custom combined footprint uses DRV-style pad numbers: pad 1 is supply,
pad 2 is output and pad 3 is ground. Both the reference PCB and Keyboard12 have
this net assignment. In the OH49E-S drawing the isolated lead is GND; in the
custom footprint the isolated land is pad 3 / GND. This supports a physical
mapping despite different numbering, but **does not establish final assembly
orientation**. Resolve all three lead-to-land positions, marked-face orientation,
symbol numbering and pick-and-place rotation before implementation. A standard
OH49E-S symbol with unchanged custom pad numbers would exchange ground/output.

The OH49E-S drawing gives approximately 2.82–3.02 mm body length, 1.50–1.70 mm
body width, 2.65–2.95 mm overall lead span and 1.445 mm maximum overall height.
Review actual land overlap, solder allowance and switch clearance, not just the
SOT-23 name. Hall active-point location and assembled magnet clearance are not
qualified by these package dimensions.

## Saved footprint comparison

Konnect's live pad query failed with `GetOpenDocuments (AS_UNHANDLED)`. The
following is a **read-only saved-file comparison**, excluding unsaved editor work:

- Keyboard12: `ECAD/Keyboard12/Keyboard12/Keyboard12.kicad_pcb`, all U3–U14.
- Local library:
  `ECAD/Keyboard12/Keyboard12/libraries/Fluxpad/Fluxpad.pretty/SW_Cherry_MX_PCB_1.00u_hall_effect.kicad_mod`.
- [Fluxpad board, pinned commit](https://github.com/sssata/fluxpad/blob/8b2e752136033a81b9da01566e2e36ccafb966d6/ECAD/FluxpadKicad/Fluxpad.kicad_pcb),
  U2/U3/U5.

Coordinates below are **serialized footprint-local millimetres**, not a bottom
view or transformed board-space coordinates. All compared placed footprints
are B.Cu at 180°. The local library declares F.Cu, so matching numeric library
coordinates alone does not establish how KiCad will transform a library refresh.

| Pad / net role | Keyboard12 placed (x, y) | Fluxpad placed (x, y) | Local library (x, y) |
| --- | --- | --- | --- |
| 1 / supply | (0.95, −0.7375) | (0.95, 0.7375) | (0.95, 0.7375) |
| 2 / output | (−0.95, −0.7375) | (−0.95, 0.7375) | (−0.95, 0.7375) |
| 3 / ground | (0, 1.1375) | (0, −1.1375) | (0, −1.1375) |

All sensor lands have serialized size 1.475 × 0.6 mm and angle 270°. Both
mounting holes remain at (±5.08, 0), diameter 1.75 mm. All twelve Keyboard12
instances share the same differing sensor arrangement. This is a reflection of
the sensor-land pattern in local Y, not simply a uniform translation; do not
apply a guessed offset or blindly refresh from the library. A live, side-aware
comparison and assembly/pin mapping are still required.

## Next gates

1. **Complete:** owner selected OH49E-S / C85573; qualification work remains open.
2. Restore reliable Konnect inspection, reconcile saved/live state and plan the
   complete symbol-to-physical-lead mapping and geometry correction.
3. Apply approved changes through Konnect, including 1.0 mm thickness; repeat
   connectivity/ERC and inspect footprint orientation and assembly origins.
4. Establish a conservative system power budget and verify sourcing. Test
   multiple KS-20/sensor pairs, static range and filtered dynamics on the intended
   assembly. Fabrication release remains separately gated by owner approval.
