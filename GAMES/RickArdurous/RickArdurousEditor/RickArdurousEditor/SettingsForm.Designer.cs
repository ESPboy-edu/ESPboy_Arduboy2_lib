namespace RickArdurousEditor
{
	partial class SettingsForm
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SettingsForm));
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.textBoxImagePath = new System.Windows.Forms.TextBox();
			this.textBoxGamePath = new System.Windows.Forms.TextBox();
			this.textBoxDocPath = new System.Windows.Forms.TextBox();
			this.buttonImagePath = new System.Windows.Forms.Button();
			this.buttonGamePath = new System.Windows.Forms.Button();
			this.buttonDocPath = new System.Windows.Forms.Button();
			this.buttonCancel = new System.Windows.Forms.Button();
			this.buttonOk = new System.Windows.Forms.Button();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.folderBrowserDialog = new System.Windows.Forms.FolderBrowserDialog();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.label4 = new System.Windows.Forms.Label();
			this.numericUpDownBulletAndDynamite = new System.Windows.Forms.NumericUpDown();
			this.groupBox1.SuspendLayout();
			this.groupBox2.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numericUpDownBulletAndDynamite)).BeginInit();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(54, 22);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(64, 13);
			this.label1.TabIndex = 0;
			this.label1.Text = "Image Path:";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(55, 48);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(63, 13);
			this.label2.TabIndex = 1;
			this.label2.Text = "Game Path:";
			this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(11, 74);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(107, 13);
			this.label3.TabIndex = 2;
			this.label3.Text = "Documentation Path:";
			this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// textBoxImagePath
			// 
			this.textBoxImagePath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.textBoxImagePath.Location = new System.Drawing.Point(124, 19);
			this.textBoxImagePath.Name = "textBoxImagePath";
			this.textBoxImagePath.Size = new System.Drawing.Size(215, 20);
			this.textBoxImagePath.TabIndex = 3;
			// 
			// textBoxGamePath
			// 
			this.textBoxGamePath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.textBoxGamePath.Location = new System.Drawing.Point(124, 45);
			this.textBoxGamePath.Name = "textBoxGamePath";
			this.textBoxGamePath.Size = new System.Drawing.Size(215, 20);
			this.textBoxGamePath.TabIndex = 4;
			// 
			// textBoxDocPath
			// 
			this.textBoxDocPath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.textBoxDocPath.Location = new System.Drawing.Point(124, 71);
			this.textBoxDocPath.Name = "textBoxDocPath";
			this.textBoxDocPath.Size = new System.Drawing.Size(215, 20);
			this.textBoxDocPath.TabIndex = 5;
			// 
			// buttonImagePath
			// 
			this.buttonImagePath.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.buttonImagePath.Location = new System.Drawing.Point(345, 16);
			this.buttonImagePath.Name = "buttonImagePath";
			this.buttonImagePath.Size = new System.Drawing.Size(60, 23);
			this.buttonImagePath.TabIndex = 6;
			this.buttonImagePath.Text = "Browse";
			this.buttonImagePath.UseVisualStyleBackColor = true;
			this.buttonImagePath.Click += new System.EventHandler(this.buttonImagePath_Click);
			// 
			// buttonGamePath
			// 
			this.buttonGamePath.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.buttonGamePath.Location = new System.Drawing.Point(345, 43);
			this.buttonGamePath.Name = "buttonGamePath";
			this.buttonGamePath.Size = new System.Drawing.Size(60, 23);
			this.buttonGamePath.TabIndex = 7;
			this.buttonGamePath.Text = "Browse";
			this.buttonGamePath.UseVisualStyleBackColor = true;
			this.buttonGamePath.Click += new System.EventHandler(this.buttonGamePath_Click);
			// 
			// buttonDocPath
			// 
			this.buttonDocPath.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.buttonDocPath.Location = new System.Drawing.Point(345, 68);
			this.buttonDocPath.Name = "buttonDocPath";
			this.buttonDocPath.Size = new System.Drawing.Size(60, 23);
			this.buttonDocPath.TabIndex = 8;
			this.buttonDocPath.Text = "Browse";
			this.buttonDocPath.UseVisualStyleBackColor = true;
			this.buttonDocPath.Click += new System.EventHandler(this.buttonDocPath_Click);
			// 
			// buttonCancel
			// 
			this.buttonCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.buttonCancel.Location = new System.Drawing.Point(12, 191);
			this.buttonCancel.Name = "buttonCancel";
			this.buttonCancel.Size = new System.Drawing.Size(75, 23);
			this.buttonCancel.TabIndex = 9;
			this.buttonCancel.Text = "Cancel";
			this.buttonCancel.UseVisualStyleBackColor = true;
			// 
			// buttonOk
			// 
			this.buttonOk.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.buttonOk.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.buttonOk.Location = new System.Drawing.Point(368, 191);
			this.buttonOk.Name = "buttonOk";
			this.buttonOk.Size = new System.Drawing.Size(75, 23);
			this.buttonOk.TabIndex = 10;
			this.buttonOk.Text = "Ok";
			this.buttonOk.UseVisualStyleBackColor = true;
			this.buttonOk.Click += new System.EventHandler(this.buttonOk_Click);
			// 
			// groupBox1
			// 
			this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Controls.Add(this.buttonDocPath);
			this.groupBox1.Controls.Add(this.textBoxImagePath);
			this.groupBox1.Controls.Add(this.buttonGamePath);
			this.groupBox1.Controls.Add(this.textBoxGamePath);
			this.groupBox1.Controls.Add(this.buttonImagePath);
			this.groupBox1.Controls.Add(this.textBoxDocPath);
			this.groupBox1.Location = new System.Drawing.Point(12, 12);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(423, 105);
			this.groupBox1.TabIndex = 11;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Paths";
			// 
			// folderBrowserDialog
			// 
			this.folderBrowserDialog.ShowNewFolderButton = false;
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.numericUpDownBulletAndDynamite);
			this.groupBox2.Controls.Add(this.label4);
			this.groupBox2.Location = new System.Drawing.Point(12, 123);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(422, 56);
			this.groupBox2.TabIndex = 12;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "Optimization";
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(11, 19);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(142, 26);
			this.label4.TabIndex = 0;
			this.label4.Text = "Bullet and Dynamite Count:\r\n(each takes 2 bytes of RAM)";
			this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// numericUpDownBulletAndDynamite
			// 
			this.numericUpDownBulletAndDynamite.Location = new System.Drawing.Point(159, 22);
			this.numericUpDownBulletAndDynamite.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
			this.numericUpDownBulletAndDynamite.Name = "numericUpDownBulletAndDynamite";
			this.numericUpDownBulletAndDynamite.Size = new System.Drawing.Size(49, 20);
			this.numericUpDownBulletAndDynamite.TabIndex = 1;
			// 
			// SettingsForm
			// 
			this.AcceptButton = this.buttonOk;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.CancelButton = this.buttonCancel;
			this.ClientSize = new System.Drawing.Size(455, 226);
			this.Controls.Add(this.groupBox2);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.buttonOk);
			this.Controls.Add(this.buttonCancel);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MinimumSize = new System.Drawing.Size(300, 200);
			this.Name = "SettingsForm";
			this.Text = "Rick Ardurous Editor Settings";
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.groupBox2.ResumeLayout(false);
			this.groupBox2.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.numericUpDownBulletAndDynamite)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.TextBox textBoxImagePath;
		private System.Windows.Forms.TextBox textBoxGamePath;
		private System.Windows.Forms.TextBox textBoxDocPath;
		private System.Windows.Forms.Button buttonImagePath;
		private System.Windows.Forms.Button buttonGamePath;
		private System.Windows.Forms.Button buttonDocPath;
		private System.Windows.Forms.Button buttonCancel;
		private System.Windows.Forms.Button buttonOk;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog;
		private System.Windows.Forms.GroupBox groupBox2;
		private System.Windows.Forms.NumericUpDown numericUpDownBulletAndDynamite;
		private System.Windows.Forms.Label label4;
	}
}