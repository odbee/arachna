#include "GuiSetup.h"




void GuiHandler::setup() {
	
}

void GuiHandler::adjustothers(std::vector<float*>&values, std::vector<float*>&cachedvalues, size_t index) {
	//adjusts the other values of the slider when one is changed.

	float factor = *cachedvalues[index] - *values[index];
	float sum = 0.0f;
	//factor /= values.size()-1;
	float multfactor;
	for (size_t i = 0; i < values.size(); i++)
	{
		if (i != index) {

			sum += *values[i];
		}
	}


	for (size_t i = 0; i < values.size(); i++)
	{
		if (i != index) {
			multfactor = (*values[i]) / sum;

			*values[i] += factor * multfactor;
		}
	}
	for (size_t i = 0; i < values.size(); i++) {
		const float uu = *values[i];
		*cachedvalues[i] = uu;
	}
}

std::string GuiHandler::makePath(std::string directory, std::string filename) {
	//make path joining input text and the absolute directory of the folder where the object resides.
	return directory  + filename;
}

void GuiHandler::checkforchange(std::vector<float*>& values, std::vector<float*>& cachedvalues) {
	float  val, cachedval;
	//console() << "checking " << endl;
	for (size_t i = 0; i < values.size(); i++)
	{
		//console() << "checking " << endl;

		val = *values[i];
		cachedval = *cachedvalues[i];

		if (val != cachedval)
		{
			adjustothers(values, cachedvalues, i);

		}
	}

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
		//}
		ImGui::Checkbox(" highlight Nth cycle",&data.highlightCycle);
		ImGui::SameLine();
		ImGui::PushItemWidth(80);
		ImGui::InputInt("N", &data.nthCycle);
		ImGui::PopItemWidth();
		ImGui::Separator();
		//if (data.highlightEdge) {
		//	ImGui::Text("highlighted edge:");
		//	ImGui::Text("isforbidden?");
		//	

		//	ImGui::Text((data.graph[data.highlightedEdge].isforbidden) ? "true" : "false");
		//}
	}
	
	ImGui::End();

}
