#include "PatternLibrary.h"
using namespace juce;

static Pattern* mk(const String& id,const String& name,const String& st,int steps,std::initializer_list<PatternStep> ss){ auto* p=new Pattern(); p->id=id;p->name=name;p->style=st;p->stepsPerBar=steps;p->swingable=true; for(auto s:ss)p->steps.add(s); return p; }
static Pattern* mkSeq(const String& id,const String& name,const String& st,int steps,const char* seq,int lo,int hi,float palm=0.0f){
    jassert(steps==12); auto* p=new Pattern(); p->id=id;p->name=name;p->style=st;p->stepsPerBar=steps;p->swingable=true;
    for(int i=0;i<steps;i++){ char c=seq[i]; if(c=='-'||c==' ') continue; PatternStep ps; ps.step=i; ps.lowString=lo; ps.highString=hi; ps.palm=palm;
        switch(c){ case 'D': ps.dir='D'; ps.vel=0.9f; ps.accent=true; break; case 'd': ps.dir='D'; ps.vel=0.82f; break; case 'U': ps.dir='U'; ps.vel=0.8f; ps.accent=true; break; case 'u': ps.dir='U'; ps.vel=0.7f; break; case 'M': ps.dir='M'; ps.vel=0.55f; ps.accent=true; break; case 'm': ps.dir='M'; ps.vel=0.45f; break; default: continue; } p->steps.add(ps);} return p; }

void PatternLibrary::initDefaults(){
    pats.clear(true);
    // Global sets
    pats.add(mk("pop8_du","Pop Down/Up 8ths","Pop/Rock",8, {{0,'D',0,5,0.85f,false},{1,'U',1,5,0.70f,false},{2,'D',0,5,0.85f,false},{3,'U',1,5,0.70f,false},{4,'D',0,5,0.85f,true},{5,'U',1,5,0.70f,false},{6,'D',0,5,0.85f,false},{7,'U',1,5,0.70f,false}}));
    pats.add(mk("funk16","Funk 16ths","Funk",16, {{0,'D',2,5,0.9f,false},{2,'U',2,5,0.7f,false},{4,'D',1,5,0.85f,false},{6,'U',2,5,0.7f,false},{8,'D',0,5,0.95f,true},{10,'U',2,5,0.75f,false},{12,'D',1,5,0.85f,false},{14,'U',2,5,0.7f,false}}));
    pats.add(mk("jazz_swing","Jazz Swing Comp","Jazz",8, {{0,'D',1,5,0.9f,true},{3,'U',2,5,0.6f,false},{4,'D',1,4,0.8f,false},{7,'U',2,5,0.6f,false}}));
    pats.add(mk("metal_chug","Metal Chug 8ths","Metal",8, {{0,'D',0,2,0.95f,true},{1,'D',0,2,0.85f,true},{2,'D',0,2,0.92f,true},{3,'D',0,2,0.85f,true},{4,'D',0,2,0.95f,true},{5,'D',0,2,0.85f,true},{6,'D',0,2,0.92f,true},{7,'D',0,2,0.85f,true}}));
    pats.add(mk("travis8","Travis Picking","Fingerstyle",8, {{0,'D',3,3,0.8f,false},{1,'U',1,1,0.7f,false},{2,'D',4,4,0.8f,false},{3,'U',2,2,0.7f,false},{4,'D',3,3,0.8f,false},{5,'U',1,1,0.7f,false},{6,'D',4,4,0.8f,false},{7,'U',2,2,0.7f,false}}));
    pats.add(mk("bossa","Bossa Nova","Latin",8, {{0,'D',2,5,0.8f,false},{3,'U',3,5,0.7f,false},{4,'D',2,5,0.85f,false},{7,'U',3,5,0.7f,false}}));

    // Persian 6/8 subset
    pats.add(mkSeq("persian68_classic_01","Persian 6/8 Classic 01","Persian 6/8",12,"D-U-U-D-U-U-",0,5));
    pats.add(mkSeq("persian68_modern_01","Persian 6/8 Modern 01","Persian 6/8",12,"D-U-mUD-U-mU",1,5,0.05f));
    pats.add(mkSeq("persian68_bandari_03","Bandari 03","Persian 6/8",12,"DD-U-DD-U-",0,3,0.15f));
    pats.add(mkSeq("persian68_babakaram_03","Baba Karam 03","Persian 6/8",12,"D-UD-U-UD-",0,4,0.08f));
    pats.add(mkSeq("persian68_reng_02","Reng 02","Persian 6/8",12,"D-uU-D-uU-",1,5));
    pats.add(mkSeq("persian68_khaliji_03","Khaliji 03","Persian 6/8",12,"DD-u-DD-u-",0,3,0.18f));
}
juce::StringArray PatternLibrary::allStyles() const{ juce::StringArray s; for(auto* p:pats) if(!s.contains(p->style)) s.add(p->style); return s; }
juce::StringArray PatternLibrary::patternsFor(const juce::String& st) const{ juce::StringArray ids; for(auto* p:pats) if(p->style==st) ids.add(p->id); return ids; }
const Pattern* PatternLibrary::get(const juce::String& id) const{ for(auto* p:pats) if(p->id==id) return p; return nullptr; }
