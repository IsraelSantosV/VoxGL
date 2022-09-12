#include "VoxPch.h"
#include "Entity.h"

namespace Vox
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene) {}
}