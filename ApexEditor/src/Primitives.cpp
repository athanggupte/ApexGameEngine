#include "apexed_pch.h"
#include "Primitives.h"

#include "Apex/Graphics/Mesh.h"

namespace Apex::Primitives {

	namespace detail 
	{
		static float cube_vertices[] = {
			// Left Face
			-0.5f, -0.5f,  0.5f,    1.f, 0.f, 0.f, 1.f,
			-0.5f,  0.5f,  0.5f,    1.f, 1.f, 0.f, 1.f,
			-0.5f,  0.5f, -0.5f,    0.f, 1.f, 0.f, 1.f,

			-0.5f,  0.5f, -0.5f,	0.f, 1.f, 1.f, 1.f,
			-0.5f, -0.5f, -0.5f,	0.f, 0.f, 1.f, 1.f,
			-0.5f, -0.5f,  0.5f,	1.f, 0.f, 1.f, 1.f,

			// Back Face
			-0.5f, -0.5f, -0.5f,    1.f, 0.f, 0.f, 1.f,
			-0.5f,  0.5f, -0.5f,    1.f, 1.f, 0.f, 1.f,
			 0.5f,  0.5f, -0.5f,    0.f, 1.f, 0.f, 1.f,

			 0.5f,  0.5f, -0.5f,	0.f, 1.f, 1.f, 1.f,
			 0.5f, -0.5f, -0.5f,	0.f, 0.f, 1.f, 1.f,
			-0.5f, -0.5f, -0.5f,	1.f, 0.f, 1.f, 1.f,

			// Right Face
			 0.5f, -0.5f, -0.5f,    1.f, 0.f, 0.f, 1.f,
			 0.5f,  0.5f, -0.5f,    1.f, 1.f, 0.f, 1.f,
			 0.5f,  0.5f,  0.5f,    0.f, 1.f, 0.f, 1.f,

			 0.5f,  0.5f,  0.5f,	0.f, 1.f, 1.f, 1.f,
			 0.5f, -0.5f,  0.5f,	0.f, 0.f, 1.f, 1.f,
			 0.5f, -0.5f, -0.5f,	1.f, 0.f, 1.f, 1.f,

			// Front Face
			 0.5f, -0.5f,  0.5f,    1.f, 0.f, 0.f, 1.f,
			 0.5f,  0.5f,  0.5f,    1.f, 1.f, 0.f, 1.f,
			-0.5f,  0.5f,  0.5f,    0.f, 1.f, 0.f, 1.f,

			-0.5f,  0.5f,  0.5f,	0.f, 1.f, 1.f, 1.f,
			-0.5f, -0.5f,  0.5f,	0.f, 0.f, 1.f, 1.f,
			 0.5f, -0.5f,  0.5f,	1.f, 0.f, 1.f, 1.f,

			// Bottom Face
			-0.5f, -0.5f,  0.5f,    1.f, 0.f, 0.f, 1.f,
			-0.5f, -0.5f, -0.5f,    1.f, 1.f, 0.f, 1.f,
			 0.5f, -0.5f, -0.5f,    0.f, 1.f, 0.f, 1.f,

			 0.5f, -0.5f, -0.5f,	0.f, 1.f, 1.f, 1.f,
			 0.5f, -0.5f,  0.5f,	0.f, 0.f, 1.f, 1.f,
			-0.5f, -0.5f,  0.5f,	1.f, 0.f, 1.f, 1.f,

			// Top Face
			-0.5f,  0.5f,  0.5f,    1.f, 0.f, 0.f, 1.f,
			 0.5f,  0.5f,  0.5f,    1.f, 1.f, 0.f, 1.f,
			 0.5f,  0.5f, -0.5f,    0.f, 1.f, 0.f, 1.f,

			 0.5f,  0.5f, -0.5f,	0.f, 1.f, 1.f, 1.f,
			-0.5f,  0.5f, -0.5f,	0.f, 0.f, 1.f, 1.f,
			-0.5f,  0.5f,  0.5f,	1.f, 0.f, 1.f, 1.f,
		};

		static BufferLayout cube_buffer_layout = {
			{ Apex::ShaderDataType::Float3, "a_Position" },
			{ Apex::ShaderDataType::Float4, "a_Color" },
		};

		static Ref<Mesh> cube_mesh;
	}

	Ref<Mesh> Cube::GetMesh()
	{
		if (!detail::cube_mesh)
			detail::cube_mesh = CreateRef<Mesh>(detail::cube_vertices, std::size(detail::cube_vertices), detail::cube_buffer_layout);

		return detail::cube_mesh;
	}

	float* Cube::GetVertices()
	{
		return detail::cube_vertices;
	}

	size_t Cube::GetVertexCount()
	{
		return sizeof(detail::cube_vertices) / sizeof(float);
	}

	BufferLayout Cube::GetLayout()
	{
		return detail::cube_buffer_layout;
	}
}
