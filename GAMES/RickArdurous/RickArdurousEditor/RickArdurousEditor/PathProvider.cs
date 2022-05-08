using System.IO;
using System.Windows.Forms;

namespace RickArdurousEditor
{
	class PathProvider
	{
		private static bool IsFullPath(string path)
		{
			try
			{
				return Path.GetFullPath(path) == path;
			}
			catch
			{
				return false;
			}
		}

		private static string MakePathAbsolute(string randomPath, char pathSeparator)
		{
			// add a folder separator at the end of the path
			if (randomPath[randomPath.Length - 1] != pathSeparator)
				randomPath += pathSeparator;

			// check if the path is local or global
			if (IsFullPath(randomPath))
				return randomPath;
			return Application.StartupPath + randomPath;
		}

		public static string GetGamePath()
		{
			return MakePathAbsolute(Properties.Settings.Default.GameRelativePath, Path.DirectorySeparatorChar);
		}
		public static string GetImagePath()
		{
			return MakePathAbsolute(Properties.Settings.Default.ImageRelativePath, Path.DirectorySeparatorChar);
		}

		public static string GetDocPath()
		{
			return MakePathAbsolute(Properties.Settings.Default.DocRelativePath, '/');
		}

		public static string GetConstVariableFileName(string mapDataFileName)
		{
			FileInfo fileInfo = new FileInfo(mapDataFileName);
			return mapDataFileName.Remove(mapDataFileName.IndexOf(fileInfo.Extension)) + "ConstVariables.h";
		}
	}
}
