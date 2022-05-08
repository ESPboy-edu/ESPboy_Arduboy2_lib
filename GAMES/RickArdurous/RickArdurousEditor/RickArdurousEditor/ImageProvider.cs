using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Windows.Forms;

namespace RickArdurousEditor
{
	class ImageProvider
	{

		private static void SetGCInPixelMode(ref Graphics gc)
		{
			gc.CompositingMode = System.Drawing.Drawing2D.CompositingMode.SourceOver;
			gc.CompositingQuality = System.Drawing.Drawing2D.CompositingQuality.Default;
			gc.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
			gc.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.None;
			gc.PixelOffsetMode = System.Drawing.Drawing2D.PixelOffsetMode.None;
		}

		private static void CreateTaintedAndMirroredImage(Bitmap sourceImage, Rectangle sourceRectangle, ref Bitmap resultImage, RotateFlipType rotateFlipOperation, float red, float green, float blue, Rectangle destinationRectangle = default)
		{
			// is the destination rectangle is empty use the full result image size
			if (destinationRectangle.IsEmpty)
				destinationRectangle = new Rectangle(0, 0, resultImage.Width, resultImage.Height);

			// Create some image attribut to change the color of the mirrored image
			ImageAttributes imageAttributes = new ImageAttributes();
			float[][] colorMatrixElements = {
			   new float[] {red,  0,  0,  0, 0},     // red scaling factor
			   new float[] {0,  green,  0,  0, 0},     // green scaling factor
			   new float[] {0,  0,  blue,  0, 0},     // blue scaling factor
			   new float[] {0,  0,  0,  1, 0},     // alpha scaling factor of 1
			   new float[] {0,  0,  0,  0, 1}};    // translations
			ColorMatrix colorMatrix = new ColorMatrix(colorMatrixElements);
			imageAttributes.SetColorMatrix(colorMatrix, ColorMatrixFlag.Default, ColorAdjustType.Bitmap);

			// set the color key so that the black becomes transparent
			imageAttributes.SetColorKey(Color.Black, Color.Gray, ColorAdjustType.Bitmap);

			// check if the source rectangle is empty, use the full source image
			if (sourceRectangle.IsEmpty)
				sourceRectangle = new Rectangle(0, 0, sourceImage.Width, sourceImage.Height);

			// paint the image in the mirrored one with the image attribute
			Graphics gc = Graphics.FromImage(resultImage);
			SetGCInPixelMode(ref gc);
			gc.DrawImage(sourceImage,       // source image
				destinationRectangle,  // destination rectangle 
				sourceRectangle.X,
				sourceRectangle.Y,          // upper-left corner of source rectangle 
				sourceRectangle.Width,      // width of source rectangle
				sourceRectangle.Height,     // height of source rectangle
				GraphicsUnit.Pixel,
				imageAttributes);

			// and mirror it if needed
			resultImage.RotateFlip(rotateFlipOperation);
		}

		public static Bitmap GetWallSpriteImage()
		{
			return new Bitmap(PathProvider.GetImagePath() + @"Walls.png");
		}

		public static Bitmap GetRickImage(Items.Item.RespawnType respawnType)
		{
			Bitmap sprite = new Bitmap(PathProvider.GetImagePath() + @"Rick.png");
			Bitmap result = new Bitmap(9, 16);
			float red = 0.1f;
			float green = 0.4f;
			float blue = 1f;
			switch (respawnType)
			{
				case Items.Item.RespawnType.START:
					red = 0f;
					green = 1f;
					blue = 1f;
					break;
			}
			CreateTaintedAndMirroredImage(sprite, new Rectangle(0, 0, 9, 16), ref result, RotateFlipType.RotateNoneFlipNone, red, green, blue);
			return result;
		}

		public static Bitmap GetHorizontalSpikeImage(bool isTop)
		{
			Bitmap sprite = new Bitmap(PathProvider.GetImagePath() + @"SpikeHorizontalBottom.png");
			Bitmap result = new Bitmap(3, 8);
			RotateFlipType rotateFlipOperation = isTop ? RotateFlipType.RotateNoneFlipY : RotateFlipType.RotateNoneFlipNone;
			CreateTaintedAndMirroredImage(sprite, new Rectangle(0, 0, 3, 8), ref result, rotateFlipOperation, 1f, 0f, 0f);
			return result;
		}

		public static Bitmap GetVerticalSpikeImage(bool isLeft)
		{
			Bitmap sprite = new Bitmap(PathProvider.GetImagePath() + @"SpikeVertical.png");
			Bitmap result = new Bitmap(4, 8);
			RotateFlipType rotateFlipOperation = isLeft ? RotateFlipType.RotateNoneFlipX : RotateFlipType.RotateNoneFlipNone;
			CreateTaintedAndMirroredImage(sprite, new Rectangle(0, 0, 4, 8), ref result, rotateFlipOperation, 1f, 0f, 0f);
			return result;
		}

		public static Bitmap GetMummyImage(bool isLeft)
		{
			Bitmap sprite = new Bitmap(PathProvider.GetImagePath() + @"Mummy.png");
			Bitmap result = new Bitmap(9, 16);
			RotateFlipType rotateFlipOperation = isLeft ? RotateFlipType.RotateNoneFlipX : RotateFlipType.RotateNoneFlipNone;
			CreateTaintedAndMirroredImage(sprite, new Rectangle(0, 0, 9, 16), ref result, rotateFlipOperation, 1f, 1f, 0f);
			return result;
		}

		public static Bitmap GetSkeletonImage(bool isLeft)
		{
			Bitmap sprite = new Bitmap(PathProvider.GetImagePath() + @"Skeleton.png");
			Bitmap result = new Bitmap(6, 16);
			RotateFlipType rotateFlipOperation = isLeft ? RotateFlipType.RotateNoneFlipX : RotateFlipType.RotateNoneFlipNone;
			CreateTaintedAndMirroredImage(sprite, new Rectangle(0, 0, 6, 16), ref result, rotateFlipOperation, 1f, 1f, 0f);
			return result;
		}

		public static Bitmap GetScorpionImage(bool isLeft)
		{
			Bitmap sprite = new Bitmap(PathProvider.GetImagePath() + @"Scorpion.png");
			Bitmap result = new Bitmap(8, 4);
			RotateFlipType rotateFlipOperation = isLeft ? RotateFlipType.RotateNoneFlipX : RotateFlipType.RotateNoneFlipNone;
			CreateTaintedAndMirroredImage(sprite, new Rectangle(0, 0, 8, 4), ref result, rotateFlipOperation, 1f, 1f, 0f);
			return result;
		}

		public static Bitmap GetGraalImage()
		{
			Bitmap sprite = new Bitmap(PathProvider.GetImagePath() + @"Graal.png");
			Bitmap result = new Bitmap(5, 8);
			CreateTaintedAndMirroredImage(sprite, new Rectangle(0, 0, 5, 8), ref result, RotateFlipType.RotateNoneFlipNone, 0.3f, 1f, 0f);
			return result;
		}

		public static Bitmap GetStatuetteImage()
		{
			Bitmap sprite = new Bitmap(PathProvider.GetImagePath() + @"Statuette.png");
			Bitmap result = new Bitmap(5, 8);
			CreateTaintedAndMirroredImage(sprite, new Rectangle(0, 0, 5, 8), ref result, RotateFlipType.RotateNoneFlipNone, 0.3f, 1f, 0f);
			return result;
		}

		public static Bitmap GetBulletCrateImage()
		{
			Bitmap sprite = new Bitmap(PathProvider.GetImagePath() + @"CrateBullet.png");
			Bitmap result = new Bitmap(11, 8);
			CreateTaintedAndMirroredImage(sprite, new Rectangle(0, 0, 11, 8), ref result, RotateFlipType.RotateNoneFlipNone, 0.3f, 1f, 0f);
			return result;
		}

		public static Bitmap GetDynamiteCrateImage()
		{
			Bitmap sprite = new Bitmap(PathProvider.GetImagePath() + @"CrateDynamite.png");
			Bitmap result = new Bitmap(11, 8);
			CreateTaintedAndMirroredImage(sprite, new Rectangle(0, 0, 11, 8), ref result, RotateFlipType.RotateNoneFlipNone, 0.3f, 1f, 0f);
			return result;
		}

		public static Bitmap GetStalagmiteImage()
		{
			Bitmap sprite = new Bitmap(PathProvider.GetImagePath() + @"Stalagmite.png");
			Bitmap result = new Bitmap(7, 8);
			CreateTaintedAndMirroredImage(sprite, new Rectangle(0, 0, 3, 8), ref result, RotateFlipType.RotateNoneFlipNone, 1f, 0.65f, 0.1f, new Rectangle(0, 0, 3, 8));
			CreateTaintedAndMirroredImage(sprite, new Rectangle(0, 0, 3, 8), ref result, RotateFlipType.RotateNoneFlipX, 1f, 0.65f, 0.1f, new Rectangle(4, 0, 3, 8));
			return result;
		}

		public static Bitmap GetStalactiteImage()
		{
			Bitmap sprite = new Bitmap(PathProvider.GetImagePath() + @"Stalactite.png");
			Bitmap result = new Bitmap(7, 8);
			CreateTaintedAndMirroredImage(sprite, new Rectangle(0, 0, 3, 8), ref result, RotateFlipType.RotateNoneFlipNone, 1f, 0.65f, 0.1f, new Rectangle(0, 0, 3, 8));
			CreateTaintedAndMirroredImage(sprite, new Rectangle(0, 0, 3, 8), ref result, RotateFlipType.RotateNoneFlipX, 1f, 0.65f, 0.1f, new Rectangle(4, 0, 3, 8));
			return result;
		}

		public static Bitmap GetArrowLauncherImage(bool isLeft)
		{
			Bitmap sprite = new Bitmap(PathProvider.GetImagePath() + @"ArrowLauncherFace.png");
			Bitmap result = new Bitmap(4, 8);
			RotateFlipType rotateFlipOperation = isLeft ? RotateFlipType.RotateNoneFlipX : RotateFlipType.RotateNoneFlipNone;
			CreateTaintedAndMirroredImage(sprite, new Rectangle(0, 0, 4, 8), ref result, rotateFlipOperation, 1f, 0f, 1f, new Rectangle(0, 0, 3, 8));
			return result;
		}

		public static Bitmap GetDestroyableBlockImage()
		{
			Bitmap sprite = new Bitmap(PathProvider.GetImagePath() + @"DestroyableBlock.png");
			Bitmap result = new Bitmap(15, 8);
			CreateTaintedAndMirroredImage(sprite, new Rectangle(0, 0, 15, 8), ref result, RotateFlipType.RotateNoneFlipNone, 0.5f, 0.5f, 0.5f, new Rectangle(0, 0, 15, 8));
			return result;
		}

		public static Bitmap GetBoulderImage(bool isMirrored, bool isSpecial)
		{
			// load the boulder image and write it two times, with one time mirrored
			Bitmap sprite = new Bitmap(PathProvider.GetImagePath() + @"HalfBoulder.png");
			Bitmap result = new Bitmap(12, 12);
			// the special flag will change the color
			float red = isSpecial ? 0.5f : 0.8f;
			float green = isSpecial ? 0.2f : 0.5f;
			float blue = isSpecial ? 0.05f : 0.35f;
			// create the sprite
			CreateTaintedAndMirroredImage(sprite, new Rectangle(0, 0, 6, 12), ref result, RotateFlipType.RotateNoneFlipX, red, green, blue, new Rectangle(0, 0, 6, 12));
			CreateTaintedAndMirroredImage(sprite, new Rectangle(0, 0, 6, 12), ref result, RotateFlipType.RotateNoneFlipNone, red, green, blue, new Rectangle(0, 0, 6, 12));
			// load the shadow and write it in the same image
			sprite = new Bitmap(PathProvider.GetImagePath() + @"BoulderShadow.png");
			RotateFlipType rotateFlipOperation = !isMirrored ? RotateFlipType.RotateNoneFlipX : RotateFlipType.RotateNoneFlipNone;
			CreateTaintedAndMirroredImage(sprite, new Rectangle(0, 0, 4, 4), ref result, rotateFlipOperation, 0.0f, 0.0f, 0.0f, new Rectangle(2, 2, 4, 4));
			return result;
		}

		public static Bitmap GetItemsSpriteImage()
		{
			// get all the image of all the items
			List<Bitmap> itemImages = new List<Bitmap>();
			itemImages.Add(GetRickImage(Items.Item.RespawnType.NORMAL));
			itemImages.Add(GetBulletCrateImage());
			itemImages.Add(GetDynamiteCrateImage());
			itemImages.Add(GetStatuetteImage());
			itemImages.Add(GetGraalImage());
			itemImages.Add(GetHorizontalSpikeImage(false));
			itemImages.Add(GetVerticalSpikeImage(false));
			itemImages.Add(GetMummyImage(false));
			itemImages.Add(GetSkeletonImage(false));
			itemImages.Add(GetScorpionImage(false));
			itemImages.Add(GetStalagmiteImage());
			itemImages.Add(GetStalactiteImage());
			itemImages.Add(GetArrowLauncherImage(false));
			itemImages.Add(GetDestroyableBlockImage());
			itemImages.Add(GetBoulderImage(false, false));

			// declare the size, and numbers of items in the toolbars
			const int itemRows = 8;
			const int itemColumns = 2;
			const int cellSize = 64;
			Bitmap itemSpriteImage = new Bitmap(cellSize * itemColumns, cellSize * itemRows);
			Graphics gc = Graphics.FromImage(itemSpriteImage);
			SetGCInPixelMode(ref gc);
			gc.PixelOffsetMode = System.Drawing.Drawing2D.PixelOffsetMode.Half;

			// clear the background
			gc.Clear(Color.Black);

			// decrease the item size to leave a margin
			int margin = 1;
			int itemSize = cellSize - (margin * 2);

			// coordinate of the item to draw
			int itemX = 0;
			int itemY = 0;

			// iterate on all the item images to draw them in the correct cell space
			foreach (Bitmap itemImage in itemImages)
			{
				// get the size of the image and get the biggest one to keep the aspect ratio
				int width = itemSize;
				int height = itemSize;
				if (itemImage.Width < itemImage.Height)
					width = (itemSize * itemImage.Width) / itemImage.Height;
				else if (itemImage.Width > itemImage.Height)
					height = (itemSize * itemImage.Height) / itemImage.Width;

				// draw the image in the correct cell
				gc.DrawImage(itemImage,
					new Rectangle((cellSize * itemX) + ((cellSize - width) / 2), (cellSize * itemY) + ((cellSize - height) / 2), width, height),
					new Rectangle(0, 0, itemImage.Width, itemImage.Height),
					GraphicsUnit.Pixel);

				// increase the item coordinates
				itemY++;
				if (itemY == itemRows)
				{
					itemY = 0;
					itemX++;
				}
			}

			return itemSpriteImage;
		}
	}
}
