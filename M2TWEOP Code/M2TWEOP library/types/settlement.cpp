﻿///
//![Lua logo](../Lua.png)
//@module LuaPlugin
//@author Fynn
//@license GPL-3.0
#include "pch.h"
#include "settlement.h"

#include "dataOffsets.h"
#include "character.h"
#include "characterRecord.h"
#include "eopBuildings.h"
#include "gameStringHelpers.h"
#include "functionsOffsets.h"
#include "battle.h"
#include "faction.h"
#include "unit.h"
#include "army.h"
#include "campaign.h"
#include "gameHelpers.h"
#include "strategyMap.h"
#include "stratModelsChange.h"

std::shared_ptr<eopSettlementDataDb> eopSettlementDataDb::instance = std::make_shared<eopSettlementDataDb>();
std::string eopSettlementDataDb::onGameSave()
{
	const auto campaignData = campaignHelpers::getCampaignData();
	for (int i = 0; i < campaignData->factionCount; i++)
	{
		const auto faction = campaignData->getFactionByOrder(i);
		for (int s = 0; s < faction->settlementsNum; s++)
		{
			const auto settlement = faction->settlements[s];
			for (int b = 0; b < settlement->buildingsNum; b++)
			{
				if (const auto building = settlement->buildings[b]; building->edbEntry->eopBuildingID != 0)
				{
					eopSettData->at(settlement->regionID).eopBuildingEntries[b] = building->edbEntry->eopBuildingID;
				}
			}
		}
	}
	std::string fPath = gameHelpers::getModPath();
	fPath += "\\eopData\\TempSaveData";
	filesystem::remove_all(fPath);
	filesystem::create_directory(fPath);
	std::string outFile = fPath;
	outFile += "\\settlementData.json";
	ofstream f1(outFile);
	jsn::json json = serialize();
	f1 << setw(4) << json;
	f1.close();
	return outFile;
	
}
void eopSettlementDataDb::onGameLoad(const std::vector<std::string>& filePaths)
{
	for (auto& path : filePaths)
	{
		if (path.find("settlementData.json") == string::npos)
			continue;
		jsn::json json;
		try
		{
			std::ifstream file(path);
			file >> json;
			file.close();
		}
		catch (jsn::json::parse_error& e)
		{
			MessageBoxA(nullptr, e.what(), "Warning!", MB_APPLMODAL | MB_SETFOREGROUND);
		}
		try
		{
			deserialize(json);
		}
		catch (jsn::json::exception& e)
		{
			MessageBoxA(nullptr, e.what(), "Warning!", MB_APPLMODAL | MB_SETFOREGROUND);
		}
		return;
	}
}

void eopSettlementDataDb::onGameLoaded()
{
	const auto stratMap = stratMapHelpers::getStratMap();
	if (!stratMap)
	{
		gameHelpers::logStringGame("eopSettlementDataDb.onGameLoaded: stratMap not found.");
		return;
	}
	for (int i = 0; i < stratMap->regionsNum; i++)
	{
		auto [settlementID, eopBuildingEntries, modelId, regionName, regionRebelsName] = eopSettData->at(i);
		const auto region = stratMapHelpers::getRegion(settlementID);
		if (!region)
			continue;
		if (!regionName.empty())
			region->changeRegionName(regionName.c_str());
		if (!regionRebelsName.empty())
			region->changeRebelsName(regionRebelsName.c_str());
		const auto sett = region->settlement;
		if (!sett)
			continue;
		for (int j = 0; j < sett->buildingsNum; j++)
		{
			if (const auto id = eopBuildingEntries[j]; id != 0)
			{
				if (const auto entry = buildEntryDB::getEopBuildEntry(id); entry)
				{
					if (const auto building = sett->getBuilding(j); building)
					{
						building->edbEntry = entry;
					}
				}
			}
		}
		if (modelId != -1)
			stratModelsChange::setModel(sett->xCoord, sett->yCoord, modelId, modelId);
	}
}

namespace settlementHelpers
{
	/*----------------------------------------------------------------------------------------------------------------*\
											 Settlement helpers
    \*----------------------------------------------------------------------------------------------------------------*/
#pragma region Settlement helpers

	settlementStruct* getSettlementByRegionID(int index)
	{
		const auto region = stratMapHelpers::getRegion(index);
		if (!region)
			return nullptr;
		return region->settlement;
	}
	
	void setSettlementOwner(settlementStruct* sett, factionStruct* newOwner, bool convertGarrison)
	{
		armyStruct* garrison = nullptr;
		std::vector<character*> characters;
		if (convertGarrison)
		{
			const int charNum = GAME_FUNC(int(__thiscall*)(settlementStruct*, int), getResidenceCharacterNum)(sett, 1);
			for (int i = 0; i < charNum; i++)
			{
				const auto thisChar = GAME_FUNC(character*(__thiscall*)(settlementStruct*, int, int), getResidenceCharacterAtIndex)(sett, i, 1);
				if (thisChar->genType->type != characterTypeStrat::spy || thisChar->characterRecord->faction == sett->faction)
				{
					DWORD funcAddr = codes::offsets.switchCharacterFaction;
					_asm
					{
						push 0
						push 0
						push newOwner
						mov ecx, thisChar
						mov eax, funcAddr
						call eax
					}
				}
				GAME_FUNC(void(__thiscall*)(character*), changeCharacterTileStuff)(thisChar);

				if (thisChar->genType->type == characterTypeStrat::namedCharacter)
				{
					characters.push_back(thisChar);
					DWORD funcAddr2 = codes::offsets.switchNamedCharacterFaction;
					_asm
					{
						push 0
						push newOwner
						mov ecx, thisChar
						mov eax, funcAddr2
						call eax
					}
				}
				GAME_FUNC(void(__thiscall*)(character*), initPlaceCharacter)(thisChar);
			}
			if (auto stack = sett->army)
			{
				garrison = stack;
				auto origFaction = stack->faction;
				DWORD funcAddr3 = codes::offsets.switchArmyFaction;
				_asm
				{
					push newOwner
					push stack
					mov ecx, origFaction
					mov eax, funcAddr3
					call eax
				}
			}
		}
		DWORD vtable = *reinterpret_cast<DWORD*>(sett);
		DWORD vFuncOffset = vtable + 0x9c;
		DWORD vFunc = *reinterpret_cast<DWORD*>(vFuncOffset);
		_asm
		{
			push 5
			push newOwner
			push 0
			mov ecx, sett
			mov eax, vFunc
			call eax
		}
		
		GAME_FUNC(void(__thiscall*)(settlementStruct*, int), removeSieges)(sett, 0);
		if (garrison)
		{
			if (sett->army && sett->army != garrison)
				armyHelpers::mergeArmies(garrison, sett->army);
			else
			{
				auto newGarrison = armyHelpers::createArmyInSettlement(sett);
				armyHelpers::mergeArmies(garrison, newGarrison);
			}
		}
		for (int i = 0; i < newOwner->stackNum; i++)
		{
			auto stack = newOwner->armies[i];
			if (!stack->gen)
				continue;
			if (stack->gen->xCoord == sett->xCoord && stack->gen->yCoord == sett->yCoord)
			{
				if (sett->army)
				{
					if (sett->army != stack)
						armyHelpers::mergeArmies(stack, sett->army);
				}
				else
				{
					auto newGarrison = armyHelpers::createArmyInSettlement(sett);
					armyHelpers::mergeArmies(stack, newGarrison);
				}	
			}
		}
		for (auto character : characters)
		{
			if (character && !character->ifMarkedToKill && character->genType->type == characterTypeStrat::namedCharacter)
			{
				if (!character->characterRecord->label || character->characterRecord->labelCrypt == 0)
				{
					auto label = std::string(character->characterRecord->shortName) + "_" + std::to_string(character->characterRecord->index);
					gameStringHelpers::setHashedString(&character->characterRecord->label, label.c_str());
				}
				int unitId = character->bodyguards->eduEntry->index;
				int xp = character->bodyguards->expScreen;
				int armour = character->bodyguards->avgArmourUpg;
				int weapon = character->bodyguards->avgWeaponUpg;
				characterHelpers::sendOffMap(character);
				if (auto newStack = armyHelpers::spawnArmy(
					newOwner,
					character->characterRecord->shortName,
					character->characterRecord->lastName,
					character->genType->type,
					character->characterRecord->label,
					character->characterRecord->portrait_custom,
					sett->xCoord,
					sett->yCoord,
					character->characterRecord->getAge(),
					false,
					character->characterRecord->originalFaction,unitId,xp,weapon,armour))
				{
					if (sett->army)
						armyHelpers::mergeArmies(newStack, sett->army);
					else
					{
						auto newGarrison = armyHelpers::createArmyInSettlement(sett);
						armyHelpers::mergeArmies(newStack, newGarrison);
					}
				}
			}
		}
		
		if (newOwner->factionHordeInfo && newOwner->factionHordeInfo->isHorde)
		{
			auto globalFort = reinterpret_cast<settlementStruct*>(dataOffsets::offsets.globalSett);
			globalFort = sett;
			auto hordeInfo = newOwner->factionHordeInfo;
			DWORD funcAddr4 = codes::offsets.doHordeStuff;
			_asm
			{
				mov ecx, hordeInfo
				mov eax, funcAddr4
				call eax
			}
		}
	}
	
	void changeOwner(settlementStruct* sett, factionStruct* newOwner)
	{
		setSettlementOwner(sett, newOwner, false);
	}
	
	void upgradeSettlement(settlementStruct* sett)
	{
		DWORD createBuildInSett = codes::offsets.createBuildInSett;
		DWORD upgradeBuildInSett = codes::offsets.upgradeBuildInSett;
		DWORD upgradeSett = codes::offsets.upgradeSett;
		int upgraded = 0;
		_asm
		{
			mov ecx, sett
			mov eax, upgradeSett
			call eax
			mov upgraded, eax
		}
		if (!upgraded)
		{
			gameHelpers::logStringGame("settlementStruct.upgrade: could not upgrade settlement.");
			return;
		}
		edbEntry* coreEntry;
		auto edb = eopBuildings::getEdb();
		
		if (sett->isCastle)
			coreEntry = edb->coreCastleBuilding;
		else
			coreEntry = edb->coreCityBuilding;

		building* existing = nullptr;
		for (int i = 0; i < sett->buildingsNum; i++)
		{
			if (sett->buildings[i]->edbEntry->buildingID == coreEntry->buildingID)
			{
				existing = sett->buildings[i];
				int factionId = sett->faction->factionID;
				int newLevel = existing->level + 1;
				_asm
				{
					push 1
					push factionId
					push newLevel
					mov ecx, existing
					mov eax, upgradeBuildInSett
					call eax
				}
				existing->constructionPercent = 0;
				existing->constructType = 5;
				if (existing->hp != 100)
					existing->setBuildingHealth(100);
				return;
			}
		}
		_asm
		{
			push 1
			push coreEntry
			push sett
			mov eax, createBuildInSett
			call eax
		}
			
	}
	
	float getReligion(settlementStruct* sett, int index)
	{
		const regionStruct* currRegion = &stratMapHelpers::getStratMap()->regions[sett->regionID];
		if (currRegion == nullptr)
			return 0.0f;
		return currRegion->religionsARR[index];
	}
	
	void setReligion(settlementStruct* sett, int index, float value)
	{
		const regionStruct* currRegion = &stratMapHelpers::getStratMap()->regions[sett->regionID];
		currRegion->religionsARR[index] = value;
	}
	
	std::string getSettlementName(settlementStruct* sett)
	{
		return gameStringHelpers::uniStringToStr(sett->localizedName);
	}
	
	void changeSettlementName(settlementStruct* sett, const char* newName)
	{
		const auto nameMem = new UNICODE_STRING*;
		sett->localizedName = nameMem;
		gameStringHelpers::createUniString(sett->localizedName, newName);
	}
	
	void createBuilding(settlementStruct* sett, const char* building_level_id)
	{
		DWORD adrFunc = codes::offsets.createBuildingFunc;
		string command = sett->name;
		command.push_back(' ');
		command += building_level_id;
		char buffer[100]{};
		const char* cmdC = command.c_str();
		_asm
		{
			lea eax, buffer
			push eax
			push cmdC
			mov eax, adrFunc
			call eax
		}
	}
	
	void destroyBuilding(settlementStruct* sett, const char* typeName, bool isReturnMoney)
	{

		DWORD adr = codes::offsets.destroyBuildingFunc;
		char** buildTypeS = gameStringHelpers::createHashedString(typeName);
		if (buildTypeS == nullptr)return;
		char* buildType = buildTypeS[0];
		char* hash = buildTypeS[1];
		_asm {
			push isReturnMoney
			push hash
			push buildType
			mov ecx, [sett]
			mov eax, [adr]
			call eax
		}
		return;
	}
	
#pragma endregion
	
	/*----------------------------------------------------------------------------------------------------------------*\
											 Construction Items
	\*----------------------------------------------------------------------------------------------------------------*/
#pragma region Construction items
	
	struct buildingOptionsDb
	{
		std::array<std::shared_ptr<settlementBuildingOptions>, 200> settOptions{};
	};
	std::shared_ptr<buildingOptionsDb> buildingOptionsDbPtr = std::make_shared<buildingOptionsDb>();
	
	bool addBuildingToQueue(buildingInQueue* building)
	{
		auto sett = building->settlement;
		if (!sett)
			return false;
		DWORD funcAddr = codes::offsets.addBuildingToQueue;
		int result = 0;
		_asm
		{
			push building
			mov ecx, sett
			mov eax, funcAddr
			call eax
			mov result, eax
		}
		return result != 0;
	}

	buildingInQueue* getBuildingInQueue(buildingsQueue* queue, int position)
	{
		if (position > 0 && position <= queue->buildingsInQueue) {
			int index = queue->firstIndex + position - 1;
			if (index > 5) { index = index - 6; }
			return &(queue->items[index]);
		}
		return nullptr;
	}
	
	int getAvailableBuildingsCount(const availableBuildings* list)
	{
		int count = 0;
		while (list)
		{
			count += list->buildingCount;
			list = list->next;
		}
		return count;
	}

	buildingInQueue* getBuildingOption(const availableBuildings* list, const int index)
	{
		int passed = 0;
		while (list)
		{
			if (index < passed + list->buildingCount)
			{
				return &list->buildingsList[index - passed];
			}
			passed += list->buildingCount;
			list = list->next;
		}
		return nullptr;
	}

	availableBuildings* getAvailableBuildingsMem()
	{
		return reinterpret_cast<availableBuildings*>(dataOffsets::offsets.constructionItems);
	}
	
	availableBuildings* getAvailableBuildings(settlementStruct* sett)
	{
		const auto mem = getAvailableBuildingsMem();
		GAME_FUNC(void(__thiscall*)(availableBuildings*), resetBuildingsMem)(mem);
		GAME_FUNC(void(__thiscall*)(settlementStruct*, availableBuildings*, int, int, int),
			getConstructionOptions)(sett, mem, 1, 1, 0);
		return mem;
	}

	int makeBuildOptionsHash(const settlementStruct* sett)
	{
		int hash = 0;
		for (int i = 0; i < sett->buildingsNum; i++)
			hash += sett->buildings[i]->edbEntry->buildingID;
		for (int i = sett->buildingsQueueArray.firstIndex; i <= sett->buildingsQueueArray.currentBuildingIndex; i++)
			if (const auto inQueue = &sett->buildingsQueueArray.items[i]; inQueue && inQueue->edbEntry)
				hash += inQueue->edbEntry->buildingID * 2;
		return hash;
	}
	
	settlementBuildingOptions* getBuildingOptions(settlementStruct* sett)
	{
		const int index = sett->regionID;
		const int turnNum = campaignHelpers::getCampaignData()->turnNumber;
		const int hash = makeBuildOptionsHash(sett);
		auto options = buildingOptionsDbPtr->settOptions[index];
		if (options && options->hash == hash && options->turn == turnNum)
			return options.get();
		
		buildingOptionsDbPtr->settOptions[index] = std::make_shared<settlementBuildingOptions>();
		options = buildingOptionsDbPtr->settOptions[index];
		options->hash = hash;
		options->turn = turnNum;
		const auto available = getAvailableBuildings(sett);
		options->count = getAvailableBuildingsCount(available);
		for (int i = 0; i < options->count; i++)
		{
			options->constructionOptions.push_back(std::make_shared<buildingInQueue>());
			*options->constructionOptions[i] = *getBuildingOption(available, i);
			options->totalCost += options->constructionOptions[i]->buildCost;
			options->totalTime += options->constructionOptions[i]->turnsToBuild;
		}
		return options.get();
	}
	
	buildingInQueue* getBuildingOptionFromDb(const settlementBuildingOptions* list, const int index)
	{
		return list->constructionOptions[index].get();
	}
	
	std::string getType(building* build)
	{
		return build->edbEntry->type;
	}
	std::string getName(building* build)
	{
		buildingLevel* level = &build->edbEntry->levels[build->level];
		return level->name;
	}
	std::string getQueueType(buildingInQueue* build)
	{
		return build->edbEntry->type;
	}
	std::string getQueueName(buildingInQueue* build)
	{
		buildingLevel* level = &build->edbEntry->levels[build->currentLevel];
		return level->name;
	}

	void addBuildingPool(edbEntry* entry, int level, int eduIndex, float initialSize, float gainPerTurn, float maxSize, int32_t exp, std::string condition)
	{
		const char* cond = condition.c_str();
		return eopBuildings::addBuildingPool(entry, level, eduIndex, initialSize, gainPerTurn, maxSize, exp, cond);
	}
	
#pragma endregion Construction items
	
	/*----------------------------------------------------------------------------------------------------------------*\
											 Recruitment Items
	\*----------------------------------------------------------------------------------------------------------------*/
#pragma region Recruitment items
	
	struct recruitOptionsDb
	{
		std::array<std::shared_ptr<settlementRecruitmentOptions>, 200> settRecruitOptions{};
	};
	std::shared_ptr<recruitOptionsDb> recruitOptionsDbPtr = std::make_shared<recruitOptionsDb>();
	
	bool addUnitToQueue(unitRQ* unit)
	{
		auto sett = unit->settlement;
		if (!sett)
			return false;
		DWORD funcAddr = codes::offsets.addUnitToQueue;
		int result = 0;
		_asm
		{
			push unit
			mov ecx, sett
			mov eax, funcAddr
			call eax
			mov result, eax
		}
		return result != 0;
	}

	recruitmentOptions* getAvailableUnitsMem()
	{
		return reinterpret_cast<recruitmentOptions*>(dataOffsets::offsets.recruitmentItems);
	}
	
	recruitmentOptions* getAvailableUnits(settlementStruct* sett)
	{
		const auto mem = getAvailableUnitsMem();
		GAME_FUNC(void(__thiscall*)(settlementStruct*, recruitmentOptions*), getRecruitmentOptions)(sett, mem);
		return mem;
	}

	recruitmentOptions* getAvailableRetrainingUnits(settlementStruct* sett)
	{
		const auto mem = getAvailableUnitsMem();
		GAME_FUNC(void(__thiscall*)(settlementStruct*, recruitmentOptions*), getRetrainingOptions)(sett, mem);
		return mem;
	}

	int makeRecruitOptionsHash(const settlementStruct* sett)
	{
		int hash = 0;
		for (int i = 0; i < sett->recruitmentPoolCount; i++)
		{
			hash += sett->recruitmentPools[i].eduIndex;
			hash += static_cast<int>(sett->recruitmentPools[i].availablePool * 100);
		}
		return hash;
	}
	
	settlementRecruitmentOptions* getRecruitOptions(settlementStruct* sett)
	{
		const int index = sett->regionID;
		const int turnNum = campaignHelpers::getCampaignData()->turnNumber;
		const int hash = makeRecruitOptionsHash(sett);
		auto options = recruitOptionsDbPtr->settRecruitOptions[index];
		if (options && options->hash == hash && options->turn == turnNum)
			return options.get();
		
		recruitOptionsDbPtr->settRecruitOptions[index] = std::make_shared<settlementRecruitmentOptions>();
		options = recruitOptionsDbPtr->settRecruitOptions[index];
		options->hash = hash;
		options->turn = turnNum;
		auto available = getAvailableUnits(sett);
		const int trainCount = (available->lastUnit - reinterpret_cast<DWORD>(available->units)) / sizeof(unitRQ);
		options->count = trainCount;
		for (int i = 0; i < options->count; i++)
		{
			options->recruitOptions.push_back(std::make_shared<unitRQ>());
			*options->recruitOptions[i] = available->units[i];
			options->totalCost += options->recruitOptions[i]->cost;
			options->totalTime += options->recruitOptions[i]->turnsToTrain;
		}
		available = getAvailableRetrainingUnits(sett);
		const int retrainCount = (available->lastUnit - reinterpret_cast<DWORD>(available->units)) / sizeof(unitRQ);
		options->count += retrainCount;
		for (int i = trainCount; i < options->count; i++)
		{
			options->recruitOptions.push_back(std::make_shared<unitRQ>());
			*options->recruitOptions[i] = available->units[i];
			options->totalCost += options->recruitOptions[i]->cost;
			options->totalTime += options->recruitOptions[i]->turnsToTrain;
		}
		return options.get();
	}
	unitRQ* getRecruitOptionFromDb(const settlementRecruitmentOptions* list, const int index)
	{
		return list->recruitOptions[index].get();
	}

#pragma endregion Recruitment items
	
	std::array<sol::table*, 200> settlementData;
	
	sol::table* getSettlementData(const settlementStruct* sett)
	{
		return settlementData[sett->regionID];
	}

	void setSettlementData(const settlementStruct* sett, sol::table* data)
	{
		settlementData[sett->regionID] = data;
	}

	void addToLua(sol::state& luaState)
	{
		struct
		{
			sol::usertype<settlementStruct>settlementStruct;
			sol::usertype<guild>guild;
			sol::usertype<building>building;
			sol::usertype<buildingsQueue>buildingsQueue;
			sol::usertype<buildingInQueue>buildingInQueue;
			sol::usertype<unitRQ>unitInQueue;
			sol::usertype<settlementCapability>settlementCapability;
			sol::usertype<recruitmentCapability>recruitmentCapability;
			sol::usertype<settlementRecruitmentPool>settlementRecruitmentPool;
			sol::usertype<settlementStats> settlementStats;
			sol::usertype<settlementBuildingOptions> constructionOptions;
			sol::usertype<settlementRecruitmentOptions> recruitmentOptions;
			sol::usertype<aiProductionController> aiProductionController;
		}types;
		
		///Settlement
		//@section Settlement

		/***
		Basic settlementStruct table

		@tfield int xCoord
		@tfield int yCoord
		@tfield character governor
		@tfield character lastGovernor
		@tfield character portAdmiral
		@tfield armyStruct army
		@tfield string name internal name of settlement
		@tfield string localizedName
		@tfield factionStruct ownerFaction
		@tfield changeOwner changeOwner
		@tfield int creatorFactionID
		@tfield int regionID
		@tfield int level
		@tfield int salliedOut
		@tfield int previousOwner
		@tfield int readyToSurrender
		@tfield int lastHordeFaction
		@tfield int moneySpentConstruction
		@tfield int moneySpentTraining
		@tfield int moneySpentRecruitment
		@tfield int timeSinceLastRebellion
		@tfield crusadeStruct takenByCrusade
		@tfield int isCastle
		@tfield int plagueDuration
		@tfield int scriptRebel
		@tfield int factionTradedFrom
		@tfield int maxHoldoutTurns
		@tfield int siegeCasualties
		@tfield int siegeDuration
		@tfield int subFactionID
		@tfield int yearFounded
		@tfield int isCapital
		@tfield int isTradeBlocked
		@tfield int harvestSuccess
		@tfield int baseFertility
		@tfield int loyaltyLastTurn
		@tfield int rebelFactionChance
		@tfield int plagued
		@tfield int plagueDeaths
		@tfield int turnsOwned start at 10 for settlements owned at game start without specification in descr_strat
		@tfield int populationSiegeStart
		@tfield int settlementTaxLevel
		@tfield int recruitmentPoolCount
		@tfield int recruitmentCapabilityNum
		@tfield int freezeRecruitmentPool
		@tfield int spiesInRecruitmentQueue
		@tfield int assassinsInRecruitmentQueue
		@tfield int diplomatsInRecruitmentQueue
		@tfield int admiralsInRecruitmentQueue
		@tfield int merchantsInRecruitmentQueue
		@tfield int priestsInRecruitmentQueue
		@tfield int foundingConversionRate
		@tfield int ownerConversionRate
		@tfield aiProductionController aiProductionController
		@tfield int unitInQueueCount
		@tfield int turmoil
		@tfield int governorDuration
		@tfield int isProvokedRebellion
		@tfield int publicHealth
		@tfield int populationSize
		@tfield int gatesAreOpened
		@tfield table savedData
		@tfield getReligion getReligion
		@tfield setReligion setReligion
		@tfield getGuildStanding getGuildStanding
		@tfield setGuildStanding setGuildStanding
		@tfield int buildingsNum
		@tfield getBuilding getBuilding
		@tfield createBuilding createBuilding
		@tfield destroyBuilding destroyBuilding
		@tfield buildingsQueue buildingsQueue
		@tfield int resourcesNum
		@tfield settlementStats settlementStats
		@tfield settlementStats settlementStatsLastTurn
		@tfield getResource getResource
		@tfield int siegesNum
		@tfield getSiege getSiege
		@tfield getSettlementCapability getSettlementCapability
		@tfield getAgentCapability getAgentCapability
		@tfield getAgentLimitCapability getAgentLimitCapability
		@tfield getRecruitmentCapability getRecruitmentCapability
		@tfield getSettlementRecruitmentPool getSettlementRecruitmentPool
		@tfield upgrade upgrade
		@tfield getConstructionOptions getConstructionOptions
		@tfield getRecruitmentOptions getRecruitmentOptions
		@tfield getUnitInQueue getUnitInQueue

		@table settlementStruct
		*/
		types.settlementStruct = luaState.new_usertype<settlementStruct>("settlementStruct");
		types.settlementStruct.set("xCoord", &settlementStruct::xCoord);
		types.settlementStruct.set("yCoord", &settlementStruct::yCoord);
		types.settlementStruct.set("governor", &settlementStruct::governor);
		types.settlementStruct.set("army", &settlementStruct::army);
		types.settlementStruct.set("name", sol::property(
			&settlementHelpers::getStringProperty<settlementStruct_name>, &settlementHelpers::setStringProperty<settlementStruct_name>
			));
		types.settlementStruct.set("localizedName", sol::property(
			&getSettlementName, &changeSettlementName
			));
		types.settlementStruct.set("ownerFaction", &settlementStruct::faction);
		/***
		Change owner faction of settlement. All agents, armies etc. leave the settlement.
		@function settlementStruct:changeOwner
		@tparam factionStruct newOwner Faction to change ownership to.
		@tparam bool convertGarrison
		@usage
		local campaign = M2TW.campaign
		local fac1 = campaign:getFaction("england")
		currSet:changeOwner(fac1, true)
		*/
		types.settlementStruct.set_function("changeOwner", sol::overload(
				sol::resolve<void(settlementStruct*, factionStruct*)>(changeOwner),
				sol::resolve<void(settlementStruct*, factionStruct*, bool)>(setSettlementOwner)
			));
		types.settlementStruct.set("creatorFactionID", &settlementStruct::fac_creatorModNum);
		types.settlementStruct.set("savedData", sol::property(
			&getSettlementData, &setSettlementData
			));
		types.settlementStruct.set("regionID", &settlementStruct::regionID);
		types.settlementStruct.set("level", &settlementStruct::level);
		types.settlementStruct.set("isCastle", &settlementStruct::isCastle);
		types.settlementStruct.set("loyaltyLastTurn", &settlementStruct::loyalty);
		types.settlementStruct.set("publicHealth", &settlementStruct::publicHealth);
		types.settlementStruct.set("scriptRebel", &settlementStruct::scriptRebel);
		types.settlementStruct.set("governorDuration", &settlementStruct::governorDuration);
		types.settlementStruct.set("portAdmiral", &settlementStruct::portAdmiral);
		types.settlementStruct.set("lastGovernor", &settlementStruct::lastGovernor);
		types.settlementStruct.set("timeSinceLastRebellion", &settlementStruct::timeSinceLastRebellion);
		types.settlementStruct.set("isTradeBlocked", &settlementStruct::isTradeBlocked);
		types.settlementStruct.set("plagueDuration", &settlementStruct::plagueDuration);
		types.settlementStruct.set("gatesAreOpened", &settlementStruct::gatesAreOpened);
		types.settlementStruct.set("previousOwner", &settlementStruct::previousOwner);
		types.settlementStruct.set("foundingConversionRate", &settlementStruct::foundingConversionRate);
		types.settlementStruct.set("ownerConversionRate", &settlementStruct::ownerConversionRate);
		types.settlementStruct.set("moneySpentConstruction", &settlementStruct::moneySpentConstruction);
		types.settlementStruct.set("moneySpentTraining", &settlementStruct::moneySpentTraining);
		types.settlementStruct.set("moneySpentRecruitment", &settlementStruct::moneySpentRecruitment);
		types.settlementStruct.set("lastHordeFaction", &settlementStruct::lastHordeFaction);
		types.settlementStruct.set("settlementTaxLevel", &settlementStruct::settlementTaxLevel);
		types.settlementStruct.set("salliedOut", &settlementStruct::salliedOut);
		types.settlementStruct.set("readyToSurrender", &settlementStruct::readyToSurrender);
		types.settlementStruct.set("siegeHoldoutTurns", &settlementStruct::maxHoldoutTurns);
		types.settlementStruct.set("takenByCrusade", &settlementStruct::takenByCrusade);
		types.settlementStruct.set("siegeCasualties", &settlementStruct::siegeCasualties);
		types.settlementStruct.set("unitInQueueCount", &settlementStruct::countRQ);
		types.settlementStruct.set("maxHoldoutTurns", &settlementStruct::maxHoldoutTurns);
		types.settlementStruct.set("siegeDuration", &settlementStruct::siegeDuration);
		types.settlementStruct.set("turnsSieged", &settlementStruct::siegeDuration);
		types.settlementStruct.set("factionTradedFrom", &settlementStruct::factionTradedFrom);
		types.settlementStruct.set("turnsOwned", &settlementStruct::turnsOwned);
		types.settlementStruct.set("subFactionID", &settlementStruct::subFactionID);
		types.settlementStruct.set("yearFounded", &settlementStruct::yearFounded);
		types.settlementStruct.set("isCapital", &settlementStruct::isCapital);
		types.settlementStruct.set("aiProductionController", &settlementStruct::aiProductionController);
		types.settlementStruct.set("harvestSuccess", &settlementStruct::harvestSuccess);
		types.settlementStruct.set("baseFertility", &settlementStruct::baseFertilityValue);
		types.settlementStruct.set("rebelFactionChance", &settlementStruct::rebelFactionChance);
		types.settlementStruct.set("plagued", &settlementStruct::plagued);
		types.settlementStruct.set("plagueDeaths", &settlementStruct::plagueDeaths);
		types.settlementStruct.set("populationSiegeStart", &settlementStruct::preSiegePopulation);
		types.settlementStruct.set("isProvokedRebellion", &settlementStruct::isProvokedRebellion);
		types.settlementStruct.set("populationSize", sol::property(&settlementStruct::getPopulation, &settlementStruct::setPopulation));
		types.settlementStruct.set("recruitmentPoolCount", &settlementStruct::recruitmentPoolCount);
		types.settlementStruct.set("freezeRecruitmentPool", &settlementStruct::freezeRecruitmentPool);
		types.settlementStruct.set("spiesInRecruitmentQueue", &settlementStruct::spiesInRecruitmentQueue);
		types.settlementStruct.set("assassinsInRecruitmentQueue", &settlementStruct::assassinsInRecruitmentQueue);
		types.settlementStruct.set("diplomatsInRecruitmentQueue", &settlementStruct::diplomatsInRecruitmentQueue);
		types.settlementStruct.set("admiralsInRecruitmentQueue", &settlementStruct::admiralsInRecruitmentQueue);
		types.settlementStruct.set("merchantsInRecruitmentQueue", &settlementStruct::merchantsInRecruitmentQueue);
		types.settlementStruct.set("priestsInRecruitmentQueue", &settlementStruct::priestsInRecruitmentQueue);
		types.settlementStruct.set("settlementStats", &settlementStruct::settlementStats);
		types.settlementStruct.set("settlementStatsLastTurn", &settlementStruct::settlementStatsLastTurn);
		types.settlementStruct.set("turmoil", &settlementStruct::turmoil);
		/***
		Get the settlement's specific regligion's value
		@function settlementStruct:getReligion
		@tparam int religionID In order of descr\_religions.txt, starting from 0
		@treturn float religionValue from 0 to 1
		@usage
		local firstRelVal = settlementStruct:getReligion(0) --get float of religion with ID 0
		*/
		types.settlementStruct.set_function("getReligion", &getReligion);
		/***
		Set the settlement's specific religion's value, make sure the sum of all religion values does not exceed 1.0!
		@function settlementStruct:setReligion
		@tparam int religionID in order of descr\_religions.txt, starting from 0
		@tparam float religionValue from 0 to 1
		@usage
		settlementStruct:setReligion(0, 0.5) --set religion with ID 0 as 50%
		*/
		types.settlementStruct.set_function("setReligion", &setReligion);
		/***
		Get a settlement's standing points with a specific guild by ID
		@function settlementStruct:getGuildStanding
		@tparam int guild_id
		@usage
		ourGuildStanding = settlementStruct:getGuildStanding(0)
		*/
		types.settlementStruct.set_function("getGuildStanding", &settlementStruct::getGuildStanding);
		/***
		Set the settlement's standing points with specific guild.
		@function settlementStruct:setGuildStanding
		@tparam int guild_id
		@tparam int standing
		@usage
		settlementStruct:setGuildStanding(0, 300)
		*/
		types.settlementStruct.set_function("setGuildStanding", &settlementStruct::setGuildStanding);
		types.settlementStruct.set("buildingsNum", &settlementStruct::buildingsNum);
		/***
		Get a specific building by it's index.
		@function settlementStruct:getBuilding
		@tparam int number
		@treturn building build
		@usage
		ourBuilding=settlementStruct:getBuilding(0);
		if(ourBuilding.level>1)
		then
			print("test");
		end
		*/
		types.settlementStruct.set_function("getBuilding", &settlementStruct::getBuilding);
		/***
		Create a building in the settlement.
		@function settlementStruct:createBuilding
		@tparam string building_level_id
		@usage
		settlementStruct:createBuilding("some_build1");
		*/
		types.settlementStruct.set_function("createBuilding", &createBuilding);
		/***
		Destroy a building of a specified type in the settlement.
		@function settlementStruct:destroyBuilding
		@tparam string typeName Type of building.
		@tparam bool isReturnMoney Should money be returned to the faction like with a manual desctruction.
		@usage
		settlementStruct:destroyBuilding("some_buildType",false);
		*/
		types.settlementStruct.set_function("destroyBuilding", &destroyBuilding);
		types.settlementStruct.set("buildingsQueue", &settlementStruct::buildingsQueueArray);
		types.settlementStruct.set("resourcesNum", &settlementStruct::resourcesNum);
		/***
		Get a specific resource by it's index.
		@function settlementStruct:getResource
		@tparam int number
		@treturn tradeResource resource
		@usage
		ourResource = settlementStruct:getResource(0)
		*/
		types.settlementStruct.set_function("getResource", &settlementStruct::getResource);
		types.settlementStruct.set("siegesNum", &settlementStruct::siegeNum);
		/***
		Get a specific siege by it's index
		@function settlementStruct:getSiege
		@tparam int siegeIdx
		@treturn siegeStruct siege
		@usage
		for i = 0, currSet.siegesNum-1 do
		   local siege=currSet:getSiege(i);
		   --etc
		end
		*/
		types.settlementStruct.set_function("getSiege", &settlementStruct::getSiege);
		/***
		Get a capability by capability type.
		@function settlementStruct:getSettlementCapability
		@tparam int capabilityType
		@treturn settlementCapability capability
		@usage
		local incomeBonus = settlementStruct:getSettlementCapability(55)
		*/
		types.settlementStruct.set_function("getSettlementCapability", &settlementStruct::getSettlementCapability);
		/***
		Get an agent capability by agent type (only recruitable agents) 0 = spy, 1 = assassin, 2 = diplomat, 3 = princess, 4 = merchant, 5 = priest.
		@function settlementStruct:getAgentCapability
		@tparam int agentType
		@treturn settlementCapability capability
		@usage
		local spyCap = settlementStruct:getAgentCapability(0)
		*/
		types.settlementStruct.set_function("getAgentCapability", &settlementStruct::getAgentCapability);
		/***
		Get an agent limit capability by agent type (only recruitable agents) 0 = spy, 1 = assassin, 2 = diplomat, 3 = princess, 4 = merchant, 5 = priest.
		@function settlementStruct:getAgentLimitCapability
		@tparam int agentType
		@treturn settlementCapability capability
		@usage
		local spyCapLimit = settlementStruct:getAgentLimitCapability(0)
		*/
		types.settlementStruct.set_function("getAgentLimitCapability", &settlementStruct::getAgentCapability);
		/***
		Get a recruitment capability by index (max 64!).
		@function settlementStruct:getRecruitmentCapability
		@tparam int index
		@treturn recruitmentCapability capability
		@usage
		local capability = settlementStruct:getRecruitmentCapability(0)
		*/
		types.settlementStruct.set_function("getRecruitmentCapability", &settlementStruct::getRecruitmentCapability);
		types.settlementStruct.set("recruitmentCapabilityNum", sol::property(&settlementStruct::getRecruitmentCapabilityNum));
		/***
		Get a recruitment pool by index.
		@function settlementStruct:getSettlementRecruitmentPool
		@tparam int index
		@treturn settlementRecruitmentPool pool
		@usage
		local pool = settlementStruct:getSettlementRecruitmentPool(0)
		*/
		types.settlementStruct.set_function("getSettlementRecruitmentPool", &settlementStruct::getSettlementRecruitmentPool);
		/***
		Upgrade a settlement to the next level.
		@function settlementStruct:upgrade
		@usage
		      settlement:upgrade()
		*/
		types.settlementStruct.set_function("upgrade", &upgradeSettlement);
		/***
		Get available construction items.
		@function settlementStruct:getConstructionOptions
		@treturn constructionOptions options
		@usage
		      local items = settlement:getConstructionOptions()
		*/
		types.settlementStruct.set_function("getConstructionOptions", &getBuildingOptions);
		/***
		Get available recruitment items.
		@function settlementStruct:getRecruitmentOptions
		@treturn recruitmentOptions options
		@usage
		      local items = settlement:getRecruitmentOptions()
		*/
		types.settlementStruct.set_function("getRecruitmentOptions", &getRecruitOptions);
		/***
		Get unit in queue.
		@function settlementStruct:getUnitInQueue
		@tparam int index
		@treturn unitInQueue unit
		@usage
		      local item = settlement:getUnitInQueue(1)
		*/
		types.settlementStruct.set_function("getUnitInQueue", &settlementStruct::getUnitInQueue);
		/***
		Create an army in a settlement (don't need a character). Used to add units to an empty settlement.
		@function settlementStruct:createArmyInSettlement
		@tparam settlementStruct settlement
		@treturn armyStruct army
		@usage
		if sett.army == nil then
		      local army = sett:createArmyInSettlement();
		end
		*/
		types.settlementStruct.set_function("createArmyInSettlement", &armyHelpers::createArmyInSettlement);
		
		/***
		Basic settlementStats table

		@tfield int PopGrowthBaseFarm - Get only
		@tfield int population
		@tfield int PopGrowthFarms - Get only
		@tfield int PopGrowthHealth - Get only
		@tfield int PopGrowthBuildings - Get only
		@tfield int PopGrowthTaxBonus - Get only
		@tfield int PopGrowthEntertainment - Get only
		@tfield int PopGrowthTrade - Get only
		@tfield int PopGrowthGovernorInfluence - Get only
		@tfield int PopGrowthSqualor - Get only
		@tfield int PopGrowthPlague - Get only
		@tfield int PopGrowthTaxPenalty - Get only
		@tfield int PublicOrderGarrison - Get only
		@tfield int PublicOrderLaw - Get only
		@tfield int PublicOrderBuildingsEntertainment - Get only
		@tfield int PublicOrderGovernorInfluence - Get only
		@tfield int PublicOrderTaxBonus - Get only
		@tfield int PublicOrderTriumph - Get only
		@tfield int PublicOrderPopulationBoom - Get only
		@tfield int PublicOrderEntertainment - Get only
		@tfield int PublicOrderHealth - Get only
		@tfield int PublicOrderReligiousOrder - Get only
		@tfield int PublicOrderFear - Get only
		@tfield int PublicOrderGlory - Get only
		@tfield int PublicOrderSqualor - Get only
		@tfield int PublicOrderDistanceToCapital - Get only
		@tfield int PublicOrderNoGovernance - Get only
		@tfield int PublicOrderTaxPenalty - Get only
		@tfield int PublicOrderUnrest - Get only
		@tfield int PublicOrderBesieged - Get only
		@tfield int PublicOrderBlockaded - Get only
		@tfield int PublicOrderCulturalUnrest - Get only
		@tfield int PublicOrderExcommunication - Get only
		@tfield int PublicOrder - Get only
		@tfield int FarmsIncome - Get only
		@tfield int TaxesIncome - Get only
		@tfield int MiningIncome - Get only
		@tfield int TradeIncome - Get only
		@tfield int DiplomaticIncome - Get only
		@tfield int DemolitionIncome - Get only
		@tfield int LootingIncome - Get only
		@tfield int BuildingsIncome - Get only
		@tfield int AdminIncome - Get only
		@tfield int ConstructionExpense - Get only
		@tfield int RecruitmentExpense - Get only
		@tfield int DiplomaticExpense - Get only
		@tfield int CorruptionExpense - Get only
		@tfield int EntertainmentExpense - Get only
		@tfield int DevastationExpense - Get only
		@tfield int TotalIncomeWithoutAdmin - Get only
		@tfield int majorityReligionID

		@table settlementStats
		*/
		types.settlementStats = luaState.new_usertype<settlementStats>("settlementStats");
		types.settlementStats.set("population", &settlementStats::population);
		types.settlementStats.set("PopGrowthBaseFarm", &settlementStats::PopGrowthBaseFarm);
		types.settlementStats.set("PopGrowthFarms", &settlementStats::PopGrowthFarms);
		types.settlementStats.set("PopGrowthHealth", &settlementStats::PopGrowthHealth);
		types.settlementStats.set("PopGrowthBuildings", &settlementStats::PopGrowthBuildings);
		types.settlementStats.set("PopGrowthTaxBonus", &settlementStats::PopGrowthTaxBonus);
		types.settlementStats.set("PopGrowthEntertainment", &settlementStats::PopGrowthEntertainment);
		types.settlementStats.set("PopGrowthTrade", &settlementStats::PopGrowthTrade);
		types.settlementStats.set("PopGrowthGovernorInfluence", &settlementStats::PopGrowthGovernorInfluence);
		types.settlementStats.set("PopGrowthSqualor", &settlementStats::PopGrowthSqualor);
		types.settlementStats.set("PopGrowthPlague", &settlementStats::PopGrowthPlague);
		types.settlementStats.set("PopGrowthTaxPenalty", &settlementStats::PopGrowthTaxPenalty);
		types.settlementStats.set("PublicOrderGarrison", &settlementStats::PublicOrderGarrison);
		types.settlementStats.set("PublicOrderLaw", &settlementStats::PublicOrderLaw);
		types.settlementStats.set("PublicOrderBuildingsEntertainment", &settlementStats::PublicOrderBuildingsEntertainment);
		types.settlementStats.set("PublicOrderGovernorInfluence", &settlementStats::PublicOrderGovernorInfluence);
		types.settlementStats.set("PublicOrderTaxBonus", &settlementStats::PublicOrderTaxBonus);
		types.settlementStats.set("PublicOrderTriumph", &settlementStats::PublicOrderTriumph);
		types.settlementStats.set("PublicOrderPopulationBoom", &settlementStats::PublicOrderPopulationBoom);
		types.settlementStats.set("PublicOrderEntertainment", &settlementStats::PublicOrderEntertainment);
		types.settlementStats.set("PublicOrderHealth", &settlementStats::PublicOrderHealth);
		types.settlementStats.set("PublicOrderReligiousOrder", &settlementStats::PublicOrderReligiousOrder);
		types.settlementStats.set("PublicOrderFear", &settlementStats::PublicOrderFear);
		types.settlementStats.set("PublicOrderGlory", &settlementStats::PublicOrderGlory);
		types.settlementStats.set("PublicOrderSqualor", &settlementStats::PublicOrderSqualor);
		types.settlementStats.set("PublicOrderDistanceToCapital", &settlementStats::PublicOrderDistanceToCapital);
		types.settlementStats.set("PublicOrderNoGovernance", &settlementStats::PublicOrderNoGovernance);
		types.settlementStats.set("PublicOrderTaxPenalty", &settlementStats::PublicOrderTaxPenalty);
		types.settlementStats.set("PublicOrderUnrest", &settlementStats::PublicOrderUnrest);
		types.settlementStats.set("PublicOrderBesieged", &settlementStats::PublicOrderBesieged);
		types.settlementStats.set("PublicOrderBlockaded", &settlementStats::PublicOrderBlockaded);
		types.settlementStats.set("PublicOrderCulturalUnrest", &settlementStats::PublicOrderCulturalUnrest);
		types.settlementStats.set("PublicOrderExcommunication", &settlementStats::PublicOrderExcommunication);
		types.settlementStats.set("PublicOrder", &settlementStats::PublicOrder);
		types.settlementStats.set("FarmsIncome", &settlementStats::FarmsIncome);
		types.settlementStats.set("TaxesIncome", &settlementStats::TaxesIncome);
		types.settlementStats.set("MiningIncome", &settlementStats::MiningIncome);
		types.settlementStats.set("TradeIncome", &settlementStats::TradeIncome);
		types.settlementStats.set("DiplomaticIncome", &settlementStats::DiplomaticIncome);
		types.settlementStats.set("DemolitionIncome", &settlementStats::DemolitionIncome);
		types.settlementStats.set("LootingIncome", &settlementStats::LootingIncome);
		types.settlementStats.set("BuildingsIncome", &settlementStats::BuildingsIncome);
		types.settlementStats.set("AdminIncome", &settlementStats::AdminIncome);
		types.settlementStats.set("ConstructionExpense", &settlementStats::ConstructionExpense);
		types.settlementStats.set("RecruitmentExpense", &settlementStats::RecruitmentExpense);
		types.settlementStats.set("DiplomaticExpense", &settlementStats::DiplomaticExpense);
		types.settlementStats.set("CorruptionExpense", &settlementStats::CorruptionExpense);
		types.settlementStats.set("EntertainmentExpense", &settlementStats::EntertainmentExpense);
		types.settlementStats.set("DevastationExpense", &settlementStats::DevastationExpense);
		types.settlementStats.set("TotalIncomeWithoutAdmin", &settlementStats::TotalIncomeWithoutAdmin);
		types.settlementStats.set("majorityReligionID", &settlementStats::TotalIncomeWithoutAdmin);

		/***
		Basic settlementCapability table

		@tfield int value
		@tfield int bonus

		@table settlementCapability
		*/
		types.settlementCapability = luaState.new_usertype<settlementCapability>("settlementCapability");
		types.settlementCapability.set("value", &settlementCapability::value);
		types.settlementCapability.set("bonus", &settlementCapability::bonus);

		/***
		Basic recruitmentCapability table

		@tfield int eduIndex
		@tfield int xp
		@tfield float initialSize
		@tfield float replenishRate
		@tfield float maxSize

		@table recruitmentCapability
		*/
		types.recruitmentCapability = luaState.new_usertype<recruitmentCapability>("recruitmentCapability");
		types.recruitmentCapability.set("eduIndex", &recruitmentCapability::eduIndex);
		types.recruitmentCapability.set("xp", &recruitmentCapability::xp);
		types.recruitmentCapability.set("initialSize", &recruitmentCapability::initialSize);
		types.recruitmentCapability.set("replenishRate", &recruitmentCapability::replenishRate);
		types.recruitmentCapability.set("maxSize", &recruitmentCapability::maxSize);
		
		/***
		Basic settlementRecruitmentPool table

		@tfield int eduIndex
		@tfield float availablePool

		@table settlementRecruitmentPool
		*/
		types.settlementRecruitmentPool = luaState.new_usertype<settlementRecruitmentPool>("settlementRecruitmentPool");
		types.settlementRecruitmentPool.set("eduIndex", &settlementRecruitmentPool::eduIndex);
		types.settlementRecruitmentPool.set("availablePool", &settlementRecruitmentPool::availablePool);
		
		/***
		Basic buildingsQueue table

		@tfield int currentlyBuilding position in queue of building currently under construction, usually 1
		@tfield int numBuildingsInQueue maximum is 6
		@tfield getBuildingInQueue getBuildingInQueue by position in queue (1-6)

		@table buildingsQueue
		*/
		types.buildingsQueue = luaState.new_usertype<buildingsQueue>("buildingsQueue");
		types.buildingsQueue.set("currentlyBuilding", &buildingsQueue::currentBuildingIndex);
		types.buildingsQueue.set("numBuildingsInQueue", &buildingsQueue::buildingsInQueue);
		/***
		Get building in queue by position

		@function buildingsQueue:getBuildingInQueue
		@tparam int position
		@treturn buildingInQueue buildingInQueue
		@usage
		if ourQueue.numBuildingsInQueue > 0 then
			local result = "ourQueue:\n\t"
			for i = 1, ourQueue.numBuildingsInQueue, 1 do
				local ourQueueBuld = ourQueue:getBuildingInQueue(i)
				result = result..i.." "..ourQueueBuld:getQueueBuildingName().."\n\t"
			end
			print(result)
		end
		*/
		types.buildingsQueue.set_function("getBuildingInQueue", &getBuildingInQueue);
		
		///Construction Options
		//@section Construction Options

		/***
		Basic constructionOptions table
		
		@tfield int buildingNum
		@tfield int totalCost
		@tfield int totalTime
		@tfield getConstructionOption getConstructionOption
		
		@table constructionOptions
		*/
		types.constructionOptions = luaState.new_usertype<settlementBuildingOptions>("constructionOptions");
		types.constructionOptions.set("buildingNum", &settlementBuildingOptions::count);
		types.constructionOptions.set("totalCost", &settlementBuildingOptions::totalCost);
		types.constructionOptions.set("totalTime", &settlementBuildingOptions::totalTime);
		
		/***
		Get an available construction item.
		@function constructionOptions:getConstructionOption
		@tparam int index
		@treturn buildingInQueue building
		@usage
			 local building = constructionOptions:getConstructionOption(0)
		*/
		types.constructionOptions.set_function("getConstructionOption", &getBuildingOptionFromDb);
		
		/***
		Basic buildingInQueue table

		@tfield building building Is nil if building doesn't exist yet.
		@tfield settlementStruct settlement
		@tfield int currentLevel
		@tfield edbEntry edbEntry
		@tfield int constructionType 0 = upgrade, 1 = normal, 4 = convert settlement
		@tfield int previousLevel
		@tfield int buildCost
		@tfield int buildTurnsPassed
		@tfield int buildTurnsRemaining
		@tfield int percentBuilt
		@tfield getQueueBuildingType getQueueBuildingType
		@tfield getQueueBuildingName getQueueBuildingName
		@tfield addBuildingToQueue addBuildingToQueue

		@table buildingInQueue
		*/
		types.buildingInQueue = luaState.new_usertype<buildingInQueue>("buildingInQueue");
		types.buildingInQueue.set("building", &buildingInQueue::existsBuilding);
		types.buildingInQueue.set("edbEntry", &buildingInQueue::edbEntry);
		types.buildingInQueue.set("constructionType", &buildingInQueue::constructType);
		types.buildingInQueue.set("settlement", &buildingInQueue::settlement);
		types.buildingInQueue.set("currentLevel", &buildingInQueue::currentLevel);
		types.buildingInQueue.set("previousLevel", &buildingInQueue::pastLevel);
		types.buildingInQueue.set("buildCost", &buildingInQueue::buildCost);
		types.buildingInQueue.set("buildTurnsPassed", &buildingInQueue::buildTurnsPassed);
		types.buildingInQueue.set("buildTurnsRemaining", &buildingInQueue::turnsToBuild);
		types.buildingInQueue.set("percentBuilt", &buildingInQueue::percentBuild);
		/***
		Get name of building in queue type (chain)

		@function buildingInQueue:getQueueBuildingType
		@treturn string buildingType (building chain name)
		@usage
		if ourQueueBld:getQueueBuildingType() = "core_building" then
			--do stuff
		end
		*/
		types.buildingInQueue.set_function("getQueueBuildingType", &getQueueType);
		/***
		Get name of building in queue level

		@function buildingInQueue:getQueueBuildingName
		@treturn string buildingName
		@usage
		if ourQueueBld:getQueueBuildingName() = "wooden_pallisade" then
			--do stuff
		end
		*/
		types.buildingInQueue.set_function("getQueueBuildingName", &getQueueName);
		/***
		Add a building to the construction queue.
		@function buildingInQueue:addBuildingToQueue
		@treturn bool success
		@usage
			build:addBuildingToQueue()
		*/
		types.buildingInQueue.set_function("addBuildingToQueue", &addBuildingToQueue);
		
		///Recruitment Options
		//@section Recruitment Options

		/***
		Basic recruitmentOptions table
	
		@tfield int unitNum
		@tfield int totalCost
		@tfield int totalTime
		@tfield getRecruitmentOption getRecruitmentOption
	
		@table recruitmentOptions
		*/
		types.recruitmentOptions = luaState.new_usertype<settlementRecruitmentOptions>("recruitmentOptions");
		types.recruitmentOptions.set("unitNum", &settlementRecruitmentOptions::count);
		types.recruitmentOptions.set("totalCost", &settlementRecruitmentOptions::totalCost);
		types.recruitmentOptions.set("totalTime", &settlementRecruitmentOptions::totalTime);
	
		/***
		Get an available recruitment item.
		@function recruitmentOptions:getRecruitmentOption
		@tparam int index
		@treturn unitInQueue item
		@usage
			 local item = recruitmentOptions:getRecruitmentOption(0)
		*/
		types.recruitmentOptions.set_function("getRecruitmentOption", &getRecruitOptionFromDb);

		/***
		Basic unitInQueue table

		@tfield int recruitType 0 = normal, 1 = ship, 2 = agent, 3 = retraining 4 = retraining ship
		@tfield int experience
		@tfield int armourUpg
		@tfield int weaponUpg
		@tfield eduEntry eduEntry
		@tfield int agentType
		@tfield int soldierCount
		@tfield int cost
		@tfield int recruitTime
		@tfield int turnsTrained
		@tfield settlementStruct settlement
		@tfield int turnNumber
		@tfield int isMercenary
		@tfield addUnitToQueue addUnitToQueue

		@table unitInQueue
		*/
		types.unitInQueue = luaState.new_usertype<unitRQ>("unitInQueue");
		types.unitInQueue.set("recruitType", &unitRQ::recruitType);
		types.unitInQueue.set("experience", &unitRQ::experience);
		types.unitInQueue.set("armourUpg", &unitRQ::armourUpg);
		types.unitInQueue.set("weaponUpg", &unitRQ::weaponUpgrade);
		types.unitInQueue.set("eduEntry", sol::property(&unitRQ::getUnitEntry, &unitRQ::setUnitEntry));
		types.unitInQueue.set("agentType", sol::property(&unitRQ::getAgentType, &unitRQ::setAgentType));
		types.unitInQueue.set("soldierCount", &unitRQ::soldierCount);
		types.unitInQueue.set("cost", &unitRQ::cost);
		types.unitInQueue.set("recruitTime", &unitRQ::turnsToTrain);
		types.unitInQueue.set("turnsTrained", &unitRQ::turnsTrainedAlready);
		types.unitInQueue.set("settlement", &unitRQ::settlement);
		types.unitInQueue.set("turnNumber", &unitRQ::turnNumber);
		types.unitInQueue.set("isMercenary", &unitRQ::isMercenary);
		
		/***
		Add a unit to the recruitment queue.
		@function unitInQueue:addUnitToQueue
		@treturn bool success
		@usage
			unitOption:addUnitToQueue()
		*/
		types.unitInQueue.set_function("addUnitToQueue", &addUnitToQueue);
		
		///Building
		//@section Building

		/***
		Basic building table

		@tfield int level
		@tfield int hp
		@tfield int factionID
		@tfield settlementStruct settlement
		@tfield edbEntry edbEntry
		@tfield getType getType
		@tfield getName getName

		@table building
		*/
		types.building = luaState.new_usertype<building>("building");
		types.building.set("level", &building::level);
		types.building.set("hp", sol::property( &building::getBuildingHealth,  &building::setBuildingHealth));
		types.building.set("settlement", &building::settlement);
		types.building.set("edbEntry", &building::edbEntry);
		types.building.set("factionID", &building::factionID);
		
		/***
		Get the name of the building type (the building chain in export\_descr\_buildings.txt).

		@function building:getType
		@treturn string buildingType (building chain name)
		@usage
		if building:getType() == "core_building" then
			--do stuff
		end
		*/
		types.building.set_function("getType", &getType);
		/***
		Get name of building level (as per export\_descr\_buildings.txt).

		@function building:getName
		@treturn string buildingName
		@usage
		if building:getName() == "large_stone_wall" then
			--do stuff
		end
		*/
		types.building.set_function("getName", &getName);

		//types.building.set_function("addCapability", &buildingStructHelpers::addCapability);
		
		///AI Production Controller
		//@section AI Production Controller

		/***
		Basic aiProductionController table

		@tfield aiFaction aiFaction
		@tfield int regionID
		@tfield settlementStruct settlement
		@tfield int autoManagePolicy
		@tfield bool isAutoManaged
		@tfield bool isAutoManagedRecruitment
		@tfield bool isAutoManagedConstruction
		@tfield bool isAutoManagedTaxes
		@tfield int spyBias
		@tfield int assassinBias
		@tfield int diplomatBias
		@tfield int admiralBias
		@tfield int priestBias
		@tfield int merchantBias
		@tfield setConstructionValue setConstructionValue
		@tfield setRecruitmentValue setRecruitmentValue
		@tfield getConstructionValue getConstructionValue
		@tfield getRecruitmentValue getRecruitmentValue
		@tfield setExtraBias setExtraBias
		@tfield getExtraBias getExtraBias

		@table aiProductionController
		*/
		types.aiProductionController = luaState.new_usertype<aiProductionController>("aiProductionController");
		types.aiProductionController.set("aiFaction", &aiProductionController::aiFaction);
		types.aiProductionController.set("regionID", &aiProductionController::regionID);
		types.aiProductionController.set("settlement", &aiProductionController::settlement);
		types.aiProductionController.set("autoManagePolicy", &aiProductionController::autoManagePolicy);
		types.aiProductionController.set("isAutoManaged", &aiProductionController::isAutoManaged);
		types.aiProductionController.set("isAutoManagedRecruitment", &aiProductionController::isAutoManagedRecruitment);
		types.aiProductionController.set("isAutoManagedConstruction", &aiProductionController::isAutoManagedConstruction);
		types.aiProductionController.set("isAutoManagedTaxes", &aiProductionController::isAutoManagedTaxes);
		types.aiProductionController.set("spyBias", &aiProductionController::spyBias);
		types.aiProductionController.set("assassinBias", &aiProductionController::assassinBias);
		types.aiProductionController.set("diplomatBias", &aiProductionController::diplomatBias);
		types.aiProductionController.set("admiralBias", &aiProductionController::admiralBias);
		types.aiProductionController.set("priestBias", &aiProductionController::priestBias);
		types.aiProductionController.set("merchantBias", &aiProductionController::merchantBias);
		/***
		Set bias value of the ai personality for a capability.
		@function aiProductionController:setConstructionValue
		@tparam int type use building capabilities enum
		@tparam int value
		@usage
			 aiProductionController:setConstructionValue(buildingCapability.law_bonus, 100)
		*/
		types.aiProductionController.set_function("setConstructionValue", &aiProductionController::setConstructionValueSett);
		/***
		Set bias value of the ai personality for a recruitment class.
		@function aiProductionController:setRecruitmentValue
		@tparam int type use unitCategoryClass enum
		@tparam int value
		@usage
			 aiProductionController:setRecruitmentValue(unitCategoryClass.heavyCavalry, 100)
		*/
		types.aiProductionController.set_function("setRecruitmentValue", &aiProductionController::setRecruitmentValueSett);
		/***
		Set bias value of the ai personality.
		@function aiProductionController:setExtraBias
		@tparam int type use productionBias enum
		@tparam int value
		@usage
			 aiProductionController:setExtraBias(productionBias.trade, 100)
		*/
		types.aiProductionController.set_function("setExtraBias", &aiProductionController::setExtraBias);
		/***
		Get bias value of the ai personality.
		@function aiProductionController:getExtraBias
		@tparam int type use productionBias enum
		@treturn int value
		@usage
			 aiProductionController:getExtraBias(productionBias.trade)
		*/
		types.aiProductionController.set_function("getExtraBias", &aiProductionController::getExtraBias);
		/***
		Get bias value of the ai personality for a capability.
		@function aiProductionController:getConstructionValue
		@tparam int type use building capabilities enum
		@treturn int value
		@usage
			local value = aiProductionController:setConstructionValue(buildingCapability.law_bonus)
		*/
		types.aiProductionController.set_function("getConstructionValue", &aiProductionController::getConstructionValueSett);
		/***
		Get bias value of the ai personality for a recruitment class.
		@function aiProductionController:getRecruitmentValue
		@tparam int type use unitCategoryClass enum
		@treturn int value
		@usage
			local value = aiProductionController:setRecruitmentValue(unitCategoryClass.heavyCavalry)
		*/
		types.aiProductionController.set_function("getRecruitmentValue", &aiProductionController::getRecruitmentValueSett);
		
		///Guild
		//@section Guild

		/***
		Basic guild table

		@tfield string name
		@tfield int id
		@tfield edbEntry entry
		@tfield int level1
		@tfield int level2
		@tfield int level3

		@table guild
		*/
		types.guild = luaState.new_usertype<guild>("guild");
		types.guild.set("name", sol::property(
			&getGuildStringProperty<guild_name>, &setGuildStringProperty<guild_name>));
		types.guild.set("id", &guild::id);
		types.guild.set("level1", &guild::level1Threshold);
		types.guild.set("level2", &guild::level2Threshold);
		types.guild.set("level3", &guild::level3Threshold);
		types.guild.set("entry", &guild::building);
	}
	
}