# SWAM-Style Electric Guitar — Pro Edition v1.3 (with UI)

- Hybrid physical-modeling core (skeleton)
- Chord detector + 6-string voicing
- Rhythm engine with **Persian 6/8** patterns
- Keyswitch control (C1..B2)
- **Pattern Browser UI**, Favorites, **Export MIDI**, **Export 1000 Pack**

## Build
```bash
mkdir build && cd build
cmake -S .. -B . -DJUCE_DIR=/path/to/JUCE/cmake -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```
