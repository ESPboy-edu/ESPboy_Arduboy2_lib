from midiutil import MIDIFile

# Create the MIDIFile Object
midi_file = MIDIFile(1)  # One track

# Add track name and tempo.
track = 0
time = 0
midi_file.addTrackName(track, time, "Sample Track")
midi_file.addTempo(track, time, 720)

# Add the notes in a three-note increasing scale
channel = 0
volume = 100
time = 0  # In beats
duration = 0.25  # In beats

# Add the notes
midi_file.addNote(track, channel, 55, time, duration, volume) 
midi_file.addNote(track, channel, 76, time + 0.25, duration, volume)
midi_file.addNote(track, channel, 53, time + 0.5, duration, volume) 
midi_file.addNote(track, channel, 49, time + 0.75, duration, volume)
midi_file.addNote(track, channel, 23, time + 1.25, duration, volume)  
midi_file.addNote(track, channel, 53, time + 1.5, duration, volume)  
midi_file.addNote(track, channel, 55, time + 1.75, duration, volume)
midi_file.addNote(track, channel, 68, time + 2, duration, volume)  
midi_file.addNote(track, channel, 45, time + 2.25, duration, volume)
midi_file.addNote(track, channel, 70, time + 2.50, duration, volume)
midi_file.addNote(track, channel, 25, time + 2.75, duration, volume)
midi_file.addNote(track, channel, 65, time + 3, duration, volume)  
midi_file.addNote(track, channel, 45, time + 3.25, duration, volume)
midi_file.addNote(track, channel, 93, time + 3.50, duration, volume)
midi_file.addNote(track, channel, 20, time + 3.75, duration, volume)
midi_file.addNote(track, channel, 45, time + 4.00, duration, volume)

# Write it to disk
with open("../music/Explosion.mid", "wb") as file:
    midi_file.writeFile(file)