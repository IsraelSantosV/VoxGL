#pragma once

#include "Vox/Events/Event.h"
#include "Vox/Core/KeyCodes.h"

#include "Vox/Scene/Scene.h"
#include "Vox/Scene/Entity.h"

#include <sstream>

namespace Vox 
{
	class SceneEvent : public Event
	{
	public:
		const Ref<Scene>& GetScene() const { return m_Scene; }
		Ref<Scene> GetScene() { return m_Scene; }

		EVENT_CLASS_CATEGORY(EventCategoryApplication | EventCategoryScene)
	protected:
		SceneEvent(const Ref<Scene>& scene)
			: m_Scene(scene) {}

		Ref<Scene> m_Scene;
	};

	class ScenePreStartEvent : public SceneEvent
	{
	public:
		ScenePreStartEvent(const Ref<Scene>& scene)
			: SceneEvent(scene) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "ScenePreStartEvent: " << m_Scene->GetName();
			return ss.str();
		}

		EVENT_CLASS_TYPE(ScenePreStart)
	};

	class ScenePostStartEvent : public SceneEvent
	{
	public:
		ScenePostStartEvent(const Ref<Scene>& scene)
			: SceneEvent(scene) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "ScenePostStartEvent: " << m_Scene->GetName();
			return ss.str();
		}

		EVENT_CLASS_TYPE(ScenePostStart)
	};

	class ScenePreStopEvent : public SceneEvent
	{
	public:
		ScenePreStopEvent(const Ref<Scene>& scene)
			: SceneEvent(scene) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "ScenePreStopEvent: " << m_Scene->GetName();
			return ss.str();
		}

		EVENT_CLASS_TYPE(ScenePreStop)
	};

	class ScenePostStopEvent : public SceneEvent
	{
	public:
		ScenePostStopEvent(const Ref<Scene>& scene)
			: SceneEvent(scene) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "ScenePostStopEvent: " << m_Scene->GetName();
			return ss.str();
		}

		EVENT_CLASS_TYPE(ScenePostStop)
	};

	// TODO(Peter): Probably move this somewhere else...
	class SelectionChangedEvent : public Event
	{
	public:
		SelectionChangedEvent(UUID selectionID, bool selected) : m_SelectionID(selectionID), m_Selected(selected)
		{
		}

		UUID GetSelectionID() const { return m_SelectionID; }
		bool IsSelected() const { return m_Selected; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "EntitySelectionChangedEvent: Selection(" << m_SelectionID << "), " << m_Selected;
			return ss.str();
		}

		EVENT_CLASS_CATEGORY(EventCategoryScene)
		EVENT_CLASS_TYPE(SelectionChanged)
	private:
		UUID m_SelectionID;
		bool m_Selected;
	};
}
