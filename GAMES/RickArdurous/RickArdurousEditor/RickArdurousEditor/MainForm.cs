using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

namespace RickArdurousEditor
{
	public partial class MainForm : Form
	{
		private enum EditAction
		{
			MOVE_ITEM,
			ADJUST_SENSOR,
		}

		private static MainForm mInstance = null;

		DocumentationForm mDocumentationForm = null;

		private Map mMap = new Map();

		// the instance of the action manager (for undo/redo)
		private Action.ActionManager mActionManager = new Action.ActionManager();

		// current selection
		private byte mCurrentSelectedSpriteId = 0;
		private Items.Item mCurrentSelectedItem = null;
		private Point mCurrentSelectedItemGrabDelta;

		// the coordinates of the current part of the level which is visible
		private Point mMapCamera;
		private Point mLastMouseDownMapCamera;

		// variable for mouse events
		private EditAction mCurrentEditAction = EditAction.MOVE_ITEM;
		private Point mLastMouseDownPosition;
		private Point mLastMouseMovePosition;
		private Point mOriginalItemPositionForItemMove;
		private int mOriginalSensorDistanceForAdjustDistance;

		private Pen mSelectedSpritePen = new Pen(Color.Yellow, 3);

		#region get/set
		public static MainForm Instance
		{
			get { return mInstance; }
		}

		public Items.Item CurrentSelectedItem
		{
			get { return mCurrentSelectedItem; }
			set
			{
				mCurrentSelectedItem = value;
				RedrawLevel();
			}
		}

		#endregion

		#region init
		public MainForm()
		{
			// set the singleton
			mInstance = this;
			// default UI init
			InitializeComponent();
			// init the sprite tool box
			RedrawWallSpriteToolbox(0, 0);
			RedrawItemsSpriteToolbox(-1, -1);
			// load the default map file
			LoadMap(PathProvider.GetGamePath() + Properties.Settings.Default.DefaultMapFileName);
		}

		private void RedrawWallSpriteToolbox(int selectedSpriteX, int selectedSpriteY)
		{
			Bitmap originalImage = ImageProvider.GetWallSpriteImage();
			Bitmap spritesImage = new Bitmap(PictureBoxWallSprites.Width, PictureBoxWallSprites.Height);
			Graphics gc = Graphics.FromImage(spritesImage);
			gc.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
			gc.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.None;
			gc.PixelOffsetMode = System.Drawing.Drawing2D.PixelOffsetMode.Half;
			gc.DrawImage(originalImage, 0, 0, spritesImage.Width, spritesImage.Height);

			// create a pen for drawing lines to split the sprites
			Pen linePen = new Pen(Color.Blue, 2);
			int width = (spritesImage.Width / 2) - 1;
			float height = (float)spritesImage.Height / 8f;
			gc.DrawLine(linePen, new Point(width, 0), new Point(width, spritesImage.Height));
			for (int i = 0; i < 8; ++i)
			{
				int lineHeight = (int)(height * i);
				gc.DrawLine(linePen, new Point(0, lineHeight), new Point(spritesImage.Width, lineHeight));
			}

			// draw the selected sprite
			if (selectedSpriteX >= 0)
				gc.DrawRectangle(mSelectedSpritePen, selectedSpriteX * width, selectedSpriteY * height, width, height);

			// set the strech image in the picture box
			PictureBoxWallSprites.Image = spritesImage;
		}

		private void RedrawItemsSpriteToolbox(int selectedItemX, int selectedItemY)
		{
			Bitmap originalImage = ImageProvider.GetItemsSpriteImage();
			Bitmap spritesImage = new Bitmap(PictureBoxItems.Width, PictureBoxItems.Height);
			Graphics gc = Graphics.FromImage(spritesImage);
			gc.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
			gc.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.None;
			gc.PixelOffsetMode = System.Drawing.Drawing2D.PixelOffsetMode.Half;
			gc.DrawImage(originalImage, 0, 0, spritesImage.Width, spritesImage.Height);

			// create a pen for drawing lines to split the sprites
			Pen linePen = new Pen(Color.Blue, 2);
			int width = (spritesImage.Width / 2) - 1;
			float height = (float)spritesImage.Height / 8f;
			gc.DrawLine(linePen, new Point(width, 0), new Point(width, spritesImage.Height));
			for (int i = 0; i < 8; ++i)
			{
				int lineHeight = (int)(height * i);
				gc.DrawLine(linePen, new Point(0, lineHeight), new Point(spritesImage.Width, lineHeight));
			}

			// draw the selected sprite
			if (selectedItemX >= 0)
				gc.DrawRectangle(mSelectedSpritePen, selectedItemX * width, selectedItemY * height, width, height);

			// set the strech image in the picture box
			PictureBoxItems.Image = spritesImage;
		}

		public void RedrawLevel()
		{
			// create image
			if (PictureBoxLevel.Image == null)
				PictureBoxLevel.Image = new Bitmap(PictureBoxLevel.Width, PictureBoxLevel.Height);
			Graphics gc = Graphics.FromImage(PictureBoxLevel.Image);
			// ask the map to redraw it
			mMap.Redraw(gc, PictureBoxLevel.Image.Width, PictureBoxLevel.Image.Height, mMapCamera.X, mMapCamera.Y, mCurrentSelectedItem);
			// and ask to refraw
			PictureBoxLevel.Refresh();
		}

		private void PanLevelCamera(Point newMousePosition)
		{
			Point mouseMove = new Point(mLastMouseDownPosition.X - newMousePosition.X, mLastMouseDownPosition.Y - newMousePosition.Y);
			mMapCamera.X = mLastMouseDownMapCamera.X + (mouseMove.X / mMap.DrawSpriteWidth);
			mMapCamera.Y = mLastMouseDownMapCamera.Y + (mouseMove.Y / mMap.DrawSpriteHeight);
			// clamp the coordinate inside the level
			mMap.ClampCoordinatesInsideLevel(ref mMapCamera);

			// redraw the level
			RedrawLevel();
		}
		#endregion

		#region menu event
		#region File Menu

		private void UpdateTitleBar()
		{
			// get the name of the open map file
			string mapName = string.Empty;
			if (mMap.FileName != string.Empty)
				mapName = " - " + Path.GetFileName(mMap.FileName);
			// change the title of the editor
			this.Text = AboutBox.AssemblyTitle + mapName;
		}

		private void LoadMap(string mapFileName)
		{
			mMap.Load(mapFileName);
			RedrawLevel();
			UpdateTitleBar();
		}

		private void SaveMap(string mapFileName)
		{
			mMap.Save(mapFileName, PathProvider.GetConstVariableFileName(mapFileName));
			UpdateTitleBar();
		}

		private void newToolStripMenuItem_Click(object sender, EventArgs e)
		{
			mMap.Reset();
			RedrawLevel();
			UpdateTitleBar();
		}

		private void openToolStripMenuItem_Click(object sender, EventArgs e)
		{
			// set the directory and path for the open file dialog
			openMapFileDialog.InitialDirectory = PathProvider.GetGamePath();
			if (Path.IsPathRooted(openMapFileDialog.FileName))
				openMapFileDialog.FileName = Path.GetFileName(openMapFileDialog.FileName);
			// show the dialog
			DialogResult result = openMapFileDialog.ShowDialog();
			if (result == DialogResult.OK)
				LoadMap(openMapFileDialog.FileName);
		}

		private void saveToolStripMenuItem_Click(object sender, EventArgs e)
		{
			if (mMap.FileName == string.Empty)
				saveAsToolStripMenuItem_Click(sender, e);
			else
				SaveMap(mMap.FileName);
		}

		private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
		{
			// set the directory and path for the open file dialog
			saveMapFileDialog.InitialDirectory = PathProvider.GetGamePath();
			if (Path.IsPathRooted(saveMapFileDialog.FileName))
				saveMapFileDialog.FileName = Path.GetFileName(saveMapFileDialog.FileName);

			// show the dialog
			DialogResult result = saveMapFileDialog.ShowDialog();
			if (result == DialogResult.OK)
				SaveMap(saveMapFileDialog.FileName);
		}

		private void preferencesToolStripMenuItem_Click(object sender, EventArgs e)
		{
			SettingsForm settingsForm = new SettingsForm();
			settingsForm.ShowDialog();
		}

		private void exitToolStripMenuItem_Click(object sender, EventArgs e)
		{
			this.Close();
		}
		#endregion

		#region Edit menu
		private void undoToolStripMenuItem_Click(object sender, EventArgs e)
		{
			mActionManager.Undo();
		}

		private void redoToolStripMenuItem_Click(object sender, EventArgs e)
		{
			mActionManager.Redo();
		}

		private void editToolStripMenuItem_DropDownOpening(object sender, EventArgs e)
		{
			// enable or disable the undo/redo menu items depending if there is some actions to undo/redo
			this.undoToolStripMenuItem.Enabled = mActionManager.CanUndo;
			this.redoToolStripMenuItem.Enabled = mActionManager.CanRedo;
			// add the name of the action to undo/redo
			this.undoToolStripMenuItem.Text = (this.undoToolStripMenuItem.Tag as string) + " " + mActionManager.UndoActionName;
			this.redoToolStripMenuItem.Text = (this.redoToolStripMenuItem.Tag as string) + " " + mActionManager.RedoActionName;
			// enable or disable the item related menu item
			this.mirrorItemToolStripMenuItem.Enabled = (mCurrentSelectedItem != null);
			this.setItemSpecialToolStripMenuItem.Enabled = (mCurrentSelectedItem != null) && Action.ActionSetItemSpecial.CanThisItemBeSetSpecial(mCurrentSelectedItem);
			this.deleteItemToolStripMenuItem.Enabled = (mCurrentSelectedItem != null);
		}

		private void mirrorItemToolStripMenuItem_Click(object sender, EventArgs e)
		{
			if (mCurrentSelectedItem != null)
				mActionManager.Do(new Action.ActionMirrorItem(mCurrentSelectedItem));
		}

		private void setItemSpecialToolStripMenuItem_Click(object sender, EventArgs e)
		{
			if (mCurrentSelectedItem != null)
				mActionManager.Do(new Action.ActionSetItemSpecial(mCurrentSelectedItem));
		}

		private void deleteItemToolStripMenuItem_Click(object sender, EventArgs e)
		{
			DeleteCurrentSelectedItem();
		}
		#endregion

		#region View Menu
		private void ToolStripMenuItemDisplayPuzzlePath_Click(object sender, EventArgs e)
		{
			SetPuzzlePathVisibility(ToolStripMenuItemDisplayPuzzlePath.Checked);
		}
		#endregion

		#region Help Menu
		private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
		{
			AboutBox aboutBox = new AboutBox();
			aboutBox.ShowDialog();
		}

		private void documentationToolStripMenuItem_Click(object sender, EventArgs e)
		{
			if (documentationToolStripMenuItem.Checked)
			{
				if (mDocumentationForm == null)
					mDocumentationForm = new DocumentationForm();
				mDocumentationForm.Show(this);
			}
			else if (mDocumentationForm != null)
			{
				mDocumentationForm.Close();
				mDocumentationForm = null;
			}
		}

		public void DocumentationFormClosedNotification()
		{
			documentationToolStripMenuItem.Checked = false;
			mDocumentationForm = null;
		}
		#endregion
		#endregion

		#region toolbar events
		private void toolStripButtonMirrorItem_Click(object sender, EventArgs e)
		{
			if (mCurrentSelectedItem != null)
				mActionManager.Do(new Action.ActionMirrorItem(mCurrentSelectedItem));
		}

		private void toolStripButtonSetItemSpecial_Click(object sender, EventArgs e)
		{
			if (mCurrentSelectedItem != null)
				mActionManager.Do(new Action.ActionSetItemSpecial(mCurrentSelectedItem));
		}

		private void toolStripButtonDeleteItem_Click(object sender, EventArgs e)
		{
			DeleteCurrentSelectedItem();
		}

		private void toolStripButtonShowPuzzlePath_CheckedChanged(object sender, EventArgs e)
		{
			SetPuzzlePathVisibility(toolStripButtonShowPuzzlePath.Checked);
		}

		private void SetPuzzlePathVisibility(bool isVisible)
		{
			// set the flag on the map
			mMap.IsPuzzlePathDrawn = isVisible;
			this.toolStripButtonShowPuzzlePath.Checked = isVisible;
			this.ToolStripMenuItemDisplayPuzzlePath.Checked = isVisible;
			// then redraw the map
			RedrawLevel();
		}
		#endregion

		#region edition event

		private void DeleteCurrentSelectedItem()
		{
			if (mCurrentSelectedItem != null)
				mActionManager.Do(new Action.ActionDeleteItem(mMap, mCurrentSelectedItem));
		}

		private void PictureBoxSprites_MouseClick(object sender, MouseEventArgs e)
		{
			int spriteX = e.Location.X / (PictureBoxWallSprites.Image.Width / 2);
			int spriteY = e.Location.Y / (PictureBoxWallSprites.Image.Height / 8);
			RedrawWallSpriteToolbox(spriteX, spriteY);
			RedrawItemsSpriteToolbox(-1, -1);
			// memorise the new selected sprite id
			mCurrentSelectedSpriteId = (byte)(spriteY + (8 * spriteX));
		}

		private void PictureBoxItems_MouseClick(object sender, MouseEventArgs e)
		{
			int spriteX = e.Location.X / (PictureBoxItems.Image.Width / 2);
			int spriteY = e.Location.Y / (PictureBoxItems.Image.Height / 8);
			RedrawWallSpriteToolbox(-1, -1);
			RedrawItemsSpriteToolbox(spriteX, spriteY);
			// memorise the new selected sprite id
			mCurrentSelectedSpriteId = (byte)(16 + spriteY + (8 * spriteX));
		}

		private void PictureBoxLevel_SizeChanged(object sender, EventArgs e)
		{
			// delete the image so that a new one can be recreated with the new size
			PictureBoxLevel.Image = null;
			RedrawLevel();
		}

		private void PictureBoxLevel_MouseEnter(object sender, EventArgs e)
		{
			PictureBoxLevel.Focus();
		}

		private Point ConvertMouseCoordToLevelCoord(Point mouseCoord)
		{
			return new Point((mMapCamera.X * Map.WALL_SPRITE_WIDTH) + (mouseCoord.X / mMap.PixelSize), (mMapCamera.Y * Map.WALL_SPRITE_HEIGHT) + (mouseCoord.Y / mMap.PixelSize));
		}

		private void PictureBoxLevel_MouseDown(object sender, MouseEventArgs e)
		{
			mLastMouseDownPosition = e.Location;
			mLastMouseMovePosition = e.Location;
			mLastMouseDownMapCamera = mMapCamera;

			switch (e.Button)
			{
				case MouseButtons.Left:
					Point mouseInLevelCoord = ConvertMouseCoordToLevelCoord(e.Location);

					// check if there is an arrow launcher selected
					if ((mCurrentSelectedItem != null) && (mCurrentSelectedItem.IsCoordOnMySensorExtrimity(mouseInLevelCoord, 2)))
					{
						mCurrentEditAction = EditAction.ADJUST_SENSOR;
						mOriginalSensorDistanceForAdjustDistance = mCurrentSelectedItem.SensorDistance;
					}
					else
					{
						mCurrentEditAction = EditAction.MOVE_ITEM;
						mCurrentSelectedItem = mMap.GetItemAt(mouseInLevelCoord);
						if (mCurrentSelectedItem != null)
						{
							mCurrentSelectedItemGrabDelta = new Point(mCurrentSelectedItem.X - mouseInLevelCoord.X, mCurrentSelectedItem.Y - mouseInLevelCoord.Y);
							mOriginalItemPositionForItemMove = mCurrentSelectedItem.Location;
						}
					}
					break;
			}
		}

		private void PictureBoxLevel_MouseMove(object sender, MouseEventArgs e)
		{
			// change the cursor, if we move hover a sensor
			if ((mCurrentSelectedItem != null) && (mCurrentSelectedItem.IsCoordOnMySensorExtrimity(ConvertMouseCoordToLevelCoord(e.Location), 2)))
				this.Cursor = Cursors.VSplit;
			else
				this.Cursor = Cursors.Default;

			switch (e.Button)
			{
				case MouseButtons.Left:
					if (mCurrentSelectedItem != null)
					{
						Point newLocation = ConvertMouseCoordToLevelCoord(e.Location);
						switch (mCurrentEditAction)
						{
							case EditAction.MOVE_ITEM:
								newLocation.X += mCurrentSelectedItemGrabDelta.X;
								newLocation.Y += mCurrentSelectedItemGrabDelta.Y;
								mCurrentSelectedItem.Move(newLocation);
								break;
							case EditAction.ADJUST_SENSOR:
								Point mouseDownInLevelCoord = ConvertMouseCoordToLevelCoord(mLastMouseMovePosition);
								int diff = newLocation.X - mouseDownInLevelCoord.X;
								if (mCurrentSelectedItem.IsMirror)
									mCurrentSelectedItem.SensorDistance -= diff;
								else
									mCurrentSelectedItem.SensorDistance += diff;
								break;
						}
						RedrawLevel();
					}
					break;
				case MouseButtons.Middle:
					PanLevelCamera(e.Location);
					break;
			}
			mLastMouseMovePosition = e.Location;
		}

		private void PictureBoxLevel_MouseUp(object sender, MouseEventArgs e)
		{
			switch (e.Button)
			{
				case MouseButtons.Left:
					try
					{
						if (mCurrentSelectedSpriteId >= 16)
						{
							if (mCurrentSelectedItem == null)
							{
								mActionManager.Do(new Action.ActionAddItem(mMap, (Items.Item.Type)(mCurrentSelectedSpriteId - 16), ConvertMouseCoordToLevelCoord(e.Location)));
							}
							else
							{
								Point newLocation = ConvertMouseCoordToLevelCoord(e.Location);

								switch (mCurrentEditAction)
								{
									case EditAction.MOVE_ITEM:
										newLocation.X += mCurrentSelectedItemGrabDelta.X;
										newLocation.Y += mCurrentSelectedItemGrabDelta.Y;
										mActionManager.Do(new Action.ActionMoveItem(mCurrentSelectedItem, mOriginalItemPositionForItemMove, newLocation));
										break;

									case EditAction.ADJUST_SENSOR:
										Point mouseDownInLevelCoord = ConvertMouseCoordToLevelCoord(mLastMouseMovePosition);
										int diff = newLocation.X - mouseDownInLevelCoord.X;
										if (mCurrentSelectedItem.IsMirror)
											mCurrentSelectedItem.SensorDistance -= diff;
										else
											mCurrentSelectedItem.SensorDistance += diff;
										// create the action
										mActionManager.Do(new Action.ActionAdjustSensor(mCurrentSelectedItem, mOriginalSensorDistanceForAdjustDistance, mCurrentSelectedItem.SensorDistance));
										break;
								}
							}
						}
						else
						{
							RedrawLevel();
						}
					}
					catch (MapSaveException)
					{
					}
					break;

				case MouseButtons.Middle:
					PanLevelCamera(e.Location);
					break;

				case MouseButtons.Right:
					if (mCurrentSelectedSpriteId < 16)
						mActionManager.Do(new Action.ActionSetMapSpriteId(mMap, mMap.GetSpriteCoordFromScreenCoord(mMapCamera, e.Location), mCurrentSelectedSpriteId));
					break;
			}
		}

		private void PictureBoxLevel_MouseWheel(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			int spriteSizeChange = (e.Delta / 100);
			mMap.PixelSize = mMap.PixelSize + spriteSizeChange;
			RedrawLevel();
		}

		private void MainForm_KeyUp(object sender, KeyEventArgs e)
		{
			if ((mCurrentSelectedItem != null) && ((e.KeyCode == Keys.Delete) || (e.KeyCode == Keys.Back)))
				DeleteCurrentSelectedItem();
		}
		#endregion

		#region Log

		public enum LogLevel
		{
			NORMAL,
			GOOD,
			WARNING,
			ERROR,
		}

		private static List<LogLevel> sLogLevelLines = new List<LogLevel>();

		public static void LogMessage(string message, LogLevel level)
		{
			// an array of colors depending on the level of the message
			Color[] messageColor = { Color.Black, Color.DarkGreen, Color.DarkOrange, Color.Red };

			// add the level to the message (so that it can be colored later)
			// message = "[" + level.ToString() + "] " + message;

			// get the main form
			MainForm mainForm = Application.OpenForms[0] as MainForm;
			// add the message in the list of string
			List<string> existingMessages = new List<string>(mainForm.textBoxLog.Lines);
			existingMessages.Add(message);
			mainForm.textBoxLog.Lines = existingMessages.ToArray();

			// add the log level to the list
			sLogLevelLines.Add(level);

			// set the colors for all the lines
			int startMessageIndex = 0;
			int endMessageIndex = 0;
			for (int i = 0; i < existingMessages.Count; ++i)
			{
				string msg = existingMessages[i];
				endMessageIndex += msg.Length;
				mainForm.textBoxLog.Select(startMessageIndex, endMessageIndex);
				mainForm.textBoxLog.SelectionColor = messageColor[(int)sLogLevelLines[i]];
				startMessageIndex += msg.Length + 1;
			}
		}

		private void toolStripMenuItemClear_Click(object sender, EventArgs e)
		{
			sLogLevelLines.Clear();
			this.textBoxLog.Clear();
		}

		private void toolStripMenuItemCopyToClipboard_Click(object sender, EventArgs e)
		{
			Clipboard.SetText(this.textBoxLog.Text);
		}
		#endregion
	}
}
