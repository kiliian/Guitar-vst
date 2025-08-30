#pragma once
#include <JuceHeader.h>
#include "ChordEngine.h"
#include "PatternLibrary.h"
class RhythmEngine{
public:
    void prepare(double sr_){ sr = sr_; }
    void setLibrary(const PatternLibrary* l){ library = l; }
    void setTempo(double bpm){ tempoBPM = juce::jmax(20.0, bpm); }
    void setTimeSig(int n,int d){ tsNum=n; tsDen=d; }
    void setPlaying(bool on){ playing=on; }
    void setActive(bool on){ active=on; stepCounter=0; stepSamplesElapsed=0; }
    void setPatternId(juce::String id){ patternId = std::move(id); }
    void setHumanizeMs(float v){ humanizeMs = juce::jlimit(0.0f,30.0f,v); }
    void setSwing(float v){ swing = juce::jlimit(0.0f,0.9f,v); }
    void setStrumMs(float v){ strumMs = juce::jlimit(0.0f,30.0f,v); }
    void setChord(const Chord& c){ chord = c; }
    void generate (int numSamples, juce::MidiBuffer& out);
private:
    double sr{48000.0}, tempoBPM{120.0}; int tsNum{4}, tsDen{4}; bool playing{false}, active{false};
    const PatternLibrary* library{nullptr}; juce::String patternId{"pop8_du"};
    float humanizeMs{6.0f}, swing{0.12f}, strumMs{8.0f};
    int stepCounter{0}, stepSamplesElapsed{0}; Chord chord;
    static juce::Array<int> defaultVoicing(const Chord& c);
    void emitStrum(const Pattern& pat, const PatternStep& ps, juce::MidiBuffer& out, int sampleOffset);
};
