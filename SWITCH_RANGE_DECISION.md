# Switch, geometry and ADC-range decision — 2026-11-04

## Owner-approved direction

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
No output above 2.5 V was observed in the inspected video; this is not a
certified maximum across units, travel, temperature or mechanical tolerances.

Local session evidence:

- `/home/bebox/Videos/2026-11-03 14-53-40.mp4`: useful screen recording.
- `/home/bebox/Videos/fluxpad_keypress_values.csv`: only one second of near-zero
  disconnected-probe noise; **not valid keypress waveform data**.

These files are outside the repository and are not a durable evidence bundle.

## Comparison findings

Both saved schematics specify DRV5056A3, 3.3 V sensing, 100-ohm series filtering
and nominal 1 uF ADC-side capacitors. Fluxpad uses the RP2040 ADC; Keyboard12
uses ADS7953. Equivalent static voltage does not prove ADS7953 input settling.

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

1. Resolve the footprint discrepancy and verify the complete mechanical stack
   against the reference unit/design. PCB thickness is confirmed by the owner's
   1.0 mm measurement; apply that target through Konnect when implementing.
2. Check several KS-20/sensor pairs on a prototype over full travel. Establish
   range margin including supply, component and mechanical variation; per-key
   calibration cannot recover a clipped signal.
3. Validate ADC settling, RC response and eventual velocity thresholds separately.
4. Continue the interconnect, power, protocol, library and ERC/DRC reviews.
   The range strategy is selected; these other blockers are not cleared.

## Sources

- Fluxpad V2 specification: https://fluxpad.xyz/pages/features-and-details
- Reference schematic/PCB: https://github.com/sssata/fluxpad/tree/fluxpad-2/ECAD/FluxpadKicad
  (inspected branch commit `8b2e752136033a81b9da01566e2e36ccafb966d6`).
- Gateron KS-20 White: https://www.gateron.com/products/gateron-ks-20-magnetic-white-switch-set
- DRV5056: https://www.ti.com/lit/ds/symlink/drv5056.pdf
