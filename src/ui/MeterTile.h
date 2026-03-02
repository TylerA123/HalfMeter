#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <algorithm>

class MeterTile final : public juce::Component
{
public:
  enum class Scale
  {
    Lufs,
    Dbfs,
    DbtpStereo
  };

  MeterTile(juce::String title, Scale scale);

  void setSingleValue(float v);
  void setStereoValues(float left, float right);

  void paint(juce::Graphics&) override;

private:
  float valueToNorm(float v) const noexcept;
  juce::String formatValue(float v) const;

  juce::String title;
  Scale scale;

  float singleValue = -100.0f;
  float leftValue = -100.0f;
  float rightValue = -100.0f;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MeterTile)
};
