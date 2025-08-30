#pragma once
#include <JuceHeader.h>
#include "FavoritesStore.h"
class GuitarProcessor;

class GuitarEditor : public juce::AudioProcessorEditor,
                     private juce::ComboBox::Listener,
                     private juce::Button::Listener,
                     private juce::ListBoxModel,
                     private juce::Timer
{
public:
    explicit GuitarEditor (GuitarProcessor& p);
    ~GuitarEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    GuitarProcessor& ap;

// Physics knobs
juce::Slider stiffKnob, fdlKnob, tensionKnob, nutKnob;
juce::Label  stiffLbl,  fdlLbl,  tensionLbl,  nutLbl;
std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> stiffAtt, fdlAtt, tensionAtt, nutAtt;

// Host indicators
juce::Label hostLbl{"host","-- bpm | --/--"};

    FavoritesStore store;

    juce::Label title{"title","SWAM-Style Electric Guitar Pro"};
    juce::ToggleButton rhythmOn{"Rhythm"};
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> rhythmAtt;

    juce::ComboBox styleBox, filterBox;
    juce::TextButton prevStyle{"<"}, nextStyle{">"};
    juce::ListBox patternList{"patterns", this};
    juce::TextButton favToggle{"★"}, exportMidi{"Export MIDI"}, exportPack{"Export 1000 Pack"};

    juce::StringArray styles, currentIds, filteredIds;

    void refreshStyles();
    void refreshPatterns();
    void selectionChanged(int newIdx);
    void comboBoxChanged(juce::ComboBox* box) override;
    void buttonClicked(juce::Button* b) override;
    int getNumRows() override;
    void paintListBoxItem (int row, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked (int row, const juce::MouseEvent& e) override;
    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarEditor)
};
