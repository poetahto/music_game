# Music Game
An idea I had for a game, sort of like an RPG where the battles involve you playing music.

Probably want to include fmod? I want to do dynamic stuff.

A couple of challenging things here:

1. Translating mic inputs into pitches
2. Playback / sync on midi files (or some other rep for music?)
3. Evaluating mic inputs against a playing midi file (that one old learn-clarinet software)
4. Finding a natural way to combine the music playing and the gameplay

# Technical stuff
Since the concept is so experimental, I probably cant rely on libraries too much.

Rolling my own systems gives the most control, cross platform might be hard so start with windows.

Don't worry about graphics too much yet, just go for basic win32, dx11, imgui integration