// Generated by ApexReflectTool for MeshGenerator.cpp
// Include for runtime type reflection
#include "Type.h"
#include "Field.h"
#include "Function.h"
#include "ClassStorage.h"
#include "TypeImpl.hpp"
#include "FunctionImpl.h"
#include "ParamerInfoImpl.h"

// Includes from MeshGenerator.cpp
#include "Util/Random.h"
#include "pch.h"
#include "Reflect.h"
#include "Apex/Core/ECS/EntityScript.h"
#include "glm/gtc/noise.hpp"
#include "Apex/Core/ECS/Components.h"

template <>
struct ClassImpl<MeshGenerator> : public _ClassImpl<MeshGenerator>, public detail::ClassStorage<MeshGenerator, 0, 4, 0, 0>
{
    DEFINE_TYPE_NAME(MeshGenerator)

    ClassImpl()
	{
		_InitializeStorage(*this);

		m_Size = sizeof(MeshGenerator);

		{
			auto& field = m_field_beg[0]; // AddField();
			field.m_Name = "xSize";
			field.m_Type = GetType<int>();
			field.m_Offset = offsetof(MeshGenerator, xSize);
			field.m_ParentClass = this;
		}

		{
			auto& field = m_field_beg[1]; // AddField();
			field.m_Name = "zSize";
			field.m_Type = GetType<int>();
			field.m_Offset = offsetof(MeshGenerator, zSize);
			field.m_ParentClass = this;
		}

		{
			auto& field = m_field_beg[2]; // AddField();
			field.m_Name = "scale";
			field.m_Type = GetType<float>();
			field.m_Offset = offsetof(MeshGenerator, scale);
			field.m_ParentClass = this;
		}

		{
			auto& field = m_field_beg[3]; // AddField();
			field.m_Name = "height";
			field.m_Type = GetType<float>();
			field.m_Offset = offsetof(MeshGenerator, height);
			field.m_ParentClass = this;
		}

		GetTypeRegistry().AddClass(static_cast<uint64_t>(HASH(STR(MeshGenerator))), this);
		APEX_LOG_DEBUG("[INIT] :: CLASS :: MeshGenerator");
	}
};

namespace { static ClassImpl<MeshGenerator> _class_MeshGenerator; }

template <>
Class* GetClassImpl(TypeTag<MeshGenerator>)
{
    return &_class_MeshGenerator;
}

template Class* GetClass<MeshGenerator>();
template Type* GetType<MeshGenerator>();
