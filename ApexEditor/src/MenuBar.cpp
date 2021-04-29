#include "MenuBar.h"

#include "Apex/Application.h"
#include <imgui.h>

#include "Apex/Utils/Utils.h"
#include "Apex/Utils/CustomDataStructures.h"

namespace Apex {
	
	static ::SingleCopyStack<std::string, 16> s_RecentFiles;
	
	static void OpenFileStub(const std::string& filename)
	{
		APEX_LOG_INFO("File opened: {}", filename.c_str());
	}
	
	static void DrawFileMenu()
	{
		if (ImGui::MenuItem("New")) {
			
		}
		if (ImGui::MenuItem("Open", "Ctrl+O")) {
			auto filename = Utils::OpenFileDialog();
			OpenFileStub(filename);
			if (!filename.empty())
				s_RecentFiles.Push(filename);
		}
		if (ImGui::BeginMenu("Open Recent", !s_RecentFiles.Empty())) {
			std::string selectedFile = "";
			for (std::string file : s_RecentFiles)
				if (ImGui::MenuItem(file.c_str())) {
					OpenFileStub(file);
					selectedFile = file;
				}
			if (!selectedFile.empty())
				s_RecentFiles.Push(selectedFile);
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Save", "Ctrl+S")) {
			
		}
		if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S")) {
			
		}
		if (ImGui::MenuItem("Exit")) {
			Application::Get().Close();
		}
	}
	
	static void DrawEditMenu()
	{
		static bool canUndo = false;
		static bool canRedo = false;
		
		if (ImGui::MenuItem("Undo", "Ctrl+Z", false, canUndo)) {
			
		}
		if (ImGui::MenuItem("Redo", "Ctrl+Shift+Z", false, canRedo)) {
			
		}
		if (ImGui::MenuItem("Preferences", "Ctrl+P")) {
			
		}
	}
	
	void MainMenuBar::OnImGuiRender()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				DrawFileMenu();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				DrawEditMenu();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}
	
}
