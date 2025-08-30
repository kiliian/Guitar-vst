#pragma once
#include <JuceHeader.h>
struct Chord { int root=-1,bass=-1; juce::String name; juce::Array<int> tones; bool isValid() const { return root>=0; } };
class ChordDetector {
public:
    void noteOn(int n); void noteOff(int n); void clear();
    Chord getCurrent() const; juce::Array<int> getHeldNotes() const;
private: juce::BigInteger held; static int pc(int m){ return ((m%12)+12)%12; }
};
