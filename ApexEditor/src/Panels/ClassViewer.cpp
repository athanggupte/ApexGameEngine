#include "apexed_pch.h"
#include "ClassViewer.h"
#include "imgui.h"

#include "Class.h"
#include "Registry.h"
#include "Type.h"
#include "Apex/Core/Strings.h"
#include "Apex/Utils/Utils.h"

namespace Apex {

	static void ShowClass(Class* c)
	{
		if (ImGui::CollapsingHeader(c->Name().data())) {
			static std::string tableID = fmt::format("Fields##{}", c->Name());

			if (ImGui::BeginTable(tableID.c_str(), 3, ImGuiTableFlags_BordersInner | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_RowBg)) {
				ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("Index").x);
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableHeadersRow();
				int i = 0;
				for (auto& field : c->fields()) {
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%d", i);
					ImGui::TableSetColumnIndex(1);
					ImGui::Text("%s", field.Name());
					ImGui::TableSetColumnIndex(2);
					ImGui::Text("%s", TO_CSTRING(field.Type()->Name()));

					++i;
				}
				ImGui::EndTable();
			}
		}
	}

	void ClassViewer::OnImGuiRender()
	{
		ImGui::Begin("Class Viewer");

		auto& typeRegistry = GetTypeRegistry();
		typeRegistry.VisitClasses(ShowClass);

		ImGui::End();
	}
}
