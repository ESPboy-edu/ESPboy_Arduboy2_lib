namespace ArudlemEditor
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
			System.Windows.Forms.ListViewItem listViewItem2 = new System.Windows.Forms.ListViewItem(new string[] {
            "Set Sprite",
            "Delete Sprite",
            "Mirror Sprite"}, -1);
			this.menuStrip1 = new System.Windows.Forms.MenuStrip();
			this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.newToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.loadLevelFromClipboardToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.saveLevelToClipboardToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.saveLevelDataToClipboardToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.editToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.undoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.redoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
			this.shiftLevelToRightToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.shiftLevelToLeftToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.shiftLevelUpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.shiftLevelDownToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.viewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.drawLevelGridToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.drawStartAndHomeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.splitContainer1 = new System.Windows.Forms.SplitContainer();
			this.splitContainer2 = new System.Windows.Forms.SplitContainer();
			this.MapSpritePictureBox = new System.Windows.Forms.PictureBox();
			this.LevelPictureBox = new System.Windows.Forms.PictureBox();
			this.StatusBar = new System.Windows.Forms.StatusStrip();
			this.StatusBarText = new System.Windows.Forms.ToolStripStatusLabel();
			this.groupBox5 = new System.Windows.Forms.GroupBox();
			this.ParachuteNumeric = new System.Windows.Forms.NumericUpDown();
			this.label17 = new System.Windows.Forms.Label();
			this.ClimbNumeric = new System.Windows.Forms.NumericUpDown();
			this.label18 = new System.Windows.Forms.Label();
			this.StairNumeric = new System.Windows.Forms.NumericUpDown();
			this.label19 = new System.Windows.Forms.Label();
			this.DigVertNumeric = new System.Windows.Forms.NumericUpDown();
			this.label14 = new System.Windows.Forms.Label();
			this.DigHorizNumeric = new System.Windows.Forms.NumericUpDown();
			this.label15 = new System.Windows.Forms.Label();
			this.DigDiagNumeric = new System.Windows.Forms.NumericUpDown();
			this.label16 = new System.Windows.Forms.Label();
			this.BombNumeric = new System.Windows.Forms.NumericUpDown();
			this.label13 = new System.Windows.Forms.Label();
			this.BlockNumeric = new System.Windows.Forms.NumericUpDown();
			this.label12 = new System.Windows.Forms.Label();
			this.WalkNumeric = new System.Windows.Forms.NumericUpDown();
			this.label11 = new System.Windows.Forms.Label();
			this.groupBox4 = new System.Windows.Forms.GroupBox();
			this.label20 = new System.Windows.Forms.Label();
			this.MinDropSpeedNumeric = new System.Windows.Forms.NumericUpDown();
			this.label10 = new System.Windows.Forms.Label();
			this.RequiredLemCountNumeric = new System.Windows.Forms.NumericUpDown();
			this.label9 = new System.Windows.Forms.Label();
			this.SpawnLemCountNumeric = new System.Windows.Forms.NumericUpDown();
			this.label8 = new System.Windows.Forms.Label();
			this.label7 = new System.Windows.Forms.Label();
			this.TimeSecNumeric = new System.Windows.Forms.NumericUpDown();
			this.label6 = new System.Windows.Forms.Label();
			this.TimeMinNumeric = new System.Windows.Forms.NumericUpDown();
			this.label5 = new System.Windows.Forms.Label();
			this.HomeYNumeric = new System.Windows.Forms.NumericUpDown();
			this.HomeXNumeric = new System.Windows.Forms.NumericUpDown();
			this.StartYNumeric = new System.Windows.Forms.NumericUpDown();
			this.StartXNumeric = new System.Windows.Forms.NumericUpDown();
			this.label4 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.groupBox3 = new System.Windows.Forms.GroupBox();
			this.TrimLevelRightCheckBox = new System.Windows.Forms.CheckBox();
			this.ExportWithWinEOLCheckBox = new System.Windows.Forms.CheckBox();
			this.TrimLevelLeftCheckBox = new System.Windows.Forms.CheckBox();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.HelpTableListView = new System.Windows.Forms.ListView();
			this.columnHeader1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.columnHeader2 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.columnHeader3 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.label2 = new System.Windows.Forms.Label();
			this.MapIdsTextBox = new System.Windows.Forms.TextBox();
			this.LocaMapNameTextBox = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.menuStrip1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
			this.splitContainer1.Panel1.SuspendLayout();
			this.splitContainer1.Panel2.SuspendLayout();
			this.splitContainer1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
			this.splitContainer2.Panel1.SuspendLayout();
			this.splitContainer2.Panel2.SuspendLayout();
			this.splitContainer2.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.MapSpritePictureBox)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.LevelPictureBox)).BeginInit();
			this.StatusBar.SuspendLayout();
			this.groupBox5.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.ParachuteNumeric)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.ClimbNumeric)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.StairNumeric)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.DigVertNumeric)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.DigHorizNumeric)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.DigDiagNumeric)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.BombNumeric)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.BlockNumeric)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.WalkNumeric)).BeginInit();
			this.groupBox4.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.MinDropSpeedNumeric)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.RequiredLemCountNumeric)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.SpawnLemCountNumeric)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.TimeSecNumeric)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.TimeMinNumeric)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.HomeYNumeric)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.HomeXNumeric)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.StartYNumeric)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.StartXNumeric)).BeginInit();
			this.groupBox3.SuspendLayout();
			this.groupBox2.SuspendLayout();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// menuStrip1
			// 
			this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.editToolStripMenuItem,
            this.viewToolStripMenuItem});
			this.menuStrip1.Location = new System.Drawing.Point(0, 0);
			this.menuStrip1.Name = "menuStrip1";
			this.menuStrip1.Size = new System.Drawing.Size(740, 24);
			this.menuStrip1.TabIndex = 0;
			this.menuStrip1.Text = "menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newToolStripMenuItem,
            this.loadLevelFromClipboardToolStripMenuItem,
            this.saveLevelToClipboardToolStripMenuItem,
            this.saveLevelDataToClipboardToolStripMenuItem,
            this.exitToolStripMenuItem});
			this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
			this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
			this.fileToolStripMenuItem.Text = "File";
			// 
			// newToolStripMenuItem
			// 
			this.newToolStripMenuItem.Name = "newToolStripMenuItem";
			this.newToolStripMenuItem.Size = new System.Drawing.Size(317, 22);
			this.newToolStripMenuItem.Text = "New Level";
			this.newToolStripMenuItem.Click += new System.EventHandler(this.newToolStripMenuItem_Click);
			// 
			// loadLevelFromClipboardToolStripMenuItem
			// 
			this.loadLevelFromClipboardToolStripMenuItem.Name = "loadLevelFromClipboardToolStripMenuItem";
			this.loadLevelFromClipboardToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.V)));
			this.loadLevelFromClipboardToolStripMenuItem.Size = new System.Drawing.Size(317, 22);
			this.loadLevelFromClipboardToolStripMenuItem.Text = "Load Level/Level Data  from Clipboard";
			this.loadLevelFromClipboardToolStripMenuItem.Click += new System.EventHandler(this.loadLevelFromClipboardToolStripMenuItem_Click);
			// 
			// saveLevelToClipboardToolStripMenuItem
			// 
			this.saveLevelToClipboardToolStripMenuItem.Name = "saveLevelToClipboardToolStripMenuItem";
			this.saveLevelToClipboardToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.C)));
			this.saveLevelToClipboardToolStripMenuItem.Size = new System.Drawing.Size(317, 22);
			this.saveLevelToClipboardToolStripMenuItem.Text = "Save Level to Clipboard";
			this.saveLevelToClipboardToolStripMenuItem.Click += new System.EventHandler(this.saveLevelToClipboardToolStripMenuItem_Click);
			// 
			// saveLevelDataToClipboardToolStripMenuItem
			// 
			this.saveLevelDataToClipboardToolStripMenuItem.Name = "saveLevelDataToClipboardToolStripMenuItem";
			this.saveLevelDataToClipboardToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift)
						| System.Windows.Forms.Keys.C)));
			this.saveLevelDataToClipboardToolStripMenuItem.Size = new System.Drawing.Size(317, 22);
			this.saveLevelDataToClipboardToolStripMenuItem.Text = "Save Level Data to Clipboard";
			this.saveLevelDataToClipboardToolStripMenuItem.Click += new System.EventHandler(this.saveLevelDataToClipboardToolStripMenuItem_Click);
			// 
			// exitToolStripMenuItem
			// 
			this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
			this.exitToolStripMenuItem.Size = new System.Drawing.Size(317, 22);
			this.exitToolStripMenuItem.Text = "Exit";
			this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
			// 
			// editToolStripMenuItem
			// 
			this.editToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.undoToolStripMenuItem,
            this.redoToolStripMenuItem,
            this.toolStripSeparator1,
            this.shiftLevelToRightToolStripMenuItem,
            this.shiftLevelToLeftToolStripMenuItem,
            this.shiftLevelUpToolStripMenuItem,
            this.shiftLevelDownToolStripMenuItem});
			this.editToolStripMenuItem.Name = "editToolStripMenuItem";
			this.editToolStripMenuItem.Size = new System.Drawing.Size(39, 20);
			this.editToolStripMenuItem.Text = "Edit";
			// 
			// undoToolStripMenuItem
			// 
			this.undoToolStripMenuItem.Name = "undoToolStripMenuItem";
			this.undoToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Z)));
			this.undoToolStripMenuItem.Size = new System.Drawing.Size(235, 22);
			this.undoToolStripMenuItem.Text = "Undo";
			this.undoToolStripMenuItem.Click += new System.EventHandler(this.undoToolStripMenuItem_Click);
			// 
			// redoToolStripMenuItem
			// 
			this.redoToolStripMenuItem.Name = "redoToolStripMenuItem";
			this.redoToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Y)));
			this.redoToolStripMenuItem.Size = new System.Drawing.Size(235, 22);
			this.redoToolStripMenuItem.Text = "Redo";
			this.redoToolStripMenuItem.Click += new System.EventHandler(this.redoToolStripMenuItem_Click);
			// 
			// toolStripSeparator1
			// 
			this.toolStripSeparator1.Name = "toolStripSeparator1";
			this.toolStripSeparator1.Size = new System.Drawing.Size(232, 6);
			// 
			// shiftLevelToRightToolStripMenuItem
			// 
			this.shiftLevelToRightToolStripMenuItem.Name = "shiftLevelToRightToolStripMenuItem";
			this.shiftLevelToRightToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Right)));
			this.shiftLevelToRightToolStripMenuItem.Size = new System.Drawing.Size(235, 22);
			this.shiftLevelToRightToolStripMenuItem.Text = "Shift Level to Right";
			this.shiftLevelToRightToolStripMenuItem.Click += new System.EventHandler(this.shiftLevelToRightToolStripMenuItem_Click);
			// 
			// shiftLevelToLeftToolStripMenuItem
			// 
			this.shiftLevelToLeftToolStripMenuItem.Name = "shiftLevelToLeftToolStripMenuItem";
			this.shiftLevelToLeftToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Left)));
			this.shiftLevelToLeftToolStripMenuItem.Size = new System.Drawing.Size(235, 22);
			this.shiftLevelToLeftToolStripMenuItem.Text = "Shift Level to Left";
			this.shiftLevelToLeftToolStripMenuItem.Click += new System.EventHandler(this.shiftLevelToLeftToolStripMenuItem_Click);
			// 
			// shiftLevelUpToolStripMenuItem
			// 
			this.shiftLevelUpToolStripMenuItem.Name = "shiftLevelUpToolStripMenuItem";
			this.shiftLevelUpToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Up)));
			this.shiftLevelUpToolStripMenuItem.Size = new System.Drawing.Size(235, 22);
			this.shiftLevelUpToolStripMenuItem.Text = "Shift Level Up";
			this.shiftLevelUpToolStripMenuItem.Click += new System.EventHandler(this.shiftLevelUpToolStripMenuItem_Click);
			// 
			// shiftLevelDownToolStripMenuItem
			// 
			this.shiftLevelDownToolStripMenuItem.Name = "shiftLevelDownToolStripMenuItem";
			this.shiftLevelDownToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Down)));
			this.shiftLevelDownToolStripMenuItem.Size = new System.Drawing.Size(235, 22);
			this.shiftLevelDownToolStripMenuItem.Text = "Shift Level Down";
			this.shiftLevelDownToolStripMenuItem.Click += new System.EventHandler(this.shiftLevelDownToolStripMenuItem_Click);
			// 
			// viewToolStripMenuItem
			// 
			this.viewToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.drawLevelGridToolStripMenuItem,
            this.drawStartAndHomeToolStripMenuItem});
			this.viewToolStripMenuItem.Name = "viewToolStripMenuItem";
			this.viewToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
			this.viewToolStripMenuItem.Text = "View";
			this.viewToolStripMenuItem.Click += new System.EventHandler(this.drawLevelGridToolStripMenuItem_Click);
			// 
			// drawLevelGridToolStripMenuItem
			// 
			this.drawLevelGridToolStripMenuItem.Checked = true;
			this.drawLevelGridToolStripMenuItem.CheckOnClick = true;
			this.drawLevelGridToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
			this.drawLevelGridToolStripMenuItem.Name = "drawLevelGridToolStripMenuItem";
			this.drawLevelGridToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F5;
			this.drawLevelGridToolStripMenuItem.Size = new System.Drawing.Size(206, 22);
			this.drawLevelGridToolStripMenuItem.Text = "Draw Level Grid";
			this.drawLevelGridToolStripMenuItem.Click += new System.EventHandler(this.drawLevelGridToolStripMenuItem_Click);
			// 
			// drawStartAndHomeToolStripMenuItem
			// 
			this.drawStartAndHomeToolStripMenuItem.Checked = true;
			this.drawStartAndHomeToolStripMenuItem.CheckOnClick = true;
			this.drawStartAndHomeToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
			this.drawStartAndHomeToolStripMenuItem.Name = "drawStartAndHomeToolStripMenuItem";
			this.drawStartAndHomeToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F6;
			this.drawStartAndHomeToolStripMenuItem.Size = new System.Drawing.Size(206, 22);
			this.drawStartAndHomeToolStripMenuItem.Text = "Draw Start and Home";
			this.drawStartAndHomeToolStripMenuItem.Click += new System.EventHandler(this.drawStartAndHomeToolStripMenuItem_Click);
			// 
			// splitContainer1
			// 
			this.splitContainer1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer1.Location = new System.Drawing.Point(0, 24);
			this.splitContainer1.Name = "splitContainer1";
			this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// splitContainer1.Panel1
			// 
			this.splitContainer1.Panel1.Controls.Add(this.splitContainer2);
			// 
			// splitContainer1.Panel2
			// 
			this.splitContainer1.Panel2.Controls.Add(this.StatusBar);
			this.splitContainer1.Panel2.Controls.Add(this.groupBox5);
			this.splitContainer1.Panel2.Controls.Add(this.groupBox4);
			this.splitContainer1.Panel2.Controls.Add(this.groupBox3);
			this.splitContainer1.Panel2.Controls.Add(this.groupBox2);
			this.splitContainer1.Panel2.Controls.Add(this.groupBox1);
			this.splitContainer1.Size = new System.Drawing.Size(740, 470);
			this.splitContainer1.SplitterDistance = 213;
			this.splitContainer1.TabIndex = 1;
			// 
			// splitContainer2
			// 
			this.splitContainer2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer2.Location = new System.Drawing.Point(0, 0);
			this.splitContainer2.Name = "splitContainer2";
			// 
			// splitContainer2.Panel1
			// 
			this.splitContainer2.Panel1.Controls.Add(this.MapSpritePictureBox);
			// 
			// splitContainer2.Panel2
			// 
			this.splitContainer2.Panel2.Controls.Add(this.LevelPictureBox);
			this.splitContainer2.Size = new System.Drawing.Size(740, 213);
			this.splitContainer2.SplitterDistance = 192;
			this.splitContainer2.TabIndex = 0;
			// 
			// MapSpritePictureBox
			// 
			this.MapSpritePictureBox.Dock = System.Windows.Forms.DockStyle.Fill;
			this.MapSpritePictureBox.Location = new System.Drawing.Point(0, 0);
			this.MapSpritePictureBox.Name = "MapSpritePictureBox";
			this.MapSpritePictureBox.Size = new System.Drawing.Size(188, 209);
			this.MapSpritePictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
			this.MapSpritePictureBox.TabIndex = 0;
			this.MapSpritePictureBox.TabStop = false;
			this.MapSpritePictureBox.MouseClick += new System.Windows.Forms.MouseEventHandler(this.MapSpritePictureBox_MouseClick);
			// 
			// LevelPictureBox
			// 
			this.LevelPictureBox.Dock = System.Windows.Forms.DockStyle.Fill;
			this.LevelPictureBox.Location = new System.Drawing.Point(0, 0);
			this.LevelPictureBox.Name = "LevelPictureBox";
			this.LevelPictureBox.Size = new System.Drawing.Size(540, 209);
			this.LevelPictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
			this.LevelPictureBox.TabIndex = 0;
			this.LevelPictureBox.TabStop = false;
			this.LevelPictureBox.MouseClick += new System.Windows.Forms.MouseEventHandler(this.LevelPictureBox_MouseClick);
			// 
			// StatusBar
			// 
			this.StatusBar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.StatusBarText});
			this.StatusBar.Location = new System.Drawing.Point(0, 227);
			this.StatusBar.Name = "StatusBar";
			this.StatusBar.Size = new System.Drawing.Size(736, 22);
			this.StatusBar.TabIndex = 5;
			// 
			// StatusBarText
			// 
			this.StatusBarText.Margin = new System.Windows.Forms.Padding(0);
			this.StatusBarText.Name = "StatusBarText";
			this.StatusBarText.Size = new System.Drawing.Size(84, 22);
			this.StatusBarText.Text = "MemoryUsage";
			// 
			// groupBox5
			// 
			this.groupBox5.Controls.Add(this.ParachuteNumeric);
			this.groupBox5.Controls.Add(this.label17);
			this.groupBox5.Controls.Add(this.ClimbNumeric);
			this.groupBox5.Controls.Add(this.label18);
			this.groupBox5.Controls.Add(this.StairNumeric);
			this.groupBox5.Controls.Add(this.label19);
			this.groupBox5.Controls.Add(this.DigVertNumeric);
			this.groupBox5.Controls.Add(this.label14);
			this.groupBox5.Controls.Add(this.DigHorizNumeric);
			this.groupBox5.Controls.Add(this.label15);
			this.groupBox5.Controls.Add(this.DigDiagNumeric);
			this.groupBox5.Controls.Add(this.label16);
			this.groupBox5.Controls.Add(this.BombNumeric);
			this.groupBox5.Controls.Add(this.label13);
			this.groupBox5.Controls.Add(this.BlockNumeric);
			this.groupBox5.Controls.Add(this.label12);
			this.groupBox5.Controls.Add(this.WalkNumeric);
			this.groupBox5.Controls.Add(this.label11);
			this.groupBox5.Location = new System.Drawing.Point(395, 83);
			this.groupBox5.Name = "groupBox5";
			this.groupBox5.Size = new System.Drawing.Size(335, 139);
			this.groupBox5.TabIndex = 4;
			this.groupBox5.TabStop = false;
			this.groupBox5.Text = "Lem Counts";
			// 
			// ParachuteNumeric
			// 
			this.ParachuteNumeric.Location = new System.Drawing.Point(287, 91);
			this.ParachuteNumeric.Maximum = new decimal(new int[] {
            9,
            0,
            0,
            0});
			this.ParachuteNumeric.Name = "ParachuteNumeric";
			this.ParachuteNumeric.Size = new System.Drawing.Size(40, 20);
			this.ParachuteNumeric.TabIndex = 29;
			// 
			// label17
			// 
			this.label17.AutoSize = true;
			this.label17.Location = new System.Drawing.Point(225, 93);
			this.label17.Name = "label17";
			this.label17.Size = new System.Drawing.Size(56, 13);
			this.label17.TabIndex = 28;
			this.label17.Text = "Parachute";
			// 
			// ClimbNumeric
			// 
			this.ClimbNumeric.Location = new System.Drawing.Point(173, 91);
			this.ClimbNumeric.Maximum = new decimal(new int[] {
            9,
            0,
            0,
            0});
			this.ClimbNumeric.Name = "ClimbNumeric";
			this.ClimbNumeric.Size = new System.Drawing.Size(40, 20);
			this.ClimbNumeric.TabIndex = 27;
			// 
			// label18
			// 
			this.label18.AutoSize = true;
			this.label18.Location = new System.Drawing.Point(135, 93);
			this.label18.Name = "label18";
			this.label18.Size = new System.Drawing.Size(32, 13);
			this.label18.TabIndex = 26;
			this.label18.Text = "Climb";
			// 
			// StairNumeric
			// 
			this.StairNumeric.Location = new System.Drawing.Point(62, 91);
			this.StairNumeric.Maximum = new decimal(new int[] {
            9,
            0,
            0,
            0});
			this.StairNumeric.Name = "StairNumeric";
			this.StairNumeric.Size = new System.Drawing.Size(40, 20);
			this.StairNumeric.TabIndex = 25;
			// 
			// label19
			// 
			this.label19.AutoSize = true;
			this.label19.Location = new System.Drawing.Point(29, 93);
			this.label19.Name = "label19";
			this.label19.Size = new System.Drawing.Size(28, 13);
			this.label19.TabIndex = 24;
			this.label19.Text = "Stair";
			// 
			// DigVertNumeric
			// 
			this.DigVertNumeric.Location = new System.Drawing.Point(287, 59);
			this.DigVertNumeric.Maximum = new decimal(new int[] {
            9,
            0,
            0,
            0});
			this.DigVertNumeric.Name = "DigVertNumeric";
			this.DigVertNumeric.Size = new System.Drawing.Size(40, 20);
			this.DigVertNumeric.TabIndex = 23;
			this.DigVertNumeric.ValueChanged += new System.EventHandler(this.DigVertNumeric_ValueChanged);
			// 
			// label14
			// 
			this.label14.AutoSize = true;
			this.label14.Location = new System.Drawing.Point(236, 61);
			this.label14.Name = "label14";
			this.label14.Size = new System.Drawing.Size(45, 13);
			this.label14.TabIndex = 22;
			this.label14.Text = "Dig Vert";
			// 
			// DigHorizNumeric
			// 
			this.DigHorizNumeric.Location = new System.Drawing.Point(173, 59);
			this.DigHorizNumeric.Maximum = new decimal(new int[] {
            9,
            0,
            0,
            0});
			this.DigHorizNumeric.Name = "DigHorizNumeric";
			this.DigHorizNumeric.Size = new System.Drawing.Size(40, 20);
			this.DigHorizNumeric.TabIndex = 21;
			this.DigHorizNumeric.ValueChanged += new System.EventHandler(this.DigHorizNumeric_ValueChanged);
			// 
			// label15
			// 
			this.label15.AutoSize = true;
			this.label15.Location = new System.Drawing.Point(120, 61);
			this.label15.Name = "label15";
			this.label15.Size = new System.Drawing.Size(50, 13);
			this.label15.TabIndex = 20;
			this.label15.Text = "Dig Horiz";
			// 
			// DigDiagNumeric
			// 
			this.DigDiagNumeric.Location = new System.Drawing.Point(62, 59);
			this.DigDiagNumeric.Maximum = new decimal(new int[] {
            9,
            0,
            0,
            0});
			this.DigDiagNumeric.Name = "DigDiagNumeric";
			this.DigDiagNumeric.Size = new System.Drawing.Size(40, 20);
			this.DigDiagNumeric.TabIndex = 19;
			this.DigDiagNumeric.ValueChanged += new System.EventHandler(this.DigDiagNumeric_ValueChanged);
			// 
			// label16
			// 
			this.label16.AutoSize = true;
			this.label16.Location = new System.Drawing.Point(9, 61);
			this.label16.Name = "label16";
			this.label16.Size = new System.Drawing.Size(48, 13);
			this.label16.TabIndex = 18;
			this.label16.Text = "Dig Diag";
			// 
			// BombNumeric
			// 
			this.BombNumeric.Location = new System.Drawing.Point(287, 26);
			this.BombNumeric.Maximum = new decimal(new int[] {
            9,
            0,
            0,
            0});
			this.BombNumeric.Name = "BombNumeric";
			this.BombNumeric.Size = new System.Drawing.Size(40, 20);
			this.BombNumeric.TabIndex = 17;
			this.BombNumeric.ValueChanged += new System.EventHandler(this.BombNumeric_ValueChanged);
			// 
			// label13
			// 
			this.label13.AutoSize = true;
			this.label13.Location = new System.Drawing.Point(247, 28);
			this.label13.Name = "label13";
			this.label13.Size = new System.Drawing.Size(34, 13);
			this.label13.TabIndex = 16;
			this.label13.Text = "Bomb";
			// 
			// BlockNumeric
			// 
			this.BlockNumeric.Location = new System.Drawing.Point(173, 26);
			this.BlockNumeric.Maximum = new decimal(new int[] {
            9,
            0,
            0,
            0});
			this.BlockNumeric.Name = "BlockNumeric";
			this.BlockNumeric.Size = new System.Drawing.Size(40, 20);
			this.BlockNumeric.TabIndex = 15;
			// 
			// label12
			// 
			this.label12.AutoSize = true;
			this.label12.Location = new System.Drawing.Point(136, 28);
			this.label12.Name = "label12";
			this.label12.Size = new System.Drawing.Size(34, 13);
			this.label12.TabIndex = 14;
			this.label12.Text = "Block";
			// 
			// WalkNumeric
			// 
			this.WalkNumeric.Location = new System.Drawing.Point(62, 26);
			this.WalkNumeric.Maximum = new decimal(new int[] {
            9,
            0,
            0,
            0});
			this.WalkNumeric.Name = "WalkNumeric";
			this.WalkNumeric.Size = new System.Drawing.Size(40, 20);
			this.WalkNumeric.TabIndex = 13;
			// 
			// label11
			// 
			this.label11.AutoSize = true;
			this.label11.Location = new System.Drawing.Point(24, 28);
			this.label11.Name = "label11";
			this.label11.Size = new System.Drawing.Size(32, 13);
			this.label11.TabIndex = 0;
			this.label11.Text = "Walk";
			// 
			// groupBox4
			// 
			this.groupBox4.Controls.Add(this.label20);
			this.groupBox4.Controls.Add(this.MinDropSpeedNumeric);
			this.groupBox4.Controls.Add(this.label10);
			this.groupBox4.Controls.Add(this.RequiredLemCountNumeric);
			this.groupBox4.Controls.Add(this.label9);
			this.groupBox4.Controls.Add(this.SpawnLemCountNumeric);
			this.groupBox4.Controls.Add(this.label8);
			this.groupBox4.Controls.Add(this.label7);
			this.groupBox4.Controls.Add(this.TimeSecNumeric);
			this.groupBox4.Controls.Add(this.label6);
			this.groupBox4.Controls.Add(this.TimeMinNumeric);
			this.groupBox4.Controls.Add(this.label5);
			this.groupBox4.Controls.Add(this.HomeYNumeric);
			this.groupBox4.Controls.Add(this.HomeXNumeric);
			this.groupBox4.Controls.Add(this.StartYNumeric);
			this.groupBox4.Controls.Add(this.StartXNumeric);
			this.groupBox4.Controls.Add(this.label4);
			this.groupBox4.Controls.Add(this.label3);
			this.groupBox4.Location = new System.Drawing.Point(10, 82);
			this.groupBox4.Name = "groupBox4";
			this.groupBox4.Size = new System.Drawing.Size(379, 141);
			this.groupBox4.TabIndex = 3;
			this.groupBox4.TabStop = false;
			this.groupBox4.Text = "Level Data";
			// 
			// label20
			// 
			this.label20.AutoSize = true;
			this.label20.Location = new System.Drawing.Point(306, 109);
			this.label20.Name = "label20";
			this.label20.Size = new System.Drawing.Size(61, 13);
			this.label20.TabIndex = 17;
			this.label20.Text = "x 27 frames";
			// 
			// MinDropSpeedNumeric
			// 
			this.MinDropSpeedNumeric.Location = new System.Drawing.Point(263, 107);
			this.MinDropSpeedNumeric.Maximum = new decimal(new int[] {
            15,
            0,
            0,
            0});
			this.MinDropSpeedNumeric.Name = "MinDropSpeedNumeric";
			this.MinDropSpeedNumeric.Size = new System.Drawing.Size(40, 20);
			this.MinDropSpeedNumeric.TabIndex = 16;
			// 
			// label10
			// 
			this.label10.AutoSize = true;
			this.label10.Location = new System.Drawing.Point(92, 109);
			this.label10.Name = "label10";
			this.label10.Size = new System.Drawing.Size(173, 13);
			this.label10.TabIndex = 15;
			this.label10.Text = "Min Drop Speed: a lem drops every";
			// 
			// RequiredLemCountNumeric
			// 
			this.RequiredLemCountNumeric.Increment = new decimal(new int[] {
            5,
            0,
            0,
            0});
			this.RequiredLemCountNumeric.Location = new System.Drawing.Point(328, 49);
			this.RequiredLemCountNumeric.Maximum = new decimal(new int[] {
            75,
            0,
            0,
            0});
			this.RequiredLemCountNumeric.Minimum = new decimal(new int[] {
            5,
            0,
            0,
            0});
			this.RequiredLemCountNumeric.Name = "RequiredLemCountNumeric";
			this.RequiredLemCountNumeric.Size = new System.Drawing.Size(40, 20);
			this.RequiredLemCountNumeric.TabIndex = 14;
			this.RequiredLemCountNumeric.Value = new decimal(new int[] {
            5,
            0,
            0,
            0});
			// 
			// label9
			// 
			this.label9.AutoSize = true;
			this.label9.Location = new System.Drawing.Point(205, 51);
			this.label9.Name = "label9";
			this.label9.Size = new System.Drawing.Size(117, 13);
			this.label9.TabIndex = 13;
			this.label9.Text = "Required Lem # to win:";
			// 
			// SpawnLemCountNumeric
			// 
			this.SpawnLemCountNumeric.Increment = new decimal(new int[] {
            5,
            0,
            0,
            0});
			this.SpawnLemCountNumeric.Location = new System.Drawing.Point(328, 23);
			this.SpawnLemCountNumeric.Maximum = new decimal(new int[] {
            75,
            0,
            0,
            0});
			this.SpawnLemCountNumeric.Minimum = new decimal(new int[] {
            5,
            0,
            0,
            0});
			this.SpawnLemCountNumeric.Name = "SpawnLemCountNumeric";
			this.SpawnLemCountNumeric.Size = new System.Drawing.Size(40, 20);
			this.SpawnLemCountNumeric.TabIndex = 12;
			this.SpawnLemCountNumeric.Value = new decimal(new int[] {
            5,
            0,
            0,
            0});
			this.SpawnLemCountNumeric.ValueChanged += new System.EventHandler(this.SpawnLemCountNumeric_ValueChanged);
			// 
			// label8
			// 
			this.label8.AutoSize = true;
			this.label8.Location = new System.Drawing.Point(246, 26);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(76, 13);
			this.label8.TabIndex = 11;
			this.label8.Text = "Spawn Lem #:";
			// 
			// label7
			// 
			this.label7.AutoSize = true;
			this.label7.Location = new System.Drawing.Point(343, 79);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(24, 13);
			this.label7.TabIndex = 10;
			this.label7.Text = "sec";
			// 
			// TimeSecNumeric
			// 
			this.TimeSecNumeric.Increment = new decimal(new int[] {
            10,
            0,
            0,
            0});
			this.TimeSecNumeric.Location = new System.Drawing.Point(294, 77);
			this.TimeSecNumeric.Maximum = new decimal(new int[] {
            60,
            0,
            0,
            0});
			this.TimeSecNumeric.Name = "TimeSecNumeric";
			this.TimeSecNumeric.Size = new System.Drawing.Size(43, 20);
			this.TimeSecNumeric.TabIndex = 9;
			this.TimeSecNumeric.ValueChanged += new System.EventHandler(this.TimeSecNumeric_ValueChanged);
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Location = new System.Drawing.Point(265, 79);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(23, 13);
			this.label6.TabIndex = 8;
			this.label6.Text = "min";
			// 
			// TimeMinNumeric
			// 
			this.TimeMinNumeric.Location = new System.Drawing.Point(219, 77);
			this.TimeMinNumeric.Maximum = new decimal(new int[] {
            42,
            0,
            0,
            0});
			this.TimeMinNumeric.Name = "TimeMinNumeric";
			this.TimeMinNumeric.Size = new System.Drawing.Size(40, 20);
			this.TimeMinNumeric.TabIndex = 7;
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(180, 79);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(33, 13);
			this.label5.TabIndex = 6;
			this.label5.Text = "Time:";
			// 
			// HomeYNumeric
			// 
			this.HomeYNumeric.Location = new System.Drawing.Point(129, 50);
			this.HomeYNumeric.Maximum = new decimal(new int[] {
            62,
            0,
            0,
            0});
			this.HomeYNumeric.Name = "HomeYNumeric";
			this.HomeYNumeric.Size = new System.Drawing.Size(48, 20);
			this.HomeYNumeric.TabIndex = 5;
			this.HomeYNumeric.ValueChanged += new System.EventHandler(this.HomeYNumeric_ValueChanged);
			// 
			// HomeXNumeric
			// 
			this.HomeXNumeric.Location = new System.Drawing.Point(75, 50);
			this.HomeXNumeric.Maximum = new decimal(new int[] {
            240,
            0,
            0,
            0});
			this.HomeXNumeric.Name = "HomeXNumeric";
			this.HomeXNumeric.Size = new System.Drawing.Size(48, 20);
			this.HomeXNumeric.TabIndex = 4;
			this.HomeXNumeric.ValueChanged += new System.EventHandler(this.HomeXNumeric_ValueChanged);
			// 
			// StartYNumeric
			// 
			this.StartYNumeric.Location = new System.Drawing.Point(129, 22);
			this.StartYNumeric.Maximum = new decimal(new int[] {
            55,
            0,
            0,
            0});
			this.StartYNumeric.Name = "StartYNumeric";
			this.StartYNumeric.Size = new System.Drawing.Size(48, 20);
			this.StartYNumeric.TabIndex = 3;
			this.StartYNumeric.ValueChanged += new System.EventHandler(this.StartYNumeric_ValueChanged);
			// 
			// StartXNumeric
			// 
			this.StartXNumeric.Location = new System.Drawing.Point(75, 22);
			this.StartXNumeric.Maximum = new decimal(new int[] {
            239,
            0,
            0,
            0});
			this.StartXNumeric.Name = "StartXNumeric";
			this.StartXNumeric.Size = new System.Drawing.Size(48, 20);
			this.StartXNumeric.TabIndex = 2;
			this.StartXNumeric.ValueChanged += new System.EventHandler(this.StartXNumeric_ValueChanged);
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(10, 52);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(59, 13);
			this.label4.TabIndex = 1;
			this.label4.Text = "Home Pos:";
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(16, 24);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(53, 13);
			this.label3.TabIndex = 0;
			this.label3.Text = "Start Pos:";
			// 
			// groupBox3
			// 
			this.groupBox3.Controls.Add(this.TrimLevelRightCheckBox);
			this.groupBox3.Controls.Add(this.ExportWithWinEOLCheckBox);
			this.groupBox3.Controls.Add(this.TrimLevelLeftCheckBox);
			this.groupBox3.Location = new System.Drawing.Point(207, 8);
			this.groupBox3.Name = "groupBox3";
			this.groupBox3.Size = new System.Drawing.Size(268, 67);
			this.groupBox3.TabIndex = 2;
			this.groupBox3.TabStop = false;
			this.groupBox3.Text = "Export Options";
			// 
			// TrimLevelRightCheckBox
			// 
			this.TrimLevelRightCheckBox.AutoSize = true;
			this.TrimLevelRightCheckBox.Checked = true;
			this.TrimLevelRightCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
			this.TrimLevelRightCheckBox.Location = new System.Drawing.Point(122, 19);
			this.TrimLevelRightCheckBox.Name = "TrimLevelRightCheckBox";
			this.TrimLevelRightCheckBox.Size = new System.Drawing.Size(98, 17);
			this.TrimLevelRightCheckBox.TabIndex = 8;
			this.TrimLevelRightCheckBox.Text = "Trim Level right";
			this.TrimLevelRightCheckBox.UseVisualStyleBackColor = true;
			this.TrimLevelRightCheckBox.CheckedChanged += new System.EventHandler(this.TrimLevelRightCheckBox_CheckedChanged);
			// 
			// ExportWithWinEOLCheckBox
			// 
			this.ExportWithWinEOLCheckBox.AutoSize = true;
			this.ExportWithWinEOLCheckBox.Checked = true;
			this.ExportWithWinEOLCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
			this.ExportWithWinEOLCheckBox.Location = new System.Drawing.Point(11, 40);
			this.ExportWithWinEOLCheckBox.Name = "ExportWithWinEOLCheckBox";
			this.ExportWithWinEOLCheckBox.Size = new System.Drawing.Size(209, 17);
			this.ExportWithWinEOLCheckBox.TabIndex = 7;
			this.ExportWithWinEOLCheckBox.Text = "Export with Windows EOL format (\\r\\n)";
			this.ExportWithWinEOLCheckBox.UseVisualStyleBackColor = true;
			this.ExportWithWinEOLCheckBox.CheckedChanged += new System.EventHandler(this.ExportWithWinEOLCheckBox_CheckedChanged);
			// 
			// TrimLevelLeftCheckBox
			// 
			this.TrimLevelLeftCheckBox.AutoSize = true;
			this.TrimLevelLeftCheckBox.Location = new System.Drawing.Point(11, 19);
			this.TrimLevelLeftCheckBox.Name = "TrimLevelLeftCheckBox";
			this.TrimLevelLeftCheckBox.Size = new System.Drawing.Size(92, 17);
			this.TrimLevelLeftCheckBox.TabIndex = 6;
			this.TrimLevelLeftCheckBox.Text = "Trim Level left";
			this.TrimLevelLeftCheckBox.UseVisualStyleBackColor = true;
			this.TrimLevelLeftCheckBox.CheckedChanged += new System.EventHandler(this.TrimLevelCheckBox_CheckedChanged);
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.HelpTableListView);
			this.groupBox2.Location = new System.Drawing.Point(484, 3);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(247, 72);
			this.groupBox2.TabIndex = 1;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "Quick Reference";
			// 
			// HelpTableListView
			// 
			this.HelpTableListView.AutoArrange = false;
			this.HelpTableListView.BackColor = System.Drawing.SystemColors.InactiveCaption;
			this.HelpTableListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2,
            this.columnHeader3});
			this.HelpTableListView.GridLines = true;
			this.HelpTableListView.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
			this.HelpTableListView.Items.AddRange(new System.Windows.Forms.ListViewItem[] {
            listViewItem2});
			this.HelpTableListView.Location = new System.Drawing.Point(6, 19);
			this.HelpTableListView.MultiSelect = false;
			this.HelpTableListView.Name = "HelpTableListView";
			this.HelpTableListView.Scrollable = false;
			this.HelpTableListView.Size = new System.Drawing.Size(232, 46);
			this.HelpTableListView.TabIndex = 2;
			this.HelpTableListView.UseCompatibleStateImageBehavior = false;
			this.HelpTableListView.View = System.Windows.Forms.View.Details;
			// 
			// columnHeader1
			// 
			this.columnHeader1.Text = "Left Button";
			this.columnHeader1.Width = 72;
			// 
			// columnHeader2
			// 
			this.columnHeader2.Text = "Middle Button";
			this.columnHeader2.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.columnHeader2.Width = 81;
			// 
			// columnHeader3
			// 
			this.columnHeader3.Text = "Right Button";
			this.columnHeader3.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.columnHeader3.Width = 75;
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.MapIdsTextBox);
			this.groupBox1.Controls.Add(this.LocaMapNameTextBox);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Location = new System.Drawing.Point(10, 3);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(188, 72);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Map Names";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(8, 48);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(45, 13);
			this.label2.TabIndex = 3;
			this.label2.Text = "Map Ids";
			// 
			// MapIdsTextBox
			// 
			this.MapIdsTextBox.Location = new System.Drawing.Point(79, 45);
			this.MapIdsTextBox.Name = "MapIdsTextBox";
			this.MapIdsTextBox.Size = new System.Drawing.Size(98, 20);
			this.MapIdsTextBox.TabIndex = 2;
			// 
			// LocaMapNameTextBox
			// 
			this.LocaMapNameTextBox.Location = new System.Drawing.Point(79, 19);
			this.LocaMapNameTextBox.Name = "LocaMapNameTextBox";
			this.LocaMapNameTextBox.Size = new System.Drawing.Size(98, 20);
			this.LocaMapNameTextBox.TabIndex = 1;
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(6, 22);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(55, 13);
			this.label1.TabIndex = 0;
			this.label1.Text = "Loca Map";
			// 
			// MainForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(740, 494);
			this.Controls.Add(this.splitContainer1);
			this.Controls.Add(this.menuStrip1);
			this.MainMenuStrip = this.menuStrip1;
			this.Name = "MainForm";
			this.Text = "Ardulem Editor";
			this.menuStrip1.ResumeLayout(false);
			this.menuStrip1.PerformLayout();
			this.splitContainer1.Panel1.ResumeLayout(false);
			this.splitContainer1.Panel2.ResumeLayout(false);
			this.splitContainer1.Panel2.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
			this.splitContainer1.ResumeLayout(false);
			this.splitContainer2.Panel1.ResumeLayout(false);
			this.splitContainer2.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
			this.splitContainer2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.MapSpritePictureBox)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.LevelPictureBox)).EndInit();
			this.StatusBar.ResumeLayout(false);
			this.StatusBar.PerformLayout();
			this.groupBox5.ResumeLayout(false);
			this.groupBox5.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.ParachuteNumeric)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.ClimbNumeric)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.StairNumeric)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.DigVertNumeric)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.DigHorizNumeric)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.DigDiagNumeric)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.BombNumeric)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.BlockNumeric)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.WalkNumeric)).EndInit();
			this.groupBox4.ResumeLayout(false);
			this.groupBox4.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.MinDropSpeedNumeric)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.RequiredLemCountNumeric)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.SpawnLemCountNumeric)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.TimeSecNumeric)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.TimeMinNumeric)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.HomeYNumeric)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.HomeXNumeric)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.StartYNumeric)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.StartXNumeric)).EndInit();
			this.groupBox3.ResumeLayout(false);
			this.groupBox3.PerformLayout();
			this.groupBox2.ResumeLayout(false);
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem loadLevelFromClipboardToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveLevelToClipboardToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.PictureBox MapSpritePictureBox;
        private System.Windows.Forms.PictureBox LevelPictureBox;
		private System.Windows.Forms.ToolStripMenuItem editToolStripMenuItem;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox MapIdsTextBox;
        private System.Windows.Forms.TextBox LocaMapNameTextBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox TrimLevelLeftCheckBox;
		private System.Windows.Forms.GroupBox groupBox3;
		private System.Windows.Forms.GroupBox groupBox2;
		private System.Windows.Forms.ListView HelpTableListView;
		private System.Windows.Forms.ColumnHeader columnHeader1;
		private System.Windows.Forms.ColumnHeader columnHeader2;
		private System.Windows.Forms.ColumnHeader columnHeader3;
		private System.Windows.Forms.GroupBox groupBox4;
		private System.Windows.Forms.NumericUpDown HomeYNumeric;
		private System.Windows.Forms.NumericUpDown HomeXNumeric;
		private System.Windows.Forms.NumericUpDown StartYNumeric;
		private System.Windows.Forms.NumericUpDown StartXNumeric;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.NumericUpDown TimeSecNumeric;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.NumericUpDown TimeMinNumeric;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.GroupBox groupBox5;
		private System.Windows.Forms.NumericUpDown ParachuteNumeric;
		private System.Windows.Forms.Label label17;
		private System.Windows.Forms.NumericUpDown ClimbNumeric;
		private System.Windows.Forms.Label label18;
		private System.Windows.Forms.NumericUpDown StairNumeric;
		private System.Windows.Forms.Label label19;
		private System.Windows.Forms.NumericUpDown DigVertNumeric;
		private System.Windows.Forms.Label label14;
		private System.Windows.Forms.NumericUpDown DigHorizNumeric;
		private System.Windows.Forms.Label label15;
		private System.Windows.Forms.NumericUpDown DigDiagNumeric;
		private System.Windows.Forms.Label label16;
		private System.Windows.Forms.NumericUpDown BombNumeric;
		private System.Windows.Forms.Label label13;
		private System.Windows.Forms.NumericUpDown BlockNumeric;
		private System.Windows.Forms.Label label12;
		private System.Windows.Forms.NumericUpDown WalkNumeric;
		private System.Windows.Forms.Label label11;
		private System.Windows.Forms.NumericUpDown MinDropSpeedNumeric;
		private System.Windows.Forms.Label label10;
		private System.Windows.Forms.NumericUpDown RequiredLemCountNumeric;
		private System.Windows.Forms.Label label9;
		private System.Windows.Forms.NumericUpDown SpawnLemCountNumeric;
		private System.Windows.Forms.Label label20;
		private System.Windows.Forms.CheckBox ExportWithWinEOLCheckBox;
		private System.Windows.Forms.ToolStripMenuItem saveLevelDataToClipboardToolStripMenuItem;
		private System.Windows.Forms.CheckBox TrimLevelRightCheckBox;
		private System.Windows.Forms.ToolStripMenuItem undoToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem redoToolStripMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
		private System.Windows.Forms.ToolStripMenuItem shiftLevelToRightToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem shiftLevelToLeftToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem shiftLevelUpToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem shiftLevelDownToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem viewToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem drawLevelGridToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem drawStartAndHomeToolStripMenuItem;
		private System.Windows.Forms.StatusStrip StatusBar;
		private System.Windows.Forms.ToolStripStatusLabel StatusBarText;
    }
}

