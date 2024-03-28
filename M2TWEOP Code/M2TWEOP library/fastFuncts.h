#pragma once
#include "functionsOffsets.h"
#include "dataOffsets.h"
#include"headersSTD.h"
#include "headersMEM.h"
#include "realGameTypes.h"
#include "techFuncs.h"
#include "exportHeader.h"
#include "globals.h"

namespace fastFuncts
{
	factionStruct* GetCurrentFaction();
	std::string GetModPath();

	NOINLINE EOP_EXPORT float GetMovepointsForReachNearTile(int x, int y, int destX, int destY);

	NOINLINE EOP_EXPORT void revealTile(factionStruct* faction, int x, int y);
	NOINLINE EOP_EXPORT void hideRevealedTile(factionStruct* faction, int x, int y);

	NOINLINE EOP_EXPORT int8_t getTileVisibility(factionStruct* faction, int x, int y);
	NOINLINE EOP_EXPORT void setTileVisibility(factionStruct* faction, int x, int y, int8_t vis);


	NOINLINE EOP_EXPORT void setSettlementOwner(settlementStruct*sett, factionStruct* newOwner);

	NOINLINE EOP_EXPORT void GetGameTileCoordsWithCursor(int& x, int& y);
	NOINLINE EOP_EXPORT void ViewTacticalMap(int x, int y);
	NOINLINE EOP_EXPORT bool IsStratMap();

	NOINLINE EOP_EXPORT void setCharacterType(general*character, int typeID,int subFaction,int factionDipNum);
	NOINLINE EOP_EXPORT UINT32 getTileRegionID(int x,int y);
	NOINLINE EOP_EXPORT oneTile* getTileStruct(int x,int y);
	NOINLINE EOP_EXPORT regionStruct* getRegionByID(UINT32 regionID);
	NOINLINE EOP_EXPORT factionStruct* getRegionOwner(int regionID);
	NOINLINE EOP_EXPORT void toggleDeveloperMode();

	//season(0-summer,1-winter)
	NOINLINE EOP_EXPORT UINT32 getSeason();

	NOINLINE EOP_EXPORT UINT32 getYear();
	//set character as heir
	NOINLINE EOP_EXPORT void setHeir(namedCharacter* gen, bool isJustSet);

	//count of factions in game
	NOINLINE EOP_EXPORT UINT32 getFactionsCount();

	//array of pointers to factions
	NOINLINE EOP_EXPORT factionStruct** getFactionsList();

	//number of passed turns
	NOINLINE EOP_EXPORT UINT32 getPassedTurnsNum();

	//get guild by index
	NOINLINE EOP_EXPORT guild* getGuild(unsigned char index);

	//-1, -1 if not recognized
	std::pair<int, int> getArmyCoords(stackStruct* army);
	//find fort on coords
	NOINLINE EOP_EXPORT stackStruct* findArmy(int x, int y);
	//find fort on coords
	NOINLINE EOP_EXPORT fortStruct* findFort(int x, int y);
	//find port on coords
	NOINLINE EOP_EXPORT portBuildingStruct* findPort(int x, int y);
	//find resource on coords
	NOINLINE EOP_EXPORT resStrat* findResource(int x, int y);
	//find settlement on coords
	NOINLINE EOP_EXPORT settlementStruct* findSettlement(int x, int y);
	//find watchtower on coords
	NOINLINE EOP_EXPORT watchTowerStruct* findWatchTower(int x, int y);
	NOINLINE EOP_EXPORT void deleteFort(const factionStruct* fac, fortStruct* fort);
	NOINLINE EOP_EXPORT void createFortXY(const factionStruct* fac, int x, int y);
	NOINLINE EOP_EXPORT void createFort(const general* gen);

	//move stratmap camera slow
	NOINLINE EOP_EXPORT void moveStratCameraSlow(int x, int y);
	//move stratmap camera fast
	NOINLINE EOP_EXPORT void moveStratCameraFast(int x, int y);
	//zoom stratmap camera
	NOINLINE EOP_EXPORT void zoomStratCamera(float zoom);

	factionStratMapDescrS* GetFactSmDescrById(int id);
	//teleport character
	NOINLINE EOP_EXPORT void  teleportCharacter(general* gen, int x, int y);

	//add trait to character
	NOINLINE EOP_EXPORT void addTrait(namedCharacter* character, const char* traitName, int traitLevel);
	NOINLINE EOP_EXPORT void removeTrait(namedCharacter* character, const char* traitName);


	//add ancillary to character
	NOINLINE EOP_EXPORT int addAncillary(namedCharacter* character, ancillary* anc);
	//remove ancillary from character
	NOINLINE EOP_EXPORT void removeAncillary(namedCharacter* character, ancillary* anc);
	//find ancillary in anc list
	NOINLINE EOP_EXPORT ancillary* findAncillary(char* ancName);

	//edit unit characteristics
	NOINLINE EOP_EXPORT void setUnitParams(unit* un, int count, int exp, int armor, int weap);
	NOINLINE EOP_EXPORT void setSoldiersCount(unit* un,int count);
	NOINLINE EOP_EXPORT void setSoldiersCountAndExp(unit* un,int count, int exp);
	NOINLINE EOP_EXPORT void setUnitMovepoints(unit* un,float movepoints);
	void NuullifyMovepoints(stackStruct* army);

	//kills
	NOINLINE EOP_EXPORT void killUnit(unit* un);
	NOINLINE EOP_EXPORT void killCharacter(general* gen);

	NOINLINE EOP_EXPORT void destroyBuilding(settlementStruct* sett, const char* typeName, bool isReturnMoney);
	NOINLINE EOP_EXPORT void createBuilding(settlementStruct* sett, const char* building_level_id);

	NOINLINE EOP_EXPORT general* createCharacter(const char* type, factionStruct* fac, int age, const char* name, const char* name2, int subFaction, const char* portrait, int x, int y);

	//very very technical func 
	NOINLINE EOP_EXPORT general* createCharacterWithoutSpawning(const char* type, factionStruct* fac, int age, const char* name, const char* name2, int subFaction, const char* portrait, int x, int y);

	NOINLINE EOP_EXPORT stackStruct* createArmy(general* character);
	NOINLINE EOP_EXPORT stackStruct* createArmyInSettlement(settlementStruct* sett);


	//facNum - dipnum
	NOINLINE EOP_EXPORT unit* createUnitN(const char* type, int regionID, int facNum, int exp, int arm, int weap);
	NOINLINE EOP_EXPORT unit* createUnitIdx(int index, int regionID, int facNum, int exp, int arm, int weap);
	NOINLINE EOP_EXPORT unit* createUnitEDB(int edb, int regionID, int facNum, int exp, int arm, int weap);

	//returns true if we have siege before
	NOINLINE EOP_EXPORT bool StopSiege(stackStruct* army);
	NOINLINE EOP_EXPORT bool StopBlockPort(stackStruct* army);
	NOINLINE EOP_EXPORT int addUnitToArmy(stackStruct* army, unit* un);
	NOINLINE EOP_EXPORT void setBodyguard(general* gen, unit* un);


	NOINLINE EOP_EXPORT void AddToSettlement(stackStruct*army,settlementStruct* set);
	NOINLINE EOP_EXPORT void AddToFort(stackStruct*army,fortStruct* fort);


	NOINLINE EOP_EXPORT void UngarisonSetOrFort(void* setOrFort);


	NOINLINE EOP_EXPORT ModelDbEntry* findBattleModel(const char* modelName);

	//allocate mem for game class/etc
	NOINLINE EOP_EXPORT DWORD allocateGameMem(size_t amount);

	//attacker or defender
	NOINLINE EOP_EXPORT bool autoWin(const char* winnerSide);


	NOINLINE EOP_EXPORT void autoResolve();

	NOINLINE EOP_EXPORT bool callGameConsoleCommand(const char* name, const char* arg, char* errorBuffer);


	NOINLINE EOP_EXPORT bool useButton(const char* buttonName);
	NOINLINE EOP_EXPORT uiElement* getUiElement(const char* elementName);
	NOINLINE EOP_EXPORT void useUiElement(uiElement* element);


	NOINLINE EOP_EXPORT void mergeArmies(stackStruct* army, stackStruct* targetArmy);
	DWORD* getTileObject(const oneTile* tile, int type);

};

