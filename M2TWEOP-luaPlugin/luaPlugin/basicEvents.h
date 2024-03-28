#pragma once
#include "pluginM2TWEOP.h"
#include <unordered_map>
#include <functional>
using namespace std;

extern "C" PLUGINM2TWEOP_API void onReadGameDbsAtStart();
extern "C" PLUGINM2TWEOP_API void onGameInit();
extern "C" PLUGINM2TWEOP_API void onAiTurn(aiFaction* aifaction);
extern "C" PLUGINM2TWEOP_API void onEndSiege(int x, int y);
extern "C" PLUGINM2TWEOP_API void onStartSiege(int x, int y);
std::unordered_map<int, const char*> religionNames = {
};


extern "C" PLUGINM2TWEOP_API void onClickAtTile(int x, int y);
extern "C" PLUGINM2TWEOP_API std::string * onSelectWorldpkgdesc(const char* selectedRec, const char* selectedGroup);
extern "C" PLUGINM2TWEOP_API int onfortificationlevelS(settlementStruct * settlement, bool* isCastle, bool* isChanged);
extern "C" PLUGINM2TWEOP_API void onChangeTurnNum(int num);
extern "C" PLUGINM2TWEOP_API void onNewGameStart();
extern "C" PLUGINM2TWEOP_API void onCampaignMapLoaded();
extern "C" PLUGINM2TWEOP_API void onLoadGamePl(std::vector<std::string>*saveFiles);
extern "C" PLUGINM2TWEOP_API std::vector<std::string>*onSaveGamePl(UNICODE_STRING * *&savePath);
extern "C" PLUGINM2TWEOP_API void onEventWrapper(DWORD eventAddr, DWORD * *vTab, DWORD arg2);


#define tryLuaBasicEventFunk(luaFunc)  \
auto funcResult = luaFunc;\
if (!funcResult.valid())\
{\
	sol::error luaError = funcResult;\
	MessageBoxA(NULL, luaError.what(), "Lua exception!", NULL);\
	if (plugData::data.luaAll.checkVar("terminateAtLuaException", 1) == true)\
	{\
		terminate();\
	}\
}\


void checkLuaFunc(sol::function** lRef);
namespace gameEvents
{
	regionStruct* getCharacterRegion(const namedCharacter* character);

	enum class EventType
	{
		standardEvent = 1,
		attackResidenceEvent = 2
	};

	class EventBase
	{
	public:
		virtual ~EventBase() = default;
		virtual int callEvent(DWORD** vTab) = 0;
	};

	template<EventType EvType> class Event :public EventBase
	{
	public:

		sol::function* funk;

		Event(const std::string& luaFunctionName, sol::state& luaState) : funk(new sol::function(luaState[luaFunctionName]))
		{
			checkLuaFunc(&funk);
		}

		int callEvent(DWORD** vTab) override
		{
			if (EvType == EventType::standardEvent)
			{
				auto eventData = reinterpret_cast<eventTrigger*>(vTab);
				if (funk != nullptr) {
					tryLuaBasicEventFunk((*funk)(eventData))
				}
				return 1;
			}
			else if (EvType == EventType::attackResidenceEvent)
			{
				auto eventData = reinterpret_cast<eventTrigger*>(vTab);
				auto character = gameHelpers::getEventNamedCharacter(eventData);
				auto settlement = character->gen->besiegingSettlement;
				fortStruct* fort = nullptr;
				if (settlement)
				{
					fort = gameHelpers::getTileFort(gameHelpers::getTile(settlement->xCoord, settlement->yCoord));
					if (fort)
						settlement = nullptr;
				}
				if (funk != nullptr) {
					tryLuaBasicEventFunk((*funk)(eventData, settlement, fort))
				}
				return 2;
			}
			else
			{
				return 0;
			}
		}
	};

	std::unordered_map<DWORD, std::unique_ptr<EventBase>> events;
	template<EventType EvType> void addEvent(DWORD key, const char* name);
}

/*
*/
sol::function* onCharacterTurnStart = nullptr;
sol::function* onCapturedCharacterRansomed = nullptr;
sol::function* onCapturedCharacterReleased = nullptr;
sol::function* onPreBattleWithdrawal = nullptr;
sol::function* onPostBattle = nullptr;
sol::function* onAbandonShowMe = nullptr;
sol::function* onGameReloaded = nullptr;
sol::function* onCrusadeCalled = nullptr;
sol::function* onPopeAcceptsCrusadeTarget = nullptr;
sol::function* onPopeRejectsCrusadeTarget = nullptr;
sol::function* onCrusadeEnds = nullptr;
sol::function* onBattleAiCommenced = nullptr;
sol::function* onBattleDelayPhaseCommenced = nullptr;
sol::function* onBattleDeploymentPhaseCommenced = nullptr;
sol::function* onBattleConflictPhaseCommenced = nullptr;
sol::function* onBattlePlayerUnitAttacksEnemyUnit = nullptr;
sol::function* onBattlePlayerUnitGoesBerserk = nullptr;
sol::function* onBattlePlayerUnitRouts = nullptr;
sol::function* onBattlePlayerSiegeEngineDestroyed = nullptr;
sol::function* onBattleGatesAttackedByPlayerEngine = nullptr;
sol::function* onBattleEnemyUnitAttacksPlayerUnit = nullptr;
sol::function* onBattleEnemyUnitGoesBerserk = nullptr;
sol::function* onBattnemyUnitRouts = nullptr;
sol::function* onBattnemySiegeEngineDestroyed = nullptr;
sol::function* onBattleGatesAttackedByEnemyEngine = nullptr;
sol::function* onBattleWinningPlaza = nullptr;
sol::function* onBattleStopsWinningPlaza = nullptr;
sol::function* onBattleDominatingPlaza = nullptr;
sol::function* onBattngineUnmanned = nullptr;
sol::function* onBattlePlayerArmyHalfDestroyed = nullptr;
sol::function* onBattnemyArmyHalfDestroyed = nullptr;
sol::function* onBattlePlayerUnderAttackIdle = nullptr;
sol::function* onBattleWinningCombat = nullptr;
sol::function* onBattleArmyTired = nullptr;
sol::function* onBattleTideofBattle = nullptr;
sol::function* onBattleUnitGoesBerserk = nullptr;
sol::function* onBattleArmyRouted = nullptr;
sol::function* onBattleSiegeEngineDestroyed = nullptr;
sol::function* onBattleReinforcementsArrive = nullptr;
sol::function* onBattleSiegeEngineDocksWall = nullptr;
sol::function* onBattleGatesAttackedByEngine = nullptr;
sol::function* onBattleGatesDestroyedByEngine = nullptr;
sol::function* onBattleWallsBreachedByEngine = nullptr;
sol::function* onBattleWallsCaptured = nullptr;
sol::function* onBattleFinished = nullptr;
sol::function* onBattleArmyHalfDestroyed = nullptr;
sol::function* onBattleUnitRouts = nullptr;
sol::function* onBattleGeneralKilled = nullptr;
sol::function* onFactionTurnStart = nullptr;
sol::function* onFactionNewCapital = nullptr;
sol::function* onPreFactionTurnStart = nullptr;
sol::function* onFactionWarDeclared = nullptr;
sol::function* onFactionAllianceDeclared = nullptr;
sol::function* onInterFactionMarriage = nullptr;
sol::function* onFactionTradeAgreementMade = nullptr;
sol::function* onGiveMoney = nullptr;
sol::function* onPopeElected = nullptr;
sol::function* onVotedForPope = nullptr;
sol::function* onFactionExcommunicated = nullptr;
sol::function* onFactionBreakAlliance = nullptr;
sol::function* onHordeFormed = nullptr;
sol::function* onGeneralCaptureResidence = nullptr;
sol::function* onGeneralCaptureSettlement = nullptr;
sol::function* onLeaderDestroyedFaction = nullptr;
sol::function* onDisaster = nullptr;
sol::function* onEventCounter = nullptr;
sol::function* onGeneralAssaultsResidence = nullptr;
sol::function* onGeneralAssaultsGeneral = nullptr;
sol::function* onOfferedForAdoption = nullptr;
sol::function* onLesserGeneralOfferedForAdoption = nullptr;
sol::function* onOfferedForMarriage = nullptr;
sol::function* onBrotherAdopted = nullptr;
sol::function* onFatherDiesNatural = nullptr;
sol::function* onBecomesFactionLeader = nullptr;
sol::function* onBecomesFactionHeir = nullptr;
sol::function* onCeasedFactionLeader = nullptr;
sol::function* onCeasedFactionHeir = nullptr;
sol::function* onUngarrisonedFort = nullptr;
sol::function* onTileSeen = nullptr;
sol::function* onObjSeen = nullptr;
sol::function* onUnitsDesertCrusade = nullptr;
sol::function* onArmyTakesCrusadeTarget = nullptr;
sol::function* onTransgression = nullptr;
sol::function* onForgiveness = nullptr;
sol::function* onUpdateAttitude = nullptr;
sol::function* onDemeanour = nullptr;
sol::function* onBirth = nullptr;
sol::function* onCharacterComesOfAge = nullptr;
sol::function* onCharacterMarries = nullptr;
sol::function* onCharacterMarriesPrincess = nullptr;
sol::function* onMarriageAlliancePossible = nullptr;
sol::function* onMarriageAllianceOffered = nullptr;
sol::function* onPriestBecomesHeretic = nullptr;
sol::function* onCharacterNearHeretic = nullptr;
sol::function* onCharacterNearWitch = nullptr;
sol::function* onCardinalPromoted = nullptr;
sol::function* onCardinalRemoved = nullptr;
sol::function* onInquisitorAppointed = nullptr;
sol::function* onCharacterBecomesAFather = nullptr;
sol::function* onCharacterTurnEnd = nullptr;
sol::function* onCharacterTurnEndInSettlement = nullptr;
sol::function* onGeneralDevastatesTile = nullptr;
sol::function* onGeneralJoinCrusade = nullptr;
sol::function* onGeneralAbandonCrusade = nullptr;
sol::function* onGeneralArrivesCrusadeTargetRegion = nullptr;
sol::function* onGeneralTakesCrusadeTarget = nullptr;
sol::function* onCharacterAttacksCrusadingGeneral = nullptr;
sol::function* onGeneralPrisonersRansomedCaptor = nullptr;
sol::function* onGeneralPrisonersRansomedCaptive = nullptr;
sol::function* onFactionLeaderPrisonersRansomedCaptor = nullptr;
sol::function* onFactionLeaderPrisonersRansomedCaptive = nullptr;
sol::function* onSpyMission = nullptr;
sol::function* onExecutesASpyOnAMission = nullptr;
sol::function* onLeaderOrderedSpyingMission = nullptr;
sol::function* onAssassinationMission = nullptr;
sol::function* onExecutesAnAssassinOnAMission = nullptr;
sol::function* onAssassinCaughtAttackingPope = nullptr;
sol::function* onLeaderOrderedAssassination = nullptr;
sol::function* onSufferAssassinationAttempt = nullptr;
sol::function* onAcquisitionMission = nullptr;
sol::function* onSufferAcquisitionAttempt = nullptr;
sol::function* onMarriageMission = nullptr;
sol::function* onSufferMarriageAttempt = nullptr;
sol::function* onDenouncementMission = nullptr;
sol::function* onSufferDenouncementAttempt = nullptr;
sol::function* onSabotageMission = nullptr;
sol::function* onLeaderOrderedSabotage = nullptr;
sol::function* onBriberyMission = nullptr;
sol::function* onLeaderOrderedBribery = nullptr;
sol::function* onAcceptBribe = nullptr;
sol::function* onRefuseBribe = nullptr;
sol::function* onInsurrection = nullptr;
sol::function* onDiplomacyMission = nullptr;
sol::function* onLeaderOrderedDiplomacy = nullptr;
sol::function* onLeaderMissionSuccess = nullptr;
sol::function* onLeaderMissionFailed = nullptr;
sol::function* onSettlementTurnStart = nullptr;
sol::function* onNewAdmiralCreated = nullptr;
sol::function* onUnitTrained = nullptr;
sol::function* onGovernorUnitTrained = nullptr;
sol::function* onGovernorBuildingCompleted = nullptr;
sol::function* onAgentCreated = nullptr;
sol::function* onGovernorAgentCreated = nullptr;
sol::function* onBuildingDestroyed = nullptr;
sol::function* onAddedToBuildingQueue = nullptr;
sol::function* onAddedToTrainingQueue = nullptr;
sol::function* onGovernorBuildingDestroyed = nullptr;
sol::function* onCityRiots = nullptr;
sol::function* onGiveSettlement = nullptr;
sol::function* onGovernorCityRiots = nullptr;
sol::function* onCityRebels = nullptr;
sol::function* onGovernorCityRebels = nullptr;
sol::function* onGovernorThrowGames = nullptr;
sol::function* onGovernorThrowRaces = nullptr;
sol::function* onUngarrisonedSettlement = nullptr;
sol::function* onOccupySettlement = nullptr;
sol::function* onSackSettlement = nullptr;
sol::function* onExterminatePopulation = nullptr;
sol::function* onCitySacked = nullptr;
sol::function* onSettlementUpgraded = nullptr;
sol::function* onSettlementConverted = nullptr;
sol::function* onGuildUpgraded = nullptr;
sol::function* onGuildDestroyed = nullptr;
sol::function* onSiegeEquipmentCompleted = nullptr;
sol::function* onSettlementTurnEnd = nullptr;
sol::function* onBuildingCompleted = nullptr;
sol::function* onCharacterSelected = nullptr;
sol::function* onEnemyCharacterSelected = nullptr;
sol::function* onSettlementSelected = nullptr;
sol::function* onMultiTurnMove = nullptr;
sol::function* onSettlementPanelOpen = nullptr;
sol::function* onFinancesPanelOpen = nullptr;
sol::function* onFactionSummaryPanelOpen = nullptr;
sol::function* onFamilyTreePanelOpen = nullptr;
sol::function* onDiplomacyPanelOpen = nullptr;
sol::function* onPreBattlePanelOpen = nullptr;
sol::function* onRecruitmentPanelOpen = nullptr;
sol::function* onConstructionPanelOpen = nullptr;
sol::function* onTradePanelOpen = nullptr;
sol::function* onIncomingMessage = nullptr;
sol::function* onMessageOpen = nullptr;
sol::function* onMessageClosed = nullptr;
sol::function* onAdviceSupressed = nullptr;
sol::function* onSettlementScrollAdviceRequested = nullptr;
sol::function* onDeclineAutomatedSettlementManagement = nullptr;
sol::function* onCharacterPanelOpen = nullptr;
sol::function* onRequestBuildingAdvice = nullptr;
sol::function* onRequestTrainingAdvice = nullptr;
sol::function* onRequestMercenariesAdvice = nullptr;
sol::function* onButtonPressed = nullptr;
sol::function* onShortcutTriggered = nullptr;
sol::function* onUIElementVisible = nullptr;
sol::function* onScrollOpened = nullptr;
sol::function* onScrollClosed = nullptr;
sol::function* onScrollAdviceRequested = nullptr;
sol::function* onEscPressed = nullptr;
sol::function* onScriptedAdvice = nullptr;
sol::function* onNavalPreBattleScrollAdviceRequested = nullptr;
sol::function* onPreBattleScrollAdviceRequested = nullptr;
sol::function* onCollegeOfCardinalsPanelOpen = nullptr;
sol::function* onDiplomaticStandingPanelOpen = nullptr;
sol::function* onBattleSpySuccess = nullptr;
