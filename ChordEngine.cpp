#include "ChordEngine.h"
using namespace juce;
void ChordDetector::noteOn(int n){ if (isPositiveAndBelow(n,128)) held.setBit(n); }
void ChordDetector::noteOff(int n){ if (isPositiveAndBelow(n,128)) held.clearBit(n); }
void ChordDetector::clear(){ held.clear(); }
Array<int> ChordDetector::getHeldNotes() const { Array<int> a; int n=held.findNextSetBit(0); while(n>=0){ a.add(n); n=held.findNextSetBit(n+1);} return a; }
static const char* pcName[12]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
Chord ChordDetector::getCurrent() const {
    auto notes=getHeldNotes(); if (notes.isEmpty()) return {};
    int hist[12]{}; for(auto n:notes) ++hist[((n%12)+12)%12]; int bassPc=((notes.getFirst()%12)+12)%12;
    struct Pat{ const int* pcs; int cnt; String lab; };
    static const int maj[3]={0,4,7}, min_[3]={0,3,7}, sus2[3]={0,2,7}, sus4[3]={0,5,7}, dom7[4]={0,4,7,10}, maj7[4]={0,4,7,11}, min7[4]={0,3,7,10};
    Pat pats[]={{maj,3,"maj"},{min_,3,"min"},{sus2,3,"sus2"},{sus4,3,"sus4"},{dom7,4,"7"},{maj7,4,"maj7"},{min7,4,"m7"}};
    int bestRoot=-1,bestScore=-1; String bestLab; Array<int> pcsOut;
    for(int r=0;r<12;++r) for(auto& p:pats){ int sc=0; Array<int> pcs; for(int i=0;i<p.cnt;++i){ int want=(r+p.pcs[i])%12; pcs.add(want); sc += (hist[want]>0);} if(sc>bestScore){bestScore=sc; bestRoot=r; bestLab=p.lab; pcsOut=pcs;}}
    Chord c; c.root=bestRoot; c.bass=bassPc; c.name=String(pcName[bestRoot])+bestLab; if (bassPc!=bestRoot) c.name<<"/"<<pcName[bassPc];
    int mid=60; for(auto pc:pcsOut){ int m=mid-((mid%12)-pc); if(m<52)m+=12; if(m>76)m-=12; c.tones.add(m);} std::sort(c.tones.begin(), c.tones.end()); c.tones=Array<int>(c.tones); return c;
}
