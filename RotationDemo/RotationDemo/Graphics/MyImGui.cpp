#include "MyImGui.h"
#include <iomanip>
#include <sstream>
#include <algorithm>

void MyImGui::DrawChart(std::vector<ChartData> data, ImVec2 min_range, ImVec2 max_range) {

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
	ImVec2 canvas_size = ImGui::GetContentRegionAvail();
	float min_size = 200;
	if (canvas_size.x < min_size) canvas_size.x = min_size;
	if (canvas_size.y < min_size) canvas_size.y = min_size;

	draw_list->AddRectFilledMultiColor(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(50, 50, 50, 255), IM_COL32(50, 50, 60, 255), IM_COL32(60, 60, 70, 255), IM_COL32(50, 50, 60, 255));
	draw_list->AddRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(200, 200, 200, 200));

	float offset = 45;
	float small_offset = 20;
	float x_axis_length = canvas_size.x - offset - small_offset;
	float y_axis_length = canvas_size.y - offset - small_offset;
	ImVec2 coords_start = ImVec2(canvas_pos.x + offset, canvas_pos.y + canvas_size.y - offset);

	draw_list->AddLine(coords_start, ImVec2(coords_start.x, coords_start.y - y_axis_length), IM_COL32(0, 0, 0, 255), 1.0f);
	draw_list->AddLine(coords_start, ImVec2(coords_start.x + x_axis_length, coords_start.y), IM_COL32(0, 0, 0, 255), 1.0f);

	int big_unit_offset = 40;
	int unit_number_x = std::floor(x_axis_length / big_unit_offset);
	int unit_number_y = std::floor(y_axis_length / big_unit_offset);

	float big_unit_line = 10;
	float middle_unit_line = 7;
	float small_unit_line = 4;

	float big_unit_value_x = (max_range.x - min_range.x) / unit_number_x;
	float big_unit_value_y = (max_range.y - min_range.y) / unit_number_y;

	float font_size = 13;

	for (int i = 0; i < unit_number_x; i++)
	{
		ImVec2 start, end;


		for (int j = 1; j < 10; j++)
		{
			end = ImVec2(coords_start.x + big_unit_offset * (i + j / 10.0), coords_start.y);
			start = ImVec2(end.x, end.y + small_unit_line);
			draw_list->AddLine(start, end, IM_COL32(0, 0, 0, 255), 1.0f);
		}

		end = ImVec2(coords_start.x + big_unit_offset * (i + 0.5), coords_start.y);
		start = ImVec2(end.x, end.y + middle_unit_line);
		draw_list->AddLine(start, end, IM_COL32(0, 0, 0, 255), 1.0f);

		if (i == 0) {
			end = ImVec2(coords_start.x, coords_start.y);
			start = ImVec2(end.x, end.y + big_unit_line);
			draw_list->AddLine(start, end, IM_COL32(0, 0, 0, 255), 1.0f);

			float value = min_range.x;
			std::stringstream stream;
			stream << std::fixed << std::setprecision(1) << value;
			draw_list->AddText(NULL, font_size, ImVec2(end.x - 15, end.y + 15), IM_COL32(255, 255, 255, 255), stream.str().c_str());
		}

		end = ImVec2(coords_start.x + big_unit_offset * (i + 1), coords_start.y);
		start = ImVec2(end.x, end.y + big_unit_line);
		draw_list->AddLine(start, end, IM_COL32(0, 0, 0, 255), 1.0f);
		draw_list->AddLine(end, ImVec2(end.x, end.y - y_axis_length), IM_COL32(80, 80, 80, 255), 1.0f);

		float value = (i + 1) * big_unit_value_x + min_range.x;
		std::stringstream stream;
		stream << std::fixed << std::setprecision(1) << value;
		draw_list->AddText(NULL, font_size, ImVec2(end.x - 15, end.y + 15), IM_COL32(255, 255, 255, 255), stream.str().c_str());
	}

	for (int i = 0; i < unit_number_y; i++)
	{
		ImVec2 start, end;

		for (int j = 1; j < 10; j++)
		{
			end = ImVec2(coords_start.x, coords_start.y - big_unit_offset * (i + j / 10.0));
			start = ImVec2(end.x - small_unit_line, end.y);
			draw_list->AddLine(start, end, IM_COL32(0, 0, 0, 255), 1.0f);
		}

		end = ImVec2(coords_start.x, coords_start.y - big_unit_offset * (i + 0.5));
		start = ImVec2(end.x - middle_unit_line, end.y);
		draw_list->AddLine(start, end, IM_COL32(0, 0, 0, 255), 1.0f);

		if (i == 0) {
			end = ImVec2(coords_start.x, coords_start.y);
			start = ImVec2(end.x - big_unit_line, end.y);
			draw_list->AddLine(start, end, IM_COL32(0, 0, 0, 255), 1.0f);

			float value = min_range.y;
			std::stringstream stream;
			stream << std::fixed << std::setprecision(1) << value;
			draw_list->AddText(NULL, font_size, ImVec2(end.x - offset + 3, end.y - 10), IM_COL32(255, 255, 255, 255), stream.str().c_str());
		}

		end = ImVec2(coords_start.x, coords_start.y - big_unit_offset * (i + 1));
		start = ImVec2(end.x - big_unit_line, end.y);
		draw_list->AddLine(start, end, IM_COL32(0, 0, 0, 255), 1.0f);
		draw_list->AddLine(end, ImVec2(end.x + x_axis_length, end.y), IM_COL32(80, 80, 80, 255), 1.0f);

		float value = (i + 1) * big_unit_value_y + min_range.y;
		std::stringstream stream;
		stream << std::fixed << std::setprecision(1) << value;
		draw_list->AddText(NULL, font_size, ImVec2(end.x - offset + 3, end.y - 10), IM_COL32(255, 255, 255, 255), stream.str().c_str());
	}


	canvas_size = ImGui::GetContentRegionAvail();
	ImVec2 clip_start = ImVec2(coords_start.x, coords_start.y - y_axis_length);
	ImVec2 clip_end1 = ImVec2(coords_start.x + x_axis_length, coords_start.y);
	ImVec2 clip_end2 = ImVec2(canvas_pos.x + canvas_size.x + ImGui::GetStyle().WindowPadding.x - 4, canvas_pos.y + canvas_size.y + ImGui::GetStyle().WindowPadding.y);
	ImVec2 clip_end = ImVec2(std::min(clip_end1.x, clip_end2.x), std::min(clip_end1.y, clip_end2.y));
	draw_list->PushClipRect(clip_start, clip_end, false);

	ImVec2 range_end = ImVec2(coords_start.x + unit_number_x * big_unit_offset, coords_start.y - unit_number_y * big_unit_offset);

	for (auto& cd : data) {
		for (int i = 0; i < (int)cd.data->size() - 1; i++)// funny size_t which cannot be less than 0 so size_t(0)-1 = max val of size_t
		{
			int a = cd.data->size() - 1;
			float start_x = (cd.data->at(i).x - min_range.x) / (max_range.x - min_range.x) * (range_end.x - coords_start.x) + coords_start.x;
			float start_y = (cd.data->at(i).y - min_range.y) / (max_range.y - min_range.y) * (range_end.y - coords_start.y) + coords_start.y;
			float end_x = (cd.data->at(i + 1).x - min_range.x) / (max_range.x - min_range.x) * (range_end.x - coords_start.x) + coords_start.x;
			float end_y = (cd.data->at(i + 1).y - min_range.y) / (max_range.y - min_range.y) * (range_end.y - coords_start.y) + coords_start.y;
			draw_list->AddLine(ImVec2(start_x, start_y), ImVec2(end_x, end_y), cd.color, 1.5f);
		}
	}

	draw_list->PopClipRect();

}