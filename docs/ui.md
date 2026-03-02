# UI Spec (Monitor)

## Layout

- Title centered: "Monitor"
- Four meter tiles:
  - Integrated LUFS
  - Short-term LUFS
  - Master Volume (Sample Peak dBFS)
  - True Peak (dBTP)

## Theme

- Background: #0B0D10
- Tile background: #12151B
- Tile stroke: #232833 (1px)
- Title text: #E9EDF3
- Label text: #AAB2BF
- Meter fill: #FF2A2A
- Tick/scale lines: #2A3140

## Meter semantics

- Integrated LUFS:
  - Bar range: -36 .. 0 LUFS
  - Numeric: current integrated value
- Short-term LUFS:
  - Bar range: -36 .. 0 LUFS
  - Numeric: current short-term value
- Master Volume:
  - Bar range: -60 .. 0 dBFS
  - Numeric: current sample peak dBFS (max of L/R)
- True Peak:
  - Bar range: -12 .. +3 dBTP
  - Display: two thin bars inside the tile (L/R)
  - Numeric: max(L,R)

## Controls (v1)

- Reset LUFS: resets integrated LUFS + LRA (and clears M/S until they refill)
- Freeze (optional): pauses meter updates and history scrolling
