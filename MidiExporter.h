#pragma once
#include <JuceHeader.h>
#include "PatternLibrary.h"
#include "RhythmEngine.h"
#include "ChordEngine.h"
class MidiExporter{
public:
    struct Options{ double sampleRate=48000.0; double tempoBPM=120.0; int bars=2; int tsNum=6, tsDen=8; float swing=0.12f, humanMs=6.0f, strumMs=8.0f; int semitoneTranspose=0; };
    struct PackOptions{ std::vector<double> tempos{90,100,110,120,130}; std::vector<int> bars{2}; std::vector<float> swings{0.0f,0.12f,0.2f}; std::vector<float> humans{0.0f,6.0f,12.0f}; std::vector<float> strums{4.0f,8.0f,12.0f}; std::vector<int> trans{0,+2,-2}; int maxFiles=1000; int tsNum=6, tsDen=8; };
    static bool exportPattern(const Pattern& pat, const juce::File& outFile, const Options& opt, const Chord& chord=Chord());
    static bool exportAll(const PatternLibrary& lib, const juce::File& dstDir, const Options& opt, const Chord& chord=Chord());
    static bool exportPack(const PatternLibrary& lib, const juce::File& dstDir, const PackOptions& pack, const Chord& chord=Chord());
private:
    static void renderToSequence(const Pattern& pat, const Options& opt, const Chord& chord, juce::MidiMessageSequence& outSeq);
};
