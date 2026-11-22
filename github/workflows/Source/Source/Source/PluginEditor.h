#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "PluginProcessor.h"

class YANGEQAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    YANGEQAudioProcessorEditor (YANGEQAudioProcessor&);
    ~YANGEQAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    YANGEQAudioProcessor& audioProcessor;

    juce::Slider lowCutSlider, lmFreqSlider, lmGainSlider, lmQSlider, hmFreqSlider, hmGainSlider, hmQSlider, highCutSlider;
    juce::Label  lowCutLabel, lmFreqLabel, lmGainLabel, lmQLabel, hmFreqLabel, hmGainLabel, hmQLabel, highCutLabel;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> lowCutAtt, lmFreqAtt, lmGainAtt, lmQAtt, hmFreqAtt, hmGainAtt, hmQAtt, highCutAtt;

    void setupKnob(juce::Slider& slider, juce::Label& label, const juce::String& text);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (YANGEQAudioProcessorEditor)
};
