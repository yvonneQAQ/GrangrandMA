#pragma once

#include <JuceHeader.h>
#include <array>

class GranularEngine
{
public:
    struct Parameters
    {
        float grainSizeMs = 120.0f;
        float densityHz = 12.0f;
        float pitchSemitones = 0.0f;
        float position = 0.35f;
        float spray = 0.12f;
        float feedback = 0.15f;
        float mix = 0.5f;
    };

    void prepare(double newSampleRate, int maximumBlockSize, int numChannels);
    void reset();
    void process(juce::AudioBuffer<float>& buffer, const Parameters& parameters, int numInputChannels);

private:
    static constexpr int maxGrains = 64;
    static constexpr float memorySeconds = 4.0f;

    struct Grain
    {
        bool active = false;
        double readPosition = 0.0;
        double increment = 1.0;
        int age = 0;
        int length = 1;
        float pan = 0.0f;
    };

    bool launchGrain(const Parameters& parameters);
    float readInterpolated(int channel, double position) const noexcept;
    double wrapPosition(double position) const noexcept;

    juce::AudioBuffer<float> history;
    std::array<Grain, maxGrains> grains;
    juce::Random random { 0x4752414e };
    double sampleRate = 44100.0;
    int writePosition = 0;
    int validSamples = 0;
    int samplesUntilNextGrain = 0;
    int channelCount = 2;
    std::array<float, 2> previousWet { 0.0f, 0.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedMix;
};
