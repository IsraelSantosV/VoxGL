#pragma once

#include "Vox/Scene/Entity.h"

namespace Vox
{
	class SelectionManager
	{
	public:
		static void Select(UUID selectionID);
		static bool IsSelected(UUID selectionID);
		static bool IsEntityOrParentSelected(const Entity entity);
		static void Deselect(UUID selectionID);
		static void DeselectAll();
		static UUID GetSelection(size_t index);

		static size_t GetSelectionCount();
		inline static const std::vector<UUID>& GetSelections() { return m_Selections; }
	private:
		inline static std::vector<UUID> m_Selections;
	};
}
