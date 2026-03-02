# HalfMeter

VST3 monitoring plugin for FL Studio focused on loudness and true peak metering.

## Goals (v1)

- Loudness: ITU-R BS.1770 (LUFS) Momentary, Short-term, Integrated; Loudness Range (LRA)
- True Peak: dBTP metering (oversampled) with peak hold and max readout
- Output level: sample peak dBFS meter ("Master Volume" in the UI)

## UI (Monitor page)

- Dark theme (black background)
- Four meters: Integrated LUFS, Short-term LUFS, Master Volume (sample peak dBFS), True Peak (dBTP)
- True Peak displays L/R bars and a max readout

Design source: `docs/figma.md`

## Docs

- Spec: `docs/spec.md`
- UI: `docs/ui.md`
- Dev setup (planned JUCE/VST3): `docs/dev-setup.md`

## Status

## Build

See `docs/dev-setup.md`.

## Status

- CMake + JUCE plugin skeleton is present (VST3 + Standalone)
- Monitor UI is implemented with real LUFS + True Peak metering (libebur128)
- Sample peak dBFS is measured and displayed as "Master Volume" (block peak)
