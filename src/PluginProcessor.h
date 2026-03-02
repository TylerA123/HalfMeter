#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include <atomic>

extern "C" {
#include <ebur128.h>
}

class HalfMeterAudioProcessor final : public juce::AudioProcessor
{
public:
  HalfMeterAudioProcessor();
  ~HalfMeterAudioProcessor() override;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;
  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  const juce::String getName() const override;
  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String& newName) override;

  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;

  float getSamplePeakDbfs(int channel) const noexcept;

  float getMomentaryLufs() const noexcept;
  float getShortTermLufs() const noexcept;
  float getIntegratedLufs() const noexcept;
  float getLraLu() const noexcept;

  float getTruePeakDbtp(int channel) const noexcept;

private:
  void recreateEburStateIfNeeded(double sampleRate, int channels);

  std::atomic<float> samplePeakDbfsL { -100.0f };
  std::atomic<float> samplePeakDbfsR { -100.0f };

  std::atomic<float> momentaryLufs { -100.0f };
  std::atomic<float> shortTermLufs { -100.0f };
  std::atomic<float> integratedLufs { -100.0f };
  std::atomic<float> lraLu { 0.0f };

  std::atomic<float> truePeakDbtpL { -100.0f };
  std::atomic<float> truePeakDbtpR { -100.0f };

  ebur128_state* ebur = nullptr;
  unsigned long eburSampleRate = 0;
  unsigned int eburChannels = 0;

  juce::HeapBlock<float> interleaved;
  int interleavedCapacityFrames = 0;

  int framesSinceLoudnessUpdate = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HalfMeterAudioProcessor)
};
