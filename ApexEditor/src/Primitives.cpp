#include "apexed_pch.h"
#include "Primitives.h"

#include "Apex/Graphics/Mesh.h"

namespace Apex::Primitives {
	
	// Cube

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
			{ ShaderDataType::Float3, VertexElementType::Position },
			{ ShaderDataType::Float4, VertexElementType::Color },
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

	// Plane

	namespace detail 
	{
		static float plane_vertices[] = {
			-0.5f, 0.f,  0.5f,    0.f, 1.f,    0.f, 1.f, 0.f,    1.f, 0.f, 0.f,    0.f, 0.f, -1.f,
			 0.5f, 0.f,  0.5f,    1.f, 1.f,    0.f, 1.f, 0.f,    1.f, 0.f, 0.f,    0.f, 0.f, -1.f,
			 0.5f, 0.f, -0.5f,    1.f, 0.f,    0.f, 1.f, 0.f,    1.f, 0.f, 0.f,    0.f, 0.f, -1.f,
			 0.5f, 0.f, -0.5f,    1.f, 0.f,    0.f, 1.f, 0.f,    1.f, 0.f, 0.f,    0.f, 0.f, -1.f,
			-0.5f, 0.f, -0.5f,    0.f, 0.f,    0.f, 1.f, 0.f,    1.f, 0.f, 0.f,    0.f, 0.f, -1.f,
			-0.5f, 0.f,  0.5f,    0.f, 1.f,    0.f, 1.f, 0.f,    1.f, 0.f, 0.f,    0.f, 0.f, -1.f,
			
		};

		static BufferLayout plane_buffer_layout = {
			{ ShaderDataType::Float3, VertexElementType::Position },
			{ ShaderDataType::Float2, VertexElementType::UV0 },
			{ ShaderDataType::Float3, VertexElementType::Normal },
			{ ShaderDataType::Float3, VertexElementType::Tangent },
			{ ShaderDataType::Float3, VertexElementType::Bitangent },
		};

		static Ref<Mesh> plane_mesh;
	}

	Ref<Mesh> Plane::GetMesh()
	{
		if (!detail::plane_mesh)
			detail::plane_mesh = CreateRef<Mesh>(detail::plane_vertices, std::size(detail::plane_vertices), detail::plane_buffer_layout);

		return detail::plane_mesh;
	}

	float* Plane::GetVertices()
	{
		return detail::plane_vertices;
	}

	size_t Plane::GetVertexCount()
	{
		return sizeof(detail::plane_vertices) / sizeof(float);
	}

	BufferLayout Plane::GetLayout()
	{
		return detail::plane_buffer_layout;
	}
}
