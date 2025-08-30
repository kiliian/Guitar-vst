#include <JuceHeader.h>
#include "ChordEngine.h"
#include "PatternLibrary.h"
#include "RhythmEngine.h"
#include "KeySwitch.h"

class GuitarEditor; // fwd

class GuitarProcessor  : public juce::AudioProcessor {
public:
    GuitarProcessor() : apvts(*this, nullptr, "PARAMS", createLayout()){ patLib.initDefaults(); }
    const juce::String getName() const override { return "SWAM-Style Electric Guitar Pro"; }
    bool acceptsMidi() const override { return true; } bool producesMidi() const override { return true; }
    double getTailLengthSeconds() const override { return 0.0; }
    bool isBusesLayoutSupported (const BusesLayout&) const override { return true; }
    int getNumPrograms() override { return 1; } int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {} const juce::String getProgramName (int) override { return {}; } void changeProgramName (int, const juce::String&) override {}
    void prepareToPlay (double sampleRate, int) override { rhythm.prepare(sampleRate); rhythm.setLibrary(&patLib); }
    void releaseResources() override {}
    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) override {
        buffer.clear();
        juce::AudioPlayHead::CurrentPositionInfo pos; if (auto* ph=getPlayHead()) ph->getCurrentPosition(pos); lastPos = pos;
        rhythm.setTempo(pos.bpm>0?pos.bpm:120.0); rhythm.setTimeSig(pos.timeSigNumerator>0?pos.timeSigNumerator:4, pos.timeSigDenominator>0?pos.timeSigDenominator:4); rhythm.setPlaying(pos.isPlaying||pos.isRecording);
        const bool rhythmActive = apvts.getRawParameterValue("rhythm")->load() > 0.5f;
        rhythm.setActive(rhythmActive || uiRhythmActive); rhythm.setHumanizeMs(*apvts.getRawParameterValue("human")); rhythm.setSwing(*apvts.getRawParameterValue("swing")); rhythm.setStrumMs(*apvts.getRawParameterValue("strum"));
        auto styles = patLib.allStyles(); if (uiStyleIndex >= styles.size()) uiStyleIndex = 0; auto pats = styles.isEmpty()? juce::StringArray{} : patLib.patternsFor(styles[uiStyleIndex]); if (uiPatternIndex >= pats.size()) uiPatternIndex = 0; if (pats.size()>0) rhythm.setPatternId(pats[uiPatternIndex]);
        juce::MidiBuffer filtered; for (const auto meta : midi){ auto m=meta.getMessage(); if (ks.handle(m, uiRhythmActive, uiStyleIndex, styles.size(), uiPatternIndex, pats.size())) continue; if (m.isNoteOn()) chord.noteOn(m.getNoteNumber()); if (m.isNoteOff()) chord.noteOff(m.getNoteNumber()); filtered.addEvent(m, meta.samplePosition); }
        rhythm.setChord(chord.getCurrent());
        juce::MidiBuffer patMidi; rhythm.generate(buffer.getNumSamples(), patMidi); for (const auto meta : patMidi) filtered.addEvent(meta.getMessage(), meta.samplePosition); midi.swapWith(filtered);
    }
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    void getStateInformation (juce::MemoryBlock& dest) override { if (auto s=apvts.copyState()) if (auto xml=s.createXml()) juce::copyXmlToBinary(*xml, dest); }
    void setStateInformation (const void* data, int size) override { if (auto xml=juce::getXmlFromBinary(data,size)) apvts.replaceState(juce::ValueTree::fromXml(*xml)); }

    // === Helpers for Editor ===
    juce::AudioProcessorValueTreeState& getVTS(){ return apvts; }
    PatternLibrary& getLibrary(){ return patLib; }
    Chord getChord() const { return chord.getCurrent(); }

    juce::StringArray getStyleNames() const { return patLib.allStyles(); }
    juce::StringArray getPatternIdsFor(int styleIdx) const {
        auto s = patLib.allStyles(); if (styleIdx < 0 || styleIdx >= s.size()) return {};
        return patLib.patternsFor(s[styleIdx]);
    }
    void setStyleIndex(int i){ uiStyleIndex = juce::jlimit(0, juce::jmax(0, getStyleNames().size()-1), i); uiPatternIndex = 0; }
    void setPatternIndex(int i){ auto pats = getPatternIdsFor(uiStyleIndex); if (pats.size()==0) return; uiPatternIndex = juce::jlimit(0, pats.size()-1, i); }
    void setPatternById(const juce::String& id){
        auto s = getStyleNames(); if (uiStyleIndex >= s.size()) return;
        auto pats = patLib.patternsFor(s[uiStyleIndex]); for (int i=0;i<pats.size();++i) if (pats[i]==id){ uiPatternIndex=i; break; }
    }
    void cycleStyle(bool next){ auto s=getStyleNames(); if (s.size()==0) return; uiStyleIndex = (uiStyleIndex + (next?1:-1) + s.size()) % s.size(); uiPatternIndex = 0; }
    int getStyleIndex() const { return uiStyleIndex; }
    int getPatternIndex() const { return uiPatternIndex; }

    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout(){
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
        p.push_back(std::make_unique<juce::AudioParameterBool>("rhythm","RhythmActive",false));
        p.push_back(std::make_unique<juce::AudioParameterFloat>("human","HumanizeMs",0.0f,30.0f,6.0f));
        p.push_back(std::make_unique<juce::AudioParameterFloat>("swing","Swing",0.0f,0.5f,0.12f));
        p.push_back(std::make_unique<juce::AudioParameterFloat>("strum","StrumMs",0.0f,30.0f,8.0f));
        p.push_back(std::make_unique<juce::AudioParameterFloat>("stiff","Stiffness",0.2f,2.5f,1.0f));
p.push_back(std::make_unique<juce::AudioParameterFloat>("fdlslope","FDLossSlope",0.0f,1.0f,0.50f));
p.push_back(std::make_unique<juce::AudioParameterFloat>("tension","TensionSens",0.0f,1.0f,0.25f));
p.push_back(std::make_unique<juce::AudioParameterFloat>("nut","NutDamp",0.0f,1.0f,0.00f));
return { p.begin(), p.end() };
    }

public:
    // Host info helpers for Editor
    double getHostBpm() const { return lastPos.bpm>0? lastPos.bpm : 120.0; }
    juce::String getHostTimeSigString() const {
        int n = lastPos.timeSigNumerator>0? lastPos.timeSigNumerator:4;
        int d = lastPos.timeSigDenominator>0? lastPos.timeSigDenominator:4;
        return juce::String(n)+"/"+juce::String(d);
    }
    bool isHostPlaying() const { return lastPos.isPlaying || lastPos.isRecording; }
private:

    juce::AudioProcessorValueTreeState apvts;
    ChordDetector chord; PatternLibrary patLib; RhythmEngine rhythm; KeySwitchRouter ks; KeySwitchMap ksMap;
    int uiStyleIndex{0}, uiPatternIndex{0}; bool uiRhythmActive{false};
    juce::AudioPlayHead::CurrentPositionInfo lastPos;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GuitarProcessor)
};

#include "Editor.h"
juce::AudioProcessorEditor* GuitarProcessor::createEditor(){ return new GuitarEditor(*this); }

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){ return new GuitarProcessor(); }
