
namespace RickArdurousEditor.Action
{
	public class ActionDeleteItem : Action
	{
		Map mMap = null;
		Items.Item mItem = null;

		public ActionDeleteItem(Map map, Items.Item item)
		{
			mMap = map;
			mItem = item;
		}

		public override string GetName()
		{
			return Properties.Resources.ActionDeleteItem.Replace("¤", mItem.ItemType.ToString());
		}

		public override void Undo()
		{
			mMap.AddItem(mItem);
			MainForm.Instance.CurrentSelectedItem = mItem;
		}

		public override void Redo()
		{
			mMap.RemoveItem(mItem);
			MainForm.Instance.CurrentSelectedItem = null;
		}
	}
}
