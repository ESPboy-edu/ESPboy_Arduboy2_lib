using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ArudlemEditor.UndoRedo
{
	class MirrorCell : Action
	{
		private int m_X = 0;
		private int m_Y = 0;

		public MirrorCell(Level level, int x, int y)
		{
			m_Level = level;
			m_X = x;
			m_Y = y;
		}

		public override void Undo()
		{
			m_Level.InverseMirrorStatus(m_X, m_Y);
		}

		public override void Redo()
		{
			m_Level.InverseMirrorStatus(m_X, m_Y);
		}
	}
}
