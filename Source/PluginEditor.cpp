#include "PluginEditor.h"

GrangrandMALookAndFeel::GrangrandMALookAndFeel()
{
    setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffeadfc8));
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff241e26));
}

void GrangrandMALookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                              float position, float startAngle, float endAngle, juce::Slider&)
{
    const auto bounds = juce::Rectangle<float>(static_cast<float>(x), static_cast<float>(y),
                                                static_cast<float>(width), static_cast<float>(height)).reduced(9.0f);
    const auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const auto centre = bounds.getCentre();
    const auto angle = startAngle + position * (endAngle - startAngle);

    g.setColour(juce::Colour(0xff332b35));
    g.fillEllipse(bounds);
    g.setColour(juce::Colour(0xff5a4b59));
    g.drawEllipse(bounds, 2.0f);

    juce::Path arc;
    arc.addCentredArc(centre.x, centre.y, radius + 5.0f, radius + 5.0f, 0.0f, startAngle, angle, true);
    g.setColour(juce::Colour(0xffe39763));
    g.strokePath(arc, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path pointer;
    pointer.addRoundedRectangle(-2.0f, -radius + 8.0f, 4.0f, radius * 0.48f, 2.0f);
    g.setColour(juce::Colour(0xffffd29e));
    g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(centre.x, centre.y));
}

GrangrandMAAudioProcessorEditor::GrangrandMAAudioProcessorEditor(GrangrandMAAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&lookAndFeel);
    setupKnob(knobs[0], "size", "SIZE");
    setupKnob(knobs[1], "density", "DENSITY");
    setupKnob(knobs[2], "pitch", "PITCH");
    setupKnob(knobs[3], "position", "POSITION");
    setupKnob(knobs[4], "spray", "SPRAY");
    setupKnob(knobs[5], "feedback", "FEEDBACK");
    setupKnob(knobs[6], "mix", "MIX");
    setResizable(true, true);
    setResizeLimits(620, 330, 1100, 600);
    setSize(820, 410);
}

GrangrandMAAudioProcessorEditor::~GrangrandMAAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void GrangrandMAAudioProcessorEditor::setupKnob(Knob& knob, const juce::String& parameterID,
                                                const juce::String& labelText)
{
    knob.slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knob.slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 82, 22);
    if (const auto* parameter = audioProcessor.parameters.getParameter(parameterID))
        knob.slider.setDoubleClickReturnValue(true, parameter->convertFrom0to1(parameter->getDefaultValue()));
    knob.label.setText(labelText, juce::dontSendNotification);
    knob.label.setJustificationType(juce::Justification::centred);
    knob.label.setColour(juce::Label::textColourId, juce::Colour(0xffb9aeb8));
    knob.label.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    addAndMakeVisible(knob.slider);
    addAndMakeVisible(knob.label);
    knob.attachment = std::make_unique<Attachment>(audioProcessor.parameters, parameterID, knob.slider);
}

void GrangrandMAAudioProcessorEditor::paint(juce::Graphics& g)
{
    juce::ColourGradient background(juce::Colour(0xff18131a), 0.0f, 0.0f,
                                    juce::Colour(0xff291f29), 0.0f, static_cast<float>(getHeight()), false);
    g.setGradientFill(background);
    g.fillAll();

    g.setColour(juce::Colour(0xfff4d6b0));
    g.setFont(juce::FontOptions(29.0f, juce::Font::bold));
    g.drawText("GRANGRANDMA", 26, 18, getWidth() - 52, 38, juce::Justification::centredLeft);
    g.setColour(juce::Colour(0xff9f8997));
    g.setFont(juce::FontOptions(12.0f));
    g.drawText("LIVE INPUT GRANULATOR  •  4 SECOND MEMORY", 28, 53, getWidth() - 56, 22,
               juce::Justification::centredLeft);
    g.setColour(juce::Colour(0x22ffffff));
    g.drawLine(28.0f, 82.0f, static_cast<float>(getWidth() - 28), 82.0f);
}

void GrangrandMAAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().withTrimmedTop(92).reduced(20, 12);
    const int firstRowCount = 4;
    const int firstWidth = area.getWidth() / firstRowCount;
    auto top = area.removeFromTop(area.getHeight() / 2);
    for (int i = 0; i < firstRowCount; ++i)
    {
        auto cell = top.removeFromLeft(firstWidth);
        knobs[static_cast<size_t>(i)].label.setBounds(cell.removeFromTop(20));
        knobs[static_cast<size_t>(i)].slider.setBounds(cell.reduced(4));
    }

    const int secondWidth = area.getWidth() / 3;
    area.reduce(secondWidth / 3, 0);
    for (int i = 4; i < 7; ++i)
    {
        auto cell = area.removeFromLeft(secondWidth);
        knobs[static_cast<size_t>(i)].label.setBounds(cell.removeFromTop(20));
        knobs[static_cast<size_t>(i)].slider.setBounds(cell.reduced(4));
    }
}
