﻿///
//![Lua logo](../Lua.png)
//@module LuaPlugin
//@author Fynn
//@license GPL-3.0
#include "pch.h"
#include "eopdu.h"
#include "functionsOffsets.h"
#include "gameHelpers.h"

std::vector<std::shared_ptr<eopEduEntry>> eopDu::eopUnitDb{};
std::unordered_map<std::string, std::shared_ptr<eopEduEntry>> eopDu::eopUnitLookup{};
std::unordered_map<int, std::shared_ptr<eopEduEntry>> eopDu::eopUnitIndexLookup{};

//struct eduThingsG
//{
//	vector<eopEduEntry>eopEdu;
//}eopData;

eopEduEntry::eopEduEntry(int baseIdx, int newIdx)
{
    eduEntry* oldEn = eopDu::getEduEntry(baseIdx);
    if (oldEn == nullptr)
    {
        std::string errs = "Can`t create eop`s unit entry:\n";
        errs += std::to_string(newIdx);
        MessageBoxA(NULL, errs.c_str(), "ERROR!", NULL);
        exit(0);
    }

    data.edu = *oldEn;
    data.edu.index = newIdx;
    isFileAdded = false;
    eopTypeName.append(data.edu.eduType);
    eopTypeName.append("_");
    eopTypeName.append(std::to_string(newIdx));//added to make typename unique
    originalTypeName = eopTypeName;
}
eopEduEntry::eopEduEntry(const char* fileName, int newIdx)
{
    int isOk = eopDu::readEduFile(fileName, &data.edu);
    if (isOk == 0)
    {
        std::string errS = "Can`t read edu file: ";
        errS += fileName;
        MessageBoxA(NULL, errS.c_str(), "ERROR!", NULL);
        throw "not good";
    }
    data.edu.index = newIdx;
    originalTypeName = data.edu.eduType;
    isFileAdded = true;
    eopTypeName.append(data.edu.eduType);
    eopTypeName.append("_");
    eopTypeName.append(std::to_string(newIdx));//added to make typename unique
}

struct pseudoFile {
	char* _ptr;
	int _cnt;
	char* _base;
	int _flag;
	char* _file;
	char* _charbuf;
	int _bufsiz;
	char* _tmpfname;
};

static void prepareEduEntry(eduEntry*& entryForFilling)
{
	GAME_FUNC(void(__thiscall*)(eduEntry*), createEduEntry)(entryForFilling);
}
	
static std::string replace_all(
	const std::string& str,   // where to work
	const std::string& find,  // substitute 'find'
	const std::string& replace //      by 'replace'
)
{
	using namespace std;
	string result;
	size_t find_len = find.size();
	size_t pos, from = 0;
	while (string::npos != (pos = str.find(find, from))) {
		result.append(str, from, pos - from);
		result.append(replace);
		from = pos + find_len;
	}
	result.append(str, from, string::npos);
	return result;
}


int eopDu::readEduFile(const std::string& fileName, eduEntry* entryForFilling)
{
	gameHelpers::logStringGame("Reading EDU file: " + fileName);
	std::string unitString;

	std::ifstream eduFile;
	eduFile.open(fileName);

	std::stringstream strStream;
	strStream << eduFile.rdbuf();
	unitString = strStream.str(); 
	eduFile.close();
	unitString=replace_all(unitString,"\n", " \n");
	prepareEduEntry(entryForFilling);

	pseudoFile ourF;
	ourF._ptr = (char*)unitString.c_str();
	ourF._cnt = unitString.size();
	ourF._base = (char*)(unitString.c_str()+ unitString.size());
	ourF._flag = (int)unitString.c_str();
	ourF._file = (char*)unitString.c_str();
	ourF._charbuf = (char*)unitString.c_str();
	ourF._bufsiz = unitString.size();
	ourF._tmpfname = (char*)&ourF;
	pseudoFile* pp = &ourF;

	int res = 0;

	DWORD adrFunc = codes::offsets.readEDUEntryFunc;
	_asm
		{
		push pp
		mov ecx, entryForFilling
		mov eax, adrFunc
		call eax
		mov res,eax
		}
	return res;
}

eduEntry* eopDu::getEopEduEntry(int idx)
{
	if (eopUnitIndexLookup.find(idx) != eopUnitIndexLookup.end())
		return &eopUnitIndexLookup[idx]->data.edu;
    return nullptr;
}
    
eduEntry* eopDu::addEopEduEntryFromFile(const char* fileName, int newIdx)
{
    if (getEduEntry(newIdx))
    {
        gameHelpers::logStringGame("Duplicate EOP index " + std::to_string(newIdx) + " in addEopEduEntryFromFile");
        return nullptr;
    }
    try
    {
        const auto sharedP = std::make_shared<eopEduEntry>(fileName, newIdx);
        if (unitHelpers::getEduEntryByName(sharedP->originalTypeName.c_str()))
        {
            gameHelpers::logStringGame("Duplicate unit name " + sharedP->originalTypeName + " in addEopEduEntryFromFile");
            std::string errS = "Can`t add: " + sharedP->originalTypeName + " Duplicate type name";
            MessageBoxA(NULL, errS.c_str(), "ERROR!", NULL);
            exit(0);
        }
        eopUnitDb.push_back(sharedP);
    	eopUnitLookup.insert_or_assign(eopUnitDb.back()->originalTypeName, sharedP);
    	eopUnitLookup.insert_or_assign(eopUnitDb.back()->eopTypeName, sharedP);
        eopUnitIndexLookup.insert_or_assign(newIdx, sharedP);
    }
    catch (...)
    {
        return nullptr;
    }
    return getEopEduEntry(newIdx);
}

eduEntry* eopDu::addEopEduEntry(int baseIdx, int newIdx)
{
    if (getEduEntry(newIdx))
    {
        gameHelpers::logStringGame("Duplicate EOP index " + to_string(newIdx) + " in addEopEduEntry");
        return nullptr;
    }
	//auto newEntry = std::make_shared<eopEduEntry>(baseIdx, newIdx);
    const auto sharedP = std::make_shared<eopEduEntry>(baseIdx, newIdx);
	eopUnitDb.push_back(sharedP);
    eopUnitLookup.insert_or_assign(eopUnitDb.back()->eopTypeName, sharedP);
    eopUnitIndexLookup.insert_or_assign(newIdx, sharedP);

    return getEopEduEntry(newIdx);
}

eopEduEntry* eopDu::getEopEduEntryInternal(int idx)
{
	if (eopUnitIndexLookup.find(idx) != eopUnitIndexLookup.end())
		return eopUnitIndexLookup[idx].get();
	for (const auto& entry : eopUnitDb)
	{
		if (entry->data.edu.index == idx)
		{
			return entry.get();
		}
	}
	return nullptr;
}

eopEduEntry* eopDu::getEopEduEntryInternalIterating(int idx)
{
	return eopUnitDb[idx].get();
}
	
int eopDu::getEopEntryNum()
{
	return eopUnitDb.size();
}

char* eopDu::getEopNameOfEduEntry(const eduEntry* entryAddress)
{
	if (eopUnitIndexLookup.find(entryAddress->index) != eopUnitIndexLookup.end())
		return const_cast<char*>(eopUnitIndexLookup[entryAddress->index]->eopTypeName.c_str());
	return nullptr;
}

eduEntry* eopDu::getEopEduEntryByName(const char* entryName)
{
	if (eopUnitLookup.find(entryName) != eopUnitLookup.end())
		return &eopUnitLookup[entryName]->data.edu;
	return nullptr;
}

eduEntry* eopDu::getEduEntry(int idx)
{
	eduEntry* entry = getEopEduEntry(idx);
	if (entry)
		return entry;
	entry = unitHelpers::getEDUEntryById(idx);
	return entry;
}

void eopDu::setEntrySoldierModelLua(int idx, const char* newModel)
{
	const eopEduEntry* entry = getEopEduEntryInternal(idx);
	entry->setEntrySoldierModel(newModel);
}

int eopDuHelpers::getEduEntryNum()
{
	return unitHelpers::getEdu()->numberOfEntries;
}

void eopEduEntry::setEntrySoldierModel(const char* newModel) const
{
	eduEntry* entry = eopDu::getEopEduEntry(data.edu.index);

	eopEduEntry* entryInternal = eopDu::getEopEduEntryInternal(data.edu.index);
	entryInternal->eopSoldierString = newModel;

	entry->modelEntry = unitHelpers::findBattleModel(newModel);
	gameStringHelpers::setHashedString(&entry->soldier, newModel);
}

namespace eopDuHelpers
{
	
	void addToLua(sol::state& luaState)
	{
		struct
		{
			sol::table M2TWEOPEDUTable;
		}tables;
		
		///M2TWEOPDU
		//@section M2TWEOPDU

		/***
		Basic M2TWEOPDU table. Contains descriptions of M2TWEOP unit types.

		@tfield addEopEduEntryFromFile addEopEduEntryFromFile
		@tfield addEopEduEntryFromEDUID addEopEduEntryFromEDUID
		@tfield getEopEduEntryByID getEopEduEntryByID Needed for change many parameters of entry.
		@tfield getEduEntry getEduEntry
		@tfield getEduEntryByType getEduEntryByType
		@tfield getEduIndexByType getEduIndexByType
		@tfield getEduEntryNum getEduEntryNum
		@tfield getEopEntryNum getEopEntryNum
		@table M2TWEOPDU
		*/
			
		tables.M2TWEOPEDUTable = luaState.create_table("M2TWEOPDU");
			
		/***
		Create new M2TWEOPDU entry from a file describing it.
		@function M2TWEOPDU.addEopEduEntryFromFile
		@tparam string filepath path to file with unit type description(like in export\_descr\_unit.txt, but only with one record and without comments)
		@tparam int eopEntryIndex Entry index, which will be assigned to a new record in DU (recommend starting from 1000, so that there is no confusion with records from EDU).
		@treturn eduEntry retEntry Usually you shouldn't use this value.
		@usage
		M2TWEOPDU.addEopEduEntryFromFile(M2TWEOP.getModPath().."/eopData/unitTypes/myTestType.txt",1000);
		*/
		tables.M2TWEOPEDUTable.set_function("addEopEduEntryFromFile", &eopDu::addEopEduEntryFromFile);

		/***
		Create new M2TWEOPDU entry.
		@function M2TWEOPDU.addEopEduEntryFromEDUID
		@tparam int baseEntryIndex Entry index number, which will be taken as the base for this DU record.
		@tparam int eopEntryIndex Entry index, which will be assigned to a new record in DU (recommend starting from 1000, so that there is no confusion with records from EDU).
		@treturn eduEntry retEntry Usually you shouldn't use this value.
		@usage
		M2TWEOPDU.addEopEduEntryFromEDUID(1,1000);
		*/
		tables.M2TWEOPEDUTable.set_function("addEopEduEntryFromEDUID", &eopDu::addEopEduEntry);

		/***
		Get eduEntry of a M2TWEOPDU entry. Needed to change many parameters of the entry.
		@function M2TWEOPDU.getEopEduEntryByID
		@tparam int eopEntryIndex Entry index in M2TWEOPDU.
		@treturn eduEntry retEntry
		@usage
		local eduEntryOfEOPDU=M2TWEOPDU.getEopEduEntryByID(1000);
		eduEntryOfEOPDU.SoldierCount=20;
		eduEntryOfEOPDU.Width=1.5;
		*/
		tables.M2TWEOPEDUTable.set_function("getEopEduEntryByID", &eopDu::getEopEduEntry);

		/***
		Get eduEntry by index. Needed to change many parameters of the entry.
		@function M2TWEOPDU.getEduEntry
		@tparam int EntryIndex Entry index (Values lower then 500 look for edu entry, values over 500 look for EOP edu entry).
		@treturn eduEntry retEntry
		@usage
		local eduEntry=M2TWEOPDU.getEduEntry(5);
		eduEntry.SoldierCount=20;
		eduEntry.Width=1.5;
		*/
		tables.M2TWEOPEDUTable.set_function("getEduEntry", &eopDu::getEduEntry);

		/***
		Get number of units in vanilla EDU.
		@function M2TWEOPDU.getEduEntryNum
		@treturn int entryNum
		@usage
		local num = M2TWEOPDU.getEduEntryNum();
		*/
		tables.M2TWEOPEDUTable.set_function("getEduEntryNum", &eopDuHelpers::getEduEntryNum);

		/***
		Get number of units in EOP EDU.
		@function M2TWEOPDU.getEopEntryNum
		@treturn int entryNum
		@usage
		local num = M2TWEOPDU.getEopEntryNum();
		*/
		tables.M2TWEOPEDUTable.set_function("getEopEntryNum", &eopDu::getEopEntryNum);

		/***
		Get eduEntry by edu type name. Needed to change many parameters of the entry.
		@function M2TWEOPDU.getEduEntryByType
		@tparam string type Unit type as in export_descr_unit.
		@treturn eduEntry retEntry
		@usage
		local eduEntry=M2TWEOPDU.getEduEntryByType("Peasants");
		eduEntry.SoldierCount=20;
		eduEntry.Width=1.5;
		*/
		tables.M2TWEOPEDUTable.set_function("getEduEntryByType", &unitHelpers::getEduEntryByName);

		/***
		Get edu index by edu type name. Needed to use many edu functions.
		@function M2TWEOPDU.getEduIndexByType
		@tparam string type Unit type as in export_descr_unit.
		@treturn int eduindex
		@usage
		local eduindex=M2TWEOPDU.getEduIndexByType("Peasants");
		M2TWEOPDU.setEntryStat(eduindex, eduStat.armour, 5, 1);
		*/
		tables.M2TWEOPEDUTable.set_function("getEduIndexByType", &unitHelpers::getEduIndex);
		/***
		Set unit info card for a M2TWEOPDU entry. Requirements for the location and parameters of the image are unchanged in relation to the game.
		@function M2TWEOPDU.setEntrySoldierModel
		@tparam int eopEntryIndex Entry index in M2TWEOPDU.
		@tparam string newSoldierModel
		@usage
		M2TWEOPDU.setEntrySoldierModel(1000,"Sword_and_Buckler_Men");
		*/
		tables.M2TWEOPEDUTable.set_function("setEntrySoldierModel", &eopDu::setEntrySoldierModelLua);
	}
}
