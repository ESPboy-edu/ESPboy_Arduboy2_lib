namespace RickArdurousEditor.Action
{
	public class ActionSetItemSpecial : Action
	{
		Items.Item mItem = null;

		public ActionSetItemSpecial(Items.Item item)
		{
			mItem = item;
		}

		public override string GetName()
		{
			return Properties.Resources.ActionSetItemSpecial.Replace("¤", mItem.ItemType.ToString());
		}

		public override void Undo()
		{
			SetItemSpecial();
			MainForm.Instance.RedrawLevel();
		}

		public override void Redo()
		{
			SetItemSpecial();
			MainForm.Instance.RedrawLevel();
		}

		private void SetItemSpecial()
		{
			if (mItem.ItemType == Items.Item.Type.RICK)
			{
				if (mItem.RickRespawnType == Items.Item.RespawnType.NORMAL)
					mItem.RickRespawnType = Items.Item.RespawnType.START;
				else
					mItem.RickRespawnType = Items.Item.RespawnType.NORMAL;
			}
			else if (mItem.ItemType == Items.Item.Type.BOULDER)
			{
				mItem.IsSpecial = !mItem.IsSpecial;
			}
		}
		static public bool CanThisItemBeSetSpecial(Items.Item item)
		{
			return (item.ItemType == Items.Item.Type.RICK) || (item.ItemType == Items.Item.Type.BOULDER);
		}
	}
}
