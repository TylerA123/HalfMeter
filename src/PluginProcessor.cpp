#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <algorithm>
#include <cmath>

#include <limits>

namespace
{
float linearToDbfs(float x) noexcept
{
  constexpr float floorLinear = 1.0e-9f;
  x = std::max(std::abs(x), floorLinear);
  return 20.0f * std::log10(x);
}

float doubleToLufs(double x) noexcept
{
  if ((std::isinf(x) && x < 0.0) || !std::isfinite(x))
    return -100.0f;
  return (float) x;
}

float linearToDb(float x) noexcept
{
  if (!std::isfinite(x) || x <= 0.0)
    return -100.0f;
  return linearToDbfs(x);
}
}

HalfMeterAudioProcessor::HalfMeterAudioProcessor()
  : juce::AudioProcessor(
      juce::AudioProcessor::BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                            .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

HalfMeterAudioProcessor::~HalfMeterAudioProcessor()
{
  releaseResources();
}

const juce::String HalfMeterAudioProcessor::getName() const { return "HalfMeter"; }
bool HalfMeterAudioProcessor::acceptsMidi() const { return false; }
bool HalfMeterAudioProcessor::producesMidi() const { return false; }
bool HalfMeterAudioProcessor::isMidiEffect() const { return false; }
double HalfMeterAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int HalfMeterAudioProcessor::getNumPrograms() { return 1; }
int HalfMeterAudioProcessor::getCurrentProgram() { return 0; }
void HalfMeterAudioProcessor::setCurrentProgram(int) {}
const juce::String HalfMeterAudioProcessor::getProgramName(int) { return {}; }
void HalfMeterAudioProcessor::changeProgramName(int, const juce::String&) {}

void HalfMeterAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
  samplePeakDbfsL.store(-100.0f, std::memory_order_relaxed);
  samplePeakDbfsR.store(-100.0f, std::memory_order_relaxed);

  momentaryLufs.store(-100.0f, std::memory_order_relaxed);
  shortTermLufs.store(-100.0f, std::memory_order_relaxed);
  integratedLufs.store(-100.0f, std::memory_order_relaxed);
  lraLu.store(0.0f, std::memory_order_relaxed);

  truePeakDbtpL.store(-100.0f, std::memory_order_relaxed);
  truePeakDbtpR.store(-100.0f, std::memory_order_relaxed);

  framesSinceLoudnessUpdate = 0;

  const int ch = juce::jlimit(1, 2, getTotalNumInputChannels());
  recreateEburStateIfNeeded(sampleRate, ch);

  interleavedCapacityFrames = std::max(2048, samplesPerBlock);
  interleaved.allocate((size_t) interleavedCapacityFrames * (size_t) ch, true);
}

void HalfMeterAudioProcessor::releaseResources()
{
  if (ebur != nullptr)
    ebur128_destroy(&ebur);
  eburSampleRate = 0;
  eburChannels = 0;
}

void HalfMeterAudioProcessor::recreateEburStateIfNeeded(double sampleRate, int channels)
{
  const unsigned long sr = (unsigned long) std::llround(sampleRate);
  const unsigned int ch = (unsigned int) channels;

  if (ebur != nullptr && eburSampleRate == sr && eburChannels == ch)
    return;

  if (ebur != nullptr)
    ebur128_destroy(&ebur);

  const int mode = EBUR128_MODE_M | EBUR128_MODE_S | EBUR128_MODE_I | EBUR128_MODE_LRA |
                   EBUR128_MODE_TRUE_PEAK | EBUR128_MODE_HISTOGRAM;

  ebur = ebur128_init(ch, sr, mode);
  eburSampleRate = sr;
  eburChannels = ch;

  if (ebur == nullptr)
    return;

  if (ch == 1)
  {
    (void) ebur128_set_channel(ebur, 0, EBUR128_LEFT);
  }
  else
  {
    (void) ebur128_set_channel(ebur, 0, EBUR128_LEFT);
    (void) ebur128_set_channel(ebur, 1, EBUR128_RIGHT);
  }
}

bool HalfMeterAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
  const auto& mainIn = layouts.getMainInputChannelSet();
  const auto& mainOut = layouts.getMainOutputChannelSet();
  return (mainIn == mainOut) && (mainOut == juce::AudioChannelSet::mono() || mainOut == juce::AudioChannelSet::stereo());
}

void HalfMeterAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
  juce::ignoreUnused(midi);

  juce::ScopedNoDenormals noDenormals;

  const int numCh = juce::jlimit(1, 2, buffer.getNumChannels());
  const int numSmps = buffer.getNumSamples();

  float peakL = 0.0f;
  float peakR = 0.0f;

  if (numCh > 0)
  {
    const float* ch0 = buffer.getReadPointer(0);
    for (int i = 0; i < numSmps; ++i)
      peakL = std::max(peakL, std::abs(ch0[i]));
  }

  if (numCh > 1)
  {
    const float* ch1 = buffer.getReadPointer(1);
    for (int i = 0; i < numSmps; ++i)
      peakR = std::max(peakR, std::abs(ch1[i]));
  }
  else
  {
    peakR = peakL;
  }

  samplePeakDbfsL.store(linearToDbfs(peakL), std::memory_order_relaxed);
  samplePeakDbfsR.store(linearToDbfs(peakR), std::memory_order_relaxed);

  if (ebur == nullptr)
    return;

  const float* in0 = buffer.getReadPointer(0);
  const float* in1 = (numCh > 1) ? buffer.getReadPointer(1) : nullptr;

  int offset = 0;
  while (offset < numSmps)
  {
    const int frames = std::min(interleavedCapacityFrames, numSmps - offset);
    float* dst = interleaved.get();

    if (numCh == 1)
    {
      for (int i = 0; i < frames; ++i)
        dst[i] = in0[offset + i];
    }
    else
    {
      for (int i = 0; i < frames; ++i)
      {
        dst[i * 2 + 0] = in0[offset + i];
        dst[i * 2 + 1] = in1[offset + i];
      }
    }

    (void) ebur128_add_frames_float(ebur, dst, (size_t) frames);

    // True peak for the most recently processed frames.
    double tp0 = 0.0;
    double tp1 = 0.0;
    if (ebur128_prev_true_peak(ebur, 0, &tp0) == EBUR128_SUCCESS)
      truePeakDbtpL.store(linearToDb((float) tp0), std::memory_order_relaxed);

    if (numCh > 1)
    {
      if (ebur128_prev_true_peak(ebur, 1, &tp1) == EBUR128_SUCCESS)
        truePeakDbtpR.store(linearToDb((float) tp1), std::memory_order_relaxed);
    }
    else
    {
      truePeakDbtpR.store(truePeakDbtpL.load(std::memory_order_relaxed), std::memory_order_relaxed);
    }

    offset += frames;
    framesSinceLoudnessUpdate += frames;
  }

  // Update loudness at ~10 Hz.
  const int updateEveryFrames = (int) std::max(1.0, getSampleRate() / 10.0);
  if (framesSinceLoudnessUpdate >= updateEveryFrames)
  {
    framesSinceLoudnessUpdate = 0;

    double m = -HUGE_VAL;
    double s = -HUGE_VAL;
    double i = -HUGE_VAL;
    double lra = 0.0;

    (void) ebur128_loudness_momentary(ebur, &m);
    (void) ebur128_loudness_shortterm(ebur, &s);
    (void) ebur128_loudness_global(ebur, &i);
    (void) ebur128_loudness_range(ebur, &lra);

    momentaryLufs.store(doubleToLufs(m), std::memory_order_relaxed);
    shortTermLufs.store(doubleToLufs(s), std::memory_order_relaxed);
    integratedLufs.store(doubleToLufs(i), std::memory_order_relaxed);
    lraLu.store((float) (std::isfinite(lra) ? lra : 0.0), std::memory_order_relaxed);
  }
}

bool HalfMeterAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* HalfMeterAudioProcessor::createEditor()
{
  return new HalfMeterAudioProcessorEditor(*this);
}

void HalfMeterAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
  destData.setSize(0);
}

void HalfMeterAudioProcessor::setStateInformation(const void*, int) {}

float HalfMeterAudioProcessor::getSamplePeakDbfs(int channel) const noexcept
{
  if (channel == 0)
    return samplePeakDbfsL.load(std::memory_order_relaxed);
  return samplePeakDbfsR.load(std::memory_order_relaxed);
}

float HalfMeterAudioProcessor::getMomentaryLufs() const noexcept
{
  return momentaryLufs.load(std::memory_order_relaxed);
}

float HalfMeterAudioProcessor::getShortTermLufs() const noexcept
{
  return shortTermLufs.load(std::memory_order_relaxed);
}

float HalfMeterAudioProcessor::getIntegratedLufs() const noexcept
{
  return integratedLufs.load(std::memory_order_relaxed);
}

float HalfMeterAudioProcessor::getLraLu() const noexcept
{
  return lraLu.load(std::memory_order_relaxed);
}

float HalfMeterAudioProcessor::getTruePeakDbtp(int channel) const noexcept
{
  if (channel == 0)
    return truePeakDbtpL.load(std::memory_order_relaxed);
  return truePeakDbtpR.load(std::memory_order_relaxed);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
  return new HalfMeterAudioProcessor();
}
