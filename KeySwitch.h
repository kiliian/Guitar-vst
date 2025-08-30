#pragma once
#include <JuceHeader.h>
struct KeySwitchMap{ int startToggle=36, stop=37, nextPattern=38, prevPattern=39, nextStyle=40, prevStyle=41, styleSel[7]={42,43,44,45,46,47,48}, patternBankLo=48, patternBankHi=59; };
class KeySwitchRouter{
public:
    void setMap(const KeySwitchMap& m){ map=m; }
    bool handle(const juce::MidiMessage& msg, bool& rhythmActive, int& styleIndex, int numStyles, int& patternIndex, int numPatternsInStyle, juce::String* directStyleName=nullptr, const juce::StringArray* styleNames=nullptr);
private: KeySwitchMap map{};
};
