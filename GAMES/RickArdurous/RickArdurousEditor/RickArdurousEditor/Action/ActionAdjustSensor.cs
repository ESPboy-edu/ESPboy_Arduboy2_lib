namespace RickArdurousEditor.Action
{
	public class ActionAdjustSensor : Action
	{
		Items.Item mItem = null;
		int mOldDistance;
		int mNewDistance;

		public ActionAdjustSensor(Items.Item item, int oldDistance, int newDistance)
		{
			mItem = item;
			mOldDistance = oldDistance;
			mNewDistance = newDistance;
		}

		public override string GetName()
		{
			return Properties.Resources.ActionAdjustSensor;
		}

		public override void Undo()
		{
			mItem.SensorDistance = mOldDistance;
			MainForm.Instance.RedrawLevel();
		}

		public override void Redo()
		{
			mItem.SensorDistance = mNewDistance;
			MainForm.Instance.RedrawLevel();
		}
	}
}
