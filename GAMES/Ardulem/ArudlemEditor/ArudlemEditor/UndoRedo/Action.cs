using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ArudlemEditor.UndoRedo
{
	abstract class Action
	{
		// the level on which the action may be executed
		protected Level m_Level = null;

		public abstract void Undo();
		public abstract void Redo();
	}
}
