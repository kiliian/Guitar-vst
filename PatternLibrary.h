#pragma once
#include <JuceHeader.h>
struct PatternStep{ int step=0; char dir='D'; int lowString=0, highString=5; float vel=0.9f, palm=0.0f; bool accent=false; };
struct Pattern{ juce::String id,name,style; int stepsPerBar=16; bool swingable=true; juce::Array<PatternStep> steps; };
class PatternLibrary{
public: void initDefaults(); juce::StringArray allStyles() const; juce::StringArray patternsFor(const juce::String& s) const; const Pattern* get(const juce::String& id) const;
private: juce::OwnedArray<Pattern> pats;
};
