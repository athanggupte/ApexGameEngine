#include "apexed_pch.h"
#include "ResourceViewer.h"

#include "IconsMaterialDesign.h"
#include "imgui.h"
#include "Apex/Application.h"

namespace Apex {

	void ResourceViewer::OnAttach()
	{
		if (m_ResourceManager == nullptr) {
			m_ResourceManager = &Application::Get().GetResourceManager();
		}
	}

	void ResourceViewer::SetContext(ResourceManager& resourceManager)
	{
		m_ResourceManager = &resourceManager;
	}

	template<typename T>
	static void ShowResourceTable(ResourceManager& resourceManager)
	{
		static std::string tableID = fmt::format("##{}-table", ResourceTypeToString(GetResourceType<T>()));

		if (ImGui::BeginTable(tableID.c_str(), 3, ImGuiTableFlags_BordersInner | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_RowBg)) {
			ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("Index").x);
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("IsLoaded?", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("IsLoaded?").x);
			ImGui::TableHeadersRow();
			int i = 0;
			for (auto& [id, ptr] : resourceManager.Iterate<Texture>()) {
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%d", i);
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%s", fmt::format("{}", Strings::Get(id)).c_str());
				ImGui::TableSetColumnIndex(2);
				ImGui::Text((ptr) ? ICON_MD_CHECK : "");

				++i;
			}
			ImGui::EndTable();
		}
	}

	void ResourceViewer::OnImGuiRender()
	{
		ImGui::Begin("Resource Viewer");

		if (ImGui::CollapsingHeader("Textures")) {
			ShowResourceTable<Texture>(*m_ResourceManager);
		}
		if (ImGui::CollapsingHeader("Shaders")) {
			ShowResourceTable<Shader>(*m_ResourceManager);
		}
		if (ImGui::CollapsingHeader("Scripts")) {
			ShowResourceTable<AScriptFactory>(*m_ResourceManager);
		}
		if (ImGui::CollapsingHeader("Meshes")) {
			ShowResourceTable<Mesh>(*m_ResourceManager);
		}
		if (ImGui::CollapsingHeader("Materials")) {
			ShowResourceTable<Material>(*m_ResourceManager);
		}

		ImGui::End();
	}

}
