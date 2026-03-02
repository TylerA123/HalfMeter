# Roadmap

## v0.1 - Project scaffolding

- Choose framework (JUCE vs alternatives)
- Create a buildable VST3 project
- Add basic editor window with dark theme + 4 tiles

## v0.2 - Sample peak + basic UI

- Implement sample peak dBFS metering (L/R)
- Render the "Master Volume" meter tile (bar + numeric)

## v0.3 - LUFS

- Implement BS.1770 loudness (M/S/I) + Reset behavior
- Render Integrated + Short-term tiles

## v0.4 - True Peak

- Implement oversampled true peak (dBTP) + peak hold + clip indicator
- Render True Peak tile with L/R sub-bars

## v0.5 - Packaging

- Add presets/state persistence
- Add Windows installer or simple zip release
- Add basic automated build (CI)
