#pragma once
#include "HelperFunctions.h"
#include "imgui/imgui_internal.h"



struct ImGuiPlotArrayGetterData
{
    const float* Values;
    int Stride;

    ImGuiPlotArrayGetterData(const float* values, int stride) { Values = values; Stride = stride; }
};

static float Plot_ArrayGetter(void* data, int idx)
{
    ImGuiPlotArrayGetterData* plot_data = (ImGuiPlotArrayGetterData*)data;
    const float v = *(const float*)(const void*)((const unsigned char*)plot_data->Values + (size_t)idx * plot_data->Stride);
    return v;
}



int PlotExx( const char* label, float (*values_getter)(void* data, int idx), void* data, void* colors, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 frame_size)
{

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return -1;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    if (frame_size.x == 0.0f)
        frame_size.x = ImGui::CalcItemWidth();
    if (frame_size.y == 0.0f)
        frame_size.y = label_size.y + (style.FramePadding.y * 2);

    const ImRect frame_bb(window->DC.CursorPos,
        {window->DC.CursorPos.x + frame_size.x,window->DC.CursorPos.y + frame_size.y});
    const ImRect inner_bb(
        { frame_bb.Min.x + style.FramePadding.x,frame_bb.Min.y + style.FramePadding.y },
        { frame_bb.Max.x - style.FramePadding.x,frame_bb.Max.y - style.FramePadding.y });
    const ImRect total_bb(
        { frame_bb.Min.x,frame_bb.Min.y },
        { frame_bb.Max.x + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0).x,
        frame_bb.Max.y + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0).y }
    );
    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, 0, &frame_bb))
        return -1;
    const bool hovered = ImGui::ItemHoverable(frame_bb, id);

    // Determine scale from values if not specified
    if (scale_min == FLT_MAX || scale_max == FLT_MAX)
    {
        float v_min = FLT_MAX;
        float v_max = -FLT_MAX;
        for (int i = 0; i < values_count; i++)
        {
            const float v = values_getter(data, i);
            if (v != v) // Ignore NaN values
                continue;
            v_min = ImMin(v_min, v);
            v_max = ImMax(v_max, v);
        }
        if (scale_min == FLT_MAX)
            scale_min = v_min;
        if (scale_max == FLT_MAX)
            scale_max = v_max;
    }

    ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

    const int values_count_min =  1;
    int idx_hovered = -1;
    if (values_count >= values_count_min)
    {
        int res_w = ImMin((int)frame_size.x, values_count) ;
        int item_count = values_count ;

        // Tooltip on hover
        if (hovered && inner_bb.Contains(g.IO.MousePos))
        {
            const float t = ImClamp((g.IO.MousePos.x - inner_bb.Min.x) / (inner_bb.Max.x - inner_bb.Min.x), 0.0f, 0.9999f);
            const int v_idx = (int)(t * item_count);
            IM_ASSERT(v_idx >= 0 && v_idx < values_count);

            const float v0 = values_getter(data, (v_idx + values_offset) % values_count);
            const float v1 = values_getter(data, (v_idx + 1 + values_offset) % values_count);
            ImGui::SetTooltip("%d: %8.4g", v_idx, v0);
            idx_hovered = v_idx;
        }

        const float t_step = 1.0f / (float)res_w;
        const float inv_scale = (scale_min == scale_max) ? 0.0f : (1.0f / (scale_max - scale_min));

        float v0 = values_getter(data, (0 + values_offset) % values_count);

        float t0 = 0.0f;
        ImVec2 tp0 = ImVec2(t0, 1.0f - ImSaturate((v0 - scale_min) * inv_scale));                       // Point in the normalized space of our target rectangle
        float histogram_zero_line_t = (scale_min * scale_max < 0.0f) ? (-scale_min * inv_scale) : (scale_min < 0.0f ? 0.0f : 1.0f);   // Where does the zero line stands

        //const ImU32 col_base = ImGui::GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLines : ImGuiCol_PlotHistogram);
        //const ImU32 col_hovered = ImGui::GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLinesHovered : ImGuiCol_PlotHistogramHovered);
        const ImU32 col_base = ImGui::GetColorU32( ImGuiCol_PlotHistogram);
        const ImU32 col_hovered = ImGui::GetColorU32( ImGuiCol_PlotHistogramHovered);
        
        for (int n = 0; n < res_w; n++)
        {
            float color = values_getter(colors, n);
            const float t1 = t0 + t_step;
            const int v1_idx = (int)(t0 * item_count + 0.5f);
            IM_ASSERT(v1_idx >= 0 && v1_idx < values_count);
            const float v1 = values_getter(data, (v1_idx + values_offset + 1) % values_count);
            const ImVec2 tp1 = ImVec2(t1, 1.0f - ImSaturate((v1 - scale_min) * inv_scale));

            // NB: Draw calls are merged together by the DrawList system. Still, we should render our batch are lower level to save a bit of CPU.
            ImVec2 pos0 = ImLerp(inner_bb.Min, inner_bb.Max, tp0);
            //ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, (plot_type == ImGuiPlotType_Lines) ? tp1 : ImVec2(tp1.x, histogram_zero_line_t));
            ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, ImVec2(tp1.x, histogram_zero_line_t));

            //if (plot_type == ImGuiPlotType_Lines)
            //{
            //    window->DrawList->AddLine(pos0, pos1, idx_hovered == v1_idx ? col_hovered : col_base);
            //}
            //else if (plot_type == ImGuiPlotType_Histogram)
            {
                if (pos1.x >= pos0.x + 2.0f)
                    pos1.x -= 1.0f;
                //window->DrawList->AddRectFilled(pos0, pos1, idx_hovered == v1_idx ? col_hovered : col_base);
                window->DrawList->AddRectFilled(pos0, pos1, idx_hovered == v1_idx ? col_hovered : ImGui::ColorConvertFloat4ToU32({ color,0.0f,0.0f,1.0f}));
            }

            t0 = t1;
            tp0 = tp1;
        }
    }

    // Text overlay
    if (overlay_text)
        ImGui::RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, overlay_text, NULL, NULL, ImVec2(0.5f, 0.0f));

    if (label_size.x > 0.0f)
        ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);

    // Return hovered index or -1 if none are hovered.
    // This is currently not exposed in the public API because we need a larger redesign of the whole thing, but in the short-term we are making it available in PlotEx().
    return idx_hovered;
}


int PlotVectors(const char* label, vector<float> data, vector<float> tensions, const char* overlay_text, float scale_min, float scale_max, ImVec2 frame_size)
{
    vector<float> vals=data;
    vector<float> cols=tensions;
    int values_count = vals.size();

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return -1;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    if (frame_size.x == 0.0f)
        frame_size.x = ImGui::CalcItemWidth();
    if (frame_size.y == 0.0f)
        frame_size.y =  (style.FramePadding.y * 2);

    const ImRect frame_bb(window->DC.CursorPos,
        { window->DC.CursorPos.x + frame_size.x,window->DC.CursorPos.y + frame_size.y });
    const ImRect inner_bb(
        { frame_bb.Min.x + style.FramePadding.x,frame_bb.Min.y + style.FramePadding.y },
        { frame_bb.Max.x - style.FramePadding.x,frame_bb.Max.y - style.FramePadding.y });
    const ImRect total_bb(
        { frame_bb.Min.x,frame_bb.Min.y },
         frame_bb.Max
    );
    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, 0, &frame_bb))
        return -1;
    const bool hovered = ImGui::ItemHoverable(frame_bb, id);

    // Determine scale from values if not specified
    if (scale_min == FLT_MAX || scale_max == FLT_MAX)
    {
        float v_min = FLT_MAX;
        float v_max = -FLT_MAX;
        for (int i = 0; i < values_count; i++)
        {
            const float v = vals[i];
            if (v != v) // Ignore NaN values
                continue;
            v_min = ImMin(v_min, v);
            v_max = ImMax(v_max, v);
        }
        if (scale_min == FLT_MAX)
            scale_min = v_min;
        if (scale_max == FLT_MAX)
            scale_max = v_max;
    }

    ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

    const int values_count_min = 1;
    int idx_hovered = -1;
    if (values_count >= values_count_min)
    {
        int res_w = ImMin((int)frame_size.x, values_count);
        int item_count = values_count;

        // Tooltip on hover
        if (hovered && inner_bb.Contains(g.IO.MousePos))
        {
            const float t = ImClamp((g.IO.MousePos.x - inner_bb.Min.x) / (inner_bb.Max.x - inner_bb.Min.x), 0.0f, 0.9999f);
            const int v_idx = (int)(t * item_count);
            IM_ASSERT(v_idx >= 0 && v_idx < values_count);

            const float v0 = vals[v_idx];
           
            ImGui::SetTooltip("%d: %8.4g", v_idx, v0);
            idx_hovered = v_idx;
        }

        const float t_step = 1.0f / (float)res_w;
        const float inv_scale = (scale_min == scale_max) ? 0.0f : (1.0f / (scale_max - scale_min));

        float v0 = vals[0];
        
        float t0 = 0.0f;
        ImVec2 tp0 = ImVec2(t0, 1.0f - ImSaturate((v0 - scale_min) * inv_scale));                       // Point in the normalized space of our target rectangle
        float histogram_zero_line_t = (scale_min * scale_max < 0.0f) ? (-scale_min * inv_scale) : (scale_min < 0.0f ? 0.0f : 1.0f);   // Where does the zero line stands

        //const ImU32 col_base = ImGui::GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLines : ImGuiCol_PlotHistogram);
        //const ImU32 col_hovered = ImGui::GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLinesHovered : ImGuiCol_PlotHistogramHovered);
        const ImU32 col_base = ImGui::GetColorU32(ImGuiCol_PlotHistogram);
        const ImU32 col_hovered = ImGui::GetColorU32(ImGuiCol_PlotHistogramHovered);
        
        
        // CALCULATE BAR WIDTH
        float barwidth = ImLerp(0.0f, inner_bb.Max.x-inner_bb.Min.x , t_step);
        int offset = (int)(floor(barwidth / 2) - 2);
        offset = (offset < 0) ? 0 : offset;
        


        //ImVec2 pos0 = ImLerp(inner_bb.Min, inner_bb.Max, tp0);
        ////ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, (plot_type == ImGuiPlotType_Lines) ? tp1 : ImVec2(tp1.x, histogram_zero_line_t));
        //ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, ImVec2(tp1.x, histogram_zero_line_t));
        // END CALCULATE BARD WIDTH
        
        for (int n = 0; n < res_w; n++)
        {

            
            const float t1 = t0 + t_step;
            const int v1_idx = (int)(t0 * item_count + 0.5f);
            IM_ASSERT(v1_idx >= 0 && v1_idx < values_count);
            //const float v1 = values_getter(data, (v1_idx + values_offset + 1) % values_count);
            float v1 = vals[v1_idx];
            float tension= cols[v1_idx];
            Color tensionCol = cRamp(tension);
            

            const ImVec2 tp1 = ImVec2(t1, 1.0f - ImSaturate((v1 - scale_min) * inv_scale));            //// NB: Draw calls are merged together by the DrawList system. Still, we should render our batch are lower level to save a bit of CPU.
            ImVec2 pos0 = ImLerp(inner_bb.Min, inner_bb.Max, tp0);
            //ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, (plot_type == ImGuiPlotType_Lines) ? tp1 : ImVec2(tp1.x, histogram_zero_line_t));
            ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, ImVec2(tp1.x, histogram_zero_line_t));

            //if (plot_type == ImGuiPlotType_Lines)
            //{
            //    window->DrawList->AddLine(pos0, pos1, idx_hovered == v1_idx ? col_hovered : col_base);
            //}
            //else if (plot_type == ImGuiPlotType_Histogram)
            {
                if (pos1.x >= pos0.x + 2.0f)
                    pos1.x -= 1.0f;
                //window->DrawList->AddRectFilled(pos0, pos1, idx_hovered == v1_idx ? col_hovered : col_base);
                window->DrawList->AddRectFilled({ pos0.x + offset,pos0.y }, { pos1.x- offset,pos1.y }, idx_hovered == v1_idx ? col_hovered : ImGui::ColorConvertFloat4ToU32({ tensionCol.r,tensionCol.g,tensionCol.b,1.0f }));
            }

            t0 = t1;
            tp0 = tp1;
        }
    }

    // Text overlay
    if (overlay_text)
        ImGui::RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, overlay_text, NULL, NULL, ImVec2(0.5f, 0.0f));

    // Return hovered index or -1 if none are hovered.
    // This is currently not exposed in the public API because we need a larger redesign of the whole thing, but in the short-term we are making it available in PlotEx().
    return idx_hovered;
}
struct edgeWithparams {
    edge_t descriptor;
    int index;
    float length;
    float tension;
    float density;
    bool isConnected;
    bool isNew;
};
bool sortbylength(const edgeWithparams& x, const edgeWithparams& y) {
    return x.length> y.length;
}


bool sortbytension(const edgeWithparams& x, const edgeWithparams& y) {
    return x.tension> y.tension;
}

template<class InputIterator, class T>
InputIterator findInStruct(InputIterator first, InputIterator last, const T& val)
{
    while (first != last) {
        if ((*first).descriptor == val) return first;
        ++first;
    }
    return last;
}



int PlotGraphEdges(const char* label, Graph &graph, const char* overlay_text, float scale_min, float scale_max, ImVec2 frame_size)
{
    tie(ei, eiend) = boost::edges(graph);
    auto num_edges = std::distance(ei, eiend);
    vector<edgeWithparams> edgelist(num_edges);
    int indekx = 0;
    for (tie(ei, eiend) = boost::edges(graph); ei != eiend; ++ei) {


        //console() << currentLengthPm[*ei] << endl;
        edgelist[indekx].length = currentLengthPm[*ei];
        edgelist[indekx].descriptor = *ei;
        edgelist[indekx].tension = currentLengthPm[*ei] / restLengthPm[*ei];
        edgelist[indekx].index = indekx;
        //console() << tensions[indekx] << endl;
        indekx++;

        
    }

    std::sort(edgelist.begin(), edgelist.end(), sortbylength);

    
    int values_count = num_edges;

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return -1;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    if (frame_size.x == 0.0f)
        frame_size.x = ImGui::CalcItemWidth();
    if (frame_size.y == 0.0f)
        frame_size.y = (style.FramePadding.y * 2);

    const ImRect frame_bb(window->DC.CursorPos,
        { window->DC.CursorPos.x + frame_size.x,window->DC.CursorPos.y + frame_size.y });
    const ImRect inner_bb(
        { frame_bb.Min.x + style.FramePadding.x,frame_bb.Min.y + style.FramePadding.y },
        { frame_bb.Max.x - style.FramePadding.x,frame_bb.Max.y - style.FramePadding.y });
    const ImRect total_bb(
        { frame_bb.Min.x,frame_bb.Min.y },
        frame_bb.Max
    );
    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, 0, &frame_bb))
        return -1;
    const bool hovered = ImGui::ItemHoverable(frame_bb, id);

    // Determine scale from values if not specified
    if (scale_min == FLT_MAX || scale_max == FLT_MAX)
    {
        float v_min = FLT_MAX;
        float v_max = -FLT_MAX;
        for (int i = 0; i < values_count; i++)
        {
            const float v = edgelist[i].length;
            if (v != v) // Ignore NaN values
                continue;
            v_min = ImMin(v_min, v);
            v_max = ImMax(v_max, v);
        }
        if (scale_min == FLT_MAX)
            scale_min = v_min;
        if (scale_max == FLT_MAX)
            scale_max = v_max;
    }

    ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
    int idx_selected = -1;
    const int values_count_min = 1;
    int idx_hovered = -1;
    int idx_connector=-1;

    if (values_count >= values_count_min)
    {
        int res_w = ImMin((int)frame_size.x, values_count);
        int item_count = values_count;

        // Tooltip on hover
        if (hovered && inner_bb.Contains(g.IO.MousePos))
        {
            const float t = ImClamp((g.IO.MousePos.x - inner_bb.Min.x) / (inner_bb.Max.x - inner_bb.Min.x), 0.0f, 0.9999f);
            const int v_idx = (int)(t * item_count);
            IM_ASSERT(v_idx >= 0 && v_idx < values_count);

            const float v0 = edgelist[v_idx].length;

            ImGui::SetTooltip("edge %d\n length: %8.4g \n tension: %8.4g ", edgelist[v_idx].index, edgelist[v_idx].length, edgelist[v_idx].tension);
            idx_hovered = v_idx;
            HOVERED = edgelist[idx_hovered].descriptor;
        }
        else {
            HOVERED = EMPTY;
        }

        const float t_step = 1.0f / (float)res_w;
        const float inv_scale = (scale_min == scale_max) ? 0.0f : (1.0f / (scale_max - scale_min));

        float v0 = edgelist[0].length;

        float t0 = 0.0f;
        ImVec2 tp0 = ImVec2(t0, 1.0f - ImSaturate((v0 - scale_min) * inv_scale));                       // Point in the normalized space of our target rectangle
        float histogram_zero_line_t = (scale_min * scale_max < 0.0f) ? (-scale_min * inv_scale) : (scale_min < 0.0f ? 0.0f : 1.0f);   // Where does the zero line stands

        //const ImU32 col_base = ImGui::GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLines : ImGuiCol_PlotHistogram);
        //const ImU32 col_hovered = ImGui::GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLinesHovered : ImGuiCol_PlotHistogramHovered);
        const ImU32 col_base = ImGui::GetColorU32(ImGuiCol_PlotHistogram);
        const ImU32 col_hovered = ImGui::GetColorU32(ImGuiCol_PlotHistogramHovered);
        



        // CALCULATE BAR WIDTH
        float barwidth = ImLerp(0.0f, inner_bb.Max.x - inner_bb.Min.x, t_step);
        float offset = (float)((barwidth / 2) - 1);
        offset = (offset < 0) ? 0 : offset;



        //ImVec2 pos0 = ImLerp(inner_bb.Min, inner_bb.Max, tp0);
        ////ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, (plot_type == ImGuiPlotType_Lines) ? tp1 : ImVec2(tp1.x, histogram_zero_line_t));
        //ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, ImVec2(tp1.x, histogram_zero_line_t));
        // END CALCULATE BARD WIDTH

        for (int n = 0; n < res_w; n++)
        {


            const float t1 = t0 + t_step;
            const int v1_idx = (int)(t0 * item_count + 0.5f);
            IM_ASSERT(v1_idx >= 0 && v1_idx < values_count);
            //const float v1 = values_getter(data, (v1_idx + values_offset + 1) % values_count);
            float v1 = edgelist[v1_idx].length;
            float tension = edgelist[v1_idx].tension;
            Color tensionCol = cRamp(tension);


            const ImVec2 tp1 = ImVec2(t1, 1.0f - ImSaturate((v1 - scale_min) * inv_scale));            //// NB: Draw calls are merged together by the DrawList system. Still, we should render our batch are lower level to save a bit of CPU.
            ImVec2 pos0 = ImLerp(inner_bb.Min, inner_bb.Max, tp0);
            //ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, (plot_type == ImGuiPlotType_Lines) ? tp1 : ImVec2(tp1.x, histogram_zero_line_t));
            ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, ImVec2(tp1.x, histogram_zero_line_t));

            //if (plot_type == ImGuiPlotType_Lines)
            //{
            //    window->DrawList->AddLine(pos0, pos1, idx_hovered == v1_idx ? col_hovered : col_base);
            //}
            //else if (plot_type == ImGuiPlotType_Histogram)
            {
                if (pos1.x >= pos0.x + 2.0f)
                    pos1.x -= 1.0f;
                //window->DrawList->AddRectFilled(pos0, pos1, idx_hovered == v1_idx ? col_hovered : col_base);
                if (idx_hovered==v1_idx){
                    window->DrawList->AddRectFilled({ pos0.x + offset,pos0.y }, { pos1.x - offset,pos1.y }, col_hovered );
                }
                else if (idx_selected == v1_idx) {
                    window->DrawList->AddRectFilled({ pos0.x + offset,pos0.y }, { pos1.x - offset,pos1.y }, ImGui::ColorConvertFloat4ToU32({ 0.5f,0.5f,0.82f,0.8f }));
                }
                else if (idx_connector == v1_idx) {
                    window->DrawList->AddRectFilled({ pos0.x + offset,pos0.y }, { pos1.x - offset,pos1.y }, ImGui::ColorConvertFloat4ToU32({ 0.65f,0.45f,0.82f,0.8f }));
                }

                else if (edgelist[v1_idx].isConnected) {
                    window->DrawList->AddRectFilled({ pos0.x + offset,pos0.y }, { pos1.x - offset,pos1.y }, ImGui::ColorConvertFloat4ToU32({ 1.0f, 0.5f, 0.0f,0.7f }));
                }
                else if (edgelist[v1_idx].isNew) {
                    window->DrawList->AddRectFilled({ pos0.x + offset,pos0.y }, { pos1.x - offset,pos1.y }, ImGui::ColorConvertFloat4ToU32({ 0.5f,0.5f,0.82f,0.8f }));
                }
                else {
                    window->DrawList->AddRectFilled({ pos0.x + offset,pos0.y }, { pos1.x - offset,pos1.y },  ImGui::ColorConvertFloat4ToU32({ tensionCol.r,tensionCol.g,tensionCol.b,1.0f }));
                }
                
            }

            t0 = t1;
            tp0 = tp1;
        }
    }

    // Text overlay
    if (overlay_text)
        ImGui::RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, overlay_text, NULL, NULL, ImVec2(0.5f, 0.0f));

    // Return hovered index or -1 if none are hovered.
    // This is currently not exposed in the public API because we need a larger redesign of the whole thing, but in the short-term we are making it available in PlotEx().
    return idx_hovered;
}


void PlotEdges(const char* label, const float* values, const float* colors , int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0), int stride = sizeof(float))
{
    ImGuiPlotArrayGetterData data(values, stride);
    ImGuiPlotArrayGetterData cols (colors, stride);

    PlotExx(label, &Plot_ArrayGetter, (void*)&data, (void*)&cols, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}
void PlotEdges2(const char* label,vector<float> values, vector<float> values2, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0), int stride = sizeof(float))
{
    
    PlotVectors(label, values, values2, overlay_text, scale_min, scale_max, graph_size);
}
