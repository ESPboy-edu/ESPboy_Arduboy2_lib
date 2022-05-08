using System.Drawing;
using System.IO;

namespace RickArdurousEditor.Items
{
	public class Item
	{
		public enum Type
		{
			RICK,
			BULLET_CRATE,
			DYNAMITE_CRATE,
			STATUETTE,
			GRAAL,
			HORIZONTAL_SPIKE,
			VERTICAL_SPIKE,
			MUMMY,
			SKELETON,
			SCORPION,
			STALAGMITE,
			STALACTITE,
			ARROW_LAUNCHER,
			DESTROYABLE_BLOCK,
			BOULDER,

			// the type count
			COUNT,
		}

		public enum RespawnType
		{
			NORMAL,
			START,
		}

		private static Pen mSelectedPen = new Pen(Color.Yellow, 2);
		private static Pen mArrowLauncherPen = new Pen(Color.Magenta, 1);

		// type of the item
		private Type mType = Type.HORIZONTAL_SPIKE;
		private bool mIsMirror = false;
		private bool mIsSpecial = false;

		// two special flags for the Rick type of Item that can identify the first and last puzzle screen
		private RespawnType mRickRespawnType = RespawnType.NORMAL;

		// coordinate of the Item
		private int mX = 0;
		private int mY = 0;

		// sprite of the item
		Bitmap mSprite = null;

		// distance of the arrow launcher
		private int mArrowLauncherDistance = 80;

		#region get/set
		public int X
		{
			get { return mX; }
		}

		public int Y
		{
			get { return mY; }
		}

		public Point Location
		{
			get { return new Point(mX, mY); }
		}

		public bool IsMirror
		{
			get { return mIsMirror; }
		}

		public bool IsSpecial
		{
			get { return mIsSpecial; }
			set
			{
				mIsSpecial = value;
				UpdateSprite();
			}
		}

		public Type ItemType
		{
			get { return mType; }
		}

		public RespawnType RickRespawnType
		{
			get { return mRickRespawnType; }
			set
			{
				mRickRespawnType = value;
				UpdateSprite();
			}
		}

		public int SensorDistance
		{
			get { return mArrowLauncherDistance; }
			set { mArrowLauncherDistance = value; }
		}

		public bool WasSaved
		{
			get;
			set;
		}
		#endregion

		public Item(Type type, bool isMirrored, bool isSpecial, int x, int y, int additionalParameter)
		{
			mType = type;
			Move(new Point(x, y)); // use the Move function because the destroyable block snap to grid
			mIsMirror = isMirrored;
			mIsSpecial = isSpecial;
			mArrowLauncherDistance = additionalParameter;
			UpdateSprite();
		}

		#region edition
		private void UpdateSprite()
		{
			switch (mType)
			{
				case Type.RICK:
					mSprite = ImageProvider.GetRickImage(mRickRespawnType);
					break;
				case Type.BULLET_CRATE:
					mSprite = ImageProvider.GetBulletCrateImage();
					break;
				case Type.DYNAMITE_CRATE:
					mSprite = ImageProvider.GetDynamiteCrateImage();
					break;
				case Type.STATUETTE:
					mSprite = ImageProvider.GetStatuetteImage();
					break;
				case Type.GRAAL:
					mSprite = ImageProvider.GetGraalImage();
					break;
				case Type.HORIZONTAL_SPIKE:
					mSprite = ImageProvider.GetHorizontalSpikeImage(mIsMirror);
					break;
				case Type.VERTICAL_SPIKE:
					mSprite = ImageProvider.GetVerticalSpikeImage(mIsMirror);
					break;
				case Type.MUMMY:
					mSprite = ImageProvider.GetMummyImage(mIsMirror);
					break;
				case Type.SKELETON:
					mSprite = ImageProvider.GetSkeletonImage(mIsMirror);
					break;
				case Type.SCORPION:
					mSprite = ImageProvider.GetScorpionImage(mIsMirror);
					break;
				case Type.STALAGMITE:
					mSprite = ImageProvider.GetStalagmiteImage();
					break;
				case Type.STALACTITE:
					mSprite = ImageProvider.GetStalactiteImage();
					break;
				case Type.ARROW_LAUNCHER:
					mSprite = ImageProvider.GetArrowLauncherImage(mIsMirror);
					break;
				case Type.DESTROYABLE_BLOCK:
					mSprite = ImageProvider.GetDestroyableBlockImage();
					break;
				case Type.BOULDER:
					mSprite = ImageProvider.GetBoulderImage(mIsMirror, mIsSpecial);
					break;
			}
		}

		public void Mirror()
		{
			mIsMirror = !mIsMirror;
			UpdateSprite();
		}

		public void Move(Point location)
		{
			if (mType == Type.DESTROYABLE_BLOCK)
			{
				// snap the destroyable block on the grid of the map, because the game engine needs it
				mX = (location.X >> 3) << 3;
				mY = ((location.Y + 4) >> 3) << 3;
			}
			else
			{
				mX = location.X;
				mY = location.Y;
			}
		}
		#endregion

		#region read/write
		private string GetInstanceName(int instanceNumber)
		{
			return GetInstanceName(mType, instanceNumber);
		}

		public static int GetMemorySize(Type type, int instanceCount)
		{
			switch (type)
			{
				case Type.BULLET_CRATE:
				case Type.DYNAMITE_CRATE:
					return (54 * instanceCount) + 216;
				case Type.STATUETTE:
					return (54 * instanceCount) + 208;
				case Type.GRAAL:
					// the graal instance is necessary for the build, therefore it is already counted in the global code size
					return 0;
				case Type.HORIZONTAL_SPIKE:
				case Type.VERTICAL_SPIKE:
					return (52 * instanceCount) + 260;
				case Type.MUMMY:
				case Type.SKELETON:
				case Type.SCORPION:
					return (54 * instanceCount) + 1812;
				case Type.STALAGMITE:
					return (46 * instanceCount) + 854;
				case Type.STALACTITE:
					return (46 * instanceCount) + 1044;
				case Type.ARROW_LAUNCHER:
					return (88 * instanceCount) + 452;
				case Type.DESTROYABLE_BLOCK:
					return (52 * instanceCount) + 736;
				case Type.BOULDER:
					return (60 * instanceCount) + 820;
			}
			return 0;
		}

		private static string GetInstanceName(Type type, int instanceNumber)
		{
			string instanceNumberString = instanceNumber.ToString();
			switch (type)
			{
				case Type.BULLET_CRATE:
					return "bulletCrate" + instanceNumberString;
				case Type.DYNAMITE_CRATE:
					return "dynamiteCrate" + instanceNumberString;
				case Type.STATUETTE:
					return "statuette" + instanceNumberString;
				case Type.GRAAL:
					return "graal" + instanceNumberString;
				case Type.HORIZONTAL_SPIKE:
				case Type.VERTICAL_SPIKE:
					return "spike" + instanceNumberString;
				case Type.MUMMY:
				case Type.SKELETON:
				case Type.SCORPION:
					return "enemy" + instanceNumberString;
				case Type.STALAGMITE:
					return "stalagmite" + instanceNumberString;
				case Type.STALACTITE:
					return "stalactite" + instanceNumberString;
				case Type.ARROW_LAUNCHER:
					return "arrowLauncher" + instanceNumberString;
				case Type.DESTROYABLE_BLOCK:
					return "destBlock" + instanceNumberString;
				case Type.BOULDER:
					return "boulder" + instanceNumberString;
			}
			return string.Empty;
		}

		public static Type GetInstanceTypeFromName(string instanceName, bool isSpecial, bool isSpecial2)
		{
			if (instanceName.StartsWith("bulletCrate"))
				return Type.BULLET_CRATE;
			else if (instanceName.StartsWith("dynamiteCrate"))
				return Type.DYNAMITE_CRATE;
			else if (instanceName.StartsWith("statuette"))
				return Type.STATUETTE;
			else if (instanceName.StartsWith("graal"))
				return Type.GRAAL;
			else if (instanceName.StartsWith("spike"))
				return isSpecial ? Type.HORIZONTAL_SPIKE : Type.VERTICAL_SPIKE;
			else if (instanceName.StartsWith("enemy"))
				return isSpecial ? Type.SCORPION : (isSpecial2 ? Type.SKELETON : Type.MUMMY);
			else if (instanceName.StartsWith("stalagmite"))
				return Type.STALAGMITE;
			else if (instanceName.StartsWith("stalactite"))
				return Type.STALACTITE;
			else if (instanceName.StartsWith("arrowLauncher"))
				return Type.ARROW_LAUNCHER;
			else if (instanceName.StartsWith("destBlock"))
				return Type.DESTROYABLE_BLOCK;
			else if (instanceName.StartsWith("boulder"))
				return Type.BOULDER;

			return Type.HORIZONTAL_SPIKE;
		}

		public static void WriteInstance(StreamWriter writer, Type type, int instanceNumber)
		{
			string instanceName = GetInstanceName(type, instanceNumber);
			switch (type)
			{
				case Type.BULLET_CRATE:
					writer.WriteLine("BulletCrate " + instanceName + ";");
					break;
				case Type.DYNAMITE_CRATE:
					writer.WriteLine("DynamiteCrate " + instanceName + ";");
					break;
				case Type.STATUETTE:
					writer.WriteLine("Statuette " + instanceName + ";");
					break;
				case Type.GRAAL:
					writer.WriteLine("Graal " + instanceName + ";");
					break;
				case Type.HORIZONTAL_SPIKE:
				case Type.VERTICAL_SPIKE:
					writer.WriteLine("Spike " + instanceName + ";");
					break;
				case Type.MUMMY:
				case Type.SKELETON:
				case Type.SCORPION:
					writer.WriteLine("Enemy " + instanceName + ";");
					break;
				case Type.STALAGMITE:
					writer.WriteLine("Stalagmite " + instanceName + ";");
					break;
				case Type.STALACTITE:
					writer.WriteLine("Stalactite " + instanceName + ";");
					break;
				case Type.ARROW_LAUNCHER:
					writer.WriteLine("ArrowLauncher " + instanceName + ";");
					break;
				case Type.DESTROYABLE_BLOCK:
					writer.WriteLine("DestroyableBlock " + instanceName + ";");
					break;
				case Type.BOULDER:
					writer.WriteLine("Boulder " + instanceName + ";");
					break;
			}
		}

		public static bool WriteSaveAndLoadLivingStatus(StreamWriter writer, Type type, int instanceNumber, int instanceCount)
		{
			string instanceName = GetInstanceName(type, instanceNumber);
			switch (type)
			{
				case Type.BULLET_CRATE:
				case Type.DYNAMITE_CRATE:
				case Type.STATUETTE:
				case Type.GRAAL:
				case Type.MUMMY:
				case Type.SKELETON:
				case Type.SCORPION:
				case Type.STALAGMITE:
				case Type.STALACTITE:
				case Type.DESTROYABLE_BLOCK:
					writer.WriteLine("\t" + instanceName + ".SaveAndLoadAliveStatus(currentScreenIdToSave, newScreenIdToLoad);");
					return true;
			}
			return false;
		}

		public void WriteCheckpoint(StreamWriter writer, int instanceNumber)
		{
			if (mType == Type.RICK)
				writer.WriteLine("\tMapManager::MemorizeCheckPoint(" + mX.ToString() + ", " + mY.ToString() + ");");
		}

		public void WriteInit(StreamWriter writer, int instanceNumber)
		{
			string mirrorFlag = mIsMirror ? "Item::PropertyFlags::MIRROR_X" : "Item::PropertyFlags::NONE";
			switch (mType)
			{
				case Type.BULLET_CRATE:
				case Type.DYNAMITE_CRATE:
				case Type.STATUETTE:
				case Type.GRAAL:
					writer.WriteLine("\t" + GetInstanceName(instanceNumber) + ".Init(" + mX.ToString() + ", " + mY.ToString() + ", Item::PropertyFlags::NONE, shouldRespawn);");
					break;
				case Type.HORIZONTAL_SPIKE:
					{
						string flags = ", Item::PropertyFlags::SPECIAL";
						if (mIsMirror)
							flags += " | Item::PropertyFlags::MIRROR_X";
						writer.WriteLine("\t" + GetInstanceName(instanceNumber) + ".Init(" + mX.ToString() + ", " + mY.ToString() + flags + ");");
						break;
					}
				case Type.VERTICAL_SPIKE:
					writer.WriteLine("\t" + GetInstanceName(instanceNumber) + ".Init(" + mX.ToString() + ", " + mY.ToString() + ", " + mirrorFlag  + ");");
					break;
				case Type.MUMMY:
					{
						string flags = ", Item::PropertyFlags::TRAP_TRIGERER";
						if (mIsMirror)
							flags += " | Item::PropertyFlags::MIRROR_X";
						writer.WriteLine("\t" + GetInstanceName(instanceNumber) + ".Init(" + mX.ToString() + ", " + mY.ToString() + flags + ", shouldRespawn);");
						break;
					}
				case Type.SKELETON:
					{
						string flags = ", Item::PropertyFlags::TRAP_TRIGERER | Item::PropertyFlags::SPECIAL_2";
						if (mIsMirror)
							flags += " | Item::PropertyFlags::MIRROR_X";
						writer.WriteLine("\t" + GetInstanceName(instanceNumber) + ".Init(" + mX.ToString() + ", " + mY.ToString() + flags + ", shouldRespawn);");
						break;
					}
				case Type.SCORPION:
					{
						string flags = ", Item::PropertyFlags::SPECIAL";
						if (mIsMirror)
							flags += " | Item::PropertyFlags::MIRROR_X";
						writer.WriteLine("\t" + GetInstanceName(instanceNumber) + ".Init(" + mX.ToString() + ", " + mY.ToString() + flags + ", shouldRespawn);");
						break;
					}
				case Type.STALAGMITE:
				case Type.STALACTITE:
					writer.WriteLine("\t" + GetInstanceName(instanceNumber) + ".Init(" + mX.ToString() + ", " + mY.ToString() + ", Item::PropertyFlags::NONE, shouldRespawn);");
					break;
				case Type.ARROW_LAUNCHER:
					writer.WriteLine("\t" + GetInstanceName(instanceNumber) + ".Init(" + mX.ToString() + ", " + mY.ToString() + ", " + mirrorFlag + ", " + mArrowLauncherDistance .ToString() + ");");
					break;
				case Type.DESTROYABLE_BLOCK:
					writer.WriteLine("\t" + GetInstanceName(instanceNumber) + ".Init(" + mX.ToString() + ", " + mY.ToString() + ", Item::PropertyFlags::NONE, shouldRespawn);");
					break;
				case Type.BOULDER:
					{
						string flags = ", Item::PropertyFlags::TRAP_TRIGERER | Item::PropertyFlags::ALIVE ";
						if (mIsMirror)
							flags += " | Item::PropertyFlags::MIRROR_X";
						if (mIsSpecial)
							flags += " | Item::PropertyFlags::SPECIAL";
						writer.WriteLine("\t" + GetInstanceName(instanceNumber) + ".Init(" + mX.ToString() + ", " + mY.ToString() + flags + ");");
						break;
					}
			}
		}
		#endregion

		public bool IsUnder(int x, int y)
		{
			return (x >= mX) && (x < mX + mSprite.Width) && (y >= mY) && (y < mY + mSprite.Height);
		}

		public bool IsInPuzzleScreen(int left, int top, int right, int bottom)
		{
			return (mX >= left) && (mX < right) && (mY >= top) && (mY < bottom);
		}

		public bool IsCoordOnMySensorExtrimity(Point coord, int mouseSensivity)
		{
			// only the arrow launcher has sensor for now
			if (mType != Type.ARROW_LAUNCHER)
				return false;

			// compute the sensible area around the extrimity of my sensor
			int sensorExtremity = GetSensorExtremity();
			int minX = sensorExtremity - mouseSensivity;
			int maxX = sensorExtremity + mouseSensivity;

			return (coord.X >= minX) && (coord.X <= maxX) && (coord.Y >= mY) && (coord.Y <= mY + mSprite.Height);
		}

		private int GetSensorExtremity()
		{
			return mIsMirror ? mX + mSprite.Width - mArrowLauncherDistance : mX + mArrowLauncherDistance;
		}

		public void Draw(Graphics gc, int pixelSize, int drawPuzzleScreenWidth, int drawPuzzleScreenHeight, int puzzleLineSeparatorWidth, int cameraXWorld, int cameraYWorld, bool isSelected)
		{
			// get the drawing coordinate from the world coord of the item and the camera world coord
			int itemXDraw = (mX - cameraXWorld) * pixelSize;
			int itemYDraw = (mY - cameraYWorld) * pixelSize;

			// add one pixel for each puzzle screen line drawn
			// First compute how much shift from the border is the camera
			int cameraShiftX = (cameraXWorld * pixelSize) % drawPuzzleScreenWidth;
			int lineCountX = (itemXDraw + cameraShiftX) / drawPuzzleScreenWidth;
			if (cameraShiftX == 0)
				lineCountX++;

			int cameraShiftY = (cameraYWorld * pixelSize) % drawPuzzleScreenHeight;
			int lineCountY = (itemYDraw + cameraShiftY) / drawPuzzleScreenHeight;
			if (cameraShiftY == 0)
				lineCountY++;

			// add the width of the lines
			itemXDraw += lineCountX * puzzleLineSeparatorWidth;
			itemYDraw += lineCountY * puzzleLineSeparatorWidth;

			// compute the rectangle in which the item will be drawn
			Rectangle drawArea = new Rectangle(itemXDraw, itemYDraw, mSprite.Width * pixelSize, mSprite.Height * pixelSize);
			gc.DrawImage(mSprite, drawArea);
			if (isSelected)
				gc.DrawRectangle(mSelectedPen, drawArea);

			// special case for the Arrow launcher, draw the sensor area
			if (mType == Type.ARROW_LAUNCHER)
			{
				int sensorX = mIsMirror ? (((mX + mSprite.Width - mArrowLauncherDistance) - cameraXWorld) * pixelSize) + (lineCountX * puzzleLineSeparatorWidth) : itemXDraw;
				Rectangle arrowLauncherArea = new Rectangle(sensorX, itemYDraw, mArrowLauncherDistance * pixelSize, mSprite.Height * pixelSize);
				gc.DrawRectangle(mArrowLauncherPen, arrowLauncherArea);
			}
		}
	}
}
