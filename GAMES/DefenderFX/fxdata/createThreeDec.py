from midiutil import MIDIFile

# Create the MIDIFile Object
midi_file = MIDIFile(1)  # One track

# Add track name and tempo.
track = 0
time = 0
midi_file.addTrackName(track, time, "Sample Track")
midi_file.addTempo(track, time, 120)

# Add the notes in a three-note increasing scale
channel = 0
volume = 100
time = 0  # In beats
duration = 0.250  # In beats

# Add the notes
midi_file.addNote(track, channel, 57, time, duration, volume)  # Middle C
midi_file.addNote(track, channel, 55, time + 0.250, duration, volume)  # D
midi_file.addNote(track, channel, 53, time + 0.5, duration, volume)  # E

# Write it to disk
with open("../music/ThreeDecreasing.mid", "wb") as file:
    midi_file.writeFile(file)