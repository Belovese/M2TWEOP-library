#pragma once

#include "headersSTD.h"
#include "realGameTypes.h"
#include "exportHeader.h"
#include "functionsOffsets.h"
#include "dataOffsets.h"


struct stratModelRecord
{
	UINT32 modelId = 0;
	bool isSettlement = false;
	string path;
	model_Rigid* modelP = nullptr;
};

struct settlementStruct;

namespace stratModelsChange
{
	model_Rigid* getModel(UINT32 modelId);
	void checkAndChangeStratModels();
	void update();
	void loadModels();
	model_Rigid* loadModel(const std::string& path, bool isSettlement = false);

	//add model to a game
	//pass path to model and its id(used for change models)
	//example of path - eopData/models_strat/northern_european_large_castle.CAS
	void addModelToGame(const std::string& path, UINT32 modelId, bool isSettlement = false);
	void addModelToGameNoBool(const std::string& path, UINT32 modelId);
	void setModel(int x, int y, UINT32 modelId, UINT32 modelId2);
	void setModelOneVar(int x, int y, UINT32 modelId);
	void changeStratModel(character* gen, const char* model);
	DWORD getCasAnimSet(const char* name);
	DWORD createModelFlexi(bool shadow);
	void fixModelFlexi(bool shadow, DWORD stratmodel, DWORD modelflexi, const char* texturepath, DWORD skeleton, float scale);
	DWORD loadStratCAS(const char* caspath, bool shadow);
	void addCharacterCas(const char* skeletonname, const char* caspath, const char* shadowcaspath, const char* typeName, const char* texturepath, float scale);
	stratModelArrayEntry* buildCharacterCas(const char* skeletonname, const char* caspath, const char* shadowcaspath, const char* typeName, const char* texturepath, float scale);
	stratModelArrayEntry* getStratModelEntry(const char* name);
	stratModelArrayEntry* findCharacterStratModel(const char* modelId);
	stratModelRecord* findStratModel(UINT32 modelId);
	void setCharacterModel(character* gen, const char* model);
	// ReSharper disable once CppInconsistentNaming
	int readTGAfile(const char* path);
	void loadCharModels();
	//for internal use only
	bool changeModel(int x, int y, model_Rigid* modelP, model_Rigid* modelP2);
	void WINAPI disableChecker();
	void changeSettlementStratModel(settlementStruct* settlement, model_Rigid* modelP);
};