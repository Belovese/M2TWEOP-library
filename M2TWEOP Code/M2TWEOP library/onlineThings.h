#pragma once
#include <string>
#include <d3d9.h>
#include <filesystem>
#include "realGameTypes.h"
struct armyStruct;
using namespace std;
namespace battleCreator
{
	bool getIsGenerationNeeded();
	bool IsRetreatsOverhaulEnabled();

	void setCreationNeeded(bool isNeeded = false);

	void readParams();
	//for creation of battles and battles results
	//not threadsafe, but looks stable
	void startCreationThread();


	void onBattleResultsScreen();

	void onBattleStratScreen();
	void onHotseatScreen();
	void onPostBattleStratScreen();
	void draw(LPDIRECT3DDEVICE9 pDevice);


	void addCharactersToCustomBattleArmy(armyStruct* army, const std::string& relativePath);
	string selectWorldpkgdesc(const std::string &selectedRec, const std::string& selectedGroup);

	void OnLoadSettlementWorldpkgdesc(const std::string &selectedRec, const std::string& selectedGroup);

	string getWorldpkgDescFromBattleFile(const std::string& relativePath);
};


