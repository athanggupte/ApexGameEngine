#include "apexed_pch.h"
#include "AssetExplorer.h"

#include "Apex/Application.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace Apex {

	static struct Data {
		Ref<Texture2D> directoryTexture;
		Ref<Texture2D> fileTexture;
	};

	static Data *s_Data = nullptr;

	AssetExplorer::AssetExplorer()
	{
	}

	void AssetExplorer::OnAttach()
	{
		if (!s_Data) {
			s_Data = new Data();
			s_Data->directoryTexture = Texture2D::Create("editor_assets/textures/ae_dir.png");
			s_Data->fileTexture = Texture2D::Create("editor_assets/textures/ae_file.png");
		}
	}

	void AssetExplorer::SetContext(const fs::path& path)
	{
		m_Context = path;
		FileSystem::VisitDirectoryRecursive(m_Context, [](const FileSystem::Metadata& metadata) {
			APEX_CORE_DEBUG("{0}: {1}", (metadata.IsDirectory() ? "Dir" : metadata.IsFile() ? "File" : "Other"), metadata.path);
		});
	}

	void AssetExplorer::OnImGuiRender()
	{
		ImGui::Begin("Asset Explorer");

		bool goToParent_disabled = (m_Context == "");

		ImGui::BeginDisabled(goToParent_disabled);
		if (ImGui::ArrowButton("ParentDirectory", ImGuiDir_Up)) {
			m_Context = m_Context.parent_path();
		}
		ImGui::EndDisabled();

		ImGui::SameLine();
		if (ImGui::Button("/"))
			m_Context = "";

		fs::path tempPath = "";
		for (auto it = m_Context.begin(); it != m_Context.end(); it++) {
			ImGui::SameLine();
			tempPath /= *it;
			if (ImGui::Button(it->string().c_str())) {
				m_Context = tempPath;
			}
		}

		static float thumbnailSize = 92.f;
		static float padding = 12.f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = std::max((int)(panelWidth / cellSize), 1);

		ImGui::Columns(columnCount, 0, false);

		FileSystem::VisitDirectory(m_Context, [this](const FileSystem::Metadata& metadata) {
			std::string pathStr = metadata.path.string();
			std::string filenameStr = metadata.path.filename().string();

			ImGui::PushID(pathStr.c_str());
			
			auto& resourceManager = Application::Get().GetResourceManager();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.f, 0.f, 0.f, 0.f });

			if (metadata.IsDirectory()) {
				if (ImGui::ImageButton((void*)(intptr_t)s_Data->directoryTexture->GetID(),
					{ thumbnailSize, thumbnailSize }, { 0.f, 0.f }, { 1.f, 1.f })) {
					m_Context = metadata.path;
				}
			}
			else if (metadata.IsFile()) {
				if (ImGui::ImageButton((void*)(intptr_t)s_Data->fileTexture->GetID(),
					{ thumbnailSize, thumbnailSize }, { 0.f, 0.f }, { 1.f, 1.f })) {
					// Display file details in InspectorPanel
				}

				if (ImGui::BeginDragDropSource()) {
					ImGui::Text("%s", filenameStr.c_str());

					ImGui::SetDragDropPayload("ASSET_BROWSER_ITEM", pathStr.data(), pathStr.size() + 1);
					ImGui::EndDragDropSource();
				}

			}

			ImGui::PopStyleColor();

			ImGui::TextWrapped(filenameStr.c_str());
			ImGui::NextColumn();
			ImGui::PopID();
		});

		ImGui::Columns(1);

		ImGui::End();
	}

}