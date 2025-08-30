#include "KeySwitch.h"
using namespace juce;
static bool inRange(int n,int lo,int hi){ return n>=lo && n<=hi; }
bool KeySwitchRouter::handle(const MidiMessage& msg, bool& rhythmActive, int& styleIndex, int numStyles, int& patternIndex, int numPatternsInStyle, String* directStyleName, const StringArray* styleNames){
    if (!msg.isNoteOn()) return false; const int n=msg.getNoteNumber();
    if(n==map.startToggle){ rhythmActive=!rhythmActive; return true; }
    if(n==map.stop){ rhythmActive=false; return true; }
    if(n==map.nextStyle){ if(numStyles>0){ styleIndex=(styleIndex+1)%numStyles; patternIndex=0;} return true; }
    if(n==map.prevStyle){ if(numStyles>0){ styleIndex=(styleIndex-1+numStyles)%numStyles; patternIndex=0;} return true; }
    for(int i=0;i<7;i++) if(n==map.styleSel[i]){ if(styleNames&&i<styleNames->size()){ if(directStyleName) *directStyleName = styleNames->operator[](i); } else { styleIndex = jlimit(0, jmax(0,numStyles-1), i);} patternIndex=0; return true; }
    if(n==map.nextPattern){ if(numPatternsInStyle>0){ patternIndex=(patternIndex+1)%numPatternsInStyle; } return true; }
    if(n==map.prevPattern){ if(numPatternsInStyle>0){ patternIndex=(patternIndex-1+numPatternsInStyle)%numPatternsInStyle; } return true; }
    if(inRange(n,map.patternBankLo,map.patternBankHi)){ patternIndex=jlimit(0,numPatternsInStyle-1,n-map.patternBankLo); return true; }
    return false;
}
