#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class GrangrandMALookAndFeel final : public juce::LookAndFeel_V4
{
public:
    GrangrandMALookAndFeel();
    void drawRotarySlider(juce::Graphics&, int, int, int, int, float, float, float, juce::Slider&) override;
};

class GrangrandMAAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit GrangrandMAAudioProcessorEditor(GrangrandMAAudioProcessor&);
    ~GrangrandMAAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    struct Knob
    {
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<Attachment> attachment;
    };

    void setupKnob(Knob&, const juce::String& parameterID, const juce::String& labelText);

    GrangrandMAAudioProcessor& audioProcessor;
    GrangrandMALookAndFeel lookAndFeel;
    std::array<Knob, 7> knobs;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrangrandMAAudioProcessorEditor)
};
