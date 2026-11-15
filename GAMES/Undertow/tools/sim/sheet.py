from PIL import Image, ImageDraw
import glob, os, sys
files = sorted(glob.glob(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'shots', '*.pbm')))
S, pad, lab, cols = 4, 16, 18, 3
rows = (len(files) + cols - 1) // cols
sheet = Image.new('RGB', (cols*(128*S+pad)+pad, rows*(64*S+pad+lab)+pad), (24, 24, 28))
d = ImageDraw.Draw(sheet)
for k, f in enumerate(files):
    im = Image.open(f).convert('L').point(lambda v: 255 - v)
    rgb = Image.merge('RGB', [im.point(lambda v, c=c: 0 if v < 128 else c) for c in (225, 240, 255)])
    rgb = rgb.resize((128*S, 64*S), Image.NEAREST)
    x = pad + (k % cols)*(128*S+pad); y = pad + (k//cols)*(64*S+pad+lab)
    d.text((x, y), os.path.basename(f)[2:-4].upper(), fill=(160, 160, 170))
    sheet.paste(rgb, (x, y+lab))
sheet.save(sys.argv[1])
