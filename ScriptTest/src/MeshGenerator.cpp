#include "pch.h"

#include "Apex/Core/ECS/EntityScript.h"
#include "Apex/Core/ECS/Components.h"
#include "Util/Random.h"
#include "glm/gtc/noise.hpp"

#include "Reflect.h"
#include "Reflection.h"
#include "glm/gtc/type_ptr.hpp"

using namespace Apex;

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
};

CLASS() SCRIPT_TEST_API MeshGenerator : public Apex::EntityScript
{
public:
	PROPERTY() int xSize = 50;
	PROPERTY() int zSize = 50;
	PROPERTY() float scale = 10.f;
	PROPERTY() float height = 15.f;

	Resource<Texture> m_PerlinNoiseTexture;

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
		const size_t total_vertices = (static_cast<size_t>(xSize) + 1) * (static_cast<size_t>(zSize) + 1);
		std::vector<glm::vec3> vertices(total_vertices);
		std::vector<glm::vec2> uvs(total_vertices);

		std::vector<float> heightTexData(total_vertices);

		for (int vert = 0, z = 0; z <= zSize; ++z) {
			for (int x = 0; x <= xSize; ++x) {
				auto _x = (float)x / (float)(xSize + 1) * scale;
				auto _z = (float)z / (float)(zSize + 1) * scale;
				auto y = glm::perlin(glm::vec2{ _x, _z } , { scale, scale });
				heightTexData[vert] = (y + 1.f) / 2.f;
				vertices[vert] = { static_cast<float>(x), y * height, static_cast<float>(-z) };
				uvs[vert] = { static_cast<float>(x), static_cast<float>(z) };
				++vert;
			}
		}

		
		Ref<Texture2D> tex = Texture2D::Create(xSize + 1, zSize + 1, { TextureAccessFormat::RED, TextureInternalFormat::R32, TextureDataType::FLOAT });
		constexpr auto pixelSize = sizeof(float);
		tex->SetData(heightTexData.data(), heightTexData.size() * pixelSize);
		m_PerlinNoiseTexture = GetResourceManager().Insert<Texture>(RESNAME("MeshGenerator_perlin-noise-texture"), tex);

		std::vector<uint32_t> indices(static_cast<size_t>(xSize) * static_cast<size_t>(zSize) * 6);

		int tris = 0, vert = 0;
		for (int z = 0; z < zSize; ++z) {
			for (int x = 0; x < xSize; ++x) {
				indices[tris + 0] = vert + 0;
				indices[tris + 1] = vert + 1;
				indices[tris + 2] = vert + zSize + 2;
				indices[tris + 3] = vert + zSize + 2;
				indices[tris + 4] = vert + zSize + 1;
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

	//void OnImGuiRender() override
	//{
	//}
};

#ifndef APEX_REFLECT_TOOL
#include "MeshGenerator.reflect.cpp"
APEX_REGISTER_SCRIPT_FACTORY_WITH_REFLECTION(MeshGenerator);
#endif
