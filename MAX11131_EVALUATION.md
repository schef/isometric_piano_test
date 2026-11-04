# MAX11131 replacement evaluation — 2026-11-02

## Decision

**2026-11-04 update:** the owner selected KS-20 White and retained ADS7953 /
REF5025, Range 1 (0–2.5 V). MAX11131 is not selected. The evaluation below records
the earlier alternative and sourcing evidence, not a pending replacement decision.
See [SWITCH_RANGE_DECISION.md](SWITCH_RANGE_DECISION.md).

**Electrically promising; not approved as the production replacement.**
MAX11131 supports a single 3.3V supply and a supply-level reference, removing
the ADS7953 input-range mismatch without adding a 5V inter-board rail.
Owner-provided JLCPCB screenshots now show MAX11131AUI+T as pre-order only,
zero stock, at an estimated US$33.0809/unit. It does not satisfy the normal-stock
requirement and is substantially more expensive than the stocked ADS7953 option.
Do not redesign the board around it. A qualified footprint is also outstanding.

This is a document-level feasibility evaluation, not a schematic change,
simulation, measured prototype, firmware implementation, or manufacturing release.
The owner clarified six boards total means five sensor boards plus the Pico 2
master carrier: 60 keys and five CS lines. There is no sixth-sensor-board requirement.

## Device and evidence

- Candidate: **MAX11131AUI+**, or tape-and-reel **MAX11131AUI+T**, subject to
  confirmation of current ordering/lifecycle data.
- Package: **28-pin TSSOP-EP**, not ordinary TSSOP without an underside pad.
  The inspected datasheet identifies package U28E+4, outline 21-0108 and
  land pattern 90-0146. MAX11131ATI+ is the different TQFN variant.
- Source inspected: manufacturer-authored MAX11129–MAX11132 datasheet,
  **19-6026, Rev. 5, September 2012**, retrieved from a third-party mirror.
  Manufacturer PDF/product-page requests failed or timed out during this session.
  Confirm the latest official datasheet and errata before final selection.
- Temporary source files: `/tmp/opencode/max11131.pdf` and `max11131.txt`.

## Electrical fit

| Requirement | Datasheet evidence | Assessment |
| --- | --- | --- |
| 12 or 16 channels | MAX11131 has 16 single-ended channels | Pass at device level |
| 12-bit readings | 12-bit SAR ADC | Pass |
| Existing 3.3V power | VDD 2.35–3.6V; OVDD 1.5–3.6V | Pass, subject to rail tolerances/transients |
| Input range covering sensor output | Unipolar input 0–REF+; REF+ allowed 1V to VDD + 50mV; electrical tables tested with REF+ = VDD | Pass with REF+ on local 3.3V |
| Pico logic compatibility | Set OVDD to 3.3V | Compatible in principle; verify full timing/logic margins |
| Shared MISO bus | DOUT high-impedance when CS high | Supported; timing/cabling still require review |
| Speed | Up to 3MSPS, external SCLK up to 48MHz | Sufficient headline capacity; not the actual shared-bus rate |

Proposed concept: VDD, OVDD and REF+ derive from the existing local 3.3V rail,
with manufacturer-specified local bypassing and short reference returns. The
16-channel part supports a single-ended reference mode; choose that mode rather
than accidentally sacrificing AIN15 to a differential-reference function.

At 3.3V reference, 1 LSB is approximately **0.806mV**. A 3.1V sensor output
is below full scale; the ADC need not use every code for useful velocity timing.
Sensor and reference voltage drops/noise must be budgeted at their actual pins.
The DRV5056 sensitivity is supply-dependent, so sharing the supply/reference
can reduce some supply-induced gain variation, but its nominal 0.6V offset
means exact cancellation of every supply error must not be assumed.

The ADC itself has a listed maximum VDD current of 6.5mA at the stated 3MSPS
test conditions. Add I/O/reference loads as applicable; the Hall sensors still
dominate the board current and the system USB/regulator budget remains open.

## Five-sensor-board transaction budget

Use external-clock, single-ended, unipolar operation; retain channel IDs.
SPI is **mode 3** (CPOL = CPHA = 1), with at least **16 clocks per frame**.
CS edges start frames; it is not a continuously asserted-CS sweep.
External-clock conversion results arrive in the next frame. Account for that
pipeline explicitly; changing a register may invalidate results until mode
control is restored. Standard_Ext can scan channels 0–11. Manual mode is
another option for explicit channel requests.

For five sensor boards × 12 keys × 4kHz (the master adds no ADC readings):

- Required useful readings: **240,000/s**, or 60 every 250us.
- Useful transfer load: **3.840Mbit/s** at 16 bits per result.
- At a provisional 20MHz SCLK, 16 clocks take **0.8us**.
- Allowing a **0.2us high/quiet gap** gives a planning slot of **1us/frame**.
  This exceeds the minimum CS pulse width and the recommended one-SCLK quiet
  interval. Check actual controller waveforms, especially CS release and MISO
  handoff; the data-output delay table is specified at only 10pF load.
- Planning allowance: **16 frames per board** (12 readings plus four frames
  reserved for pipeline/boundary overhead), five sensor boards = **80us per sweep**,
  leaving 170us of the nominal 250us period for other overhead.

This is a feasibility allocation, not a proven schedule: configuration is not
necessarily needed every sweep; the final command/result table must establish
the exact number of boundary frames, channel order, timestamps and which
results are discarded. Confirm Pico SPI clock generation, DMA/CS control,
interrupt latency and actual cable loading before treating 20MHz as achieved.
No external CNVST or EOC line is needed for the proposed external-clock method;
unused multifunction pins need defined treatment in the replacement schematic.

The 3MSPS chip rating does **not** make eight-sample averaging fit a 20MHz
shared bus: that still needs 1.920M conversions/s and at least 30.720Mbit/s.
The chosen no-averaging budget preserves the existing provisional requirement,
not a measured performance claim or a new firmware task.

## Existing Hall filters and acquisition

- Keep the distinction between sensor response and ADC input settling. The
  existing 100-ohm/1uF filter has a 100us time constant and approximately
  220us ideal 10–90% step response; that musical timing tradeoff still exists.
- The MAX11131 electrical table lists 52ns acquisition and nominal sampling
  capacitance of 15pF or 7.5pF depending on range configuration. External scan
  selection changes on the thirteenth SCLK falling edge. A slower 20MHz clock
  and explicit CS gap provide more acquisition time than the 48MHz headline
  mode, but exact acquisition behavior must follow the selected mode/diagram.
- As a rough charge-reservoir check, 15pF sampling a full 3.3V step would move
  about 49.5pC; sharing that with an ideal 1uF capacitor changes its voltage by
  about 49.5uV (0.061 LSB). This is encouraging, **not settling qualification**:
  effective capacitance under bias, ADC switch resistance, package/trace
  inductance, sensor dynamics, leakage and previous-channel history are omitted.
- The listed maximum 1.5uA static input leakage would create 0.15mV across
  100 ohms alone (0.19 LSB), excluding sensor output impedance.
- Before approving an unbuffered input, validate alternating high/low channels
  and real key strokes at the intended scan rate, or perform a more complete
  manufacturer-model analysis followed by prototype measurements.

## Schematic/layout impact if selected

- Replace the 38-pin ADS7953 symbol and footprint with verified MAX11131AUI+
  TSSOP-EP assets. This is **not a pin-compatible substitution**.
- Remap the 12 existing KEYx_ADC nets to AIN0–AIN11; retain shared SPI and one
  local CS per board. Resolve unused AIN12–AIN15/multifunction pins explicitly.
- Remove REF5025 and its dedicated support circuitry only after the new
  reference/bypass circuit is reviewed. C4 or other capacitors may be reusable,
  but do not delete/reassign components without checking PCB placement and nets.
- Add/allocate the recommended supply/reference bypass network. The inspected
  pin table calls for 10uF || 0.1uF at VDD and OVDD and 0.47uF at REF+; the
  application section also discusses 10uF/0.1uF reference bypassing. Resolve
  the final network from the latest datasheet/reference design and rail-noise
  requirements rather than assuming any generic capacitor set is sufficient.
- Verify and ground the exposed pad with the correct land/paste/thermal-via
  design. It is less straightforward to inspect/rework than a lead-only TSSOP.
- The existing five CS lines are sufficient for five sensor boards; no sixth CS
  is required. Review the missing local CS pull-up; replacing the ADC does not
  automatically define its deselected state during master reset/disconnection.
- Repeat ERC, native netlist inspection, footprint pin mapping and PCB DRC after
  an approved change. Do not synchronize the PCB as part of this evaluation.

## Sourcing and cost — blocking

### Owner-provided JLCPCB screenshots, 2026-11-02

These supersede the inconclusive public-web lookup and indicative distributor
price below. Quantities/prices reflect the screenshots, not a reserved stock
allocation, final assembly quote, or independently refreshed catalog check.

| MPN shown | JLCPCB code | Stock shown | Unit price shown (USD) | Status |
| --- | --- | --- | --- | --- |
| ADS7953SDBTR | C701670 | 13 | 8.5952 at 1+ | Extended, stocked listing |
| ADS7953SDBT | C701093 | 0 | 13.8791 estimated | Extended, pre-order |
| MAX11131AUI+T | C7457799 | 0 | 33.0809 estimated | Extended, pre-order |

At the displayed unit tiers, five stocked ADS7953SDBTR devices would cost about
US$42.98, versus US$165.40 for five MAX11131AUI+T devices at the pre-order
estimate, before assembly, shipping and tax. Confirm quantity-specific tiers.
The MAX therefore fails the current normal-stock sourcing gate; do not choose it.
TI's [ADS7953SDBTR ordering page](https://www.ti.com/product/ADS7953/part-details/ADS7953SDBTR)
now confirms that ADS7953SDBTR and ADS7953SDBT are the same product/package/pinout
with different carriers (tape-and-reel versus tubes). ADS7953SDBTR / C701670 is
the documented sourcing choice if retaining ADS7953. Saved KiCad C701092 fields
have not been edited. The owner subsequently retained REF5025 / Range 1;
prototype range margin and velocity-threshold validation remain open. Earlier
reference alternatives are recorded in `SCHEMATIC_REVIEW.md` for context.

### Earlier automated lookup (historical, not the current sourcing conclusion)

- No local Konnect JLCPCB database is installed. No full multi-million-part
  database was downloaded solely for this candidate.
- Public JLCPCB searches for `MAX11131` and `MAX11131AUI` both returned
  `0 Found`. As already documented in HANDOVER/PCB_DESIGN, this public endpoint
  has false-empty results. **This is not proof of no stock.**
- LCSC's public search returned no usable product detail. Web searches did not
  establish an exact LCSC C-number for this candidate.
- Therefore **normal JLCPCB catalog status, quantity, assembly eligibility,
  package-specific MPN and price are all unverified**. Do not substitute Global
  Sourcing or consigned stock for the owner's normal-stock requirement.
- A search-index result for DigiKey's MAX11131AUI+ page showed approximately
  **US$28.94 at quantity one** and 59 units, but direct page access returned
  HTTP 403. This is an unverified indicative cost warning, not a current quote
  or a JLCPCB price. Five ADCs at that unit price would be about US$145 alone,
  materially above the old US$5.12/unit ADS7953 estimate (also not a live quote).

**Original sourcing gate (now failed for the shown MAX listing):** check MAX11131AUI+ / MAX11131AUI+T in the logged-in JLCPCB
BOM tool for normal catalog stock of at least six units (five boards + spare),
exact package, assembly service and total cost. Until that evidence exists,
retain this as an electrical candidate and do not alter the schematic around it.
If it fails the gate, prioritize a new search from JLCPCB-stocked 12-/16-channel
SPI parts, then validate reference range and shared-bus timing against their
datasheets before recommending one.

## Links

- [Manufacturer product page](https://www.analog.com/en/products/max11131.html).
- [Inspected datasheet mirror](https://pdf.datasheet.live/37b8ead7/maximintegrated.com/MAX11131ATI+T.pdf)
  (includes the TSSOP variant in its ordering/pin tables).
- [JLCPCB search](https://jlcpcb.com/parts/componentSearch?searchTxt=MAX11131AUI).
- [DigiKey part page](https://www.digikey.com/en/products/detail/analog-devices-inc-maxim-integrated/MAX11131AUI/3877200).
