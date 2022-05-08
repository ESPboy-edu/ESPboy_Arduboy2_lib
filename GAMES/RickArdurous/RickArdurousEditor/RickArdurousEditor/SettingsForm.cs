using System;
using System.Windows.Forms;

namespace RickArdurousEditor
{
	public partial class SettingsForm : Form
	{
		public SettingsForm()
		{
			InitializeComponent();

			// init UI from Settings values
			InitializeUI();
		}

		private void InitializeUI()
		{
			this.textBoxImagePath.Text = Properties.Settings.Default.ImageRelativePath;
			this.textBoxGamePath.Text = Properties.Settings.Default.GameRelativePath;
			this.textBoxDocPath.Text = Properties.Settings.Default.DocRelativePath;
			this.numericUpDownBulletAndDynamite.Value = Properties.Settings.Default.BulletAndDynamiteCount;
		}

		private void buttonImagePath_Click(object sender, EventArgs e)
		{
			BrowseForPath(this.textBoxImagePath);
		}

		private void buttonGamePath_Click(object sender, EventArgs e)
		{
			BrowseForPath(this.textBoxGamePath);
		}

		private void buttonDocPath_Click(object sender, EventArgs e)
		{
			BrowseForPath(this.textBoxDocPath);
			this.textBoxDocPath.Text = this.textBoxDocPath.Text.Replace('\\', '/');
		}

		private void BrowseForPath(TextBox textbox)
		{
			this.folderBrowserDialog.SelectedPath = textbox.Text;
			DialogResult result = this.folderBrowserDialog.ShowDialog();
			if (result == DialogResult.OK)
				textbox.Text = this.folderBrowserDialog.SelectedPath;
		}

		private void buttonOk_Click(object sender, EventArgs e)
		{
			// update the settings
			Properties.Settings.Default.ImageRelativePath = this.textBoxImagePath.Text;
			Properties.Settings.Default.GameRelativePath = this.textBoxGamePath.Text;
			Properties.Settings.Default.DocRelativePath = this.textBoxDocPath.Text;
			Properties.Settings.Default.BulletAndDynamiteCount = (int)this.numericUpDownBulletAndDynamite.Value;
			// then save it
			Properties.Settings.Default.Save();
		}
	}
}
