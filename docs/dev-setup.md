# Dev Setup (Planned)

This project is intended to be a Windows-first VST3 plugin.

## Recommended stack

- JUCE (plugin framework)
- Steinberg VST3 SDK (via JUCE modules or standalone SDK)
- Visual Studio 2022 (MSVC)

## Next steps

1. Create a JUCE Audio Plugin project named `HalfMeter` (VST3 enabled)
2. Implement audio analysis (BS.1770 + True Peak) in the processor
3. Implement the Monitor UI in the editor (dark theme, 4 tiles)

When code is added, this doc will include exact build commands and paths.
