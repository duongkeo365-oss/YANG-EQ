#include "PluginProcessor.h"
#include "PluginEditor.h"

YANGEQAudioProcessorEditor::YANGEQAudioProcessorEditor (YANGEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setupKnob(lowCutSlider,  lowCutLabel,  "LOW CUT (Hz)");
    setupKnob(lmFreqSlider,  lmFreqLabel,  "LM FREQ");
    setupKnob(lmGainSlider,  lmGainLabel,  "LM GAIN (dB)");
    setupKnob(lmQSlider,     lmQLabel,     "LM Q");
    setupKnob(hmFreqSlider,  hmFreqLabel,  "HM FREQ");
    setupKnob(hmGainSlider,  hmGainLabel,  "HM GAIN (dB)");
    setupKnob(hmQSlider,     hmQLabel,     "HM Q");
    setupKnob(highCutSlider, highCutLabel, "HIGH CUT (Hz)");

    lowCutAtt  = std::make_unique<Attachment>(audioProcessor.apvts, "LOWCUT_FREQ",  lowCutSlider);
    lmFreqAtt  = std::make_unique<Attachment>(audioProcessor.apvts, "LM_FREQ",      lmFreqSlider);
    lmGainAtt  = std::make_unique<Attachment>(audioProcessor.apvts, "LM_GAIN",      lmGainSlider);
    lmQAtt     = std::make_unique<Attachment>(audioProcessor.apvts, "LM_Q",         lmQSlider);
    hmFreqAtt  = std::make_unique<Attachment>(audioProcessor.apvts, "HM_FREQ",      hmFreqSlider);
    hmGainAtt  = std::make_unique<Attachment>(audioProcessor.apvts, "HM_GAIN",      hmGainSlider);
    hmQAtt     = std::make_unique<Attachment>(audioProcessor.apvts, "HM_Q",         hmQSlider);
    highCutAtt = std::make_unique<Attachment>(audioProcessor.apvts, "HIGHCUT_FREQ", highCutSlider);

    setSize (680, 320);
}

YANGEQAudioProcessorEditor::~YANGEQAudioProcessorEditor() {}

void YANGEQAudioProcessorEditor::setupKnob(juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff00e5ff));
    slider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    label.setFont(juce::Font(12.0f, juce::Font::bold));
    addAndMakeVisible(label);
}

void YANGEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour(0xff141619));

    g.setColour(juce::Colour(0xff1f2328));
    g.fillRect(0, 0, getWidth(), 45);

    g.setColour (juce::Colour(0xff00e5ff));
    g.setFont (juce::Font (22.0f, juce::Font::bold));
    g.drawFittedText ("YANG EQ", 20, 0, 200, 45, juce::Justification::centredLeft, 1);

    g.setColour (juce::Colours::grey);
    g.setFont (juce::Font (12.0f));
    g.drawFittedText ("4-BAND PRECISION CUT EQUALIZER", getWidth() - 250, 0, 230, 45, juce::Justification::centredRight, 1);
}

void YANGEQAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    area.removeFromTop(45);

    auto topRow = area.removeFromTop(area.getHeight() / 2);
    auto bottomRow = area;

    int colWidth = area.getWidth() / 4;

    auto r1c1 = topRow.removeFromLeft(colWidth);
    lowCutLabel.setBounds(r1c1.removeFromTop(20));
    lowCutSlider.setBounds(r1c1);

    auto r1c2 = topRow.removeFromLeft(colWidth);
    lmFreqLabel.setBounds(r1c2.removeFromTop(20));
    lmFreqSlider.setBounds(r1c2);

    auto r1c3 = topRow.removeFromLeft(colWidth);
    hmFreqLabel.setBounds(r1c3.removeFromTop(20));
    hmFreqSlider.setBounds(r1c3);

    auto r1c4 = topRow;
    highCutLabel.setBounds(r1c4.removeFromTop(20));
    highCutSlider.setBounds(r1c4);

    bottomRow.removeFromLeft(colWidth);

    auto r2c2 = bottomRow.removeFromLeft(colWidth);
    auto lmGainArea = r2c2.removeFromLeft(colWidth / 2);
    lmGainLabel.setBounds(lmGainArea.removeFromTop(20));
    lmGainSlider.setBounds(lmGainArea);
    lmQLabel.setBounds(r2c2.removeFromTop(20));
    lmQSlider.setBounds(r2c2);

    auto r2c3 = bottomRow.removeFromLeft(colWidth);
    auto hmGainArea = r2c3.removeFromLeft(colWidth / 2);
    hmGainLabel.setBounds(hmGainArea.removeFromTop(20));
    hmGainSlider.setBounds(hmGainArea);
    hmQLabel.setBounds(r2c3.removeFromTop(20));
    hmQSlider.setBounds(r2c3);
}
