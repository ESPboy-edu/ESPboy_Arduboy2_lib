from midiutil import MIDIFile

# Create the MIDIFile Object
midi_file = MIDIFile(1)  # One track

# Add track name and tempo.
track = 0
time = 0
midi_file.addTrackName(track, time, "Sample Track")
midi_file.addTempo(track, time, 306)

# Add the notes in a three-note increasing scale
channel = 0
volume = 100
time = 0  # In beats
duration = 0.25  # In beats

# Add the notes
midi_file.addNote(track, channel, 65, time, duration, volume)  # Middle C
midi_file.addNote(track, channel, 67, time + 0.25, duration, volume)  # D
midi_file.addNote(track, channel, 63, time + 0.5, duration, volume)  # E

# Write it to disk
with open("../music/PlayerBlip.mid", "wb") as file:
    midi_file.writeFile(file)