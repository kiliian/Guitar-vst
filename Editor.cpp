#include "Editor.h"
#include "MidiExporter.h"

class GuitarProcessor; // forward

// We'll include minimal headers only; processor helpers are declared in Main.cpp
#include "Main.cpp"
using namespace juce;

GuitarEditor::GuitarEditor (GuitarProcessor& p) : AudioProcessorEditor(&p), ap(p)
{
    setSize(820, 540);
    addAndMakeVisible(title);
    title.setJustificationType(Justification::centredLeft);
    title.setFont(Font(18.0f, Font::bold));

    addAndMakeVisible(rhythmOn);
    rhythmAtt.reset(new AudioProcessorValueTreeState::ButtonAttachment(ap.getVTS(), "rhythm", rhythmOn));

    addAndMakeVisible(styleBox); addAndMakeVisible(filterBox);
    styleBox.onChange = [this]{ selectionChanged(0); refreshPatterns(); };
    filterBox.addItem("All", 1); filterBox.addItem("★ Favorites", 2); filterBox.addItem("Recent", 3); filterBox.setSelectedId(1);
    filterBox.addListener(this);

    addAndMakeVisible(prevStyle); addAndMakeVisible(nextStyle);
    prevStyle.addListener(this); nextStyle.addListener(this);

    addAndMakeVisible(patternList);
    patternList.setRowHeight(24);

    addAndMakeVisible(favToggle); favToggle.addListener(this);
    addAndMakeVisible(exportMidi); exportMidi.addListener(this);
    addAndMakeVisible(exportPack); exportPack.addListener(this);

    refreshStyles();
    refreshPatterns();
    
// Physics knobs setup
auto setupKnob = [&](juce::Slider& s, juce::Label& l, const juce::String& txt){
    addAndMakeVisible(s); addAndMakeVisible(l);
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 56, 18);
    l.setText(txt, juce::dontSendNotification);
    l.setJustificationType(juce::Justification::centred);
};
setupKnob(stiffKnob,   stiffLbl,   "Stiffness");
setupKnob(fdlKnob,     fdlLbl,     "FD Loss");
setupKnob(tensionKnob, tensionLbl, "Tension");
setupKnob(nutKnob,     nutLbl,     "Nut Damp");

stiffAtt.reset  (new juce::AudioProcessorValueTreeState::SliderAttachment(ap.getVTS(), "stiff",    stiffKnob));
fdlAtt.reset    (new juce::AudioProcessorValueTreeState::SliderAttachment(ap.getVTS(), "fdlslope", fdlKnob));
tensionAtt.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(ap.getVTS(), "tension",  tensionKnob));
nutAtt.reset    (new juce::AudioProcessorValueTreeState::SliderAttachment(ap.getVTS(), "nut",      nutKnob));

// Host label
addAndMakeVisible(hostLbl);
hostLbl.setJustificationType(juce::Justification::centredRight);
hostLbl.setFont(juce::Font(14.0f, juce::Font::plain));

startTimerHz(10);

}

void GuitarEditor::paint(Graphics& g){
    g.fillAll(Colours::black);
    g.setColour(Colours::white.withAlpha(0.08f)); g.fillRoundedRectangle(getLocalBounds().toFloat(), 8.0f);
    g.setColour(Colours::white); g.drawFittedText("Pattern Browser", {20, 44, getWidth()-40, 20}, Justification::centredLeft, 1);
}

void GuitarEditor::resized(){
    auto r = getLocalBounds().reduced(12);
    auto row1 = r.removeFromTop(28);
    title.setBounds(row1.removeFromLeft(420));
    hostLbl.setBounds(row1.removeFromRight(160));
        rhythmOn.setBounds(row1.removeFromRight(100));

    auto row2 = r.removeFromTop(32);
    prevStyle.setBounds(row2.removeFromLeft(40));
    styleBox.setBounds(row2.removeFromLeft(260));
    nextStyle.setBounds(row2.removeFromLeft(40));
    filterBox.setBounds(row2.removeFromLeft(180));
    favToggle.setBounds(row2.removeFromLeft(60));
    exportPack.setBounds(row2.removeFromRight(180));
    exportMidi.setBounds(row2.removeFromRight(140));

    
// Reserve bottom area for physics knobs
auto knobsRow = r.removeFromBottom(120);
auto w = knobsRow.getWidth() / 4;
auto place = [&](juce::Slider& s, juce::Label& l, juce::Rectangle<int> slot){
    auto top = slot.removeFromTop(slot.getHeight()-18);
    s.setBounds(top.reduced(6)); l.setBounds(slot);
};
place(stiffKnob,   stiffLbl,   knobsRow.removeFromLeft(w));
place(fdlKnob,     fdlLbl,     knobsRow.removeFromLeft(w));
place(tensionKnob, tensionLbl, knobsRow.removeFromLeft(w));
place(nutKnob,     nutLbl,     knobsRow.removeFromLeft(w));

patternList.setBounds(r.reduced(2));

}

void GuitarEditor::refreshStyles(){
    styles = ap.getStyleNames();
    styleBox.clear(juce::dontSendNotification);
    for (int i=0;i<styles.size();++i) styleBox.addItem(styles[i], i+1);
    styleBox.setSelectedItemIndex(ap.getStyleIndex(), juce::sendNotification);
}

void GuitarEditor::refreshPatterns(){
    currentIds = ap.getPatternIdsFor(styleBox.getSelectedItemIndex());
    filteredIds.clear();
    const int filt = filterBox.getSelectedId();
    if (filt == 2){ auto favs = store.getFavs(); for (auto& id : currentIds) if (favs.contains(id)) filteredIds.add(id); }
    else if (filt == 3){ auto recs = store.getRecents(); for (auto& id : recs) if (currentIds.contains(id)) filteredIds.add(id); }
    else filteredIds = currentIds;

    patternList.updateContent();
    patternList.selectRow(ap.getPatternIndex());
}

void GuitarEditor::selectionChanged(int idx){
    ap.setStyleIndex(styleBox.getSelectedItemIndex());
    ap.setPatternIndex(idx);
}

void GuitarEditor::comboBoxChanged(ComboBox* box){
    if (box == &filterBox) refreshPatterns();
}

void GuitarEditor::buttonClicked(Button* b){
    if (b == &prevStyle){
        ap.cycleStyle(false);
        styleBox.setSelectedItemIndex(ap.getStyleIndex());
        refreshPatterns();
    } else if (b == &nextStyle){
        ap.cycleStyle(true);
        styleBox.setSelectedItemIndex(ap.getStyleIndex());
        refreshPatterns();
    } else if (b == &favToggle){
        int row = patternList.getSelectedRow();
        if (isPositiveAndBelow(row, filteredIds.size())){
            store.toggleFav(filteredIds[row]);
            refreshPatterns();
        }
    } else if (b == &exportMidi){
        File dir = File::getSpecialLocation(File::userDesktopDirectory).getChildFile("GuitarVST_MIDI");
        MidiExporter::Options opt; opt.tempoBPM = 110.0; opt.bars = 2; opt.tsNum = 6; opt.tsDen = 8; opt.swing = 0.12f; opt.humanMs = 6.0f; opt.strumMs = 8.0f;
        MidiExporter::exportAll(ap.getLibrary(), dir, opt, ap.getChord());
    } else if (b == &exportPack){
        File dir = File::getSpecialLocation(File::userDesktopDirectory).getChildFile("GuitarVST_MIDI_1000");
        MidiExporter::PackOptions pack; pack.maxFiles = 1000;
        MidiExporter::exportPack(ap.getLibrary(), dir, pack, ap.getChord());
    }
}

int GuitarEditor::getNumRows(){ return filteredIds.size(); }

void GuitarEditor::paintListBoxItem (int row, Graphics& g, int width, int height, bool sel){
    g.fillAll(sel ? Colours::darkgrey.brighter(0.1f) : Colours::darkgrey);
    if (row < filteredIds.size()){
        auto id = filteredIds[row];
        bool fav = store.isFav(id);
        g.setColour(Colours::white);
        g.drawFittedText(id, 8, 0, width-60, height, Justification::centredLeft, 1);
        g.setColour(fav ? Colours::yellow : Colours::grey);
        g.drawText("★", width-24, 0, 20, height, Justification::centred);
    }
}

void GuitarEditor::listBoxItemClicked (int row, const MouseEvent& e){
    if (!isPositiveAndBelow(row, filteredIds.size())) return;
    ap.setPatternById(filteredIds[row]);
    store.pushRecent(filteredIds[row]);
    patternList.repaintRow(row);
    if (e.mods.isRightButtonDown()){
        store.toggleFav(filteredIds[row]);
        patternList.repaintRow(row);
    }
}

void GuitarEditor::timerCallback(){
    
// Update host tempo/TS
hostLbl.setText(juce::String((int)std::round(ap.getHostBpm())) + " bpm | " + ap.getHostTimeSigString(), juce::dontSendNotification);

if (styleBox.getSelectedItemIndex() != ap.getStyleIndex()){

        styleBox.setSelectedItemIndex(ap.getStyleIndex(), dontSendNotification);
        refreshPatterns();
    }
}
