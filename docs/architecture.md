# Architecture Notes

This plugin is a real-time audio meter. The audio thread must stay lock-free and allocation-free.

## Threading model

- Audio thread:
  - Reads input buffers
  - Updates metering state (LUFS + peaks)
  - Publishes snapshots for the UI (atomics or a lock-free ring buffer)
- UI thread:
  - Polls latest snapshot at a fixed rate (e.g. 10-30 Hz)
  - Renders meters, labels, and peak-hold markers

## Data flow

- Maintain per-channel peak trackers (sample peak and true peak).
- Maintain loudness state per BS.1770 (momentary/short-term windows + integrated + LRA).
- Publish a compact struct:
  - `float momentaryLUFS`, `shortTermLUFS`, `integratedLUFS`, `lraLU`
  - `float samplePeakDbfsL`, `samplePeakDbfsR`
  - `float truePeakDbtpL`, `truePeakDbtpR`
  - `bool tpClipped` (tp > 0.0 dBTP)
  - plus max/hold values for display

## True peak approach (v1)

- Oversample (4x or 8x) the post-plugin signal and compute the maximum intersample peak.
- Keep it simple first: a polyphase FIR resampler or an existing oversampling utility from the chosen framework.

## Loudness approach (v1)

- Use a known-good implementation of BS.1770 if possible (to avoid subtle gating/window bugs).
- A common choice is `libebur128` (open source) for LUFS + LRA.

## UI update rates

- Audio processing: per block
- Meter snapshot publish: per block
- UI redraw: 30-60 FPS (but values update at ~10 Hz is typically enough)
