#include "VoxPch.h"
#include "Vox/Physics/2D/ContactListener2D.h"
#include "Vox/Scene/Entity.h"

namespace Vox 
{
	void ContactListener2D::BeginContact(b2Contact* contact)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		if (scene == nullptr) return;

		if (!scene->IsRunning())
		{
			return;
		}

		UUID aID = (UUID)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
		UUID bID = (UUID)contact->GetFixtureB()->GetBody()->GetUserData().pointer;

		Entity a = scene->GetEntityWithId(aID);
		Entity b = scene->GetEntityWithId(bID);

		auto callOnCollision2DBegin = [](Entity entity, Entity other)
		{
			if (!entity.HasComponent<ScriptComponent>())
			{
				return;
			}

			const auto& sc = entity.GetComponent<ScriptComponent>();
			if (!ScriptEngine::EntityClassExists(sc.ClassName))
			{
				return;
			}

			//ScriptEngine::OnCollisionBegin(entity, other);
		};

		callOnCollision2DBegin(a, b);
		callOnCollision2DBegin(b, a);
	}

	void ContactListener2D::EndContact(b2Contact* contact)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		if (scene == nullptr) return;

		if (!scene->IsRunning())
		{
			return;
		}

		UUID aID = (UUID)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
		UUID bID = (UUID)contact->GetFixtureB()->GetBody()->GetUserData().pointer;

		Entity a = scene->GetEntityWithId(aID);
		Entity b = scene->GetEntityWithId(bID);

		auto callOnCollision2DEnd = [](Entity entity, Entity other)
		{
			if (!entity.HasComponent<ScriptComponent>())
			{
				return;
			}

			const auto& sc = entity.GetComponent<ScriptComponent>();
			if (!ScriptEngine::EntityClassExists(sc.ClassName))
			{
				return;
			}

			//ScriptEngine::OnCollisionEnd(entity, other);
		};

		callOnCollision2DEnd(a, b);
		callOnCollision2DEnd(b, a);
	}

}

