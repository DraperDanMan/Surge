// ImGui Bezier widget. @r-lyeh, public domain
// v1.03: improve grabbing, confine grabbers to area option, adaptive size, presets, preview.
// v1.02: add BezierValue(); comments; usage
// v1.01: out-of-bounds coord snapping; custom border width; spacing; cosmetics
// v1.00: initial version
//
// [ref] http://robnapier.net/faster-bezier
// [ref] http://easings.net/es#easeInSine
//
// Usage:
// {  static float v[5] = { 0.390f, 0.575f, 0.565f, 1.000f }; 
//    ImGui::Bezier( "easeOutSine", v );       // draw
//    float y = ImGui::BezierValue( 0.5f, v ); // x delta in [0..1] range
// }

#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <ctime>

namespace ImGui
{
    template <int steps>
    void bezier_table( ImVec2 P[4], ImVec2 results[steps + 1] )
    {
        static float c[(steps + 1) * 4];
        static float *k = nullptr;
        if ( !k )
        {
            k = c;
            for ( unsigned step = 0; step <= steps; ++step )
            {
                const float t = static_cast<float>( step ) / static_cast<float>( steps );
                c[step * 4 + 0] = (1 - t) * (1 - t) * (1 - t); // * P0
                c[step * 4 + 1] = 3 * (1 - t) * (1 - t) * t; // * P1
                c[step * 4 + 2] = 3 * (1 - t) * t * t; // * P2
                c[step * 4 + 3] = t * t * t; // * P3
            }
        }
        for ( unsigned step = 0; step <= steps; ++step )
        {
            const ImVec2 point = {
                k[step * 4 + 0] * P[0].x + k[step * 4 + 1] * P[1].x + k[step * 4 + 2] * P[2].x + k[step * 4 + 3] * P[3].
                x,
                k[step * 4 + 0] * P[0].y + k[step * 4 + 1] * P[1].y + k[step * 4 + 2] * P[2].y + k[step * 4 + 3] * P[3].
                y
            };
            results[step] = point;
        }
    }

    inline float BezierValue( const float dt01, float p[4] )
    {
        constexpr int STEPS = 256;
        ImVec2 q[4] = { { 0, 0 }, { p[0], p[1] }, { p[2], p[3] }, { 1, 1 } };
        ImVec2 results[STEPS + 1];
        bezier_table<STEPS>( q, results );
        return results[static_cast<int>( ( dt01 < 0 ? 0 : dt01 > 1 ? 1 : dt01 ) * static_cast<int>( STEPS ) )].y;
    }

    inline bool Bezier( const char* label, float P[5] )
    {
        // visuals
        constexpr int  SMOOTHNESS = 64; // curve smoothness: the higher number of segments, the smoother curve
        constexpr int CURVE_WIDTH = 4; // main curved line width
        constexpr int LINE_WIDTH = 1; // handlers: small lines width
        constexpr int GRAB_RADIUS = 8; // handlers: circle radius
        constexpr int GRAB_BORDER = 2; // handlers: circle border width
        constexpr bool AREA_CONSTRAINED = true; // should grabbers be constrained to grid area?
        constexpr int AREA_WIDTH = 128; // area width in pixels. 0 for adaptive size (will use max avail width)

        // curve presets
        static struct
        {
            const char* name;
            float points[4];
        } presets[] = {
                { "Linear", { 0.000f, 0.000f, 1.000f, 1.000f } },

                { "In Sine", { 0.470f, 0.000f, 0.745f, 0.715f } },
                { "In Quad", { 0.550f, 0.085f, 0.680f, 0.530f } },
                { "In Cubic", { 0.550f, 0.055f, 0.675f, 0.190f } },
                { "In Quart", { 0.895f, 0.030f, 0.685f, 0.220f } },
                { "In Quint", { 0.755f, 0.050f, 0.855f, 0.060f } },
                { "In Expo", { 0.950f, 0.050f, 0.795f, 0.035f } },
                { "In Circ", { 0.600f, 0.040f, 0.980f, 0.335f } },

                { "Out Sine", { 0.390f, 0.575f, 0.565f, 1.000f } },
                { "Out Quad", { 0.250f, 0.460f, 0.450f, 0.940f } },
                { "Out Cubic", { 0.215f, 0.610f, 0.355f, 1.000f } },
                { "Out Quart", { 0.165f, 0.840f, 0.440f, 1.000f } },
                { "Out Quint", { 0.230f, 1.000f, 0.320f, 1.000f } },
                { "Out Expo", { 0.190f, 1.000f, 0.220f, 1.000f } },
                { "Out Circ", { 0.075f, 0.820f, 0.165f, 1.000f } },

                { "InOut Sine", { 0.445f, 0.050f, 0.550f, 0.950f } },
                { "InOut Quad", { 0.455f, 0.030f, 0.515f, 0.955f } },
                { "InOut Cubic", { 0.645f, 0.045f, 0.355f, 1.000f } },
                { "InOut Quart", { 0.770f, 0.000f, 0.175f, 1.000f } },
                { "InOut Quint", { 0.860f, 0.000f, 0.070f, 1.000f } },
                { "InOut Expo", { 1.000f, 0.000f, 0.000f, 1.000f } },
                { "InOut Circ", { 0.785f, 0.135f, 0.150f, 0.860f } },
            };


        // preset selector

        bool reload = false;
        PushID( label );
        if ( ArrowButton( "##lt", ImGuiDir_Left ) )
        {
            if ( --P[4] >= 0 ) reload = true;
            else ++P[4];
        }
        SameLine();

        if ( Button( "Presets" ) )
        {
            OpenPopup( "!Presets" );
        }
        if ( BeginPopup( "!Presets" ) )
        {
            for ( int i = 0; i < IM_ARRAYSIZE( presets ); ++i )
            {
                if ( i == 1 || i == 8 || i == 15 ) ImGui::Separator();
                if ( ImGui::MenuItem( presets[i].name, nullptr, static_cast<int>( P[4] ) == i ) )
                {
                    P[4] = static_cast<float>( i );
                    reload = true;
                }
            }
            EndPopup();
        }
        SameLine();

        if ( ArrowButton( "##rt", ImGuiDir_Right ) )
        {
            if ( ++P[4] < IM_ARRAYSIZE( presets ) ) reload = true;
            else --P[4];
        }
        SameLine();
        PopID();

        bool changed = false;

        if ( reload )
        {
            memcpy( P, presets[static_cast<int>( P[4] )].points, sizeof( float ) * 4 );
            changed = true;
        }

        // bezier widget

        const ImGuiStyle& Style = GetStyle();
        ImDrawList* draw_list = GetWindowDrawList();
        ImGuiWindow* window = GetCurrentWindow();
        if ( window->SkipItems )
        {
            return false;
        }

        // header and spacing
        changed |= SliderFloat4( label, P, 0, 1, "%.3f", 1.0f );
        int hovered = IsItemActive() || IsItemHovered();
        Dummy( ImVec2( 0, 3 ) );

        // prepare canvas
        const float avail = GetContentRegionMax().x;
        // ReSharper disable once CppUnreachableCode
        const float dim = AREA_WIDTH > 0 ? AREA_WIDTH : avail;
        const ImVec2 canvas( dim, dim );

        const ImRect bb( window->DC.CursorPos, window->DC.CursorPos + canvas );
        ItemSize( bb );
        if ( !ItemAdd( bb, NULL ) )
        {
            return changed;
        }

        const ImGuiID id = window->GetID( label );
        hovered |= 0 != ItemHoverable( ImRect( bb.Min, bb.Min + ImVec2( avail, dim ) ), id );

        RenderFrame( bb.Min, bb.Max, GetColorU32( ImGuiCol_FrameBg, 1 ), true, Style.FrameRounding );

        // background grid
        const int canvasX = static_cast<int>( canvas.x );
        const int canvasXInc = static_cast<int>( canvas.x / 4 );
        for ( int i = 0; i <= canvasX; i += canvasXInc )
        {
            const auto val = static_cast<float>( i );
            draw_list->AddLine(
                ImVec2( bb.Min.x + val, bb.Min.y ),
                ImVec2( bb.Min.x + val, bb.Max.y ),
                GetColorU32( ImGuiCol_TextDisabled ) );
        }
        const int canvasY = static_cast<int>( canvas.y );
        const int canvasYInc = static_cast<int>( canvas.y / 4 );
        for ( int i = 0; i <= canvasY; i += canvasYInc )
        {
            const auto val = static_cast<float>( i );
            draw_list->AddLine(
                ImVec2( bb.Min.x, bb.Min.y + val ),
                ImVec2( bb.Max.x, bb.Min.y + val ),
                GetColorU32( ImGuiCol_TextDisabled ) );
        }

        // eval curve
        ImVec2 Q[4] = { { 0, 0 }, { P[0], P[1] }, { P[2], P[3] }, { 1, 1 } };
        ImVec2 results[SMOOTHNESS + 1];
        bezier_table<SMOOTHNESS>( Q, results );

        // control points: 2 lines and 2 circles
        {
            const ImVec2 mouse = GetIO().MousePos;
            // handle grabbers
            ImVec2 pos[2];
            float distance[2];

            for ( int i = 0; i < 2; ++i )
            {
                pos[i] = ImVec2( P[i * 2 + 0], 1 - P[i * 2 + 1] ) * ( bb.Max - bb.Min ) + bb.Min;
                distance[i] = ( pos[i].x - mouse.x ) * ( pos[i].x - mouse.x ) + ( pos[i].y - mouse.y ) * ( pos[i].y - mouse.y );
            }

            const int selected = distance[0] < distance[1] ? 0 : 1;
            if ( distance[selected] < (4 * GRAB_RADIUS * 4 * GRAB_RADIUS) )
            {
                SetTooltip( "(%4.3f, %4.3f)", static_cast<double>( P[selected * 2 + 0] ), static_cast<double>( P[selected * 2 + 1] ) );

                if ( IsMouseClicked( 0 ) || IsMouseDragging( 0 ) )
                {
                    float& px = P[selected * 2 + 0] += GetIO().MouseDelta.x / canvas.x;
                    float& py = P[selected * 2 + 1] -= GetIO().MouseDelta.y / canvas.y;

                    if constexpr ( AREA_CONSTRAINED )
                    {
                        px = (px < 0 ? 0 : (px > 1 ? 1 : px));
                        py = (py < 0 ? 0 : (py > 1 ? 1 : py));
                    }

                    changed = true;
                }
            }
        }
        
        // draw curve
        {
            const ImColor color( GetStyle().Colors[ImGuiCol_PlotLines] );
            for ( int i = 0; i < SMOOTHNESS; ++i )
            {
                const ImVec2 p = { results[i + 0].x, 1 - results[i + 0].y };
                const ImVec2 q = { results[i + 1].x, 1 - results[i + 1].y };
                ImVec2 r( p.x * ( bb.Max.x - bb.Min.x ) + bb.Min.x, p.y * ( bb.Max.y - bb.Min.y ) + bb.Min.y );
                ImVec2 s( q.x * ( bb.Max.x - bb.Min.x ) + bb.Min.x, q.y * ( bb.Max.y - bb.Min.y ) + bb.Min.y );
                draw_list->AddLine( r, s, color, CURVE_WIDTH );
            }
        }

        const ImVec4 white( GetStyle().Colors[ImGuiCol_Text] );

        // draw lines and grabbers
        const float luma = IsItemActive() || IsItemHovered() ? 0.5f : 1.0f;
        const ImVec4 pink( 1.00f, 0.00f, 0.75f, luma );
        const ImVec4 cyan( 0.00f, 0.75f, 1.00f, luma );
        const ImVec2 p1 = ImVec2( P[0], 1 - P[1] ) * ( bb.Max - bb.Min ) + bb.Min;
        const ImVec2 p2 = ImVec2( P[2], 1 - P[3] ) * ( bb.Max - bb.Min ) + bb.Min;
        draw_list->AddLine( ImVec2( bb.Min.x, bb.Max.y ), p1, ImColor( white ), LINE_WIDTH );
        draw_list->AddLine( ImVec2( bb.Max.x, bb.Min.y ), p2, ImColor( white ), LINE_WIDTH );
        draw_list->AddCircleFilled( p1, GRAB_RADIUS, ImColor( white ) );
        draw_list->AddCircleFilled( p1, GRAB_RADIUS - GRAB_BORDER, ImColor( pink ) );
        draw_list->AddCircleFilled( p2, GRAB_RADIUS, ImColor( white ) );
        draw_list->AddCircleFilled( p2, GRAB_RADIUS - GRAB_BORDER, ImColor( cyan ) );
        
        return changed;
    }

    inline void ShowBezierDemo()
    {
        {
            static float v[5] = { 0.950f, 0.050f, 0.795f, 0.035f };
            Bezier( "easeInExpo", v );
        }
    }
}
