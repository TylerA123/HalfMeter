#include "MeterTile.h"

#include <cmath>

namespace
{
const juce::Colour kTileBg { 0xFF12151B };
const juce::Colour kTileStroke { 0xFF232833 };
const juce::Colour kLabel { 0xFFAAB2BF };
const juce::Colour kValue { 0xFFE9EDF3 };
const juce::Colour kMeter { 0xFFFF2A2A };
const juce::Colour kTicks { 0xFF2A3140 };

float clamp01(float x) noexcept
{
  return std::max(0.0f, std::min(1.0f, x));
}
}

MeterTile::MeterTile(juce::String t, Scale s)
  : title(std::move(t)),
    scale(s)
{
  setInterceptsMouseClicks(false, false);
}

void MeterTile::setSingleValue(float v)
{
  singleValue = v;
}

void MeterTile::setStereoValues(float left, float right)
{
  leftValue = left;
  rightValue = right;
}

float MeterTile::valueToNorm(float v) const noexcept
{
  float minV = -36.0f;
  float maxV = 0.0f;

  if (scale == Scale::Dbfs)
  {
    minV = -60.0f;
    maxV = 0.0f;
  }
  else if (scale == Scale::DbtpStereo)
  {
    minV = -12.0f;
    maxV = 3.0f;
  }

  return clamp01((v - minV) / (maxV - minV));
}

juce::String MeterTile::formatValue(float v) const
{
  if (v <= -99.0f)
    return "-inf";

  if (scale == Scale::Lufs)
    return juce::String(v, 1) + " LUFS";
  if (scale == Scale::DbtpStereo)
    return juce::String(v, 1) + " dBTP";
  return juce::String(v, 1) + " dBFS";
}

void MeterTile::paint(juce::Graphics& g)
{
  auto b = getLocalBounds().toFloat();
  const float radius = 12.0f;

  g.setColour(kTileBg);
  g.fillRoundedRectangle(b, radius);
  g.setColour(kTileStroke);
  g.drawRoundedRectangle(b.reduced(0.5f), radius, 1.0f);

  auto content = getLocalBounds().reduced(14);

  // Value readout (top)
  g.setColour(kValue);
  g.setFont(juce::Font(18.0f));

  if (scale == Scale::DbtpStereo)
  {
    const float maxTp = std::max(leftValue, rightValue);
    g.drawFittedText(formatValue(maxTp), content.removeFromTop(24), juce::Justification::centredLeft, 1);
  }
  else
  {
    g.drawFittedText(formatValue(singleValue), content.removeFromTop(24), juce::Justification::centredLeft, 1);
  }

  content.removeFromTop(10);

  // Meter area
  auto meterArea = content.removeFromTop(content.getHeight() - 26);
  meterArea.removeFromBottom(6);

  // Subtle ticks
  g.setColour(kTicks);
  for (int i = 1; i <= 4; ++i)
  {
    const float y = juce::jmap((float) i / 5.0f, (float) meterArea.getBottom(), (float) meterArea.getY());
    g.drawLine((float) meterArea.getX(), y, (float) meterArea.getRight(), y, 1.0f);
  }

  if (scale == Scale::DbtpStereo)
  {
    auto leftArea = meterArea.removeFromLeft(meterArea.getWidth() / 2).reduced(6, 0);
    auto rightArea = meterArea.reduced(6, 0);

    const float nl = valueToNorm(leftValue);
    const float nr = valueToNorm(rightValue);

    const int lh = (int) std::round(leftArea.getHeight() * nl);
    const int rh = (int) std::round(rightArea.getHeight() * nr);

    g.setColour(kMeter);
    g.fillRect(leftArea.withY(leftArea.getBottom() - lh).withHeight(lh));
    g.fillRect(rightArea.withY(rightArea.getBottom() - rh).withHeight(rh));
  }
  else
  {
    const float n = valueToNorm(singleValue);
    const int h = (int) std::round(meterArea.getHeight() * n);

    g.setColour(kMeter);
    g.fillRect(meterArea.withY(meterArea.getBottom() - h).withHeight(h));
  }

  // Label (bottom)
  g.setColour(kLabel);
  g.setFont(juce::Font(14.0f));
  g.drawFittedText(title, content, juce::Justification::centred, 1);
}
