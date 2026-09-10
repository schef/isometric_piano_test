# PCB Design — 60-Key Velocity-Sensitive MIDI Keyboard (Winner: SPI Bus / Design B)

This document specifies the winning hardware architecture for a scalable,
velocity-sensitive MIDI piano keyboard. It is the authoritative reference for
any agent implementing the firmware or schematic of this board, and inherits
all the working velocity / USB-MIDI logic from the existing `main.c` prototype.

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
boards + CS lines), keeps velocity timing fine via per-board parallel ADC reads,
and is noise-robust over distance compared to the analog-MUX alternative.

## 2. Why this design (decision rationale)

| Criterion                | Design A (analog MUX)       | Design B (SPI ADC/board)  |
|--------------------------|-----------------------------|----------------------------|
| Max keys                 | ~64 (Pico 4-ADC wall)       | many (add boards + CS)    |
| Full 60-key sweep        | ~1.2 ms serial (coarse)     | ~100–300 µs parallel      |
| Velocity timing granularity | coarser / hacky          | fine, preserved           |
| Scalability              | Poor                        | Good                       |
| Distance/noise immunity  | Poor (analog over wire)     | Good (digital at source)  |

The Pico has 4 ADC pins; an analog design wall-caps around 64 keys and degrades
velocity timing when many keys are scanned serially. The SPI design converts to
digital at each board, so it scales and stays fast. **This wins.**

## 3. Key board (JLC-assembled, qty 5)

### 3.1 Block diagram

```
12 × DRV5056A3QDBZ linear Hall sensors
   │  analog outputs (VCC/3.3, GND, VOUT)
   ▼
ADS7953 (12-bit, 16-channel, 1-MSPS, SPI ADC)   ← 1 chip per board
   │
   ▼
SPI: SCLK, MOSI(SDI), MISO(SDO), CS#n          → shared bus to master
```

### 3.2 ADC selection (THE WINNER PART)

- Selected: **ADS7953SDBT** — 12-bit, **16-channel**, 1-MSPS, single-ended,
   SPI, 38-pin TSSOP. JLC/LCSC part **C701092**.
  - Its external reference is **REF5025AIDR** (SOIC-8, JLC/LCSC **C11341**),
    producing 2.5 V. Follow SLAS605C: place a 10 uF ceramic directly between
    ADS7953 REFP and REFM, short MXO to AINP for the unbuffered path, and tie
    AINM to the analog-ground plane.
  - Use channels 0–11 for the 12 keys. Exclude channels 12–15 from the
    sequencer; tie them to ground if the final configuration requires them to
    be connected.
  - The 38-pin TSSOP is preferred over VQFN for visual inspection and rework.
  - 20 MHz serial interface; 1 MSPS aggregate. A 12-key board requires only
    12 × 4 kHz = **48 kSPS**, while a 12-channel sweep permits ~83 kSPS per
    key: more than 20× the required per-key scan rate.
  - Five boards require approximately 3.84 Mbit/s of 16-bit SPI transfers at
    a 4 kHz scan rate, well below the 20 MHz serial-interface limit.
  - Observed JLC stock was 27 units; order 6 for five boards plus one spare,
    subject to live BOM-tool verification when ordering.

### 3.3 Sensor — DRV5056A3QDBZ (per key)

- Use the same TI **DRV5056A3QDBZ** linear, ratiometric, unipolar Hall-effect
  sensor as the existing Fluxpad design. This avoids qualifying a new sensor
  for the key mechanism.
- 50 mV/mT sensitivity, ±79 mT range, 20 kHz bandwidth, 3.3 V operation,
  SOT-23 package.
- One per key: 3 pins (VCC, GND, VOUT) → VOUT directly to an ADC channel.
- Reuse Fluxpad's `Tang:SW_Cherry_MX_PCB_1.00u_hall_effect` footprint and its
  sensor placement relative to the Cherry-MX-style key. The Fluxpad schematic
  identifies the JLC/LCSC part as **C85573**; confirm current availability
  before placing the assembly order.
- Small neodymium magnet on each key stem; sensor fixed under the key.
- Wiring short (on-board), so no buffering needed.

### 3.4 Per-key analog circuit (four parts)

Copy the non-LED portion of Fluxpad's Hall-sensor circuit for every key:

1. DRV5056A3QDBZ Hall sensor.
2. 1 µF capacitor from the sensor's 3.3 V supply to ground.
3. 100 Ω series resistor between the sensor output and the ADS7953 channel.
4. 1 µF capacitor from the ADC-side of that resistor to ground.

Do **not** include Fluxpad's per-key LED components: the 22 Ω LED resistor
(`R4` for Fluxpad key 4) and white counterpost LED (`D4`) are unrelated to the
Hall signal path. This design has no per-key LEDs, avoiding their power,
routing, cost, and firmware requirements.

The 100 Ω + 1 µF output filter intentionally smooths the Hall signal. The ADC
has ample scan-rate margin, but validate this filter's effect on velocity timing
with the completed 12-key prototype before finalizing its values.

### 3.5 Key mechanism

- Use **Wooting Lekker V2 L60 Linear** Hall-effect switches for all 60 analog
  keys. Their 60 g springs provide the deliberately firm resistance selected
  for this musical controller and retain Cherry-MX-compatible stems.
- Use **DSA-profile, Cherry-MX-stem keycaps**. The Fluxpad source includes the
  reference CAD model at `../fluxpad/CAD/DSA_Keycap_CherryMX.step`.
- Magnet glued to the stem; DRV5056A3QDBZ sensor directly beneath.
- Fluxpad's Gateron Red switches and Kailh MX hot-swap sockets are for its
  separate digital keys and are not part of this all-analog keyboard design.

### 3.5 Interconnect / connectors

- Each board needs 2 inter-board connectors (daisy-chain in/out) OR 1 (star) —
  see section 6. Signals per board:
  - VCC (3.3 V), GND
  - SPI: SCLK, MOSI, MISO (shared)
  - CS#n (unique per board)
- Recommend a small IDC / JST ribbon connector (8-conductor) per board.

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

| Pico 2 GPIO | Function |
|------------|----------|
| GPIO0      | SCLK (SPI0)   |
| GPIO1      | MOSI (SPI0)   |
| GPIO2      | MISO (SPI0)   |
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
  ≈0.4 A worst-case key-board load. Feed the key boards from a carrier-mounted
  5 V-to-3.3 V regulator rated for **at least 1 A**, with a shared ground to
  the Pico 2.
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

5 octaves, chromatic, **C1–B5 (MIDI 36–95)** — semitone step per key across the
60 keys (board n channel i → note `36 + n*12 + i`).

## 6. Board-to-board wiring: daisy-chain vs star

- **Daisy-chain (recommended):** each key board has 2 connectors (in/out);
  the shared SPI lines and VCC/GND run board→board→→master. Fewest cables.
- **Star:** all 5 boards each wire directly to the master.
- Either is electrically equivalent (SPI is a shared bus; CS#n is unique per
  board). Daisy-chain is preferred for a row of keys.

## 7. Firmware requirements (for the implementing agent)

Reuse the existing two-threshold timing + velocity curve from `main.c`; the
changes are all in the read path:

- `ANALOG_KEY_COUNT`  → 60 (5 boards × 12 keys).
- Replace direct Pico ADC reads with **SPI reads of the ADS7953** on each board:
  - Select board by asserting its CS#n, write channel-addressing command over
    SPI, read back the 12-bit channel value.
  - Sequence / interleave reads across the 5 boards to keep per-key timing tight.
- `midi_notes[]` → 60-entry chromatic `{36..95}`.
- Two-threshold timing + square-root velocity curve: **unchanged**.
- MUX/hardware init → SPI controller init in `main()`.
- Add a **per-key calibration pass** (samples resting + bottom of travel per key,
  stores offsets in flash) to normalize key-to-key response across the 60 keys.

### 7.1 ADS7953 SPI protocol notes

- SPI mode / command framing per ADS79xx datasheet (SLAS605C).
- 12-bit SAR, zero latency; word size includes leading zeros + channel address.
- Use the ADC's **sequencer / auto-scan** to sweep the 12 channels of a board in
  one CS assertion for efficiency.

## 8. Cost estimate (JLC-assembled key boards + hand-soldered master)

| Item                           | Qty | Unit    | Subtot  |
|--------------------------------|-----|---------|---------|
| Raspberry Pi Pico 2 module (master) | 1 | TBD | TBD |
| ADS7953SDBT ADC (C701092)      | 6   | $5.1201 | $30.72 |
| DRV5056A3QDBZ Hall sensor      | 60  | TBD      | TBD     |
| Neodymium magnets              | 60  | $0.05   | $3.00   |
| Wooting Lekker V2 L60 Linear switch | 60 | €29.99 / 70-pack | €29.99 |
| USB-C + MCU support (master)   | 1   | $3.50   | $3.50   |
| Passives / conns (all boards)  | batch | —     | ~$12.00 |
| Key PCBs ×5 (JLC)              | panels | —    | $10–25  |
| JLC SMT assembly (key boards)  | 5   | —       | $30–70  |
| Master PCB (hand-solder)       | 1–5 | —       | $5–10   |
| **Total complete system**      |     |         | **~$110–170** |

Scale note: to go beyond 60 keys, add more key boards (new CS line per board)
and extend the note map; no MCU change required.

## 9. Open items / decisions pending

- Reconfirm ADS7953SDBT / C701092 stock and assembly eligibility in JLCPCB's
  BOM tool immediately before ordering.
- Verify the Pico 2 footprint, underside USB-test-pad routing, and USB-C
  connector mechanical clearance before laying out the master carrier board.
- Confirm DRV5056A3QDBZ / LCSC C85573 availability and current pricing before
  ordering; it replaces the previously proposed SS49E.
- Confirm Wooting Lekker V2 L60 availability, pricing, and magnet dimensions
  before ordering; the current quoted price is €29.99 per 70-pack, which covers
  the 60 switches required. Validate the final magnet-to-sensor travel during
  calibration.
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
- ADS7886 (single-channel fast ADC) and ADS7953 (16-ch) as fallback options.
- Existing `main.c` (two-threshold velocity algorithm) — the firmware foundation.
- `../fluxpad/ECAD/FluxpadKicad` — existing KiCad project whose
  `Fluxpad.kicad_sch` uses the DRV5056A3QDBZ sensor and whose
  `SW_Cherry_MX_PCB_1.00u_hall_effect` footprint, sensor support circuitry,
  USB-C footprint, and JLC gerber export pipeline are the source design to
  reuse.
- `../fluxpad/CAD/DSA_Keycap_CherryMX.step` — DSA-profile, Cherry-MX-stem
  keycap reference model.
