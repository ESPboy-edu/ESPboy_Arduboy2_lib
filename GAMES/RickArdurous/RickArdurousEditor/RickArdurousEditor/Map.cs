using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;

namespace RickArdurousEditor
{
	public class Map
	{
		private const int ARDUBOY_PUZZLE_SCREEN_WIDTH = 16;
		private const int ARDUBOY_PUZZLE_SCREEN_HEIGHT = 8;
		private const int LEVEL_WIDTH = 256;
		private const int LEVEL_HEIGHT = 32;
		public const int WALL_SPRITE_WIDTH = 8;
		public const int WALL_SPRITE_HEIGHT = 8;

		// the file name where the map is saved
		private string mFileName = string.Empty;

		private byte[,] mLevel = new byte[LEVEL_WIDTH, LEVEL_HEIGHT];

		// all the items in the level
		private Dictionary<Items.Item.Type, List<Items.Item>> mItems = new Dictionary<Items.Item.Type, List<Items.Item>>();

		private const int WALL_SPRITE_COUNT = 16;
		private Bitmap[] mWallSprites = new Bitmap[WALL_SPRITE_COUNT];
		private Bitmap[] mWallSpritesMirrored = new Bitmap[WALL_SPRITE_COUNT];

		private Pen mPuzzleScreenSeparatorLinePen = new Pen(Color.CornflowerBlue, 2);
		private Pen mValidPuzzlePathLinePen = new Pen(Color.LawnGreen, 4);
		private Pen mInvalidPuzzlePathLinePen = new Pen(Color.OrangeRed, 4);
		private Brush mMainMenuTextBrush = new SolidBrush(Color.FromArgb(60, Color.Wheat));

		// variable computed during the save of the map and needed to be exported
		private int mLevelWidth = 0;
		private int mLevelHeight = 0;
		private int mScreenCount = 0;
		private int mFirstPuzzleScreenCameraX = 0;
		private int mFirstPuzzleScreenCameraY = 0;

		// variable to measure the level memory size
		private int mLevelMemorySize = 0;
		private int mItemsMemorySize = 0;

		#region get/set
		public string FileName
		{
			get { return mFileName; }
		}

		public int DrawSpriteWidth
		{
			get { return WALL_SPRITE_WIDTH * mPixelSize; }
		}

		public int DrawSpriteHeight
		{
			get { return WALL_SPRITE_HEIGHT * mPixelSize; }
		}

		public int DrawPuzzleScreenWidth
		{
			get { return ARDUBOY_PUZZLE_SCREEN_WIDTH * WALL_SPRITE_WIDTH * mPixelSize; }
		}

		public int DrawPuzzleScreenHeight
		{
			get { return ARDUBOY_PUZZLE_SCREEN_HEIGHT * WALL_SPRITE_HEIGHT * mPixelSize; }
		}

		private int mPixelSize = 2;
		public int PixelSize
		{
			get { return mPixelSize; }
			set
			{
				mPixelSize = value;
				if (mPixelSize < 2)
					mPixelSize = 2;
			}
		}

		public bool IsPuzzlePathDrawn
		{
			get;
			set;
		}
		#endregion

		/// <summary>
		/// A small struct to store a puzzle id and coordinate for storing the puzzle path
		/// </summary>
		private struct PuzzlePathNode
		{
			public int screenId;
			public int screenX;
			public int screenY;

			public PuzzlePathNode(int id, int x, int y)
			{
				screenId = id;
				screenX = x;
				screenY = y;
			}

			public static bool operator ==(PuzzlePathNode obj1, PuzzlePathNode obj2)
			{
				return (obj1.screenX == obj2.screenX) && (obj1.screenY == obj2.screenY);
			}
			public static bool operator !=(PuzzlePathNode obj1, PuzzlePathNode obj2)
			{
				return (obj1.screenX != obj2.screenX) || (obj1.screenY != obj2.screenY);
			}
		}

		/// <summary>
		/// List of all the puzzle screens that create the puzzle path
		/// </summary>
		private List<PuzzlePathNode> mPuzzlePath = new List<PuzzlePathNode>();

		private enum WallId
		{
			ROCK_CEILING = 0,
			ROCK,
			ROCK_2,
			ROCK_GROUND,
			BLOCK_16_8_RIGHT,
			BLOCK_8_8,
			BLOCK_8_8_SPLIT,
			STAIR,
			ROCK_LEFT_WALL,
			BIG_STATUE_TOP,
			BIG_STATUE_BOTTOM,
			ROCK_CEILING_THIN,
			PLATFORM,
			LADDER,
			PLATFORM_WITH_LADDER,
			NOTHING = 15,
		};

		#region init
		public Map()
		{
			InitWallSpriteImages();
			Reset();
		}

		private Bitmap GetSprite(Bitmap originalImage, int x, int y, bool isMirrored)
		{
			Bitmap sprite = new Bitmap(8, 8);
			Graphics gc = Graphics.FromImage(sprite);
			gc.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
			gc.PixelOffsetMode = System.Drawing.Drawing2D.PixelOffsetMode.None;
			if (isMirrored)
			{
				gc.TranslateTransform(8, 0);
				gc.ScaleTransform(-1, 1);
			}
			gc.DrawImage(originalImage, new Rectangle(0, 0, 8, 8), new Rectangle(x * 8, y * 8, 8, 8), GraphicsUnit.Pixel);
			return sprite;
		}

		private void InitWallSpriteImages()
		{
			Bitmap originalImage = ImageProvider.GetWallSpriteImage();
			for (int y = 0; y < 8; ++y)
				for (int x = 0; x < 2; ++x)
				{
					int spriteId = y + (x * 8);
					mWallSprites[spriteId] = GetSprite(originalImage, x, y, false);
					mWallSpritesMirrored[spriteId] = GetSprite(originalImage, x, y, true);
				}
		}
		#endregion

		#region write

		List<Items.Item.Type>[] mSimilarTypes = new List<Items.Item.Type>[]
				{   new List<Items.Item.Type>(new Items.Item.Type[] { Items.Item.Type.BULLET_CRATE } ),
					new List<Items.Item.Type>(new Items.Item.Type[] { Items.Item.Type.DYNAMITE_CRATE } ),
					new List<Items.Item.Type>(new Items.Item.Type[] { Items.Item.Type.STATUETTE } ),
					new List<Items.Item.Type>(new Items.Item.Type[] { Items.Item.Type.GRAAL } ),
					new List<Items.Item.Type>(new Items.Item.Type[] { Items.Item.Type.HORIZONTAL_SPIKE, Items.Item.Type.VERTICAL_SPIKE } ),
					new List<Items.Item.Type>(new Items.Item.Type[] { Items.Item.Type.MUMMY, Items.Item.Type.SKELETON, Items.Item.Type.SCORPION }),
					new List<Items.Item.Type>(new Items.Item.Type[] { Items.Item.Type.STALAGMITE } ),
					new List<Items.Item.Type>(new Items.Item.Type[] { Items.Item.Type.STALACTITE } ),
					new List<Items.Item.Type>(new Items.Item.Type[] { Items.Item.Type.ARROW_LAUNCHER } ),
					new List<Items.Item.Type>(new Items.Item.Type[] { Items.Item.Type.DESTROYABLE_BLOCK } ),
					new List<Items.Item.Type>(new Items.Item.Type[] { Items.Item.Type.BOULDER } ),
				};

		private void WriteHeader(StreamWriter writer)
		{
			writer.WriteLine("/*");
			writer.WriteLine(" * This is a generated file, use the Editor to modify it.");
			writer.WriteLine(" */");
			writer.WriteLine();
			writer.WriteLine("#include \"MapData.h\"");
			writer.WriteLine("#include \"SpriteData.h\"");
			writer.WriteLine("#include \"MapManager.h\"");
			writer.WriteLine("#include \"GameManager.h\"");
			writer.WriteLine("#include \"ArrowLauncher.h\"");
			writer.WriteLine("#include \"Spike.h\"");
			writer.WriteLine("#include \"Statuette.h\"");
			writer.WriteLine("#include \"Dynamite.h\"");
			writer.WriteLine("#include \"DynamiteCrate.h\"");
			writer.WriteLine("#include \"Graal.h\"");
			writer.WriteLine("#include \"BulletCrate.h\"");
			writer.WriteLine("#include \"Enemy.h\"");
			writer.WriteLine("#include \"DestroyableBlock.h\"");
			writer.WriteLine("#include \"Stalactite.h\"");
			writer.WriteLine("#include \"Stalagmite.h\"");
			writer.WriteLine("#include \"Boulder.h\"");
			writer.WriteLine("#include <avr/pgmspace.h>");
			writer.WriteLine();
			writer.WriteLine("#define ID(id1,id2) ((id1<< 4) | id2)");
			writer.WriteLine();
		}

		private void WriteLevelId(StreamWriter writer, byte id, ref bool isWritingHighBit, ref int idCount)
		{
			if (isWritingHighBit)
			{
				writer.Write("ID(" + id.ToString() + ",");
				idCount++;
			}
			else
			{
				writer.Write(id.ToString() + "),");
			}
			// inverse the flag
			isWritingHighBit = !isWritingHighBit;
		}

		/// <summary>
		/// This function compute the bouding area of the level which countains something.
		/// It returns a bounding aligned on the puzzle screen.
		/// </summary>
		/// <param name="startX">the min x coord included</param>
		/// <param name="endX">the max x coord included</param>
		/// <param name="startY">the min y coord included</param>
		/// <param name="endY">the max y coord included</param>
		private void GetLevelBoundingBox(out int startX, out int endX, out int startY, out int endY)
		{
			// init the return values with the max and min values
			startX = LEVEL_WIDTH - 1;
			endX = 0;
			startY = LEVEL_HEIGHT - 1;
			endY = 0;
			// iterate on the whole level to find non emty cells
			bool isLevelEmpty = true;
			for (int y = 0; y < LEVEL_HEIGHT; ++y)
				for (int x = 0; x < LEVEL_WIDTH; ++x)
					if (mLevel[x, y] != (byte)WallId.NOTHING)
					{
						// we found something, get the puzzle screen coord where is located the sprite
						int minScreenX = (x / ARDUBOY_PUZZLE_SCREEN_WIDTH) * ARDUBOY_PUZZLE_SCREEN_WIDTH;
						int maxScreenX = minScreenX + ARDUBOY_PUZZLE_SCREEN_WIDTH - 1;
						int minScreenY = (y / ARDUBOY_PUZZLE_SCREEN_HEIGHT) * ARDUBOY_PUZZLE_SCREEN_HEIGHT;
						int maxScreenY = minScreenY + ARDUBOY_PUZZLE_SCREEN_HEIGHT - 1;
						// check if we found a coordinate better that what we already found
						if (minScreenX < startX)
							startX = minScreenX;
						if (maxScreenX > endX)
							endX = maxScreenX;
						if (minScreenY < startY)
							startY = minScreenY;
						if (maxScreenY > endY)
							endY = maxScreenY;
						// set the flag to tell that we found something, so the level is not empty
						isLevelEmpty = false;
					}

			// if the level is empty, init the start values
			if (isLevelEmpty)
			{
				startX = 0;
				startY = 0;
			}
		}

		private void WriteLevelData(StreamWriter writer)
		{
			// get the bounding volume of the data to export
			int startX;
			int endX;
			int startY;
			int endY;
			GetLevelBoundingBox(out startX, out endX, out startY, out endY);

			// declare a list to store the index of each beggining of lines
			List<int> lineIndex = new List<int>(endY - startY + 2);
			int idCount = 0;

			writer.WriteLine("const unsigned char MapManager::Level[] PROGMEM = {");
			for (int y = startY; y <= endY; ++y)
			{
				// add the index of the first id of the line in the list
				lineIndex.Add(idCount);
				// start with a tab
				writer.Write("\t");
				bool shouldCountSpace = false;
				bool isWritingHighBit = true;
				byte spaceCounter = 1;
				for (int x = startX; x <= endX; ++x)
				{
					// get the current id
					byte id = mLevel[x, y];
					if (shouldCountSpace)
					{
						if (id == (byte)WallId.NOTHING)
						{
							spaceCounter++;
							if (spaceCounter < 15)
							{
								continue;
							}
							else
							{
								WriteLevelId(writer, spaceCounter, ref isWritingHighBit, ref idCount);
								shouldCountSpace = false;
								spaceCounter = 1;
								continue;
							}
						}
						else
						{
							WriteLevelId(writer, spaceCounter, ref isWritingHighBit, ref idCount);
							shouldCountSpace = false;
							spaceCounter = 1;
						}
					}
					else if (id == (byte)WallId.NOTHING)
					{
						shouldCountSpace = true;
					}
					WriteLevelId(writer, id, ref isWritingHighBit, ref idCount);
				}
				// write the last low bit if we need to finish the line
				if (!isWritingHighBit)
				{
					if (shouldCountSpace)
						WriteLevelId(writer, spaceCounter, ref isWritingHighBit, ref idCount);
					else
						WriteLevelId(writer, 0, ref isWritingHighBit, ref idCount);
				}
				else if (shouldCountSpace)
				{
					// also if the last low bit was a space, write the space count in the next high and pad with a zero
					WriteLevelId(writer, spaceCounter, ref isWritingHighBit, ref idCount);
					WriteLevelId(writer, 0, ref isWritingHighBit, ref idCount);
				}
				writer.WriteLine();
			}
			writer.WriteLine("};");
			writer.WriteLine();

			// add the index of the first id of the last line
			lineIndex.Add(idCount);

			// memorize the level memory size. It's the number of id multiplied by the sizeof of char
			mLevelMemorySize = idCount;

			// write the line index array
			writer.Write("const unsigned int MapManager::LevelLineIndex[] PROGMEM = {");
			foreach (int index in lineIndex)
				writer.Write(index.ToString() + ",");
			writer.WriteLine("};");
			writer.WriteLine();

			// memorize the level size
			mLevelWidth = endX - startX + 1;
			mLevelHeight = endY - startY + 1;
		}

		private int GetPuzzleScreenIdFromGameWorldCoord(int x, int y)
		{
			return ((y / (ARDUBOY_PUZZLE_SCREEN_HEIGHT * WALL_SPRITE_HEIGHT)) * (LEVEL_WIDTH / ARDUBOY_PUZZLE_SCREEN_WIDTH)) + (x / (ARDUBOY_PUZZLE_SCREEN_WIDTH * WALL_SPRITE_WIDTH));
		}

		private void GetPuzzleScreenCoordFromGameWorldCoord(int x, int y, out int screenX, out int screenY)
		{
			screenX = (x / (ARDUBOY_PUZZLE_SCREEN_WIDTH * WALL_SPRITE_WIDTH)) * ARDUBOY_PUZZLE_SCREEN_WIDTH;
			screenY = (y / (ARDUBOY_PUZZLE_SCREEN_HEIGHT * WALL_SPRITE_HEIGHT)) * ARDUBOY_PUZZLE_SCREEN_HEIGHT;
		}

		private int GetMaxItemCount(List<Items.Item.Type> itemTypes)
		{
			int[] maxCountPerSreen = new int[(LEVEL_WIDTH / ARDUBOY_PUZZLE_SCREEN_WIDTH) * (LEVEL_HEIGHT /ARDUBOY_PUZZLE_SCREEN_HEIGHT)];

			// iterate on the list of all the item of the specified type
			foreach (Items.Item.Type itemType in itemTypes)
				if (mItems.ContainsKey(itemType))
					foreach (Items.Item item in mItems[itemType])
						maxCountPerSreen[GetPuzzleScreenIdFromGameWorldCoord(item.X, item.Y)]++;

			// then iterate on the max count to find the highest
			int maxCount = 0;
			foreach (int count in maxCountPerSreen)
				if (maxCount < count)
					maxCount = count;

			// return the max found
			return maxCount;
		}

		private void WriteItemInstances(StreamWriter writer)
		{
			// reset the total item size
			mItemsMemorySize = 0;
			for (int i = 0; i < mSimilarTypes.Length; ++i)
			{
				// get the max number of instances that we need 
				int maxItemCount = GetMaxItemCount(mSimilarTypes[i]);
				for (int j = 1; j <= maxItemCount; ++j)
					Items.Item.WriteInstance(writer, mSimilarTypes[i][0], j);
				// increase the total item size depending on the number of instances and the size of the item
				mItemsMemorySize += Items.Item.GetMemorySize(mSimilarTypes[i][0], maxItemCount);
			}
		}

		private List<Items.Item> GetItemsOnScreen(int left, int top)
		{
			List<Items.Item> result = new List<Items.Item>();
			// convert the scren coord to game world coord
			int leftX = left * WALL_SPRITE_WIDTH;
			int rightX = (left + ARDUBOY_PUZZLE_SCREEN_WIDTH ) * WALL_SPRITE_WIDTH;
			int topX = top * WALL_SPRITE_HEIGHT;
			int bottomX  = (top + ARDUBOY_PUZZLE_SCREEN_HEIGHT) * WALL_SPRITE_HEIGHT;
			// iterate on all items to find those inside the screen
			foreach (List<Items.Item> itemList in mItems.Values)
				foreach (Items.Item item in itemList)
					if (item.IsInPuzzleScreen(leftX, topX, rightX, bottomX))
						result.Add(item);
			// return the list
			return result;
		}
		private int ComputeMaxItemCount()
		{
			int maxItemCount = 1; // start with minimum one, to avoid creating an array of size zero (which doesn't compile)
			int previousScreenItemCount = 0;

			// iterate on the puzzle path, and sum up items of two consecutive screens
			for (int i = 0; i < mPuzzlePath.Count; ++i)
			{
				// get the current screen path node
				PuzzlePathNode screenPathNode = mPuzzlePath[i];

				// get the items of the current screen
				List<Items.Item> itemList = GetItemsOnScreen(screenPathNode.screenX, screenPathNode.screenY);

				// count the number of items in the current list, if there is arrow launcher, we need to add 1 for the arrow
				// also don't count the RICK items because they are just spawn points
				int currenScreenItemCount = itemList.Count;
				foreach (Items.Item item in itemList)
					if (item.ItemType == Items.Item.Type.ARROW_LAUNCHER)
						currenScreenItemCount++;
					else if (item.ItemType == Items.Item.Type.RICK)
						currenScreenItemCount--;

				// calculate the count for this screen and the previous one
				int currentItemCountOnTwoScreens = previousScreenItemCount + currenScreenItemCount;
				if (currentItemCountOnTwoScreens > maxItemCount)
					maxItemCount = currentItemCountOnTwoScreens;

				// put the count of the current screen in the previous one for the next iteration
				previousScreenItemCount = currenScreenItemCount;
			}

			// check the total number of item instances that we have in the game
			// don't count RICK item types as they don't create instances
			// count double for ArrowLauncher as they need a space for their arrow
			int totalInstanceCount = 0;
			for (int i = 0; i < mSimilarTypes.Length; ++i)
				if (mSimilarTypes[i][0] == Items.Item.Type.ARROW_LAUNCHER)
					totalInstanceCount += GetMaxItemCount(mSimilarTypes[i]) * 2;
				else if (mSimilarTypes[i][0] != Items.Item.Type.RICK)
					totalInstanceCount += GetMaxItemCount(mSimilarTypes[i]);

			// check if the max item count is greater than the actual number of instances,
			// in such a case, take the number of instances, because we cannot have more in the array than the number of instances
			if (maxItemCount > totalInstanceCount)
				maxItemCount = totalInstanceCount;

			// Rick as 5 bullets and 5 dynamite, so add with 10 if he uses them all at the same time
			maxItemCount += Properties.Settings.Default.BulletAndDynamiteCount;

			// return the computed value
			return maxItemCount;
		}

		private int IncreaseItemCounterAndGetId(ref int[] itemCount, Items.Item item)
		{
			// search in which Similar Item group the specified item belongs to
			for (int i = 0; i < mSimilarTypes.Length; ++i)
				if (mSimilarTypes[i].Contains(item.ItemType))
				{
					// increase the counter
					itemCount[i]++;
					return itemCount[i];
				}
			return 0;
		}

		private void WriteInitFunctionForOneScreen(StreamWriter writer, int screenId, int screenLeft, int screenTop, bool isMainMenuScreen)
		{
			// get all the items on the specified screen
			List<Items.Item> itemsOnScreen = GetItemsOnScreen(screenLeft, screenTop);
			List<Items.Item> itemsOnScreenWhilePlaying = new List<Items.Item>();

			// special case the main menu, we split the items in two list
			if (isMainMenuScreen)
			{
				for (int i = 0; i < itemsOnScreen.Count; ++i)
				{
					Items.Item item = itemsOnScreen[i];
					if (item.ItemType != Items.Item.Type.RICK && item.ItemType != Items.Item.Type.STALACTITE && item.ItemType != Items.Item.Type.STALAGMITE)
					{
						itemsOnScreenWhilePlaying.Add(item);
						itemsOnScreen.RemoveAt(i);
						--i;
					}
				}
			}

			// declare an array to count the items per type for the current screen
			int[] itemCount = new int[mSimilarTypes.Length];

			// begin of the init function
			writer.WriteLine();
			writer.WriteLine("void InitScreen" + screenId.ToString() + "(bool shouldRespawn)");
			writer.WriteLine("{");

			// add the items in the map manager
			writer.WriteLine("\t// Add a checkpoint if we need to");
			foreach (Items.Item item in itemsOnScreen)
			{				
				item.WriteCheckpoint(writer, IncreaseItemCounterAndGetId(ref itemCount, item));
				item.WasSaved = true;
			}
			writer.WriteLine();

			// reset the item count array
			Array.Clear(itemCount, 0, itemCount.Length);

			// init the position of the item
			writer.WriteLine("\t// init all the item of the current puzzle screen");
			foreach (Items.Item item in itemsOnScreen)
			{
				item.WriteInit(writer, IncreaseItemCounterAndGetId(ref itemCount, item));
				item.WasSaved = true;
			}

			// special hard coded case for the level 0 (the main menu), init only some specific items in playing state of the game
			if (isMainMenuScreen)
			{
				writer.WriteLine();
				writer.WriteLine("\t// The following items must only be init for the game, and not in the main menu");
				writer.WriteLine("\tif (GameManager::CurrentGameState == GameManager::PLAYING)");
				writer.WriteLine("\t{");
				foreach (Items.Item item in itemsOnScreenWhilePlaying)
				{
					writer.Write("\t");
					item.WriteInit(writer, IncreaseItemCounterAndGetId(ref itemCount, item));
					item.WasSaved = true;
				}
				writer.WriteLine("\t}");
			}

			// finish the init function
			writer.WriteLine("}");
		}

		private bool IsAPuzzleScreenExit(byte wallId)
		{
			return (wallId == (byte)WallId.NOTHING) || (wallId == (byte)WallId.LADDER) || (wallId == (byte)WallId.PLATFORM_WITH_LADDER);
		}

		private bool GetStartOrEndPuzzleScreenCoordinates(bool isStart, out int screenX, out int screenY)
		{
			// default value if we didn't find the special respawn point
			screenX = 0;
			screenY = 0;

			// configure the type of item to search (either RICK for start or GRAAL for end)
			Items.Item.Type itemTypeToSearch = isStart ? Items.Item.Type.RICK : Items.Item.Type.GRAAL;

			// search in the rick item the one that match the specified parameter
			if (mItems.ContainsKey(itemTypeToSearch))
				foreach (Items.Item respawnItem in mItems[itemTypeToSearch])
					if (!isStart || (respawnItem.RickRespawnType == Items.Item.RespawnType.START))
					{
						GetPuzzleScreenCoordFromGameWorldCoord(respawnItem.X, respawnItem.Y, out screenX, out screenY);
						return true;
					}
			return false;
		}

		private int WriteInitFunctions(StreamWriter writer)
		{
			// first clear all the flag that tell that an item has been saved
			foreach (KeyValuePair<Items.Item.Type, List<Items.Item>> itemList in mItems)
				foreach (Items.Item item in itemList.Value)
					item.WasSaved = false;

			// write the init function of the Main Menu
			WriteInitFunctionForOneScreen(writer, 0, 0, 0, true);

			// rebuild the path
			BuildPuzzlePath();

			// write the init function for all the screens
			for (int i = 0; i < mPuzzlePath.Count; ++i)
			{
				// get the current screen path node
				PuzzlePathNode screenPathNode = mPuzzlePath[i];

				// special hard code case, if one of the screen of the puzzle path, is the main menu, then just skip it
				// because we already wrote the init funtion
				if ((screenPathNode.screenX == 0) && (screenPathNode.screenY == 0))
				{
					mPuzzlePath[i] = new PuzzlePathNode(screenPathNode.screenX, screenPathNode.screenY, 0);
					continue;
				}

				// write the init function for the current path node
				WriteInitFunctionForOneScreen(writer, screenPathNode.screenId, screenPathNode.screenX, screenPathNode.screenY, false);
			}

			// return the number of screens
			return mPuzzlePath.Count + 1;
		}

		private void WriteInitFunctionArray(StreamWriter writer)
		{
			// write the array of init function
			writer.WriteLine();
			writer.WriteLine("// The array that contains all the items");
			writer.WriteLine("ItemInitFunction MapManager::ItemInitFunctions[] = {");
			writer.Write("\t");
			// init the Screen 0 (which is the main menu) which is not in the mPuzzlePath
			writer.Write("&InitScreen0, ");
			// init the other screens
			foreach (PuzzlePathNode screenPathNode in mPuzzlePath)
				writer.Write("&InitScreen" + screenPathNode.screenId.ToString() + ", ");
			writer.WriteLine();
			writer.WriteLine("};");
			writer.WriteLine();
		}

		private void WriteSaveAndLoadAliveStatusFunction(StreamWriter writer)
		{
			writer.WriteLine();
			writer.WriteLine("// this function is to save and load the living status of the items");
			writer.WriteLine("void MapManager::SaveAndLoadAliveStatusForAllItems(unsigned char currentScreenIdToSave, unsigned char newScreenIdToLoad)");
			writer.WriteLine("{");

			int instanceCount = 0;
			for (int i = 0; i < mSimilarTypes.Length; ++i)
			{
				// get the max number of instances that we need 
				int maxItemCount = GetMaxItemCount(mSimilarTypes[i]);
				for (int j = 1; j <= maxItemCount; ++j)
					if (Items.Item.WriteSaveAndLoadLivingStatus(writer, mSimilarTypes[i][0], j, instanceCount))
						instanceCount++;
			}

			writer.WriteLine("}");
		}

		private void WriteNotUsedItems(StreamWriter writer)
		{
			writer.WriteLine();
			writer.WriteLine("// The following items are not used in the game, but they are written here,");
			writer.WriteLine("// in order for the Editor to reload them.");
			writer.WriteLine("/*");

			// Iterate on all the items and saved those that have not been saved
			foreach (KeyValuePair<Items.Item.Type, List<Items.Item>> itemList in mItems)
				foreach (Items.Item item in itemList.Value)
					if (!item.WasSaved)
					{
						item.WriteInit(writer, 1);
						item.WasSaved = true;
					}

			writer.WriteLine("*/");
		}

		private bool SaveMapData(string mapDataFileName)
		{
			bool wasSaveOk = true;
			System.Text.Encoding utf8WithoutBom = new System.Text.UTF8Encoding(false);
			StreamWriter writer = new StreamWriter(mapDataFileName, false, utf8WithoutBom);
			try
			{
				WriteHeader(writer);
				WriteLevelData(writer);
				WriteItemInstances(writer);
				mScreenCount = WriteInitFunctions(writer);
				WriteInitFunctionArray(writer);
				WriteSaveAndLoadAliveStatusFunction(writer);
				WriteNotUsedItems(writer); // to save them, if you want to reload the map without loosing them
			}
			catch (MapSaveException)
			{
				wasSaveOk = false;
			}
			writer.Flush();
			writer.Close();
			// return the save status
			return wasSaveOk;
		}

		private bool SaveConstVariables(string constVariablesFileName)
		{
			bool wasSaveOk = true;
			System.Text.Encoding utf8WithoutBom = new System.Text.UTF8Encoding(false);
			StreamWriter writer = new StreamWriter(constVariablesFileName, false, utf8WithoutBom);
			try
			{
				writer.WriteLine("#ifndef _MAP_DATA_CONST_VARIABLES_H_");
				writer.WriteLine("#define _MAP_DATA_CONST_VARIABLES_H_");
				writer.WriteLine();
				writer.WriteLine("/*");
				writer.WriteLine(" * Warning this file was generated by the Level Editor");
				writer.WriteLine("*/");
				writer.WriteLine();
				writer.WriteLine("namespace MapManager");
				writer.WriteLine("{");
				writer.WriteLine("\t// size of the level");
				writer.WriteLine("\tstatic constexpr int LEVEL_WIDTH = " + mLevelWidth.ToString() + ";");
				writer.WriteLine("\tstatic constexpr int LEVEL_HEIGHT = " + mLevelHeight.ToString() + ";");
				writer.WriteLine();
				writer.WriteLine("\t// number of puzzle screen");
				writer.WriteLine("\tstatic constexpr int PUZZLE_SCREEN_COUNT = " + mScreenCount.ToString() + ";");
				int screenByteCount = (mScreenCount >> 3) + 1;
				if ((mScreenCount % 8) == 0)
					screenByteCount--;
				writer.WriteLine("\tstatic constexpr int PUZZLE_SCREEN_BYTE_COUNT = " + screenByteCount.ToString() + ";");
				writer.WriteLine();
				writer.WriteLine("\t// coordinates of the first puzzle screen and main menu");
				writer.WriteLine("\tstatic constexpr int MAIN_MENU_CAMERA_X = 0;");
				writer.WriteLine("\tstatic constexpr int MAIN_MENU_CAMERA_Y = 0;");
				writer.WriteLine("\tstatic constexpr int FIRST_PUZZLE_SCREEN_CAMERA_X = " + mFirstPuzzleScreenCameraX.ToString() + ";");
				writer.WriteLine("\tstatic constexpr int FIRST_PUZZLE_SCREEN_CAMERA_Y = " + mFirstPuzzleScreenCameraY.ToString() + ";");
				writer.WriteLine();
				writer.WriteLine("\t// Number of statuettes to collect");
				int statuetteCount = 0;
				if (mItems.ContainsKey(Items.Item.Type.STATUETTE))
					statuetteCount = mItems[Items.Item.Type.STATUETTE].Count;
				writer.WriteLine("\tstatic constexpr int MAX_STATUETTE_COUNT = " + statuetteCount.ToString() + ";");
				writer.WriteLine();
				writer.WriteLine("\t// Max Number of Item that can be updated at the same time");
				int maxItemCount = ComputeMaxItemCount();
				writer.WriteLine("\tstatic constexpr int MAX_UPDATABLE_ITEM_COUNT = " + maxItemCount.ToString() + ";");
				writer.WriteLine("};");
				writer.WriteLine();
				writer.WriteLine("#endif");
			}
			catch (MapSaveException)
			{
				wasSaveOk = false;
			}
			writer.Flush();
			writer.Close();
			// return the save status
			return wasSaveOk;
		}

		public void Save(string mapDataFileName, string constVariablesFileName)
		{
			bool wasSaveOk = true;
			wasSaveOk = SaveMapData(mapDataFileName);
			wasSaveOk = SaveConstVariables(constVariablesFileName) && wasSaveOk;
			// memorise the name with which the map was saved
			mFileName = mapDataFileName;
			// log that the map was correctly saved
			if (wasSaveOk)
			{
				int totalSize = 19986 + mLevelMemorySize + mItemsMemorySize;
				string message = Properties.Resources.LogMapWasSaveSuccessfully.Replace("¤¤¤", totalSize.ToString()).Replace("¤¤", mItemsMemorySize.ToString()).Replace("¤", mLevelMemorySize.ToString());
				MainForm.LogMessage(message, MainForm.LogLevel.GOOD);
			}
		}
		#endregion

		#region read
		private void ReadLevelData(StreamReader reader)
		{
			int x = 0;
			int y = 0;
			string line = reader.ReadLine();
			while (!reader.EndOfStream && !line.Contains("}"))
			{
				string[] tokens = line.Split(new string[] { "ID" }, StringSplitOptions.RemoveEmptyEntries);
				bool nextId1IsCount = false;
				for (int t = 0; t < tokens.Length; ++t)
				{
					// get the current token
					string token = tokens[t];
					// check what it is
					if (token.StartsWith("("))
					{
						string[] ids = token.Split(new char[] { ',' });
						int id1 = int.Parse(ids[0].Substring(1));
						int id2 = int.Parse(ids[1].Remove(ids[1].IndexOf(')')));
						// check id1 first
						if (nextId1IsCount)
						{
							x += id1;
							nextId1IsCount = false;
						}
						else if (id1 == (int)WallId.NOTHING)
						{
							x += id2;
							continue;
						}
						else
						{
							mLevel[x, y] = (byte)id1;
							x++;
						}
						// check id2
						if (id2 == (int)WallId.NOTHING)
						{
							nextId1IsCount = true;
						}
						else
						{
							// special case for the last id of the line, it may be just a padding value
							if ((t < tokens.Length - 1) || ((x % ARDUBOY_PUZZLE_SCREEN_WIDTH) != 0))
								mLevel[x, y] = (byte)id2;
							x++;
						}
					}
				}
				// read the next line
				line = reader.ReadLine();
				y++;
				x = 0;
			}
		}

		private void ReadItemInit(StreamReader reader, string line)
		{
			const string propertyFlagType = "Item::PropertyFlags::";
			const string initFunctionName = ".Init(";
			string instanceName = line.Remove(line.IndexOf(initFunctionName)).Trim();
			string[] tokens = line.Substring(line.IndexOf(initFunctionName) + initFunctionName.Length).Split(new char[] { ',', ')', '|' });

			// declare some boolean variable to store the property flags
			bool isSpecial = false;
			bool isSpecial2 = false;
			bool isMirror = false;

			// parse the two coordinate of the checkpoint respawn
			int intValueIndex = 0;
			int[] intValues = new int[] { 0, 0, 0 };
			foreach (string token in tokens)
			{
				if ((intValueIndex < intValues.Length) && int.TryParse(token, out intValues[intValueIndex]))
				{
					intValueIndex++;
				}
				else if (token.Contains(propertyFlagType))
				{
					isSpecial = isSpecial || (!token.Contains("SPECIAL_2") && token.Contains("SPECIAL"));
					isSpecial2 = isSpecial2 || token.Contains("SPECIAL_2");
					isMirror = isMirror || token.Contains("MIRROR_X");
				}
			}

			// instantiate the correct item
			try
			{
				AddItem(CreateItem(Items.Item.GetInstanceTypeFromName(instanceName, isSpecial, isSpecial2), isMirror, isSpecial, new Point(intValues[0], intValues[1]), intValues[2]));
			}
			catch (MapSaveException)
			{
				// ignore exception for example if there's more than one graal in the Map
			}
		}

		private void ReadCheckpointRespawn(StreamReader reader, string line)
		{
			const string checkpointFunctionName = "MemorizeCheckPoint(";
			string[] tokens = line.Substring(line.IndexOf(checkpointFunctionName) + checkpointFunctionName.Length).Split(new char[] { ',', ')' });

			// parse the two coordinate of the checkpoint respawn
			int coordIndex = 0;
			int[] coord = new int[]{ 0, 0 };
			foreach (string token in tokens)
				if (int.TryParse(token, out coord[coordIndex]))
				{
					coordIndex++;
					if (coordIndex >= coord.Length)
						break;
				}

			// instantiate a RICK item
			AddItem(CreateItem(Items.Item.Type.RICK, false, false, new Point(coord[0], coord[1])));
		}

		public void Load(string mapDataFileName)
		{
			// clear the level
			Reset();
			// and read the cpp file
			StreamReader reader = new StreamReader(mapDataFileName, System.Text.Encoding.UTF8);
			while (!reader.EndOfStream)
			{
				string line = reader.ReadLine();
				if (line.Contains("MapManager::Level[]"))
					ReadLevelData(reader);
				else if (line.Contains(".Init("))
					ReadItemInit(reader, line);
				else if (line.Contains("MemorizeCheckPoint"))
					ReadCheckpointRespawn(reader, line);
			}
			reader.Close();

			// set the first rick respawn type
			if (mItems.ContainsKey(Items.Item.Type.RICK))
			{
				List<Items.Item> respawnList = mItems[Items.Item.Type.RICK];
				if (respawnList.Count > 0)
					respawnList[0].RickRespawnType = Items.Item.RespawnType.START;
			}

			// memorise the name with which the map was loaded
			mFileName = mapDataFileName;
		}
		#endregion

		#region util function
		public void ClampCoordinatesInsideLevel(ref Point coord)
		{
			coord.X = Math.Max(Math.Min(coord.X, LEVEL_WIDTH), 0);
			coord.Y = Math.Max(Math.Min(coord.Y, LEVEL_HEIGHT), 0);
		}

		public Point GetSpriteCoordFromScreenCoord(Point cameraLocation, Point screenLocation)
		{
			// iterate from the camera position, until we reach the correct screen coordinate in x
			int mapX = cameraLocation.X - 1;
			int screenX = 0;
			while (screenX <= screenLocation.X)
			{
				mapX++;
				screenX += DrawSpriteWidth;
				if ((mapX % ARDUBOY_PUZZLE_SCREEN_WIDTH) == 0)
					screenX += (int)mPuzzleScreenSeparatorLinePen.Width;
			}
			// and in y
			int mapY = cameraLocation.Y - 1;
			int screenY = 0;
			while (screenY <= screenLocation.Y)
			{
				mapY++;
				screenY += DrawSpriteHeight;
				if ((mapY % ARDUBOY_PUZZLE_SCREEN_HEIGHT) == 0)
					screenY += (int)mPuzzleScreenSeparatorLinePen.Width;
			}
			// return the point computed
			return new Point(mapX, mapY);
		}

		private void BuildPuzzlePath()
		{
			PuzzlePathNode emptyNode = new PuzzlePathNode(-1, -1, -1);

			// first let the algorithm search from the first exit that it can find
			bool isPathFound = BuildPuzzlePath(emptyNode);

			// no path was found but at least a path was started, then  search again, from the other exit (so excluding the second puzzle screen)
			if (!isPathFound && (mPuzzlePath.Count >= 2))
				isPathFound = BuildPuzzlePath(mPuzzlePath[1]);

			// if path was still not found on the other exit then throw the exception
			if (!isPathFound)
			{
				int screenX = 0;
				int screenY = 0;
				if (mPuzzlePath.Count > 0)
				{
					screenX = mPuzzlePath[mPuzzlePath.Count - 1].screenX;
					screenY = mPuzzlePath[mPuzzlePath.Count - 1].screenY;
				}
				throw new MapSaveException(Properties.Resources.ErrorDeadEndNotExitFound, MainForm.LogLevel.ERROR, screenX.ToString(), screenY.ToString());
			}
		}

		private bool BuildPuzzlePath(PuzzlePathNode pathNodeToExclude)
		{
			// get the coordinate of the start and End screen
			int endScreenX;
			int endScreenY;
			if (!GetStartOrEndPuzzleScreenCoordinates(true, out mFirstPuzzleScreenCameraX, out mFirstPuzzleScreenCameraY))
				throw new MapSaveException(Properties.Resources.ErrorNoStartPuzzleScreen, MainForm.LogLevel.ERROR);
			if (!GetStartOrEndPuzzleScreenCoordinates(false, out endScreenX, out endScreenY))
				throw new MapSaveException(Properties.Resources.ErrorNoEndPuzzleScreen, MainForm.LogLevel.ERROR);

			// clear the puzzle path before rebuilding it
			mPuzzlePath.Clear();

			// init the current screen coord with the start one
			int currentScreenId = 1;
			PuzzlePathNode currentScreen = new PuzzlePathNode(currentScreenId, mFirstPuzzleScreenCameraX, mFirstPuzzleScreenCameraY);
			PuzzlePathNode previousScreen = new PuzzlePathNode(currentScreenId, mFirstPuzzleScreenCameraX, mFirstPuzzleScreenCameraY);
			while ((currentScreen.screenX != endScreenX) || (currentScreen.screenY != endScreenY))
			{
				// check if we didn't already exported the current puzzle screen, otherwise that means we have a cyclic path
				if (mPuzzlePath.Contains(currentScreen))
					throw new MapSaveException(Properties.Resources.ErrorCyclicPuzzlePathDetected, MainForm.LogLevel.ERROR);

				// set the current screen id and add it in the path
				mPuzzlePath.Add(new PuzzlePathNode(currentScreenId, currentScreen.screenX, currentScreen.screenY));

				// increase the current screen id
				currentScreenId++;

				// get the next screen coordinates above or below
				bool isExitFound = false;
				for (int x = currentScreen.screenX + 1; x < currentScreen.screenX + ARDUBOY_PUZZLE_SCREEN_WIDTH - 1; ++x)
				{
					if ((currentScreen.screenY > 0) &&
						(currentScreen.screenY - ARDUBOY_PUZZLE_SCREEN_HEIGHT != previousScreen.screenY) &&
						((pathNodeToExclude.screenX != currentScreen.screenX) || (pathNodeToExclude.screenY != currentScreen.screenY - ARDUBOY_PUZZLE_SCREEN_HEIGHT)) &&
						IsAPuzzleScreenExit(mLevel[x, currentScreen.screenY]))
					{
						previousScreen.screenX = currentScreen.screenX;
						previousScreen.screenY = currentScreen.screenY;
						currentScreen.screenY -= ARDUBOY_PUZZLE_SCREEN_HEIGHT;
						isExitFound = true;
						break;
					}
					if ((currentScreen.screenY < LEVEL_HEIGHT - ARDUBOY_PUZZLE_SCREEN_HEIGHT) &&
						(currentScreen.screenY + ARDUBOY_PUZZLE_SCREEN_HEIGHT != previousScreen.screenY) &&
						((pathNodeToExclude.screenX != currentScreen.screenX) || (pathNodeToExclude.screenY != currentScreen.screenY + ARDUBOY_PUZZLE_SCREEN_HEIGHT)) &&
						IsAPuzzleScreenExit(mLevel[x, currentScreen.screenY + ARDUBOY_PUZZLE_SCREEN_HEIGHT - 1]))
					{
						previousScreen.screenX = currentScreen.screenX;
						previousScreen.screenY = currentScreen.screenY;
						currentScreen.screenY += ARDUBOY_PUZZLE_SCREEN_HEIGHT;
						isExitFound = true;
						break;
					}
				}

				// get the next screen coordinates on left or right
				if (!isExitFound)
				{
					for (int y = currentScreen.screenY + 1; y < currentScreen.screenY + ARDUBOY_PUZZLE_SCREEN_HEIGHT - 1; ++y)
					{
						if ((currentScreen.screenX > 0) &&
							(currentScreen.screenX - ARDUBOY_PUZZLE_SCREEN_WIDTH != previousScreen.screenX) &&
							((pathNodeToExclude.screenX != currentScreen.screenX - ARDUBOY_PUZZLE_SCREEN_WIDTH) || (pathNodeToExclude.screenY != currentScreen.screenY)) &&
							IsAPuzzleScreenExit(mLevel[currentScreen.screenX, y]))
						{
							previousScreen.screenX = currentScreen.screenX;
							previousScreen.screenY = currentScreen.screenY;
							currentScreen.screenX -= ARDUBOY_PUZZLE_SCREEN_WIDTH;
							isExitFound = true;
							break;
						}
						if ((currentScreen.screenX < LEVEL_WIDTH - ARDUBOY_PUZZLE_SCREEN_WIDTH) &&
							(currentScreen.screenX + ARDUBOY_PUZZLE_SCREEN_WIDTH != previousScreen.screenX) &&
							((pathNodeToExclude.screenX != currentScreen.screenX + ARDUBOY_PUZZLE_SCREEN_WIDTH) || (pathNodeToExclude.screenY != currentScreen.screenY)) &&
							IsAPuzzleScreenExit(mLevel[currentScreen.screenX + ARDUBOY_PUZZLE_SCREEN_WIDTH - 1, y]))
						{
							previousScreen.screenX = currentScreen.screenX;
							previousScreen.screenY = currentScreen.screenY;
							currentScreen.screenX += ARDUBOY_PUZZLE_SCREEN_WIDTH;
							isExitFound = true;
							break;
						}
					}
				}

				// if we didn't found any exit, and didn't reach the last screen, we are in a dead end!
				if (!isExitFound && ((currentScreen.screenX != endScreenX) || (currentScreen.screenY != endScreenY)))
					return false;
			}

			// add the last screen
			mPuzzlePath.Add(new PuzzlePathNode(currentScreenId, currentScreen.screenX, currentScreen.screenY));

			// we found a path, so return true
			return true;
		}

		#endregion

		#region map edition
		public void Reset()
		{
			// clear the level
			for (int x = 0; x < LEVEL_WIDTH; ++x)
				for (int y = 0; y < LEVEL_HEIGHT; ++y)
					mLevel[x, y] = (byte)WallId.NOTHING;

			// clear all the items
			mItems.Clear();

			// clear also the file name
			mFileName =	string.Empty;
		}

		public void SetSpriteId(Point coord, byte id)
		{
			if ((coord.X >= 0) && (coord.X < LEVEL_WIDTH) && (coord.Y >= 0) && (coord.Y < LEVEL_HEIGHT))
				mLevel[coord.X, coord.Y] = id;
		}

		public byte GetSpriteIdAt(Point coord)
		{
			return mLevel[coord.X, coord.Y];
		}

		public Items.Item GetItemAt(Point location)
		{
			foreach (List<Items.Item> itemList in mItems.Values)
				foreach (Items.Item item in itemList)
					if (item.IsUnder(location.X, location.Y))
						return item;
			return null;
		}

		public Items.Item CreateItem(Items.Item.Type itemType, bool isMirrored, bool isSpecial, Point location, int additionalParameter = 80)
		{
			// add the list of items of the specified type if not already in the dictionary
			if (!mItems.ContainsKey(itemType))
				mItems.Add(itemType, new List<Items.Item>());

			// special case for the graal, there can be only one in the Map
			if ((itemType == Items.Item.Type.GRAAL) && (mItems[Items.Item.Type.GRAAL].Count == 1))
				throw new MapSaveException(Properties.Resources.ErrorMapCanOnlyHaveOneGraal, MainForm.LogLevel.WARNING);

			// add the item in the correct list and return it
			return new Items.Item(itemType, isMirrored, isSpecial, location.X, location.Y, additionalParameter);
		}

		public Items.Item AddItem(Items.Item itemToAdd)
		{
			mItems[itemToAdd.ItemType].Add(itemToAdd);
			return itemToAdd;
		}

		public void RemoveItem(Items.Item itemToRemove)
		{
			if (mItems.ContainsKey(itemToRemove.ItemType))
				mItems[itemToRemove.ItemType].Remove(itemToRemove);
		}
		#endregion

		#region draw
		public void Redraw(Graphics gc, int width, int height, int cameraX, int cameraY, Items.Item selectedItem)
		{
			// compute the limit of the camera coordinates
			Point startCamera;
			Point endCamera;
			int availableSpriteCountX;
			int availableSpriteCountY;
			ComputeCameraCoordinateLimits(width, height, cameraX, cameraY, out startCamera, out endCamera, out availableSpriteCountX, out availableSpriteCountY);

			// draw the various element on screen
			DrawLevelWalls(gc, width, height, startCamera, endCamera, availableSpriteCountX, availableSpriteCountY);
			// Draw the main menu for the first screen
			DrawMainMenu(gc, startCamera.X, startCamera.Y);
			// Draw all the items
			DrawItems(gc, width, height, startCamera.X, startCamera.Y, selectedItem);
			// draw a line following all the puzzle chain
			if (IsPuzzlePathDrawn)
				DrawPuzzlePath(gc, width, height, startCamera.X, startCamera.Y);
		}

		private void ComputeCameraCoordinateLimits(int width, int height, int cameraX, int cameraY, out Point startCamera, out Point endCamera, out int availableSpriteCountX, out int availableSpriteCountY)
		{
			// compute the number of puzzle screens that can be displayed in the available space
			int horizontalPuzzleScreenCount = width / DrawSpriteWidth / ARDUBOY_PUZZLE_SCREEN_WIDTH;
			int verticalPuzzleScreenCount = height / DrawSpriteWidth / ARDUBOY_PUZZLE_SCREEN_WIDTH;

			// compute the number of sprite that we can draw on the available space
			availableSpriteCountX = 1 + ((width - ((int)mPuzzleScreenSeparatorLinePen.Width * horizontalPuzzleScreenCount)) / DrawSpriteWidth);
			availableSpriteCountY = 1 + ((height - ((int)mPuzzleScreenSeparatorLinePen.Width * verticalPuzzleScreenCount)) / DrawSpriteHeight);

			// compute the last sprite to be drawn
			endCamera = new Point(cameraX + availableSpriteCountX, cameraY + availableSpriteCountY);
			ClampCoordinatesInsideLevel(ref endCamera);

			// avoid to start drawing too close to the end
			int maxCameraX = endCamera.X - availableSpriteCountX;
			if (cameraX > maxCameraX)
				cameraX = maxCameraX;
			int maxCameraY = endCamera.Y - availableSpriteCountY;
			if (cameraY > maxCameraY)
				cameraY = maxCameraY;
			startCamera = new Point(cameraX, cameraY);
			ClampCoordinatesInsideLevel(ref startCamera);

			// shift the camera by one line, if we need to draw the bottom of the level
			if ((endCamera.Y == LEVEL_HEIGHT) && (startCamera.Y > 0))
				startCamera.Y++;
			// shift the camera by one line, if we need to draw the right of the level
			if ((endCamera.X == LEVEL_WIDTH) && (startCamera.X > 0))
				startCamera.X++;
		}

		private void DrawLevelWalls(Graphics gc, int width, int height, Point startCamera, Point endCamera, int availableSpriteCountX, int availableSpriteCountY)
		{
			// set the drawing mode
			gc.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
			gc.PixelOffsetMode = System.Drawing.Drawing2D.PixelOffsetMode.Half;

			// compute the width and height of the lines
			availableSpriteCountX = endCamera.X - startCamera.X;
			int lineWidth = Math.Min(width, (DrawSpriteWidth * availableSpriteCountX) + ((int)mPuzzleScreenSeparatorLinePen.Width * (availableSpriteCountX / ARDUBOY_PUZZLE_SCREEN_WIDTH)));
			availableSpriteCountY = endCamera.Y - startCamera.Y;
			int lineHeight = Math.Min(height, (DrawSpriteHeight * availableSpriteCountY) + ((int)mPuzzleScreenSeparatorLinePen.Width * (availableSpriteCountY / ARDUBOY_PUZZLE_SCREEN_HEIGHT)));

			// clear the image
			gc.Clear(Color.Black);

			// count the lines
			int horizontalLinesCount = 0;
			int halfPenWidth = (int)(mPuzzleScreenSeparatorLinePen.Width / 2);

			// draw all the sprites
			for (int y = startCamera.Y; y < endCamera.Y; ++y)
			{
				int yPixel = ((y - startCamera.Y) * DrawSpriteHeight) + horizontalLinesCount;
				// draw the puzzle screen lines
				if ((y % ARDUBOY_PUZZLE_SCREEN_HEIGHT) == 0)
				{
					gc.DrawLine(mPuzzleScreenSeparatorLinePen, 0, yPixel + halfPenWidth, lineWidth, yPixel + halfPenWidth);
					horizontalLinesCount += (int)mPuzzleScreenSeparatorLinePen.Width;
					yPixel += (int)mPuzzleScreenSeparatorLinePen.Width;
				}
				int verticalLinesCount = 0;
				for (int x = startCamera.X; x < endCamera.X; ++x)
				{
					int xPixel = ((x - startCamera.X) * DrawSpriteWidth) + verticalLinesCount;
					// draw the puzzle screen lines
					if ((x % ARDUBOY_PUZZLE_SCREEN_WIDTH) == 0)
					{
						// draw the vertical line only one time
						if (y == startCamera.Y)
							gc.DrawLine(mPuzzleScreenSeparatorLinePen, xPixel + halfPenWidth, 0, xPixel + halfPenWidth, lineHeight);
						verticalLinesCount += (int)mPuzzleScreenSeparatorLinePen.Width;
						xPixel += (int)mPuzzleScreenSeparatorLinePen.Width;
					}

					byte spriteId = mLevel[x, y];
					byte previousSpriteId = mLevel[Math.Max(x - 1, 0), y];
					bool isMirror = false;
					if ((spriteId <= (int)WallId.BLOCK_16_8_RIGHT) || (spriteId == (int)WallId.ROCK_CEILING_THIN))
					{
						int randomValue = ((y & 0xFF) << 4) | (x & 0xFF);
						randomValue ^= randomValue << 2;
						randomValue ^= randomValue >> 3;
						isMirror = (randomValue % 2) == 1;
					}
					else if ((spriteId == (int)WallId.STAIR) || (spriteId == (int)WallId.ROCK_LEFT_WALL))
						isMirror = (previousSpriteId == (int)WallId.NOTHING);
					else if ((spriteId == (int)WallId.BIG_STATUE_TOP) || (spriteId == (int)WallId.BIG_STATUE_BOTTOM))
						isMirror = (previousSpriteId == spriteId);

					// draw the sprites
					if (isMirror)
						gc.DrawImage(mWallSpritesMirrored[spriteId], xPixel, yPixel, DrawSpriteWidth, DrawSpriteHeight);
					else
						gc.DrawImage(mWallSprites[spriteId], xPixel, yPixel, DrawSpriteWidth, DrawSpriteHeight);
				}
				// draw the last line if needed
				if (endCamera.X == LEVEL_WIDTH)
				{
					int xPixel = ((endCamera.X - startCamera.X) * DrawSpriteWidth) + verticalLinesCount;
					gc.DrawLine(mPuzzleScreenSeparatorLinePen, xPixel + halfPenWidth, 0, xPixel + halfPenWidth, lineHeight);
				}
			}
			// draw the last line if needed
			if (endCamera.Y == LEVEL_HEIGHT)
			{
				int yPixel = ((endCamera.Y - startCamera.Y) * DrawSpriteHeight) + horizontalLinesCount;
				gc.DrawLine(mPuzzleScreenSeparatorLinePen, 0, yPixel + halfPenWidth, lineWidth, yPixel + halfPenWidth);
			}
		}

		private void DrawItems(Graphics gc, int width, int height, int cameraX, int cameraY, Items.Item selectedItem)
		{
			// after drawing the map, move the compositing mode to sourc over, for the item to be drawn with transparency
			gc.CompositingMode = System.Drawing.Drawing2D.CompositingMode.SourceOver;
			// compute the camera position in game world coordinates
			int cameraXWorld = cameraX * WALL_SPRITE_WIDTH;
			int cameraYWorld = cameraY * WALL_SPRITE_HEIGHT;
			int puzzleLineSeparatorWidth = (int)mPuzzleScreenSeparatorLinePen.Width;
			foreach (List<Items.Item> itemList in mItems.Values)
				foreach (Items.Item item in itemList)
					item.Draw(gc, mPixelSize, DrawPuzzleScreenWidth, DrawPuzzleScreenHeight, puzzleLineSeparatorWidth, cameraXWorld, cameraYWorld, (selectedItem == item));
		}

		private void DrawMainMenu(Graphics gc, int cameraX, int cameraY)
		{
			// after drawing the map, move the compositing mode to sourc over, for the item to be drawn with transparency
			gc.CompositingMode = System.Drawing.Drawing2D.CompositingMode.SourceOver;
			// compute the camera position in game world coordinates
			int cameraXWorld = cameraX * WALL_SPRITE_WIDTH;
			int cameraYWorld = cameraY * WALL_SPRITE_HEIGHT;

			// compute the menuX position
			float playMenuX = (27f - cameraXWorld) * mPixelSize;
			float otherMenuX = (40f - cameraXWorld) * mPixelSize;
			Font mainMenuTextFont = new Font(FontFamily.GenericMonospace, 8f * mPixelSize, FontStyle.Bold);

			// draw the menus
			gc.DrawString("> Play", mainMenuTextFont, mMainMenuTextBrush, playMenuX, (30 - cameraYWorld) * mPixelSize);
			gc.DrawString("Sound On", mainMenuTextFont, mMainMenuTextBrush, otherMenuX, (41 - cameraYWorld) * mPixelSize);
			gc.DrawString("Controls", mainMenuTextFont, mMainMenuTextBrush, otherMenuX, (52 - cameraYWorld) * mPixelSize);
		}

		private void DrawPuzzlePath(Graphics gc, int width, int height, int cameraX, int cameraY)
		{
			// start to use the valid pen, and if the Build of the puzzle throw an exception, change to invalid pen
			Pen drawingPen = mValidPuzzlePathLinePen;

			try
			{
				// rebuild the path
				BuildPuzzlePath();
			}
			catch (Exception)
			{
				drawingPen = mInvalidPuzzlePathLinePen;
			}

			// declare variables to store the pixel coordinates of the previous screen
			int previousXPixel = 0;
			int previousYPixel = 0;

			// iterate on the path to draw all the lines
			for (int i = 0; i < mPuzzlePath.Count; ++i)
			{
				// convert the screen coordinate into pixel coordinate
				int currentXPixel = ((mPuzzlePath[i].screenX + 8 - cameraX) * DrawSpriteWidth);
				int currentYPixel = ((mPuzzlePath[i].screenY + 4 - cameraY) * DrawSpriteHeight);

				// draw the line, except for the first point
				if (i > 0)
					gc.DrawLine(drawingPen, previousXPixel, previousYPixel, currentXPixel, currentYPixel);

				// store the current screen coordinate in the previous variables
				previousXPixel = currentXPixel;
				previousYPixel = currentYPixel;
			}
		}
		#endregion
	}
}
