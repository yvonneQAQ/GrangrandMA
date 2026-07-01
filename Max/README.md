# GrangrandMA for Max/MSP

Open `GrangrandMA.maxpat` in Max 8 or Max 9. The patch uses only standard Max/MSP objects and
does not require the VST3, RNBO, Java, or third-party externals.

1. Use headphones to avoid microphone feedback.
2. Click the **AUDIO** speaker button.
3. Speak into audio input 1.
4. If necessary, choose the microphone in **Options > Audio Status**.

The patch records a four-second circular microphone buffer and runs 32 overlapping grain voices.
`POSITION = 0` uses the newest safe audio for low-latency processing; larger values reach further
back into the recorded memory.
