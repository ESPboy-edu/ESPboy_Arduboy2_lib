using System.Drawing;

namespace RickArdurousEditor.Action
{
	public class ActionMoveItem : Action
	{
		Items.Item mItem = null;
		Point mOldLocation;
		Point mNewLocation;

		public ActionMoveItem(Items.Item item, Point oldLocation, Point newLocation)
		{
			mItem = item;
			mOldLocation = oldLocation;
			mNewLocation = newLocation;
		}

		public override string GetName()
		{
			return Properties.Resources.ActionMoveItem.Replace("¤", mItem.ItemType.ToString());
		}

		public override void Undo()
		{
			mItem.Move(mOldLocation);
			MainForm.Instance.RedrawLevel();
		}

		public override void Redo()
		{
			mItem.Move(mNewLocation);
			MainForm.Instance.RedrawLevel();
		}

	}
}
