using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ArudlemEditor.UndoRedo
{
	class ShiftHorizontally : Action
	{
		private int[] m_OriginalDataColumn = new int[Level.LEVEL_HEIGHT];
        private bool[] m_OriginalMirrorColumn = new bool[Level.LEVEL_HEIGHT];
		private int m_Shift = 0;
		private int m_DeletedColumnIndex = 0;

		public ShiftHorizontally(Level level, int shift)
		{
			m_Level = level;
			m_Shift = shift;

			// determines which column will disapear depending on the direction of the shift
			if (shift > 0)
				m_DeletedColumnIndex = Level.LEVEL_WIDTH - 1;
			else
				m_DeletedColumnIndex = 0;

			// copy the row that will disapear with the shift
			for (int j = 0; j < Level.LEVEL_HEIGHT; j++)
			{
				m_OriginalDataColumn[j] = level.GetSprite(m_DeletedColumnIndex, j);
				m_OriginalMirrorColumn[j] = level.IsSpriteMirrored(m_DeletedColumnIndex, j);
			}
		}

		public override void Undo()
		{
			// shift in opposite direction
			m_Level.ShiftHorizontally(-m_Shift);

			// then recopy the full row that was deleted
			for (int j = 0; j < Level.LEVEL_HEIGHT; j++)
			{
				m_Level.SetSprite(m_DeletedColumnIndex, j, m_OriginalDataColumn[j]);
				m_Level.SetMirrorStatus(m_DeletedColumnIndex, j, m_OriginalMirrorColumn[j]);
			}
		}

		public override void Redo()
		{
			m_Level.ShiftHorizontally(m_Shift);
		}
	}
}
