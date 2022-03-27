#include "apex_pch.h"
#include "FBXImporter.h"

#include "Material.h"
#include "Apex/Application.h"
#include "Apex/Core/ECS/Scene.h"
#include "Apex/Core/ECS/Components.h"

#include <fbxsdk.h>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Apex {

	namespace fbx {

		struct ControlPoint
		{
			glm::vec3 Position;
		};

		struct Vertex
		{
			glm::vec3 Position;
			glm::vec2 UV;
			glm::vec3 Normal;
			glm::vec4 Tangent;
		};
		static constexpr size_t VERTEX_SIZE = sizeof(Vertex) / sizeof(float);

		FbxScene* ParseScene(const std::filesystem::path& filepath);
		void ProcessNode(FbxNode* node, Scene* scene, bool recursive = true);
		void ProcessAttribute(FbxNode* node, FbxNodeAttribute* attribute);
		[[nodiscard]] Ref<Mesh> ProcessMesh(FbxNode* node, FbxNodeAttribute* attribute);
		void ProcessControlPoints(FbxMesh* mesh, std::vector<ControlPoint>& outControlPoints);
		void ProcessGeometry(FbxNode* node, FbxMesh* mesh, const std::vector<ControlPoint>& controlPoints,
		                 std::vector<Vertex>& outVertices, std::vector<uint32_t>& outIndices);
		[[nodiscard]] Ref<Material> ProcessMaterial(FbxNode* node, FbxSurfaceMaterial* material);
		void ProcessNormal(FbxMesh* mesh, int controlPointIndex, int vertexCounter, glm::vec3& outNormal);
		void ProcessTangent(FbxMesh* mesh, int controlPointIndex, int vertexCounter, glm::vec4& outTangent);
		void ProcessBitangent(FbxMesh* mesh, int controlPointIndex, int vertexCounter, glm::vec3& outBitangent);
		void GenerateTangentsAndBitangents();
		void ProcessUV(FbxMesh* mesh, int controlPointIndex, int uvIndex, glm::vec2& outTexCoord);


		void Vec2FbxToGlm(const FbxDouble2& from, glm::vec2& to);
		void Vec3FbxToGlm(const FbxDouble3& from, glm::vec3& to);
		void Vec4FbxToGlm(const FbxDouble4& from, glm::vec4& to);

		const char* GetAttributeTypeName(FbxNodeAttribute::EType type);
	}


	static struct FBXData
	{
		FbxManager* fbxManager = nullptr;
	} s_Data;

	void FBXImporter::Init()
	{
		s_Data.fbxManager = FbxManager::Create();
		APEX_CORE_ASSERT(s_Data.fbxManager, "FBX Manager could not be initialized!");

		s_Data.fbxManager->SetIOSettings(FbxIOSettings::Create(s_Data.fbxManager, IOSROOT));
	}

	void FBXImporter::Shutdown()
	{
		s_Data.fbxManager->Destroy();
	}

	void FBXImporter::Import(const Ref<File>& file, const Ref<Scene>& scene)
	{
		FbxScene* fbxScene = fbx::ParseScene(file->GetPhysicalPath());

		APEX_CORE_INFO("Importing FBX scene: {0}", fbxScene->GetName());
		if (FbxNode* rootNode = fbxScene->GetRootNode()) {
			APEX_CORE_INFO("FBXImport ({0}) : child count = {2}", fbxScene->GetName(), rootNode->GetName(), rootNode->GetChildCount());
			for (int i = 0; i < rootNode->GetChildCount(); i++)
				fbx::ProcessNode(rootNode->GetChild(i), scene.get());
		}
	}

	Ref<Mesh> FBXImporter::LoadMesh(const Ref<File>& file, const std::string& nodeName,
	                                const Ref<Scene>& scene)
	{
		FbxScene* fbxScene = fbx::ParseScene(file->GetPhysicalPath());

		FbxNode* node = nullptr;
		if (!nodeName.empty()) {
			if (auto childNode = fbxScene->GetRootNode()->FindChild(nodeName.c_str()))
				node = childNode;
		}
		if (node == nullptr)
			node =  fbxScene->GetRootNode()->GetChild(0);
		 	
		// fbx::ProcessNode(node, scene.get(), false);
		auto meshAttr = node->GetMesh();
		if (meshAttr) {
			meshAttr->ComputeBBox();
			auto bbMin = meshAttr->BBoxMin.Get();
			auto bbMinStr = fmt::format("({}, {}, {})", bbMin[0], bbMin[1], bbMin[2]);
			auto bbMax = meshAttr->BBoxMax.Get();
			auto bbMaxStr = fmt::format("({}, {}, {})", bbMax[0], bbMax[1], bbMax[2]);
			APEX_CORE_DEBUG("FBXImporter ({0}) :: BBoxMin : {} | BBoxMax : {}", node->GetName(), bbMinStr, bbMaxStr);
			return fbx::ProcessMesh(node, meshAttr);
		}
		return { nullptr };
	}

	FbxScene* fbx::ParseScene(const std::filesystem::path& filepath)
	{
		auto filenameStr = filepath.filename().string();
		auto filepathStr = filepath.string();
		::FbxImporter* fbxImporter = ::FbxImporter::Create(s_Data.fbxManager, filenameStr.c_str());
		APEX_CORE_ASSERT(fbxImporter->Initialize(filepathStr.c_str(), -1, s_Data.fbxManager->GetIOSettings()),
			fmt::format("FbxImporter Initialize failed for '{0}'! Error: {1}", filepath, fbxImporter->GetStatus().GetErrorString()));

		FbxScene* fbxScene = FbxScene::Create(s_Data.fbxManager, filenameStr.c_str());
		if (!fbxImporter->Import(fbxScene)) {
			FbxStatus importStatus = fbxImporter->GetStatus();
			APEX_CORE_ERROR("Could not load FBX scene : {} :: {}", filepathStr, importStatus.GetErrorString());
		}
		fbxImporter->Destroy();

		if(fbxScene->GetGlobalSettings().GetSystemUnit() != FbxSystemUnit::m)
		{
			//constexpr FbxSystemUnit::ConversionOptions fbxConversionOptions = {
			//	false, /* mConvertRrsNodes */
			//	true, /* mConvertAllLimits */
			//	true, /* mConvertClusters */
			//	true, /* mConvertLightIntensity */
			//	true, /* mConvertPhotometricLProperties */
			//	true  /* mConvertCameraClipPlanes */
			//};

			// Convert the scene to meters using the defined options.
			FbxSystemUnit::m.ConvertScene(fbxScene/*, fbxConversionOptions*/);
		}

		return fbxScene;
	}

	void fbx::ProcessNode(FbxNode* node, Scene* scene, bool recursive)
	{
		const char* nodeName = node->GetName();
		glm::vec3 translation, rotation, scale;
		Vec3FbxToGlm(node->LclTranslation.Get(), translation);
		Vec3FbxToGlm(node->LclRotation.Get(), rotation);
		Vec3FbxToGlm(node->LclScaling.Get(), scale);

		Entity entity;

		APEX_CORE_INFO("FBXImport ({0}) : T({1}) | R({2}) | S({3})", nodeName, glm::to_string(translation), glm::to_string(rotation), glm::to_string(scale));

		auto attribute = node->GetNodeAttribute();
		const char* attrName = attribute->GetName();
		const char* typeName = GetAttributeTypeName(attribute->GetAttributeType());

		APEX_CORE_INFO("FBXImport ({0}) :: attr ({1}) : {2}", nodeName, typeName, attrName);

		switch (attribute->GetAttributeType()) {
		case FbxNodeAttribute::eSkeleton:
		{
			APEX_CORE_DEBUG("FBXImport ({0}) :: Skeleton", nodeName);
			break;
		}
		case FbxNodeAttribute::eMesh:
		{
			FbxGeometryConverter geometryConverter(s_Data.fbxManager);
			attribute = geometryConverter.Triangulate(attribute, false);
			auto _mesh = ProcessMesh(node, attribute);

			if (Application::Get().GetResourceManager().Exists(RESNAME(attrName))) {
				Application::Get().GetResourceManager().Get<Mesh>(RESNAME(attrName)).Get() = _mesh;
			} else {
				Application::Get().GetResourceManager().Insert<Mesh>(RESNAME(attrName), _mesh);
			}

			std::vector<Ref<Material>> materials;
			for (int i = 0; i < node->GetMaterialCount(); i++) {
				materials.push_back(ProcessMaterial(node , node->GetMaterial(i)));
			}

			if (scene) {
				entity = scene->CreateEntity();
				entity.GetComponent<TagComponent>().tag = HASH(nodeName);
				auto& transform = entity.GetComponent<TransformComponent>();
				transform.translation = translation;
				transform.rotation = glm::radians(rotation);
				transform.scale = scale;

				auto meshRes = Application::Get().GetResourceManager().Get<Mesh>(RESNAME(attrName));
				auto shaderRes = Application::Get().GetResourceManager().Get<Shader>(RESNAME("shader_StandardPBR"));
				auto material = CreateRef<Material>();
				material->SetShader(shaderRes);
				auto matRes = Application::Get().GetResourceManager().Insert<Material>(RESNAME(fmt::format("material_{}", attrName)), material);
				// material->SetTexture("Albedo", Application::Get().GetResourceManager().Get<Texture>(RESNAME("metal_plate_diff_2k")));
				entity.AddComponent<MeshRendererComponent>(meshRes, matRes);
			}

			break;
		}
		case FbxNodeAttribute::eCamera: break;
		case FbxNodeAttribute::eLight: break;
		default: break;
		}

		for (int i = 0; i < node->GetChildCount(); i++)
			ProcessNode(node->GetChild(i), scene);
	}

	Ref<Mesh> fbx::ProcessMesh(FbxNode* node, FbxNodeAttribute* attribute)
	{
		std::vector<ControlPoint> controlPoints;
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		ProcessControlPoints((FbxMesh*)attribute, controlPoints);
		ProcessGeometry(node, (FbxMesh*)attribute, controlPoints, vertices, indices);

		BufferLayout layout = {
			{ ShaderDataType::Float3, VertexElementType::Position },
			{ ShaderDataType::Float2, VertexElementType::UV0 },
			{ ShaderDataType::Float3, VertexElementType::Normal },
			{ ShaderDataType::Float4, VertexElementType::Tangent },
		};
		return CreateRef<Mesh>(reinterpret_cast<float*>(vertices.data()), vertices.size() * VERTEX_SIZE, indices.data(), indices.size(), layout);
	}

	void fbx::ProcessControlPoints(FbxMesh* mesh, std::vector<ControlPoint>& outControlPoints)
	{
		const int controlPointsCount = mesh->GetControlPointsCount();
		
		outControlPoints.reserve(controlPointsCount);

		for (int i = 0; i < controlPointsCount; i++) {
			outControlPoints.push_back({});
			outControlPoints[i].Position.x = (float)mesh->GetControlPointAt(i)[0];
			outControlPoints[i].Position.y = (float)mesh->GetControlPointAt(i)[1];
			outControlPoints[i].Position.z = (float)mesh->GetControlPointAt(i)[2];
		}
	}

	void fbx::ProcessGeometry(FbxNode* node, FbxMesh* mesh, const std::vector<ControlPoint>& controlPoints, std::vector<Vertex>& outVertices, std::vector<uint32_t>& outIndices)
	{
		const size_t triangleCount = mesh->GetPolygonCount();
		uint32_t vertexCounter = 0;
		outIndices.reserve(triangleCount * 3);
		outVertices.reserve(triangleCount * 3);

		APEX_CORE_ASSERT(mesh->GetElementNormalCount() > 0, fmt::format("Mesh ({0}) :: Does not contain Normals!", mesh->GetName()));
		//APEX_CORE_ASSERT(mesh->GetElementTangentCount() > 0, fmt::format("Mesh ({0}) :: Does not contain Tangents!", mesh->GetName()));
		//APEX_CORE_ASSERT(mesh->GetElementBinormalCount() > 0, fmt::format("Mesh ({0}) :: Does not contain Bitangents!", mesh->GetName()));

		bool hasTangents = mesh->GetElementTangentCount() > 0;
		bool hasBitangents = mesh->GetElementBinormalCount() > 0;

		for (int i = 0; i < triangleCount; i++) {
			for (int j = 0; j < 3; j++) {
				outVertices.push_back({});
				auto& vertex = outVertices.back();

				int controlPointIndex = mesh->GetPolygonVertex(i, j);

				vertex.Position = controlPoints[controlPointIndex].Position;
				ProcessNormal(mesh, controlPointIndex, vertexCounter, vertex.Normal);
				ProcessUV(mesh, controlPointIndex, vertexCounter /*mesh->GetTextureUVIndex(i, j)*/, vertex.UV);

				vertex.Tangent = glm::vec4{ 0.f };
				if (hasTangents)
					ProcessTangent(mesh, controlPointIndex, vertexCounter, vertex.Tangent);

				if (hasBitangents) {
					glm::vec3 bitangent;
					ProcessBitangent(mesh, controlPointIndex, vertexCounter, bitangent);
					vertex.Tangent.w = (glm::dot(glm::cross(glm::vec3(vertex.Tangent), bitangent), vertex.Normal) > 0.f) ? 1.f : -1.f;
				}

				outIndices.push_back(vertexCounter++);
			}
		}

		if (!hasTangents) {
			APEX_CORE_INFO("Tangent data not found! Calculating Tangents from Position and UV data.");
			std::vector<glm::vec3> tangents(triangleCount * 3);
			std::vector<glm::vec3> bitangents(triangleCount * 3);

			for (int i = 0; i < triangleCount; i++) {
				const auto vIndex = i * 3;
				auto& v0 = outVertices[vIndex];
				auto& v1 = outVertices[vIndex + 1];
				auto& v2 = outVertices[vIndex + 2];
				glm::vec3 e1 = v1.Position - v0.Position,
				          e2 = v2.Position - v0.Position;
				float x1 = v1.UV.x - v0.UV.x, x2 = v2.UV.x - v0.UV.x,
				      y1 = v1.UV.y - v0.UV.y, y2 = v2.UV.y - v0.UV.y;

				float det = 1.f / (x1 * y2 - x2 * y1);
				glm::vec3 t = (e1 * y2 - e2 * y1) * det;
				glm::vec3 b = (e2 * x1 - e1 * x2) * det;

				tangents[vIndex] += t;
				tangents[vIndex + 1] += t;
				tangents[vIndex + 2] += t;

				bitangents[vIndex] += b;
				bitangents[vIndex + 1] += b;
				bitangents[vIndex + 2] += b;
			}

			for (auto i = 0; i < outVertices.size(); i++) {
				// Gram-Schmidt Orthonormalization
				auto& vertex = outVertices[i];
				const auto& t = tangents[i];
				const auto& b = bitangents[i];
				const auto& n = vertex.Normal;

				vertex.Tangent = glm::vec4(glm::normalize(t - glm::dot(t, n) * n), 0.f);
				vertex.Tangent.w = (glm::dot(glm::cross(t, b), n) > 0.f) ? 1.f : -1.f;
			}

		}
	}

	Ref<Material> fbx::ProcessMaterial(FbxNode* node, FbxSurfaceMaterial* material)
	{
		APEX_CORE_INFO("FBXImport ({0}) : material ( {1} )", node->GetName(), material->GetName());
		const auto materialClassId = material->GetClassId();
		// If the material is a Phong or Lambert shading model then handle it as Phong
		if (materialClassId.Is(FbxSurfacePhong::ClassId) || materialClassId.Is(FbxSurfaceLambert::ClassId)) {
			auto phongMaterial = static_cast<FbxSurfacePhong*>(material);
			//auto apxShader = Application::Get().GetResourceManager().Get<Shader>(RESNAME("shader_StandardPBR"));
			//auto apxMaterial = CreateRef<Material>(apxShader);

			//glm::vec3 diffuse;
			//Vec3FbxToGlm(phongMaterial->Diffuse.Get(), diffuse);

			



			//apxMaterial->SetAltColor("Albedo", glm::vec4(diffuse, 1.f));
			
		} else {
			
		}

		APEX_CORE_DEBUG("===== FBX Material Properties =====");
		APEX_CORE_DEBUG("Material: {}", material->GetName());
		for (int textureIndex = 0; textureIndex < FbxLayerElement::sTypeTextureCount; ++textureIndex) {
			FbxProperty property =	material->FindProperty(FbxLayerElement::sTextureChannelNames[textureIndex]);
			if (property.IsValid()) {
				APEX_CORE_DEBUG("	Property: {}", property.GetName());
				const int textureCount = property.GetSrcObjectCount<FbxTexture>();
				for (int j = 0; j < textureCount; ++j) {
					auto* layeredTexture = property.GetSrcObject<FbxLayeredTexture>();
					if (layeredTexture) {
						APEX_CORE_DEBUG("		Layered Texture:");
					} else {
						auto* texture = property.GetSrcObject<FbxTexture>(j);
						if (texture) {
							APEX_CORE_DEBUG("		Texture [{}]: {}", j, texture->GetName());
							auto* fileTexture = FbxCast<FbxFileTexture>(texture);
							if (fileTexture) {
								APEX_CORE_DEBUG("			Filepath: {}", fileTexture->GetRelativeFileName());
							}
						}
					}
				}
			}
		}
		return { nullptr };
	}

	void fbx::ProcessNormal(FbxMesh* mesh, int controlPointIndex, int vertexCounter, glm::vec3& outNormal)
	{
		FbxGeometryElementNormal* vertexNormal = mesh->GetElementNormal();

		switch (vertexNormal->GetMappingMode()) {
		case FbxLayerElement::eByControlPoint:  // One normal per control point (smooth edge)
		{
			switch (vertexNormal->GetReferenceMode()) {
			case FbxLayerElement::eDirect:
			{
				outNormal.x = (float)vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[0];
				outNormal.y = (float)vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[1];
				outNormal.z = (float)vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[2];
				break;
			}
			case FbxLayerElement::eIndexToDirect:
			{
				int index = vertexNormal->GetIndexArray().GetAt(controlPointIndex);
				outNormal.x = (float)vertexNormal->GetDirectArray().GetAt(index).mData[0];
				outNormal.y = (float)vertexNormal->GetDirectArray().GetAt(index).mData[1];
				outNormal.z = (float)vertexNormal->GetDirectArray().GetAt(index).mData[2];
				break;
			}
			default:
				APEX_CORE_CRITICAL("Mesh ({0}) :: Invalid reference mode for normal!", mesh->GetName());
				break;
			}
			break;
		}
		case FbxLayerElement::eByPolygonVertex:  // One normal per polygon face vertex (sharp edge)
		{
			switch (vertexNormal->GetReferenceMode()) {
			case FbxLayerElement::eDirect:
			{
				outNormal.x = (float)vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[0];
				outNormal.y = (float)vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[1];
				outNormal.z = (float)vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[2];
				break;
			}
			case FbxLayerElement::eIndexToDirect:
			{
				int index = vertexNormal->GetIndexArray().GetAt(vertexCounter);
				outNormal.x = (float)vertexNormal->GetDirectArray().GetAt(index).mData[0];
				outNormal.y = (float)vertexNormal->GetDirectArray().GetAt(index).mData[1];
				outNormal.z = (float)vertexNormal->GetDirectArray().GetAt(index).mData[2];
				break;
			}
			default:
				APEX_CORE_CRITICAL("Mesh ({0}) :: Invalid reference mode for normal!", mesh->GetName());
				break;
			}
			break;
		}
		default:
			APEX_CORE_CRITICAL("Mesh ({0}) :: Invalid normal mode for normal!", mesh->GetName());
			break;
		}
	}

	void fbx::ProcessTangent(FbxMesh* mesh, int controlPointIndex, int vertexCounter, glm::vec4& outTangent)
	{
		FbxGeometryElementTangent* vertexTangent = mesh->GetElementTangent();

		switch (vertexTangent->GetMappingMode()) {
		case FbxLayerElement::eByControlPoint:  // One normal per control point (smooth edge)
		{
			switch (vertexTangent->GetReferenceMode()) {
			case FbxLayerElement::eDirect:
			{
				outTangent.x = (float)vertexTangent->GetDirectArray().GetAt(controlPointIndex).mData[0];
				outTangent.y = (float)vertexTangent->GetDirectArray().GetAt(controlPointIndex).mData[1];
				outTangent.z = (float)vertexTangent->GetDirectArray().GetAt(controlPointIndex).mData[2];
				break;
			}
			case FbxLayerElement::eIndexToDirect:
			{
				int index = vertexTangent->GetIndexArray().GetAt(controlPointIndex);
				outTangent.x = (float)vertexTangent->GetDirectArray().GetAt(index).mData[0];
				outTangent.y = (float)vertexTangent->GetDirectArray().GetAt(index).mData[1];
				outTangent.z = (float)vertexTangent->GetDirectArray().GetAt(index).mData[2];
				break;
			}
			default:
				APEX_CORE_CRITICAL("Mesh ({0}) :: Invalid reference mode for normal!", mesh->GetName());
				break;
			}
			break;
		}
		case FbxLayerElement::eByPolygonVertex:  // One normal per polygon face vertex (sharp edge)
		{
			switch (vertexTangent->GetReferenceMode()) {
			case FbxLayerElement::eDirect:
			{
				outTangent.x = (float)vertexTangent->GetDirectArray().GetAt(vertexCounter).mData[0];
				outTangent.y = (float)vertexTangent->GetDirectArray().GetAt(vertexCounter).mData[1];
				outTangent.z = (float)vertexTangent->GetDirectArray().GetAt(vertexCounter).mData[2];
				break;
			}
			case FbxLayerElement::eIndexToDirect:
			{
				int index = vertexTangent->GetIndexArray().GetAt(vertexCounter);
				outTangent.x = (float)vertexTangent->GetDirectArray().GetAt(index).mData[0];
				outTangent.y = (float)vertexTangent->GetDirectArray().GetAt(index).mData[1];
				outTangent.z = (float)vertexTangent->GetDirectArray().GetAt(index).mData[2];
				break;
			}
			default:
				APEX_CORE_CRITICAL("Mesh ({0}) :: Invalid reference mode for normal!", mesh->GetName());
				break;
			}
			break;
		}
		default:
			APEX_CORE_CRITICAL("Mesh ({0}) :: Invalid normal mode for normal!", mesh->GetName());
			break;
		}
	}

	void fbx::ProcessBitangent(FbxMesh* mesh, int controlPointIndex, int vertexCounter, glm::vec3& outBitangent)
	{
		FbxGeometryElementBinormal* vertexBitangent = mesh->GetElementBinormal();

		switch (vertexBitangent->GetMappingMode()) {
		case FbxLayerElement::eByControlPoint:  // One normal per control point (smooth edge)
		{
			switch (vertexBitangent->GetReferenceMode()) {
			case FbxLayerElement::eDirect:
			{
				outBitangent.x = (float)vertexBitangent->GetDirectArray().GetAt(controlPointIndex).mData[0];
				outBitangent.y = (float)vertexBitangent->GetDirectArray().GetAt(controlPointIndex).mData[1];
				outBitangent.z = (float)vertexBitangent->GetDirectArray().GetAt(controlPointIndex).mData[2];
				break;
			}
			case FbxLayerElement::eIndexToDirect:
			{
				int index = vertexBitangent->GetIndexArray().GetAt(controlPointIndex);
				outBitangent.x = (float)vertexBitangent->GetDirectArray().GetAt(index).mData[0];
				outBitangent.y = (float)vertexBitangent->GetDirectArray().GetAt(index).mData[1];
				outBitangent.z = (float)vertexBitangent->GetDirectArray().GetAt(index).mData[2];
				break;
			}
			default:
				APEX_CORE_CRITICAL("Mesh ({0}) :: Invalid reference mode for normal!", mesh->GetName());
				break;
			}
			break;
		}
		case FbxLayerElement::eByPolygonVertex:  // One normal per polygon face vertex (sharp edge)
		{
			switch (vertexBitangent->GetReferenceMode()) {
			case FbxLayerElement::eDirect:
			{
				outBitangent.x = (float)vertexBitangent->GetDirectArray().GetAt(vertexCounter).mData[0];
				outBitangent.y = (float)vertexBitangent->GetDirectArray().GetAt(vertexCounter).mData[1];
				outBitangent.z = (float)vertexBitangent->GetDirectArray().GetAt(vertexCounter).mData[2];
				break;
			}
			case FbxLayerElement::eIndexToDirect:
			{
				int index = vertexBitangent->GetIndexArray().GetAt(vertexCounter);
				outBitangent.x = (float)vertexBitangent->GetDirectArray().GetAt(index).mData[0];
				outBitangent.y = (float)vertexBitangent->GetDirectArray().GetAt(index).mData[1];
				outBitangent.z = (float)vertexBitangent->GetDirectArray().GetAt(index).mData[2];
				break;
			}
			default:
				APEX_CORE_CRITICAL("Mesh ({0}) :: Invalid reference mode for normal!", mesh->GetName());
				break;
			}
			break;
		}
		default:
			APEX_CORE_CRITICAL("Mesh ({0}) :: Invalid normal mode for normal!", mesh->GetName());
			break;
		}
	}

	void fbx::GenerateTangentsAndBitangents()
	{

	}

	void fbx::ProcessUV(FbxMesh* mesh, int controlPointIndex, int uvIndex, glm::vec2& outUV)
	{
		APEX_CORE_ASSERT(mesh->GetElementUVCount() > 0, fmt::format("Mesh ({0}) :: Does not contain UVs!", mesh->GetName()));
		FbxGeometryElementUV* vertexUV = mesh->GetElementUV();

		switch (vertexUV->GetMappingMode()) {
		case FbxLayerElement::eByControlPoint:  // One normal per control point (smooth edge)
		{
			switch (vertexUV->GetReferenceMode()) {
			case FbxLayerElement::eDirect:
			{
				outUV.x = (float)vertexUV->GetDirectArray().GetAt(controlPointIndex).mData[0];
				outUV.y = (float)vertexUV->GetDirectArray().GetAt(controlPointIndex).mData[1];
				break;
			}
			case FbxLayerElement::eIndexToDirect:
			{
				int index = vertexUV->GetIndexArray().GetAt(controlPointIndex);
				outUV.x = (float)vertexUV->GetDirectArray().GetAt(index).mData[0];
				outUV.y = (float)vertexUV->GetDirectArray().GetAt(index).mData[1];
				break;
			}
			default:
				APEX_CORE_CRITICAL("Mesh ({0}) :: Invalid reference mode for normal!", mesh->GetName());
				break;
			}
			break;
		}
		case FbxLayerElement::eByPolygonVertex:
		{
			switch (vertexUV->GetReferenceMode()) {
			case FbxLayerElement::eDirect:
			{
				outUV.x = (float)vertexUV->GetDirectArray().GetAt(uvIndex).mData[0];
				outUV.y = (float)vertexUV->GetDirectArray().GetAt(uvIndex).mData[1];
				break;
			}
			case FbxLayerElement::eIndexToDirect:
			{
				int index = vertexUV->GetIndexArray().GetAt(uvIndex);
				outUV.x = (float)vertexUV->GetDirectArray().GetAt(index).mData[0];
				outUV.y = (float)vertexUV->GetDirectArray().GetAt(index).mData[1];
				break;
			}
			default:
				APEX_CORE_CRITICAL("Mesh ({0}) :: Invalid reference mode for normal!", mesh->GetName());
				break;
			}
			break;
		}
		default:
			APEX_CORE_CRITICAL("Mesh ({0}) :: Invalid normal mode for normal!", mesh->GetName());
			break;
		}
	}

	void fbx::Vec2FbxToGlm(const FbxDouble2& from, glm::vec2& to)
	{
		to.x = (float)from[0];
		to.y = (float)from[1];
	}

	void fbx::Vec3FbxToGlm(const FbxDouble3& from, glm::vec3& to)
	{
		to.x = (float)from[0];
		to.y = (float)from[1];
		to.z = (float)from[2];
	}

	void fbx::Vec4FbxToGlm(const FbxDouble4& from, glm::vec4& to)
	{
		to.x = (float)from[0];
		to.y = (float)from[1];
		to.z = (float)from[2];
		to.w = (float)from[3];
	}

	const char* fbx::GetAttributeTypeName(FbxNodeAttribute::EType type) {
        switch(type) {
            case FbxNodeAttribute::eUnknown: return "unidentified";
            case FbxNodeAttribute::eNull: return "null";
            case FbxNodeAttribute::eMarker: return "marker";
            case FbxNodeAttribute::eSkeleton: return "skeleton";
            case FbxNodeAttribute::eMesh: return "mesh";
            case FbxNodeAttribute::eNurbs: return "nurbs";
            case FbxNodeAttribute::ePatch: return "patch";
            case FbxNodeAttribute::eCamera: return "camera";
            case FbxNodeAttribute::eCameraStereo: return "stereo";
            case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
            case FbxNodeAttribute::eLight: return "light";
            case FbxNodeAttribute::eOpticalReference: return "optical reference";
            case FbxNodeAttribute::eOpticalMarker: return "marker";
            case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
            case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
            case FbxNodeAttribute::eBoundary: return "boundary";
            case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
            case FbxNodeAttribute::eShape: return "shape";
            case FbxNodeAttribute::eLODGroup: return "lodgroup";
            case FbxNodeAttribute::eSubDiv: return "subdiv";
            default: return "unknown";
        }
    }

}
