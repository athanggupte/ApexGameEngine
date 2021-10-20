#pragma once

namespace Apex {

	class Mesh;
	struct BufferLayout;

	namespace Primitives {

		struct Cube {
			static Ref<Mesh> GetMesh();
			static float* GetVertices();
			static size_t GetVertexCount();
			static BufferLayout GetLayout();
		};
		
	}

}
