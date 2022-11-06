#include "VoxPch.h"
#include "SelectionManager.h"
#include "Vox/Core/Application.h"
#include "Vox/Events/SceneEvents.h"

namespace Vox 
{
	void SelectionManager::Select(UUID selectionID)
	{
		if (std::find(m_Selections.begin(), m_Selections.end(), selectionID) != m_Selections.end())
		{
			return;
		}

		m_Selections.push_back(selectionID);
		Application::Get().DispatchEvent<SelectionChangedEvent>(selectionID, true);
	}

	bool SelectionManager::IsSelected(UUID selectionID)
	{
		if (std::find(m_Selections.begin(), m_Selections.end(), selectionID) != m_Selections.end())
		{
			return true;
		}

		return false;
	}


	bool SelectionManager::IsEntityOrParentSelected(const Entity entity)
	{
		Entity e = entity;
		while (e)
		{
			if (IsSelected(e.GetId()))
			{
				return true;
			}
			e = e.GetParent();
		}
		return false;
	}

	void SelectionManager::Deselect(UUID selectionID)
	{
		auto it = std::find(m_Selections.begin(), m_Selections.end(), selectionID);
		if (it == m_Selections.end())
		{
			return;
		}
			
		Application::Get().DispatchEvent<SelectionChangedEvent>(selectionID, false);
		m_Selections.erase(it);
	}

	void SelectionManager::DeselectAll()
	{
		for (const auto& selectionID : m_Selections)
		{
			Application::Get().DispatchEvent<SelectionChangedEvent>(selectionID, false);
		}
		m_Selections.clear();
	}

	UUID SelectionManager::GetSelection(size_t index)
	{
		if (index < 0 || index >= m_Selections.size())
		{
			return 0;
		}

		return m_Selections[index];
	}

	size_t SelectionManager::GetSelectionCount()
	{
		return m_Selections.size();
	}

}
