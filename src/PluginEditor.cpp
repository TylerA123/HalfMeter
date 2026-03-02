#include "PluginEditor.h"
#include "ui/MeterTile.h"

#include <algorithm>

namespace
{
const juce::Colour kBg { 0xFF0B0D10 };
const juce::Colour kTitle { 0xFFE9EDF3 };
}

HalfMeterAudioProcessorEditor::HalfMeterAudioProcessorEditor(HalfMeterAudioProcessor& p)
  : AudioProcessorEditor(&p),
    processor(p)
{
  integratedTile = std::make_unique<MeterTile>("Integrated LUFS", MeterTile::Scale::Lufs);
  shortTermTile = std::make_unique<MeterTile>("Short-term LUFS", MeterTile::Scale::Lufs);
  masterTile = std::make_unique<MeterTile>("Master Volume", MeterTile::Scale::Dbfs);
  truePeakTile = std::make_unique<MeterTile>("True Peak", MeterTile::Scale::DbtpStereo);

  for (auto* t : { integratedTile.get(), shortTermTile.get(), masterTile.get(), truePeakTile.get() })
    addAndMakeVisible(t);

  setResizable(true, true);
  setSize(900, 500);

  startTimerHz(30);
}

HalfMeterAudioProcessorEditor::~HalfMeterAudioProcessorEditor() = default;

void HalfMeterAudioProcessorEditor::paint(juce::Graphics& g)
{
  g.fillAll(kBg);

  g.setColour(kTitle);
  g.setFont(juce::Font(32.0f));
  g.drawFittedText("Monitor", getLocalBounds().removeFromTop(70), juce::Justification::centred, 1);
}

void HalfMeterAudioProcessorEditor::resized()
{
  auto r = getLocalBounds();
  r.removeFromTop(90);
  r.reduce(24, 24);

  const int gap = 22;
  const int tileW = (r.getWidth() - gap * 3) / 4;
  const int tileH = juce::jmin(280, r.getHeight());
  const int y = r.getY() + (r.getHeight() - tileH) / 2;

  integratedTile->setBounds(r.getX(), y, tileW, tileH);
  shortTermTile->setBounds(r.getX() + tileW + gap, y, tileW, tileH);
  masterTile->setBounds(r.getX() + (tileW + gap) * 2, y, tileW, tileH);
  truePeakTile->setBounds(r.getX() + (tileW + gap) * 3, y, tileW, tileH);
}

void HalfMeterAudioProcessorEditor::timerCallback()
{
  const float peakL = processor.getSamplePeakDbfs(0);
  const float peakR = processor.getSamplePeakDbfs(1);
  const float peakMax = std::max(peakL, peakR);

  masterTile->setSingleValue(peakMax);

  integratedTile->setSingleValue(processor.getIntegratedLufs());
  shortTermTile->setSingleValue(processor.getShortTermLufs());

  truePeakTile->setStereoValues(processor.getTruePeakDbtp(0), processor.getTruePeakDbtp(1));

  repaint();
}
