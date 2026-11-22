#include "PluginProcessor.h"
#include "PluginEditor.h"

YANGEQAudioProcessor::YANGEQAudioProcessor()
    : AudioProcessor (BusesProperties()
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
}

YANGEQAudioProcessor::~YANGEQAudioProcessor() {}

const juce::String YANGEQAudioProcessor::getName() const { return "YANG EQ"; }
bool YANGEQAudioProcessor::acceptsMidi() const { return false; }
bool YANGEQAudioProcessor::producesMidi() const { return false; }
bool YANGEQAudioProcessor::isMidiEffect() const { return false; }
double YANGEQAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int YANGEQAudioProcessor::getNumPrograms() { return 1; }
int YANGEQAudioProcessor::getCurrentProgram() { return 0; }
void YANGEQAudioProcessor::setCurrentProgram (int index) {}
const juce::String YANGEQAudioProcessor::getProgramName (int index) { return {}; }
void YANGEQAudioProcessor::changeProgramName (int index, const juce::String& newName) {}

void YANGEQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    updateFilters();
}

void YANGEQAudioProcessor::releaseResources() {}

bool YANGEQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void YANGEQAudioProcessor::updateFilters()
{
    auto sampleRate = getSampleRate();
    if (sampleRate <= 0) return;

    float lowCutFreq = apvts.getRawParameterValue("LOWCUT_FREQ")->load();
    auto lowCutCoeffs = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(lowCutFreq, sampleRate, 2);
    *leftChain.get<LowCut>().get<0>().coefficients = *lowCutCoeffs[0];
    *rightChain.get<LowCut>().get<0>().coefficients = *lowCutCoeffs[0];

    float lmFreq = apvts.getRawParameterValue("LM_FREQ")->load();
    float lmGain = apvts.getRawParameterValue("LM_GAIN")->load();
    float lmQ    = apvts.getRawParameterValue("LM_Q")->load();
    auto lmCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, lmFreq, lmQ, juce::Decibels::decibelsToGain(lmGain));
    *leftChain.get<LowMid>().coefficients = *lmCoeffs;
    *rightChain.get<LowMid>().coefficients = *lmCoeffs;

    float hmFreq = apvts.getRawParameterValue("HM_FREQ")->load();
    float hmGain = apvts.getRawParameterValue("HM_GAIN")->load();
    float hmQ    = apvts.getRawParameterValue("HM_Q")->load();
    auto hmCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, hmFreq, hmQ, juce::Decibels::decibelsToGain(hmGain));
    *leftChain.get<HighMid>().coefficients = *hmCoeffs;
    *rightChain.get<HighMid>().coefficients = *hmCoeffs;

    float highCutFreq = apvts.getRawParameterValue("HIGHCUT_FREQ")->load();
    auto highCutCoeffs = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(highCutFreq, sampleRate, 2);
    *leftChain.get<HighCut>().get<0>().coefficients = *highCutCoeffs[0];
    *rightChain.get<HighCut>().get<0>().coefficients = *highCutCoeffs[0];
}

void YANGEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    updateFilters();

    juce::dsp::AudioBlock<float> block(buffer);
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);
}

bool YANGEQAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* YANGEQAudioProcessor::createEditor() { return new YANGEQAudioProcessorEditor (*this); }

void YANGEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void YANGEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessorValueTreeState::ParameterLayout YANGEQAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>("LOWCUT_FREQ", "Low Cut Freq", juce::NormalisableRange<float>(20.f, 500.f, 1.f, 0.3f), 20.f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("LM_FREQ", "Low-Mid Freq", juce::NormalisableRange<float>(100.f, 1500.f, 1.f, 0.3f), 350.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LM_GAIN", "Low-Mid Gain", juce::NormalisableRange<float>(-18.f, 18.f, 0.1f), 0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LM_Q",    "Low-Mid Q",    juce::NormalisableRange<float>(0.3f, 10.f, 0.05f), 1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("HM_FREQ", "High-Mid Freq", juce::NormalisableRange<float>(1000.f, 8000.f, 1.f, 0.3f), 2500.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("HM_GAIN", "High-Mid Gain", juce::NormalisableRange<float>(-18.f, 18.f, 0.1f), 0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("HM_Q",    "High-Mid Q",    juce::NormalisableRange<float>(0.3f, 10.f, 0.05f), 1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("HIGHCUT_FREQ", "High Cut Freq", juce::NormalisableRange<float>(2000.f, 20000.f, 1.f, 0.3f), 20000.f));

    return layout;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new YANGEQAudioProcessor(); }
