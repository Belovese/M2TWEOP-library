#include "pch.h"
#include "m2tweopMap.h"
#include "dataOffsets.h"
#include "strategyMap.h"

void m2tweopMap::buildMap()
{
	tiles.clear();
	borders.clear();

	gameDataAllStruct* gameDataAll = reinterpret_cast<gameDataAllStruct*>(dataOffsets::offsets.gameDataAllOffset);
	stratMap* tilesMap = gameDataAll->stratMap;
	mapWidth = tilesMap->mapWidth;
	mapHeight = tilesMap->mapHeight;

	for (int x = 0; x < mapWidth; x++)
	{
		for (int y = 0; y < mapHeight; y++)
		{
			shared_ptr< m2tweopTile >newTile = std::shared_ptr<m2tweopTile>(new m2tweopTile);

			newTile->buildTile(x, y);


			tiles.push_back(std::move(newTile));
		}
	}


	for (int x = 1; x < mapWidth - 1; x++)
	{
		for (int y = 1; y < mapHeight - 1; y++)
		{
			auto currTile = tiles[x * mapHeight + y];

			auto leftTile = tiles[(x - 1) * mapHeight + y];
			auto rightTile = tiles[(x + 1) * mapHeight + y];
			auto upTile = tiles[x * mapHeight + y + 1];
			auto lowTile = tiles[x * mapHeight + y - 1];
			currTile->buildBorder(
				tiles[(x - 1) * mapHeight + y],
				tiles[(x + 1) * mapHeight + y],
				tiles[x * mapHeight + y + 1],
				tiles[x * mapHeight + y - 1]
			);
		}
	}
}

void m2tweopMap::drawMap(const ImVec2& size)
{
	borders.clear();


	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0,0 });
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, { 0,0 });
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);

	ImDrawList* drawList = ImGui::GetWindowDrawList();
	ImVec2 mapStartPos = ImGui::GetWindowPos();
	mapStartPos.x -= ImGui::GetScrollX();
	mapStartPos.y -= ImGui::GetScrollY();


	for (int y = mapHeight - 1; y >= 0; y--)
	{
		for (int x = 0; x < mapWidth; x++)
		{
			auto& currTile = tiles[x * mapHeight + y];
			bool isSelected = (selectedTile!=nullptr&&selectedTile->IsSameCoords(currTile));
			if (true == currTile->drawTile(size, mapStartPos, &borders, isSelected))
			{
				selectedTile = tiles[x * mapHeight + y];
			}

			ImGui::SameLine();
		}

		ImGui::Dummy(size);

	}

	if(needDrawBorders==true)
	{ 
		for (size_t i = 0; i < borders.size()/2; i++)
		{
			drawList->AddLine(borders[i*2], borders[i*2+1], IM_COL32(200, 200, 200, 255), 0.2f * size.y);
		}
	}

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}

void m2tweopMap::drawInteract()
{
	const ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_SpanAvailWidth;
	if (ImGui::TreeNodeEx("Map settings", nodeFlags) == true)
	{
		ImGui::Checkbox("Draw borders", &needDrawBorders);


		ImGui::TreePop();
	}
	if (ImGui::TreeNodeEx("Selected tile", nodeFlags) == true)
	{
		if (selectedTile != nullptr)
		{
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				oneTile* tile = stratMapHelpers::getTile(selectedTile->xTile, selectedTile->yTile);
				ImGui::Text("Tile pointer:%p", reinterpret_cast<DWORD>(tile));
				ImGui::InputScalar("X coordinate", ImGuiDataType_U32, &selectedTile->xTile, nullptr, nullptr, "%u");
				ImGui::InputScalar("Y coordinate", ImGuiDataType_U32, &selectedTile->yTile, nullptr, nullptr, "%u");
				ImGui::Separator();
				ImGui::PopStyleVar();
		}
		ImGui::TreePop();
	}

}

