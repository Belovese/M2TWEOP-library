﻿#pragma once
#include "campaign.h"
#include "realGameTypes.h"
#include "unit.h"
#include "lua/sol.hpp"

struct attachedAncillary
{
	int count;
	int level;
};

struct siegeEquipmentQueueItem
{
	int type;
	bool frozen;
	bool valid;
	int8_t byte6[2];
	int cost;
	int remainingCost;
	int percentComplete;
};

struct siegeS {
	trackedObject object;
	struct settlementStruct* goal; /* settlement or fort */
	struct armyStruct* army;
	struct siegeEquipmentQueueItem equipmentQueue[25];
	int queueCount;
	int sapProgress[8];
	int siegeTurns;
	int siegeCasualties;
	int siegeAssaults;
	int soldierCountStart;
	bool finished;
	char pad_x[3];
public:
	settlementStruct* getSiegedSettlement();
	fortStruct* getSiegedFort();
	siegeEquipmentQueueItem* getQueueItem(const int index)
	{
		if (index < 0 || index >= queueCount)
			return nullptr;
		return &equipmentQueue[index];
	}
};

//army data
struct armyStruct { /* structure of stack */
	trackedObject object;
	spyingInfo spyInfo;
	DWORD armyVerification1;
	DWORD armyVerification2;
	DWORD armyVerification3;
	bool damageEnemyObj;
	bool damageTileObj;
	bool destroySupplyObj;
	bool destroyCampObj;
	void* battleSoldiers;
	struct factionStruct* faction;
	int regionID;
	struct unit** units;
	int32_t unitsSize; //0x0058
	int numOfUnits;
	struct unit **deadUnits;
	int deadUnitsSize;
	int deadUnitsNum;
	attachedAncillary ancillaries[4];
	int hiddenUnitCount;
	bool isAdmiral;
	char pad_0091[3]; //0x0091
	void* trackedVtblBoarded;
	struct armyStruct* boardedArmy; /* if fleet, here army */
	void* trackedVtblShip;
	struct armyStruct* shipArmy; /* here fleet what contain our army */
	bool canRetreat;
	char byte_a5[3];
	int supportingArmiesCampaign;
	struct siegeS* siege;
	struct portDockStrat* blockedPort; /* if port blocked by this army when it here */
	int8_t battleLadders;
	int8_t battleRams;
	int8_t battleTowers;
	int8_t ladders;
	int8_t rams;
	int8_t towers;
	char pad_ba[2];
	struct unitGroup *unitGroups;
	int maxUnitGroups;
	int stratFatigue;
	int loyalty;
	int upkeepModifier;
	void* trackedGenVtbl;
	struct character* gen; /* 0 if in settlement/fort */
	struct unit* generalsUnit; //0x00D8
	struct generalInfo* generalInfo; //0x00DC
    float generalCommandRadius; //0x00E0
	int32_t generalBattleCommand;
	int32_t generalDeathTimer;
	int32_t generalRoutTimer;
	struct character** characters; //0x00F0
	int charactersSize; //0x00F8
	int charactersNum; //0x00F8
	void* trackedResidenceVtbl;
	struct settlementStruct* settlement; //0x0100
	int currentReinforcingUnit;
	armyStruct *commandingArmy;
	bool isReinforcing;
	bool inBattle;
	bool finishedBattle;
	bool isReinforcement;
	void *trackedPointerArmy_vtbl3;
	struct armyStruct* enemyArmy;
	int32_t battleOutcome; //0x0118
	bool routedBattle; //0x011C
	bool moraleBroken; //0x011D
	bool pad_11e[2]; //0x011e
    int totalStrengthStart;
	int totalStrength;
	float reform_point_x;
	float reform_point_y;
	int32_t battleSide; //0x0130
	int retreatType;
	bool destroyOnDefeat;
	char pad139[3];
	void *battleStandard;
	bool hasPrisonersRansomed;
	char pad_141[3];
	int32_t armyIDBattleSide; //0x0144
	int32_t battleDifficulty; //0x0148
	bool isHalfDestroyed; //0x014C
	bool tired;
	char pad_14e[2];
	int winningCombat;
	int winningCombatLast;
	armyStruct *commandingArmyThatGivesCommands;
	struct descrRebelEntry* descrRebel; //0x015C
	int32_t subFactionID; //0x0160
public:
	std::pair<int, int> getCoords();
	void nullifyMovePoints();
	unit* getUnit(const int index)
	{
		if (index < 0 || index >= numOfUnits)
			return nullptr;
		return units[index];
	}
	character* getCharacter(const int index)
	{
		if (index < 0 || index >= charactersNum)
			return nullptr;
		return characters[index];
	}
	unitGroup* getGroup(const int index)
	{
		if (index < 0 || index >= maxUnitGroups)
			return nullptr;
		return &unitGroups[index];
	}
	unit* getDeadUnit(const int index)
	{
		if (index < 0 || index >= deadUnitsNum)
			return nullptr;
		return deadUnits[index];
	}
	int attackArmy(armyStruct* defender);
	void sortStack(int sortType, int sortType2, int sortType3);
	settlementStruct* findInSettlement();
	fortStruct* findInFort();
	void setAiActiveSet(int set);
	void releaseUnits();
	unit* createUnit(const char* type, int exp, int arm, int weapon);
	unit* createUnitByIndex(int eduIndex, int exp, int arm, int weapon);
	unit* createEopUnit(int eduIndex, int exp, int arm, int weapon);
	void mergeArmiesLua(armyStruct* targetArmy, bool force = true);
	void mergeArmiesLua(armyStruct* targetArmy);
};

namespace armyHelpers
{
	//Army Actions
	bool blockadePort(armyStruct* fleet, portBuildingStruct* port);
	void buildWatchTower(armyStruct* army);
	bool stopSiege(armyStruct* army);
	bool stopBlockPort(armyStruct* army);
	void mergeArmies(armyStruct* army, armyStruct* targetArmy);

	//Army Creation
	armyStruct* createArmy(character* character);
	armyStruct* createArmyInSettlement(settlementStruct* sett);
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
		);
	
	float getMinimumMovePointsForArmy(const armyStruct* army);
	int addUnitToArmy(armyStruct* army, unit* un);
	void addToSettlement(armyStruct* army, settlementStruct* set);
	void addToFort(armyStruct*army, fortStruct* fort);
	void addToLua(sol::state& luaState);
	void nullifyMovePoints(armyStruct* army);
	void ungarrisonResidence(void* setOrFort);
};