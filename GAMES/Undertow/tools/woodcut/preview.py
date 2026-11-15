import sys
from PIL import Image, ImageDraw
from figures import all_frames
fr = all_frames()
S, pad, lab, cols = 5, 10, 14, 4
rows = (len(fr) + cols - 1) // cols
sheet = Image.new('RGB', (cols * (64 * S + pad) + pad, rows * (48 * S + pad + lab) + pad), (40, 40, 48))
d = ImageDraw.Draw(sheet)
for k, (name, c) in enumerate(fr):
    x = pad + (k % cols) * (64 * S + pad); y = pad + (k // cols) * (48 * S + pad + lab)
    d.text((x, y), name, fill=(200, 200, 210))
    im = c.image().resize((64 * S, 48 * S), Image.NEAREST)
    sheet.paste(im, (x, y + lab))
sheet.save(sys.argv[1])
