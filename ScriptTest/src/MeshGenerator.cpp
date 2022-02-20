#include "pch.h"

#include "Apex/Core/ECS/EntityScript.h"
#include "Apex/Core/ECS/Components.h"
#include "Apex/Utils/Random.h"
#include "glm/gtc/noise.hpp"

using namespace Apex;

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
};

class SCRIPT_TEST_API MeshGenerator : public Apex::EntityScript
{
public:
	int xSize = 50, zSize = 50;
	float scale = 10.f;
	float height = 15.f;

protected:
	void OnCreate() override
	{
		auto& meshComp = GetComponent<MeshRendererComponent>();
		meshComp.mesh = GetResourceManager().Insert(RESNAME("generated-mesh-" XSTR(__COUNTER__)), GenerateMesh());
	}

	void OnUpdate(Timestep ts) override
	{
		/*auto& meshComp = GetComponent<MeshRendererComponent>();
		auto& vbo = meshComp.mesh->GetVAO()->GetVertexBuffers()[0];
		Vertex* buffer = static_cast<Vertex*>(vbo->MapBuffer(true, true));

		size_t count = vbo->GetSize() / sizeof(Vertex);

		for (int i = 0; i < count; i++) {
			auto _x = (float)buffer[i].position.x / (float)(xSize + 1);
			auto _z = (float)buffer[i].position.z / (float)(zSize + 1);
			buffer[i].position.y += glm::cos(Timer::GetElapsedTime()) * glm::perlin(glm::vec2{ _x, _z });
		}

		vbo->UnmapBuffer();*/
	}

	Ref<Mesh> GenerateMesh()
	{
		std::vector<glm::vec3> vertices((xSize + 1) * (zSize + 1));
		std::vector<glm::vec2> uvs((xSize + 1) * (zSize + 1));

		for (int vert = 0, z = 0; z <= zSize; ++z) {
			for (int x = 0; x <= xSize; ++x) {
				auto _x = (float)x / (float)(xSize + 1);
				auto _z = (float)z / (float)(zSize + 1);
				auto y = glm::perlin(glm::vec2{ _x, _z }, { 1.f/scale, 1.f/scale });
				vertices[vert] = { static_cast<float>(x), y * height, static_cast<float>(-z) };
				uvs[vert] = { static_cast<float>(x), static_cast<float>(z) };
				++vert;
			}
		}

		std::vector<uint32_t> indices(xSize * zSize * 6);

		int tris = 0, vert = 0;
		for (int z = 0; z < zSize; ++z) {
			for (int x = 0; x < xSize; ++x) {
				indices[tris + 0] = vert + 0;
				indices[tris + 1] = vert + 1;
				indices[tris + 2] = vert + xSize + 2;
				indices[tris + 3] = vert + xSize + 2;
				indices[tris + 4] = vert + xSize + 1;
				indices[tris + 5] = vert + 0;

				vert++;
				tris += 6;
			}
			vert++;
		}

		MeshBuilder builder;
		builder.SetPositions(vertices);
		builder.SetIndices(indices);
		builder.SetUVs(uvs);

		builder.RegenerateSoftNormals();
		builder.RegenerateTangents();
		
		return builder.BuildMesh();
	}
};

APEX_REGISTER_SCRIPT_FACTORY(MeshGenerator);
