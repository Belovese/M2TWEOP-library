#include "pch.h"
#include "stratModelsChange.h"
#include "campaign.h"
#include "functionsOffsets.h"
#include "dataOffsets.h"
#include "character.h"
#include "gameHelpers.h"
#include "strategyMap.h"

namespace stratModelsChange
{
	enum class modelsChangeStatus
	{
		changed = 0,
		needFixHiding = 1,
		needChange = 2
	};
	modelsChangeStatus changeModelsNeededNow = modelsChangeStatus::changed;

	struct stratModelCharacterRecord
	{
		const char* modelId;
		const char* skeletonname;
		const char* caspath;
		const char* shadowcaspath;
		const char* texturepath;
		float scale;
		stratModelArrayEntry* entry;
	};

	map<UINT32, stratModelRecord*>stratModels;
	vector<stratModelCharacterRecord*>characterStratModels;

	void addModelToGame(const std::string& path, const UINT32 modelId, const bool isSettlement)
	{
		auto* modRec = new stratModelRecord();
		modRec->path = path;
		modRec->modelId = modelId;
		modRec->isSettlement = isSettlement;
		stratModels[modelId] = modRec;
	}

	void addModelToGameNoBool(const std::string& path, const UINT32 modelId)
	{
		addModelToGame(path, modelId);
	}

	struct stratModelChangeRecord
	{
		UINT32 modelId = 0;
		UINT32 modelId2 = 0;
		int x = 0;
		int y = 0;
		bool isFort = false;
	};

	struct stratModelCharacterRecordChange
	{
		character* gen;

		const char* modelId;
	};

	vector<stratModelChangeRecord*>stratModelChangeList;
	int fortEntryCount;
	vector<stratModelCharacterRecordChange*>stratModelCharacterChangeList;


	void setModel(const int x, const int y, const UINT32 modelId, const UINT32 modelId2)
	{
		if (stratModels.find(modelId) == stratModels.end())
			return;
		for (size_t i = 0; i < stratModelChangeList.size(); i++)
		{
			if (stratModelChangeList[i]->x == x
				&& stratModelChangeList[i]->y == y
				)
			{
				delete stratModelChangeList[i];
				stratModelChangeList.erase(stratModelChangeList.begin() + i);
				i--;
			}
		}
		auto* rec = new stratModelChangeRecord();
		rec->modelId = modelId;
		rec->modelId2 = modelId2;
		rec->x = x;
		rec->y = y;
		rec->isFort = false;
		if (const auto tile = stratMapHelpers::getTile(x, y); tile->getFort())
		{
			fortEntryCount++;
			rec->isFort = true;	
		}
		else if (const auto sett = tile->getSettlement(); sett)
		{
			eopSettlementDataDb::get()->getSettlementData(tile->regionId, sett->minorSettlementIndex)->modelId = modelId;
		}
		stratModelChangeList.push_back(rec);
		changeModelsNeededNow = modelsChangeStatus::needChange;
	}

	void setModelOneVar(int x, int y, UINT32 modelId)
	{
		setModel(x, y, modelId, modelId);
	}

	stratModelRecord* findStratModel(const UINT32 modelId)
	{
		try
		{
			return stratModels.at(modelId);
		}
		catch (...)
		{
			return nullptr;
		}
	}


	model_Rigid* getModel(const UINT32 modelId)
	{
		try
		{
			return stratModels.at(modelId)->modelP;
		}
		catch (...)
		{
			return nullptr;
		}
	}
	struct visibilityCrashFixS
	{
		visibilityCrashFixS(int x, int y, factionStruct* fac, int8_t vis)
			:xCoord(x), yCoord(y), fac(fac), vis(vis)
		{

		}
		int xCoord;
		int yCoord;
		factionStruct* fac = nullptr;
		int8_t vis = 0;
	};
	vector<visibilityCrashFixS> crashFixAr;

	void checkAndChangeStratModels()
	{
		if (changeModelsNeededNow == modelsChangeStatus::changed)
			return;
		
		changeModelsNeededNow = modelsChangeStatus::changed;
		for (const stratModelChangeRecord* changeMod : stratModelChangeList) //static models
		{
			const stratModelRecord* mod1 = findStratModel(changeMod->modelId);
			if (mod1 == nullptr)
				continue;
			const stratModelRecord* mod2 = findStratModel(changeMod->modelId2);
			changeModel(changeMod->x, changeMod->y, mod1->modelP, mod2->modelP);
		}
		
		for (const stratModelCharacterRecordChange* changeMod : stratModelCharacterChangeList) //character models
		{
			if (changeMod->gen)
				changeStratModel(changeMod->gen, changeMod->modelId);
		}

		for (UINT32 i = 0; i < stratModelCharacterChangeList.size(); i++) //remove character models from change list because it only has to happen once!
		{
			delete stratModelCharacterChangeList[i];
			stratModelCharacterChangeList.erase(stratModelCharacterChangeList.begin() + i);
			i--;
		}

	}

	void update()
	{
	}

	bool MODELS_LOADED = false;
	bool CHAR_MODELS_LOADED = false;
	void __stdcall disableChecker()
	{
		MODELS_LOADED = false;
		CHAR_MODELS_LOADED = false;
	}
	void loadModels()
	{
		//if (MODELS_LOADED == true)
		//	return;

		// ReSharper disable once CppUseElementsView
		for (const auto& [index, record] : stratModels)
		{
			record->modelP = loadModel(record->path, record->isSettlement);
		}
		MODELS_LOADED = true;
		changeModelsNeededNow = modelsChangeStatus::needChange;
	}

	void loadCharModels() //rebuild character CAS entries to be sure no pointers were cleaned up
	{
		for (const stratModelCharacterRecord* modRec : characterStratModels)
		{
			*modRec->entry = *buildCharacterCas(modRec->skeletonname, modRec->caspath, modRec->shadowcaspath, modRec->modelId, modRec->texturepath, modRec->scale);
		}
	}

	model_Rigid* loadModel(const std::string& path, const bool isSettlement)
	{
		struct simpleCas
		{
			void* cas = nullptr;
		}cas;
		const char* casPath = GAME_FUNC(const char*(__cdecl*)(const char*), getRelativePath)(path.c_str());
		gameHelpers::logStringGame("Loading model: " + string(casPath));
		GAME_FUNC(bool(__thiscall*)(simpleCas*, const char*), parseCas)(&cas, casPath);
		auto modelRigid = techFuncs::createGameClass<model_Rigid>();
		modelRigid = GAME_FUNC(model_Rigid*(__thiscall*)(model_Rigid*), createModelRigid)(modelRigid);
		char textureName[MAX_PATH];
		strncpy(textureName, casPath, sizeof(textureName));
		textureName[sizeof(textureName) -1] = '\0';
		GAME_FUNC(void(__cdecl*)(char*), appendTexturesFolder)(textureName);
		const DWORD casModel = GAME_FUNC(DWORD(__thiscall*)(simpleCas*), getCasModel)(&cas);
		const uint32_t flags = isSettlement ? 2428 : 2332;
		GAME_FUNC(char(__thiscall*)(model_Rigid*, DWORD, char*, uint32_t, int ,int, int, bool)
			, loadModelRigid)(modelRigid, casModel, textureName, flags, 2, 1, 1, true);
		GAME_FUNC(void(__thiscall*)(simpleCas*), closeCas)(&cas);
		GAME_FUNC(void(__thiscall*)(simpleCas*), simpleCasDestructor)(&cas);
		return modelRigid;
	}

	void setCharacterModel(character* gen, const char* model) //add character to be changed to the queue
	{
		if (!gen || !model)
			return;
		const size_t stringSize = strlen(model);
		for (UINT32 i = 0; i < stratModelCharacterChangeList.size(); i++)
		{
			if (stratModelCharacterChangeList[i]->gen == gen)
			{
				delete stratModelCharacterChangeList[i];
				stratModelCharacterChangeList.erase(stratModelCharacterChangeList.begin() + i);
				i--;
			}
		}
		auto* rec = new stratModelCharacterRecordChange();
		rec->gen = gen;
		const auto modelNameCopy = new char[stringSize];
		strcpy(modelNameCopy, model);
		rec->modelId = modelNameCopy;
		stratModelCharacterChangeList.push_back(rec);
		const auto charData = eopCharacterDataDb::get()->getOrCreateData(gen->characterRecord->giveValidLabel(), gen->getTypeID());
		charData->model = model;

		changeModelsNeededNow = modelsChangeStatus::needChange;
	}

	void changeStratModel(character* gen, const char* model)
	{
		if (gen == nullptr) { //maybe captain dont exist anymore
			return;
		}
		stratModelArrayEntry* entry = findCharacterStratModel(model); //get eop strat model from vector
		if (entry == nullptr) {
			entry = getStratModelEntry(model); //get vanilla strat model from 255 array
		}
		if (entry == nullptr) {
			return;
		}
		const size_t stringSize = strlen(model);

		const auto characterFacEntry = new genMod; //make new descr character faction entry
		*characterFacEntry = *gen->genType; //get data of old entry and copy it in

		auto* modelArray = new descrCharacterStratModelArray; //make new model array
		*modelArray = *gen->genType->stratInfo; //get data of old model array
		characterFacEntry->stratInfo = modelArray; //assign new model array to new descr character faction entry

		for (int i = 0; i < characterFacEntry->modelCount; i++) //change all models to new one in this array
		{
			if (&characterFacEntry->stratInfo->stratModelsArray[i] != nullptr)
			{
				const auto modelNameCopy = new char[stringSize];
				strcpy(modelNameCopy, model);
				characterFacEntry->stratInfo->stratModelsArray[i].modelName = modelNameCopy;
				characterFacEntry->stratInfo->stratModelsArray[i].stratModelEntry = entry;
			}
		}

		gen->genType = characterFacEntry; //assign new array to general

		changeModelsNeededNow = modelsChangeStatus::needChange;
	}


	stratModelArrayEntry* findCharacterStratModel(const char* modelId) //find eop model from vector
	{
		for (const stratModelCharacterRecord* newRec : characterStratModels)
		{
			if (strcmp(modelId, newRec->modelId) == 0) {
				return newRec->entry;
			}
		}

		return nullptr;
	}

	stratModelArrayEntry* getStratModelEntry(const char* name) //find vanilla model from array at offset
	{
		auto* array = reinterpret_cast<stratModelArrayEntry*>(dataOffsets::offsets.stratModelArray);
		for (int i = 0; i < 255; i++)
		{
			if (&array[i] && array[i].typeName && array[i].model_flexi_m)
			{
				if (strcmp(name, array[i].typeName) == 0)
				{
					return &array[i];
				}
			}
		}
		return nullptr;
	}

	DWORD getCasAnimSet(const char* name) //function game uses to find an animation set by name
	{
		DWORD getSkelFunc = codes::offsets.getSkeleton;
		const char** stringpointer = &name;
		DWORD skeleton = 0;
		DWORD skeletonDatabasePointer = dataOffsets::offsets.skeletonDatabase;

		_asm
		{
			push stringpointer
			mov ecx, skeletonDatabasePointer
			mov eax, getSkelFunc
			call eax
			mov ecx, [eax + 0x14]
			mov skeleton, ecx
		}

		return skeleton;
	}


	DWORD createModelFlexi(bool shadow) //create new empty model flexi using game's allocation function
	{
		DWORD assignfunc = codes::offsets.allocMemFunc;
		DWORD newModelFlexi = 0;
		DWORD createModelFlexi = shadow == true ? codes::offsets.createModelFlexiShadow : codes::offsets.createModelFlexi;
		DWORD memsize = shadow == true ? 0x11c : 0x12c;

		_asm
		{
			push memsize
			mov eax, assignfunc
			call eax
			add esp, 0x4
			mov ecx, eax
			mov eax, createModelFlexi
			call eax
			mov newModelFlexi, eax
		}

		return newModelFlexi;
	}

	void fixModelFlexi(bool shadow, DWORD stratmodel, DWORD modelflexi, const char* texturepath, DWORD skeleton, float scale) //what game does to apply loaded char cas to model flexi
	{
		DWORD funcAddr = codes::offsets.assignCasToFlexi;
		if (shadow == true)
		{
			funcAddr = codes::offsets.assignShadowCasToFlexi;
			texturepath = nullptr;
		}
		float pushScale = scale;
		_asm
		{
			push 1
			push 0
			push 1
			push pushScale
			push texturepath
			push skeleton
			push stratmodel
			mov ecx, modelflexi
			mov eax, funcAddr
			call eax
		}
	}

	DWORD loadStratCAS(const char* caspath, bool shadow) //load the cas
	{
		size_t count = strlen(caspath);
		int zero = 0;
		int* zeropointer = &zero;
		DWORD loadFunc = codes::offsets.loadStratCharModel;
		DWORD stratModel = 0;
		if (shadow) {  // NOLINT(bugprone-branch-clone)
			_asm
			{
				push count
				push caspath
				mov ecx, zeropointer
				mov eax, loadFunc
				call eax
			}
		}
		else {
			_asm
			{
				push count
				push caspath
				mov ecx, zeropointer
				mov eax, loadFunc
				call eax
			}
		}

		return *zeropointer;
	}


	void addCharacterCas(const char* skeletonname, const char* caspath, const char* shadowcaspath, const char* typeName, const char* texturepath, float scale)
	{
		//add entry to vector, using all info that is needed to rebuild cas
		const size_t stringsize = strlen(typeName);
		auto* newRec = new stratModelCharacterRecord();
		const auto typeNameCopy2 = new char[stringsize];
		strcpy(typeNameCopy2, typeName);
		newRec->modelId = typeNameCopy2;
		const auto skeletonCopy = new char[strlen(skeletonname)];
		strcpy(skeletonCopy, skeletonname);
		newRec->skeletonname = skeletonCopy;
		const auto caspathCopy = new char[strlen(caspath)];
		strcpy(caspathCopy, caspath);
		newRec->caspath = caspathCopy;
		const auto shadowpathCopy = new char[strlen(shadowcaspath)];
		strcpy(shadowpathCopy, shadowcaspath);
		newRec->shadowcaspath = shadowpathCopy;
		const auto textureCopy = new char[strlen(texturepath)];
		strcpy(textureCopy, texturepath);
		newRec->texturepath = textureCopy;
		newRec->scale = scale;
		newRec->entry = buildCharacterCas(newRec->skeletonname, newRec->caspath, newRec->shadowcaspath, newRec->modelId, newRec->texturepath, newRec->scale);
		characterStratModels.push_back(newRec);
	}


	stratModelArrayEntry* buildCharacterCas(const char* skeletonname, const char* caspath, const char* shadowcaspath, const char* typeName, const char* texturepath, float scale)
	{
		//build new cas file
		const size_t stringsize = strlen(typeName);
		const DWORD skeleton = getCasAnimSet(skeletonname);

		std::string texturepathString = texturepath;
		size_t pos = texturepathString.find_last_of("\\/") + 1;
		texturepathString = texturepathString.substr(0, pos);
		for (int i = 0; i < 2; i++)
		{
			pos = texturepathString.find_first_of("\\/") + 1;
			texturepathString = texturepathString.substr(pos);
		}
		const int textureindex = readTGAfile(texturepath);
		
		const DWORD newModelFlexi = createModelFlexi(false);
		const DWORD newModelFlexiShadow = createModelFlexi(true);
		DWORD stratmodel = loadStratCAS(caspath, false) + 0x64;
		DWORD stratmodelShadow = loadStratCAS(shadowcaspath, true) + 0x64;

		fixModelFlexi(false, stratmodel, newModelFlexi, texturepathString.c_str(), skeleton, scale);
		fixModelFlexi(true, stratmodelShadow, newModelFlexiShadow, texturepathString.c_str(), skeleton, scale);

		auto* newEntry = new stratModelArrayEntry;
		newEntry->model_flexi_m = newModelFlexi;
		newEntry->zeropoint = 0; //just filling the 0 stuff with loops and assignments so that there isn't random crap in there, we don't know what's used as 0
		for (int& i : newEntry->zeroarray)
		{
			i = 0;
		}
		newEntry->lodRange = 1000000.0f;
		for (int& i : newEntry->zeroarray2)
		{
			i = 0;
		}
		for (auto& [textureIndex, glossTextureIndex] : newEntry->modelTexturesFactionArray)
		{ //assigning texture for every faction
			textureIndex = textureindex;
			glossTextureIndex = -1;
		}
		for (int& i : newEntry->somearray)
		{
			i = -1;
		}
		for (int& i : newEntry->somearray2)
		{
			i = 0;
		}
		newEntry->lod2 = 1000000.0f;
		newEntry->scale = 0;
		newEntry->skeleton = skeleton;
		newEntry->modelCountNotShadow = 1;
		newEntry->indevRangeSquared = 160000;
		newEntry->shadow_model_flexi = newModelFlexiShadow;
		for (int i = 0; i < 7; i++) {
			newEntry->zeroarray3[i] = 0;
			newEntry->zeroarray4[i] = 0;
		}
		newEntry->lod3 = 1000000.0f;
		newEntry->modelCountShadow = 1;
		newEntry->zeroint = 0;
		const auto typeNameCopy = new char[stringsize];
		strcpy(typeNameCopy, typeName);
		newEntry->typeName = typeNameCopy;
		newEntry->N54788969 = 54788969;
		return newEntry;
	}

	// ReSharper disable once CppInconsistentNaming
	int readTGAfile(const char* path) //read new tga file and add to game tga database
	{
		DWORD funcAddr = codes::offsets.readTgaFile;
		DWORD tgaDataBase = dataOffsets::offsets.tgaDataBase;
		int dbIndex = 0;
		_asm
		{
			push 2
			push 2
			push path
			mov ecx, tgaDataBase
			mov eax, funcAddr
			call eax
			mov dbIndex, eax
		}

		return dbIndex;
	}
}
