# Metering Spec

This document defines what the plugin measures and how values should behave.

## Loudness (LUFS)

- Standard: ITU-R BS.1770 (K-weighted)
- Outputs:
  - Momentary loudness (M): 400 ms window
  - Short-term loudness (S): 3 s window
  - Integrated loudness (I): running measurement with gating
  - Loudness Range (LRA): per EBU Tech 3342 (derived from short-term distribution)

### Display

- Primary unit: LUFS
- Suggested display range: -36 .. 0 LUFS
- UI update rate: ~10 Hz (do not render per-sample)
- Hold behavior:
  - M/S: optional max-hold until Reset

### Integration controls (v1)

- Reset clears integrated, LRA, and max values.
- Integration runs continuously while audio is non-silent.

## True Peak (dBTP)

- Standard concept: "True Peak" per ITU-R BS.1770 (intersample peak estimation)
- Measurement: oversample the post-plugin signal and compute peak of reconstructed waveform.
- Outputs:
  - True Peak Left (dBTP)
  - True Peak Right (dBTP)
  - True Peak Max (max of L/R)

### Display

- Suggested display range: -12 .. +3 dBTP
- Clip indicator:
  - Trigger when TP > 0.0 dBTP (optional threshold configurable later)
- Peak hold:
  - Hold marker with decay or "until Reset" (v1: until Reset)

## Output Level (Sample Peak, dBFS)

- Measure sample peak on the post-plugin output (no oversampling)
- Output: peak dBFS (max of L/R for the main numeric readout)
- Display range: -60 .. 0 dBFS
