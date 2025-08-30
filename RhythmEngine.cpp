#include "RhythmEngine.h"
using namespace juce;
Array<int> RhythmEngine::defaultVoicing(const Chord& c){
    Array<int> tones = c.tones.isEmpty()? Array<int>{48,52,55} : c.tones; Array<int> out; out.ensureStorageAllocated(6);
    int base=40; for(int s=0;s<6;++s){ int idx=jmin(s, tones.size()-1); int n=tones[idx]; while(n<base+s*3)n+=12; while(n>76)n-=12; out.add(n);} std::sort(out.begin(), out.end()); return out;
}
void RhythmEngine::emitStrum(const Pattern& pat, const PatternStep& ps, MidiBuffer& out, int sampleOffset){
    auto notes=defaultVoicing(chord); const int lo=jlimit(0,5,ps.lowString), hi=jlimit(0,5,ps.highString); const bool up=(ps.dir=='U'); const int cnt=hi-lo+1;
    const int perString=(int) jmax(0.0f, strumMs*0.001f*(float)sr/(float)jmax(1,cnt-1));
    for(int i=0;i<cnt;i++){ int idx=up?(hi-i):(lo+i); int midi=jlimit(0,127, notes[idx]); int ch=idx+1; int jitter=(int)((Random::getSystemRandom().nextFloat()*2.0f-1.0f)*(humanizeMs*0.001f*sr));
        int when=sampleOffset+i*perString+jitter; when=jmax(0,when); uint8 vel=(uint8) jlimit(1,127,(int)std::round(ps.vel*110.0f+(ps.accent?15:0)));
        out.addEvent(MidiMessage::noteOn(ch,midi,vel/127.0f), when); out.addEvent(MidiMessage::noteOff(ch,midi), when+(int)(0.12f*sr)); }
}
void RhythmEngine::generate (int numSamples, MidiBuffer& out){
    if(!active||!playing||library==nullptr||!chord.isValid()) return; const Pattern* pat=library->get(patternId); if(!pat) return;
    const double spb=(60.0/tempoBPM)*sr; const double stepsPerBeat=pat->stepsPerBar/(double)tsNum; const double sps=spb/stepsPerBeat;
    int remaining=numSamples, cursor=0; while(remaining>0){ const int toNext=(int)(sps-stepSamplesElapsed); const int blk=jmin(remaining,toNext);
        stepSamplesElapsed+=blk; cursor+=blk; remaining-=blk; if(stepSamplesElapsed>=sps-1){ const int local=stepCounter%pat->stepsPerBar; int swingOffset=0;
            if(pat->swingable&&(local%2==1)) swingOffset=(int)(swing*(sps*0.5)); for(auto& st:pat->steps){ if(st.step==local) emitStrum(*pat, st, out, jmax(0, cursor + swingOffset - blk)); }
            stepSamplesElapsed=0; ++stepCounter; } }
}
