# Switch, geometry and ADC-range decision — 2026-11-04

> **Subsequent owner approval:** select OH49E-S / C85573 for the main board,
> superseding the earlier DRV5056 retention and pending-reconfirmation text below.
> KS-20 White, 1.0 mm and ADS7953 / REF5025 Range 1 remain selected.
> See `OH49E_EVALUATION.md`; no KiCad changes or prototype qualification claimed.

## Owner-approved direction

**Later finding on the same date:** Fluxpad's sourcing fields and assembly
database identify **OH49E-S / C85573**, despite the DRV5056 symbol label.
The following records the earlier decision; retention of DRV5056A3 now needs
owner reconfirmation before sensing changes. No OH49E-S substitution is approved.
The KS-20 White, 1.0 mm target and ADS7953 / REF5025 Range 1 decisions remain
recorded. See [FLUXPAD_SENSOR_IDENTIFICATION.md](FLUXPAD_SENSOR_IDENTIFICATION.md).

- Use **Gateron KS-20 White** magnetic switches instead of the previously
  specified Wooting Lekker V2 L60.
- Retain **DRV5056A3QDBZ**, the existing **ADS7953 / REF5025 2.5 V reference**,
  and **Range 1 (0–2.5 V)**. No extra analog rail, near-3 V reference or per-key
  attenuation is selected.
- Reproduce Fluxpad V2's local switch/sensor geometry: sensor position,
  orientation and PCB side, switch mounting height, magnet alignment and travel.
- Copy Fluxpad V2's PCB thickness. The inspected upstream `fluxpad-2` board
  declares **1.0 mm**, whereas the saved Keyboard12 PCB declares **1.6 mm**.
  The owner also physically measured the Fluxpad PCB as **1.0 mm** on
  2026-11-04, confirming the source-design target (measurement method and
  uncertainty not recorded). Confirm fabrication requirements before implementing;
  do not substitute a default thickness.

This is a design selection, not prototype qualification or permission to release
fabrication outputs. **No KiCad source has been changed for this decision.**

## Measurement evidence and limits

The owner measured a physical Fluxpad V2 RevC, purchased 2023-11-26, using an
Analog Discovery 3. Supply measurement was approximately **3.3068 V**, consistent
with the multimeter. The middle analog key showed approximately **1.48 V released**
and **0.87 V fully pressed**, probing the raw sensor-output side of R14.
The owner confirmed that noisy near-zero sections were probe movement.
No output above 2.5 V was reported in the preliminary measurements; this is not a
certified maximum across units, travel, temperature or mechanical tolerances.

In a subsequent test at the same raw R14 point, lifting the switch approximately
1 mm away from the sensor gave **1.56 V released / 1.17 V pressed**; with the
switch fully removed the output was **1.63 V**. The marking **1320 / 152D**,
half-supply baseline and upstream sourcing evidence strongly support OH49E-S.
These readings must not be attributed to DRV5056A3 or used to qualify it.

## Comparison findings

Both saved schematics use a DRV5056A3 symbol label, 3.3 V sensing, 100-ohm
series filtering and nominal 1 uF ADC-side capacitors. However, Fluxpad's
`JLC PN = C85573` identifies OH49E-S, so matching symbol names do not establish
matching sensors. Fluxpad uses the RP2040 ADC; Keyboard12 uses ADS7953.
Neither static reference-unit readings nor common pin roles prove electrical
equivalence or ADS7953 input settling.

The local footprint library matches the upstream switch/sensor pad geometry,
but the sensor pad positions relative to the switch centre in the saved
Keyboard12 PCB differ from upstream. Both inspected board footprints are on
B.Cu. Resolve the actual geometry, pin mapping and assembly orientation through
Konnect before routing; identical footprint names are not sufficient evidence.

The current Fluxpad V2 product specification identifies **Gateron KS-20**.
The GitHub README's Lekker description refers to an older two-analog-key,
SAMD21-based model. The physical white switches/Gateron marking support the
KS-20 identification, but the exact historical production variant is not
independently certified. Current KS-20 White specifications do not establish
that all historical units have identical magnets.

## Remaining validation

1. Reconfirm the main-board sensor with the owner; explicitly evaluate OH49E-S
   if reproducing the physical Fluxpad response is the goal. Recheck sourcing,
   polarity, range, power and package geometry for the approved part.
2. Resolve the footprint discrepancy and verify the complete mechanical stack
   against the reference unit/design. PCB thickness is confirmed by the owner's
   1.0 mm measurement; apply that target through Konnect when implementing.
3. Check several KS-20/sensor pairs on a prototype over full travel. Establish
   range margin including supply, component and mechanical variation; per-key
   calibration cannot recover a clipped signal.
4. Validate ADC settling, RC response and eventual velocity thresholds separately.
5. Continue the interconnect, power, protocol, library and ERC/DRC reviews.
   The range strategy is selected; these other blockers are not cleared.

## Sources

- [Official Gateron KS-20 White engineering specification (PDF)](https://www.gateron.com/u_file/2311/10/file/GATERONMagneticWhiteSwitch-KS-20TF10B045NW-X72.pdf):
  KS-20TF10B045NW-X72; page 5 includes the mechanical cross-section and drawing
  dated 2023-09-13, version 4. It specifies 4.1 ±0.2 mm travel, north-pole
  magnetic flux density of 102 ±15 G initially and 905 ±80 G at full travel.
  It does not provide a complete magnet material/grade specification or a
  field-versus-distance curve. Reconcile the specified magnetic direction and
  sensor geometry with the observed falling voltage before extrapolating to
  different PCB thicknesses; no quantitative thickness-to-voltage prediction
  has been validated.
- Fluxpad V2 specification: https://fluxpad.xyz/pages/features-and-details
- Reference schematic/PCB: https://github.com/sssata/fluxpad/tree/fluxpad-2/ECAD/FluxpadKicad
  (inspected branch commit `8b2e752136033a81b9da01566e2e36ccafb966d6`).
- Gateron KS-20 White: https://www.gateron.com/products/gateron-ks-20-magnetic-white-switch-set
- DRV5056: https://www.ti.com/lit/ds/symlink/drv5056.pdf
