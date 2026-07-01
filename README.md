# GrangrandMA

A real-time input granulator built with JUCE 8. It continuously records four seconds of incoming audio and creates overlapping, pitchable grains from that memory.

## Download

Download the latest macOS build from the [GitHub Releases page](https://github.com/yvonneQAQ/GrangrandMA/releases/latest).
The release contains Universal binaries for both Apple Silicon and Intel Macs:

- **Standalone**: open `GrangrandMA.app` and grant microphone access.
- **VST3**: copy `GrangrandMA.vst3` to `~/Library/Audio/Plug-Ins/VST3/`.
- **Audio Unit**: copy `GrangrandMA.component` to `~/Library/Audio/Plug-Ins/Components/`.

See [INSTALL.md](INSTALL.md) for Max/MSP setup and macOS troubleshooting.

## Build

The project uses an installed JUCE CMake package when available; otherwise CMake
downloads the pinned JUCE version automatically. Then run:

```sh
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --config Release -j 4
```

To use a local JUCE checkout instead of downloading it:

```sh
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release -DJUCE_SOURCE_DIR=/path/to/JUCE
cmake --build build-release --config Release -j 4
```

Formats: Audio Unit, VST3, and Standalone.

## Controls

- **Size**: grain duration (20–500 ms)
- **Density**: grains generated per second
- **Pitch**: playback transposition (±24 semitones)
- **Position**: how far back into the four-second input memory grains begin
- **Spray**: randomises each grain's starting point
- **Feedback**: feeds the granular output back into memory
- **Mix**: dry/wet balance

Start playback and send audio through the plug-in. It needs a moment to fill its live input memory.

## Using a microphone

The Standalone app requests microphone permission and enables live input automatically. Keep
headphones on while using a microphone to prevent acoustic feedback. Open **Options > Audio/MIDI
Settings** if you need to select a different input device or input channels.

In a DAW, place the plug-in on an audio track, select your microphone as that track's input, and
enable input monitoring. Mono microphone input is duplicated correctly into the stereo granular
output.

## License

Copyright (C) 2026 yvonneQAQ. GrangrandMA is distributed under the GNU Affero General Public
License v3.0. It uses JUCE 8 under the terms of the JUCE AGPLv3 option.
