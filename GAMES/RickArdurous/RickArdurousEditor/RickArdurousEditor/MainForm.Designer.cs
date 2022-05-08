namespace RickArdurousEditor
{
	partial class MainForm
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
			this.ToolBar = new System.Windows.Forms.ToolStrip();
			this.toolStripButtonMirrorItem = new System.Windows.Forms.ToolStripButton();
			this.toolStripButtonSetItemSpecial = new System.Windows.Forms.ToolStripButton();
			this.toolStripButtonDeleteItem = new System.Windows.Forms.ToolStripButton();
			this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
			this.toolStripButtonShowPuzzlePath = new System.Windows.Forms.ToolStripButton();
			this.SplitContainerToolAndLevel = new System.Windows.Forms.SplitContainer();
			this.SplitContainerWallAndItems = new System.Windows.Forms.SplitContainer();
			this.PictureBoxWallSprites = new System.Windows.Forms.PictureBox();
			this.PictureBoxItems = new System.Windows.Forms.PictureBox();
			this.splitContainerMapAndLogWindow = new System.Windows.Forms.SplitContainer();
			this.PictureBoxLevel = new System.Windows.Forms.PictureBox();
			this.textBoxLog = new System.Windows.Forms.RichTextBox();
			this.contextMenuStripLogWindow = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.toolStripMenuItemClear = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripMenuItemCopyToClipboard = new System.Windows.Forms.ToolStripMenuItem();
			this.MenuStripMain = new System.Windows.Forms.MenuStrip();
			this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.newToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.saveAsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripSeparator5 = new System.Windows.Forms.ToolStripSeparator();
			this.preferencesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
			this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.editToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.undoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.redoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
			this.mirrorItemToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.setItemSpecialToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
			this.deleteItemToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.viewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.ToolStripMenuItemDisplayPuzzlePath = new System.Windows.Forms.ToolStripMenuItem();
			this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.aboutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.documentationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.openMapFileDialog = new System.Windows.Forms.OpenFileDialog();
			this.saveMapFileDialog = new System.Windows.Forms.SaveFileDialog();
			this.ToolBar.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.SplitContainerToolAndLevel)).BeginInit();
			this.SplitContainerToolAndLevel.Panel1.SuspendLayout();
			this.SplitContainerToolAndLevel.Panel2.SuspendLayout();
			this.SplitContainerToolAndLevel.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.SplitContainerWallAndItems)).BeginInit();
			this.SplitContainerWallAndItems.Panel1.SuspendLayout();
			this.SplitContainerWallAndItems.Panel2.SuspendLayout();
			this.SplitContainerWallAndItems.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.PictureBoxWallSprites)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.PictureBoxItems)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.splitContainerMapAndLogWindow)).BeginInit();
			this.splitContainerMapAndLogWindow.Panel1.SuspendLayout();
			this.splitContainerMapAndLogWindow.Panel2.SuspendLayout();
			this.splitContainerMapAndLogWindow.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.PictureBoxLevel)).BeginInit();
			this.contextMenuStripLogWindow.SuspendLayout();
			this.MenuStripMain.SuspendLayout();
			this.SuspendLayout();
			// 
			// ToolBar
			// 
			this.ToolBar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButtonMirrorItem,
            this.toolStripButtonSetItemSpecial,
            this.toolStripButtonDeleteItem,
            this.toolStripSeparator2,
            this.toolStripButtonShowPuzzlePath});
			this.ToolBar.Location = new System.Drawing.Point(0, 24);
			this.ToolBar.Name = "ToolBar";
			this.ToolBar.Size = new System.Drawing.Size(800, 25);
			this.ToolBar.TabIndex = 0;
			this.ToolBar.Text = "toolStrip1";
			// 
			// toolStripButtonMirrorItem
			// 
			this.toolStripButtonMirrorItem.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.toolStripButtonMirrorItem.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonMirrorItem.Image")));
			this.toolStripButtonMirrorItem.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripButtonMirrorItem.Name = "toolStripButtonMirrorItem";
			this.toolStripButtonMirrorItem.Size = new System.Drawing.Size(23, 22);
			this.toolStripButtonMirrorItem.Text = "Mirror";
			this.toolStripButtonMirrorItem.ToolTipText = "Mirror the selected Item";
			this.toolStripButtonMirrorItem.Click += new System.EventHandler(this.toolStripButtonMirrorItem_Click);
			// 
			// toolStripButtonSetItemSpecial
			// 
			this.toolStripButtonSetItemSpecial.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.toolStripButtonSetItemSpecial.Image = global::RickArdurousEditor.Properties.Resources.RespawnPointType;
			this.toolStripButtonSetItemSpecial.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripButtonSetItemSpecial.Name = "toolStripButtonSetItemSpecial";
			this.toolStripButtonSetItemSpecial.Size = new System.Drawing.Size(23, 22);
			this.toolStripButtonSetItemSpecial.Text = "toolStripButton1";
			this.toolStripButtonSetItemSpecial.ToolTipText = "Set the selected Item Special";
			this.toolStripButtonSetItemSpecial.Click += new System.EventHandler(this.toolStripButtonSetItemSpecial_Click);
			// 
			// toolStripButtonDeleteItem
			// 
			this.toolStripButtonDeleteItem.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.toolStripButtonDeleteItem.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonDeleteItem.Image")));
			this.toolStripButtonDeleteItem.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripButtonDeleteItem.Name = "toolStripButtonDeleteItem";
			this.toolStripButtonDeleteItem.Size = new System.Drawing.Size(23, 22);
			this.toolStripButtonDeleteItem.ToolTipText = "Delete the selected Item";
			this.toolStripButtonDeleteItem.Click += new System.EventHandler(this.toolStripButtonDeleteItem_Click);
			// 
			// toolStripSeparator2
			// 
			this.toolStripSeparator2.Name = "toolStripSeparator2";
			this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
			// 
			// toolStripButtonShowPuzzlePath
			// 
			this.toolStripButtonShowPuzzlePath.CheckOnClick = true;
			this.toolStripButtonShowPuzzlePath.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.toolStripButtonShowPuzzlePath.Image = global::RickArdurousEditor.Properties.Resources.PuzzlePathIcon;
			this.toolStripButtonShowPuzzlePath.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripButtonShowPuzzlePath.Name = "toolStripButtonShowPuzzlePath";
			this.toolStripButtonShowPuzzlePath.Size = new System.Drawing.Size(23, 22);
			this.toolStripButtonShowPuzzlePath.Text = "toolStripButton1";
			this.toolStripButtonShowPuzzlePath.ToolTipText = "Toggle the visibility of the Puzzle Path";
			this.toolStripButtonShowPuzzlePath.CheckedChanged += new System.EventHandler(this.toolStripButtonShowPuzzlePath_CheckedChanged);
			// 
			// SplitContainerToolAndLevel
			// 
			this.SplitContainerToolAndLevel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.SplitContainerToolAndLevel.Dock = System.Windows.Forms.DockStyle.Fill;
			this.SplitContainerToolAndLevel.Location = new System.Drawing.Point(0, 49);
			this.SplitContainerToolAndLevel.Name = "SplitContainerToolAndLevel";
			// 
			// SplitContainerToolAndLevel.Panel1
			// 
			this.SplitContainerToolAndLevel.Panel1.Controls.Add(this.SplitContainerWallAndItems);
			// 
			// SplitContainerToolAndLevel.Panel2
			// 
			this.SplitContainerToolAndLevel.Panel2.Controls.Add(this.splitContainerMapAndLogWindow);
			this.SplitContainerToolAndLevel.Size = new System.Drawing.Size(800, 401);
			this.SplitContainerToolAndLevel.SplitterDistance = 95;
			this.SplitContainerToolAndLevel.TabIndex = 1;
			// 
			// SplitContainerWallAndItems
			// 
			this.SplitContainerWallAndItems.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.SplitContainerWallAndItems.Dock = System.Windows.Forms.DockStyle.Fill;
			this.SplitContainerWallAndItems.Location = new System.Drawing.Point(0, 0);
			this.SplitContainerWallAndItems.Name = "SplitContainerWallAndItems";
			this.SplitContainerWallAndItems.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// SplitContainerWallAndItems.Panel1
			// 
			this.SplitContainerWallAndItems.Panel1.Controls.Add(this.PictureBoxWallSprites);
			// 
			// SplitContainerWallAndItems.Panel2
			// 
			this.SplitContainerWallAndItems.Panel2.Controls.Add(this.PictureBoxItems);
			this.SplitContainerWallAndItems.Size = new System.Drawing.Size(95, 401);
			this.SplitContainerWallAndItems.SplitterDistance = 255;
			this.SplitContainerWallAndItems.TabIndex = 1;
			// 
			// PictureBoxWallSprites
			// 
			this.PictureBoxWallSprites.Dock = System.Windows.Forms.DockStyle.Fill;
			this.PictureBoxWallSprites.Location = new System.Drawing.Point(0, 0);
			this.PictureBoxWallSprites.Name = "PictureBoxWallSprites";
			this.PictureBoxWallSprites.Size = new System.Drawing.Size(91, 251);
			this.PictureBoxWallSprites.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
			this.PictureBoxWallSprites.TabIndex = 0;
			this.PictureBoxWallSprites.TabStop = false;
			this.PictureBoxWallSprites.MouseClick += new System.Windows.Forms.MouseEventHandler(this.PictureBoxSprites_MouseClick);
			// 
			// PictureBoxItems
			// 
			this.PictureBoxItems.Dock = System.Windows.Forms.DockStyle.Fill;
			this.PictureBoxItems.Location = new System.Drawing.Point(0, 0);
			this.PictureBoxItems.Name = "PictureBoxItems";
			this.PictureBoxItems.Size = new System.Drawing.Size(91, 138);
			this.PictureBoxItems.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
			this.PictureBoxItems.TabIndex = 0;
			this.PictureBoxItems.TabStop = false;
			this.PictureBoxItems.MouseClick += new System.Windows.Forms.MouseEventHandler(this.PictureBoxItems_MouseClick);
			// 
			// splitContainerMapAndLogWindow
			// 
			this.splitContainerMapAndLogWindow.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.splitContainerMapAndLogWindow.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainerMapAndLogWindow.Location = new System.Drawing.Point(0, 0);
			this.splitContainerMapAndLogWindow.Name = "splitContainerMapAndLogWindow";
			this.splitContainerMapAndLogWindow.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// splitContainerMapAndLogWindow.Panel1
			// 
			this.splitContainerMapAndLogWindow.Panel1.Controls.Add(this.PictureBoxLevel);
			// 
			// splitContainerMapAndLogWindow.Panel2
			// 
			this.splitContainerMapAndLogWindow.Panel2.Controls.Add(this.textBoxLog);
			this.splitContainerMapAndLogWindow.Size = new System.Drawing.Size(701, 401);
			this.splitContainerMapAndLogWindow.SplitterDistance = 308;
			this.splitContainerMapAndLogWindow.TabIndex = 1;
			// 
			// PictureBoxLevel
			// 
			this.PictureBoxLevel.Dock = System.Windows.Forms.DockStyle.Fill;
			this.PictureBoxLevel.Location = new System.Drawing.Point(0, 0);
			this.PictureBoxLevel.Name = "PictureBoxLevel";
			this.PictureBoxLevel.Size = new System.Drawing.Size(697, 304);
			this.PictureBoxLevel.TabIndex = 0;
			this.PictureBoxLevel.TabStop = false;
			this.PictureBoxLevel.SizeChanged += new System.EventHandler(this.PictureBoxLevel_SizeChanged);
			this.PictureBoxLevel.MouseDown += new System.Windows.Forms.MouseEventHandler(this.PictureBoxLevel_MouseDown);
			this.PictureBoxLevel.MouseEnter += new System.EventHandler(this.PictureBoxLevel_MouseEnter);
			this.PictureBoxLevel.MouseMove += new System.Windows.Forms.MouseEventHandler(this.PictureBoxLevel_MouseMove);
			this.PictureBoxLevel.MouseUp += new System.Windows.Forms.MouseEventHandler(this.PictureBoxLevel_MouseUp);
			this.PictureBoxLevel.MouseWheel += new System.Windows.Forms.MouseEventHandler(this.PictureBoxLevel_MouseWheel);
			// 
			// textBoxLog
			// 
			this.textBoxLog.ContextMenuStrip = this.contextMenuStripLogWindow;
			this.textBoxLog.Dock = System.Windows.Forms.DockStyle.Fill;
			this.textBoxLog.Location = new System.Drawing.Point(0, 0);
			this.textBoxLog.Name = "textBoxLog";
			this.textBoxLog.Size = new System.Drawing.Size(697, 85);
			this.textBoxLog.TabIndex = 0;
			this.textBoxLog.Text = "";
			// 
			// contextMenuStripLogWindow
			// 
			this.contextMenuStripLogWindow.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItemClear,
            this.toolStripMenuItemCopyToClipboard});
			this.contextMenuStripLogWindow.Name = "contextMenuStripLogWindow";
			this.contextMenuStripLogWindow.Size = new System.Drawing.Size(172, 48);
			this.contextMenuStripLogWindow.Text = "Clear";
			// 
			// toolStripMenuItemClear
			// 
			this.toolStripMenuItemClear.Name = "toolStripMenuItemClear";
			this.toolStripMenuItemClear.Size = new System.Drawing.Size(171, 22);
			this.toolStripMenuItemClear.Text = "Clear";
			this.toolStripMenuItemClear.Click += new System.EventHandler(this.toolStripMenuItemClear_Click);
			// 
			// toolStripMenuItemCopyToClipboard
			// 
			this.toolStripMenuItemCopyToClipboard.Name = "toolStripMenuItemCopyToClipboard";
			this.toolStripMenuItemCopyToClipboard.Size = new System.Drawing.Size(171, 22);
			this.toolStripMenuItemCopyToClipboard.Text = "Copy to Clipboard";
			this.toolStripMenuItemCopyToClipboard.Click += new System.EventHandler(this.toolStripMenuItemCopyToClipboard_Click);
			// 
			// MenuStripMain
			// 
			this.MenuStripMain.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.editToolStripMenuItem,
            this.viewToolStripMenuItem,
            this.helpToolStripMenuItem});
			this.MenuStripMain.Location = new System.Drawing.Point(0, 0);
			this.MenuStripMain.Name = "MenuStripMain";
			this.MenuStripMain.Size = new System.Drawing.Size(800, 24);
			this.MenuStripMain.TabIndex = 2;
			// 
			// fileToolStripMenuItem
			// 
			this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newToolStripMenuItem,
            this.openToolStripMenuItem,
            this.saveToolStripMenuItem,
            this.saveAsToolStripMenuItem,
            this.toolStripSeparator5,
            this.preferencesToolStripMenuItem,
            this.toolStripSeparator1,
            this.exitToolStripMenuItem});
			this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
			this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
			this.fileToolStripMenuItem.Text = "File";
			// 
			// newToolStripMenuItem
			// 
			this.newToolStripMenuItem.Name = "newToolStripMenuItem";
			this.newToolStripMenuItem.Size = new System.Drawing.Size(155, 22);
			this.newToolStripMenuItem.Text = "New";
			this.newToolStripMenuItem.Click += new System.EventHandler(this.newToolStripMenuItem_Click);
			// 
			// openToolStripMenuItem
			// 
			this.openToolStripMenuItem.Name = "openToolStripMenuItem";
			this.openToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
			this.openToolStripMenuItem.Size = new System.Drawing.Size(155, 22);
			this.openToolStripMenuItem.Text = "Open...";
			this.openToolStripMenuItem.Click += new System.EventHandler(this.openToolStripMenuItem_Click);
			// 
			// saveToolStripMenuItem
			// 
			this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
			this.saveToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
			this.saveToolStripMenuItem.Size = new System.Drawing.Size(155, 22);
			this.saveToolStripMenuItem.Text = "Save";
			this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
			// 
			// saveAsToolStripMenuItem
			// 
			this.saveAsToolStripMenuItem.Name = "saveAsToolStripMenuItem";
			this.saveAsToolStripMenuItem.Size = new System.Drawing.Size(155, 22);
			this.saveAsToolStripMenuItem.Text = "Save As...";
			this.saveAsToolStripMenuItem.Click += new System.EventHandler(this.saveAsToolStripMenuItem_Click);
			// 
			// toolStripSeparator5
			// 
			this.toolStripSeparator5.Name = "toolStripSeparator5";
			this.toolStripSeparator5.Size = new System.Drawing.Size(152, 6);
			// 
			// preferencesToolStripMenuItem
			// 
			this.preferencesToolStripMenuItem.Name = "preferencesToolStripMenuItem";
			this.preferencesToolStripMenuItem.Size = new System.Drawing.Size(155, 22);
			this.preferencesToolStripMenuItem.Text = "Preferences...";
			this.preferencesToolStripMenuItem.Click += new System.EventHandler(this.preferencesToolStripMenuItem_Click);
			// 
			// toolStripSeparator1
			// 
			this.toolStripSeparator1.Name = "toolStripSeparator1";
			this.toolStripSeparator1.Size = new System.Drawing.Size(152, 6);
			// 
			// exitToolStripMenuItem
			// 
			this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
			this.exitToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Alt | System.Windows.Forms.Keys.F4)));
			this.exitToolStripMenuItem.Size = new System.Drawing.Size(155, 22);
			this.exitToolStripMenuItem.Text = "Exit";
			this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
			// 
			// editToolStripMenuItem
			// 
			this.editToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.undoToolStripMenuItem,
            this.redoToolStripMenuItem,
            this.toolStripSeparator3,
            this.mirrorItemToolStripMenuItem,
            this.setItemSpecialToolStripMenuItem,
            this.toolStripSeparator4,
            this.deleteItemToolStripMenuItem});
			this.editToolStripMenuItem.Name = "editToolStripMenuItem";
			this.editToolStripMenuItem.Size = new System.Drawing.Size(39, 20);
			this.editToolStripMenuItem.Text = "Edit";
			this.editToolStripMenuItem.DropDownOpening += new System.EventHandler(this.editToolStripMenuItem_DropDownOpening);
			// 
			// undoToolStripMenuItem
			// 
			this.undoToolStripMenuItem.Name = "undoToolStripMenuItem";
			this.undoToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Z)));
			this.undoToolStripMenuItem.Size = new System.Drawing.Size(157, 22);
			this.undoToolStripMenuItem.Tag = "Undo";
			this.undoToolStripMenuItem.Text = "Undo";
			this.undoToolStripMenuItem.Click += new System.EventHandler(this.undoToolStripMenuItem_Click);
			// 
			// redoToolStripMenuItem
			// 
			this.redoToolStripMenuItem.Name = "redoToolStripMenuItem";
			this.redoToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Y)));
			this.redoToolStripMenuItem.Size = new System.Drawing.Size(157, 22);
			this.redoToolStripMenuItem.Tag = "Redo";
			this.redoToolStripMenuItem.Text = "Redo";
			this.redoToolStripMenuItem.Click += new System.EventHandler(this.redoToolStripMenuItem_Click);
			// 
			// toolStripSeparator3
			// 
			this.toolStripSeparator3.Name = "toolStripSeparator3";
			this.toolStripSeparator3.Size = new System.Drawing.Size(154, 6);
			// 
			// mirrorItemToolStripMenuItem
			// 
			this.mirrorItemToolStripMenuItem.Name = "mirrorItemToolStripMenuItem";
			this.mirrorItemToolStripMenuItem.Size = new System.Drawing.Size(157, 22);
			this.mirrorItemToolStripMenuItem.Text = "Mirror Item";
			this.mirrorItemToolStripMenuItem.Click += new System.EventHandler(this.mirrorItemToolStripMenuItem_Click);
			// 
			// setItemSpecialToolStripMenuItem
			// 
			this.setItemSpecialToolStripMenuItem.Name = "setItemSpecialToolStripMenuItem";
			this.setItemSpecialToolStripMenuItem.Size = new System.Drawing.Size(157, 22);
			this.setItemSpecialToolStripMenuItem.Text = "Set Item Special";
			this.setItemSpecialToolStripMenuItem.Click += new System.EventHandler(this.setItemSpecialToolStripMenuItem_Click);
			// 
			// toolStripSeparator4
			// 
			this.toolStripSeparator4.Name = "toolStripSeparator4";
			this.toolStripSeparator4.Size = new System.Drawing.Size(154, 6);
			// 
			// deleteItemToolStripMenuItem
			// 
			this.deleteItemToolStripMenuItem.Name = "deleteItemToolStripMenuItem";
			this.deleteItemToolStripMenuItem.Size = new System.Drawing.Size(157, 22);
			this.deleteItemToolStripMenuItem.Text = "Delete Item";
			this.deleteItemToolStripMenuItem.Click += new System.EventHandler(this.deleteItemToolStripMenuItem_Click);
			// 
			// viewToolStripMenuItem
			// 
			this.viewToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItemDisplayPuzzlePath});
			this.viewToolStripMenuItem.Name = "viewToolStripMenuItem";
			this.viewToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
			this.viewToolStripMenuItem.Text = "View";
			// 
			// ToolStripMenuItemDisplayPuzzlePath
			// 
			this.ToolStripMenuItemDisplayPuzzlePath.CheckOnClick = true;
			this.ToolStripMenuItemDisplayPuzzlePath.Name = "ToolStripMenuItemDisplayPuzzlePath";
			this.ToolStripMenuItemDisplayPuzzlePath.Size = new System.Drawing.Size(175, 22);
			this.ToolStripMenuItemDisplayPuzzlePath.Text = "Display Puzzle Path";
			this.ToolStripMenuItemDisplayPuzzlePath.Click += new System.EventHandler(this.ToolStripMenuItemDisplayPuzzlePath_Click);
			// 
			// helpToolStripMenuItem
			// 
			this.helpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.aboutToolStripMenuItem,
            this.documentationToolStripMenuItem});
			this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
			this.helpToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
			this.helpToolStripMenuItem.Text = "Help";
			// 
			// aboutToolStripMenuItem
			// 
			this.aboutToolStripMenuItem.Name = "aboutToolStripMenuItem";
			this.aboutToolStripMenuItem.Size = new System.Drawing.Size(166, 22);
			this.aboutToolStripMenuItem.Text = "About...";
			this.aboutToolStripMenuItem.Click += new System.EventHandler(this.aboutToolStripMenuItem_Click);
			// 
			// documentationToolStripMenuItem
			// 
			this.documentationToolStripMenuItem.CheckOnClick = true;
			this.documentationToolStripMenuItem.Name = "documentationToolStripMenuItem";
			this.documentationToolStripMenuItem.Size = new System.Drawing.Size(166, 22);
			this.documentationToolStripMenuItem.Text = "Documentation...";
			this.documentationToolStripMenuItem.Click += new System.EventHandler(this.documentationToolStripMenuItem_Click);
			// 
			// openMapFileDialog
			// 
			this.openMapFileDialog.DefaultExt = "cpp";
			this.openMapFileDialog.FileName = "MapData.cpp";
			this.openMapFileDialog.Filter = "C++ files|*.cpp|All files|*.*";
			this.openMapFileDialog.Title = "Open Map file";
			// 
			// saveMapFileDialog
			// 
			this.saveMapFileDialog.DefaultExt = "cpp";
			this.saveMapFileDialog.FileName = "MapData.cpp";
			this.saveMapFileDialog.Filter = "C++ files|*.cpp|All files|*.*";
			this.saveMapFileDialog.Title = "Save Map as";
			// 
			// MainForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(800, 450);
			this.Controls.Add(this.SplitContainerToolAndLevel);
			this.Controls.Add(this.ToolBar);
			this.Controls.Add(this.MenuStripMain);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.KeyPreview = true;
			this.MainMenuStrip = this.MenuStripMain;
			this.Name = "MainForm";
			this.Text = "Rick Ardurous Editor";
			this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.MainForm_KeyUp);
			this.ToolBar.ResumeLayout(false);
			this.ToolBar.PerformLayout();
			this.SplitContainerToolAndLevel.Panel1.ResumeLayout(false);
			this.SplitContainerToolAndLevel.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.SplitContainerToolAndLevel)).EndInit();
			this.SplitContainerToolAndLevel.ResumeLayout(false);
			this.SplitContainerWallAndItems.Panel1.ResumeLayout(false);
			this.SplitContainerWallAndItems.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.SplitContainerWallAndItems)).EndInit();
			this.SplitContainerWallAndItems.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.PictureBoxWallSprites)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.PictureBoxItems)).EndInit();
			this.splitContainerMapAndLogWindow.Panel1.ResumeLayout(false);
			this.splitContainerMapAndLogWindow.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainerMapAndLogWindow)).EndInit();
			this.splitContainerMapAndLogWindow.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.PictureBoxLevel)).EndInit();
			this.contextMenuStripLogWindow.ResumeLayout(false);
			this.MenuStripMain.ResumeLayout(false);
			this.MenuStripMain.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.ToolStrip ToolBar;
		private System.Windows.Forms.SplitContainer SplitContainerToolAndLevel;
		private System.Windows.Forms.PictureBox PictureBoxWallSprites;
		private System.Windows.Forms.PictureBox PictureBoxLevel;
		private System.Windows.Forms.MenuStrip MenuStripMain;
		private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
		private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
		private System.Windows.Forms.SplitContainer SplitContainerWallAndItems;
		private System.Windows.Forms.PictureBox PictureBoxItems;
		private System.Windows.Forms.ToolStripButton toolStripButtonMirrorItem;
		private System.Windows.Forms.ToolStripButton toolStripButtonDeleteItem;
		private System.Windows.Forms.ToolStripButton toolStripButtonSetItemSpecial;
		private System.Windows.Forms.SplitContainer splitContainerMapAndLogWindow;
		private System.Windows.Forms.RichTextBox textBoxLog;
		private System.Windows.Forms.ToolStripButton toolStripButtonShowPuzzlePath;
		private System.Windows.Forms.ContextMenuStrip contextMenuStripLogWindow;
		private System.Windows.Forms.ToolStripMenuItem toolStripMenuItemClear;
		private System.Windows.Forms.ToolStripMenuItem toolStripMenuItemCopyToClipboard;
		private System.Windows.Forms.ToolStripMenuItem editToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem undoToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem redoToolStripMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
		private System.Windows.Forms.ToolStripMenuItem mirrorItemToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem setItemSpecialToolStripMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
		private System.Windows.Forms.ToolStripMenuItem deleteItemToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem viewToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItemDisplayPuzzlePath;
		private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem aboutToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem documentationToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem preferencesToolStripMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator5;
		private System.Windows.Forms.ToolStripMenuItem saveAsToolStripMenuItem;
		private System.Windows.Forms.OpenFileDialog openMapFileDialog;
		private System.Windows.Forms.SaveFileDialog saveMapFileDialog;
		private System.Windows.Forms.ToolStripMenuItem newToolStripMenuItem;
	}
}

