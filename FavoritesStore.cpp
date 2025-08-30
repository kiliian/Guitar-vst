#include "FavoritesStore.h"
using namespace juce;

static File statePath(){
    auto dir = File::getSpecialLocation(File::userApplicationDataDirectory).getChildFile("SWAMStyleGuitarPro");
    dir.createDirectory();
    return dir.getChildFile("browser_state.json");
}

FavoritesStore::FavoritesStore() : stateFile(statePath()){ load(); }
void FavoritesStore::load(){
    favs.clear(); recents.clear();
    if (!stateFile.existsAsFile()) return;
    var v = JSON::parse(stateFile);
    if (auto* o = v.getDynamicObject()){
        if (auto* fa = o->getProperty("favorites").getArray())
            for (auto& x : *fa) favs.add((String)x.toString());
        if (auto* ra = o->getProperty("recents").getArray())
            for (auto& x : *ra) recents.add((String)x.toString());
    }
}
void FavoritesStore::save() const{
    DynamicObject::Ptr o = new DynamicObject();
    var fa; for (auto& s : favs) fa.append(s);
    var ra; for (auto& s : recents) ra.append(s);
    o->setProperty("favorites", fa);
    o->setProperty("recents", ra);
    stateFile.replaceWithText(JSON::toString(var(o.get())));
}
bool FavoritesStore::isFav(const String& id) const { return favs.contains(id); }
void FavoritesStore::toggleFav(const String& id){ if (favs.contains(id)) favs.removeString(id); else favs.addIfNotAlreadyThere(id); save(); }
void FavoritesStore::pushRecent(const String& id){ recents.removeString(id); recents.insert(0,id); recents.removeRange(64, recents.size()); save(); }
StringArray FavoritesStore::getFavs() const { return favs; }
StringArray FavoritesStore::getRecents() const { return recents; }
