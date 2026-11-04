# PCB Design — 60-Key Velocity-Sensitive MIDI Keyboard (Winner: SPI Bus / Design B)

> **2026-11-04 owner decision:** use **Gateron KS-20 White**, retain the existing
> **REF5025 / ADS7953 Range 1 (0–2.5 V)**, and copy Fluxpad V2 sensing geometry
> and PCB thickness (upstream **1.0 mm**; saved Keyboard12 is **1.6 mm**).
> See [SWITCH_RANGE_DECISION.md](SWITCH_RANGE_DECISION.md). Prototype validation
> and other electrical/mechanical reviews remain open; no KiCad changes applied.

> **Later sensor finding:** Fluxpad's assembly identity is **OH49E-S / C85573**,
> despite its DRV5056-labelled symbols. Saved Keyboard12 specifies DRV5056A3;
> owner reconfirmation of the sensor is required before sensing changes. No
> substitution is approved. See [FLUXPAD_SENSOR_IDENTIFICATION.md](FLUXPAD_SENSOR_IDENTIFICATION.md).

Current switch reference: [Gateron KS-20 White engineering specification (PDF)](https://www.gateron.com/u_file/2311/10/file/GATERONMagneticWhiteSwitch-KS-20TF10B045NW-X72.pdf),
part KS-20TF10B045NW-X72; see page 5 for the mechanical cross-section and magnetic
specifications. This reference does not establish an exact voltage-versus-travel
curve for the chosen PCB/sensor assembly.

This document specifies the winning hardware architecture for a scalable,
velocity-sensitive MIDI piano keyboard. It is the authoritative reference for
any agent implementing the firmware or schematic of this board, and inherits
the velocity / USB-MIDI approach from the Fluxpad example at
`examples/fluxpad_midi/src/main.c`. The 60-key controller is the main project;
the three-key example is reference firmware, not an implementation of this board.

## 1. Architecture summary

- **5 × identical 12-key "key boards"** — no MCU, only sensors + one SPI ADC.
- **1 × master carrier board** — a Raspberry Pi Pico 2 module ("one brain"), dedicated
  3.3 V power, and one firmware.
- **Interconnect: one shared SPI bus.** Each key board is one SPI slave with its
  own chip-select (CS). All boards share SCLK/MOSI/MISO; the master drives one
  CS per board.
- **JLC only assembles the 5 key boards.** The user hand-assembles the master
  carrier board and plugs in the Pico 2 module (with 5 carrier PCBs as spares).

This is **Design B** (the winner) because it scales well beyond 60 keys (add
boards + CS lines) and digitizes locally instead of carrying analog signals
between boards. Reads share one SPI bus; timing and cable integrity remain to
be validated, not assumed from the architecture.

## 2. Why this design (decision rationale)

| Criterion                | Design A (analog MUX)       | Design B (SPI ADC/board)  |
|--------------------------|-----------------------------|----------------------------|
| Max keys                 | Depends on MUX and scan budget | Add boards + CS, subject to bus/power limits |
| Full 60-key sweep        | Historical estimate: ~1.2 ms | Target: ≤250 µs on shared SPI; unvalidated |
| Velocity timing granularity | Depends on scan schedule | Provisional 4 kHz/key target; unvalidated |
| Scalability              | Poor                        | Good                       |
| Distance/noise immunity  | Poor (analog over wire)     | Good (digital at source)  |

The earlier analog-MUX comparison was a design estimate, not a measured limit
or a universal 64-key cap. The selected SPI design digitizes at each board;
the actual scan schedule and interconnect still require validation.

## 3. Key board (JLC-assembled, qty 5)

### 3.1 Block diagram

```
12 × linear Hall sensors (saved: DRV5056A3; selection to reconfirm)
   │  analog outputs (VCC/3.3, GND, VOUT)
   ▼
ADS7953 (12-bit, 16-channel, 1-MSPS, SPI ADC)   ← 1 chip per board
   │
   ▼
SPI: SCLK, MOSI(SDI), MISO(SDO), CS#n          → shared bus to master
```

### 3.2 ADC selection (THE WINNER PART)

- Owner-selected ADC family: **ADS7953**, 12-bit,
   **16-channel**, 1-MSPS, single-ended SPI, 38-pin TSSOP.
   **Updated sourcing choice: ADS7953SDBTR / JLCPCB C701670.** TI confirms it is
   the same device/package/pinout as ADS7953SDBT, supplied in tape-and-reel rather
   than tubes. The current KiCad fields still say ADS7953SDBT / C701092 and have
   not been edited. Validate the existing footprint and update fields through
   Konnect after verifying identities and approval for the field updates.
   - Its **current schematic** external reference is **REF5025AIDR** (SOIC-8,
     recorded JLC/LCSC **C11341**, exact sourcing still to verify), producing
      2.5 V. The owner selected this reference with Range 1 (0–2.5 V);
      prototype range margin and settling remain unvalidated. Follow SLAS605C:
      place a 10 uF ceramic directly between
    ADS7953 REFP and REFM, short MXO to AINP for the unbuffered path, and tie
    AINM to the analog-ground plane.
  - Use channels 0–11 for the 12 keys. Exclude channels 12–15 from the
    sequencer; tie them to ground if the final configuration requires them to
    be connected.
  - The 38-pin TSSOP is preferred over VQFN for visual inspection and rework.
   - 20 MHz serial interface; up to 1 MSPS per ADC under specified conditions.
     Each 12-key board requires **48 kSPS** at 4 kHz/key. The headline ADC rate
     does not establish available per-key throughput on the shared bus.
  - Five boards require approximately 3.84 Mbit/s of 16-bit SPI transfers at
     a 4 kHz scan rate, before pipeline/CS/acquisition and other overhead.
     Eightfold averaging requires at least 30.72 Mbit/s and does not fit a
     single 20 MHz shared bus; see `HANDOVER.md` for the system budget.
   - Owner-provided JLCPCB screenshot on 2026-11-02 shows C701670 with **13 in
     stock at US$8.5952 each (1+ tier)**, Extended. Six for five boards plus a
     spare would be approximately US$51.57 at that tier. Recheck stock, quantity
     pricing and assembly eligibility before ordering; no stock is reserved.

#### Historical reference/range alternatives — not selected

The current selection is the existing 2.5 V reference / Range 1 without
attenuation. The alternatives below are retained for context, not as open choices.

See [SCHEMATIC_REVIEW.md](SCHEMATIC_REVIEW.md) for datasheet evidence and tests.

| Approach | Nominal ADC input range | Wiring/circuit impact | Open condition |
| --- | --- | --- | --- |
| 2.0 V reference, Range 2 | 0–4.0 V | New reference plus additional nominal 5 V analog rail to every ADC; sensors/SPI remain 3.3 V; revise connector power wiring | Verify minimum analog supply ≥ 2 × actual VREF, USB/cable losses and all tolerances |
| Near-3.0 V reference, Range 1 | About 0–3.0 V | Keep single 3.3 V interconnect rail; change reference circuit | Measure useful key-travel voltages; permit clipping only outside needed velocity/release crossings; reference must not exceed ADC's 3.0 V maximum including tolerance |
| Existing 2.5 V reference, Range 2 | 0–5.0 V | Add analog rail and connector wiring | Raw USB 5 V does not guarantee the required minimum; tighter supply/reference margin |
| Existing 2.5 V reference, Range 1 | 0–2.5 V | No range-related hardware change if measurements prove adequate | Actual threshold crossings and variation must fit below full scale |
| Attenuate each Hall output with a voltage divider (buffer only if needed), existing 2.5 V reference / Range 1 | Sensor waveform scaled into 0–2.5 V | Keep ADC and single 3.3 V rail; add/rework per-key resistors and RC filter; recalibrate thresholds | Size for worst-case output/reference/tolerances; verify sensor loading, ADC settling, noise and velocity timing; values not selected |
| Different 12-/16-channel ADC | Depends on selected circuit | ADC/footprint and support-circuit redesign | Qualify electrical fit, settling, timing and normal JLCPCB stock/cost |

For the saved DRV5056 design, approximately 3.1 V is the linear output ceiling at nominal 3.3 V,
not a measured switch output. A 3.0 V measuring range might lose approximately
0.1 V near that ceiling, but the actual loss may differ or be absent. For the
two-threshold piano-velocity method, clipping can be acceptable outside the two
timing thresholds and release region. First measure the actual selected
switch/sensor geometry over travel, multiple units and representative operating
conditions, including dynamic response after the RC filter. Validate that the
selected 2.5 V range covers the useful crossings with sufficient margin.
Reference dropout/startup/noise and supply tolerances remain design checks.
The attenuation option reduces sensor voltage before it reaches the ADC, rather
than changing sensor power or relying on clipping. For example only, a 0.70
divider ratio maps 3.1 V to about 2.17 V. It preserves more of the travel waveform
than clipping, but reduces ADC counts per unit movement and changes sensor load
and filter behavior. No divider values or buffer are selected; see the review
for the circuit concept and required calculations/measurements.
MAX11131 was evaluated but is not selected: the shown MAX11131AUI+T / C7457799
listing is pre-order only, zero stock, estimated US$33.0809 each.

### 3.3 Sensor — saved DRV5056A3QDBZ; selection to reconfirm

- Saved Keyboard12 specifies TI **DRV5056A3QDBZ**. The earlier retention decision
  assumed this was the physical Fluxpad sensor; that assumption is contradicted
  by Fluxpad's sourcing fields and the reference-unit evidence. **Reconfirm the
  sensor with the owner before implementation.** No substitution is approved.
- DRV5056A3 characteristics: unipolar output, nominal 0.6 V quiescent voltage; typical sensitivity is
  **30 mV/mT at 3.3 V** (50 mV/mT at 5 V), not a bipolar ±79 mT range.
  Linear output extends to approximately VCC − 0.2 V; 20 kHz bandwidth,
  SOT-23 package. See `SCHEMATIC_REVIEW.md` for datasheet references.
- One per key: 3 pins (VCC, GND, VOUT) → VOUT through the RC filter to an ADC channel.
- Reuse Fluxpad's `Tang:SW_Cherry_MX_PCB_1.00u_hall_effect` footprint and its
  sensor placement relative to the Cherry-MX-style key only after resolving the
  placed-footprint discrepancy and validating the approved sensor package.
- **C85573 is OH49E-S, not DRV5056A3.** Fluxpad's schematic and JLCPCB database
  assign this code to the DRV5056-labelled symbols. LCSC's OH49E-S photo has
  marking 1320; its half-supply baseline agrees with the measured 1.63 V with
  the switch removed. Do not transfer the measured 1.48–0.87 V response to TI's
  sensor. See `FLUXPAD_SENSOR_IDENTIFICATION.md` for sources and limits.
- If OH49E-S is approved later, verify its sourcing, package/pin geometry,
  current, polarity, output range, noise and ADC settling explicitly. Common
  nominal pin roles do not make its behaviour equivalent to DRV5056A3.
- Small neodymium magnet on each key stem; sensor fixed under the key.
- Wiring is short and on-board; an unbuffered path is intended, but ADC
  acquisition settling and sensor/filter loading still need validation.

### 3.4 Per-key analog circuit (four parts)

The saved Keyboard12 circuit follows the non-LED portion of Fluxpad's circuit.
Review the sensor-dependent loading/settling after sensor reconfirmation:

1. Hall sensor: saved DRV5056A3QDBZ; final selection to reconfirm.
2. 1 µF capacitor from the sensor's 3.3 V supply to ground.
3. 100 Ω series resistor between the sensor output and the ADS7953 channel.
4. 1 µF capacitor from the ADC-side of that resistor to ground.

Do **not** include Fluxpad's per-key LED components: the 22 Ω LED resistor
(`R4` for Fluxpad key 4) and white counterpost LED (`D4`) are unrelated to the
Hall signal path. This design has no per-key LEDs, avoiding their power,
routing, cost, and firmware requirements.

The 100 Ω + 1 µF output filter intentionally smooths the Hall signal. Neither
headline ADC speed nor the reference-unit readings qualify settling; validate this filter's effect on velocity timing
with the completed 12-key prototype before finalizing its values.

### 3.5 Key mechanism

- Use **Gateron KS-20 White** magnetic switches for all 60 analog keys,
  with Cherry-MX-compatible stems. Follow the official engineering specification
  and reproduce Fluxpad V2 local sensor geometry with a **1.0 mm PCB**; the
  saved Keyboard12 PCB is still 1.6 mm and its placed sensor geometry differs.
- Use **DSA-profile, Cherry-MX-stem keycaps**. The Fluxpad source includes the
  [reference CAD model](https://github.com/sssata/fluxpad/blob/8b2e752136033a81b9da01566e2e36ccafb966d6/CAD/DSA_Keycap_CherryMX.step).
- Use the switch's built-in magnet; verify its alignment, polarity and clearance
  against the approved sensor package and the physical Fluxpad reference.
- Fluxpad's Gateron Red switches and Kailh MX hot-swap sockets are for its
  separate digital keys and are not part of this all-analog keyboard design.

### 3.6 Interconnect / connectors

- Each board needs 2 inter-board connectors (daisy-chain in/out) OR 1 (star) —
  see section 6. Signals per board:
  - VCC (3.3 V), GND
  - SPI: SCLK, MOSI, MISO (shared)
  - CS#n (unique per board)
- The saved schematic uses 10 conductors: 3.3 V, GND, SCLK, MOSI, MISO and
  CS0–CS4. J1/J2 are custom SMD chain connectors; J3/J4 are alternative 1×10
  headers. Final connector type, population, current rating and cable layout
  remain open; an eight-conductor shared-chain recommendation is obsolete.

## 4. Master carrier board (hand-assembled by user, qty 1 used)

### 4.1 Block diagram

```
USB-C receptacle ──► carrier USB circuit ──► Pico 2 USB test pads ──► SPI controller
                                                                     │
                                                                     ├── CS0 ──► key board 0
                                                                     ├── CS1 ──► key board 1
                                                                     ├── CS2 ──► key board 2
                                                                     ├── CS3 ──► key board 3
                                                                     └── CS4 ──► key board 4
                                                                     └── SCLK / MOSI / MISO (shared bus)
```

Use a **Raspberry Pi Pico 2** module. This first revision deliberately avoids
a bare RP2350: its USB, QSPI flash, crystal, and core power circuitry are
already proven, making bring-up, hand assembly, and module replacement lower
risk. A later compact production revision may integrate a bare RP2350 after
the key boards, calibration, SPI scan timing, and mechanical layout are proven.

### 4.2 Pico 2 pin map

**Proposed hardware SPI0 mapping, not implemented or validated on a carrier.**
The earlier GPIO0=SCLK / GPIO1=MOSI / GPIO2=MISO assignment was incorrect.
Verify this proposal against the Pico 2 / RP2350 datasheets before schematic use.

| Pico 2 GPIO | Function |
|------------|----------|
| GPIO0      | MISO / RX (SPI0) |
| GPIO1      | MOSI / TX (SPI0) |
| GPIO2      | SCLK (SPI0)   |
| GPIO3      | CS0 → board 0 |
| GPIO4      | CS1 → board 1 |
| GPIO5      | CS2 → board 2 |
| GPIO6      | CS3 → board 3 |
| GPIO7      | CS4 → board 4 |

Pico 2 has an onboard Micro-USB connector, but the keyboard exposes USB-C at
the carrier-board edge. Route the Pico 2 underside USB test pads (VBUS, GND,
D+, and D−) to the carrier's USB-C receptacle; leave the onboard Micro-USB
unused inside the enclosure. The Pico 2 uses its internal USB peripheral for
TinyUSB CDC-serial + USB-MIDI (as in the existing `main.c`).

### 4.3 Support parts

- A dedicated 3.3 V regulator for the key boards. Do **not** power all 60
  sensors and five ADCs from the Pico 2's 3.3 V regulator. A standard
  Raspberry Pi Pico's `3V3_OUT` is limited to about 300 mA, below the estimated
  DRV5056 sensor load alone: **360 mA typical / 600 mA maximum** for 60 sensors,
  before ADCs, references, master and losses. Feed the key boards from a carrier-mounted
  5 V-to-3.3 V regulator rated for **at least 1 A**, with a shared ground to
  the Pico 2. A 1 A regulator rating alone does not validate the USB input-current
  budget; check the source/current contract, efficiency, cable drop and startup.
  These sensor figures apply to DRV5056, not OH49E-S; recalculate if the sensor
  changes rather than substituting a different part's typical current as a maximum.
- On the carrier USB-C circuit, include 5.1 kΩ pull-down resistors from CC1
  and CC2 to ground, USB ESD protection at the receptacle, and short parallel
  D+/D− traces over a continuous ground plane. Connect USB 5 V to the Pico
  2's USB/VBUS input, never directly to the 3.3 V rail. Do not connect a host
  to the carrier USB-C and onboard Micro-USB ports at the same time.
- Decoupling capacitors at every ADC and Hall-sensor supply, plus master-board
  bulk capacitance. Reuse the Fluxpad sensor-support implementation when
  creating the key-board schematic.
- Optional flash chip for calibration storage (see §7).

## 5. Note map

5 octaves, chromatic, **MIDI 36–95** (C2–B6 using middle C = C4) — semitone step per key across the
60 keys (board n channel i → note `36 + n*12 + i`).

## 6. Board-to-board wiring: daisy-chain vs star

- **Daisy-chain (recommended):** each key board has 2 connectors (in/out);
  the shared SPI lines and VCC/GND run board→board→→master. Fewest cables.
- **Star:** all 5 boards each wire directly to the master.
- Both use a shared SPI bus with unique CS#n, but are **not signal-integrity
  equivalents**. Validate cable length, stubs, ground returns, current capacity
  and MISO handoff before finalizing the topology. A shared chain carries all
  five CS lines, with exactly one local selection jumper bridged per board.

## 7. Firmware requirements (for the implementing agent)

Reuse the existing two-threshold timing + velocity curve from
`examples/fluxpad_midi/src/main.c`. Replace the read path and add calibration
and scan scheduling for the main controller:

- `ANALOG_KEY_COUNT`  → 60 (5 boards × 12 keys).
- Replace direct Pico ADC reads with **SPI reads of the ADS7953** on each board:
  - Select board by asserting its CS#n, write channel-addressing command over
    SPI, read back the 12-bit channel value.
  - Sequence / interleave reads across the 5 boards to keep per-key timing tight.
- `midi_notes[]` → 60-entry chromatic `{36..95}`.
- Retain the two-threshold timing and square-root velocity-curve approach;
  thresholds and timing constants require validation/calibration for this hardware.
- MUX/hardware init → SPI controller init in `main()`.
- Add a **per-key calibration pass** (samples resting + bottom of travel per key,
  stores offsets in flash) to normalize key-to-key response across the 60 keys.

### 7.1 ADS7953 SPI protocol notes

- SPI mode / command framing per ADS79xx datasheet (SLAS605C).
- Each conversion frame begins with a **CS falling edge** and uses 16 clocks;
  result words include the channel address and 12-bit conversion value.
- Channel selection is pipelined relative to command frames. “Zero latency”
  does not mean a new channel command returns its result immediately.
- Use sequencer / auto-scan only with the documented per-frame CS timing, not
  one continuously asserted CS for a 12-channel sweep. Account for the 325 ns
  acquisition requirement, maximum 1 MSPS, pipeline/boundary frames and bus
  handoff in the five-board schedule. The final transaction table is outstanding.

## 8. Cost estimate (JLC-assembled key boards + hand-soldered master)

| Item                           | Qty | Unit    | Subtot  |
|--------------------------------|-----|---------|---------|
| Raspberry Pi Pico 2 module (master) | 1 | TBD | TBD |
| ADS7953SDBTR ADC (C701670), screenshot price 2026-11-02 | 6 | $8.5952 | $51.57 |
| Hall sensor (saved DRV5056A3; selection to reconfirm) | 60 | TBD | TBD |
| Gateron KS-20 White switch (built-in magnet) | 60 | TBD | TBD |
| USB-C + MCU support (master)   | 1   | $3.50   | $3.50   |
| Passives / conns (all boards)  | batch | —     | ~$12.00 |
| Key PCBs ×5 (JLC)              | panels | —    | $10–25  |
| JLC SMT assembly (key boards)  | 5   | —       | $30–70  |
| Master PCB (hand-solder)       | 1–5 | —       | $5–10   |
| **Historical total complete system estimate (not refreshed for updated ADC/reference/power choices)** | | | **~$110–170** |

Scale note: to go beyond 60 keys, add more key boards (new CS line per board)
and extend the note map; no MCU change required.

## 9. Open items / decisions pending

- **Next pre-routing task:** reconfirm the sensor with the owner, then resolve the placed sensor-footprint discrepancy
  against Fluxpad V2, verify mapping/orientation and implement the approved
  1.0 mm thickness through Konnect. The owner selected ADS7953 / REF5025,
  Range 1; full theoretical sensor coverage is not guaranteed, but actual switch
  clipping is unmeasured. Prototype range margin, acquisition, power,
  interconnect and protocol validation remain open. The owner clarified
  **six boards total: five populated 12-key
  sensor boards plus one Pico 2 master carrier**, for 60 keys. Five chip-select
  lines suffice; the master itself needs no CS and there is no sixth-sensor-board
  expansion requirement. The sixth ADC in the cost table is a spare, not another
  populated board.
  See `HANDOVER.md` for provisional 4 kHz/key throughput and averaging constraints.
- Reconfirm **ADS7953SDBTR / C701670** stock, quantity-specific price and assembly
  eligibility in JLCPCB's BOM tool immediately before ordering. Validate real
  KS-20 sensor travel/output and useful threshold margin within the selected
  0–2.5 V range across representative units and operating conditions.
- Verify the Pico 2 footprint, underside USB-test-pad routing, and USB-C
  connector mechanical clearance before laying out the master carrier board.
- Confirm the approved sensor's exact MPN, catalog identity, availability and
  pricing before ordering. **C85573 identifies OH49E-S, not DRV5056A3**; do not
  order or update assembly fields using the previous incorrect association.
- Confirm KS-20 White exact variant, availability and pricing before ordering;
  no current switch cost is established. Validate the complete mechanical stack
  and final magnet-to-sensor travel against the official specification and reference.
- Confirm daisy-chain connector type (IDC vs JST) once board-to-board distance
  and physical layout is fixed.

### 9.1 JLCPCB part-availability check

JLCPCB has no stable public parts-search API, but its public catalog page can
be queried from a shell before releasing a BOM:

```sh
part="ADS7953SDBT"

curl -fsSLG "https://jlcpcb.com/parts/componentSearch" \
  --data-urlencode "searchTxt=$part"
```

For readable terminal output, if `lynx` is installed:

```sh
part="ADS7953SDBT"

curl -fsSLG "https://jlcpcb.com/parts/componentSearch" \
  --data-urlencode "searchTxt=$part" |
  lynx -stdin -dump -nolist
```

Search the output for the result count and catalog part table. The public
search page has returned `0 Found` even for generic components known to be in
the catalog, so do **not** treat that response as stock evidence. Verify the
exact LCSC part number, assembly eligibility, and live quantity in JLCPCB's
logged-in BOM tool before committing to an ADC. Select a normal catalog-stock
ADC rather than relying on Global Sourcing or consigned parts.

## 10. References

- ADS79xx datasheet (SLAS605C) — TI.
- ADS7953 (16-channel) is selected. A single-channel ADS7886 is not a direct
  replacement for the required 12-/16-channel board ADC.
- `examples/fluxpad_midi/src/main.c` (two-threshold velocity algorithm) — reference firmware.
- [Fluxpad reference KiCad project](https://github.com/sssata/fluxpad/tree/8b2e752136033a81b9da01566e2e36ccafb966d6/ECAD/FluxpadKicad) — existing project whose
  `Fluxpad.kicad_sch` retains DRV5056-labelled symbols but assigns OH49E-S / C85573;
  its `SW_Cherry_MX_PCB_1.00u_hall_effect` footprint, sensor support circuitry,
  USB-C footprint, and JLC gerber export pipeline are the source design to
  reuse.
- [Fluxpad DSA keycap CAD model](https://github.com/sssata/fluxpad/blob/8b2e752136033a81b9da01566e2e36ccafb966d6/CAD/DSA_Keycap_CherryMX.step) — DSA-profile, Cherry-MX-stem
  keycap reference model.
