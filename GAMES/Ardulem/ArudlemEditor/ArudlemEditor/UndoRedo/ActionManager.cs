using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ArudlemEditor.UndoRedo
{
	class ActionManager
	{
		private List<Action> m_UndoStack = new List<Action>();
		private int m_CurrentUndoStackIndex = -1;

		public void Do(Action newAction)
		{
			// clear the end of the undo stack if we add a new action
			int lastIndex = m_UndoStack.Count - 1;
			if (m_CurrentUndoStackIndex < lastIndex)
				m_UndoStack.RemoveRange(m_CurrentUndoStackIndex + 1, lastIndex - m_CurrentUndoStackIndex);

			// then add the new action and increase the index
			m_UndoStack.Add(newAction);
			m_CurrentUndoStackIndex++;

			// and do the action
			newAction.Redo();
		}

		public bool Undo()
		{
			// check if we have something to undo
			if (m_CurrentUndoStackIndex >= 0)
			{
				// undo the current action
				m_UndoStack[m_CurrentUndoStackIndex].Undo();
				// and decrease the index
				m_CurrentUndoStackIndex--;
				// action undone, return ok
				return true;
			}
			return false;
		}

		public bool Redo()
		{
			// check if we have an action to redo
			if (m_CurrentUndoStackIndex < m_UndoStack.Count - 1)
			{
				// increase the index
				m_CurrentUndoStackIndex++;
				// and redo the action
				m_UndoStack[m_CurrentUndoStackIndex].Redo();
				// action redone, return ok
				return true;
			}
			return false;
		}
	}
}
