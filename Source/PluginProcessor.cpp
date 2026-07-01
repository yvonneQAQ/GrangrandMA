#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace IDs
{
    constexpr auto size = "size";
    constexpr auto density = "density";
    constexpr auto pitch = "pitch";
    constexpr auto position = "position";
    constexpr auto spray = "spray";
    constexpr auto feedback = "feedback";
    constexpr auto mix = "mix";
}

GrangrandMAAudioProcessor::GrangrandMAAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout GrangrandMAAudioProcessor::createParameterLayout()
{
    using Parameter = juce::AudioParameterFloat;
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<Parameter>(juce::ParameterID { IDs::size, 1 }, "Size",
               juce::NormalisableRange<float> { 20.0f, 500.0f, 1.0f, 0.45f }, 120.0f, "ms"));
    layout.add(std::make_unique<Parameter>(juce::ParameterID { IDs::density, 1 }, "Density",
               juce::NormalisableRange<float> { 1.0f, 50.0f, 0.1f, 0.5f }, 12.0f, "grains/s"));
    layout.add(std::make_unique<Parameter>(juce::ParameterID { IDs::pitch, 1 }, "Pitch",
               juce::NormalisableRange<float> { -24.0f, 24.0f, 0.01f }, 0.0f, "st"));
    layout.add(std::make_unique<Parameter>(juce::ParameterID { IDs::position, 1 }, "Position",
               juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f }, 0.35f, juce::AudioParameterFloatAttributes().withStringFromValueFunction(
                   [] (float value, int) { return juce::String(juce::roundToInt(value * 100.0f)) + " %"; })));
    layout.add(std::make_unique<Parameter>(juce::ParameterID { IDs::spray, 1 }, "Spray",
               juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f }, 0.12f, juce::AudioParameterFloatAttributes().withStringFromValueFunction(
                   [] (float value, int) { return juce::String(juce::roundToInt(value * 100.0f)) + " %"; })));
    layout.add(std::make_unique<Parameter>(juce::ParameterID { IDs::feedback, 1 }, "Feedback",
               juce::NormalisableRange<float> { 0.0f, 0.9f, 0.001f }, 0.15f, juce::AudioParameterFloatAttributes().withStringFromValueFunction(
                   [] (float value, int) { return juce::String(juce::roundToInt(value * 100.0f)) + " %"; })));
    layout.add(std::make_unique<Parameter>(juce::ParameterID { IDs::mix, 1 }, "Mix",
               juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f }, 0.5f, juce::AudioParameterFloatAttributes().withStringFromValueFunction(
                   [] (float value, int) { return juce::String(juce::roundToInt(value * 100.0f)) + " %"; })));
    return layout;
}

void GrangrandMAAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    granularEngine.prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
}

void GrangrandMAAudioProcessor::releaseResources() { granularEngine.reset(); }

bool GrangrandMAAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto input = layouts.getMainInputChannelSet();
    const auto output = layouts.getMainOutputChannelSet();
    return (output == juce::AudioChannelSet::mono() || output == juce::AudioChannelSet::stereo())
        && (input == output
            || (input == juce::AudioChannelSet::mono() && output == juce::AudioChannelSet::stereo()));
}

void GrangrandMAAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    GranularEngine::Parameters values;
    values.grainSizeMs = parameters.getRawParameterValue(IDs::size)->load();
    values.densityHz = parameters.getRawParameterValue(IDs::density)->load();
    values.pitchSemitones = parameters.getRawParameterValue(IDs::pitch)->load();
    values.position = parameters.getRawParameterValue(IDs::position)->load();
    values.spray = parameters.getRawParameterValue(IDs::spray)->load();
    values.feedback = parameters.getRawParameterValue(IDs::feedback)->load();
    values.mix = parameters.getRawParameterValue(IDs::mix)->load();
    granularEngine.process(buffer, values, getTotalNumInputChannels());
}

void GrangrandMAAudioProcessor::getStateInformation(juce::MemoryBlock& destination)
{
    if (auto xml = parameters.copyState().createXml())
        copyXmlToBinary(*xml, destination);
}

void GrangrandMAAudioProcessor::setStateInformation(const void* data, int size)
{
    if (auto xml = getXmlFromBinary(data, size); xml != nullptr)
        if (xml->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* GrangrandMAAudioProcessor::createEditor()
{
    return new GrangrandMAAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GrangrandMAAudioProcessor();
}
