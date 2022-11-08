#include "VoxPch.h"
#include "Vox/Physics/2D/Physics2D.h"
#include "Vox/Scripting/ScriptEngine.h"

namespace Vox 
{
	std::vector<Raycast2DResult> Physics2D::Raycast(Ref<Scene> scene, const glm::vec2& origin, const glm::vec2& destination)
	{
		class RayCastCallback : public b2RayCastCallback
		{
		public:
			RayCastCallback(std::vector<Raycast2DResult>& results, glm::vec2 p0, glm::vec2 p1)
				: m_Results(results), m_Origin(p0), m_Destination(p1) { }

			virtual float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
			{
				UUID entityID = *(UUID*)&fixture->GetBody()->GetUserData();
				Entity& entity = ScriptEngine::GetSceneContext()->GetEntityWithId(entityID);
				float distance = glm::distance(m_Origin, m_Destination) * fraction;
				m_Results.emplace_back(entity, glm::vec2(point.x, point.y), glm::vec2(normal.x, normal.y), distance);
				return 1.0f;
			}
		private:
			std::vector<Raycast2DResult>& m_Results;
			glm::vec2 m_Origin, m_Destination;
		};

		auto& b2dWorld = scene->m_Registry.get<Box2DWorldComponent>(scene->m_SceneEntity).World;

		std::vector<Raycast2DResult> results;
		RayCastCallback callback(results, origin, destination);
		b2dWorld->RayCast(&callback, { origin.x, origin.y }, { destination.x, destination.y });
		return results;
	}
}
