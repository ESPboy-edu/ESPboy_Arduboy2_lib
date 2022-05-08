
namespace RickArdurousEditor.Action
{
	public abstract class Action
	{
		public abstract string GetName();
		public abstract void Undo();
		public abstract void Redo();
	}
}
