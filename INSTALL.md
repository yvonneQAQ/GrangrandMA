# Installing GrangrandMA on macOS

The release zip contains Universal binaries for Apple Silicon and Intel Macs.

## Standalone

Open `GrangrandMA.app`, allow microphone access, and select your microphone under
**Options > Audio/MIDI Settings**. Headphones are strongly recommended to avoid feedback.

Note: If you encounter an issue with trying to open the standalone file on Mac; that is expected.


You'll need to instruct MacOS to [remove the quarantine attribute](//superuser.com/questions/28384/what-should-i-do-about-com-apple-quarantine). 


Open your terminal and run the following command:

```
sudo xattr -rd com.apple.quarantine <YOUR_PATH_TO_STANDALONE>/GrangrandMA.app
```

Replace `<YOUR_PATH_TO_STANDALONE>` with the path to the GrangrandMA app. 

## VST3

Copy the entire `GrangrandMA.vst3` bundle to:

```text
~/Library/Audio/Plug-Ins/VST3/
```

Restart your DAW or rescan its plug-ins after copying.

## Audio Unit

Copy the entire `GrangrandMA.component` bundle to:

```text
~/Library/Audio/Plug-Ins/Components/
```

Restart the host after copying.

## Max/MSP

After installing the VST3, create this signal path:

```text
[adc~ 1]
    |
[vst~ 1 2 @prefer VST3]
   |     |
[dac~ 1 2]
```

Send `plug_vst3 GrangrandMA` to `vst~`. If Max cannot find it, send `scan 3`, wait for the scan,
and try again. Double-click `vst~` to open the plug-in editor.

## macOS security notice

This community build is ad-hoc signed and is not Apple-notarized. macOS may block software that
was downloaded from the internet. If it does, first try **System Settings > Privacy & Security >
Open Anyway**. Only if you trust the downloaded file, you can remove its quarantine attribute in
Terminal before installing:

```sh
xattr -dr com.apple.quarantine GrangrandMA.app
xattr -dr com.apple.quarantine GrangrandMA.vst3
xattr -dr com.apple.quarantine GrangrandMA.component
```
