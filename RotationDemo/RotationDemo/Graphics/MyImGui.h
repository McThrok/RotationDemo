#pragma once

#pragma once
#include <vector>
#include "ImGUI\\imgui.h"
#include "ImGUI\\imgui_impl_win32.h"
#include "ImGUI\\imgui_impl_dx11.h"

using namespace std;

struct ChartData {
public:
	ChartData() {}
	ChartData(vector<ImVec2>* _data, ImU32 _color) :data(_data), color(_color) {}
	vector<ImVec2>* data;
	ImU32 color;
};

class MyImGui {
public:
	static void DrawChart(std::vector<ChartData> data, ImVec2 min_range, ImVec2 max_range);
};
