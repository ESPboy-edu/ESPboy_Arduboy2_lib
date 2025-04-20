from PIL import Image, ImageColor
import numpy as np

# Load your image
original_image = Image.open("image.png").convert("RGBA")

# Parameters
num_frames = 96
tiles_x = 8
tiles_y = 12
tile_width, tile_height = 128, 64
zoom_factors = np.geomspace(0.2, 50, num=num_frames)  # from 20% to 5000%
tilemap_width = tile_width * tiles_x
tilemap_height = tile_height * tiles_y
transparent_color = ImageColor.getcolor("#ff00ff", "RGBA")

# Create blank tilemap
tilemap = Image.new("RGBA", (tilemap_width, tilemap_height), (0, 0, 0, 0))

for i, zoom in enumerate(zoom_factors):
    new_size = (int(original_image.width * zoom), int(original_image.height * zoom))
    resized = original_image.resize(new_size, resample=Image.Resampling.NEAREST)

    # Create frame and paste resized image centered
    frame = Image.new("RGBA", (tile_width, tile_height), (0, 0, 0, 0))
    left = (tile_width - resized.width) // 2
    top = (tile_height - resized.height) // 2
    frame.paste(resized, (left, top), resized)

    # Make pink transparent
    data = frame.getdata()
    new_data = [(r, g, b, 0) if (r, g, b) == transparent_color[:3] else (r, g, b, a) for (r, g, b, a) in data]
    frame.putdata(new_data)

    # Position in tilemap
    grid_x = (i % tiles_x) * tile_width
    grid_y = (i // tiles_x) * tile_height
    tilemap.paste(frame, (grid_x, grid_y), frame)

# Save output
tilemap.save("zoom_tilemap_5000_grid_8x12_transparent.png")