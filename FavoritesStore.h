#pragma once
#include <JuceHeader.h>
class FavoritesStore {
public:
    FavoritesStore();
    bool isFav(const juce::String& id) const;
    void toggleFav(const juce::String& id);
    void pushRecent(const juce::String& id);
    juce::StringArray getFavs()    const;
    juce::StringArray getRecents() const;
private:
    juce::File stateFile;
    juce::StringArray favs, recents;
    void load();
    void save() const;
};
