#pragma once
#include "Apex/Core/ResourceManager/ResourceManager.h"

namespace Apex {

	class MaterialPanel
	{
	public:
		MaterialPanel() = default;

		void SetContext(Handle materialId);
		void SetContext(const Resource<Material>& materialResource);
		Resource<Material> GetContext() const;

		void OnImGuiRender();

	protected:
		void DrawMaterialList();
		void DrawMaterialProperties();

	private:
		Resource<Material> m_ContextMaterial;
	};

}
