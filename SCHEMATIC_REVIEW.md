# Keyboard12 schematic review — 2026-11-02

> **2026-11-04 update:** owner selected KS-20 White, retained 2.5 V / Range 1,
> and chose to copy Fluxpad V2 sensing geometry and thickness. See
> [SWITCH_RANGE_DECISION.md](SWITCH_RANGE_DECISION.md). The range choice is no
> longer pending; the full theoretical sensor-range limitation remains true,
> while prototype coverage and other review items remain unvalidated.

## Scope and evidence

Initial electrical review of the saved single-sheet design:
`ECAD/Keyboard12/Keyboard12/Keyboard12.kicad_sch`.
No KiCad design sources were modified. This is not fabrication sign-off or a
PCB/mechanical audit. Unsaved edits in an editor are outside this review.

- Schematic SHA-256: `e8b00ecfa1351c08c4843460a37df6e674dc542170a73d5a180389a4e77b5e7c`.
- Project SHA-256: `972aae0decac58816907847f316d6b8c26bab97e12346b9a8b696d919bc1ddc9`.
- Tools: Konnect 0.13.0 and KiCad CLI 10.0.6.
- ERC: **0 errors, 0 warnings**, requested at `info` severity. Project has no
  ERC exclusions. This checks configured ERC rules, not component suitability.
- Native KiCad netlist and 4096-pixel schematic render were exported and inspected.
  When Konnect's connectivity query returned null for unnamed nets (including
  GPIO pulls and reference Trim/NR), the native KiCad netlist was used as authority.
- Session artifacts: `/tmp/opencode/Keyboard12-erc.json`, `Keyboard12.net`,
  `Keyboard12-net-summary.json`, and `Keyboard12.png`. These are temporary files,
  not a production package or durable repository evidence bundle.

## 1. Pre-routing blocker: ADC input-range compatibility

**Confirmed design limitation; actual switch clipping has not been measured.**

Native netlist establishes:

| Circuit | Actual connection |
| --- | --- |
| U1 ADS7953 analog supply | Pins 5 and 29 to +3.3V |
| U1 digital supply | Pin 36 to +3.3V |
| U2 REF5025AIDR input | Pin 2 to +3.3V |
| Reference output | U2 pin 6 to U1 pin 4, `VREF_2V5` |
| Reference ground | U1 pin 3 to GND |
| Reference capacitor | C4, 10uF, between VREF_2V5 and GND |
| Sensor supplies | U3–U14 pin 1 to +3.3V |

ADS7953 datasheet SLAS605C, electrical characteristics pp. 12–13:

- Range 1: 0 to VREF, therefore **0–2.5V** here.
- Range 2: 0 to 2 × VREF, explicitly **while 2 × VREF ≤ +VA**.
  A 2.5V reference therefore needs at least 5V analog supply for this range.
- External reference operating range: **2–3V**. Substituting a 1.65V reference
  to obtain a nominal 3.3V doubled range is not a specified solution.

DRV5056 datasheet SBAS644C, magnetic characteristics pp. 5–6:

- A3 quiescent output is nominally 0.6V; it is unipolar, not centered at half supply.
- Sensitivity at 3.3V is **30mV/mT typical**, not the 50mV/mT specified at 5V.
- Linear output extends to **VCC − 0.2V**, approximately **3.1V at 3.3V**.

Consequently, the current circuit cannot guarantee digitizing the full sensor
output range: Range 1 clips above 2.5V and Range 2 is outside the stated supply
condition. Whether the selected switch actually exceeds 2.5V needs mechanical
field/travel evidence or measurements; do not claim measured clipping.

**The REF5025 itself is not disqualified by its 3.3V input.** Its nominal headroom
is 0.8V; REF50 datasheet SBOS410O specifies VOUT + 0.2V minimum input for this
variant. C5 is 1uF input bypass, C6 is 1uF output bypass, and C7 is 1uF from
Trim/NR to ground. C4 also loads the reference, giving 11uF nominal output
capacitance before DC-bias derating. This is within the stated 1–50uF range for
REF50xxAI, subject to actual capacitor selection and transient/layout validation.
C7 increases reference startup time; firmware must wait for settling.

### Historical resolution options — alternatives not selected

The owner subsequently selected the existing REF5025 / Range 1 with KS-20 White
and Fluxpad geometry. The options below preserve the earlier review, not an open
selection request. Prototype range margin and other validation remain outstanding.

1. **2.0V reference, Range 2: nominal 0–4.0V.** Replace the 2.5V reference
   circuit with a qualified 2.0V reference. Supply U1's analog pins 5/29 from an
   additional nominal 5V rail; keep VBD pin 36 and the sensors on 3.3V so SPI
   remains compatible with the Pico. This covers the approximately 3.1V sensor
   ceiling with more USB-supply headroom than a 0–5V range. It still requires
   **an extra power conductor/connector capacity and revised schematic/routing**:
   current chain connectors carry only 3.3V and GND for power. Verify worst-case
   voltage at the ADC against 2 × actual VREF, USB/cable losses, reference
   tolerances, power sequencing and decoupling. No specific 2.0V reference is
   selected. Lower reference voltages have reduced SNR per the ADS datasheet.
2. **Nominal 3.0V reference, Range 1: nominal 0–3.0V.** Retain the single 3.3V
   inter-board supply and replace the reference circuit with a qualified source
   near 3.0V. This does not need the additional 5V rail. It could clip the upper
   **approximately 0.1V** of a sensor that actually reaches 3.1V at a nominal
   3.3V supply; neither that output nor that clipped span is a measured result.
   Actual sensor output, supply variation, reference tolerance and ADC gain/
   offset must be included. 3.0V is the ADC's specified reference upper limit:
   do not select a nominal 3.0V source whose worst-case positive error exceeds
   it. Verify reference input headroom at the lowest local 3.3V supply under
   load, dropout, startup and settling; a higher reference is not automatically
   a drop-in change to U2. This option is conditional on measurements below.
3. **Keep the 2.5V reference and use Range 2 with an adequate analog supply.**
   Supply its analog rail at a suitably regulated voltage that remains at least
   2 × actual VREF but within the ADC's 5.25V maximum operating supply, keeping
   its digital rail and sensors at 3.3V. Nominal USB 5V alone is not a guarantee
   of that minimum. This also needs additional rail distribution, connector
   changes and power-sequencing review. The supply/reference tolerance window
   is tighter than option 1.
4. Select a 12-/16-channel ADC with a specified 3.3V-compatible input range and
   adequate five-sensor-board shared-bus throughput. A supply-referenced/ratiometric
   approach is worth evaluating. MAX11131 fits electrically in principle but is
   not selected: its shown JLCPCB listing is pre-order only and expensive.
5. **Scale each sensor output down to match the existing 0–2.5V Range 1.**
   Keep ADS7953, REF5025 and the single 3.3V supply. Add an analog attenuation
   circuit to each key's signal path, typically a two-resistor voltage divider:

   ```text
   Hall OUT ── Rtop ──┬── ADC channel
                     │
                   Rbottom
                     │
                    GND
   ```

   Ideally, Vadc = Vsensor × Rbottom / (Rtop + Rbottom). For illustration,
   a ratio of 0.70 changes a 3.1V signal to about 2.17V, leaving headroom below
   2.5V. **This is an illustrative ratio, not selected resistor values or a
   completed tolerance design.** Choose the final ratio using worst-case sensor
   output/supply, resistor tolerances, minimum reference voltage and noise margin,
   not simply 2.5/3.1 at nominal conditions.

   This scales the whole waveform rather than intentionally clipping its top;
   voltage differences between key positions also shrink, so ADC counts per
   unit travel decrease. Recalibrate velocity/release thresholds to the scaled
   signal. Ideal constant scaling preserves the timing of corresponding voltage
   crossings, but real filtering/noise/loading can change timing performance.

   The divider loads the sensor and adds source impedance at the ADC. Check
   the Hall output's allowed load and drive capability, ADC leakage and SAR
   acquisition settling. Redesign the **combined divider + existing 100-ohm/
   1uF filter**, accounting for all series resistance and the divider's Thevenin
   resistance; do not just append arbitrary resistors and assume the present
   100us filter time constant remains unchanged. A buffer may be needed if the
   load/settling requirements cannot be met passively, adding power, parts and
   layout complexity. Final resistor/capacitor values and any buffer are pending.

   Tradeoff: additional per-key parts and analog validation, but **no extra
   inter-board power rail and no ADC replacement**. Measure the unscaled sensor
   output first, then validate scaled static range and dynamic threshold timing
   on the actual mechanism before choosing this option.

The approach is now selected; complete the remaining geometry and electrical
checks before routing. Selection does not establish prototype qualification.

### Measurement plan — validate the selected 2.5V range

The approximately 3.1V value is the sensor's nominal linear output ceiling,
**not a prediction that the chosen switch produces 3.1V**. Magnet polarity,
strength, spacing, alignment and travel determine the actual output. First
establish the direction of voltage change when pressing; do not assume the
Fluxpad example's falling thresholds apply unchanged.

Future prototype measurements should use the selected KS-20 White mechanism,
the intended DRV5056A3 placement and a representative 3.3V supply:

1. Measure local sensor supply and OUT-to-GND voltage at rest, through travel
   and at bottom-out. Record voltage against position, including both travel
   directions. Measure multiple sensor/switch pairs and mechanical tolerances.
2. Observe slow and fast strokes on an oscilloscope or suitable acquisition
   instrument, including the ADC-side node after the 100-ohm/1uF filter.
   Record extrema, noise, direction and times across candidate threshold pairs.
   A multimeter can establish static endpoints but cannot qualify velocity timing.
3. Select start/finish thresholds corresponding to two useful travel positions,
   plus a reliable release threshold. Keep all thresholds and their noise/
   supply/temperature/unit-to-unit margins comfortably inside the usable ADC
   range. Validate fast/slow presses, repeated presses and partial release.
4. Verify that the selected 0–2.5V range covers those threshold crossings with margin.
   Clipping outside the timing/release regions can be acceptable for this
   two-threshold velocity method; clipping a required crossing is not. It also
   removes continuous position information in that region, relevant to future
   aftertouch-like behavior even if note velocity remains usable.

These are planned qualification tests, not authorization to build/flash firmware.
The preliminary one-key Fluxpad measurements in `SWITCH_RANGE_DECISION.md` do
not establish multi-unit or worst-case coverage. The owner selected 2.5V / Range 1;
validate useful crossings and margin rather than treating that decision as proof.

### Sourcing update: ADS7953SDBTR / C701670

Owner-provided JLCPCB screenshot shows **ADS7953SDBTR, C701670**, Extended,
13 in stock, US$8.5952 at the displayed 1+ tier on 2026-11-02. Five devices
are approximately US$42.98; six including a spare approximately US$51.57,
before assembly, shipping and tax. Stock is not reserved and must be rechecked.

TI's [ADS7953SDBTR ordering page](https://www.ti.com/product/ADS7953/part-details/ADS7953SDBTR)
explicitly identifies ADS7953SDBT as the **same product in a different carrier**:
SDBT is supplied in tubes, SDBTR in tape-and-reel. Both use the 38-pin DBT TSSOP
package with the same functionality/pinout. **C701670 is the documented sourcing
choice for retaining this ADC**, not a solution to its input-range limitation.
No footprint change is required solely because of the final R suffix, but the
existing custom footprint still needs validation. The saved KiCad source still
says ADS7953SDBT / C701092; this documentation update does not edit those fields.

## 2. Connectivity that checks out at schematic level

- ADS7953 TSSOP pin assignments inspected against its pin-function table.
  U1A/U1B are units of one ADC, not two ADCs.
- U1 MXO pin 7 is joined to AINP pin 8 on `MXO_AINP`; AINM pin 9 is grounded.
- All listed analog/digital ground pins are on GND. Physical plane continuity
  and reference return paths are not established by a schematic.
- Unused CH12–CH15 are grounded; exclude them from the intended 12-key scan.
- C1–C3 provide three nominal 1uF supply bypass capacitors. Their physical
  allocation and proximity to the three supply pins remain layout checks.
- All 12 sensor outputs have a 100-ohm series resistor and 1uF ADC-side filter,
  plus a separate 1uF supply bypass; sensor SOT-23 pin numbering matches TI.
- Channel map: CH0 U3/R1/C8; CH1 U4/R2/C11; CH2 U14/R12/C31;
  CH3 U5/R3/C13; CH4 U6/R4/C15; CH5 U7/R5/C17;
  CH6 U8/R6/C19; CH7 U9/R7/C21; CH8 U10/R8/C23;
  CH9 U11/R9/C25; CH10 U12/R10/C27; CH11 U13/R11/C29.
- R13 pulls ADC GPIO2 down through 10k; R14 pulls GPIO3 up through 10k.
  GPIO0/1 are intentionally unconnected. GPIO alternate-function programming
  still needs review with eventual firmware.
- SDI is MOSI, SDO is MISO, and SCLK is SCLK. ADS7953 SDO is tri-stated as
  documented; shared-bus feasibility still depends on CS timing and wiring.

## 3. Other findings requiring resolution

### Five-sensor-board interconnect: capacity sufficient; other checks remain

All four connector symbols have this identical pinout:

| Pin | Net |
| --- | --- |
| 1 | +3.3V |
| 2 | GND |
| 3 | SCLK |
| 4 | MOSI |
| 5 | MISO |
| 6–10 | CS0–CS4 |

JP1–JP5 connect one of CS0–CS4 to the local `ADC_CS`. There is **no CS5**.
This supports the five required sensor boards. **The earlier missing-sixth-CS
finding is withdrawn following owner clarification:** the sixth physical board
is the Pico 2 master carrier, which does not need a chip-select. There is no
sixth-sensor-board expansion requirement.
J1/J2 use custom SMD chain footprints; J3/J4 use through-hole 1x10 headers.
Population and use of these alternative connectors need to be specified.

Only one selection jumper per board should be bridged; bridging multiple ones
shorts master CS outputs together. With all jumpers open, ADC_CS floats, and
there is no local CS pull-up. Consider a local pull-up for a defined deselected
state during master reset/disconnection, alongside defined master startup behavior.
Cable lengths, ground returns, connector current ratings and SPI integrity are
not verified. Daisy-chain and star wiring are not signal-integrity equivalents.

### Sensor current is higher than the documented worst-case estimate

TI specifies **6mA typical, 10mA maximum per DRV5056** (SBAS644C p. 5):

- One 12-key board: 72mA typical / 120mA maximum, sensors alone.
- Five boards: 360mA typical / **600mA maximum**, sensors alone.

ADCs, references, master and regulator losses add to these figures. The
specification's approximately 0.4A *worst-case* estimate is not supported by this
datasheet. A 1A regulator rating is not proof of an adequate USB input-power
budget. Recalculate with the chosen regulator, USB source/current contract,
cable voltage drop, startup and local bulk capacitance before freezing power.
No local board bulk capacitor separate from the reference capacitor was found;
evaluate distributed supply transient requirements rather than counting C4 as
3.3V bulk capacitance.

### Filter response and ADC acquisition need separate validation

100 ohms × 1uF gives tau = **100us**, fc approximately **1.59kHz** and an ideal
10–90% step rise of approximately **220us**. This is significant beside a 250us
scan interval. It is not automatically wrong, but threshold/velocity timing must
be validated on the actual mechanism. ADC charge redistribution, cross-channel
steps and available acquisition time must also be checked; scan-rate headroom
alone does not prove 12-bit settling.

### Protocol statements in PCB_DESIGN.md need correction

SLAS605C section 8.1 specifies that each frame begins on a **CS falling edge**,
with 16 clocks per conversion. Keeping CS low for a whole 12-channel sweep, as
the specification previously suggested, is not the documented framing method. Channel
selection is pipelined relative to command frames; “zero latency” must not be
interpreted as an immediate response to a new channel command.

Also respect the 325ns acquisition requirement, CS timing and maximum 1MSPS:
20MHz × 16-bit transfer arithmetic alone is insufficient. Five sensor boards × 12 keys
× 4kHz = 240k conversions/s remains below the nominal 1MSPS aggregate ceiling,
but a real transaction schedule and interconnect analysis are outstanding.
Eightfold averaging at that per-key rate still exceeds the shared-bus budget.

### Documentation and sourcing discrepancies

The Markdown sensor specifications, connector description and protocol guidance
were corrected on 2026-11-04. Saved KiCad fields and hardware validation remain
unchanged; the findings below record the original review and outstanding checks.

- U2's symbol Datasheet property points to `ref5030a-q1.pdf`, not the selected
  REF5025AIDR datasheet. Verify the exact MPN and C11341 sourcing association.
- The Hall description/specification quotes 50mV/mT without the 5V qualification;
  its 3.3V sensitivity and unipolar magnetic range need to be stated correctly.
- The current five-CS connector implementation differs from the document's
  tentative eight-conductor recommendation; that recommendation is not a pinout.
- Pico 2 pin mapping, USB circuitry and carrier power are not validated here;
  a separate master-carrier schematic has not been found in this repository.
- Schematic ERC does not qualify the unusual combined sensor/switch footprint,
  sensor alignment, assembly placement origin, or pin-to-pad geometry.

## Current next steps

The MAX11131 has now received a preliminary replacement evaluation in
[MAX11131_EVALUATION.md](MAX11131_EVALUATION.md). Its 3.3V/reference architecture
fits electrically, but owner-provided JLCPCB screenshots show zero stock,
pre-order only, at an estimated US$33.0809 each. It fails the normal-stock
requirement and is not selected; no schematic changes have been made.

The owner has selected ADS7953 / REF5025, Range 1. Next resolve the placed
sensor-footprint discrepancy against Fluxpad and apply approved geometry/thickness
changes through Konnect. Review the five-board interconnect, power budget and
protocol; repeat ERC/netlist review after schematic changes and complete the
remaining placement/routing readiness checks. See `TODO.md`.

## Primary references

- [ADS7953 datasheet](https://www.ti.com/lit/ds/symlink/ads7953.pdf), SLAS605C:
  pin tables pp. 6–7, electrical characteristics pp. 12–13, timing p. 17,
  operation/reference sections 8.1 and 8.3.1, input-path application guidance.
- [DRV5056 datasheet](https://www.ti.com/lit/ds/symlink/drv5056.pdf), SBAS644C:
  pin table p. 3, current and magnetic characteristics pp. 5–6.
- [REF50 family datasheet](https://www.ti.com/lit/gpn/ref50), SBOS410O:
  operating conditions and sections 8.3.6, 8.4.1 and 8.4.2.
