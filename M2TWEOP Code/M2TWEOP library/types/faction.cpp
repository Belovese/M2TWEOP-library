﻿///
//![Lua logo](../Lua.png)
//@module LuaPlugin
//@author Fynn
//@license GPL-3.0
#include "pch.h"
#include "faction.h"
#include "dataOffsets.h"
#include "gameHelpers.h"
#include "fort.h"
#include "unit.h"
#include "army.h"
#include "campaign.h"
#include "strategyMap.h"
#include "techFuncs.h"

#define factionStruct_ai_label 1
#define factionStruct_name 2

void factionStruct::revealTile(const int x, const int y)
{
	if (stratMapHelpers::isStratMap() == false)
		return;
	int coords[2] = {x, y};
	GAME_FUNC(int*(__thiscall*)(void*, int*, int, float), revealTile)(tilesFac, coords, 1, -1.0);
}

void factionStruct::setColor(const uint8_t r, const uint8_t g, const uint8_t b)
{
	const auto facRecord = factionRecord;
	facRecord->primary_colour_red = r;
	facRecord->primary_colour_green = g;
	facRecord->primary_colour_blue = b;
	gameHelpers::scriptCommand("console_command", "toggle_fow");
	gameHelpers::scriptCommand("console_command", "toggle_fow");
}

void factionStruct::setSecondaryColor(const uint8_t r, const uint8_t g, const uint8_t b)
{
	const auto facRecord = factionRecord;
	facRecord->secondary_colour_red = r;
	facRecord->secondary_colour_green = g;
	facRecord->secondary_colour_blue = b;;
	gameHelpers::scriptCommand("console_command", "toggle_fow");
	gameHelpers::scriptCommand("console_command", "toggle_fow");
}

void factionStruct::hideRevealedTile(const int x, const int y)
{
	if (stratMapHelpers::isStratMap() == false)
		return;
	struct someArgForHiding
	{
		int x;
		int y;
		int some = 2;
		float some2 = -1;
	};
	const auto revealedTiles = reinterpret_cast<void**>(tilesFac);
	const auto tilesArr = static_cast<someArgForHiding**>(revealedTiles[10]);
	const int num = reinterpret_cast<int>(revealedTiles[12]);
	for (int i = 0; i < num; i++)
	{
		if (tilesArr[i]->x == x && tilesArr[i]->y == y)
		{
			GAME_FUNC(int*(__thiscall*)(void*, someArgForHiding*, unsigned char), hideRevealedTile)(tilesFac, tilesArr[i], 1);
			return;
		}
	}
}

namespace factionHelpers
{
	
	factionRecord* getFactionRecord(const int id)
	{
		if (dataOffsets::offsets.descr_sm_factionslist == nullptr)
			return nullptr;
		
		const int facNum = dataOffsets::offsets.descr_sm_factionslist->size;
		for (int i = 0; i < facNum; ++i)
		{
			if (dataOffsets::offsets.descr_sm_factionslist->facDescrs[i].id==id)
				return &dataOffsets::offsets.descr_sm_factionslist->facDescrs[i];
		}
		return nullptr;
	}
	
	/*----------------------------------------------------------------------------------------------------------------*\
											 Faction helpers
	\*----------------------------------------------------------------------------------------------------------------*/
#pragma region Faction helpers

	template <char fieldIndex>
	std::string getStringProperty(const factionStruct* fac)
	{
		char* retS = nullptr;
		if (fieldIndex == factionStruct_ai_label)
			retS = fac->aiLabel;
		if (fieldIndex == factionStruct_name)
			retS = fac->factionRecord->facName;

		if (retS != nullptr)
			return std::string(retS);
		return std::string("");
	}

	template <char fieldIndex>
	void setStringProperty(factionStruct* fac, std::string newS)
	{
		if (fieldIndex == factionStruct_ai_label)
			gameStringHelpers::setHashedString(&fac->aiLabel, newS.c_str());
		if (fieldIndex == factionStruct_name)
			gameStringHelpers::setHashedString(&fac->factionRecord->facName, newS.c_str());
	}

	std::string getFactionName(const factionStruct* fac)
	{
		return std::string(fac->factionRecord->facName);
	}
	
	void changeFactionName(factionStruct* fac, const char* newName)
	{

		UNICODE_STRING** nameMem = new UNICODE_STRING*;
		fac->localizedName = nameMem;
		gameStringHelpers::createUniString(fac->localizedName, newName);
	}

	UNICODE_STRING** getFactionNameLocal(factionStruct* fac)
	{
		std::string facName = fac->factionRecord->facName;
		std::transform(facName.begin(), facName.end(), facName.begin(), ::toupper);
		auto nameMem = new UNICODE_STRING*;
		gameStringHelpers::createUniString(nameMem, facName.c_str());
		UNICODE_STRING*** nameMem2 = &nameMem;
		DWORD funcAddr = codes::offsets.getStringFromTable;
		DWORD stringTable = dataOffsets::offsets.stringTable;
		_asm
		{
			push nameMem2
			mov ecx, stringTable
			mov ecx, [ecx]
			mov eax, funcAddr
			call eax
			mov nameMem2, eax
		}
		return *nameMem2;
	}
	
	//legacy
	bool getTileVisibility(factionStruct* faction, const int x, const int y)
	{
		return faction->getTileVisibility(x,y);
	}

	bool hasMilitaryAccess(const factionStruct* fac1, const factionStruct* fac2)
	{
		const auto campaign = campaignHelpers::getCampaignData();
		return campaign->diplomaticStandings[fac1->factionID][fac2->factionID].hasMilitaryAccess;
	}

	void setMilitaryAccess(const factionStruct* fac1, const factionStruct* fac2, bool set)
	{
		const auto campaign = campaignHelpers::getCampaignData();
		campaign->diplomaticStandings[fac1->factionID][fac2->factionID].hasMilitaryAccess = set;
	}

	float getFactionStanding(const factionStruct* fac1, const factionStruct* fac2)
	{
		const auto campaign =campaignHelpers::getCampaignData();
		return campaign->diplomaticStandings[fac1->factionID][fac2->factionID].factionStanding;
	}

	watchTowerStruct* spawnWatchtower(const factionStruct* fac, int x, int y)
	{
		auto tile = stratMapHelpers::getTile(x, y);
		if (!tile)
			return nullptr;
		auto region = stratMapHelpers::getRegion(tile->regionId);
		if (!region || !region->settlement || region->factionOwner != fac)
			return nullptr;
		auto settlement = region->settlement;
		auto tower = nullptr;
		DWORD makeTowerAddr = codes::offsets.makeWatchTower;
		_asm
		{
			push settlement
			mov eax, makeTowerAddr
			call eax
			mov tower, eax
			add esp, 4
		}
		DWORD spawnCreatedObject = codes::offsets.spawnCreatedObject;
		auto spawnCoords = new coordPair();
		spawnCoords->xCoord = x;
		spawnCoords->yCoord = y;
		_asm
		{
			push spawnCoords
			push tower
			mov eax, spawnCreatedObject
			call eax
		}
		auto watchtowers = &campaignHelpers::getCampaignData()->watchtowers;
		DWORD addToWatchtowerList = codes::offsets.addToWatchtowerList;
		_asm
		{
			push tower
			mov ecx, watchtowers
			mov eax, addToWatchtowerList
			call eax
		}
		DWORD blockadeStuff = codes::offsets.updateBlockades;
		_asm
		{
			mov ecx, tower
			mov eax, blockadeStuff
			call eax
		}
		delete spawnCoords;
		return tower;
	}

	void setFactionStanding(const factionStruct* fac1, const factionStruct* fac2, float standing)
	{
		const auto campaign = campaignHelpers::getCampaignData();
		campaign->diplomaticStandings[fac1->factionID][fac2->factionID].factionStanding = standing;
	}
	
	std::string getLocalizedFactionName(factionStruct* fac)
	{
		UNICODE_STRING** localizedName = fac->localizedName;
		if (const UNICODE_STRING* name = *localizedName; name->Length == 0)
			localizedName = getFactionNameLocal(fac);
		return gameStringHelpers::uniStringToStr(localizedName);
	}

	float distance(int x1, int y1, int x2, int y2)
	{
		return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	}

	void* disembark(armyStruct* army, int x, int y)
	{
		DWORD cadClass = techFuncs::allocateGameMem(0x301Cu);
		cadClass = GAME_FUNC(DWORD(__thiscall*)(DWORD, int), createCadDisembark)(cadClass, 0);
		auto charPtr = &army->gen;
		coordPair coords{x, y};
		auto coordsPtr = &coords;
		GAME_FUNC(void(__thiscall*)(character**, coordPair*, DWORD), setCadClass)(charPtr, coordsPtr, cadClass);
		DWORD cadClass2 = *reinterpret_cast<DWORD*>(dataOffsets::offsets.globalCadClass);
		DWORD finalize = codes::offsets.finalizeActionStrat;
		auto character = army->gen;
		_asm
		{
			push cadClass2
			mov ecx, character
			mov eax, finalize
			call eax
		}
	}

	character* factionCreateCharacter(
		factionStruct* fac,
		const std::string& type,
		const int age,
		const std::string& name,
		const std::string& lastName,
		const int subFaction,
		const std::string& portrait,
		const int x,
		const int y
		)
	{
		return characterHelpers::createCharacter(type.c_str(), fac, age, name.c_str(), lastName.c_str(), subFaction, portrait.c_str(), x, y);
	}

	armyStruct* splitArmy(factionStruct *faction, const sol::table& units, int x, int y)
	{
		unit* unitList[20]{};
		coordPair targetCoords{x, y};
		const int unitCount = units.size();
		if (unitCount > 20 || unitCount < 1)
		{
			gameHelpers::logStringGame("factionStruct.splitArmy: unit count must be between 1 and 20.");
			return nullptr;
		}
		auto tile = stratMapHelpers::getTile(x, y);
		if (auto tileChar = tile->getCharacter(); tileChar)
		{
			if (tileChar->armyLeaded && tileChar->armyLeaded->faction != faction)
			{
				gameHelpers::logStringGame("factionStruct.splitArmy: can not split army, tile is occupied by enemy.");
				return nullptr;
			}
		}
		if (auto tileSett = tile->getSettlement(); tileSett && tileSett->faction != faction)
		{
			gameHelpers::logStringGame("factionStruct.splitArmy: can not split army, tile is occupied by enemy settlement.");
			return nullptr;
		}
		if (auto tileFort = tile->getFort(); tileFort && tileFort->faction != faction)
		{
			gameHelpers::logStringGame("factionStruct.splitArmy: can not split army, tile is occupied by enemy fort.");
			return nullptr;
		}
		armyStruct* stack = nullptr;
		for (int i{ 1 }; i <= unitCount; i++)
		{
			const auto& un = units.get<sol::optional<unit*>>(i);
			if (!un)
				break;
			unit* unit = un.value_or(nullptr);
			if (!unit)
				break;
			if (!stack)
				stack = unit->army;
			if (unit && unit->army == stack)
			    unitList[i - 1] = unit;
		}
		if (!stack)
			return nullptr;
		if (stack->shipArmy)
		{
			if (distance(stack->shipArmy->gen->xCoord, stack->shipArmy->gen->yCoord, x, y) > 1.5)
				disembark(stack->shipArmy, x, y);
		}
		auto stratPathFind = gameHelpers::getGameDataAll()->stratPathFinding; 
		if (!GAME_FUNC(bool(__thiscall*)(stratPathFinding*, unit**, int, coordPair*), canArmySplit)
			(stratPathFind, &unitList[0], unitCount, &targetCoords))
		{
			gameHelpers::logStringGame("factionStruct.splitArmy: can not split army.");
			return nullptr;
		}
		DWORD splitArmy = codes::offsets.splitArmy;
		auto coordsPtr = &targetCoords;
		auto listPtr = &unitList[0];
		_asm
		{
			push coordsPtr
			push unitCount
			push listPtr
			mov ecx, faction
			mov eax, splitArmy
			call eax
		}
		return unitList[0]->army;
	}

	ltgdGlobals* getLtgdGlobals()
	{
		return reinterpret_cast<ltgdGlobals*>(dataOffsets::offsets.ltgdGlobals);
	}

	militaryValuesLTGD* getAiFactionValues(factionStruct* fac)
	{
		ltgdGlobals* ltgd = getLtgdGlobals();
		if (!ltgd)
			return nullptr;
		return &ltgd->ltgdFactionValues[fac->factionID];
	}

	interFactionLTGD* getInterFactionLTGD(factionStruct* fac, factionStruct* targetFac)
	{
		ltgdGlobals* ltgd = getLtgdGlobals();
		if (!ltgd)
			return nullptr;
		return &ltgd->interFactionLTGD[fac->factionID][targetFac->factionID];
	}
	
#pragma endregion Faction helpers
	
	
    void addToLua(sol::state& luaState)
    {
        struct
        {
            sol::usertype<factionStruct>factionStruct;
            sol::usertype<factionRecord>factionRecord;
            sol::usertype<factionEconomy>factionEconomy;
            sol::usertype<factionRanking>factionRanking;
            sol::usertype<aiFaction> aiFaction;
            sol::usertype<aiLongTermGoalDirector> aiLongTermGoalDirector;
            sol::usertype<aiPersonalityValues> aiPersonality;
            sol::usertype<aiGlobalStrategyDirector> aiGlobalStrategyDirector;
            sol::usertype<decisionValuesLTGD> decisionValuesLTGD;
            sol::usertype<militaryValuesLTGD> aiFactionValues;
        	sol::usertype<interFactionLTGD> interFactionLTGD;
        	sol::usertype<holdRegionsWinCondition>holdRegionsWinCondition;
        	sol::usertype<battleFactionCounter>battleFactionCounter;
        }types;
    	
		///Faction
		//@section Faction

		/***
		Basic factionStruct table

		@tfield int factionID
		@tfield getFactionName getFactionName
		@tfield int cultureID
		@tfield int aiPersonalityType
		@tfield int aiPersonalityName
		@tfield aiFaction aiFaction
		@tfield string ai_label
		@tfield string name
		@tfield string localizedName
		@tfield settlementStruct capital
		@tfield characterRecord leader
		@tfield characterRecord heir
		@tfield int isPlayerControlled 0=AI, 1=player
		@tfield int neighbourFactionsBitmap
		@tfield int religion
		@tfield int isUndiscovered
		@tfield int missionCount
		@tfield int freezeFactionAI
		@tfield int treasuryTurnStart
		@tfield int incomeDoubled
		@tfield int battlesWon
		@tfield int battlesLost
		@tfield int settlementsCaptured
		@tfield int settlementsLost
		@tfield int unitsRecruited
		@tfield int spottedCharactersCount
		@tfield int unitsLost
		@tfield int lastOpponentId
		@tfield int otherFactionCount
		@tfield int money
		@tfield bool hasSeaAccess
		@tfield bool autoManageAll
		@tfield bool autoManageRecruitment
		@tfield bool autoManageBuildings
		@tfield bool autoManageTaxes
		@tfield bool isExcommunicated
		@tfield bool isHorde
		@tfield float spendingLimit
		@tfield int kingsPurse
		@tfield int regionsOwnedStart
		@tfield factionRecord facStrat
		@tfield factionRecord factionRecord
		@tfield int characterRecordNum includes literally all characters without distinction (so also wives, children, dead and those sent off map)
		@tfield getCharacterRecord getCharacterRecord
		@tfield int numOfCharacters includes all the characters present on the strat map
		@tfield getCharacter getCharacter
		@tfield int stacksNum
		@tfield int deadStatus
		@tfield int revoltReason
		@tfield getStack getStack
		@tfield int settlementsNum
		@tfield getSettlement getSettlement
		@tfield int fortsNum
		@tfield getFort getFort
		@tfield int portsNum
		@tfield getPort getPort
		@tfield int watchtowersNum
		@tfield getWatchtower getWatchtower
		@tfield int neighbourRegionsNum
		@tfield deleteFort deleteFort
		@tfield createFortXY createFortXY
		@tfield createWatchtower createWatchtower
		@tfield hasMilitaryAccess hasMilitaryAccess
		@tfield setMilitaryAccess setMilitaryAccess
		@tfield getFactionStanding getFactionStanding
		@tfield setFactionStanding setFactionStanding
		@tfield getFactionRanking getFactionRanking
		@tfield getFactionEconomy getFactionEconomy
		@tfield getNeighbourRegionID getNeighbourRegionID
		@tfield getBattleVsFactionStats getBattleVsFactionStats
		@tfield setCharacterNameFaction setCharacterNameFaction
		@tfield isNeighbourFaction isNeighbourFaction
		@tfield getAiFactionValues getAiFactionValues
		@tfield getInterFactionLTGD getInterFactionLTGD
		@tfield splitArmy splitArmy
		@tfield getSpottedCharacter getSpottedCharacter
		@tfield getTileVisibility getTileVisibility
		@tfield setTileVisibility setTileVisibility
		@tfield revealTile revealTile
		@tfield hideRevealedTile hideRevealedTile
		@tfield setColor setColor
		@tfield setSecondaryColor setSecondaryColor
		@tfield spawnArmy spawnArmy
		@tfield createCharacter createCharacter

		@table factionStruct
		*/
		types.factionStruct = luaState.new_usertype<factionStruct>("factionStruct");
		types.factionStruct.set("dipNum", &factionStruct::factionID);
		types.factionStruct.set("factionID", &factionStruct::factionID);
		types.factionStruct.set("aiFaction", &factionStruct::aiFaction);
		types.factionStruct.set("hasSeaAccess", &factionStruct::hasSeaAccess);
		types.factionStruct.set("spottedCharactersCount", &factionStruct::spottedCharactersCount);
		types.factionStruct.set("aiPersonalityType", &factionStruct::aiPersonalityType);
		types.factionStruct.set("aiPersonalityName", &factionStruct::aiPersonalityName);
		types.factionStruct.set_function("getFactionName", &getFactionName);
		types.factionStruct.set("cultureID", &factionStruct::cultureID);
		types.factionStruct.set("ai_label", sol::property(
			&getStringProperty<factionStruct_ai_label>, &setStringProperty<factionStruct_ai_label>
			));
		types.factionStruct.set("name", sol::property(
			&getStringProperty<factionStruct_name>, &setStringProperty<factionStruct_name>
			));
		types.factionStruct.set("localizedName", sol::property(
			&getLocalizedFactionName, &changeFactionName
			));
		types.factionStruct.set("regionsOwnedStart", &factionStruct::regionsOwnedStart);
		types.factionStruct.set("capital", &factionStruct::capital);
		types.factionStruct.set("leader", &factionStruct::leader);
		types.factionStruct.set("leader", &factionStruct::leader);
		types.factionStruct.set("heir", &factionStruct::heir);
		types.factionStruct.set("neighbourFactionsBitmap", &factionStruct::neighBourFactionsBitmap);
		types.factionStruct.set("isPlayerControlled", &factionStruct::isPlayerControlled);
		types.factionStruct.set("religion", &factionStruct::religion);
		types.factionStruct.set("missionCount", &factionStruct::missionCount);
		types.factionStruct.set("isUndiscovered", &factionStruct::isUndiscovered);
		types.factionStruct.set("neighbourRegionsNum", &factionStruct::neighBourRegionsNum);
		types.factionStruct.set("money", &factionStruct::money);
		types.factionStruct.set("deadStatus", &factionStruct::deadStatus);
		types.factionStruct.set("kingsPurse", &factionStruct::kingsPurse);
		types.factionStruct.set("facStrat", &factionStruct::factionRecord);
		types.factionStruct.set("factionRecord", &factionStruct::factionRecord);
		types.factionStruct.set("autoManageAll", &factionStruct::autoManageAll);
		types.factionStruct.set("autoManageRecruitment", &factionStruct::autoManageRecruitment);
		types.factionStruct.set("autoManageBuildings", &factionStruct::autoManageBuildings);
		types.factionStruct.set("autoManageTaxes", &factionStruct::autoManageTaxes);
		types.factionStruct.set("spendingLimit", &factionStruct::spendingLimit);
		types.factionStruct.set("isHorde", &factionStruct::isHorde);
		types.factionStruct.set("isExcommunicated", &factionStruct::isExcommunicated);
		types.factionStruct.set("revoltReason", &factionStruct::revoltReason);
		types.factionStruct.set("freezeFactionAI", &factionStruct::freezeFactionAI);
		types.factionStruct.set("treasuryTurnStart", &factionStruct::treasuryTurnStart);
		types.factionStruct.set("incomeDoubled", &factionStruct::incomeDoubled);
		types.factionStruct.set("battlesWon", &factionStruct::battlesWon);
		types.factionStruct.set("battlesLost", &factionStruct::battlesLost);
		types.factionStruct.set("unitsRecruited", &factionStruct::unitsRecruited);
		types.factionStruct.set("unitsLost", &factionStruct::unitsLost);
		types.factionStruct.set("lastOpponentId", &factionStruct::lastOpponentId);
		types.factionStruct.set("settlementsCaptured", &factionStruct::settlementsCaptured);
		types.factionStruct.set("settlementsLost", &factionStruct::settlementsLost);
		types.factionStruct.set("otherFactionCount", &factionStruct::otherFactionCount);
		types.factionStruct.set("numOfNamedCharacters", &factionStruct::characterRecordNum);
		types.factionStruct.set("characterRecordNum", &factionStruct::characterRecordNum);
		types.factionStruct.set_function("getNamedCharacter", &factionStruct::getCharacterRecord);
		/***
		Get a character record using it's index.
		@function factionStruct:getCharacterRecord
		@tparam int number
		@treturn characterRecord retCharacter
		@usage
		ourNamedCharacter = M2TW.campaign:getFaction("england"):getCharacterRecord(0)
		if ourNamedCharacter.command > 5 then
			ourFac.money = ourFac.money - (ourNamedCharacter.command * 10)
		end
		*/
		types.factionStruct.set_function("getCharacterRecord", &factionStruct::getCharacterRecord);
		types.factionStruct.set("numOfCharacters", &factionStruct::numOfCharacters);
		/***
		Get a character using it's index.
		@function factionStruct:getCharacter
		@tparam int number
		@treturn character retCharacter
		@usage
		ourCharacter = M2TW.campaign:getFaction("england"):getCharacter(0)
		if ourCharacter.xCoord > 150 and ourCharacter.movePoints < 10 then
			ourCharacter:kill()
		end
		*/
		types.factionStruct.set_function("getCharacter", &factionStruct::getCharacter);
		/***
		Get a spotted character using it's index.
		@function factionStruct:getSpottedCharacter
		@tparam int number
		@treturn character retCharacter
		@usage
		ourCharacter = M2TW.campaign:getFaction("england"):getSpottedCharacter(0)
		if ourCharacter.xCoord > 150 and ourCharacter.movePoints < 10 then
			ourCharacter:kill()
		end
		*/
		types.factionStruct.set_function("getSpottedCharacter", &factionStruct::getSpottedCharacter);
		types.factionStruct.set("stacksNum", &factionStruct::stackNum);
		/***
		Get an army using it's index.
		@function factionStruct:getStack
		@tparam int number
		@treturn armyStruct army
		@usage
		function FindArmy(x,y)
			for i = 0, M2TW.campaign.numberOfFactions - 1 do
				local faction =  M2TW.campaign:getFactionByOrder(i)
				if not faction then
					return nil
				end
				local armiesNum = faction.stacksNum
				for j = 0, armiesNum - 1 do
					local army = faction:getStack(j)
					local x2, y2 = 0, 0
					if (army.leader) then
						x2 = army.leader.xCoord
						y2 = army.leader.yCoord
					elseif army:findInSettlement() then
						x2 = army:findInSettlement().xCoord
						y2 = army:findInSettlement().yCoord
					elseif army:findInFort() then
						x2 = army:findInFort().xCoord
						y2 = army:findInFort().yCoord
					elseif army.shipArmy and army.shipArmy.leader then
						x2 = army.shipArmy.leader.xCoord
						y2 = army.shipArmy.leader.yCoord
					else
						return nil
					end
					if (x2 == x and y2 == y) then
						return army
					end
				end
			end
			return nil
		end
		*/
		types.factionStruct.set_function("getStack", &factionStruct::getArmy);
		types.factionStruct.set("settlementsNum", &factionStruct::settlementsNum);
		/***
		Get a settlement using it's index.
		@function factionStruct:getSettlement
		@tparam int number
		@treturn settlementStruct settlement
		@usage
		ourSett = CAMPAIGN:getFaction("byzantium"):getSettlement(0)
		if ourSett.isProvokedRebellion ~= 0 then
			ourFac.money = ourFac.money - (ourSett.level*5000)
		end
		*/
		types.factionStruct.set_function("getSettlement", &factionStruct::getSettlement);
		types.factionStruct.set("fortsNum", &factionStruct::fortsNum);
		/***
		Get a fort using it's index.
		@function factionStruct:getFort
		@tparam int number
		@treturn fortStruct fort
		@usage
		ourFort = CAMPAIGN:getFaction("byzantium"):getFort(0)
		if ourFort then
			print(ourFort.xCoord..", "..ourFort.yCoord)
		end
		*/
		types.factionStruct.set_function("getFort", &factionStruct::getFort);
		types.factionStruct.set("portsNum", &factionStruct::portBuildingsNum);
		/***
		Get a port using it's index.
		@function factionStruct:getPort
		@tparam int number
		@treturn portStruct port
		@usage
		local thisFac, portList = M2TW.campaign.currentFaction, "Ports:"
		for i = 0, thisFac.portsNum - 1, 1 do
			local thisPort = thisFac:getPort(i)
			local thisDock = thisPort.dock
			portList = portList.."\n\t"..i.." "..thisPort.settlement.name.." ("..thisPort.xCoord..", "..thisPort.yCoord..") "
			if thisDock then
				portList = portList.."\n\t\tDock: ("..thisDock.xCoord..", "..thisDock.yCoord..")"
			end
		end
		print(portList)
		*/
		types.factionStruct.set_function("getPort", &factionStruct::getPort);
		types.factionStruct.set("watchtowersNum", &factionStruct::watchtowersNum);
		/***
		Get a watchtower using it's index.
		@function factionStruct:getWatchtower
		@tparam int number
		@treturn watchtowerStruct watchtower
		@usage
		local thisFac, watchtowerList = M2TW.campaign.currentFaction, "Watchtowers:"
		for i = 0, thisFac.watchtowersNum - 1, 1 do
			local thisTower = thisFac:getWatchtower(i)
			watchtowerList = watchtowerList.."\n\t"..i.." ("..thisTower.xCoord..", "..thisTower.yCoord..") "
		end
		print(watchtowerList)
		*/
		types.factionStruct.set_function("getWatchtower", &factionStruct::getWatchtower);


		/***
		Delete a specific fort.
		@function factionStruct:deleteFort
		@tparam fortStruct fort
		@usage
		fac:deleteFort(someFort)
		*/
		types.factionStruct.set_function("deleteFort", &fortHelpers::deleteFort);

		/***
		Create a fort at the specified coordinates.
		@function factionStruct:createFortXY
		@tparam int X
		@tparam int Y
		@usage
		fac:createFortXY(193, 283)
		*/
		types.factionStruct.set_function("createFortXY", &fortHelpers::createFortXY);

		/***
		Create a watchtower at the specified coordinates.
		@function factionStruct:createWatchtower
		@tparam int X
		@tparam int Y
		@treturn watchtowerStruct watchTower
		@usage
		     fac:createWatchtower(193, 283)
		*/
		types.factionStruct.set_function("createWatchtower", &spawnWatchtower);

		/***
		Check if a faction has military access to another faction.
		@function factionStruct:hasMilitaryAccess
		@tparam factionStruct targetFaction
		@treturn bool hasMilitaryAccess
		@usage
		local hasAccess = fac:hasMilitaryAccess(targetFac)
		*/
		types.factionStruct.set_function("hasMilitaryAccess", &hasMilitaryAccess);

		/***
		Set if a faction has military access to another faction.
		@function factionStruct:setMilitaryAccess
		@tparam factionStruct targetFaction
		@tparam bool hasMilitaryAccess
		@usage
		fac:setMilitaryAccess(targetFac, true)
		*/
		types.factionStruct.set_function("setMilitaryAccess", &setMilitaryAccess);

		/***
		Get the faction standing between 2 factions (a faction with itself returns global standing).
		@function factionStruct:getFactionStanding
		@tparam factionStruct targetFaction
		@treturn float factionStanding
		@usage
		local standing = fac:getFactionStanding(targetFac)
		*/
		types.factionStruct.set_function("getFactionStanding", &getFactionStanding);
		/***
		Set the faction standing between 2 factions (a faction with itself sets global standing).
		@function factionStruct:setFactionStanding
		@tparam factionStruct targetFaction
		@tparam float factionStanding
		@usage
		fac:setFactionStanding(targetFac, 0.5)
		*/
		types.factionStruct.set_function("setFactionStanding", &setFactionStanding);

		/***
		Get the faction ranking scores.
		@function factionStruct:getFactionRanking
		@tparam int turnNumber
		@treturn factionRanking factionRanking
		@usage
		local rankings = fac:getFactionRanking(30)
		*/
		types.factionStruct.set_function("getFactionRanking", &factionStruct::getFactionRanking);

		/***
		Get the faction ecomomy table, specified number of turns ago (max 10).
		@function factionStruct:getFactionEconomy
		@tparam int turnsAgo
		@treturn factionEconomy factionEconomy
		@usage
		local economyTable = fac:getFactionEconomy(0)
		*/
		types.factionStruct.set_function("getFactionEconomy", &factionStruct::getFactionEconomy);

		/***
		Check if 2 factions are neighbours.
		@function factionStruct:isNeighbourFaction
		@tparam factionStruct targetFaction
		@treturn bool isNeighbour
		@usage
		local isNeighbour = fac:isNeighbourFaction(targetFac)
		*/
		types.factionStruct.set_function("isNeighbourFaction", &factionStruct::isNeighbourFaction);

		/***
		Get a region ID of a neighbouring region by index.
		@function factionStruct:getNeighbourRegionID
		@tparam int index
		@treturn int nRegionID
		@usage
		local nRegionID = fac:getNeighbourRegionID(0)
		*/
		types.factionStruct.set_function("getNeighbourRegionID", &factionStruct::getNeighbourRegionID);

		/***
		Get stats versus a specific other faction.
		@function factionStruct:getBattleVsFactionStats
		@tparam int targetFactionID
		@treturn battleFactionCounter battleStats
		@usage
		local battleStats = fac:getBattleVsFactionStats(2)
		*/
		types.factionStruct.set_function("getBattleVsFactionStats", &factionStruct::getBattleVsFactionStats);

		/***
		Get various statistics the long term goal director uses.
		@function factionStruct:getAiFactionValues
		@treturn aiFactionValues aiFactionValues
		@usage
		local aiFacValues = fac:getAiFactionValues()
		*/
		types.factionStruct.set_function("getAiFactionValues", &getAiFactionValues);

		/***
		Get various statistics the long term goal director uses versus a target faction.
		@function factionStruct:getInterFactionLTGD
		@tparam factionStruct targetFaction
		@treturn interFactionLTGD interFactionLTGD
		@usage
		local iLTGD = fac:getInterFactionLTGD(targetFac)
		*/
		types.factionStruct.set_function("getInterFactionLTGD", &getInterFactionLTGD);

		/***
		Set the faction characters of a specific type draw their names from.
		@function factionStruct:setCharacterNameFaction
		@tparam int characterTypeIndex
		@tparam int factionID
		@usage
		     fac:setCharacterNameFaction(characterType.spy, 2)
		*/
		types.factionStruct.set_function("setCharacterNameFaction", &factionStruct::setCharacterNameFaction);

		/***
		Get faction tile visibility.
		@function factionStruct:getTileVisibility
		@tparam int x
		@tparam int y
		@treturn int visibility use tileVisibility enum
		@usage
		     local vis = fac:getTileVisibility(172, 293)
		*/
		types.factionStruct.set_function("getTileVisibility", &factionStruct::getTileVisibility);

		/***
		Set faction tile visibility.
		@function factionStruct:setTileVisibility
		@tparam int x
		@tparam int y
		@tparam int visibility use tileVisibility enum
		@usage
		     fac:setTileVisibility(172, 293, tileVisibility.seen)
		*/
		types.factionStruct.set_function("setTileVisibility", &factionStruct::setTileVisibility);

		/***
		Reveal a tile.
		@function factionStruct:revealTile
		@tparam int x
		@tparam int y
		@usage
		     local vis = fac:revealTile(172, 293)
		*/
		types.factionStruct.set_function("revealTile", &factionStruct::revealTile);

		/***
		Hide revealed tile.
		@function factionStruct:hideRevealedTile
		@tparam int x
		@tparam int y
		@usage
		     fac:hideRevealedTile(172, 293)
		*/
		types.factionStruct.set_function("hideRevealedTile", &factionStruct::hideRevealedTile);

		/***
		Set the faction's color.
		@function factionStruct:setColor
		@tparam int R
		@tparam int G
		@tparam int B
		@usage
		     fac:setColor(255, 255, 255)
		*/
		types.factionStruct.set_function("setColor", &factionStruct::setColor);
		
		/***
		Set the faction's secondary color.
		@function factionStruct:setSecondaryColor
		@tparam int R
		@tparam int G
		@tparam int B
		@usage
		     fac:setSecondaryColor(255, 255, 255)
		*/
		types.factionStruct.set_function("setSecondaryColor", &factionStruct::setSecondaryColor);
		
		/***
		Create a new character at the specified coordinates. If you are not spawning an agent it is preferred to use spawnArmy instead.
		@function factionStruct:createCharacter
		@tparam string type Character type, for example "named character".
		@tparam int age The character's age
		@tparam string name The short name of the character.
		@tparam string name2 The full name of the character.
		@tparam int subFaction Set to 31 to disable.
		@tparam string portrait_custom cannot be nil Name of the folder inside 'data/ui/custom_portraits folder. Can not be nil!
		@tparam int xCoord X coordinate of the new character
		@tparam int yCoord Y coordinate of the new character
		@treturn character newCharacter Returns a character class, not a named character class!
		@usage
		local newCharacter = myFaction:createCharacter("named character", 18, "Name1", "Name2", 31, "custom_portrait_name", 263, 182)
		*/
		types.factionStruct.set_function("createCharacter", &factionCreateCharacter);
		/***
		Create a new army at the specified coordinates. Works similarly to the script command spawn_army. You can respawn off-map characters using it. You can not re-use labels!
		@function factionStruct:spawnArmy
		@tparam string name The short name of the character. Use random_name to pick a random name.
		@tparam string name2 The full name of the character.
		@tparam int type characterType.named_character or characterType.general or characterType.admiral.
		@tparam string label label of the character, has to be unique!. Can be nil.
		@tparam string portrait Name of the folder inside 'data/ui/custom_portraits folder.
		@tparam int x X coordinate of the new character
		@tparam int y Y coordinate of the new character
		@tparam int age The character's age
		@tparam bool family should character be auto adopted?
		@tparam int subFaction Set to 31 to disable.
		@tparam int unitIndex Index of the unit in the unit list. Can be EOP or normal.
		@tparam int exp
		@tparam int wpn
		@tparam int armour
		@treturn armyStruct newArmy
		@usage
		local army = myFaction:spawnArmy(
		"Rufus",
		"",
		characterType.named_character,
		"rufus_1",
		"",
		106, 149,
		18, false, 31,
		M2TWEOPDU.getEduIndexByType("Peasants"), 3, 0, 0
		)
		*/
		types.factionStruct.set_function("spawnArmy", &armyHelpers::spawnArmy);
		
		/***
		Split an army. If there is an army at target coords they merge. They embark/disembark. They enter and leave settlements.
		@function factionStruct:splitArmy
		@tparam table units
		@tparam int targetX
		@tparam int targetY
		@usage
		     local units = {}
		     for i = 0, myStack.unitsNum / 2 - 1 do
		         table.insert(units, myStack:getUnit(i))
		     end
		     fac:splitArmy(units, 154, 84)
		*/
		types.factionStruct.set_function("splitArmy", &splitArmy);

		/***
		Basic battleFactionCounter table

		@tfield int battlesWon
		@tfield int battlesLost
		@tfield int lastResult 0 - lose, 1 - draw, 2 - win

		@table battleFactionCounter
		*/
		types.battleFactionCounter = luaState.new_usertype<battleFactionCounter>("battleFactionCounter");
		types.battleFactionCounter.set("battlesWon", &battleFactionCounter::battlesWon);
		types.battleFactionCounter.set("battlesLost", &battleFactionCounter::battlesLost);
		types.battleFactionCounter.set("lastResult", &battleFactionCounter::lastResult);

		/***
		Basic holdRegionsWinCondition table

		@tfield int regionsToHoldCount
		@tfield int numberOfRegions
		@tfield getRegionToHoldName getRegionToHoldName
		@tfield getRegionToHoldLength getRegionToHoldLength
		
		@table holdRegionsWinCondition
		*/
		types.holdRegionsWinCondition = luaState.new_usertype<holdRegionsWinCondition>("holdRegionsWinCondition");
		types.holdRegionsWinCondition.set("regionsToHoldCount", &holdRegionsWinCondition::regionsToHoldCount);
		types.holdRegionsWinCondition.set("numberOfRegions", &holdRegionsWinCondition::numberOfRegions);

		/***
		Get the name of the region that has to be held to win the campaign.
		@function holdRegionsWinCondition:getRegionToHoldName
		@tparam int index
		@treturn string regionName
		@usage
		local regionName = fac.winCondition:getRegionToHoldName(0)
		*/
		types.holdRegionsWinCondition.set_function("getRegionToHoldName", &holdRegionsWinCondition::getRegionToHoldName);

		/***
		Get the number of turns the region has to be held to win the campaign.
		@function holdRegionsWinCondition:getRegionToHoldLength
		@tparam int index
		@treturn int turnsToHold
		@usage
		local turnsToHold = fac.winCondition:getRegionToHoldLength(0)
		*/
		types.holdRegionsWinCondition.set_function("getRegionToHoldLength", &holdRegionsWinCondition::getRegionToHoldLength);
		
		/***
		Basic factionEconomy table

		@tfield int farmingIncome
		@tfield int taxesIncome
		@tfield int miningIncome
		@tfield int tradeIncome
		@tfield int merchantIncome
		@tfield int constructionIncome
		@tfield int lootingIncome
		@tfield int missionIncome
		@tfield int diplomacyIncome
		@tfield int tributesIncome
		@tfield int adminIncome
		@tfield int kingsPurseIncome
		@tfield int wagesExpense
		@tfield int upkeepExpense
		@tfield int constructionExpenseBuildings
		@tfield int constructionExpenseField
		@tfield int recruitmentExpenseBuildings
		@tfield int recruitmentExpenseMercs
		@tfield int corruptionExpense
		@tfield int diplomacyExpense
		@tfield int tributesExpense
		@tfield int disasterExpense
		@tfield int entertainmentExpense
		@tfield int devastationExpense

		@table factionEconomy
		*/
		types.factionEconomy = luaState.new_usertype<factionEconomy>("factionEconomy");
		types.factionEconomy.set("farmingIncome", &factionEconomy::farmingIncome);
		types.factionEconomy.set("taxesIncome", &factionEconomy::taxesIncome);
		types.factionEconomy.set("miningIncome", &factionEconomy::miningIncome);
		types.factionEconomy.set("tradeIncome", &factionEconomy::tradeIncome);
		types.factionEconomy.set("merchantIncome", &factionEconomy::merchantIncome);
		types.factionEconomy.set("constructionIncome", &factionEconomy::constructionIncome);
		types.factionEconomy.set("lootingIncome", &factionEconomy::lootingIncome);
		types.factionEconomy.set("missionIncome", &factionEconomy::missionIncome);
		types.factionEconomy.set("diplomacyIncome", &factionEconomy::diplomacyIncome);
		types.factionEconomy.set("tributesIncome", &factionEconomy::tributesIncome);
		types.factionEconomy.set("adminIncome", &factionEconomy::adminIncome);
		types.factionEconomy.set("kingsPurseIncome", &factionEconomy::kingsPurseIncome);
		types.factionEconomy.set("wagesExpense", &factionEconomy::wagesExpense);
		types.factionEconomy.set("upkeepExpense", &factionEconomy::upkeepExpense);
		types.factionEconomy.set("constructionExpenseBuildings", &factionEconomy::constructionExpenseBuildings);
		types.factionEconomy.set("constructionExpenseField", &factionEconomy::constructionExpenseField);
		types.factionEconomy.set("recruitmentExpenseBuildings", &factionEconomy::recruitmentExpenseBuildings);
		types.factionEconomy.set("recruitmentExpenseMercs", &factionEconomy::recruitmentExpenseMercs);
		types.factionEconomy.set("corruptionExpense", &factionEconomy::corruptionExpense);
		types.factionEconomy.set("diplomacyExpense", &factionEconomy::diplomacyExpense);
		types.factionEconomy.set("tributesExpense", &factionEconomy::tributesExpense);
		types.factionEconomy.set("disasterExpense", &factionEconomy::disasterExpense);
		types.factionEconomy.set("entertainmentExpense", &factionEconomy::entertainmentExpense);
		types.factionEconomy.set("devastationExpense", &factionEconomy::devastationExpense);
		
		/***
		Basic factionRanking table

		@tfield float totalRankingScore
		@tfield float militaryRankingScore
		@tfield float productionRankingScore
		@tfield float territoryRankingScore
		@tfield float financialRankingScore
		@tfield float populationRankingScore

		@table factionRanking
		*/
		types.factionRanking = luaState.new_usertype<factionRanking>("factionRanking");
		types.factionRanking.set("totalRankingScore", &factionRanking::totalRanking);
		types.factionRanking.set("militaryRankingScore", &factionRanking::militaryRanking);
		types.factionRanking.set("productionRankingScore", &factionRanking::productionRanking);
		types.factionRanking.set("territoryRankingScore", &factionRanking::territoryRanking);
		types.factionRanking.set("financialRankingScore", &factionRanking::financialRanking);
		types.factionRanking.set("populationRankingScore", &factionRanking::populationRanking);

		///aiFaction
		//@section aiFaction

		/***
		Basic aiFaction table

		@tfield factionStruct faction
		@tfield int factionID
		@tfield aiLongTermGoalDirector LTGD
		@tfield aiPersonality aiPersonality

		@table aiFaction
		*/
		types.aiFaction = luaState.new_usertype<aiFaction>("aiFaction");
		types.aiFaction.set("faction", &aiFaction::faction);
		types.aiFaction.set("factionID", &aiFaction::factionID);
		types.aiFaction.set("LTGD", &aiFaction::aiLongTermGoalDirector);
		types.aiFaction.set("aiPersonality", &aiFaction::aiProductionControllers);

		///Long Term Goal Director
		//@section Long Term Goal Director

		/***
		Basic aiLongTermGoalDirector table

		@tfield factionStruct faction
		@tfield aiFaction aiFaction
		@tfield int trustedAllyEnemiesBitfield
		@tfield int freeStrengthEnemy
		@tfield int freeStrengthEnemyBalance
		@tfield int consideringNavalInvasion
		@tfield int navalTargetRegionID
		@tfield int stagingRegionID
		@tfield int longTermPolicy
		@tfield int longTermTroopStatus
		@tfield getLongTermGoalValues getLongTermGoalValues
		@tfield isTrustedAllyEnemy isTrustedAllyEnemy

		@table aiLongTermGoalDirector
		*/
		types.aiLongTermGoalDirector = luaState.new_usertype<aiLongTermGoalDirector>("aiLongTermGoalDirector");
		types.aiLongTermGoalDirector.set("faction", &aiLongTermGoalDirector::faction);
		types.aiLongTermGoalDirector.set("aiFaction", &aiLongTermGoalDirector::aiFaction);
		types.aiLongTermGoalDirector.set("trustedAllyEnemiesBitfield", &aiLongTermGoalDirector::trustedAllyEnemies);
		types.aiLongTermGoalDirector.set("freeStrengthEnemy", &aiLongTermGoalDirector::freeStrengthEnemy);
		types.aiLongTermGoalDirector.set("freeStrengthEnemyBalance", &aiLongTermGoalDirector::freeStrengthEnemyBalance);
		types.aiLongTermGoalDirector.set("consideringNavalInvasion", &aiLongTermGoalDirector::consideringNavalInvasion);
		types.aiLongTermGoalDirector.set("navalTargetRegionID", &aiLongTermGoalDirector::navalTargetRegionID);
		types.aiLongTermGoalDirector.set("stagingRegionID", &aiLongTermGoalDirector::stagingRegionID);
		types.aiLongTermGoalDirector.set("longTermPolicy", &aiLongTermGoalDirector::longTermPolicy);
		types.aiLongTermGoalDirector.set("longTermTroopStatus", &aiLongTermGoalDirector::longTermTroopStatus);

		/***
		Get long term goal director values versus a specific other faction.
		@function aiLongTermGoalDirector:getLongTermGoalValues
		@tparam int targetFactionID
		@treturn decisionValuesLTGD longTermGoalValues
		@usage
		local longTermGoalValues = LTGD:getLongTermGoalValues(2)
		*/
		types.aiLongTermGoalDirector.set_function("getLongTermGoalValues", &aiLongTermGoalDirector::getlongTermGoalValues);

		/***
		Check if another faction is an enemy of one the faction's trusted allies.
		@function aiLongTermGoalDirector:isTrustedAllyEnemy
		@tparam int targetFactionID
		@treturn bool isAllyEnemy
		@usage
		local isAllyEnemy = LTGD:isTrustedAllyEnemy(2)
		*/
		types.aiLongTermGoalDirector.set_function("isTrustedAllyEnemy", &aiLongTermGoalDirector::isTrustedAllyEnemy);
		
		/***
		Basic decisionValuesLTGD table

		@tfield int defendType
		@tfield int defendPriority
		@tfield int invasionType
		@tfield int invadePriority
		@tfield int atWar
		@tfield int wantPeace
		@tfield int wantAlly
		@tfield int forceInvade
		@tfield int wantBeProtect
		@tfield int wantOfferProtect
		@tfield int allianceAgainst
		@tfield int ptsDesire
		@tfield int ptsAlliance
		@tfield int pointsInvasion
		@tfield int pointsDefense
		@tfield int canForceInvade

		@table decisionValuesLTGD
		*/
		types.decisionValuesLTGD = luaState.new_usertype<decisionValuesLTGD>("decisionValuesLTGD");
		types.decisionValuesLTGD.set("defendType", &decisionValuesLTGD::defendType);
		types.decisionValuesLTGD.set("defendPriority", &decisionValuesLTGD::defendPriority);
		types.decisionValuesLTGD.set("invasionType", &decisionValuesLTGD::invasionType);
		types.decisionValuesLTGD.set("invadePriority", &decisionValuesLTGD::invadePriority);
		types.decisionValuesLTGD.set("atWar", &decisionValuesLTGD::atWar);
		types.decisionValuesLTGD.set("wantPeace", &decisionValuesLTGD::wantPeace);
		types.decisionValuesLTGD.set("wantAlly", &decisionValuesLTGD::wantAlly);
		types.decisionValuesLTGD.set("wantBeProtect", &decisionValuesLTGD::wantBeProtect);
		types.decisionValuesLTGD.set("wantOfferProtect", &decisionValuesLTGD::wantOfferProtect);
		types.decisionValuesLTGD.set("allianceAgainst", &decisionValuesLTGD::allianceAgainst);
		types.decisionValuesLTGD.set("ptsDesire", &decisionValuesLTGD::ptsDesire);
		types.decisionValuesLTGD.set("ptsAlliance", &decisionValuesLTGD::ptsAlliance);
		types.decisionValuesLTGD.set("canForceInvade", &decisionValuesLTGD::canForceInvade);
		types.decisionValuesLTGD.set("forceInvade", &decisionValuesLTGD::forceInvade);
		types.decisionValuesLTGD.set("pointsInvasion", &decisionValuesLTGD::pointsInvasion);
		types.decisionValuesLTGD.set("pointsDefense", &decisionValuesLTGD::pointsDefense);
		
		/***
		Basic aiFactionValues table

		@tfield int totalPopulation
		@tfield int tileCount
		@tfield int averagePopulation
		@tfield int productionValue
		@tfield int nonAlliedBorderLength
		@tfield int alliedBorderLength
		@tfield int fleetCount
		@tfield int navalPowerPerFleet
		@tfield int navalStrength
		@tfield int armyCount
		@tfield int strengthPerArmy
		@tfield int totalStrength
		@tfield int freeStrength
		@tfield int enemyNum
		@tfield int immediateEnemyStrength
		@tfield int protectorateOf

		@table aiFactionValues
		*/
		types.aiFactionValues = luaState.new_usertype<militaryValuesLTGD>("aiFactionValues");
		types.aiFactionValues.set("totalPopulation", &militaryValuesLTGD::totalPopulation);
		types.aiFactionValues.set("tileCount", &militaryValuesLTGD::tileCount);
		types.aiFactionValues.set("averagePopulation", &militaryValuesLTGD::averagePopulation);
		types.aiFactionValues.set("productionValue", &militaryValuesLTGD::productionValue);
		types.aiFactionValues.set("nonAlliedBorderLength", &militaryValuesLTGD::nonAlliedBorderLength);
		types.aiFactionValues.set("alliedBorderLength", &militaryValuesLTGD::alliedBorderLength);
		types.aiFactionValues.set("fleetCount", &militaryValuesLTGD::fleetCount);
		types.aiFactionValues.set("navalPowerPerFleet", &militaryValuesLTGD::navalPowerPerFleet);
		types.aiFactionValues.set("navalStrength", &militaryValuesLTGD::navalStrength);
		types.aiFactionValues.set("armyCount", &militaryValuesLTGD::armyCount);
		types.aiFactionValues.set("strengthPerArmy", &militaryValuesLTGD::strengthPerArmy);
		types.aiFactionValues.set("totalStrength", &militaryValuesLTGD::totalStrength);
		types.aiFactionValues.set("freeStrength", &militaryValuesLTGD::freeStrength);
		types.aiFactionValues.set("enemyNum", &militaryValuesLTGD::enemyNum);
		types.aiFactionValues.set("immediateEnemyStrength", &militaryValuesLTGD::immediateEnemyStrength);
		types.aiFactionValues.set("protectorateOf", &militaryValuesLTGD::protectorateOf);

		/***
		Basic interFactionLTGD table

		@tfield int borderTiles
		@tfield int frontLineBalance
		@tfield int hasAllianceAgainst
		@tfield int isStrongestNeighbour
		@tfield int isWeakestNeighbour

		@table interFactionLTGD
		*/
		types.interFactionLTGD = luaState.new_usertype<interFactionLTGD>("interFactionLTGD");
		types.interFactionLTGD.set("borderTiles", &interFactionLTGD::borderTiles);
		types.interFactionLTGD.set("frontLineBalance", &interFactionLTGD::frontLineBalance);
		types.interFactionLTGD.set("hasAllianceAgainst", &interFactionLTGD::hasAllianceAgainst);
		types.interFactionLTGD.set("isStrongestNeighbour", &interFactionLTGD::isStrongestNeighbour);
		types.interFactionLTGD.set("isWeakestNeighbour", &interFactionLTGD::isWeakestNeighbour);
		
		///AI Personality
		//@section AI Personality

		/***
		Basic aiPersonality table

		@tfield aiFaction aiFaction
		@tfield int aiProductionControllersNum
		@tfield int aiPersonalityType
		@tfield int aiPersonalityName
		@tfield int balancedPolicyNum
		@tfield int financialPolicyNum
		@tfield int militaryPolicyNum
		@tfield int growthPolicyNum
		@tfield int culturalPolicyNum
		@tfield setConstructionValue setConstructionValue
		@tfield setConstructionAgentValue setConstructionAgentValue
		@tfield setConstructionUnitValue setConstructionUnitValue
		@tfield setRecruitmentValue setRecruitmentValue
		@tfield setAgentValue setAgentValue
		@tfield getConstructionValue getConstructionValue
		@tfield getConstructionAgentValue getConstructionAgentValue
		@tfield getConstructionUnitValue getConstructionUnitValue
		@tfield getAgentValue getAgentValue
		@tfield getRecruitmentValue getRecruitmentValue
		@tfield getProductionController getProductionController

		@table aiPersonality
		*/
		types.aiPersonality = luaState.new_usertype<aiPersonalityValues>("aiPersonality");
		types.aiPersonality.set("aiFaction", &aiPersonalityValues::aiFaction);
		types.aiPersonality.set("aiProductionControllersNum", &aiPersonalityValues::aiProductionControllersNum);
		types.aiPersonality.set("aiPersonalityType", &aiPersonalityValues::AIPersonalityType);
		types.aiPersonality.set("aiPersonalityName", &aiPersonalityValues::AIPersonalityName);
		types.aiPersonality.set("balancedPolicyNum", &aiPersonalityValues::balancedPolicyNum);
		types.aiPersonality.set("financialPolicyNum", &aiPersonalityValues::financialPolicyNum);
		types.aiPersonality.set("militaryPolicyNum", &aiPersonalityValues::militaryPolicyNum);
		types.aiPersonality.set("growthPolicyNum", &aiPersonalityValues::growthPolicyNum);
		types.aiPersonality.set("culturalPolicyNum", &aiPersonalityValues::culturalPolicyNum);
		/***
		Set bias value of the ai personality for a capability.
		@function aiPersonality:setConstructionValue
		@tparam int type use building capabilities enum
		@tparam int value
		@usage
		     aiPersonality:setConstructionValue(buildingCapability.law_bonus, 100)
		*/
		types.aiPersonality.set_function("setConstructionValue", &aiPersonalityValues::setConstructionValue);
		/***
		Set bias value of the ai personality for a capability.
		@function aiPersonality:setConstructionAgentValue
		@tparam int type use characterTypes
		@tparam int value
		@usage
		     aiPersonality:setConstructionAgentValue(characterType.diplomat, 100)
		*/
		types.aiPersonality.set_function("setConstructionAgentValue", &aiPersonalityValues::setConstructionAgentValue);
		/***
		Set bias value of the ai personality for a capability.
		@function aiPersonality:setConstructionUnitValue
		@tparam int type use unitCategoryClass enum
		@tparam int value
		@usage
		     aiPersonality:setConstructionUnitValue(unitCategoryClass.heavyCavalry, 100)
		*/
		types.aiPersonality.set_function("setConstructionUnitValue", &aiPersonalityValues::setConstructionUnitValue);
		/***
		Set bias value of the ai personality for a recruitment class.
		@function aiPersonality:setRecruitmentValue
		@tparam int type use unitCategoryClass enum
		@tparam int value
		@usage
			 aiPersonality:setRecruitmentValue(unitCategoryClass.heavyCavalry, 100)
		*/
		types.aiPersonality.set_function("setRecruitmentValue", &aiPersonalityValues::setRecruitmentValue);
		/***
		Set bias value of the ai personality for an agent type.
		@function aiPersonality:setAgentValue
		@tparam int type use characterTypes
		@tparam int value
		@usage
			 aiPersonality:setAgentValue(characterType.diplomat, 100)
		*/
		types.aiPersonality.set_function("setAgentValue", &aiPersonalityValues::setAgentValue);
		/***
		Get bias value of the ai personality for a capability.
		@function aiPersonality:getConstructionValue
		@tparam int type use building capabilities enum
		@treturn int value
		@usage
			local value = aiPersonality:getConstructionValue(buildingCapability.law_bonus)
		*/
		types.aiPersonality.set_function("getConstructionValue", &aiPersonalityValues::getConstructionValue);
		/***
		Get bias value of the ai personality for a capability.
		@function aiPersonality:getConstructionAgentValue
		@tparam int type use characterTypes
		@treturn int value
		@usage
			local value = aiPersonality:getConstructionAgentValue(characterType.diplomat)
		*/
		types.aiPersonality.set_function("getConstructionAgentValue", &aiPersonalityValues::getConstructionAgentValue);
		/***
		Get bias value of the ai personality for a capability.
		@function aiPersonality:getConstructionUnitValue
		@tparam int type use unitCategoryClass enum
		@treturn int value
		@usage
			local value = aiPersonality:getConstructionUnitValue(unitCategoryClass.heavyCavalry)
		*/
		types.aiPersonality.set_function("getConstructionUnitValue", &aiPersonalityValues::getConstructionUnitValue);
		/***
		Get bias value of the ai personality for a recruitment class.
		@function aiPersonality:getRecruitmentValue
		@tparam int type use unitCategoryClass enum
		@treturn int value
		@usage
			local value = aiPersonality:getRecruitmentValue(unitCategoryClass.heavyCavalry)
		*/
		types.aiPersonality.set_function("getRecruitmentValue", &aiPersonalityValues::getRecruitmentValue);
		/***
		Get bias value of the ai personality for an agent type.
		@function aiPersonality:getAgentValue
		@tparam int type use characterTypes
		@treturn int value
		@usage
			local value = aiPersonality:getAgentValue(characterType.diplomat)
		*/
		types.aiPersonality.set_function("getAgentValue", &aiPersonalityValues::getAgentValue);
		/***
		Get a production controller by index.
		@function aiPersonality:getProductionController
		@tparam int index
		@treturn aiProductionController controller
		@usage
			local prodController = aiPersonality:getProductionController(0)
		*/
		types.aiPersonality.set_function("getProductionController", &aiPersonalityValues::getProductionController);

		///Faction Record
		//@section Faction Record

		/***
		Basic factionRecord table

		@tfield int primaryColorRed Warning: resets on reload.
		@tfield int primaryColorGreen Warning: resets on reload.
		@tfield int primaryColorBlue Warning: resets on reload.
		@tfield int secondaryColorRed Warning: resets on reload.
		@tfield int secondaryColorGreen Warning: resets on reload.
		@tfield int secondaryColorBlue Warning: resets on reload.
		@tfield int triumphValue Usage unknown.
		@tfield int religionID
		@tfield int standardIndex Warning: resets on reload.
		@tfield int logoIndex Warning: resets on reload.
		@tfield int smallLogoIndex Warning: resets on reload.
		@tfield int customBattleAvailability
		@tfield int periodsUnavailableInCustomBattle
		@tfield bool canSap shouldnt do anything in med 2, but could potentially use flag to store some other info about this faction
		@tfield int prefersNavalInvasions
		@tfield bool canHavePrincess
		@tfield bool canHorde
		@tfield bool hasFamilyTree
		@tfield bool teutonic
		@tfield bool disbandToPools
		@tfield bool canBuildSiegeTowers
		@tfield bool canTransmitPlague
		@tfield int shadowedByID
		@tfield int shadowingID
		@tfield int spawnsOnRevoltID
		@tfield int spawnsOnRevoltOwnerID
		@tfield bool spawnsOnEvent
		@tfield bool roman
		@tfield bool barbarian
		@tfield bool eastern
		@tfield bool slave
		@tfield int hordeMinUnits
		@tfield int hordeMaxUnits
		@tfield int reductionPerHorde
		@tfield int hordeUnitPerSettlementPop
		@tfield int hordeMinNamedCharacters
		@tfield int hordeMaxPercentArmyStack

		@table factionRecord
		*/
		types.factionRecord = luaState.new_usertype<factionRecord>("factionRecord");
		types.factionRecord.set("primaryColorRed", &factionRecord::primary_colour_red);
		types.factionRecord.set("primaryColorGreen", &factionRecord::primary_colour_green);
		types.factionRecord.set("primaryColorBlue", &factionRecord::primary_colour_blue);
		types.factionRecord.set("canHorde", &factionRecord::canHorde);
		types.factionRecord.set("secondaryColorRed", &factionRecord::secondary_colour_red);
		types.factionRecord.set("secondaryColorGreen", &factionRecord::secondary_colour_green);
		types.factionRecord.set("secondaryColorBlue", &factionRecord::secondary_colour_blue);
		types.factionRecord.set("triumphValue", &factionRecord::triumph_value);
		types.factionRecord.set("standardIndex", &factionRecord::standard_index);
		types.factionRecord.set("logoIndex", &factionRecord::logo_index);
		types.factionRecord.set("smallLogoIndex", &factionRecord::small_logo_index);
		types.factionRecord.set("religionID", &factionRecord::religionID);
		types.factionRecord.set("customBattleAvailability", &factionRecord::customBattleAvailability);
		types.factionRecord.set("periodsUnavailableInCustomBattle", &factionRecord::periodsUnavailableInCustomBattle);
		types.factionRecord.set("canSap", &factionRecord::canSap);
		types.factionRecord.set("prefersNavalInvasions", &factionRecord::prefersNavalInvasions);
		types.factionRecord.set("canHavePrincess", &factionRecord::canHavePrincess);
		types.factionRecord.set("hasFamilyTree", &factionRecord::hasFamilyTree);
		types.factionRecord.set("teutonic", &factionRecord::teutonic);
		types.factionRecord.set("disbandToPools", &factionRecord::disbandToPools);
		types.factionRecord.set("canBuildSiegeTowers", &factionRecord::canBuildSiegeTowers);
		types.factionRecord.set("canTransmitPlague", &factionRecord::canTransmitPlague);
		types.factionRecord.set("shadowedByID", &factionRecord::shadowFactionId);
		types.factionRecord.set("shadowingID", &factionRecord::shadowFactionOwnerId);
		types.factionRecord.set("spawnsOnRevoltID", &factionRecord::spawnsOnRevoltID);
		types.factionRecord.set("spawnsOnRevoltOwnerID", &factionRecord::spawnsOnRevoltOwnerID);
		types.factionRecord.set("roman", &factionRecord::roman);
		types.factionRecord.set("barbarian", &factionRecord::barbarian);
		types.factionRecord.set("eastern", &factionRecord::eastern);
		types.factionRecord.set("slave", &factionRecord::slave);
		types.factionRecord.set("hordeMinUnits", &factionRecord::hordeMinUnits);
		types.factionRecord.set("hordeMaxUnits", &factionRecord::hordeMaxUnits);
		types.factionRecord.set("reductionPerHorde", &factionRecord::reductionPerHorde);
		types.factionRecord.set("hordeUnitPerSettlementPop", &factionRecord::hordeUnitPerSettlementPop);
		types.factionRecord.set("hordeMinNamedCharacters", &factionRecord::hordeMinNamedCharacters);
		types.factionRecord.set("hordeMaxPercentArmyStack", &factionRecord::hordeMaxPercentArmyStack);
    }
}