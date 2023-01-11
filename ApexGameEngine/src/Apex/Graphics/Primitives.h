#pragma once

namespace Apex {

	class Mesh;
	class BufferLayout;

	namespace Primitives {

		struct Cube
		{
			static Ref<Mesh> GetMesh();
			static float* GetVertices();
			static size_t GetVertexCount();
			static BufferLayout GetLayout();
		};

		struct Plane
		{
			static Ref<Mesh> GetMesh();
			static float* GetVertices();
			static size_t GetVertexCount();
			static BufferLayout GetLayout();
		};
	}

}
