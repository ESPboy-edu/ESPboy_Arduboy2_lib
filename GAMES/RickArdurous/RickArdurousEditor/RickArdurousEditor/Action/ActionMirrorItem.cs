namespace RickArdurousEditor.Action
{
	public class ActionMirrorItem : Action
	{
		Items.Item mItem = null;

		public ActionMirrorItem(Items.Item item)
		{
			mItem = item;
		}

		public override string GetName()
		{
			return Properties.Resources.ActionMirrorItem.Replace("¤", mItem.ItemType.ToString());
		}

		public override void Undo()
		{
			mItem.Mirror();
			MainForm.Instance.RedrawLevel();
		}

		public override void Redo()
		{
			mItem.Mirror();
			MainForm.Instance.RedrawLevel();
		}
	}
}
