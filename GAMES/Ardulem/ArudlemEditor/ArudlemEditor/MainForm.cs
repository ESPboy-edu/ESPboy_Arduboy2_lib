using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using ArudlemEditor.UndoRedo;

namespace ArudlemEditor
{
    public partial class MainForm : Form
    {
        #region parameters
        // number of sprites in the sprite map
        private const int MAP_SPRITE_WIDTH = 8;
        private const int MAP_SPRITE_HEIGHT = 4;
        // the zoom that should be applied on the original images used for the arduboy
        private const int IMAGE_ZOOM_SCALE = 16;
		// current size of the memory pool
		private const int MEMORY_ALLOCATED_IN_ARDUBOY = 328;
        #endregion

        #region variables
        private Bitmap m_MapSpriteImage = null;
		private Bitmap m_MapSpriteImageMirrored = null;
		private Bitmap m_StartImage = null;
		private Bitmap m_HomeImage = null;
		private Pen m_MapSpriteLinesPen = new Pen(Color.Yellow, 4f);
        private Pen m_MapSpriteSelectionPen = new Pen(Color.Red, 4f);
        private Pen m_LevelLinesPen = new Pen(Color.Yellow, 1f);

        private int m_CurrentSpriteIndex = 0;

        private Level m_CurrentLevel = new Level();
		private LevelData m_CurrentLevelData = new LevelData();
		private ActionManager m_ActionManager = new ActionManager();
        #endregion

        #region init
        public MainForm()
        {
            InitializeComponent();
            LoadMapSpriteImage();
            DrawLevel();
        }

        private void LoadMapSpriteImage()
        {
            // get the original image
            string mapSpritePath = Application.StartupPath + @"/MapSprite.png";
            Bitmap originalMapSpriteImage = new Bitmap(mapSpritePath);
			m_MapSpriteImage = new Bitmap(originalMapSpriteImage.Size.Width * IMAGE_ZOOM_SCALE, originalMapSpriteImage.Size.Height * IMAGE_ZOOM_SCALE);
			Graphics gc = Graphics.FromImage(m_MapSpriteImage);
			SetGCInPixelMode(ref gc);
			gc.DrawImage(originalMapSpriteImage,	// source image
				new Rectangle(0, 0, m_MapSpriteImage.Width, m_MapSpriteImage.Height),  // destination rectangle 
				new Rectangle(0, 0, originalMapSpriteImage.Width, originalMapSpriteImage.Height),	// source rectangle
				GraphicsUnit.Pixel);
			// create the mirrored image from the loaded one
			m_MapSpriteImageMirrored = new Bitmap(m_MapSpriteImage);
			CreateTaintedAndMirroredImage(m_MapSpriteImage, Rectangle.Empty, ref m_MapSpriteImageMirrored, true, 0.5f, 0.7f, 1f);
            // and init the image
            DrawMapSprite();

			// get the START
			string startSpritePath = Application.StartupPath + @"/Start.png";
			Bitmap startSpriteImage = new Bitmap(startSpritePath);
			//draw only the last frame of the start and mirror it
			Bitmap lastFrameImage = new Bitmap(8,8);
			CreateTaintedAndMirroredImage(startSpriteImage, new Rectangle(40,0,8,8), ref lastFrameImage, false, 1f, 0.7f, 0.5f);
			Bitmap lastFrameMirroredImage = new Bitmap(8, 8);
			CreateTaintedAndMirroredImage(startSpriteImage, new Rectangle(40,0,8,8), ref lastFrameMirroredImage, true, 1f, 0.7f, 0.5f);
			// draw the two part in the final image
			m_StartImage = new Bitmap(16, 8);
			gc = Graphics.FromImage(m_StartImage);
			SetGCInPixelMode(ref gc);
			gc.DrawImage(lastFrameImage,	// source image
				new Rectangle(0, 0, 8, 8),  // destination rectangle 
				new Rectangle(0, 0, 8, 8),	// source rectangle
				GraphicsUnit.Pixel);
			gc.DrawImage(lastFrameMirroredImage,// source image
				new Rectangle(8, 0, 8, 8),		// destination rectangle 
				new Rectangle(0, 0, 8, 8),		// source rectangle
				GraphicsUnit.Pixel);
			m_StartImage.MakeTransparent(Color.Black);

			// get the HOME
			string homeSpritePath = Application.StartupPath + @"/Home.png";
			Bitmap homeSpriteImage = new Bitmap(homeSpritePath);
			// get the bottom and top parts
			Bitmap homeBottomImage = new Bitmap(15, 8);
			CreateTaintedAndMirroredImage(homeSpriteImage, new Rectangle(0, 0, 15, 8), ref homeBottomImage, false, 1f, 0.7f, 0.5f);
			Bitmap homeTopImage = new Bitmap(5, 4);
			CreateTaintedAndMirroredImage(homeSpriteImage, new Rectangle(16, 0, 5, 4), ref homeTopImage, false, 1f, 0.7f, 0.5f);
			// draw the two parts of home
			m_HomeImage = new Bitmap(15, 12);
			gc = Graphics.FromImage(m_HomeImage);
			SetGCInPixelMode(ref gc);
			gc.Clear(Color.Black);
			gc.DrawImage(homeBottomImage,	// source image
				new Rectangle(0, 4, 15, 8), // destination rectangle 
				new Rectangle(0, 0, 15, 8),	// source rectangle
				GraphicsUnit.Pixel);
			gc.DrawImage(homeTopImage,		// source image
				new Rectangle(5, 0, 5, 4),  // destination rectangle 
				new Rectangle(0, 0, 5, 4),	// source rectangle
				GraphicsUnit.Pixel);
			m_HomeImage.MakeTransparent(Color.Black);
		}

		private void CreateTaintedAndMirroredImage(Bitmap sourceImage, Rectangle sourceRectangle, ref Bitmap resultImage, bool shouldMirror, float red, float green, float blue)
		{
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

			// check if the source rectangle is empty, use the full source image
			if (sourceRectangle.IsEmpty)
				sourceRectangle = new Rectangle(0, 0, sourceImage.Width, sourceImage.Height);

			// paint the image in the mirrored one with the image attribute
			Graphics gc = Graphics.FromImage(resultImage);
			SetGCInPixelMode(ref gc);
			gc.DrawImage(sourceImage,		// source image
				new Rectangle(0, 0, resultImage.Width, resultImage.Height),  // destination rectangle 
				sourceRectangle.X,
				sourceRectangle.Y,			// upper-left corner of source rectangle 
				sourceRectangle.Width,		// width of source rectangle
				sourceRectangle.Height,		// height of source rectangle
				GraphicsUnit.Pixel,
				imageAttributes);

			// and mirror it if needed
			if (shouldMirror)
				resultImage.RotateFlip(RotateFlipType.RotateNoneFlipX);
		}

		private void SetGCInPixelMode(ref Graphics gc)
		{
			gc.CompositingMode = System.Drawing.Drawing2D.CompositingMode.SourceCopy;
			gc.CompositingQuality = System.Drawing.Drawing2D.CompositingQuality.Default;
			gc.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
			gc.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.None;
			gc.PixelOffsetMode = System.Drawing.Drawing2D.PixelOffsetMode.Half;
		}
        #endregion

        #region drawing
        private SizeF GetMapSpriteCellSize(SizeF size)
        {
            return new SizeF(size.Width / MAP_SPRITE_WIDTH, size.Height / MAP_SPRITE_HEIGHT);
        }

        private SizeF GetLevelCellSize(SizeF size)
        {
            return new SizeF(size.Width / Level.LEVEL_WIDTH, size.Height / Level.LEVEL_HEIGHT);
        }

        private void DrawMapSpriteLines(Graphics gc, Size imageSise, int horizontalCellCount, int verticalCellCount, Pen pen)
        {
            float cellWidth = imageSise.Width / horizontalCellCount;
            float cellHeight = imageSise.Height / verticalCellCount;

            // get the x and y of the picture box
            float x = 0;
            float y = 0;

            // draw the vertical lines
            for (int i = 1; i < horizontalCellCount; ++i)
            {
                x = i * cellWidth;
                gc.DrawLine(pen, x, y, x, y + imageSise.Height);
            }

            // Draw the horizontal lines
            x = 0;
            for (int i = 1; i < verticalCellCount; ++i)
            {
                y = i * cellHeight;
                gc.DrawLine(pen, x, y, x + imageSise.Width, y);
            }
        }

        private void DrawMapSprite()
        {
            // resize the image to draw the lines
            this.MapSpritePictureBox.Image = new Bitmap(m_MapSpriteImage);
            
            // get the gc of the image
            Graphics gc = Graphics.FromImage(this.MapSpritePictureBox.Image);
			SetGCInPixelMode(ref gc);

            // draw the lines
            DrawMapSpriteLines(gc, this.MapSpritePictureBox.Image.Size, MAP_SPRITE_WIDTH, MAP_SPRITE_HEIGHT, m_MapSpriteLinesPen);

            // and draw the the selected sprite
            SizeF cellSize = GetMapSpriteCellSize(this.MapSpritePictureBox.Image.Size);
            gc.DrawRectangle(m_MapSpriteSelectionPen, (m_CurrentSpriteIndex / MAP_SPRITE_HEIGHT) * cellSize.Width, (m_CurrentSpriteIndex % MAP_SPRITE_HEIGHT) * cellSize.Height, cellSize.Width, cellSize.Height);
        }

        private void DrawLevel()
        {
            this.LevelPictureBox.Image = new Bitmap(Level.LEVEL_WIDTH * IMAGE_ZOOM_SCALE, Level.LEVEL_HEIGHT * IMAGE_ZOOM_SCALE);

            // get the gc of the image
            Graphics gc = Graphics.FromImage(this.LevelPictureBox.Image);
			SetGCInPixelMode(ref gc);

            // get the sprite cell size and level cell size
            SizeF spriteCellSize = GetMapSpriteCellSize(m_MapSpriteImage.Size);
            SizeF levelCellSize = GetLevelCellSize(this.LevelPictureBox.Image.Size);

            // iterate through the level data to paint the correct sprite
            for (int i = 0; i < Level.LEVEL_WIDTH; ++i)
                for (int j = 0; j < Level.LEVEL_HEIGHT; ++j)
                {
                    Rectangle levelRectangle = new Rectangle((int)(i * levelCellSize.Width), (int)(j * levelCellSize.Height), (int)levelCellSize.Width, (int)levelCellSize.Height);
                    if (m_CurrentLevel.IsSpriteSet(i, j))
                    {
                        int spriteIndex = m_CurrentLevel.GetSprite(i, j);
						if (m_CurrentLevel.IsSpriteMirrored(i, j))
						{
							Rectangle spriteRectangle = new Rectangle((int)((MAP_SPRITE_WIDTH-1 - (spriteIndex / MAP_SPRITE_HEIGHT)) * spriteCellSize.Width), (int)((spriteIndex % MAP_SPRITE_HEIGHT) * spriteCellSize.Height), (int)spriteCellSize.Width, (int)spriteCellSize.Height);
							gc.DrawImage(m_MapSpriteImageMirrored, levelRectangle, spriteRectangle, GraphicsUnit.Pixel);
						}
						else
						{
							Rectangle spriteRectangle = new Rectangle((int)((spriteIndex / MAP_SPRITE_HEIGHT) * spriteCellSize.Width), (int)((spriteIndex % MAP_SPRITE_HEIGHT) * spriteCellSize.Height), (int)spriteCellSize.Width, (int)spriteCellSize.Height);
							gc.DrawImage(m_MapSpriteImage, levelRectangle, spriteRectangle, GraphicsUnit.Pixel);
						}
                    }
                    else
                    {
                        gc.FillRectangle(Brushes.Black, levelRectangle);
                    }
                }

			// draw the start and home
			if (this.drawStartAndHomeToolStripMenuItem.Checked)
			{
				gc.CompositingMode = System.Drawing.Drawing2D.CompositingMode.SourceOver;
				DrawStartAndHome(gc, m_CurrentLevelData.StartX, m_CurrentLevelData.StartY, this.m_StartImage);
				DrawStartAndHome(gc, m_CurrentLevelData.HomeX-7, m_CurrentLevelData.HomeY-11, this.m_HomeImage);
			}

            // finally draw the grid on top of it
            if (this.drawLevelGridToolStripMenuItem.Checked)
                DrawMapSpriteLines(gc, this.LevelPictureBox.Image.Size, Level.LEVEL_WIDTH, Level.LEVEL_HEIGHT, m_LevelLinesPen);
        }

		private void DrawStartAndHome(Graphics gc, int x, int y, Bitmap startOrHomeImage)
		{
			SizeF scale = new SizeF((float)this.LevelPictureBox.Image.Width / 256f, (float)this.LevelPictureBox.Image.Height / 64f);
			Rectangle sourceRectangle = new Rectangle(0, 0, startOrHomeImage.Width, startOrHomeImage.Height);
			Rectangle destRectangle = new Rectangle((int)(x * scale.Width), (int)(y * scale.Height), (int)(startOrHomeImage.Width * scale.Width), (int)(startOrHomeImage.Height * scale.Height));
			gc.DrawImage(startOrHomeImage, destRectangle, sourceRectangle, GraphicsUnit.Pixel);
		}
        #endregion

		#region memory estimation
		private int GetMemoryUsageEstimation()
		{
			// a lem class take 3 chars in Arduboy (this is a exact value)
			int spawnLemSize = m_CurrentLevelData.SpawnLemCount * 3;

			// A exploders make a hole of 11 pixel wide on 9 pixels heigh,
			// which makes between 16 to 22 modif depending on the explosion Y
			// However, since the lem is rarely totally enclosed by pixel when exploding
			// we can estimate that half the 20 modfifs will be necessary
			int bomberModifSize = m_CurrentLevelData.Bomber * (20 / 2);

			// Stair makers, build 10 steps of 2 pixels (including those under their feet at the beggining),
			// so if they build in straight line (their direction is not reversed by a wall or a blocker)
			// they will use 20 modifs. However, the two modifs under the feet at start, or when prolonging
			// a stair are free, so lets count only 18.
			int stairModifSize = m_CurrentLevelData.Stairer * 18;

			// Vertical diggers are the cheapest ones. They dig a hole of 5 contiguous columns.
			// Each column is 8 pixels high and the maxium they can dig is the total screen
			// which only contains 8 columns. So at the maximum, they use 5*8 = 40 modifs.
			// but since they don't often dig the whole, screen, we can estimate that
			// they dig one third of the screen before falling
			int vertDiggerModifSize = m_CurrentLevelData.VertDigger * (40 / 3);

			// diagonal digger are a bit more expensive that vertical but less than horizontal 
			// (as we could expect). They are also limited by the height of the screen, but not
			// limited horizontally. Diag Digger dig a tunnel of 6 pixel high, so it's between 
			// 1 or 2 modif for each pixel of the tunnel. Diag digger advance 2 pixel for each pixel
			// going down. We can estimate their tunnel length around 20 pixels before they fall.
			// for which you should take 50% more for when one x needs 2 modifs
			int diagDiggerModifSize = m_CurrentLevelData.DiagDigger * (int)(20 * 1.5f);

			// horizontal digger are the most expensive one. They take 1 or 2 modif per x
			// of their tunnel, depending on the y of the tunnel.
			// They can dig the whole level (256 pixels), for maybe 512 modif!!!
			// it's more than the memory pool allocated in the game.
			// let's estimate their tunnel lenght like the diag digger
			int horizDiggerModifSize = m_CurrentLevelData.HorizDigger * (int)(20 * 1.5f);

			// return the sum
			return spawnLemSize + bomberModifSize + stairModifSize + vertDiggerModifSize + diagDiggerModifSize + horizDiggerModifSize;
		}

		private void UpdateStatusBar()
		{
			int memoryUsage = GetMemoryUsageEstimation();
			this.StatusBarText.Text = "Memory Usage Estimation: " + memoryUsage.ToString() + " bytes (max " + MEMORY_ALLOCATED_IN_ARDUBOY + " bytes)";

			// change the color of the background of the status depending on the size
			if (memoryUsage > MEMORY_ALLOCATED_IN_ARDUBOY)
				this.StatusBar.BackColor = Color.LightSalmon;
			else
				this.StatusBar.BackColor = Color.LightGreen;
		}
		#endregion

		#region form events from menus
		#region File menu
		private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // instantiate an empty new level
            m_CurrentLevel = new Level();
			m_CurrentLevelData = new LevelData();
			SetUIValueFromLevelDataValues();
            DrawLevel();
        }

		private void SetUIValueFromLevelDataValues()
		{
			// set the various data parsed
			this.StartXNumeric.Value = m_CurrentLevelData.StartX;
			this.StartYNumeric.Value = m_CurrentLevelData.StartY;
			this.HomeXNumeric.Value = m_CurrentLevelData.HomeX;
			this.HomeYNumeric.Value = m_CurrentLevelData.HomeY;
			this.TimeMinNumeric.Value = m_CurrentLevelData.TimeMin;
			this.TimeSecNumeric.Value = m_CurrentLevelData.TimeSec;
			this.SpawnLemCountNumeric.Value = m_CurrentLevelData.SpawnLemCount;
			this.RequiredLemCountNumeric.Value = m_CurrentLevelData.RequiredLemCount;
			this.MinDropSpeedNumeric.Value = m_CurrentLevelData.MinDropSpeed;
			this.WalkNumeric.Value = m_CurrentLevelData.Walker;
			this.BlockNumeric.Value = m_CurrentLevelData.Blocker;
			this.BombNumeric.Value = m_CurrentLevelData.Bomber;
			this.DigDiagNumeric.Value = m_CurrentLevelData.DiagDigger;
			this.DigHorizNumeric.Value = m_CurrentLevelData.HorizDigger;
			this.DigVertNumeric.Value = m_CurrentLevelData.VertDigger;
			this.StairNumeric.Value = m_CurrentLevelData.Stairer;
			this.ClimbNumeric.Value = m_CurrentLevelData.Climber;
			this.ParachuteNumeric.Value = m_CurrentLevelData.m_Parachuter;
			// also set the map names from the level data
			this.LocaMapNameTextBox.Text = m_CurrentLevelData.m_LocaMapName;
			this.MapIdsTextBox.Text = m_CurrentLevelData.m_MapIdsName;
		}

        private void loadLevelFromClipboardToolStripMenuItem_Click(object sender, EventArgs e)
        {
			// try to load the level first
            bool isOk = m_CurrentLevel.LoadFromClipboard();
			if (isOk)
			{
				// redraw the level
				DrawLevel();

				// set the values in the combo boxes
				this.LocaMapNameTextBox.Text = m_CurrentLevel.m_LocaMapName;
				this.MapIdsTextBox.Text = m_CurrentLevel.m_MapIdsName;
			}
			else
			{
				// now try to load the level data
				isOk = m_CurrentLevelData.LoadFromClipboard();
				if (isOk)
				{
					// redraw the level (because start and home position may have change)
					DrawLevel();
					UpdateStatusBar();
					SetUIValueFromLevelDataValues();
				}
				else
				{
					MessageBox.Show("Error while parsing the clipboard, please try again to copy.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
					return;
				}
			}            
        }

		private void SetExportConfigToLevel()
		{
			// set the export config before calling the save
			m_CurrentLevel.m_ShouldTrimLevelLeftAtExport = this.TrimLevelLeftCheckBox.Checked;
			m_CurrentLevel.m_ShouldTrimLevelRightAtExport = this.TrimLevelRightCheckBox.Checked;
			m_CurrentLevel.m_ExportWithWindowsEOL = this.ExportWithWinEOLCheckBox.Checked;
			m_CurrentLevelData.m_ExportWithWindowsEOL = this.ExportWithWinEOLCheckBox.Checked;
		}

        private void saveLevelToClipboardToolStripMenuItem_Click(object sender, EventArgs e)
        {
			SetExportConfigToLevel();
			// set the values from the combo boxes to the current level before calling the save
			m_CurrentLevel.m_LocaMapName = this.LocaMapNameTextBox.Text;
			m_CurrentLevel.m_MapIdsName = this.MapIdsTextBox.Text;
			// and copy the current level to clipboard
            m_CurrentLevel.SaveToClipboard();
        }

		private void saveLevelDataToClipboardToolStripMenuItem_Click(object sender, EventArgs e)
		{
			// set the export config
			SetExportConfigToLevel();

			// set the values from the combo boxes to the current level before calling the save
			m_CurrentLevelData.StartX = (int)this.StartXNumeric.Value;
			m_CurrentLevelData.StartY = (int)this.StartYNumeric.Value;
			m_CurrentLevelData.HomeX = (int)this.HomeXNumeric.Value;
			m_CurrentLevelData.HomeY = (int)this.HomeYNumeric.Value;
			m_CurrentLevelData.TimeMin = (int)this.TimeMinNumeric.Value;
			m_CurrentLevelData.TimeSec = (int)this.TimeSecNumeric.Value;
			m_CurrentLevelData.SpawnLemCount = (int)this.SpawnLemCountNumeric.Value;
			m_CurrentLevelData.RequiredLemCount = (int)this.RequiredLemCountNumeric.Value;
			m_CurrentLevelData.MinDropSpeed = (int)this.MinDropSpeedNumeric.Value;
			m_CurrentLevelData.Walker = (int)this.WalkNumeric.Value;
			m_CurrentLevelData.Blocker = (int)this.BlockNumeric.Value;
			m_CurrentLevelData.Bomber = (int)this.BombNumeric.Value;
			m_CurrentLevelData.DiagDigger = (int)this.DigDiagNumeric.Value;
			m_CurrentLevelData.HorizDigger = (int)this.DigHorizNumeric.Value;
			m_CurrentLevelData.VertDigger = (int)this.DigVertNumeric.Value;
			m_CurrentLevelData.Stairer = (int)this.StairNumeric.Value;
			m_CurrentLevelData.Climber = (int)this.ClimbNumeric.Value;
			m_CurrentLevelData.m_Parachuter = (int)this.ParachuteNumeric.Value;
			m_CurrentLevelData.m_LocaMapName = this.LocaMapNameTextBox.Text;
			m_CurrentLevelData.m_MapIdsName = this.MapIdsTextBox.Text;

			// check if the time is null
			if ((m_CurrentLevelData.TimeMin == 0) && (m_CurrentLevelData.TimeSec == 0))
			{
				DialogResult result = MessageBox.Show("The time is null, then the level will exit immediately. Do you want to continue anyway?",
					"Export Warning", MessageBoxButtons.YesNo, MessageBoxIcon.Warning, MessageBoxDefaultButton.Button2);
				if (result == System.Windows.Forms.DialogResult.No)
					return;
			}

			// check the memory usage
			int memUsageEstimation = GetMemoryUsageEstimation();
			if (memUsageEstimation > MEMORY_ALLOCATED_IN_ARDUBOY)
			{
				DialogResult result = MessageBox.Show("You have used a lot of Lems to spawn with a lot of lem modifiers, the Arduboy may not have enough memory to store all the modification.\n" +
					"The memory usage estimation is " + memUsageEstimation.ToString() + " bytes. Try to reduce the Spawn lems (bigger impact), Diggers, Stair makers or exploder. Do you want to continue anyway?",
					"Export Warning", MessageBoxButtons.YesNo, MessageBoxIcon.Warning, MessageBoxDefaultButton.Button2);
				if (result == System.Windows.Forms.DialogResult.No)
					return;
			}


			// and copy the current level to clipboard
			m_CurrentLevelData.SaveToClipboard(m_CurrentLevel.GetMinX());
		}

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }
		#endregion

		#region Edit menu
		private void undoToolStripMenuItem_Click(object sender, EventArgs e)
		{
			m_ActionManager.Undo();
			DrawLevel();
		}

		private void redoToolStripMenuItem_Click(object sender, EventArgs e)
		{
			m_ActionManager.Redo();
			DrawLevel();
		}

		private void shiftLevelToRightToolStripMenuItem_Click(object sender, EventArgs e)
		{
			m_ActionManager.Do(new ShiftHorizontally(m_CurrentLevel, 1));
			DrawLevel();
		}

		private void shiftLevelToLeftToolStripMenuItem_Click(object sender, EventArgs e)
		{
			m_ActionManager.Do(new ShiftHorizontally(m_CurrentLevel, -1));
			DrawLevel();
		}

		private void shiftLevelUpToolStripMenuItem_Click(object sender, EventArgs e)
		{
			m_ActionManager.Do(new ShiftVertically(m_CurrentLevel, -1));
			DrawLevel();
		}

		private void shiftLevelDownToolStripMenuItem_Click(object sender, EventArgs e)
		{
			m_ActionManager.Do(new ShiftVertically(m_CurrentLevel, 1));
			DrawLevel();
		}
		#endregion

		#region View menu
		private void drawLevelGridToolStripMenuItem_Click(object sender, EventArgs e)
		{
			DrawLevel();
		}

		private void drawStartAndHomeToolStripMenuItem_Click(object sender, EventArgs e)
		{
			DrawLevel();
		}
		#endregion
		#endregion

		#region click events
		private void MapSpritePictureBox_MouseClick(object sender, MouseEventArgs e)
        {
            SizeF cellSize = GetMapSpriteCellSize(this.MapSpritePictureBox.Size);

            // update the selected sprite index
            m_CurrentSpriteIndex = (int)(e.Y / cellSize.Height) + ((int)(e.X / cellSize.Width) * MAP_SPRITE_HEIGHT);

            // redraw the map sprite
            DrawMapSprite();
        }

        private void LevelPictureBox_MouseClick(object sender, MouseEventArgs e)
        {
            SizeF cellSize = GetLevelCellSize(this.LevelPictureBox.Size);

            // compute the coordinates
            int x = (int)(e.X / cellSize.Width);
            int y = (int)(e.Y / cellSize.Height);

            // set the current index in the correct column
			if (e.Button == System.Windows.Forms.MouseButtons.Left)
				m_ActionManager.Do(new SetCell(m_CurrentLevel, x, y, m_CurrentSpriteIndex));
			else if (e.Button == System.Windows.Forms.MouseButtons.Middle)
				m_ActionManager.Do(new ClearCell(m_CurrentLevel, x, y));
			else if (e.Button == System.Windows.Forms.MouseButtons.Right)
				m_ActionManager.Do(new MirrorCell(m_CurrentLevel, x, y));

            // redraw the map sprite
            DrawLevel();
        }
        #endregion

        #region event for parameter changes
        private void TrimLevelCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            m_CurrentLevel.m_ShouldTrimLevelLeftAtExport = this.TrimLevelLeftCheckBox.Checked;
        }

		private void TrimLevelRightCheckBox_CheckedChanged(object sender, EventArgs e)
		{
			m_CurrentLevel.m_ShouldTrimLevelRightAtExport = this.TrimLevelRightCheckBox.Checked;
		}

		private void ExportWithWinEOLCheckBox_CheckedChanged(object sender, EventArgs e)
		{
			// set both for Level and LevelData as they both use it
			m_CurrentLevel.m_ExportWithWindowsEOL = this.ExportWithWinEOLCheckBox.Checked;
			m_CurrentLevelData.m_ExportWithWindowsEOL = this.ExportWithWinEOLCheckBox.Checked;
		}

		private void TimeSecNumeric_ValueChanged(object sender, EventArgs e)
		{
			// only accecpt 10th of seconds
			int modulo = ((int)this.TimeSecNumeric.Value % 10);
			if (modulo != 0)
			{
				int tenth = (int)TimeSecNumeric.Value / 10;
				if (modulo < 5)
					this.TimeSecNumeric.Value = tenth * 10;
				else
					this.TimeSecNumeric.Value = (tenth + 1) * 10;
			}

			if (this.TimeSecNumeric.Value == 60)
			{
				// reset the seconds and add one minute if possible
				this.TimeSecNumeric.Value = 0;
				if (this.TimeMinNumeric.Value < this.TimeMinNumeric.Maximum)
					this.TimeMinNumeric.Value = (int)this.TimeMinNumeric.Value + 1;
			}
		}

		private void StartXNumeric_ValueChanged(object sender, EventArgs e)
		{
			m_CurrentLevelData.StartX = (int)this.StartXNumeric.Value;
			DrawLevel();
		}

		private void StartYNumeric_ValueChanged(object sender, EventArgs e)
		{
			m_CurrentLevelData.StartY = (int)this.StartYNumeric.Value;
			DrawLevel();
		}

		private void HomeXNumeric_ValueChanged(object sender, EventArgs e)
		{
			m_CurrentLevelData.HomeX = (int)this.HomeXNumeric.Value;
			DrawLevel();
		}

		private void HomeYNumeric_ValueChanged(object sender, EventArgs e)
		{
			m_CurrentLevelData.HomeY = (int)this.HomeYNumeric.Value;
			DrawLevel();
		}

		private void SpawnLemCountNumeric_ValueChanged(object sender, EventArgs e)
		{
			m_CurrentLevelData.SpawnLemCount = (int)this.SpawnLemCountNumeric.Value;
			UpdateStatusBar();
		}

		private void BombNumeric_ValueChanged(object sender, EventArgs e)
		{
			m_CurrentLevelData.Bomber = (int)this.BombNumeric.Value;
			UpdateStatusBar();
		}

		private void DigDiagNumeric_ValueChanged(object sender, EventArgs e)
		{
			m_CurrentLevelData.DiagDigger = (int)this.DigDiagNumeric.Value;
			UpdateStatusBar();
		}

		private void DigHorizNumeric_ValueChanged(object sender, EventArgs e)
		{
			m_CurrentLevelData.HorizDigger = (int)this.DigHorizNumeric.Value;
			UpdateStatusBar();
		}

		private void DigVertNumeric_ValueChanged(object sender, EventArgs e)
		{
			m_CurrentLevelData.VertDigger = (int)this.DigVertNumeric.Value;
			UpdateStatusBar();
		}

		private void StairNumeric_ValueChanged(object sender, EventArgs e)
		{
			m_CurrentLevelData.Stairer = (int)this.StairNumeric.Value;
			UpdateStatusBar();
		}
		#endregion
	}
}
