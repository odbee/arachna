#include "GuiSetup.h"




void GuiHandler::setup() {

}




void GuiHandler::setupImGui() {

	ImGui::Initialize();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	//io.Fonts->AddFontFromFileTTF("../resources/fonts/Abordage-Regular.ttf", 14);
	//io.Fonts->AddFontFromFileTTF("../resources/fonts/Karrik-Regular.ttf", 15);

	//ImGui::GetStyle().WindowRounding = 20.0f;// <- Set this on init or use ImGui::PushStyleVar()
	//ImGui::GetStyle().WindowPadding.x = 30.0f;// <- Set this on init or use ImGui::PushStyleVar()
	//ImGui::GetStyle().WindowPadding.y = 20.0f;// <- Set this on init or use ImGui::PushStyleVar()
	//ImGui::GetStyle().FramePadding.y = 5.0f;// <- Set this on init or use ImGui::PushStyleVar()
	//ImGui::GetStyle().FramePadding.x = 8.0f;// <- Set this on init or use ImGui::PushStyleVar()

	//ImGui::GetStyle().ChildRounding = 0.0f;
	//ImGui::GetStyle().FrameRounding = 5.0f;
	//ImGui::GetStyle().GrabRounding = 20.0f;
	//ImGui::GetStyle().PopupRounding = 0.0f;
	//ImGui::GetStyle().ScrollbarRounding = 12.0f;
	//ImGui::GetStyle().WindowBorderSize = 2.0f;


	//ImGui::GetStyle().FrameBorderSize = 0.0f;
	//ImGui::GetStyle().ChildBorderSize = 0.0f;
	//ImGui::GetStyle().GrabMinSize = 20.0f;

	//ImGui::GetStyle().Colors[ImGuiCol_Border] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	//ImGui::GetStyle().Colors[ImGuiCol_Border] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	//ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
}
void GuiHandler::drawParametersWindow() {
	bool isopen;
	ImGui::Begin("Parameters");
	data.my_log.Draw("Log");

	if (data.selected_edge != data.empty_edge) {
			ImGui::Text("highlighted edge:");
			ImGui::Text("Unique Index: %i\n", g[data.selected_edge].uniqueIndex);
			ImGui::Text(" Index: %i\n", g[data.selected_edge].index);
			ImGui::Text("isforbidden?");			
			ImGui::Text((g[data.selected_edge].isforbidden) ? "true" : "false");
			ImGui::Text("daughter edges : ");
			ImGui::Text(stringfromVec(edgesG[g[data.selected_edge].uniqueIndex-1].daughterEdges).c_str());
			ImGui::Text("\n");
			ImGui::Text("edge lengths : ");
			ImGui::Text(stringfromVec(edgesG[g[data.selected_edge].uniqueIndex-1].DivEdgeLengths).c_str());
			ImGui::Text("\n");
			ImGui::Text("current length : ");
			ImGui::Text("%f",g[data.selected_edge].currentlength);
			ImGui::Text("\n");
			ImGui::Text("rest length : ");
			ImGui::Text("%f", g[data.selected_edge].restlength);
			ImGui::Text("\n");
			ImGui::Text("inter points : ");
			ImGui::Text(stringfromVec(edgesG[g[data.selected_edge].uniqueIndex-1].interPts).c_str());
			ImGui::Text("\n");
			

	}
	if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::InputText("import directory path ", &data.fullPath)) {
			iniHand.overwriteTagImGui("graph directory", data.fullPath);
		}

		//ImGui::InputInt("x iterations", &x_iterations, 50, 200);
		//if (ImGui::Button("run x iterations")) {
		//	runxiterations(x_iterations, g, relaxc, cycles);

		//}


		//checkforchange(vallist, cachelist);
		//ImGui::Checkbox("check for forbidden edges", &CHECKFORBIDDEN);
		//if (ImGui::Button("Reset web")) {
		//	console() << "resetting web" << endl;
		//	resetweb(g, relaxc, cycles);

		ImGui::SameLine();
		ImGui::PushItemWidth(80);
		
		ImGui::PopItemWidth();
		ImGui::Separator();

	}

	drawArduPortTab();
	ImGui::End();

}

void  GuiHandler::drawArduPortTab() {
	if (ImGui::CollapsingHeader("Arduino Port", ImGuiTreeNodeFlags_DefaultOpen)) {


		if (ImGui::InputText("COM Port ", &data.COMPort)) {
			iniHand.overwriteTagImGui("communication port", data.COMPort);
		}
		if (ImGui::Button("start connection")) {
			
			data.my_log.AddLog((arduP.connectArduino(data.COMPort) + "\n").c_str());
			data.my_log.AddLog("Waiting To Receive Init Signal...\n");
			
			if (arduP.checkForInit()) {
				data.my_log.AddLog("Successfully Received Init Signal\n");
			}
		}
		
		if (ImGui::Button("Print Edge")) {
			arduP.printEdge(data.selectedOriginalEdgeInd);
			//TODO UNSELECTABLE WHEN NOT CONNECTED
		}
		if (ImGui::ListBoxHeader("listBox 1")) {
			
			for (auto it = gh.originalEdges.begin(); it != gh.originalEdges.end(); it++)
			{
				const bool is_selected = (data.selectedOriginalEdgeInd == it->first);
				if (it->second.isDrawn)
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));

				if (ImGui::Selectable(std::to_string(it->first).c_str(), is_selected)) {
					data.selectedOriginalEdgeInd = it->first;
				}
				if (it->second.isDrawn)
					ImGui::PopStyleColor();

				//if (is_selected)
				//	ImGui::SetItemDefaultFocus();
				//TODO: bei anklicken auf den iterationsschritt springen wo die kante gebaut wird und die kante highlighten.ist eigentlich sekundär

			}
			ImGui::ListBoxFooter();

		}
	}


}