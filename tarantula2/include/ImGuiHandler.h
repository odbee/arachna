#pragma once
#include "GraphSetup.h"
#include "GraphFunctions.h"
#include "CachedData.h"
class ImGuiHandler
{
	public:
		void setupImGui() {

			ImGui::Initialize();
			ImGuiIO& io = ImGui::GetIO();
			ImGui::StyleColorsDark();
			//io.Fonts->AddFontFromFileTTF("../resources/fonts/Abordage-Regular.ttf", 14);
			//io.Fonts->AddFontFromFileTTF("../resources/fonts/Karrik-Regular.ttf", 15);

			////ImGui::GetStyle().WindowRounding = 20.0f;// <- Set this on init or use ImGui::PushStyleVar()
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

		void drawEdgesDiagram() {
			bool isopen;

			ImGui::Begin("STUFS", &isopen, /*ImGuiWindowFlags_NoTitleBar +*/ ImGuiWindowFlags_NoBackground + ImGuiWindowFlags_NoScrollbar + ImGuiWindowFlags_NoResize);

			//	static float xs[51], ys1[51], ys2[51];
			//for (int i = 0; i < 51; ++i) {
			//	xs[i] = i * 0.02;
			//	ys1[i] = 1.0 + 0.5 * sin(25 * xs[i]) * cos(2 * xs[i]);
			//	ys2[i] = 0.5 + 0.25 * sin(10 * xs[i]) * sin(xs[i]);
			//}
			//ImGui::PlotLines("frames", xs, IM_ARRAYSIZE(xs));
			//PlotEdges("", ys1, xs, IM_ARRAYSIZE(xs), 0, 0, 0, 1, { 600,300 });
			//
			tie(ei, eiend) = boost::edges(g);
			auto num_edges = std::distance(ei, eiend);
			vector<float> lengths(num_edges);
			vector<float> tensions(num_edges);
			int  indekx = 0;

			for (tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {

				//console() << currentLengthPm[*ei] << endl;
				lengths[indekx] = currentLengthPm[*ei];

				tensions[indekx] = currentLengthPm[*ei] / restLengthPm[*ei];
				//console() << tensions[indekx] << endl;
				indekx++;

				//console() << to_string(tensionCols[indekx]) << endl;

				//myfile << "[" << "(" << to_string(position[boost::source(*ei, g)]) << ");(" << to_string(position[boost::target(*ei, g)]) << ")" << "]" << endl;
				//if (!forbiddenPm[*ei]) {
				//	myfile << "[" << "{" << to_string(position[boost::source(*ei, g)]) << "};{" << to_string(position[boost::target(*ei, g)]) << "}" << "]" << endl;
				//}
			}
			//console() << lengths[32] << endl;

			//PlotEdges2("framess", lengths, tensions, 0, 0, FLT_MAX, { 600,300 });
			PlotGraphEdges("framess", g, 0, 0, FLT_MAX, { 600,300 });

			ImGui::End();
		}
		void drawParametersWindow(IniHandler iniHand) {
			bool isopen;

			ImGui::Begin("Params", &isopen, ImGuiWindowFlags_NoTitleBar + ImGuiWindowFlags_NoBackground + ImGuiWindowFlags_NoScrollbar + ImGuiWindowFlags_NoResize);

			//ImGui::StyleColorsLight();


			if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Text("number of iterations = %i", iterationcounter);



				ImGui::SliderFloat("density", &G_density, 0.0001f, .999f, "%.2f");
				ImGui::SliderFloat("length", &G_length, 0.0001f, .999f, "%.2f");
				ImGui::SliderFloat("tension", &G_tension, 0.0001f, .999f, "%.2f");

				ImGui::InputInt("x iterations", &x_iterations, 50, 200);
				if (ImGui::Button("run x iterations")) {
					runxiterations(x_iterations, g, relaxc, cycles);

				}


				checkforchange(vallist, cachelist);
				ImGui::Checkbox("check for forbidden edges", &CHECKFORBIDDEN);
				if (ImGui::Button("Reset web")) {
					console() << "resetting web" << endl;
					resetweb(g, relaxc, cycles);
				}

			}

			ImGui::Dummy(ImVec2(0.0f, 20.0f));



			if (ImGui::CollapsingHeader(" Debug Settings"), ImGuiTreeNodeFlags_DefaultOpen) {
				if (highlightEdge) {
					ImGui::Text("highlighted edge:");
					ImGui::Text("isforbidden?");
	

					ImGui::Text((g[highlightedEdge].isforbidden) ? "true" : "false");
				}
				ImGui::InputInt("cycle number", &displayCycle_i);
				ImGui::InputInt("edge number", &EDGEINDEX);
				if (ImGui::InputText("import directory path ", &dirPath)) {
					iniHand.overwriteTagImGui("graph directory", dirPath);
				}

				OBJFILENAME = dirPath + INITIALGRAPHDIRECTORYEXTENSION;
				CYCLESFILENAME = dirPath + CYCLESDIRECTORYEXTENSION;
				VOXELFILENAME = dirPath + VOXELSDIRECTORYEXTENSION;
				ANCHORPOINTSFILENAME = dirPath + ANCHORPOINTSDIRECTORYEXTENSION;
				ImGui::InputText("export filename: ", &EXPORTTITLE);
				//if (displayCycle_i >= cycles.size()) {
				//	displayCycle_i = displayCycle_i % cycles.size();
				//}
				if (ImGui::Button("Load Graph")) {
					InitialWebFromObj(&g, relaxc, OBJFILENAME, cycles);

				}
				if (ImGui::Button("Load Cycles")) {
					loadcycles(&g, cycles, CYCLESFILENAME);
					addCyclesToVertices(&g, cycles);
				}

				ImGui::Checkbox("Draw Vertices", &mDrawVertices);

				ImGui::SliderFloat("Alpha Value", &ALPHA, 0.0001f, .999f, "%.2f");


				ImGui::Checkbox("RELAX", &RUNRELAX);
				ImGui::Checkbox("Carousel", &CAROUSEL);
				ImGui::Checkbox("Run Through Cycles", &RUNCYCLES);

				ImGui::Checkbox("Draw Edge Numbers", &mDrawNumbers);
				ImGui::Checkbox("Draw Vertex Info", &mDrawVertexInfo);

				ImGui::Checkbox("highlight Nth Cycle", &drawNCycle);
				ImGui::Checkbox("Color Edge Indices", &colorEdges);
				ImGui::Checkbox("Color Edge Tensions", &colorTens);
				ImGui::Checkbox("Color Edge Lengths", &colorLength);

			}
			ImGui::End();
		}
};

