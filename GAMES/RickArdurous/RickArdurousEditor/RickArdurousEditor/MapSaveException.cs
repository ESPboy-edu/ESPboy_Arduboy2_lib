using System;

namespace RickArdurousEditor
{
	class MapSaveException : System.Exception
	{
		public MapSaveException(string message, MainForm.LogLevel level)
		{
			MainForm.LogMessage(message, level);
		}
		public MapSaveException(string message, MainForm.LogLevel level, string param1)
		{
			MainForm.LogMessage(message.Replace("¤", param1), level);
		}
		public MapSaveException(string message, MainForm.LogLevel level, string param1, string param2)
		{
			MainForm.LogMessage(message.Replace("¤¤", param2).Replace("¤", param1), level);
		}
	}
}
