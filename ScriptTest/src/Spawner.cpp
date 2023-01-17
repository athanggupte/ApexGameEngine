#include "pch.h"
#include "Apex.h"
#include "Reflect.h"

using namespace Apex;

CLASS() SCRIPT_TEST_API Spawner : public EntityScript
{
public:
	PROPERTY() float spawnDelay = 0.5f; // seconds
	PROPERTY() int maxNumber = 10000;
	PROPERTY() Entity baseEntity;

protected:
	void OnCreate() override
	{
		m_TimeUntilNextUpdate = spawnDelay;

		baseEntity = parentScene()->CreateEntity(TEXT("Base-Entity"));
		baseEntity.Transform() = self().Transform();

		auto& meshComp = baseEntity.AddComponent<MeshRendererComponent>();
		meshComp.mesh = GetResourceManager().Get<Mesh>(RESNAME("default-cube"));
		meshComp.material = GetResourceManager().Get<Material>(RESNAME("default-material"));

		baseEntity.AddComponent<RigidBodyComponent>().type = RigidBodyType::Dynamic;
		(void)baseEntity.AddComponent<BoxCollider>();
	}

	void OnUpdate(Timestep ts) override
	{
		if (m_Count < maxNumber) {
			m_TimeUntilNextUpdate -= ts;
			if (m_TimeUntilNextUpdate < 0.f) {
				auto new_entity = parentScene()->CreateEntity(TEXT("spawned-entity"));
				new_entity.Transform() = self().Transform();
				new_entity.AddComponent<MeshRendererComponent>() = baseEntity.GetComponent<MeshRendererComponent>();
				if (baseEntity.HasComponent<RigidBodyComponent>())
					new_entity.AddComponent<RigidBodyComponent>() = baseEntity.GetComponent<RigidBodyComponent>();
				if (baseEntity.HasComponent<BoxCollider>())
					new_entity.AddComponent<BoxCollider>() = baseEntity.GetComponent<BoxCollider>();
				if (baseEntity.HasComponent<SphereCollider>())
					new_entity.AddComponent<SphereCollider>() = baseEntity.GetComponent<SphereCollider>();

				m_TimeUntilNextUpdate = spawnDelay;
				++m_Count;
			}
		}
	}

private:
	Timestep m_TimeUntilNextUpdate;
	int m_Count = 0;
};


APEX_REGISTER_SCRIPT_FACTORY(Spawner)
