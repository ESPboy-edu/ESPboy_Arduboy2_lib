using System.Drawing;

namespace RickArdurousEditor.Action
{
	public class ActionSetMapSpriteId : Action
	{
		Map mMap = null;
		Point mLocation;
		byte mOldId;
		byte mNewId;

		public ActionSetMapSpriteId(Map map, Point coord, byte id)
		{
			mMap = map;
			mLocation = coord;
			mOldId = map.GetSpriteIdAt(coord);
			mNewId = id;
		}

		public override string GetName()
		{
			return Properties.Resources.ActionSetMapSpriteId;
		}

		public override void Undo()
		{
			mMap.SetSpriteId(mLocation, mOldId);
			MainForm.Instance.RedrawLevel();
		}

		public override void Redo()
		{
			mMap.SetSpriteId(mLocation, mNewId);
			MainForm.Instance.RedrawLevel();
		}
	}
}
