﻿///
//![Lua logo](../Lua.png)
//M2TWEOP structures and functions. There are not many examples and descriptions here. Also note that the examples do not include many of the checks that would be required when creating modifications.
//@module LuaPlugin
//@author Fynn
//@license GPL-3.0
#include "pch.h"
#include "army.h"
#include "campaign.h"
#include "cultures.h"
#include "dataOffsets.h"
#include "eopdu.h"
#include "faction.h"
#include "fort.h"
#include "gameHelpers.h"
#include "luaPlugin.h"
#include "strategyMap.h"

settlementStruct* siegeS::getSiegedSettlement()
{
    return stratMapHelpers::getTile(goal->xCoord, goal->yCoord)->getSettlement();
}

fortStruct* siegeS::getSiegedFort()
{
    return stratMapHelpers::getTile(goal->xCoord, goal->yCoord)->getFort();
}
    
std::pair<int, int> armyStruct::getCoords()
{
	if (gen != nullptr)
		return { gen->xCoord, gen->yCoord };
	if (settlement != nullptr)
		return { settlement->xCoord, settlement->yCoord };
	return { -1,-1 };
}
	
void armyStruct::nullifyMovePoints()
{
	for (int i = 0; i < numOfUnits; ++i)
		unitHelpers::setUnitMovePoints(units[i], 0);
}

int armyStruct::attackArmy(armyStruct* defender)
{
	if (!defender || defender->settlement)
		return 0;
	auto attacker = this;
	DWORD funcAddr = codes::offsets.attackArmy;
	_asm
	{
		push defender
		mov ecx, attacker
		mov eax, funcAddr
		call eax
	}
	return 1;
}
settlementStruct* armyStruct::findInSettlement()
{
	if (!settlement)
		return nullptr;
	return stratMapHelpers::getTile(settlement->xCoord, settlement->yCoord)->getSettlement();
}
fortStruct* armyStruct::findInFort()
{
	if (!settlement)
		return nullptr;
	return stratMapHelpers::getTile(settlement->xCoord, settlement->yCoord)->getFort();
}
void armyStruct::sortStack(int sortType, int sortType2, int sortType3)
{
		std::vector<int> sortTypes = { sortType };
		if (sortType2 != 0)
			sortTypes.push_back(sortType2);
		if (sortType3 != 0)
			sortTypes.push_back(sortType3);
		// Lambda function to compare unit objects based on multiple criteria
		auto compareUnits = [&sortTypes](const unit* unitA, const unit* unitB)
		{
			// Ensuring Generals stay at the front/start of the stack
			if (unitA->general && !unitB->general)
				return true;
			if (!unitA->general && unitB->general)
				return false;
			for (const int sType : sortTypes)
			{
				switch (sType)
				{
					case 1: // By EDU Type
						if (std::string(unitA->eduEntry->eduType) != std::string(unitB->eduEntry->eduType))
							return std::string(unitA->eduEntry->eduType) < std::string(unitB->eduEntry->eduType);
						break;
					case 2: // By Category
						if (unitA->eduEntry->category != unitB->eduEntry->category)
							return unitA->eduEntry->category < unitB->eduEntry->category;
						break;
					case 3: // By Class
						if (unitA->eduEntry->unitClass != unitB->eduEntry->unitClass)
							return unitA->eduEntry->unitClass < unitB->eduEntry->unitClass;
						break;
					case 4: // Soldier Count
						if (unitA->SoldierCountStrat != unitB->SoldierCountStrat)
							return unitA->SoldierCountStrat > unitB->SoldierCountStrat;
						break;
					case 5: // Experience
						if (unitA->expScreen != unitB->expScreen)
							return unitA->expScreen > unitB->expScreen;
						break;
					case 6: // categoryClass
						if (unitA->eduEntry->unitProductionClass != unitB->eduEntry->unitProductionClass)
							return unitA->eduEntry->unitProductionClass < unitB->eduEntry->unitProductionClass;
						break;
					case 7: // aiUnitValue
						if (unitA->eduEntry->aiUnitValuePerSoldier != unitB->eduEntry->aiUnitValuePerSoldier)
							return unitA->eduEntry->aiUnitValuePerSoldier < unitB->eduEntry->aiUnitValuePerSoldier;
						break;
					default:
						if (std::string(unitA->eduEntry->eduType) != std::string(unitB->eduEntry->eduType))
							return std::string(unitA->eduEntry->eduType) < std::string(unitB->eduEntry->eduType);
						break;
				}
			}
			return false;
		};
		// Stable sort the units
		std::stable_sort(units, units + numOfUnits, compareUnits);
}
unit* armyStruct::createUnit(const char* type, const int exp, const int arm, const int weapon)
{
	unit *newUnit = unitHelpers::createUnitN(type, regionID, faction->factionID, exp, arm, weapon);
	if (newUnit == nullptr)
		return newUnit;
	armyHelpers::addUnitToArmy(this, newUnit);
	return newUnit;
}
unit* armyStruct::createUnitByIndex(const int eduIndex, const int exp, const int arm, const int weapon)
{
	unit *newUnit = unitHelpers::createUnitIdx(eduIndex, regionID, faction->factionID, exp, arm, weapon);
	if (newUnit == nullptr)
		newUnit = createEopUnit(eduIndex, exp, arm, weapon);
	if (newUnit == nullptr)
		return newUnit;
	armyHelpers::addUnitToArmy(this, newUnit);
	return newUnit;
}
unit* armyStruct::createEopUnit(const int eduIndex, const int exp, const int arm, const int weapon)
{
	unit *newUnit = createUnitByIndex(eduIndex, exp, arm, weapon);
	if (newUnit == nullptr)
		return newUnit;
	/*const int eopIdx = eopDu::getDataEopEdu(eduIndex);
	if (eopIdx == 0)
		return nullptr;
	unit *newUnit = unitHelpers::createUnitEDB(eopIdx, regionID, faction->factionID, exp, arm, weapon);
	if (newUnit == nullptr)
		return newUnit;*/
	armyHelpers::addUnitToArmy(this, newUnit);
	return newUnit;
}
void armyStruct::setAiActiveSet(int set)
{
	for (int i = 0; i < numOfUnits; i++)
		units[i]->aiActiveSet = set;
}
void armyStruct::releaseUnits()
{
	for (int i = 0; i < numOfUnits; i++)
		units[i]->releaseUnit();
}
void armyStruct::mergeArmiesLua(armyStruct* targetArmy, const bool force)
{
	if (!targetArmy)
		return;
	
	if (force)
	{
		armyHelpers::mergeArmies(this, targetArmy);
	}
	else
	{
		int targetX, targetY;
		if (targetArmy->gen)
		{
			targetX = targetArmy->gen->xCoord;
			targetY = targetArmy->gen->yCoord;
		}
		else if (targetArmy->settlement)
		{
			targetX = targetArmy->settlement->xCoord;
			targetY = targetArmy->settlement->yCoord;
		}
		else
			return;
		sol::table unitTable = sol::state_view(plugData::data.luaAll.luaState).create_table();
		for (int i = 0; i < numOfUnits; i++)
		{
			unitTable.add(units[i]);
		}
		factionHelpers::splitArmy(faction, unitTable, targetX, targetY);
		unitTable.clear();
	}
}

void armyStruct::mergeArmiesLua(armyStruct* targetArmy)
{
	mergeArmiesLua(targetArmy, true);
}

namespace armyHelpers
{
	/*----------------------------------------------------------------------------------------------------------------*\
                                                    Army actions
    \*----------------------------------------------------------------------------------------------------------------*/
#pragma region Army actions
    bool blockadePort(armyStruct* fleet, portBuildingStruct* port)
    {
        if (!fleet || !port )
            return false;
        int success = 0;
        DWORD funcAddr = codes::offsets.assaultObject;
        _asm
        {
            push 0
            push port
            mov ecx, fleet
            mov eax, funcAddr
            call eax
            mov success, eax
        }
        return success != 0;
    }
	
    void buildWatchTower(armyStruct* army)
    {
        if (!army->gen || !army->gen->characterRecord || !army->gen->characterRecord->faction)
            return;
        int cultureId = army->gen->characterRecord->faction->cultureID;
        auto cultures = cultures::getCultureDb();
        if (int cost = cultures->cultures[cultureId].watchTowerCost;
            army->gen->characterRecord->faction->money < cost)
            return;
        DWORD funcAddr = codes::offsets.buildWatchtower;
        _asm
        {
            mov ecx, army
            mov eax, funcAddr
            call eax
        }
    }

	void mergeArmies(armyStruct* army, armyStruct* targetArmy)
    {
    	if (army->numOfUnits + targetArmy->numOfUnits > 20)
    		return;
    	DWORD codeOffset = codes::offsets.mergeArmies;
    	_asm
		{
			push army
			mov ecx, targetArmy
			mov eax, codeOffset
			call eax
		}
    }
	
	bool stopSiege(armyStruct* army)
    {
    	if (army->siege == nullptr)
    		return false;
    	GAME_FUNC(void(__thiscall*)(armyStruct*, int), stopSiege)(army, 1);
    	return true;
    }

	bool stopBlockPort(armyStruct* army)
    {
    	if (army->siege == nullptr)
    		return false;
    	GAME_FUNC(void(__thiscall*)(armyStruct*), stopBlockade)(army);
    	return true;
    }

	void ungarrisonResidence(void* setOrFort)
    {
    	GAME_FUNC(void(__thiscall*)(void*), ungarrisonResidence)(setOrFort);
    }
	
#pragma endregion Army actions
	
	/*----------------------------------------------------------------------------------------------------------------*\
													Army Creation
    \*----------------------------------------------------------------------------------------------------------------*/
#pragma region Army Creation
    armyStruct* createArmy(character* character)
    {
        armyStruct* stack = nullptr;
        factionStruct* fac = character->characterRecord->faction;
        int isAdmiral = character->genType->type == 3 ? 1 : 0;
        DWORD adrFunc = codes::offsets.createArmyFunc;
        _asm
        {
            push isAdmiral
            push character
            mov ecx, fac
            mov eax, adrFunc
            call eax
            mov stack, eax
        }
        return stack;
    }

    armyStruct* createArmyInSettlement(settlementStruct* sett)
    {
        armyStruct* stack = nullptr;
        DWORD adrFunc = codes::offsets.createArmyInSettlementFunc;
        _asm
        {
            mov ecx, sett
            mov eax, adrFunc
            call eax
            mov stack, eax
        }
        return stack;
    }
	
	int addUnitToArmy(armyStruct* army, unit* un)
    {
    	if (army->numOfUnits == 20)
    		return 0;
    	DWORD adr = codes::offsets.addUnitToArmyFunc;
    	_asm {
    		push 0
			push un
			mov ecx, army
			mov eax, adr
			call eax
		}
    	return 1;
    }
	
	//used for spawnArmy function
	std::unordered_map<int, const char*> characterTypes = {
	    {0,"spy"},
		{1,"assassin"},
		{2,"diplomat"},
		{3,"admiral"},
		{4,"merchant"},
		{5,"priest"},
		{6,"general"},
		{7,"named character"},
		{8,"princess"},
		{9,"heretic"},
		{10,"witch"},
		{11,"inquisitor"},
		{13,"pope"}
	};
	
	armyStruct* spawnArmy(
		factionStruct* faction,
		const char* name,
		const char* name2,
		int characterType,
		const char* label,
		const char* portrait,
		int x,
		int y,
		int age,
		bool family,
		int subFaction,
		int unitIndex,
		int exp,
		int wpn,
		int armour
		)
	{
		if (!faction)
			return nullptr;
		if (portrait != nullptr && strlen(portrait) == 0)
			portrait = nullptr;
		if (label != nullptr && strlen(label) == 0)
			label = nullptr;
		queue<std::pair<int, int>> q;
		set<std::pair<int, int>> visited;
		q.emplace(x, y);
		visited.insert({x, y});
		stratMap* map = stratMapHelpers::getStratMap();
		auto spawnCoords = new coordPair;
		spawnCoords->xCoord = x;
		spawnCoords->yCoord = y;
		spawnCoords = stratMapHelpers::findValidTileNearTile(spawnCoords, characterType);
		character* gen = nullptr;
		const char* typeName = characterTypes.find(characterType)->second;
		campaign* campaign = campaignHelpers::getCampaignData();
		if (label && !std::string(label).empty())
		{
			for (int i = 0; i < campaign->factionCount; i++)
			{
				auto fac = campaign->factionsSortedByTurnOrder[i];
				for(int j = 0; j < fac->characterRecordNum; j++)
				{
					if (auto namedChar = fac->characterRecords[j]; namedChar->label != nullptr && std::string(namedChar->label) == std::string(label))
					{
						if ((namedChar->status & 8) != 0)
						{

							char** cryptS = gameStringHelpers::createHashedString(typeName);
							DWORD adrType = reinterpret_cast<DWORD>(cryptS);
							gen = GAME_FUNC(character*(__cdecl*)(DWORD, int, characterRecord*, const char*), respawnOffMapCharacterFunc)
							(adrType, faction->factionID, namedChar, portrait);
							if (gen)
							{
								DWORD adrFunc = codes::offsets.spawnCreatedObject;
							
								_asm
								{
									push spawnCoords
									push gen
									mov eax, adrFunc
									call eax
								}
							}
							break;
						}
						delete spawnCoords;
						return nullptr;
					}
				}
				if (gen)
					break;
			}
		}
		if (!gen)
		{
			int nameFaction = faction->agentNameFactionId[characterType];
			if (subFaction != 31)
				nameFaction = subFaction;
			if (const auto nameS = std::string(name); nameS == "random_name")
			{
				const int checkCountMax = faction->characterRecordNum * 2;
				int checkCount = 0;
				int firstNameIndex = 0;
				int secondNameIndex = 0;
				while (checkCount < checkCountMax)
				{
					GAME_FUNC(int(__cdecl*)(int*, int, int, int*, int*), getRandomNameFunc)
					(&campaign->lastRandomSeed, nameFaction, 0, &firstNameIndex, &secondNameIndex);
					bool research = false;
					for(int i = 0; i < faction->characterRecordNum; i++)
					{
						if (const int nameIndex = GAME_FUNC(int(__thiscall*)(characterRecord*), getNameIndexFunc)(faction->characterRecords[i]);
							firstNameIndex == nameIndex)
						{
							research = true;
							break;
						}
					}
					checkCount++;
					if (research)
						continue;
					break;
				}

				name = GAME_FUNC(const char*(__cdecl*)(int, int, int), getCharacterName)(0, nameFaction, firstNameIndex);
				name2 = GAME_FUNC(const char*(__cdecl*)(int, int, int), getCharacterName)(2, nameFaction, secondNameIndex);
			}
			gen = characterHelpers::createCharacterWithoutSpawning(typeName, faction, age, name, name2, subFaction, portrait, x, y);
			auto namedChar = gen->characterRecord;
			namedChar->isFamily = family;
			if (!faction->leader)
			{
				namedChar->isFamily = 1;
				DWORD codeOffset = codes::offsets.setFactionLeaderFunc;
				_asm
				{
					push namedChar
					mov ecx, faction
					mov eax, codeOffset
					call eax
				}
			}
			if (namedChar->isFamily)
			{
				DWORD facDWORD = reinterpret_cast<DWORD>(faction);
				facDWORD += 0xEC8;
				auto parent = GAME_FUNC(characterRecord*(__thiscall*)(DWORD, characterRecord*), findParentForAdoptionFunc)
				(facDWORD, namedChar);
				if (parent)
				{
					namedChar->parent = parent;
					int childNum = parent->numberOfChildren;
					parent->numberOfChildren = childNum + 1;
					namedChar->parent->childs[childNum] = namedChar;
				}
			}
		}
		if (gen)
		{
			
			DWORD adrFunc = codes::offsets.doSomeWithCharacterFunc;
			void* some = faction->tilesFac;
			_asm
			{
				push 0
				push gen
				mov ecx, some
				mov eax, adrFunc
				call eax
			}
				
			armyStruct* army = createArmy(gen);
			const oneTile* tile = stratMapHelpers::getTile(spawnCoords->xCoord, spawnCoords->yCoord);
			int regionId = tile->regionId;
			GAME_FUNC(void(__thiscall*)(stratMap*, armyStruct*, int), setArmyRegionEntriesFunc)(map, army, regionId);
			unit* bgUnit = nullptr;
			if (eopDu::getEduEntry(unitIndex))
			{
				bgUnit = unitHelpers::createUnitIdx(unitIndex, regionId, faction->factionID, exp, armour, wpn);
			}
			else
			{
				bgUnit = unitHelpers::createUnitIdx(0, regionId, faction->factionID, exp, armour, wpn);
			}
			addUnitToArmy(army, bgUnit);
			if (characterType == 7)
				characterHelpers::setBodyguard(gen, bgUnit);
			adrFunc = codes::offsets.factionResurrectStuffFunc;
			_asm
			{
				mov ecx, faction
				push spawnCoords
				mov eax, adrFunc
				call eax
			}
			delete spawnCoords;
			if (army && label && strcmp(label, "") == 0)
				gameStringHelpers::setHashedString(&gen->characterRecord->label, label);
			return army;
		}
		delete spawnCoords;
		return nullptr;
	}
    
	void addToSettlement(armyStruct* army, settlementStruct* set)
    {
    	if (army->settlement)
    		ungarrisonResidence(army->settlement);
    	if (set->army)
    	{
    		mergeArmies(army, set->army);
    		return;
    	}
    	if (!army->gen)
    	{
    		gameHelpers::logStringGame("addToSettlement: army has no character");
    		return;
    	}
    	GAME_FUNC(void(__thiscall*)(settlementStruct*, character*), addToSettlement)(set, army->gen);
    }

	void addToFort(armyStruct* army, fortStruct* fort)
	{
		if (army->settlement)
			ungarrisonResidence(army->settlement);
		if (fort->army)
		{
			mergeArmies(army, fort->army);
			return;
		}
		if (!army->gen)
		{
			gameHelpers::logStringGame("addToFort: army has no character");
			return;
		}
		GAME_FUNC(void(__thiscall*)(fortStruct*, character*), addToFort)(fort, army->gen);
	}
#pragma endregion Army Creation

	

	float getMinimumMovePointsForArmy(const armyStruct* army)
	{
		if (!army)
			return 0;
		if (army->numOfUnits < 1)
			return 0;
		float minMp = GAME_FUNC(float(__thiscall*)(unit*), getUnitFullMovePoints)(army->units[0]);
		for (int i = 1; i < army->numOfUnits; ++i)
		{
			if (const float unitFullMp = GAME_FUNC(float(__thiscall*)(unit*), getUnitFullMovePoints)(army->units[i]);
				unitFullMp < minMp)
			{
				minMp = unitFullMp;
			}
		}
		return minMp;
	}
	
    void addToLua(sol::state& luaState)
    {
        struct
        {
            sol::usertype<armyStruct>armyStruct;
            sol::usertype<siegeS>siege;
            sol::usertype<siegeEquipmentQueueItem>siegeEquipmentQueueItem;
        }types;
		
		///Army
		//@section Army

		/***
		Basic armyStruct table

		@tfield factionStruct faction
		@tfield getUnit getUnit
		@tfield int numOfUnits
		@tfield int deadUnitsNum
		@tfield bool isAdmiral
		@tfield bool canRetreat
		@tfield int supportingArmiesCampaign
		@tfield int ladders
		@tfield int rams
		@tfield int towers
		@tfield int battleLadders How many left in battle.
		@tfield int battleRams How many left in battle.
		@tfield int battleTowers How many left in battle.
		@tfield getCharacter getCharacter
		@tfield int numOfCharacters Includes Auxiliary generals and agents (i.e all characters excluding the leading general)
		@tfield armyStruct boardedArmy army embarked on this fleet stack
		@tfield armyStruct shipArmy fleet that this army stack is embarked on
		@tfield dockStruct blockedPort
		@tfield character leader Returns nil if stack is inside residence (fleet, settlement, fort).
		@tfield findInSettlement findInSettlement
		@tfield findInFort findInFort
		@tfield int totalStrength
		@tfield int subFactionID
		@tfield int regionID
		@tfield int totalStrengthStart
		@tfield int alliance
		@tfield int generalDeathTimer
		@tfield int generalRoutTimer
		@tfield bool isHalfDestroyed
		@tfield int generalBattleCommand
		@tfield number generalCommandRadius
		@tfield armyStruct commandingArmy if not leading army but reinforcement
		@tfield armyStruct commandingArmyThatGivesCommands reinforcement and listening to player commands
		@tfield bool inBattle
		@tfield bool isReinforcing Actively reinforcing (moving units into the battlefield).
		@tfield bool finishedBattle Just fought a battle.
		@tfield bool isReinforcement
		@tfield bool tired
		@tfield bool routedBattle
		@tfield bool moraleBroken
		@tfield int battleOutcome
		@tfield int reformPointX
		@tfield int reformPointY
		@tfield int maxGroups
		@tfield int hiddenUnitCount
		@tfield float reform_point_x X coordinate to which the retreating units will go.
		@tfield float reform_point_y Y coordinate to which the retreating units will go.
		@tfield createEOPUnit createEOPUnit
		@tfield createUnit createUnit
		@tfield sortStack sortStack
		@tfield mergeArmies mergeArmies
		@tfield createUnitByIDX createUnitByIDX
		@tfield siegeFort siegeFort
		@tfield blockadePort blockadePort
		@tfield attackArmy attackArmy
		@tfield getGroup getGroup
		@tfield siegeStruct siege Current siege.
		@tfield defineUnitGroup defineUnitGroup
		@tfield getDeadUnit getDeadUnit
		@tfield setAiActiveSet setAiActiveSet
		@tfield releaseUnits releaseUnits
		@tfield buildWatchTower buildWatchTower
		@tfield liftSiege liftSiege
		@tfield liftBlockade liftBlockade
		@tfield siegeSettlement siegeSettlement


		@table armyStruct
		*/
		types.armyStruct = luaState.new_usertype<armyStruct>("armyStruct");
		types.armyStruct.set("faction", &armyStruct::faction);
		types.armyStruct.set("isAdmiral", &armyStruct::isAdmiral);
		types.armyStruct.set("supportingArmiesCampaign", &armyStruct::supportingArmiesCampaign);
		types.armyStruct.set("canRetreat", &armyStruct::canRetreat);
		types.armyStruct.set("battleLadders", &armyStruct::battleLadders);
		types.armyStruct.set("battleRams", &armyStruct::battleRams);
		types.armyStruct.set("battleOutcome", &armyStruct::battleOutcome);
		types.armyStruct.set("battleTowers", &armyStruct::battleTowers);
		types.armyStruct.set("deadUnitsNum", &armyStruct::deadUnitsNum);
		types.armyStruct.set("ladders", &armyStruct::ladders);
		types.armyStruct.set("rams", &armyStruct::rams);
		types.armyStruct.set("hiddenUnitCount", &armyStruct::hiddenUnitCount);
		types.armyStruct.set("maxGroups", &armyStruct::maxUnitGroups);
		types.armyStruct.set("towers", &armyStruct::towers);
		types.armyStruct.set("siege", &armyStruct::siege);
		types.armyStruct.set("reformPointX", &armyStruct::reform_point_x);
		types.armyStruct.set("reformPointY", &armyStruct::reform_point_y);
		types.armyStruct.set("isReinforcing", &armyStruct::isReinforcing);
		types.armyStruct.set("inBattle", &armyStruct::inBattle);
		types.armyStruct.set("routedBattle", &armyStruct::routedBattle);
		types.armyStruct.set("moraleBroken", &armyStruct::moraleBroken);
		types.armyStruct.set("finishedBattle", &armyStruct::finishedBattle);
		types.armyStruct.set("isReinforcement", &armyStruct::isReinforcement);
		types.armyStruct.set("generalDeathTimer", &armyStruct::generalDeathTimer);
		types.armyStruct.set("generalRoutTimer", &armyStruct::generalRoutTimer);
		types.armyStruct.set("regionID", &armyStruct::regionID);
		types.armyStruct.set("subFactionID", &armyStruct::subFactionID);
		types.armyStruct.set("totalStrengthStart", &armyStruct::totalStrengthStart);
		types.armyStruct.set("totalStrengthStart", &armyStruct::totalStrengthStart);
		types.armyStruct.set("generalCommandRadius", &armyStruct::generalCommandRadius);
		types.armyStruct.set("generalBattleCommand", &armyStruct::generalBattleCommand);
		types.armyStruct.set("alliance", &armyStruct::battleSide);
		types.armyStruct.set("isHalfDestroyed", &armyStruct::isHalfDestroyed);
		types.armyStruct.set("tired", &armyStruct::tired);
		types.armyStruct.set("commandingArmy", &armyStruct::commandingArmy);
		types.armyStruct.set("commandingArmyThatGivesCommands", &armyStruct::commandingArmyThatGivesCommands);

		/***
		Sort units in a stack. Use the sortType enum to specify the sorting mode.
		@function armyStruct:sortStack
		@tparam int sortMode
		@tparam int sortMode2
		@tparam int sortMode3
		@usage

		function onFactionTurnStart(eventData)
		local faction = eventData.faction
	    -- If it's not the players turn, don't sort
	    if faction.isPlayerControlled == 0 then return end

		function onFactionTurnStart(eventData)
			CAMPAIGN = M2TW.campaign
			local faction = eventData.faction
			-- If it's not the players turn, don't sort
			if faction.isPlayerControlled == 0 then return end

			-- Sort all the stacks on the map right before the turn starts
			local factionsNum = CAMPAIGN.numberOfFactions;
			for i = 1, #factionsNum do
				local fac = CAMPAIGN.factionsSortedByDescrStrat[i]
				for j = 0, fac.stacksNum - 1 do
					local stack = fac:getStack(j)
					if stack then
						-- Sort the stack by category + class, then by soldier count, then by experience
						stack:sortStack(sortType.categoryClass, sortType.soldierCount, sortType.experience)
					end
				end
			end
		end
		
		*/
		types.armyStruct.set_function("sortStack", &armyStruct::sortStack);

		/***
		Get a unit by it's index.
		@function armyStruct:getUnit
		@tparam int number
		@treturn unit retUnit
		@usage
		local ourUnit = army:getUnit(0)
		ourUnit:kill()
		*/
		types.armyStruct.set_function("getUnit", &armyStruct::getUnit);
		types.armyStruct.set("numOfUnits", &armyStruct::numOfUnits);
		/***
		Get a character (agent or non-leading named character) by it's index.
		@function armyStruct:getCharacter
		@tparam int number
		@treturn character retCharacter
		@usage
		ourChar = army:getCharacter(0)
		*/
		types.armyStruct.set_function("getCharacter", &armyStruct::getCharacter);
		types.armyStruct.set("numOfCharacters", &armyStruct::charactersNum);
		types.armyStruct.set("boardedArmy", &armyStruct::boardedArmy);
		types.armyStruct.set("shipArmy", &armyStruct::shipArmy);
		types.armyStruct.set("blockedPort", &armyStruct::blockedPort);
		types.armyStruct.set("leader", &armyStruct::gen);
		/***
		Find the settlement in which the army is located. Returns nil if the army is not in a settlement.

		Returns nil if the army is not in the settlement.

		@function armyStruct:findInSettlement
		@treturn  settlementStruct  settlement
		@usage
		local ourSett = army:findInSettlement()
		if (ourSett ~= nil) then
			--something
		end
		*/
		types.armyStruct.set_function("findInSettlement", &armyStruct::findInSettlement);
		/***
		Find the fort in which the army is located. Returns nil if the army is not in a fort.

		Returns nil if the army is not in the fort.

		@function armyStruct:findInFort
		@treturn  fortStruct  fort
		@usage
		local ourFort = army:findInFort()
		if (ourFort ~= nil) then
			--something
		end
		*/
		types.armyStruct.set_function("findInFort", &armyStruct::findInFort);
		types.armyStruct.set("totalStrength", &armyStruct::totalStrength);
		types.armyStruct.set("reform_point_x", &armyStruct::reform_point_x);
		types.armyStruct.set("reform_point_y", &armyStruct::reform_point_y);
		types.armyStruct.set_function("createEOPUnit", &armyStruct::createEopUnit);
		
		/***
		Create a unit in the army by type from export\_descr\_unit.txt
		@function armyStruct:createUnit
		@tparam string type
		@tparam int exp Experience. Maximum: 9.
		@tparam int armor Armour level.
		@tparam int weapon Weapon upgrade. Maximum: 1.
		@treturn unit newUnit
		@usage
		local newUnit = army:createUnit("Axemen of Lossarnach",1,1,1);
		*/
		types.armyStruct.set_function("createUnit", &armyStruct::createUnit);

		/***
		Create a unit in the army by index from export\_descr\_unit.txt
		@function armyStruct:createUnitByIDX
		@tparam int index Index (order in export\_descr\_unit.txt)
		@tparam int exp Experience. Maximum: 9.
		@tparam int armor Armour level.
		@tparam int weapon Weapon upgrade. Maximum: 1.
		@treturn unit newUnit
		@usage
		local newUnit = army:createUnitByIDX(255,1,1,1)
		*/
		types.armyStruct.set_function("createUnitByIDX", &armyStruct::createUnitByIndex);


		/***
		Merge 2 armies on the strat map. Does nothing if the total size of the new army exceeds 20 units.
		@function armyStruct:mergeArmies
		@tparam armyStruct targetArmy
		@tparam bool force optional
		@usage
		army:mergeArmies(anotherArmy)
		*/
		types.armyStruct.set_function("mergeArmies", sol::overload(
				sol::resolve<void(armyStruct*)>(&armyStruct::mergeArmiesLua),
				sol::resolve<void(armyStruct*, bool)>(&armyStruct::mergeArmiesLua)
			));

		/***
		Besiege the specified settlement, or attack it if already besieging it. Requires movement points.
		@function armyStruct:siegeSettlement
		@tparam settlementStruct settlement
		@tparam bool isAttack if this is false it makes the army maintain a siege
		@usage
		army:siegeSettlement(settlement, true)
		*/
		types.armyStruct.set_function("siegeSettlement", &characterHelpers::siegeSettlement);

		/***
		Besiege the specified fort, or attack it if already besieging it. Requires movement points.
		@function armyStruct:siegeFort
		@tparam fortStruct fort
		@tparam bool isAttack if this is false it makes the army maintain a siege
		@usage
		army:siegeFort(fort)
		*/
		types.armyStruct.set_function("siegeFort", &characterHelpers::siegeFort);

		/***
		Blockade a port.
		@function armyStruct:blockadePort
		@tparam portStruct port
		@treturn bool success
		@usage
		local success = army:blockadePort(port)
		*/
		types.armyStruct.set_function("blockadePort", &blockadePort);

		/***
		Lift an active siege.
		@function armyStruct:liftSiege
		@usage
			army:liftSiege()
		*/
		types.armyStruct.set_function("liftSiege", &stopSiege);

		/***
		Lift an active blockade.
		@function armyStruct:liftBlockade
		@usage
			army:liftBlockade()
		*/
		types.armyStruct.set_function("liftBlockade", &stopBlockPort);
		
		/***
		Attack another army. Requires movement points.

		@function armyStruct:attackArmy
		@tparam armyStruct defender
		@treturn  int Success Failed = 0.
		@usage
		local success = army:attackArmy(defenderArmy)
		if (success ~= 0) then
			--something
		end
		*/
		types.armyStruct.set_function("attackArmy", &armyStruct::attackArmy);
		
		/***
		Get dead unit at index.
		@function armyStruct:getDeadUnit
		@tparam int index
		@treturn unit deadUnit
		@usage
		    local unit = army:getDeadUnit(0)
		*/
		types.armyStruct.set_function("getDeadUnit", &armyStruct::getDeadUnit);
		
		/***
		Get unit group at index.
		@function armyStruct:getGroup
		@tparam int index
		@treturn unitGroup group
		@usage
		    local group = army:getGroup(0)
		*/
		types.armyStruct.set_function("getGroup", &armyStruct::getGroup);
		
		/***
		Define a new unit group with a label.
		@function armyStruct:defineUnitGroup
		@tparam string name
		@tparam unit unit
		@treturn unitGroup group
		@usage
		    local group = army:defineUnitGroup("group1", unit)
		*/
		types.armyStruct.set_function("defineUnitGroup", &unitHelpers::defineUnitGroup);
		
		/***
		Set ai active set to a value for the whole army (0 = inacitve, 1 = active, 2 = script controlled)
		@function armyStruct:setAiActiveSet
		@tparam int activeSet
		@usage
		     army:setAiActiveSet(1)
		*/
		types.armyStruct.set_function("setAiActiveSet", &armyStruct::setAiActiveSet);
		
		/***
		Set ai active set to on or off depending if army is player controlled
		@function armyStruct:releaseUnits
		@usage
		     army:releaseUnits()
		*/
		types.armyStruct.set_function("releaseUnits", &armyStruct::releaseUnits);
		
		/***
		Build a watchtower (payment applies)
		@function armyStruct:buildWatchTower
		@usage
		     army:buildWatchTower()
		*/
		types.armyStruct.set_function("buildWatchTower", &buildWatchTower);
		
		///Siege
		//@section Siege

		/***
		Basic siegeStruct table

		@tfield armyStruct besieger
		@tfield settlementStruct besiegedSettlement
		@tfield fortStruct besiegedFort
		@tfield int equipmentQueueCount
		@tfield int siegeTurns
		@tfield int siegeCasualties
		@tfield int siegeAssaults Amount of times the besieger has assaulted the settlement.
		@tfield int soldierCountStart
		@tfield bool finished
		@tfield getQueueItem getQueueItem

		@table siegeStruct
		*/
		types.siege = luaState.new_usertype<siegeS>("siegeStruct");
		types.siege.set("besieger", &siegeS::army);
		types.siege.set("siegeTurns", &siegeS::siegeTurns);
		types.siege.set("siegeCasualties", &siegeS::siegeCasualties);
		types.siege.set("siegeAssaults", &siegeS::siegeAssaults);
		types.siege.set("finished", &siegeS::finished);
		types.siege.set("equipmentQueueCount", &siegeS::queueCount);
		types.siege.set("soldierCountStart", &siegeS::soldierCountStart);
		types.siege.set("besiegedSettlement", sol::property(
			&siegeS::getSiegedSettlement));
		types.siege.set("besiegedFort", sol::property(
		&siegeS::getSiegedFort));
		
		/***
		Get an item in the siege equipment queue by index.
		@function siegeStruct:getQueueItem
		@tparam int index
		@treturn siegeEquipmentQueueItem item
		@usage
		     local item = siege:getQueueItem(0)
		*/
		types.siege.set_function("getQueueItem", &siegeS::getQueueItem);
		

		/***
		Basic siegeEquipmentQueueItem table

		@tfield int type
		@tfield bool frozen
		@tfield bool valid
		@tfield int cost
		@tfield int remainingCost
		@tfield int percentComplete

		@table siegeEquipmentQueueItem
		*/
		types.siegeEquipmentQueueItem = luaState.new_usertype<siegeEquipmentQueueItem>("siegeEquipmentQueueItem");
		types.siegeEquipmentQueueItem.set("type", &siegeEquipmentQueueItem::type);
		types.siegeEquipmentQueueItem.set("frozen", &siegeEquipmentQueueItem::frozen);
		types.siegeEquipmentQueueItem.set("valid", &siegeEquipmentQueueItem::valid);
		types.siegeEquipmentQueueItem.set("cost", &siegeEquipmentQueueItem::cost);
		types.siegeEquipmentQueueItem.set("remainingCost", &siegeEquipmentQueueItem::remainingCost);
		types.siegeEquipmentQueueItem.set("percentComplete", &siegeEquipmentQueueItem::percentComplete);
		
    }
};