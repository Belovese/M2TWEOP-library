#include "pch.h"
#include "onlineThings.h"
#include "imgui_notify.h"

#include "onlineThings2.h"
#include "globals.h"
#include "fort.h"
#include "unit.h"
#include "characterRecord.h"

#include "techFuncs.h"
#include "imgui/imgui.h"
#include "character.h"
#include "faction.h"
#include "army.h"

#include "battle.h"
#include "campaign.h"
#include "strategyMap.h"
#include "imgui/ImFileDialog.h"
#include "imgui/imgui_internal.h"
namespace battleCreator
{
	struct
	{
		bool isGenerationNeeded = true;
		bool isResultTransferNeeded = true;
		bool isPlannedRetreatRoute = true;

		atomic_bool isRunStarted{ false };
		atomic_bool isRunEnded{ false };



		bool isTransferResultsRunning = false;

		std::string lastSettlementWorldRec;
	}data;

	void setCreationNeeded(bool isNeeded)
	{
		data.isGenerationNeeded = isNeeded;
	}

	bool getIsGenerationNeeded()
	{
		return data.isGenerationNeeded;
	}
	bool IsRetreatsOverhaulEnabled()
	{
		return data.isPlannedRetreatRoute;
	}

	void readParams()
	{
		jsn::json json;

		std::string fPath = globals::dataS.modPath;
		fPath += "\\eopData\\config\\battlesCfg.json";


		std::ifstream f1(fPath);

		f1 >> json;

		f1.close();

		try
		{
			json.at("enableAutoGeneration").get_to(data.isGenerationNeeded);
			json.at("enableResultsTransfer").get_to(data.isResultTransferNeeded);
			json.at("isPlannedRetreatRoute").get_to(data.isPlannedRetreatRoute);
		}
		catch (jsn::json::type_error& e)
		{
			MessageBoxA(NULL, e.what(), "Warning!", MB_APPLMODAL | MB_SETFOREGROUND);
		}
	}

	string makeWeatherString()
	{
		string retS;
		if (campaignHelpers::getCampaignData()->season == 0)
		{
			retS = "summer";
		}
		else
		{
			retS = "winter";
		}
		return retS;
	}
	
	string makeBattleName()
	{
		string battleName = makeWeatherString();
		const auto campaignData = campaignHelpers::getCampaignData();
		battleName += "_T";
		battleName += to_string(campaignData->turnNumber);
		battleName += "_";
		const factionStruct* currFac = campaignData->currentFactionTurn;
		battleName += currFac->factionRecord->facName;
		return battleName;
	}


	set<string>getFacsNamesInBattle()
	{
		set<string>retSet;
		battleDataS* battleData = battleHelpers::getBattleData();

		for (int i = 0; i < battleData->sidesNum; i++)
		{
			battleSide& side = battleData->sides[i];


			for (int j = 0; j < side.armiesNum; j++)
			{
				armyStruct* army = side.armies[j].stack;

				retSet.emplace(army->faction->factionRecord->facName);
			}
		}

		return retSet;
	}


	string getSettlLevelString(int lev)
	{
		string s;
		if (lev == 0)
			s = "village";
		else if (lev == 1)
		{
			s = "town";
		}
		else if (lev == 2)
		{
			s = "large_town";
		}
		else if (lev == 3)
		{
			s = "city";
		}
		else if (lev == 4)
		{
			s = "large_city";
		}
		else if (lev == 5)
		{
			s = "huge_city";
		}
		else
		{
			//MessageBoxA(NULL, "Not correct town level :-(", "Not good, see descr_battle.txt", NULL);
			s = "unknown level :-) = ";
			s.append(to_string(lev));
		}
		return s;
	}
	void writeFortInfo(vector<string>& fileStrings, fortStruct* fort)
	{
		fileStrings.push_back(";this is fort, placed by eop.");
		fileStrings.push_back("settlement castle");
		fileStrings.push_back("{");


		fileStrings.push_back(string("	level ").append("huge_city"));
		fileStrings.push_back(string("	tile ").append(to_string(fort->xCoord).append(" ").append(to_string(fort->yCoord))));
		fileStrings.push_back("	year_founded 0");
		fileStrings.push_back("	population 999");
		fileStrings.push_back("	plan_set default_set");

		fileStrings.push_back("}");
	}
	void writeSettlementInfo(vector<string>& fileStrings, settlementStruct* sett)
	{
		string tempS;
		if (sett->isCastle == 1)
		{
			fileStrings.push_back("settlement castle");
		}
		else
		{
			fileStrings.push_back("settlement");
		}

		fileStrings.push_back("{");

		fileStrings.push_back(string("	level ").append(getSettlLevelString(sett->level)));
		fileStrings.push_back(string("	tile ").append(to_string(sett->xCoord).append(" ").append(to_string(sett->yCoord))));
		fileStrings.push_back("	year_founded 0");
		fileStrings.push_back("	population 999");
		fileStrings.push_back("	plan_set default_set");


		const auto* facCreator = factionHelpers::getFactionRecord(sett->fac_creatorModNum);
		if (facCreator != nullptr)
		{
			fileStrings.push_back(string("	faction_creator ").append(facCreator->facName));
		}
		for (int bnum = 0; bnum < sett->buildingsNum; bnum++)
		{
			fileStrings.push_back("	building");

			string buildType = sett->buildings[bnum]->edbEntry->type;
			buildingLevel level = sett->buildings[bnum]->edbEntry->levels[sett->buildings[bnum]->level];
			string buildName = level.name;
			fileStrings.push_back("	{");

			tempS = "		type ";
			tempS.append(buildType);
			tempS.append(" ");
			tempS.append(buildName);
			fileStrings.push_back(tempS);

			fileStrings.push_back("	}");
		}
		fileStrings.push_back("}");
	}

	void writeCharacterInfo(vector<string>& fileStrings, character* gen)
	{
		int age = (gen->characterRecord->age >> 3) & 0x7f;

		string tempS = "character	";
		if (gen->characterRecord->fullName == nullptr || string(gen->characterRecord->fullName).empty())
		{
			tempS.append("default");
		}
		else
		{
			tempS.append(gen->characterRecord->fullName);
		}
		tempS.append(", named character, male");
		if (gen->characterRecord->status & 2)
		{
			tempS.append(", heir");
		}
		else if (gen->characterRecord->status & 5)
		{
			tempS.append(", leader");
		}
		tempS.append(", age ").append(to_string(age));
		tempS.append(", x ").append(to_string(gen->xCoord));
		tempS.append(", y ").append(to_string(gen->yCoord));
		if (gen->characterRecord->portrait_custom)
		{
			tempS.append(", portrait ").append(gen->characterRecord->portrait_custom);
		}
		if (gen->characterRecord->modelName)
		{
			tempS.append(", battle_model ").append(gen->characterRecord->modelName);
		}
		if (gen->ability)
		{
			tempS.append(", hero_ability ").append(gen->ability);
		}
		if (gen->characterRecord->label)
		{
			tempS.append(", label ").append(gen->characterRecord->label);
		}




		fileStrings.push_back(tempS);

		if (gen->characterRecord->traitList.head != nullptr)
		{
			tempS = "traits";
			auto traitCont = gen->characterRecord->traitList.head;
			while (traitCont != nullptr)
			{
				tempS.append(" ").append(traitCont->element->traitEntry->name).append(" ").append(to_string(traitCont->element->level->level)).append(" ,");

				traitCont = traitCont->next;
			};
			fileStrings.push_back(tempS);
		}

		if (gen->characterRecord->ancNum != 0)
		{
			tempS = "ancillaries";
			UINT32 ancNum = gen->characterRecord->ancNum;
			for (UINT32 i = 0; i < ancNum; i++)
			{
				tempS.append(" ").append(gen->characterRecord->ancillaries[i]->dataAnc->ancName).append(" ,");
			}
			fileStrings.push_back(tempS);
		}
	}
	void createHeaderSection(vector<string>& fileStrings)
	{
		string battleName = makeBattleName();
		string tempS;

		fileStrings.push_back(";Generated by new M2TW Engine Overhaul Project\n");

		tempS = "battle		";
		tempS.append(battleName);
		tempS.append("		multiplayer");
		fileStrings.push_back(tempS);

		fileStrings.push_back("playable");

		set<string>facsNames = getFacsNamesInBattle();
		for (const string& fName : facsNames)
		{
			fileStrings.push_back(string("	").append(fName));
		}
		fileStrings.push_back("end");
		fileStrings.push_back("nonplayable");
		fileStrings.push_back("end");
		fileStrings.push_back("\n");
		fileStrings.push_back("\n");

		const auto year = campaignHelpers::getCampaignData()->currentDate;
		tempS = "start_date	";
		tempS.append(to_string(year)).append(" ").append(makeWeatherString());
		fileStrings.push_back(tempS);

		tempS = "end_date	";
		tempS.append(to_string(year)).append(" ").append(makeWeatherString());
		fileStrings.push_back(tempS);
		fileStrings.push_back("\n");
		fileStrings.push_back("\n");
		fileStrings.push_back(";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;");

	}
	void createFactionsSection(vector<string>& fileStrings)
	{
		battleDataS* battleData = battleHelpers::getBattleData();
		string tempS;

		fileStrings.push_back("; >>>> start of factions section <<<<\n");

		for (int i = 0; i < battleData->sidesNum; i++)
		{
			battleSide& side = battleData->sides[i];

			set<string> facsNames = getFacsNamesInBattle();


			for (int j = 0; j < side.armiesNum; j++)
			{
				armyStruct* army = side.armies[j].stack;
				if (facsNames.count(army->faction->factionRecord->facName))
				{
					facsNames.erase(army->faction->factionRecord->facName);

					fileStrings.push_back(string("faction	").append(army->faction->factionRecord->facName));
				}

				if (army->gen != nullptr && army->gen->xCoord == battleData->xCoord
					&& army->gen->yCoord == battleData->yCoord
					)
				{
					const auto tile = stratMapHelpers::getTile(army->gen->xCoord, army->gen->yCoord);
					if (settlementStruct* sett = tile->getSettlement(); sett)
						writeSettlementInfo(fileStrings, sett);
					if (fortStruct* fort = tile->getFort(); fort)
						writeFortInfo(fileStrings, fort);
				}

				if (army->gen != nullptr)
				{
					character* gen = army->gen;
					writeCharacterInfo(fileStrings, gen);
				}
				else
				{
					fileStrings.push_back("character	default, general, male");
				}
				fileStrings.push_back("\n");

				fileStrings.push_back("army");
				for (int k = 0; k < army->numOfUnits; k++)
				{
					unit* un = army->units[k];
					tempS = "unit		";
					tempS.append(un->eduEntry->eduType);
					tempS.append("				soldiers ").append(to_string(un->SoldierCountStrat));
					tempS.append(" exp ").append(to_string(un->expScreen));
					tempS.append(" armour ").append(to_string(un->avgArmourUpg));
					tempS.append(" weapon_lvl ").append(to_string(un->avgWeaponUpg));

					fileStrings.push_back(tempS);
				}
				fileStrings.push_back("\n");
			}
		}
		fileStrings.push_back(";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;");
	}
	void createBattleSection(vector<string>& fileStrings)
	{
		battleDataS* battleData = battleHelpers::getBattleData();
		string tempS;

		fileStrings.push_back("; >>>> start of battle section <<<<\n");

		tempS = "battle	";
		tempS.append(to_string(battleData->xCoord)).append(", ").append(to_string(battleData->yCoord));
		fileStrings.push_back(tempS);

		tempS = "battle_time	";
		tempS.append(to_string( campaignHelpers::getCampaignData()->timeAtStartBattle));
		tempS.append("	0.00");
		fileStrings.push_back(tempS);

		fileStrings.push_back("weather	clear\n");

		tempS = "home_faction	";
		tempS.append(battleData->sides[0].armies[0].stack->faction->factionRecord->facName);
		fileStrings.push_back(tempS);


		for (int i = 0; i < battleData->sidesNum; i++)
		{
			battleSide& side = battleData->sides[i];
			tempS = "alliance	";
			if (side.canDeploy == true)
			{
				tempS.append("can_deploy	");
			}
			tempS.append("can_view	");
			set<string>sideFactions;
			for (int j = 0; j < side.armiesNum; j++)
			{
				armyStruct* army = side.armies[j].stack;

				sideFactions.emplace(army->faction->factionRecord->facName);
			}
			for (const string& fac : sideFactions)
			{
				tempS.append(fac).append(", ");
			}
			if (side.isDefender == true)
			{
				tempS.append("	defending");
			}
			else
			{
				tempS.append("	attacking");
			}
			fileStrings.push_back(tempS);
		}



		for (int i = 0; i < battleData->sidesNum; i++)
		{
			battleSide& side = battleData->sides[i];
			map<string, int>facArms;

			for (int j = 0; j < side.armiesNum; j++)
			{
				armyStruct* army = side.armies[j].stack;
				char* armyFacName = army->faction->factionRecord->facName;

				fileStrings.push_back("");
				tempS = "army	";
				tempS.append(armyFacName);



				int ifHere = facArms.count(armyFacName);
				int newCount = 0;
				if (ifHere > 0)
				{
					auto res = facArms.find(armyFacName);

					res->second++;
					newCount = res->second;
				}

				facArms.insert(pair<string, int>(armyFacName, newCount));


				tempS.append(", ");
				tempS.append(to_string(newCount));
				tempS.append(", no_withdraw, supporting_armies 0, reform_point ");
				tempS.append(to_string(army->reform_point_x));
				tempS.append(", ");
				tempS.append(to_string(army->reform_point_y));
				fileStrings.push_back(tempS);
				fileStrings.push_back("\n");

				int coordsPairsNum = side.armies[j].deploymentAreas.get(0)->area.coordsNum;
				battlePos* coords = side.armies[j].deploymentAreas.get(0)->area.coordsPairs;
				for (int k = 0; k < coordsPairsNum; k++)
				{
					tempS = "deployment_area_point	";
					tempS.append(to_string(coords[k].xCoord));
					tempS.append(", ");
					tempS.append(to_string(coords[k].yCoord));
					fileStrings.push_back(tempS);
				}
				fileStrings.push_back("\n");

				for (int k = 0; k < army->numOfUnits; k++)
				{
					unit* un = army->units[k];
					while (1)
					{
						if (un->soldiersBattleArr == nullptr)
						{
							continue;
						}
						if (un->soldiersBattleArr[0]->xCoord == 0 && un->soldiersBattleArr[0]->yCoord == 0 && un->soldiersBattleArr[0]->zCoord == 0)
						{
							continue;
						}

						tempS = "unit			";
						tempS.append(to_string(k));
						tempS.append(", ");
						tempS.append(to_string(un->soldiersBattleArr[0]->xCoord));
						tempS.append(", ");
						tempS.append(to_string(un->soldiersBattleArr[0]->yCoord));
						tempS.append(", ");
						tempS.append("0");
						tempS.append(", formation_type square, formation_width ");
						tempS.append("50");


						if (un->eduEntry->category == 0 && un->engineRec != nullptr)
						{
							engineRecord* enRec = un->engineRec;
							if (enRec->classID == 18)//ram
							{
								tempS.append(", attach_engine ram");
							}
							else if (enRec->classID == 19)//ladder
							{
								tempS.append(", attach_engine ladder");
							}
							else if (enRec->classID == 17)//tower
							{
								tempS.append(", attach_engine tower");
							}
						}

						fileStrings.push_back(tempS);
						break;
					}

				}
			}

		}
		fileStrings.push_back(";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;");
	}

	void createObjectivesSection(vector<string>& fileStrings)
	{
		battleDataS* battleData = battleHelpers::getBattleData();
		string tempS;

		fileStrings.push_back("; >>>> start of objectives scripting section <<<<\n");

		fileStrings.push_back("\nobjectives");

		for (int i = 0; i < battleData->sidesNum; i++)
		{
			battleSide& side = battleData->sides[i];
			tempS = "\nalliance ";
			tempS.append(to_string(i));

			fileStrings.push_back(tempS);

			for (int i = 0; i < 4; i++)
			{
				DWORD winCond = side.winConditions[i];
				if (winCond != 0)
				{
					tempS = "condition ";
					tempS.append(battleHelpers::getWinConditionS(winCond));
					fileStrings.push_back(tempS);
				}
			}


		}
		fileStrings.push_back("\n;Generation completed");
	}



	void createBattle()
	{

		std::string bName = makeBattleName();

		ImGuiToast bMsg(ImGuiToastType_Info, 25000);

		bMsg.set_title("Battle generation");
		bMsg.set_content("Started generation of %s", bName.c_str());
		ImGui::InsertNotification(bMsg);


		vector<string>fileStrings;
		createHeaderSection(fileStrings);


		bMsg = ImGuiToast(ImGuiToastType_Info, 10000);
		bMsg.set_title("Battle generation");
		bMsg.set_content("Header section of %s done", bName.c_str());
		ImGui::InsertNotification(bMsg);


		createFactionsSection(fileStrings);

		bMsg = ImGuiToast(ImGuiToastType_Info, 10000);
		bMsg.set_title("Battle generation");
		bMsg.set_content("Factions section of %s done", bName.c_str());
		ImGui::InsertNotification(bMsg);

		createBattleSection(fileStrings);

		bMsg = ImGuiToast(ImGuiToastType_Info, 10000);
		bMsg.set_title("Battle generation");
		bMsg.set_content("Battle section of %s done", bName.c_str());
		ImGui::InsertNotification(bMsg);

		createObjectivesSection(fileStrings);

		bMsg = ImGuiToast(ImGuiToastType_Info, 10000);
		bMsg.set_title("Battle generation");
		bMsg.set_content("Objectives section of %s done", bName.c_str());
		ImGui::InsertNotification(bMsg);

		std::string fPath = globals::dataS.modPath;
		fPath += "\\eopData\\config";
		filesystem::create_directory(fPath);
		fPath += "\\lastBattle";
		filesystem::remove_all(fPath);
		filesystem::create_directory(fPath);
		fPath += "\\";
		fPath += bName;


		filesystem::create_directory(fPath);
		writeCharactersJson(fPath + "\\m2tweopDescr1.json");
		writeSettlementJson(fPath + "\\m2tweopDescr2.json", data.lastSettlementWorldRec);

		fPath += "\\descr_battle.txt";
		ofstream f1(fPath);
		for (string& s : fileStrings)
		{
			f1 << s << endl;
		}
		f1.close();

		bMsg = ImGuiToast(ImGuiToastType_Success, 25000);
		bMsg.set_title("Battle generation");
		bMsg.set_content("Done generation of %s", bName.c_str());
		ImGui::InsertNotification(bMsg);
	}




	void doWork(std::atomic_bool& isStarted)
	{
		if (isStarted == true)
		{
			return;
		}

		if (data.isGenerationNeeded == false)
		{
			return;
		}

		isStarted = true;


		initStructsForResults();
		createBattle();

		//clearStructs();
		isStarted = false;
	}
	void startCreationThread()
	{


		if (data.isGenerationNeeded == false)
		{
			return;
		}




		std::thread thrUrl(
			doWork, std::ref(data.isRunStarted));
		thrUrl.detach();
	}
	void onBattleResultsScreen()
	{
		if (data.isGenerationNeeded == false)
		{
			return;
		}

		std::thread thrUrl(createResultsFile);
		thrUrl.detach();
	}



	bool battleResultsMenuOpened = false;
	bool battleResultsSuccessOpened = false;
	void onBattleStratScreen()
	{
		if (data.isResultTransferNeeded == false)
		{
			return;
		}

		battleResultsMenuOpened = true;
	}

	void onHotseatScreen()
	{
		battleResultsMenuOpened = false;
	}

	void onPostBattleStratScreen()
	{
		if (data.isResultTransferNeeded == false)
		{
			return;
		}
		if (data.isTransferResultsRunning == false)
		{
			return;
		}

		data.isTransferResultsRunning = false;

		transferResults2();
	}

	void draw(LPDIRECT3DDEVICE9 pDevice)
	{
		if (battleResultsMenuOpened == true)
		{
			ImGui::OpenPopup("Select winner##Online battles results transfering");
		}
		if (battleResultsSuccessOpened == true)
		{
			ImGui::OpenPopup("Succes##Online battles results transfer");
		}


		ImGui::SetNextWindowSize({ 300,0 }, ImGuiCond_Always);
		if (ImGui::BeginPopupModal("Select winner##Online battles results transfering", &battleResultsMenuOpened, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
		{
			static int resolveVariant = 0;

			const float ItemSpacing = ImGui::GetStyle().ItemSpacing.x;
			static float defenderButtonWidth = 10.0f;
			static float drawButtonWidth = 10.0f;
			float pos = 0;




			ImGui::RadioButton("attacker", &resolveVariant, 0);

			ImGui::SameLine(ImGui::GetWindowWidth() / 2 - defenderButtonWidth / 2);
			ImGui::RadioButton("defender", &resolveVariant, 1);
			defenderButtonWidth = ImGui::GetItemRectSize().x;

			pos = drawButtonWidth + ItemSpacing;
			ImGui::SameLine(ImGui::GetWindowWidth() - pos);
			ImGui::RadioButton("draw", &resolveVariant, 2);
			drawButtonWidth = ImGui::GetItemRectSize().x;


			ImGui::NewLine();

			static std::string selectedFile;
			static bool noCasualities = false;

			if (ImGui::Button("select results file", { -1.0f,0.0f }))
			{
				ifd::FileDialog::Instance().Open("select results file", "select results file", "results file (*.json;){.json},.*");
			}
			if (ifd::FileDialog::Instance().IsDone("select results file")) {
				if (ifd::FileDialog::Instance().HasResult()) {
					selectedFile = ifd::FileDialog::Instance().GetResult().u8string();
				}
				ifd::FileDialog::Instance().Close();
			}
			//ImGui::SameLine();
			ImGui::Checkbox("no casualities", &noCasualities);

			if (!selectedFile.empty()|| noCasualities==true)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				ImGui::Separator();
				ImGui::TextWrapped(selectedFile.c_str());
				ImGui::Separator();
				//ImGui::InputText("Selected results file", &selectedFile);
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();

				ImGui::NewLine();
				ImGui::NewLine();
				ImGui::NewLine();
				if (ImGui::Button("Transfer results now", { -1.0f,0.0f }))
				{
					bool res = transferResults(selectedFile, resolveVariant, noCasualities);
					if (res == true)
					{
						data.isTransferResultsRunning = true;
						battleResultsMenuOpened = false;
						battleResultsSuccessOpened = true;
						ImGui::CloseCurrentPopup();
						selectedFile.clear();
						resolveVariant = 0;
						noCasualities = false;
					}
				}
			}
			else
			{
				ImGui::NewLine();
				ImGui::NewLine();
				ImGui::NewLine();
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);

				ImGui::Button("Transfer results now", { -1.0f,0.0f });

				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}

			ImGui::NewLine();

			/*if (ImGui::Button("decline"))
			{
				ImGui::CloseCurrentPopup();
				battleResultsMenuOpened = false;
			}*/

			ImGui::EndPopup();
		}


		ImGui::SetNextWindowSize({ 300,0 }, ImGuiCond_Always);
		if (ImGui::BeginPopupModal("Succes##Online battles results transfer", &battleResultsSuccessOpened, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
		{

			if (ImGui::Button("Continue the game", { -1.0f,0.0f }))
			{
				ImGui::CloseCurrentPopup();
				battleResultsSuccessOpened = false;
			}
			ImGui::EndPopup();
		}
	}

	void addCharactersToCustomBattleArmy(armyStruct* army, const std::string& relativePath)
	{

		std::filesystem::path charactersPath = relativePath;
		charactersPath.replace_filename("m2tweopDescr1.json");
		onLoadCharacter(army, charactersPath);
	}
	string selectWorldpkgdesc(const std::string& selectedRec, const std::string& selectedGroup)
	{
		if (selectedGroup != "settlement")
		{
			return "";
		}

		auto campaign = campaignHelpers::getCampaignData();
		if (!campaign)
			return "";
		if (campaign->currentDescrFile == nullptr)
			return "";

		std::string relativePath = techFuncs::uniToAnsi(campaign->currentDescrFile);

		if (relativePath.find("battle") != std::string::npos)//need load/
		{
			return battleCreator::getWorldpkgDescFromBattleFile(relativePath);
		}
		else//need save
		{
			return "";
		}

	}
	void OnLoadSettlementWorldpkgdesc(const std::string& selectedRec, const std::string& selectedGroup)
	{
		if (selectedGroup != "settlement")
		{
			return;
		}
		data.lastSettlementWorldRec = selectedRec;
	}
	string getWorldpkgDescFromBattleFile(const std::string& relativePath)
	{
		std::filesystem::path settPath = relativePath;
		settPath.replace_filename("m2tweopDescr2.json");
		return onLoadSettlementWord(settPath);
	}
};
