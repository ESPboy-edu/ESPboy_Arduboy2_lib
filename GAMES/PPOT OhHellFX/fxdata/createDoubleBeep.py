from midiutil import MIDIFile

# Create the MIDIFile Object
midi_file = MIDIFile(1)  # One track

# Add track name and tempo.
track = 0
time = 0
midi_file.addTrackName(track, time, "Sample Track")
midi_file.addTempo(track, time, 240)

# Add the notes in a three-note increasing scale
channel = 0
volume = 80
time = 0  # In beats
duration = 0.25  # In beats

# Add the notes
midi_file.addNote(track, channel, 72, time, duration, volume)  # Middle C
midi_file.addNote(track, channel, 71, time + 0.35, duration, volume)  # D

# Write it to disk
with open("../music/DoubleBeepDown.mid", "wb") as file:
    midi_file.writeFile(file)