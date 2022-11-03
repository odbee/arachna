#pragma once
#include "cinder/app/App.h"

#include <quickhull/QuickHull.hpp>"


#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>




struct EdgeProperties
{
	float restlength;
	float currentlength;
	int index;
	bool isforbidden;
	std::vector<std::array<float, 2>> densityparams;
	float probability;
};

struct VertexProperties
{
	std::size_t index;
	bool isfixed;
	ci::vec3 pos;
	ci::vec3 movevec;
	std::vector<size_t> cycles;
};

struct GraphProperties {
	int currentindex;
};

typedef boost::adjacency_list<boost::vecS,
	boost::vecS, boost::undirectedS, VertexProperties, EdgeProperties> Graph;
Graph g;


boost::property_map< Graph, ci::vec3 VertexProperties::* >::type position = boost::get(&VertexProperties::pos, g);
boost::property_map< Graph, bool VertexProperties::* >::type fixedBool = boost::get(&VertexProperties::isfixed, g);
boost::property_map< Graph, ci::vec3 VertexProperties::* >::type moveVecPm = boost::get(&VertexProperties::movevec, g);
boost::property_map< Graph, std::vector<size_t> VertexProperties::* >::type cyclesPm = boost::get(&VertexProperties::cycles, g);


boost::property_map< Graph, float EdgeProperties::* >::type currentLengthPm = boost::get(&EdgeProperties::currentlength, g);
boost::property_map< Graph, float EdgeProperties::* >::type probabilityPm = boost::get(&EdgeProperties::probability, g);

boost::property_map< Graph, float EdgeProperties::* >::type restLengthPm = boost::get(&EdgeProperties::restlength, g);
boost::property_map< Graph, int EdgeProperties::* >::type indexPm = boost::get(&EdgeProperties::index, g);

boost::property_map< Graph, bool EdgeProperties::* >::type forbiddenPm = boost::get(&EdgeProperties::isforbidden, g);
boost::property_map< Graph, std::vector<std::array<float, 2>> EdgeProperties::* >::type densityPm = boost::get(&EdgeProperties::densityparams, g);


typedef boost::graph_traits<Graph>::vertex_descriptor vertex_t;

typedef Graph::edge_descriptor edge_t;
typedef boost::graph_traits<Graph>::edge_iterator edge_ti;

boost::graph_traits<Graph>::edge_iterator ei, eiend;


edge_t selected_edge, hovered_edge, empty_edge;
bool justclicked = false;
std::vector<std::vector<size_t>> cycles;
int cycleN;
int index_in_commoncyc = 0;
std::vector<size_t> commoncycles, empty_vector;
float f_closestDist;

std::string infoContent;
std::string dirPath = "web";


void clearcycles(Graph& g) {

}

struct ExampleAppLog
{
    ImGuiTextBuffer     Buf;

    ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
    bool                AutoScroll;  // Keep scrolling if already at the bottom.

    ExampleAppLog()
    {
        AutoScroll = true;
        Clear();
    }

    void    Clear()
    {
        Buf.clear();
        LineOffsets.clear();
        LineOffsets.push_back(0);
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        int old_size = Buf.size();
        va_list args;
        va_start(args, fmt);
        Buf.appendfv(fmt, args);
        va_end(args);
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                LineOffsets.push_back(old_size + 1);
    }

    void    Draw(const char* title, bool* p_open = NULL)
    {

        bool clear = ImGui::Button("Clear");
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::SameLine();


        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0, 50), false, ImGuiWindowFlags_HorizontalScrollbar);

        if (clear)
            Clear();
        if (copy)
            ImGui::LogToClipboard();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        const char* buf = Buf.begin();
        const char* buf_end = Buf.end();

        {
            ImGuiListClipper clipper;
            clipper.Begin(LineOffsets.Size);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char* line_start = buf + LineOffsets[line_no];
                    const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                    ImGui::TextUnformatted(line_start, line_end);
                }
            }
            clipper.End();
        }
        ImGui::PopStyleVar();

        if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();

    }
};

static ExampleAppLog my_log;
