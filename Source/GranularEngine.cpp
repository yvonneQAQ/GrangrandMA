#include "GranularEngine.h"

void GranularEngine::prepare(double newSampleRate, int, int numChannels)
{
    sampleRate = newSampleRate;
    channelCount = juce::jlimit(1, 2, numChannels);
    history.setSize(channelCount, juce::roundToInt(memorySeconds * static_cast<float>(sampleRate)));
    smoothedMix.reset(sampleRate, 0.025);
    smoothedMix.setCurrentAndTargetValue(0.5f);
    reset();
}

void GranularEngine::reset()
{
    history.clear();
    writePosition = 0;
    validSamples = 0;
    samplesUntilNextGrain = 0;
    previousWet.fill(0.0f);
    for (auto& grain : grains)
        grain = {};
}

double GranularEngine::wrapPosition(double position) const noexcept
{
    const auto size = static_cast<double>(history.getNumSamples());
    while (position < 0.0)
        position += size;
    while (position >= size)
        position -= size;
    return position;
}

float GranularEngine::readInterpolated(int channel, double position) const noexcept
{
    position = wrapPosition(position);
    const int first = static_cast<int>(position);
    const int second = (first + 1) % history.getNumSamples();
    const float fraction = static_cast<float>(position - static_cast<double>(first));
    const auto* data = history.getReadPointer(juce::jmin(channel, history.getNumChannels() - 1));
    return data[first] + fraction * (data[second] - data[first]);
}

bool GranularEngine::launchGrain(const Parameters& parameters)
{
    auto slot = std::find_if(grains.begin(), grains.end(), [] (const Grain& grain) { return ! grain.active; });
    if (slot == grains.end())
        return false;

    const int length = juce::jlimit(16, history.getNumSamples() / 2,
                                    juce::roundToInt(parameters.grainSizeMs * 0.001f * static_cast<float>(sampleRate)));
    const double increment = std::pow(2.0, static_cast<double>(parameters.pitchSemitones) / 12.0);

    // Keep the read head away from both edges of valid delay memory for the
    // grain's whole lifetime: high pitches approach the writer, low pitches
    // approach the oldest available sample.
    const double safetySamples = 0.012 * sampleRate;
    const double traversal = static_cast<double>(length - 1);
    const double relativeTravelTowardWriter = juce::jmax(0.0, increment - 1.0) * traversal;
    const double relativeTravelTowardOldest = juce::jmax(0.0, 1.0 - increment) * traversal;
    const double minimumDelay = safetySamples + relativeTravelTowardWriter;
    // During startup most of the circular buffer has never been written. Limit
    // grains to the recorded region so they cannot cross from zero-filled memory
    // into live audio halfway through their envelope (an audible pop).
    const double recordedHistory = static_cast<double>(validSamples);
    const double maximumDelay = juce::jmin(static_cast<double>(history.getNumSamples()), recordedHistory)
                              - safetySamples - relativeTravelTowardOldest;

    if (maximumDelay < minimumDelay)
        return false;
    const double position = juce::jlimit(0.0, 1.0, static_cast<double>(parameters.position));
    const double baseDelay = minimumDelay + juce::jmax(0.0, maximumDelay - minimumDelay) * position;
    const double sprayOffset = (random.nextDouble() * 2.0 - 1.0)
                             * juce::jlimit(0.0, 1.0, static_cast<double>(parameters.spray))
                             * 0.75 * sampleRate;
    const double delay = juce::jlimit(minimumDelay, maximumDelay, baseDelay + sprayOffset);

    slot->active = true;
    slot->readPosition = wrapPosition(static_cast<double>(writePosition) - delay);
    slot->increment = increment;
    slot->age = 0;
    slot->length = length;
    slot->pan = random.nextFloat() * 2.0f - 1.0f;
    return true;
}

void GranularEngine::process(juce::AudioBuffer<float>& buffer, const Parameters& parameters, int numInputChannels)
{
    if (history.getNumSamples() == 0)
        return;

    const int channels = juce::jmin(channelCount, buffer.getNumChannels());
    const int inputs = juce::jlimit(1, channels, numInputChannels);
    const int interval = juce::jmax(1, juce::roundToInt(sampleRate / juce::jmax(0.1f, parameters.densityHz)));
    smoothedMix.setTargetValue(juce::jlimit(0.0f, 1.0f, parameters.mix));

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        std::array<float, 2> dry { 0.0f, 0.0f };
        for (int channel = 0; channel < channels; ++channel)
        {
            // Duplicate a mono microphone into both sides when the output is
            // stereo, so the dry voice and grains remain centred and audible.
            const int sourceChannel = juce::jmin(channel, inputs - 1);
            dry[static_cast<size_t>(channel)] = buffer.getSample(sourceChannel, sample);
            history.setSample(channel, writePosition,
                              dry[static_cast<size_t>(channel)]
                              + previousWet[static_cast<size_t>(channel)] * juce::jlimit(0.0f, 0.92f, parameters.feedback));
        }

        if (--samplesUntilNextGrain <= 0)
        {
            launchGrain(parameters);
            samplesUntilNextGrain = interval;
        }

        std::array<float, 2> wet { 0.0f, 0.0f };
        float windowEnergy = 0.0f;
        for (auto& grain : grains)
        {
            if (! grain.active)
                continue;

            const float phase = static_cast<float>(grain.age)
                              / static_cast<float>(juce::jmax(1, grain.length - 1));
            const float window = 0.5f - 0.5f * std::cos(juce::MathConstants<float>::twoPi * phase);

            if (channels == 1)
            {
                wet[0] += readInterpolated(0, grain.readPosition) * window;
            }
            else
            {
                const float leftPan = std::sqrt(0.5f * (1.0f - grain.pan));
                const float rightPan = std::sqrt(0.5f * (1.0f + grain.pan));
                wet[0] += readInterpolated(0, grain.readPosition) * window * leftPan;
                wet[1] += readInterpolated(1, grain.readPosition) * window * rightPan;
            }

            windowEnergy += window * window;
            grain.readPosition = wrapPosition(grain.readPosition + grain.increment);
            if (++grain.age >= grain.length)
                grain.active = false;
        }

        // Normalising by the integer grain count changes the gain abruptly at
        // every launch, even though the new grain's envelope starts at zero.
        // Envelope energy changes continuously, so this avoids onset clicks.
        const float normalisation = 1.0f / std::sqrt(juce::jmax(1.0f, windowEnergy));
        const float mix = smoothedMix.getNextValue();
        for (int channel = 0; channel < channels; ++channel)
        {
            wet[static_cast<size_t>(channel)] *= normalisation;
            previousWet[static_cast<size_t>(channel)] = wet[static_cast<size_t>(channel)];
            buffer.setSample(channel, sample,
                             dry[static_cast<size_t>(channel)] * (1.0f - mix)
                             + wet[static_cast<size_t>(channel)] * mix);
        }

        writePosition = (writePosition + 1) % history.getNumSamples();
        validSamples = juce::jmin(validSamples + 1, history.getNumSamples());
    }
}
