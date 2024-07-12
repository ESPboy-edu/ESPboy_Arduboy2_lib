from pretty_midi import PrettyMIDI

PERIODS = {}
for n in range(128):
    freq = 440 * pow(2, float(n - 69) / 12)
    period = int(16000000.0 / freq / 4)
    if period > 65535: period = 65535
    PERIODS[n] = period

def vf_default(n, t):
    return n[2]

def vf_fadeout(n, t):
    v = n[2]
    f = (float(t) - n[0]) / (n[1] - n[0])
    k = 1 - f
    return int(k * v)

def vf_fadeout2(n, t):
    v = n[2]
    f = (float(t) - n[0]) / (n[1] - n[0])
    k = 1
    if f > 0.5: k = 1 - (f - 0.5) * 2
    return int(k * v)

def vf_fadeout4(n, t):
    v = n[2]
    f = (float(t) - n[0]) / (n[1] - n[0])
    k = 1
    if f > 0.75: k = 1 - (f - 0.75) * 4
    return int(k * v)

def convert(fin, fout, tracks=4, tps=48, vol=0.5, transpose=0, slowdown=1.0, vf=vf_default):

    # read midi data and quantize notes
    d = PrettyMIDI(fin)
    notes = []
    ticks = 0
    lownotes = 0
    for i in d.instruments:
        if i.is_drum: continue
        for n in i.notes:
            a = int(n.start * slowdown * tps)
            b = int(n.end   * slowdown * tps)
            if a == b: continue
            volume = int(vol * 2 * n.velocity)
            pitch = n.pitch + transpose
            if pitch not in PERIODS: continue
            period = PERIODS[pitch]
            if period < 65535:
                notes.append([a, b, volume, period])
                if b > ticks: ticks = b
            else:
                lownotes += 1
    print('low notes:', lownotes)
    
    # sort notes by descending pitch (favor high/melody notes)
    notes = sorted(notes, key=lambda x: x[3])
    
    # merge identical notes
    for i in range(len(notes)):
        for j in range(len(notes)):
            if i == j: continue;
            n1 = notes[i]
            n2 = notes[j]
            if n1[0] != n2[0]: continue
            if n1[1] != n2[1]: continue
            if n1[3] != n2[3]: continue
            notes[i][2] = max(n1[2], n2[2])
            notes[j][2] = 0
    
    # assign notes to tracks
    # (notes must stay on the same track from tick to tick)
    ta = [-1] * (ticks * tracks)
    lost_notes = 0
    for ni in range(len(notes)):
        n = notes[ni]
        if n[2] <= 0: continue
        for t in range(tracks):
            found = True
            for i in range(n[0], n[1]):
                if ta[i * tracks + t] >= 0:
                    found = False
                    break
            if found:
                for i in range(n[0], n[1]):
                    ta[i * tracks + t] = ni
                break
        if not found:
            lost_notes += 1
    print('lost', lost_notes, 'out of', len(notes), 'notes')
    
    # generate command sequence
    commands = [[0, 255, 255] * tracks + [1] for _ in range(ticks)]
    commands += [[0, 0, 0] * tracks + [1]]
    maxvol = 0
    for tick in range(ticks):
        i = tick * tracks
        totvol = 0
        for track in range(tracks):
            a = ta[i + track]
            if a < 0: continue
            n = notes[a]
            volume = vf(n, tick)
            period = n[3]
            commands[tick][track * 3 + 0] = volume
            commands[tick][track * 3 + 1] = period % 256
            commands[tick][track * 3 + 2] = (period >> 8) % 256
            totvol += volume
        if totvol > maxvol: maxvol = totvol
    print('max volume: ', maxvol)
    
    # merge identical commands
    i = 0
    while i < len(commands):
        j = i + 1;
        while j < len(commands):
            if commands[i] != commands[j]:
                break
            j += 1
        n = j - i
        if n > 1:
            if n > 255: n = 255
            commands[i][-1] = n
            del commands[i+1:j]
        i += 1
    
    # produce output
    bytes = bytearray()
    for c in commands:
        for byte in c:
            bytes.append(byte)
    with open(fout, 'wb') as f:
        f.write(bytes)

def convert_sym(sym, outfile, **args):
    kwargs = {
        'tracks': 4,
        'tps':    52,
        'vol':    0.5,
        'vf':     vf_fadeout4
        }
    for k, v in args.items():
        kwargs[k] = v
    convert(
        sym + '.mid',
        './music/' + outfile + '.bin',
        **kwargs)
 

#convert_sym('../music/Death', 'SFX_Death', tracks=1)
#convert_sym('../music/Victory', 'SFX_Victory', tracks=1)
convert_sym('../music/PlayerBlip', 'SFX_PlayerBlip', tracks=1)
convert_sym('../music/EnemyBlip', 'SFX_EnemyBlip', tracks=1)
convert_sym('../music/Explosion', 'SFX_Explosion', tracks=1)
convert_sym('../music/Death', 'SFX_Death', tracks=1)
convert_sym('../music/ThreeDecreasing', 'SFX_ThreeDec', tracks=1)
convert_sym('../music/DoubleBeep', 'SFX_DoubleBeep', tracks=1)
convert_sym('../music/DoubleBeep', 'SFX_DoubleBeepDown', tracks=1)
convert_sym('../music/bet1', 'SFX_Bet1', tracks=1)
convert_sym('../music/bet2', 'SFX_Bet2', tracks=1)
convert_sym('../music/bet3', 'SFX_Bet3', tracks=1)
convert_sym('../music/bet4', 'SFX_Bet4', tracks=1)
convert_sym('../music/bet5', 'SFX_Bet5', tracks=1)
convert_sym('../music/bet6', 'SFX_Bet6', tracks=1)
convert_sym('../music/click', 'SFX_Click', tracks=1)
convert_sym('../music/smallBlind', 'SFX_SmallBlind', tracks=1)
convert_sym('../music/bigBlind', 'SFX_BigBlind', tracks=1)

convert_sym('../music/TheGambler', 'TheGambler', tracks=6)
