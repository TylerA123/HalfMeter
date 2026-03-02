#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

#include <memory>

class MeterTile;

class HalfMeterAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                            private juce::Timer
{
public:
  explicit HalfMeterAudioProcessorEditor(HalfMeterAudioProcessor&);
  ~HalfMeterAudioProcessorEditor() override;

  void paint(juce::Graphics&) override;
  void resized() override;

private:
  void timerCallback() override;

  HalfMeterAudioProcessor& processor;

  std::unique_ptr<MeterTile> integratedTile;
  std::unique_ptr<MeterTile> shortTermTile;
  std::unique_ptr<MeterTile> masterTile;
  std::unique_ptr<MeterTile> truePeakTile;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HalfMeterAudioProcessorEditor)
};
