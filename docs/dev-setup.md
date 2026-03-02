# Dev Setup (Planned)

This project is intended to be a Windows-first VST3 plugin.

## Recommended stack

- JUCE (plugin framework)
- Steinberg VST3 SDK (required by some JUCE setups)
- Visual Studio 2022 (MSVC)
- CMake 3.21+

## Getting the source

This repo uses CMake and fetches JUCE at configure time.

## Build (Windows)

From the repo root:

```bat
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

JUCE will produce artefacts under a path like:

- `build/HalfMeter_artefacts/Release/VST3/HalfMeter.vst3`
- `build/HalfMeter_artefacts/Release/Standalone/HalfMeter.exe`

## VST3 SDK

If CMake configure fails with a message about the VST3 SDK, install the Steinberg VST3 SDK and re-run configure with:

```bat
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DVST3_SDK_PATH="C:/path/to/VST_SDK"
```

## Next steps

1. Replace placeholder LUFS/True Peak values with real metering (see `docs/spec.md`)
2. Add Reset/Freeze controls and a shared snapshot model (see `docs/architecture.md`)
