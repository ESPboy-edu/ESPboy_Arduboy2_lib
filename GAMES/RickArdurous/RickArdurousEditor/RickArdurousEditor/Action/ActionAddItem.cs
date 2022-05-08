using System.Drawing;

namespace RickArdurousEditor.Action
{
	public class ActionAddItem : Action
	{
		Map mMap = null;
		Items.Item mItem = null;

		public ActionAddItem(Map map, Items.Item.Type itemType, Point levelCoord)
		{
			mMap = map;
			mItem = mMap.CreateItem(itemType, false, false, levelCoord);
		}

		public override string GetName()
		{
			return Properties.Resources.ActionAddItem.Replace("¤", mItem.ItemType.ToString());
		}

		public override void Undo()
		{
			mMap.RemoveItem(mItem);
			MainForm.Instance.CurrentSelectedItem = null;
		}

		public override void Redo()
		{
			mMap.AddItem(mItem);
			MainForm.Instance.CurrentSelectedItem = mItem;
		}
	}
}
