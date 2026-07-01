autowatch = 1;
inlets = 5;
outlets = 1;

var voices = jsarguments.length > 1 ? jsarguments[1] : 32;
var memoryMs = jsarguments.length > 2 ? jsarguments[2] : 4000;
var voice = 1;
var writePhase = 0;
var lastWritePhase = 0;
var bufferHasWrapped = false;
var grainSize = 120;
var pitch = 0;
var position = 0;
var spray = 0.12;

function msg_float(value)
{
    if (inlet === 0)
    {
        if (value < lastWritePhase - 0.5)
            bufferHasWrapped = true;

        lastWritePhase = writePhase;
        writePhase = Math.max(0, Math.min(1, value));
    }
    else if (inlet === 1) grainSize = Math.max(20, Math.min(500, value));
    else if (inlet === 2) pitch = Math.max(-24, Math.min(24, value));
    else if (inlet === 3) position = Math.max(0, Math.min(1, value));
    else if (inlet === 4) spray = Math.max(0, Math.min(1, value));
}

function msg_int(value) { msg_float(value); }

function bang()
{
    if (inlet !== 0)
        return;

    var ratio = Math.pow(2, pitch / 12);
    var safety = 12;
    var towardWriter = Math.max(0, ratio - 1) * grainSize;
    var towardOldest = Math.max(0, 1 - ratio) * grainSize;
    var minimumDelay = safety + towardWriter;
    var recorded = bufferHasWrapped ? memoryMs : writePhase * memoryMs;
    var maximumDelay = Math.min(memoryMs, recorded) - safety - towardOldest;

    if (maximumDelay < minimumDelay)
        return;

    var delay = minimumDelay + (maximumDelay - minimumDelay) * position;
    delay += (Math.random() * 2 - 1) * spray * 750;
    delay = Math.max(minimumDelay, Math.min(maximumDelay, delay));

    var sourceSpan = grainSize * ratio;
    var start = writePhase * memoryMs - delay;
    while (start < 0) start += memoryMs;
    while (start >= memoryMs) start -= memoryMs;

    // play~ does not wrap across the end of a buffer. Moving the complete
    // source window inside the buffer keeps every grain click-free.
    start = Math.max(0, Math.min(memoryMs - sourceSpan - 1, start));
    var end = start + sourceSpan;
    var pan = Math.random() * 2 - 1;

    outlet(0, "target", voice);
    outlet(0, "grain", start, end, grainSize, pan);
    voice = voice % voices + 1;
}
