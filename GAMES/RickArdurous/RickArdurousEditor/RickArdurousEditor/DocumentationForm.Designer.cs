namespace RickArdurousEditor
{
	partial class DocumentationForm
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(DocumentationForm));
			this.WebBrowserDocumentation = new System.Windows.Forms.WebBrowser();
			this.SuspendLayout();
			// 
			// WebBrowserDocumentation
			// 
			this.WebBrowserDocumentation.Dock = System.Windows.Forms.DockStyle.Fill;
			this.WebBrowserDocumentation.Location = new System.Drawing.Point(0, 0);
			this.WebBrowserDocumentation.MinimumSize = new System.Drawing.Size(20, 20);
			this.WebBrowserDocumentation.Name = "WebBrowserDocumentation";
			this.WebBrowserDocumentation.Size = new System.Drawing.Size(568, 450);
			this.WebBrowserDocumentation.TabIndex = 0;
			this.WebBrowserDocumentation.Url = new System.Uri("", System.UriKind.Relative);
			// 
			// DocumentationForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(568, 450);
			this.Controls.Add(this.WebBrowserDocumentation);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "DocumentationForm";
			this.Text = "Documentation";
			this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.DocumentationForm_FormClosed);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.WebBrowser WebBrowserDocumentation;
	}
}