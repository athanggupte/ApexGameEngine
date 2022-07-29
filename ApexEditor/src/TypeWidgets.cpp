#include "apexed_pch.h"
#include "TypeWidgets.h"

#include "Type.h"
#include "imgui.h"
#include "glm/gtc/type_ptr.hpp"
#include "misc/cpp/imgui_stdlib.h"

#include <glm/glm.hpp>

namespace Apex {
	namespace detail {
		bool TypeEdit_Integral(const char* label, Any& any)
		{
			if (any.IsType<bool>()) {
				ImGui::Checkbox(label, any.CastPtr<bool>());
				return true;
			} else if (any.IsType<char>()) {
				ImGui::DragScalar(label, ImGuiDataType_S8, any.CastPtr<char>());
				return true;
			} else if (any.IsType<int16_t>()) {
				ImGui::DragScalar(label, ImGuiDataType_S16, any.CastPtr<int16_t>());
				return true;
			} else if (any.IsType<int32_t>()) {
				ImGui::DragScalar(label, ImGuiDataType_S32, any.CastPtr<int32_t>());
				return true;
			} else if (any.IsType<int64_t>()) {
				ImGui::DragScalar(label, ImGuiDataType_S64, any.CastPtr<int64_t>());
				return true;
			} else if (any.IsType<uint8_t>()) {
				ImGui::DragScalar(label, ImGuiDataType_U8, any.CastPtr<uint8_t>());
				return true;
			} else if (any.IsType<uint16_t>()) {
				ImGui::DragScalar(label, ImGuiDataType_U16, any.CastPtr<uint16_t>());
				return true;
			} else if (any.IsType<uint32_t>()) {
				ImGui::DragScalar(label, ImGuiDataType_U32, any.CastPtr<uint32_t>());
				return true;
			} else if (any.IsType<uint64_t>()) {
				ImGui::DragScalar(label, ImGuiDataType_U64, any.CastPtr<uint64_t>());
				return true;
			}
			return false;
		}

		bool TypeEdit_FloatingPoint(const char* label, Any& any)
		{
			if (any.IsType<float>()) {
				ImGui::DragScalar(label, ImGuiDataType_Float, any.CastPtr<float>());
				return true;
			} else if (any.IsType<double>()) {
				ImGui::DragScalar(label, ImGuiDataType_Double, any.CastPtr<double>());
				return true;
			}
			return false;
		}

		bool TypeEdit_String(const char* label, Any& any)
		{
			if (any.IsType<std::string>()) {
				ImGui::InputText(label, any.CastPtr<std::string>());
				return true;
			} else if (any.IsType<std::wstring>()) {
				ImGui::TextColored({ 1.f, 0.f, 0.f, 1.f }, "%s (std::wstring)", label);
				return true;
			}
		}

		bool TypeEdit_Vector(const char* label, Any& any)
		{
			if (any.IsType<glm::vec2>()) {
				ImGui::DragFloat2(label, glm::value_ptr(any.Cast<glm::vec2>()));
				return true;
			} else if (any.IsType<glm::vec3>()) {
				ImGui::DragFloat3(label, glm::value_ptr(any.Cast<glm::vec3>()));
				return true;
			} else if (any.IsType<glm::vec4>()) {
				ImGui::DragFloat4(label, glm::value_ptr(any.Cast<glm::vec4>()));
				return true;
			} else if (any.IsType<glm::ivec2>()) {
				ImGui::DragInt2(label, glm::value_ptr(any.Cast<glm::ivec2>()));
				return true;
			} else if (any.IsType<glm::ivec3>()) {
				ImGui::DragInt3(label, glm::value_ptr(any.Cast<glm::ivec3>()));
				return true;
			} else if (any.IsType<glm::ivec4>()) {
				ImGui::DragInt4(label, glm::value_ptr(any.Cast<glm::ivec4>()));
				return true;
			}
			return false;
		}

		bool TypeEdit_Matrix(const char* label, Any& any)
		{
			if (any.IsType<glm::mat2>()) {
				ImGui::DragFloat2(fmt::format("%s [row 0]", label).c_str(), glm::value_ptr(any.Cast<glm::mat2>()[0]));
				ImGui::DragFloat2(fmt::format("%s [row 1]", label).c_str(), glm::value_ptr(any.Cast<glm::mat2>()[1]));
				return true;
			} else if (any.IsType<glm::mat3>()) {
				ImGui::DragFloat3(fmt::format("%s [row 0]", label).c_str(), glm::value_ptr(any.Cast<glm::mat3>()[0]));
				ImGui::DragFloat3(fmt::format("%s [row 1]", label).c_str(), glm::value_ptr(any.Cast<glm::mat3>()[1]));
				ImGui::DragFloat3(fmt::format("%s [row 2]", label).c_str(), glm::value_ptr(any.Cast<glm::mat3>()[2]));
				return true;
			} else if (any.IsType<glm::vec4>()) {
				ImGui::DragFloat4(fmt::format("%s [row 0]", label).c_str(), glm::value_ptr(any.Cast<glm::mat4>()[0]));
				ImGui::DragFloat4(fmt::format("%s [row 1]", label).c_str(), glm::value_ptr(any.Cast<glm::mat4>()[1]));
				ImGui::DragFloat4(fmt::format("%s [row 2]", label).c_str(), glm::value_ptr(any.Cast<glm::mat4>()[2]));
				ImGui::DragFloat4(fmt::format("%s [row 3]", label).c_str(), glm::value_ptr(any.Cast<glm::mat4>()[3]));
				return true;
			}
			return false;
		}

	}

	bool TypeEdit(const char* label, Any& any)
	{
		switch (any.Type()->GetTypeInfo())
		{
		case Reflect::eIntegral:
			return detail::TypeEdit_Integral(label, any);
		case Reflect::eFloatingPoint:
			return detail::TypeEdit_FloatingPoint(label, any);
		case Reflect::eString:
			return detail::TypeEdit_String(label, any);
		case Reflect::eClass: break;
		case Reflect::eEnum: break;

		case Reflect::eUniquePointer: break;
		case Reflect::eSharedPointer: break;
		case Reflect::eVector:
			return detail::TypeEdit_Vector(label, any);
		case Reflect::eMatrix:
			return detail::TypeEdit_Matrix(label, any);

		case Reflect::eList: break;
		case Reflect::ePair: break;
		case Reflect::eMap: break;
		case Reflect::eTuple: break;

		case Reflect::eUnion: break;
		case Reflect::eVoid: break;
		case Reflect::eNullptr: break;
		}
		return false;
	}
}

