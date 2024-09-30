#include "pch.h"
#include "cultures.h"

#include "campaign.h"
#include "dataOffsets.h"
#include "gameHelpers.h"

namespace cultures
{

    std::vector<std::shared_ptr<portraitDbEntry>> eopPortraitDb::portraits{};
    int eopPortraitDb::entryCount = 0;
    
    std::unordered_map<portraitType, std::string> folderNames = {
        {portraitType::civilians, "civilians"},
        {portraitType::generals, "generals"},
        {portraitType::rogues, "rogues"},
        {portraitType::princesses, "princesses"},
        {portraitType::merchants, "merchants"},
        {portraitType::priests, "priests"},
        {portraitType::heretics, "heretics"},
        {portraitType::witches, "witches"},
        {portraitType::inquisitors, "inquisitors"}
    };

    portraitType increment( portraitType p ) {
        return static_cast<portraitType>(static_cast<int>(p) + 1);
    }
    
    std::unordered_map<portraitAge, std::string> folderNamesAge = {
        {portraitAge::young, "young"},
        {portraitAge::old, "old"},
        {portraitAge::dead, "dead"}
    };

    portraitAge increment( portraitAge p ) {
        return static_cast<portraitAge>(static_cast<int>(p) + 1);
    }

    bool folderExists(const std::string& path)
    {
        struct stat info;
        if (stat(path.c_str(), &info) != 0)
            return false;
        
        return (info.st_mode & S_IFDIR) != 0;
    }

    bool fileExists(const std::string& path)
    {
        struct stat info;
        if (stat(path.c_str(), &info) != 0)
            return false;
        
        return (info.st_mode & S_IFREG) != 0;
    }

    const char* getReligionName(const int index)
    {
        const auto* religionDb = *reinterpret_cast <religionDatabase**>(dataOffsets::offsets.religionDatabase);
        const wchar_t* name = religionDb->religionNames[index]->string;
        // Determine the size of the required buffer
        const int size = WideCharToMultiByte(CP_UTF8, 0, name, -1, nullptr, 0, nullptr, nullptr);
        if (size == 0) {
            return nullptr;
        }
        // Allocate a buffer for the converted string
        const auto buffer = new char[size];
        if (buffer == nullptr) {
            // Allocation failed
            return nullptr;
        }
        // Convert the string
        WideCharToMultiByte(CP_UTF8, 0, name, -1, buffer, size, nullptr, nullptr);
        // Return the converted string
        return buffer;
    }

    std::vector<int> shuffleIndexesRandomly(std::vector<int> indexes)
    {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(indexes.begin(), indexes.end(), g);
        return indexes;
    }

    static std::vector<std::string> CULTURE_NAMES{};

    std::string getCultureName(int cultureID)
    {
        if (cultureID >= getCultureCount())
            return "";
        return CULTURE_NAMES[cultureID];
    }

    cultureSettlement* getCultureSettlement(const int settlementLevel, const int factionId)
    {
        const auto cultureDb = getCultureDb();
        const auto facRecord = factionHelpers::getFactionRecord(factionId);
        const auto cultureId = facRecord->facCulture->cultureID;
        return &cultureDb->cultures[cultureId].cultureSettlements[settlementLevel];
    }

    int getCultureCount()
    {
        return CULTURE_NAMES.size();
    }

    portraitDbEntry* eopPortraitDb::getEntry(int cultureID)
    {
        return portraits[cultureID].get();
    }
    	
    culturesDB* getCultureDb()
    {
        return reinterpret_cast<culturesDB*>(dataOffsets::offsets.cultureDatabase);
    }

    std::string getRandomPortrait(const int cultureId, const int religionId)
    {
        const auto cultureDb = reinterpret_cast<culturesDB*>(dataOffsets::offsets.cultureDatabase);
        std::vector<std::string> portraitPaths;
        const auto modName = gameHelpers::getModFolderName();
        string folderPath;
        folderPath.append("data/ui/");
        folderPath.append(cultureDb->cultures[cultureId].cultureName);
        folderPath.append("/portraits/portraits/young/");
        if (religionId > -1)
        {
            const auto religionName = getReligionName(religionId);
            folderPath.append(religionName);
            folderPath.append("_generals/");
        }
        else
            folderPath.append("generals/");
        if (folderExists(gameHelpers::getModPath() + "/" + folderPath))
        {
            auto portraitIndex = 0;
            while (true)
            {
                auto stringNum = std::to_string(portraitIndex);
                constexpr size_t n = 3;
                std::string result = std::string(n - stringNum.length(), '0') + stringNum;
                portraitIndex++;
                auto filePath = folderPath;
                filePath.append(result);
                filePath.append(".tga");
                if (fileExists(gameHelpers::getModPath() + "/" + filePath))
                {
                    portraitPaths.push_back(filePath);
                }
                else
                    break;
            }
        }
        if (portraitPaths.empty())
            return "";
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(portraitPaths.begin(), portraitPaths.end(), g);
        return portraitPaths[0];
    }
    
    
    void eopPortraitDb::createEopPortraitDb()
    {
        const auto cultureDb = reinterpret_cast<culturesDB*>(dataOffsets::offsets.cultureDatabase);
        auto religions = *reinterpret_cast <religionDatabase**>(dataOffsets::offsets.religionDatabase);
        auto modPath = gameHelpers::getModPath();
        for (int cultureID = 0; cultureID < cultureDb->culturesCount; cultureID++)
        {
            auto culture = cultureDb->cultures[cultureID];
            portraits.push_back(std::make_shared<portraitDbEntry>());
            CULTURE_NAMES.emplace_back(culture.cultureName);
            portraits[cultureID]->cultureName = culture.cultureName;
            portraits[cultureID]->cultureNameHash = culture.cultureNameHash;
            for (portraitType pType = portraitType::civilians; pType <= portraitType::inquisitors; pType = increment(pType))
            {
                auto characterType = folderNames[pType];
                for (auto pAge = portraitAge::young; pAge <= portraitAge::dead; pAge = increment(pAge))
                {
                    std::vector<int> portraitIndexes;
                    auto characterAge = folderNamesAge[pAge];
                    auto folderPathDefault = modPath + "/" + "data/ui/" + culture.cultureName + "/portraits/portraits/";
                    folderPathDefault.append(characterAge);
                    folderPathDefault.append("/");
                    folderPathDefault.append(characterType);
                    if (folderExists(folderPathDefault))
                    {
                        auto portraitIndex = 0;
                        while (true)
                        {
                            auto stringNum = std::to_string(portraitIndex);
                            size_t n = 3;
                            std::string result = std::string(n - stringNum.length(), '0') + stringNum;
                            auto filePath = folderPathDefault + "/" + result + ".tga";
                            if (fileExists(filePath))
                            {
                                portraitIndex++;
                                if (portraits[cultureID]->portraits[static_cast<int>(pType)].ages[static_cast<int>(pAge)].count < portraitIndex)
                                {
                                    portraitIndexes.push_back(portraitIndex);
                                    portraits[cultureID]->portraits[static_cast<int>(pType)].ages[static_cast<int>(pAge)].count = portraitIndex;
                                }
                            }
                            else
                                break;
                        }
                    }
                    else if (pAge == portraitAge::old)
                    {
                        portraits[cultureID]->portraits[static_cast<int>(pType)].ages[static_cast<int>(portraitAge::old)].count = portraits[cultureID]->portraits[static_cast<int>(pType)].ages[static_cast<int>(portraitAge::young)].count;
                        for (auto i = 0; i < portraits[cultureID]->portraits[static_cast<int>(pType)].ages[static_cast<int>(portraitAge::old)].count; i++)
                        {
                            portraitIndexes.push_back(portraits[cultureID]->portraits[static_cast<int>(pType)].ages[static_cast<int>(portraitAge::young)].portraitIndexes[i]);
                        }
                    }
                    else if (pAge == portraitAge::dead)
                    {
                        folderPathDefault = modPath + "/" + "data/ui/" + culture.cultureName + "/portraits/portraits/dead";
                        if (folderExists(folderPathDefault))
                        {
                            auto portraitIndex = 0;
                            while (true)
                            {
                                auto stringNum = std::to_string(portraitIndex);
                                size_t n = 3;
                                std::string result = std::string(n - stringNum.length(), '0') + stringNum;
                                auto filePath = folderPathDefault + "/" + result + ".tga";
                                if (fileExists(filePath))
                                {
                                    portraitIndex++;
                                    if (portraits[cultureID]->portraits[static_cast<int>(pType)].ages[static_cast<int>(pAge)].count < portraitIndex)
                                    {
                                        portraitIndexes.push_back(portraitIndex);
                                        portraits[cultureID]->portraits[static_cast<int>(pType)].ages[static_cast<int>(pAge)].count = portraitIndex;
                                    }
                                }
                                else
                                    break;
                            }
                        }
                    }
                    for (int religionID = 0; religionID < religions->religionCount; religionID++)
                    {
                        auto religionName = getReligionName(religionID);
                        auto folderPath = modPath + "/" + "data/ui/" + culture.cultureName + "/portraits/portraits/";
                        folderPath.append(characterAge);
                        folderPath.append("/");
                        folderPath.append(religionName);
                        folderPath.append("_");
                        folderPath.append(characterType);
                        if (folderExists(folderPath))
                        {
                            auto portraitIndex = 0;
                            while (true)
                            {
                                auto stringNum = std::to_string(portraitIndex);
                                size_t n = 3;
                                std::string result = std::string(n - stringNum.length(), '0') + stringNum;
                                auto filePath = folderPathDefault + "/" + result + ".tga";
                                if (fileExists(filePath))
                                {
                                    portraitIndex++;
                                    if (portraits[cultureID]->portraits[static_cast<int>(pType)].ages[static_cast<int>(pAge)].count < portraitIndex)
                                    {
                                        portraitIndexes.push_back(portraitIndex);
                                        portraits[cultureID]->portraits[static_cast<int>(pType)].ages[static_cast<int>(pAge)].count = portraitIndex;
                                    }
                                }
                                else
                                    break;
                            }
                        }
                    }
                    portraits[cultureID]->portraits[static_cast<int>(pType)].ages[static_cast<int>(pAge)].portraitIndexes = new int[portraits[cultureID]->portraits[static_cast<int>(pType)].ages[static_cast<int>(pAge)].count];
                    portraitIndexes = shuffleIndexesRandomly(portraitIndexes);
                    for (auto i = 0; i < static_cast<int>(portraitIndexes.size()); i++)
                    {
                        portraits[cultureID]->portraits[static_cast<int>(pType)].ages[static_cast<int>(pAge)].portraitIndexes[i] = portraitIndexes[i];
                    }
                }
            }
        }
    }
}
