using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ArudlemEditor.UndoRedo
{
	class ShiftVertically : Action
	{
		private int[] m_OriginalDataRow = new int[Level.LEVEL_WIDTH];
        private bool[] m_OriginalMirrorRow = new bool[Level.LEVEL_WIDTH];
		private int m_Shift = 0;
		private int m_DeletedRowIndex = 0;

		public ShiftVertically(Level level, int shift)
		{
			m_Level = level;
			m_Shift = shift;

			// determines which column will disapear depending on the direction of the shift
			if (shift > 0)
				m_DeletedRowIndex = Level.LEVEL_HEIGHT - 1;
			else
				m_DeletedRowIndex = 0;

			// copy the row that will disapear with the shift
			for (int i = 0; i < Level.LEVEL_WIDTH; i++)
			{
				m_OriginalDataRow[i] = level.GetSprite(i, m_DeletedRowIndex);
				m_OriginalMirrorRow[i] = level.IsSpriteMirrored(i, m_DeletedRowIndex);
			}
		}

		public override void Undo()
		{
			// shift in opposite direction
			m_Level.ShiftVertically(-m_Shift);

			// then recopy the full row that was deleted
			for (int i = 0; i < Level.LEVEL_WIDTH; i++)
			{
				m_Level.SetSprite(i, m_DeletedRowIndex, m_OriginalDataRow[i]);
				m_Level.SetMirrorStatus(i, m_DeletedRowIndex, m_OriginalMirrorRow[i]);
			}
		}

		public override void Redo()
		{
			m_Level.ShiftVertically(m_Shift);
		}
	}
}
