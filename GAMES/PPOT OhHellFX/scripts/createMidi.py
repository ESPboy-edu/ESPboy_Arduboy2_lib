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
duration = 0.04  # In beats

# Add the notes
# midi_file.addNote(track, channel, 79, time, duration, volume)  # Middle C
# midi_file.addNote(track, channel, 77, time + 0.25, duration, volume)  # D
# midi_file.addNote(track, channel, 75, time + 0.5, duration, volume)  # E


# midi_file.addNote(track, channel, 79, time, duration, volume)  # Middle C
# midi_file.addNote(track, channel, 77, time + 0.25, duration, 0)  # D
# midi_file.addNote(track, channel, 75, time + 0.5, duration, 0)  # E

# # Write it to disk
# with open("three_note_decreasing_scale.mid", "wb") as file:
#     midi_file.writeFile(file)


midi_file.addNote(track, channel, 50, time, duration, volume)  # Middle C
midi_file.addNote(track, channel, 55, time + 0.10, duration, volume)  # Middle C

# Write it to disk
with open("bigBlind.mid", "wb") as file:
    midi_file.writeFile(file)