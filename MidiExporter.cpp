#include "MidiExporter.h"
using namespace juce;
static double samplesPerStep_6_8(double sr,double bpm,int stepsPerBar){ const double secPerBeat=60.0/jmax(1.0,bpm); const double barSec=secPerBeat*3.0; return (barSec/(double)stepsPerBar)*sr; }
static double jitterSeconds(Random& rng, float humanMs){ return (rng.nextFloat()*2.0f-1.0f)*(double)humanMs*0.001; }
void MidiExporter::renderToSequence(const Pattern& pat, const Options& opt, const Chord& chordIn, MidiMessageSequence& seq){
    const double sr=opt.sampleRate, sps=samplesPerStep_6_8(sr,opt.tempoBPM,pat.stepsPerBar); const int bars=jmax(1,opt.bars), total=pat.stepsPerBar*bars;
    Chord chord=chordIn; if(!chord.isValid()){ chord.root=0; chord.name="C"; chord.tones={48,52,55}; }
    Array<int> tones=chord.tones.isEmpty()?Array<int>{48,52,55}:chord.tones; Array<int> voice; int base=40; for(int s=0;s<6;++s){ int idx=jmin(s, tones.size()-1); int n=tones[idx]+opt.semitoneTranspose; while(n<base+s*3)n+=12; while(n>76)n-=12; voice.add(n);} std::sort(voice.begin(), voice.end());
    Random rng(0xB00B5); const double secPerBeat=60.0/opt.tempoBPM; seq.addEvent(MidiMessage::tempoMetaEvent((int)std::round(1e6*secPerBeat)),0.0);
    for(int st=0;st<total;++st){ const int local=st%pat.stepsPerBar;
        for(auto& ps:pat.steps){ if(ps.step!=local) continue; const double tBase=(st*sps)/sr; const bool up=(ps.dir=='U'); const int lo=jlimit(0,5,ps.lowString), hi=jlimit(0,5,ps.highString); const int cnt=hi-lo+1;
            const bool inBeat2=((local%6)>=3); const double swingShift=(inBeat2?(opt.swing*(sps/sr)*0.5):0.0); const double perStr=(cnt>1? (opt.strumMs*0.001)/(cnt-1) : 0.0);
            for(int i=0;i<cnt;i++){ const int idx=up?(hi-i):(lo+i); const int midi=jlimit(0,127, voice[idx]); const uint8 vel=(uint8) jlimit(1,127,(int)std::round(ps.vel*110.0f+(ps.accent?15:0)));
                const double onT=tBase+swingShift+(i*perStr)+jitterSeconds(rng,opt.humanMs); const double offT=onT+0.11; seq.addEvent(MidiMessage::noteOn(1+idx,midi,vel/127.0f), onT); seq.addEvent(MidiMessage::noteOff(1+idx,midi), offT); }
        }
    }
}
bool MidiExporter::exportPattern(const Pattern& pat, const File& outFile, const Options& opt, const Chord& chord){
    MidiMessageSequence seq; renderToSequence(pat,opt,chord,seq); MidiFile mf; mf.setTicksPerQuarterNote(960); mf.addTrack(seq); auto parent=outFile.getParentDirectory(); parent.createDirectory(); FileOutputStream fos(outFile); return fos.openedOk() && mf.writeTo(fos);
}
bool MidiExporter::exportAll(const PatternLibrary& lib, const File& dstDir, const Options& opt, const Chord& chord){
    if(!dstDir.createDirectory()) return false; bool ok=true; auto styles=lib.allStyles(); for(auto& st:styles){ auto ids=lib.patternsFor(st); File sub=dstDir.getChildFile(st.replaceCharacter('/', '-')); sub.createDirectory(); for(auto& id:ids){ const Pattern* p=lib.get(id); if(!p) continue; File out=sub.getChildFile(id + ".mid"); ok &= exportPattern(*p, out, opt, chord);} } return ok;
}
bool MidiExporter::exportPack(const PatternLibrary& lib, const File& dstDir, const PackOptions& pack, const Chord& chord){
    if(!dstDir.createDirectory()) return false; int written=0; bool ok=true; auto styles=lib.allStyles();
    for(auto& st:styles){ if(written>=pack.maxFiles) break; auto ids=lib.patternsFor(st);
        for(auto& id:ids){ if(written>=pack.maxFiles) break; const Pattern* p=lib.get(id); if(!p) continue;
            for(double bpm:pack.tempos){ if(written>=pack.maxFiles) break;
                for(int bars:pack.bars){ if(written>=pack.maxFiles) break;
                    for(float sw:pack.swings){ if(written>=pack.maxFiles) break;
                        for(float hm:pack.humans){ if(written>=pack.maxFiles) break;
                            for(float stms:pack.strums){ if(written>=pack.maxFiles) break;
                                for(int tr:pack.trans){ if(written>=pack.maxFiles) break;
                                    Options opt; opt.tempoBPM=bpm; opt.bars=bars; opt.tsNum=pack.tsNum; opt.tsDen=pack.tsDen; opt.swing=sw; opt.humanMs=hm; opt.strumMs=stms; opt.semitoneTranspose=tr;
                                    File sub=dstDir.getChildFile(st.replaceCharacter('/', '-')); sub.createDirectory(); String fname = id + String::formatted("__%dbpm_sw%.2f_hm%.0fms_st%.0fms_tr%+d.mid", (int)std::round(bpm), sw, hm, stms, tr);
                                    File out=sub.getChildFile(fname); ok &= exportPattern(*p, out, opt, chord); if(!ok) return false; if(++written>=pack.maxFiles) break;
                                }
                            }
                        }
                    }
                }
            }
        }
    } return ok && written>0;
}
