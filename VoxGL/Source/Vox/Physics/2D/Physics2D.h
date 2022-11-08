#pragma once

#include "Vox/Scene/Scene.h"
#include "Vox/Scene/Entity.h"
#include "Vox/Physics/2D/ContactListener2D.h"

namespace Vox 
{
	struct Raycast2DResult
	{
		Entity HitEntity;
		glm::vec2 Point;
		glm::vec2 Normal;
		float Distance;

		Raycast2DResult(Entity entity, glm::vec2 point, glm::vec2 normal, float distance)
			: HitEntity(entity), Point(point), Normal(normal), Distance(distance) {}
	};

	struct Box2DWorldComponent
	{
		Scope<b2World> World;
		ContactListener2D ContactListener;
	};

	class Physics2D
	{
	public:
		static std::vector<Raycast2DResult> Raycast(Ref<Scene> scene, const glm::vec2& point0, const glm::vec2& point1);
	};

}
