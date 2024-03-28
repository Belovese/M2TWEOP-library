///
//![Lua logo](_static/Lua.png)
//M2TWEOP structures and functions. There are not many examples and descriptions here. Also note that the examples do not include many of the checks that would be required when creating modifications.
//@module LuaPlugin
//@author youneuoy
//@license GPL-3.0
#include "luaP.h"
#include "plugData.h"
void luaP::onPluginLoadF()
{
	///EOP Events
	//@section eventsList

	/***
	Events functions list.
	Just list, use it without EventsFunctionsList.!!!

	@tfield onCharacterTurnStart onCharacterTurnStart
	@tfield onCapturedCharacterRansomed onCapturedCharacterRansomed
	@tfield onCapturedCharacterReleased onCapturedCharacterReleased
	@tfield onFatherDiesNatural onFatherDiesNatural
	@tfield onPreBattleWithdrawal onPreBattleWithdrawal
	@tfield onPostBattle onPostBattle
	@tfield onHireMercenaries onHireMercenaries
	@tfield onGeneralCaptureResidence onGeneralCaptureResidence
	@tfield onLeaderDestroyedFaction onLeaderDestroyedFaction
	@tfield onOfferedForAdoption onOfferedForAdoption
	@tfield onLesserGeneralOfferedForAdoption onLesserGeneralOfferedForAdoption
	@tfield onOfferedForMarriage onOfferedForMarriage
	@tfield onBrotherAdopted onBrotherAdopted
	@tfield onBirth onBirth
	@tfield onCharacterComesOfAge onCharacterComesOfAge
	@tfield onCharacterMarries onCharacterMarries
	@tfield onCharacterMarriesPrincess onCharacterMarriesPrincess
	@tfield onMarriageAlliancePossible onMarriageAlliancePossible
	@tfield onMarriageAllianceOffered onMarriageAllianceOffered
	@tfield onPriestBecomesHeretic onPriestBecomesHeretic
	@tfield onCharacterNearHeretic onCharacterNearHeretic
	@tfield onCharacterNearWitch onCharacterNearWitch
	@tfield onCardinalPromoted onCardinalPromoted
	@tfield onCharacterBecomesAFather onCharacterBecomesAFather
	@tfield onGeneralDevastatesTile onGeneralDevastatesTile
	@tfield onExecutesASpyOnAMission onExecutesASpyOnAMission
	@tfield onExecutesAnAssassinOnAMission onExecutesAnAssassinOnAMission
	@tfield onSufferAssassinationAttempt onSufferAssassinationAttempt
	@tfield onSufferAcquisitionAttempt onSufferAcquisitionAttempt
	@tfield onSufferMarriageAttempt onSufferMarriageAttempt
	@tfield onSufferDenouncementAttempt onSufferDenouncementAttempt
	@tfield onLeaderOrderedSabotage onLeaderOrderedSabotage
	@tfield onAcceptBribe onAcceptBribe
	@tfield onRefuseBribe onRefuseBribe
	@tfield onInsurrection onInsurrection
	@tfield onLeaderOrderedDiplomacy onLeaderOrderedDiplomacy
	@tfield onNewAdmiralCreated onNewAdmiralCreated
	@tfield onGovernorBuildingDestroyed onGovernorBuildingDestroyed
	@tfield onGovernorThrowGames onGovernorThrowGames
	@tfield onGovernorThrowRaces onGovernorThrowRaces
	@tfield onCharacterSelected onCharacterSelected
	@tfield onEnemyCharacterSelected onEnemyCharacterSelected
	@tfield onMultiTurnMove onMultiTurnMove
	@tfield onCharacterPanelOpen onCharacterPanelOpen
	@tfield onLeaderMissionSuccess onLeaderMissionSuccess
	@tfield onLeaderMissionFailed onLeaderMissionFailed
	@tfield onGeneralJoinCrusade onGeneralJoinCrusade
	@tfield onGeneralAbandonCrusade onGeneralAbandonCrusade
	@tfield onGeneralArrivesCrusadeTargetRegion onGeneralArrivesCrusadeTargetRegion
	@tfield onGeneralTakesCrusadeTarget onGeneralTakesCrusadeTarget
	@tfield onCharacterTurnEnd onCharacterTurnEnd
	@tfield onCharacterTurnEndInSettlement onCharacterTurnEndInSettlement
	@tfield onBecomesFactionLeader onBecomesFactionLeader
	@tfield onCeasedFactionLeader onCeasedFactionLeader
	@tfield onBecomesFactionHeir onBecomesFactionHeir
	@tfield onCeasedFactionHeir onCeasedFactionHeir
	@tfield onCharacterDamagedByDisaster onCharacterDamagedByDisaster
	@tfield onGeneralCaptureSettlement onGeneralCaptureSettlement
	@tfield onGeneralAssaultsResidence onGeneralAssaultsResidence
	@tfield onGeneralAssaultsGeneral onGeneralAssaultsGeneral
	@tfield onCharacterAttacksCrusadingGeneral onCharacterAttacksCrusadingGeneral
	@tfield onGeneralPrisonersRansomedCaptor onGeneralPrisonersRansomedCaptor
	@tfield onGeneralPrisonersRansomedCaptive onGeneralPrisonersRansomedCaptive
	@tfield onFactionLeaderPrisonersRansomedCaptor onFactionLeaderPrisonersRansomedCaptor
	@tfield onFactionLeaderPrisonersRansomedCaptive onFactionLeaderPrisonersRansomedCaptive
	@tfield onSpyMission onSpyMission
	@tfield onAssassinationMission onAssassinationMission
	@tfield onAcquisitionMission onAcquisitionMission
	@tfield onMarriageMission onMarriageMission
	@tfield onDenouncementMission onDenouncementMission
	@tfield onSabotageMission onSabotageMission
	@tfield onBriberyMission onBriberyMission
	@tfield onDiplomacyMission onDiplomacyMission
	@tfield onLeaderOrderedSpyingMission onLeaderOrderedSpyingMission
	@tfield onLeaderOrderedAssassination onLeaderOrderedAssassination
	@tfield onLeaderOrderedBribery onLeaderOrderedBribery
	@tfield onSettlementTurnStart onSettlementTurnStart
	@tfield onUngarrisonedSettlement onUngarrisonedSettlement
	@tfield onSettlementUpgraded onSettlementUpgraded
	@tfield onSettlementConverted onSettlementConverted
	@tfield onSiegeEquipmentCompleted onSiegeEquipmentCompleted
	@tfield onSettlementTurnEnd onSettlementTurnEnd
	@tfield onSettlementSelected onSettlementSelected
	@tfield onSettlementPanelOpen onSettlementPanelOpen
	@tfield onRecruitmentPanelOpen onRecruitmentPanelOpen
	@tfield onConstructionPanelOpen onConstructionPanelOpen
	@tfield onTradePanelOpen onTradePanelOpen
	@tfield onSettlementScrollAdviceRequested onSettlementScrollAdviceRequested
	@tfield onGuildUpgraded onGuildUpgraded
	@tfield onGuildDestroyed onGuildDestroyed
	@tfield onOccupySettlement onOccupySettlement
	@tfield onSackSettlement onSackSettlement
	@tfield onExterminatePopulation onExterminatePopulation
	@tfield onCityRiots onCityRiots
	@tfield onGiveSettlement onGiveSettlement
	@tfield onCityRebels onCityRebels
	@tfield onCitySacked onCitySacked
	@tfield onGovernorCityRiots onGovernorCityRiots
	@tfield onGovernorCityRebels onGovernorCityRebels
	@tfield onAbandonShowMe onAbandonShowMe
	@tfield onGameReloaded onGameReloaded
	@tfield onBattleWinningPlaza onBattleWinningPlaza
	@tfield onBattleStopsWinningPlaza onBattleStopsWinningPlaza
	@tfield onBattleDominatingPlaza onBattleDominatingPlaza
	@tfield onBattngineUnmanned onBattngineUnmanned
	@tfield onBattlePlayerArmyHalfDestroyed onBattlePlayerArmyHalfDestroyed
	@tfield onBattnemyArmyHalfDestroyed onBattnemyArmyHalfDestroyed
	@tfield onBattleFinished onBattleFinished
	@tfield onBattleArmyHalfDestroyed onBattleArmyHalfDestroyed
	@tfield onEscPressed onEscPressed
	@tfield onScriptedAdvice onScriptedAdvice
	@tfield onNavalPreBattleScrollAdviceRequested onNavalPreBattleScrollAdviceRequested
	@tfield onPreBattleScrollAdviceRequested onPreBattleScrollAdviceRequested
	@tfield onCollegeOfCardinalsPanelOpen onCollegeOfCardinalsPanelOpen
	@tfield onDiplomaticStandingPanelOpen onDiplomaticStandingPanelOpen
	@tfield onBattlePlayerUnderAttackIdle onBattlePlayerUnderAttackIdle
	@tfield onBattleWinningCombat onBattleWinningCombat
	@tfield onBattleArmyTired onBattleArmyTired
	@tfield onBattleSpySuccess onBattleSpySuccess
	@tfield onBattleTideofBattle onBattleTideofBattle
	@tfield onBattleUnitGoesBerserk onBattleUnitGoesBerserk
	@tfield onBattleSiegeEngineDestroyed onBattleSiegeEngineDestroyed
	@tfield onBattleSiegeEngineDocksWall onBattleSiegeEngineDocksWall
	@tfield onBattleGatesAttackedByEngine onBattleGatesAttackedByEngine
	@tfield onBattleGatesDestroyedByEngine onBattleGatesDestroyedByEngine
	@tfield onBattleWallsBreachedByEngine onBattleWallsBreachedByEngine
	@tfield onBattleWallsCaptured onBattleWallsCaptured
	@tfield onBattleUnitRouts onBattleUnitRouts
	@tfield onUnitDisbanded onUnitDisbanded
	@tfield onUnitTrained onUnitTrained
	@tfield onGovernorUnitTrained onGovernorUnitTrained
	@tfield onGovernorBuildingCompleted onGovernorBuildingCompleted
	@tfield onAgentCreated onAgentCreated
	@tfield onGovernorAgentCreated onGovernorAgentCreated
	@tfield onBuildingDestroyed onBuildingDestroyed
	@tfield onAddedToBuildingQueue onAddedToBuildingQueue
	@tfield onBuildingCompleted onBuildingCompleted
	@tfield onRequestBuildingAdvice onRequestBuildingAdvice
	@tfield onRequestTrainingAdvice onRequestTrainingAdvice
	@tfield onAddedToTrainingQueue onAddedToTrainingQueue
	@tfield onBattleArmyRouted onBattleArmyRouted
	@tfield onBattleReinforcementsArrive onBattleReinforcementsArrive
	@tfield onRequestMercenariesAdvice onRequestMercenariesAdvice
	@tfield onButtonPressed onButtonPressed
	@tfield onShortcutTriggered onShortcutTriggered
	@tfield onUIElementVisible onUIElementVisible
	@tfield onScrollOpened onScrollOpened
	@tfield onScrollClosed onScrollClosed
	@tfield onScrollAdviceRequested onScrollAdviceRequested
	@tfield onAdviceSupressed onAdviceSupressed
	@tfield onBattleGeneralRouted onBattleGeneralRouted
	@tfield onBattleGeneralKilled onBattleGeneralKilled
	@tfield onCrusadeCalled onCrusadeCalled
	@tfield onPopeAcceptsCrusadeTarget onPopeAcceptsCrusadeTarget
	@tfield onPopeRejectsCrusadeTarget onPopeRejectsCrusadeTarget
	@tfield onCrusadeEnds onCrusadeEnds
	@tfield onPreFactionTurnStart onPreFactionTurnStart
	@tfield onFactionTurnStart onFactionTurnStart
	@tfield onFactionNewCapital onFactionNewCapital
	@tfield onFactionTurnEnd onFactionTurnEnd
	@tfield onVotedForPope onVotedForPope
	@tfield onFactionExcommunicated onFactionExcommunicated
	@tfield onHordeFormed onHordeFormed
	@tfield onCardinalRemoved onCardinalRemoved
	@tfield onInquisitorAppointed onInquisitorAppointed
	@tfield onAssassinCaughtAttackingPope onAssassinCaughtAttackingPope
	@tfield onFinancesPanelOpen onFinancesPanelOpen
	@tfield onFactionSummaryPanelOpen onFactionSummaryPanelOpen
	@tfield onFamilyTreePanelOpen onFamilyTreePanelOpen
	@tfield onDiplomacyPanelOpen onDiplomacyPanelOpen
	@tfield onPreBattlePanelOpen onPreBattlePanelOpen
	@tfield onIncomingMessage onIncomingMessage
	@tfield onMessageOpen onMessageOpen
	@tfield onMessageClosed onMessageClosed
	@tfield onDeclineAutomatedSettlementManagement onDeclineAutomatedSettlementManagement
	@tfield onBattleAiCommenced onBattleAiCommenced
	@tfield onBattleDelayPhaseCommenced onBattleDelayPhaseCommenced
	@tfield onBattleDeploymentPhaseCommenced onBattleDeploymentPhaseCommenced
	@tfield onBattleConflictPhaseCommenced onBattleConflictPhaseCommenced
	@tfield onFactionWarDeclared onFactionWarDeclared
	@tfield onFactionAllianceDeclared onFactionAllianceDeclared
	@tfield onInterFactionMarriage onInterFactionMarriage
	@tfield onFactionTradeAgreementMade onFactionTradeAgreementMade
	@tfield onPopeElected onPopeElected
	@tfield onFactionBreakAlliance onFactionBreakAlliance
	@tfield onUpdateAttitude onUpdateAttitude
	@tfield onDemeanour onDemeanour
	@tfield onGiveMoney onGiveMoney
	@tfield onBattlePlayerUnitAttacksEnemyUnit onBattlePlayerUnitAttacksEnemyUnit
	@tfield onBattleEnemyUnitAttacksPlayerUnit onBattleEnemyUnitAttacksPlayerUnit
	@tfield onBattlePlayerUnitGoesBerserk onBattlePlayerUnitGoesBerserk
	@tfield onBattlePlayerUnitRouts onBattlePlayerUnitRouts
	@tfield onBattlePlayerSiegeEngineDestroyed onBattlePlayerSiegeEngineDestroyed
	@tfield onBattleGatesAttackedByPlayerEngine onBattleGatesAttackedByPlayerEngine
	@tfield onBattleEnemyUnitGoesBerserk onBattleEnemyUnitGoesBerserk
	@tfield onBattnemyUnitRouts onBattnemyUnitRouts
	@tfield onBattnemySiegeEngineDestroyed onBattnemySiegeEngineDestroyed
	@tfield onBattleGatesAttackedByEnemyEngine onBattleGatesAttackedByEnemyEngine
	@tfield onDisaster onDisaster
	@tfield onEventCounter onEventCounter
	@tfield onUngarrisonedFort onUngarrisonedFort
	@tfield onObjSeen onObjSeen
	@tfield onTileSeen onTileSeen
	@tfield onTransgression onTransgression
	@tfield onForgiveness onForgiveness
	@tfield onArmyTakesCrusadeTarget onArmyTakesCrusadeTarget
	@tfield onUnitsDesertCrusade onUnitsDesertCrusade
	@tfield draw draw
	@tfield onLoadingFonts onLoadingFonts
	@tfield onNewGameStart onNewGameStart
	@tfield onReadGameDbsAtStart onReadGameDbsAtStart
	@tfield onGameInit onGameInit
	@tfield onAiTurn onAiTurn
	@tfield onClickAtTile onClickAtTile
	@tfield onCampaignMapLoaded onCampaignMapLoaded
	@tfield onCreateSaveFile onCreateSaveFile
	@tfield onLoadSaveFile onLoadSaveFile
	@tfield onChangeTurnNum onChangeTurnNum
	@tfield onSelectWorldpkgdesc onSelectWorldpkgdesc
	@tfield onfortificationlevelS onfortificationlevelS
	@tfield onEndSiege onEndSiege
	@tfield onStartSiege onStartSiege
	@tfield onPluginLoad onPluginLoad




	@table EventsFunctionsList
	*/


	/***
	Called at a character's turn start.
	Exports: character, faction, regionID, characterType, religion

	@function onCharacterTurnStart
	@tparam eventTrigger eventData

	@usage
	function onCharacterTurnStart(eventData)
	--something here
	end
	*/

	/***
	A captured character has been successfully ransomed back from the enemy.
	Exports: character, faction, regionID, characterType, religion

	@function onCapturedCharacterRansomed
	@tparam eventTrigger eventData

	@usage
	function onCapturedCharacterRansomed(eventData)
	--something here
	end
	*/

	/***
	A captured character has been released by the enemy.
	Exports: character, faction, regionID, characterType, religion

	@function onCapturedCharacterReleased
	@tparam eventTrigger eventData

	@usage
	function onCapturedCharacterReleased(eventData)
	--something here
	end
	*/

	/***
	A character father died of natural causes.
	Exports: character, faction, regionID, characterType, religion

	@function onFatherDiesNatural
	@tparam eventTrigger eventData

	@usage
	function onFatherDiesNatural(eventData)
	--something here
	end
	*/

	/***
	When a battle is about to start but one of the armies withdraws.
	Exports: character, faction, regionID, characterType, religion

	@function onPreBattleWithdrawal
	@tparam eventTrigger eventData

	@usage
	function onPreBattleWithdrawal(eventData)
	--something here
	end
	*/

	/***
	When a battle has finished.
	Exports: character, faction, regionID, characterType, religion

	@function onPostBattle
	@tparam eventTrigger eventData

	@usage
	function onPostBattle(eventData)
	--something here
	end
	*/

	/***
	A General has hired some mercenaries.
	Exports: character, faction, regionID, characterType, religion

	@function onHireMercenaries
	@tparam eventTrigger eventData

	@usage
	function onHireMercenaries(eventData)
	--something here
	end
	*/

	/***
	A General has captured a residence such as a fort or watchtower.
	Exports: character, faction, regionID, characterType, religion

	@function onGeneralCaptureResidence
	@tparam eventTrigger eventData

	@usage
	function onGeneralCaptureResidence(eventData)
	--something
	end
	*/

	/***
	A faction has been destroyed.
	Exports: character, faction, regionID, characterType, religion

	@function onLeaderDestroyedFaction
	@tparam eventTrigger eventData

	@usage
	function onLeaderDestroyedFaction(eventData)
	--something
	end
	*/

	/***
	An adoption has been proposed.
	Exports: character, faction, regionID, characterType, religion

	@function onOfferedForAdoption
	@tparam eventTrigger eventData

	@usage
	function onOfferedForAdoption(eventData)
	--something
	end
	*/

	/***
	A lesser general adoption has been proposed (man of the hour event).
	Exports: character, faction, regionID, characterType, religion

	@function onLesserGeneralOfferedForAdoption
	@tparam eventTrigger eventData

	@usage
	function onLesserGeneralOfferedForAdoption(eventData)
	--something
	end
	*/

	/***
	A marriage offer has been proposed.
	Exports: character, faction, regionID, characterType, religion

	@function onOfferedForMarriage
	@tparam eventTrigger eventData

	@usage
	function onOfferedForMarriage(eventData)
	--something
	end
	*/

	/***
	A brother has been adopted.
	Exports: character, faction, regionID, characterType, religion

	@function onBrotherAdopted
	@tparam eventTrigger eventData

	@usage
	function onBrotherAdopted(eventData)
	--something
	end
	*/

	/***
	A child is born to the faction leader.
	Exports: character, faction, regionID, characterType, religion

	@function onBirth
	@tparam eventTrigger eventData

	@usage
	function onBirth(eventData)
	--something
	end
	*/

	/***
	A character has come of age.
	Exports: character, faction, regionID, characterType, religion

	@function onCharacterComesOfAge
	@tparam eventTrigger eventData

	@usage
	function onCharacterComesOfAge(eventData)
	--something
	end
	*/

	/***
	A character has married.
	Exports: character, faction, regionID, characterType, religion

	@function onCharacterMarries
	@tparam eventTrigger eventData

	@usage
	function onCharacterMarries(eventData)
	--something
	end
	*/

	/***
	A character has married a princess.
	Exports: character, faction, regionID, characterType, religion

	@function onCharacterMarriesPrincess
	@tparam eventTrigger eventData

	@usage
	function onCharacterMarriesPrincess(eventData)
	--something
	end
	*/

	/***
	A marriage alliance is possible.
	Exports: character, faction, regionID, characterType, religion

	@function onMarriageAlliancePossible
	@tparam eventTrigger eventData

	@usage
	function onMarriageAlliancePossible(eventData)
	--something
	end
	*/

	/***
	A marriage alliance has been offered.
	Exports: character, faction, regionID, characterType, religion

	@function onMarriageAllianceOffered
	@tparam eventTrigger eventData

	@usage
	function onMarriageAllianceOffered(eventData)
	--something
	end
	*/

	/***
	A priest has gone mad.
	Exports: character, faction, regionID, characterType, religion

	@function onPriestBecomesHeretic
	@tparam eventTrigger eventData

	@usage
	function onPriestBecomesHeretic(eventData)
	--something
	end
	*/

	/***
	A character is adjacent to a heretic.
	Exports: character, faction, regionID, characterType, religion

	@function onCharacterNearHeretic
	@tparam eventTrigger eventData

	@usage
	function onCharacterNearHeretic(eventData)
	--something
	end
	*/

	/***
	A character is adjacent to a witch.
	Exports: character, faction, regionID, characterType, religion

	@function onCharacterNearWitch
	@tparam eventTrigger eventData

	@usage
	function onCharacterNearWitch(eventData)
	--something
	end
	*/

	/***
	A character has been promoted to a cardinal.
	Exports: character, faction, regionID, characterType, religion

	@function onCardinalPromoted
	@tparam eventTrigger eventData

	@usage
	function onCardinalPromoted(eventData)
	--something
	end
	*/

	/***
	A character has become a father.
	Exports: character, faction, regionID, characterType, religion

	@function onCharacterBecomesAFather
	@tparam eventTrigger eventData

	@usage
	function onCharacterBecomesAFather(eventData)
	--something
	end
	*/

	/***
	A General and his army has devastated an enemy's fertile land.
	Exports: character, faction, regionID, characterType, religion

	@function onGeneralDevastatesTile
	@tparam eventTrigger eventData

	@usage
	function onGeneralDevastatesTile(eventData)
	--something
	end
	*/

	/***
	A spying mission has failed and the spy is executed by the target.
	Exports: character, faction, regionID, characterType, religion

	@function onExecutesASpyOnAMission
	@tparam eventTrigger eventData

	@usage
	function onExecutesASpyOnAMission(eventData)
	--something
	end
	*/

	/***
	An assassination mission has failed and the assassin is executed by the target.
	Exports: character, faction, regionID, characterType, religion

	@function onExecutesAnAssassinOnAMission
	@tparam eventTrigger eventData

	@usage
	function onExecutesAnAssassinOnAMission(eventData)
	--something
	end
	*/

	/***
	Someone has had an attempt on their life.
	Exports: character, faction, regionID, characterType, religion

	@function onSufferAssassinationAttempt
	@tparam eventTrigger eventData

	@usage
	function onSufferAssassinationAttempt(eventData)
	--something
	end
	*/

	/***
	Someone has had an attempt on their assets.
	Exports: character, faction, regionID, characterType, religion

	@function onSufferAcquisitionAttempt
	@tparam eventTrigger eventData

	@usage
	function onSufferAcquisitionAttempt(eventData)
	--something
	end
	*/

	/***
	Someone has had an attempt on their bachelorhood.
	Exports: character, faction, regionID, characterType, religion

	@function onSufferMarriageAttempt
	@tparam eventTrigger eventData

	@usage
	function onSufferMarriageAttempt(eventData)
	--something
	end
	*/

	/***
	Someone has had a denouncement attempt.
	Exports: character, faction, regionID, characterType, religion

	@function onSufferDenouncementAttempt
	@tparam eventTrigger eventData

	@usage
	function onSufferDenouncementAttempt(eventData)
	--something
	end
	*/

	/***
	A Faction leader has ordered a sabotage mission.
	Exports: character, faction, regionID, characterType, religion

	@function onLeaderOrderedSabotage
	@tparam eventTrigger eventData

	@usage
	function onLeaderOrderedSabotage(eventData)
	--something
	end
	*/

	/***
	Someone has been bribed.
	Exports: character, faction, regionID, characterType, religion

	@function onAcceptBribe
	@tparam eventTrigger eventData

	@usage
	function onAcceptBribe(eventData)
	--something
	end
	*/

	/***
	Someone has refused a bribe.
	Exports: character, faction, regionID, characterType, religion

	@function onRefuseBribe
	@tparam eventTrigger eventData

	@usage
	function onRefuseBribe(eventData)
	--something
	end
	*/

	/***
	Insurgence has been provoked.
	Exports: character, faction, regionID, characterType, religion

	@function onInsurrection
	@tparam eventTrigger eventData

	@usage
	function onInsurrection(eventData)
	--something
	end
	*/

	/***
	A Faction leader has ordered a diplomacy mission.
	Exports: character, faction, regionID, characterType, religion

	@function onLeaderOrderedDiplomacy
	@tparam eventTrigger eventData

	@usage
	function onLeaderOrderedDiplomacy(eventData)
	--something
	end
	*/

	/***
	A new admiral has been created for a new ship.
	Exports: stratCharacter, character, faction, regionID, characterType, religion

	@function onNewAdmiralCreated
	@tparam eventTrigger eventData

	@usage
	function onNewAdmiralCreated(eventData)
	--something
	end
	*/

	/***
	A building has been destroyed.
	Exports: character, faction, regionID, characterType, religion

	@function onGovernorBuildingDestroyed
	@tparam eventTrigger eventData

	@usage
	function onGovernorBuildingDestroyed(eventData)
	--something
	end
	*/

	/***
	Games have been thrown.
	Exports: character, faction, regionID, characterType, religion

	@function onGovernorThrowGames
	@tparam eventTrigger eventData

	@usage
	function onGovernorThrowGames(eventData)
	--something
	end
	*/

	/***
	Races have been thrown.
	Exports: character, faction, regionID, characterType, religion

	@function onGovernorThrowRaces
	@tparam eventTrigger eventData

	@usage
	function onGovernorThrowRaces(eventData)
	--something
	end
	*/

	/***
	The player has selected a character.
	Exports: character, targetSettlement, faction, regionID, characterType, religion

	@function onCharacterSelected
	@tparam eventTrigger eventData

	@usage
	function onCharacterSelected(eventData)
	--something
	end
	*/

	/***
	The player has selected an enemy character.
	Exports: character, faction, regionID, characterType, religion

	@function onEnemyCharacterSelected
	@tparam eventTrigger eventData

	@usage
	function onEnemyCharacterSelected(eventData)
	--something
	end
	*/

	/***
	The player has selected a position beyond the character's extents.
	Exports: character, faction, regionID, characterType, religion

	@function onMultiTurnMove
	@tparam eventTrigger eventData

	@usage
	function onMultiTurnMove(eventData)
	--something
	end
	*/

	/***
	The player has opened the panel for the selected character.
	Exports: character, faction, regionID, characterType, religion

	@function onCharacterPanelOpen
	@tparam eventTrigger eventData

	@usage
	function onCharacterPanelOpen(eventData)
	--something
	end
	*/

	/***
	A mission has been completed.
	Exports: character, faction, regionID, characterType, missionDetails, religion

	@function onLeaderMissionSuccess
	@tparam eventTrigger eventData

	@usage
	function onLeaderMissionSuccess(eventData)
	--something
	end
	*/

	/***
	A mission has failed.
	Exports: character, faction, regionID, characterType, missionDetails, religion

	@function onLeaderMissionFailed
	@tparam eventTrigger eventData

	@usage
	function onLeaderMissionFailed(eventData)
	--something
	end
	*/

	/***
	A General has been sent on Crusade/Jihad.
	Exports: character, targetSettlement, faction, targetFaction, regionID, targetRegionID, characterType, religion, targetReligion, crusade

	@function onGeneralJoinCrusade
	@tparam eventTrigger eventData

	@usage
	function onGeneralJoinCrusade(eventData)
	--something
	end
	*/

	/***
	A General has left a Crusade/Jihad.
	Exports: character, targetSettlement, faction, targetFaction, regionID, targetRegionID, characterType, religion, targetReligion, crusade

	@function onGeneralAbandonCrusade
	@tparam eventTrigger eventData

	@usage
	function onGeneralAbandonCrusade(eventData)
	--something
	end
	*/

	/***
	A General has arrived in the Crusade/Jihad target region.
	Exports: character, targetSettlement, faction, targetFaction, army, regionID, targetRegionID, characterType, religion, targetReligion, crusade

	@function onGeneralArrivesCrusadeTargetRegion
	@tparam eventTrigger eventData

	@usage
	function onGeneralArrivesCrusadeTargetRegion(eventData)
	--something
	end
	*/

	/***
	A General has taken the Crusade/Jihad target settlement.
	Exports: character, targetSettlement, faction, targetFaction, regionID, targetRegionID, characterType, religion, targetReligion, crusade

	@function onGeneralTakesCrusadeTarget
	@tparam eventTrigger eventData

	@usage
	function onGeneralTakesCrusadeTarget(eventData)
	--something
	end
	*/

	/***
	A Character has finished its turn.
	Exports: character, settlement, faction, regionID, characterType, religion

	@function onCharacterTurnEnd
	@tparam eventTrigger eventData

	@usage
	function onCharacterTurnEnd(eventData)
	--something
	end
	*/

	/***
	A Character has finished its turn in a settlement.
	Exports: character, settlement, faction, regionID, characterType, religion

	@function onCharacterTurnEndInSettlement
	@tparam eventTrigger eventData

	@usage
	function onCharacterTurnEndInSettlement(eventData)
	--something
	end
	*/

	/***
	The character has been made the faction leader.
	Exports: character, faction, regionID, characterType, religion

	@function onBecomesFactionLeader
	@tparam eventTrigger eventData

	@usage
	function onBecomesFactionLeader(eventData)
		print("Function: onBecomesFactionLeader()\n\tLeader: ".. eventData.character.fullName)
	end
	*/

	/***
	The character is no longer faction leader.
	Exports: character, faction, regionID, characterType, religion

	@function onCeasedFactionLeader
	@tparam eventTrigger eventData

	@usage
	function onCeasedFactionLeader(eventData)
		--something
	end
	*/

	/***
	The character has been made a faction heir.
	Exports: character, faction, regionID, characterType, religion

	@function onBecomesFactionHeir
	@tparam eventTrigger eventData

	@usage
	function onBecomesFactionHeir(eventData)
		print("Function: onBecomesFactionHeir()\n\tHeir: ".. eventData.character.fullName)
	end
	*/

	/***
	The character is no longer faction heir.
	Exports: character, faction, regionID, characterType, religion

	@function onCeasedFactionHeir
	@tparam eventTrigger eventData

	@usage
	function onCeasedFactionHeir(eventData)
		print("Function: onCeasedFactionHeir()\n\tHeir: "..  eventData.character.fullName)
	end
	*/

	/***
	A character has been injured by a disaster.
	Exports: character, faction, regionID, disasterType, characterType, religion
	disasterTypes = earthquake, flood, horde, storm, volcano, dustbowl, locusts, famine, plague, riot, fire

	@function onCharacterDamagedByDisaster
	@tparam eventTrigger eventData

	@usage
	function onCharacterDamagedByDisaster(eventData)
	--something
	end
	*/

	/***
	A General has captured a settlement.
	Exports: character, settlement, targetSettlement, faction, targetFaction, regionID, characterType, religion

	@function onGeneralCaptureSettlement
	@tparam eventTrigger eventData

	@usage
	function onGeneralCaptureSettlement(eventData)
	--something
	end
	*/

	/***
	An assault has taken place. NOTE: settlement and fort are not in eventData! They are separate arguments!.
	Exports: character, faction, targetFaction, regionID, targetRegionID, characterType, religion, targetReligion

	@function onGeneralAssaultsResidence
	@tparam eventTrigger eventData
	@tparam settlementStruct|nil settlement
	@tparam fortStruct|nil fort

	@usage
	function onGeneralAssaultsResidence(eventData, settlement, fort)
	--something
	end
	*/

	/***
	An assault has taken place.
	Exports: character, targetCharacter, faction, targetFaction, regionID, characterType, targetCharacterType, religion, targetReligion

	@function onGeneralAssaultsGeneral
	@tparam eventTrigger eventData

	@usage
	function onGeneralAssaultsGeneral(eventData)
	--something
	end
	*/

	/***
	A general on crusade/jihad has been attacked by other character (it includes crusading generals attacked in a residence or on navy and generals attacked by spotted and killed assassin).
	Exports: character, targetCharacter, faction, targetFaction, regionID, characterType, targetCharacterType, religion, targetReligion

	@function onCharacterAttacksCrusadingGeneral
	@tparam eventTrigger eventData

	@usage
	function onCharacterAttacksCrusadingGeneral(eventData)
	--something
	end
	*/

	/***
	A General of a captor faction has made a ransom decision.
	Exports: character, faction, targetFaction, regionID, characterType, religion, targetReligion, captureInfo, ransomType

	@function onGeneralPrisonersRansomedCaptor
	@tparam eventTrigger eventData

	@usage
	function onGeneralPrisonersRansomedCaptor(eventData)
	--something
	end
	*/

	/***
	A General of a captive faction has made a ransom decision.
	Exports: character, faction, targetFaction, regionID, characterType, religion, targetReligion, captureInfo, ransomType

	@function onGeneralPrisonersRansomedCaptive
	@tparam eventTrigger eventData

	@usage
	function onGeneralPrisonersRansomedCaptive(eventData)
	--something
	end
	*/

	/***
	A captor faction has made a ransom decision.
	Exports: character, faction, targetFaction, regionID, characterType, religion, targetReligion, captureInfo, ransomType

	@function onFactionLeaderPrisonersRansomedCaptor
	@tparam eventTrigger eventData

	@usage
	function onFactionLeaderPrisonersRansomedCaptor(eventData)
	--something
	end
	*/

	/***
	A captive faction has made a ransom decision.
	Exports: character, faction, targetFaction, regionID, characterType, religion, targetReligion, captureInfo, ransomType

	@function onFactionLeaderPrisonersRansomedCaptive
	@tparam eventTrigger eventData

	@usage
	function onFactionLeaderPrisonersRansomedCaptive(eventData)
	--something
	end
	*/

	/***
	A spy mission has completed. May also export fort or settlement if target was a garrison residence.
	Exports: character, settlement, fort, faction, targetFaction, regionID, characterType, missionSuccessLevel, missionProbability, religion, targetReligion

	@function onSpyMission
	@tparam eventTrigger eventData

	@usage
	function onSpyMission(eventData)
	--something
	end
	*/

	/***
	An assassination mission has completed.
	Exports: character, faction, regionID, characterType, missionSuccessLevel, missionProbability, religion

	@function onAssassinationMission
	@tparam eventTrigger eventData

	@usage
	function onAssassinationMission(eventData)
	--something
	end
	*/

	/***
	An acquisition mission has completed.
	Exports: character, faction, regionID, characterType, missionSuccessLevel, missionProbability, religion

	@function onAcquisitionMission
	@tparam eventTrigger eventData

	@usage
	function onAcquisitionMission(eventData)
	--something
	end
	*/

	/***
	A marriage mission has completed.
	Exports: character, faction, regionID, characterType, missionSuccessLevel, missionProbability, religion

	@function onMarriageMission
	@tparam eventTrigger eventData

	@usage
	function onMarriageMission(eventData)
	--something
	end
	*/

	/***
	A denouncement mission has completed.
	Exports: character, faction, regionID, characterType, missionSuccessLevel, missionProbability, religion

	@function onDenouncementMission
	@tparam eventTrigger eventData

	@usage
	function onDenouncementMission(eventData)
	--something
	end
	*/

	/***
	A sabotage mission has completed.
	Exports: character, faction, regionID, characterType, missionSuccessLevel, missionProbability, religion

	@function onSabotageMission
	@tparam eventTrigger eventData

	@usage
	function onSabotageMission(eventData)
	--something
	end
	*/

	/***
	A bribery mission has completed.
	Exports: character, faction, targetFaction, regionID, characterType, missionSuccessLevel, religion, targetReligion

	@function onBriberyMission
	@tparam eventTrigger eventData

	@usage
	function onBriberyMission(eventData)
	--something
	end
	*/

	/***
	A diplomacy mission has completed.
	Exports: character, faction, targetFaction, regionID, characterType, missionSuccessLevel, religion, targetReligion

	@function onDiplomacyMission
	@tparam eventTrigger eventData

	@usage
	function onDiplomacyMission(eventData)
	--something
	end
	*/

	/***
	A Faction leader has ordered a spying mission.
	Exports: character, settlement, fort, faction, targetFaction, regionID, characterType, missionSuccessLevel, religion, targetReligion

	@function onLeaderOrderedSpyingMission
	@tparam eventTrigger eventData

	@usage
	function onLeaderOrderedSpyingMission(eventData)
	--something
	end
	*/

	/***
	A Faction leader has ordered an assassination mission.
	Exports: character, targetCharacter, faction, regionID, characterType, missionSuccessLevel, religion

	@function onLeaderOrderedAssassination
	@tparam eventTrigger eventData

	@usage
	function onLeaderOrderedAssassination(eventData)
	--something
	end
	*/

	/***
	A Faction leader has ordered a bribery mission.
	Exports: character, faction, regionID, characterType, religion

	@function onLeaderOrderedBribery
	@tparam eventTrigger eventData

	@usage
	function onLeaderOrderedBribery(eventData)
	--something
	end
	*/

	/***
	A settlement is being processed for the start of its faction's turn.
	Exports: settlement, faction, regionID, religion

	@function onSettlementTurnStart
	@tparam eventTrigger eventData

	@usage
	function onSettlementTurnStart(eventData)
	--something
	end
	*/

	/***
	A settlement is no longer garrisoned.
	Exports: settlement, faction, regionID, religion

	@function onUngarrisonedSettlement
	@tparam eventTrigger eventData

	@usage
	function onUngarrisonedSettlement(eventData)
	--something
	end
	*/

	/***
	A settlement has been upgraded.
	Exports: settlement, faction, regionID, religion

	@function onSettlementUpgraded
	@tparam eventTrigger eventData

	@usage
	function onSettlementUpgraded(eventData)
	--something
	end
	*/

	/***
	A settlement has been converted.
	Exports: settlement, faction, regionID, religion

	@function onSettlementConverted
	@tparam eventTrigger eventData

	@usage
	function onSettlementConverted(eventData)
	--something
	end
	*/

	/***
	Siege equipment has been completed by one of the besieging armies.
	Exports: settlement, faction, regionID, religion

	@function onSiegeEquipmentCompleted
	@tparam eventTrigger eventData

	@usage
	function onSiegeEquipmentCompleted(eventData)
	--something
	end
	*/

	/***
	A Settlement is being processed for the end of its faction's turn.
	Exports: settlement, faction, regionID, religion

	@function onSettlementTurnEnd
	@tparam eventTrigger eventData

	@usage
	function onSettlementTurnEnd(eventData)
	--something
	end
	*/

	/***
	The player has selected a settlement.
	Exports: settlement, faction, regionID, religion

	@function onSettlementSelected
	@tparam eventTrigger eventData

	@usage
	function onSettlementSelected(eventData)
	--something
	end
	*/

	/***
	The player has opened the panel for the selected settlement.
	Exports: settlement, faction, regionID, religion

	@function onSettlementPanelOpen
	@tparam eventTrigger eventData

	@usage
	function onSettlementPanelOpen(eventData)
	--something
	end
	*/

	/***
	The player has opened a recruitment panel.
	Exports: settlement, faction, regionID, religion

	@function onRecruitmentPanelOpen
	@tparam eventTrigger eventData

	@usage
	function onRecruitmentPanelOpen(eventData)
	--something
	end
	*/

	/***
	The player has opened a construction panel.
	Exports: settlement, faction, regionID, religion

	@function onConstructionPanelOpen
	@tparam eventTrigger eventData

	@usage
	function onConstructionPanelOpen(eventData)
	--something
	end
	*/

	/***
	The player has opened a trade panel.
	Exports: settlement, faction, regionID, religion

	@function onTradePanelOpen
	@tparam eventTrigger eventData

	@usage
	function onTradePanelOpen(eventData)
	--something
	end
	*/

	/***
	The player has requested advice on the settlement scroll.
	Exports: settlement, faction, regionID, resourceDescription, religion

	@function onSettlementScrollAdviceRequested
	@tparam eventTrigger eventData

	@usage
	function onSettlementScrollAdviceRequested(eventData)
	--something
	end
	*/

	/***
	A guild has been created/upgraded.
	Exports: settlement, faction, regionID, resourceDescription, guild, religion

	@function onGuildUpgraded
	@tparam eventTrigger eventData

	@usage
	function onGuildUpgraded(eventData)
	--something
	end
	*/

	/***
	A guild has been destroyed.
	Exports: settlement, faction, regionID, guild, religion

	@function onGuildDestroyed
	@tparam eventTrigger eventData

	@usage
	function onGuildDestroyed(eventData)
	--something
	end
	*/

	/***
	A settlement has been captured and occupied.
	Exports: character, faction, targetFaction, regionID, characterType, religion, targetReligion

	@function onOccupySettlement
	@tparam eventTrigger eventData

	@usage
	function onOccupySettlement(eventData)
	--something
	end
	*/

	/***
	A settlement has been captured and sacked.
	Exports: character, faction, targetFaction, regionID, characterType, religion, targetReligion

	@function onSackSettlement
	@tparam eventTrigger eventData

	@usage
	function onSackSettlement(eventData)
	--something
	end
	*/

	/***
	A settlement has been captured and some of its population has been decimated.
	Exports: character, faction, targetFaction, regionID, characterType, religion, targetReligion

	@function onExterminatePopulation
	@tparam eventTrigger eventData
	
	@usage
	function onExterminatePopulation(eventData)
	--something
	end
	*/

	/***
	A settlement has rioted.
	Exports: settlement, faction, targetFaction, regionID, religion, targetReligion

	@function onCityRiots
	@tparam eventTrigger eventData

	@usage
	function onCityRiots(eventData)
	--something
	end
	*/

	/***
	A settlement has been given to another faction.
	Exports: settlement, faction, targetFaction, regionID, religion, targetReligion

	@function onGiveSettlement
	@tparam eventTrigger eventData

	@usage
	function onGiveSettlement(eventData)
	--something
	end
	*/

	/***
	A settlement has rebelled.
	Exports: settlement, faction, targetFaction, regionID, religion, targetReligion

	@function onCityRebels
	@tparam eventTrigger eventData

	@usage
	function onCityRebels(eventData)
	--something
	end
	*/

	/***
	A settlement has been razed.
	Exports: settlement, faction, targetFaction, regionID, religion, targetReligion

	@function onCitySacked
	@tparam eventTrigger eventData

	@usage
	function onCitySacked(eventData)
	--something
	end
	*/

	/***
	A settlement has rioted.
	Exports: character, settlement, faction, targetFaction, regionID, characterType, religion, targetReligion

	@function onGovernorCityRiots
	@tparam eventTrigger eventData

	@usage
	function onGovernorCityRiots(eventData)
	--something
	end
	*/

	/***
	A settlement has rebelled.
	Exports: character, settlement, faction, targetFaction, regionID, characterType, religion, targetReligion

	@function onGovernorCityRebels
	@tparam eventTrigger eventData

	@usage
	function onGovernorCityRebels(eventData)
	--something
	end
	*/

	/***
	The player has abandoned a show me scipt.

	@function onAbandonShowMe
	@tparam eventTrigger eventData

	@usage
	function onAbandonShowMe(eventData)
	--something here
	end
	*/

	/***
	A strat map game has been reloaded.

	@function onGameReloaded
	@tparam eventTrigger eventData

	@usage
	function onGameReloaded(eventData)
	--something here
	end
	*/

	/***
	The plaza is being captured.

	@function onBattleWinningPlaza
	@tparam eventTrigger eventData

	@usage
	function onBattleWinningPlaza(eventData)
	--something here
	end
	*/

	/***
	The plaza capture has been stopped.

	@function onBattleStopsWinningPlaza
	@tparam eventTrigger eventData

	@usage
	function onBattleStopsWinningPlaza(eventData)
	--something here
	end
	*/

	/***
	The enemy will have captured the plaza in 30s.

	@function onBattleDominatingPlaza
	@tparam eventTrigger eventData

	@usage
	function onBattleDominatingPlaza(eventData)
	--something here
	end
	*/

	/***
	A siege engine is now unmanned.

	@function onBattngineUnmanned
	@tparam eventTrigger eventData

	@usage
	function onBattngineUnmanned(eventData)
	--something here
	end
	*/

	/***
	Half of the player's army has been destroyed.

	@function onBattlePlayerArmyHalfDestroyed
	@tparam eventTrigger eventData

	@usage
	function onBattlePlayerArmyHalfDestroyed(eventData)
	--something here
	end
	*/

	/***
	Half of the enemy's army has been destroyed.

	@function onBattnemyArmyHalfDestroyed
	@tparam eventTrigger eventData

	@usage
	function onBattnemyArmyHalfDestroyed(eventData)
	--something here
	end
	*/

	/***
	The battle has finished.

	@function onBattleFinished
	@tparam eventTrigger eventData

	@usage
	function onBattleFinished(eventData)
	--something here
	end
	*/

	/***
	Half of an army has been destroyed.

	@function onBattleArmyHalfDestroyed
	@tparam eventTrigger eventData

	@usage
	function onBattleArmyHalfDestroyed(eventData)
	--something here
	end
	*/

	/***
	The escape key has been pressed. This trigger will only fire if the command StealEscKey has been used.

	@function onEscPressed
	@tparam eventTrigger eventData

	@usage
	function onEscPressed(eventData)
	--something here
	end
	*/

	/***
	The player has been issued with advice by a script.

	@function onScriptedAdvice
	@tparam eventTrigger eventData

	@usage
	function onScriptedAdvice(eventData)
	--something here
	end
	*/

	/***
	The player has requested advice on the naval prebattle scroll.

	@function onNavalPreBattleScrollAdviceRequested
	@tparam eventTrigger eventData

	@usage
	function onNavalPreBattleScrollAdviceRequested(eventData)
	--something here
	end
	*/

	/***
	The player has requested advice on the prebattle scroll.

	@function onPreBattleScrollAdviceRequested
	@tparam eventTrigger eventData

	@usage
	function onPreBattleScrollAdviceRequested(eventData)
	--something here
	end
	*/

	/***
	The player has opened the college of cardinals panel.

	@function onCollegeOfCardinalsPanelOpen
	@tparam eventTrigger eventData

	@usage
	function onCollegeOfCardinalsPanelOpen(eventData)
	--something here
	end
	*/

	/***
	The player has opened the diplomatic standing panel.

	@function onDiplomaticStandingPanelOpen
	@tparam eventTrigger eventData

	@usage
	function onDiplomaticStandingPanelOpen(eventData)
	--something here
	end
	*/

	/***
	An idle unit is under missile fire.

	@function onBattlePlayerUnderAttackIdle
	@tparam eventTrigger eventData

	@usage
	function onBattlePlayerUnderAttackIdle(eventData)
	--something here
	end
	*/

	/***
	A team has gained the advantage in combat.

	@function onBattleWinningCombat
	@tparam eventTrigger eventData

	@usage
	function onBattleWinningCombat(eventData)
	--something here
	end
	*/

	/***
	The whole army is tired.

	@function onBattleArmyTired
	@tparam eventTrigger eventData

	@usage
	function onBattleArmyTired(eventData)
	--something here
	end
	*/

	/***
	A spy has successfully opened the gates.

	@function onBattleSpySuccess
	@tparam eventTrigger eventData

	@usage
	function onBattleSpySuccess(eventData)
	--something here
	end
	*/

	/***
	A different team is now the strongest.

	@function onBattleTideofBattle
	@tparam eventTrigger eventData

	@usage
	function onBattleTideofBattle(eventData)
	--something here
	end
	*/

	/***
	A unit has gone berserk.
	Exports: unit

	@function onBattleUnitGoesBerserk
	@tparam eventTrigger eventData

	@usage
	function onBattleUnitGoesBerserk(eventData)
	--something here
	end
	*/

	/***
	A siege engine has been destroyed.
	Exports: unit

	@function onBattleSiegeEngineDestroyed
	@tparam eventTrigger eventData

	@usage
	function onBattleSiegeEngineDestroyed(eventData)
	--something here
	end
	*/

	/***
	A siege engine has docked with a wall.
	Exports: unit

	@function onBattleSiegeEngineDocksWall
	@tparam eventTrigger eventData

	@usage
	function onBattleSiegeEngineDocksWall(eventData)
	--something here
	end
	*/

	/***
	An engine has started attacking a gate.
	Exports: unit

	@function onBattleGatesAttackedByEngine
	@tparam eventTrigger eventData

	@usage
	function onBattleGatesAttackedByEngine(eventData)
	--something here
	end
	*/

	/***
	An engine has destroyed a gate.
	Exports: unit

	@function onBattleGatesDestroyedByEngine
	@tparam eventTrigger eventData

	@usage
	function onBattleGatesDestroyedByEngine(eventData)
	--something here
	end
	*/

	/***
	A siege engine has knocked down a wall.
	Exports: unit

	@function onBattleWallsBreachedByEngine
	@tparam eventTrigger eventData

	@usage
	function onBattleWallsBreachedByEngine(eventData)
	--something here
	end
	*/

	/***
	A wall has been captured.
	Exports: unit

	@function onBattleWallsCaptured
	@tparam eventTrigger eventData

	@usage
	function onBattleWallsCaptured(eventData)
	--something here
	end
	*/

	/***
	A unit has routed.
	Exports: unit

	@function onBattleUnitRouts
	@tparam eventTrigger eventData

	@usage
	function onBattleUnitRouts(eventData)
	--something here
	end
	*/

	/***
	A unit has been disbanded.
	Exports: faction, playerUnit, eduEntry, religion

	@function onUnitDisbanded
	@tparam eventTrigger eventData

	@usage
	function onUnitDisbanded(eventData)
	--something here
	end
	*/

	/***
	A unit has been trained.
	Exports: settlement, faction, playerUnit, eduEntry, religion

	@function onUnitTrained
	@tparam eventTrigger eventData

	@usage
	function onUnitTrained(eventData)
	--something here
	end
	*/

	/***
	A unit has been trained.
	Exports: character, settlement, faction, regionID, playerUnit, eduEntry, characterType, religion

	@function onGovernorUnitTrained
	@tparam eventTrigger eventData

	@usage
	function onGovernorUnitTrained(eventData)
	--something here
	end
	*/

	/***
	A building has been completed.
	Exports: character, settlement, faction, regionID, priorBuild, characterType, religion

	@function onGovernorBuildingCompleted
	@tparam eventTrigger eventData

	@usage
	function onGovernorBuildingCompleted(eventData)
	--something here
	end
	*/

	/***
	An agent has been trained.
	Exports: character, settlement, faction, regionID, characterType, religion

	@function onAgentCreated
	@tparam eventTrigger eventData

	@usage
	function onAgentCreated(eventData)
	--something here
	end
	*/

	/***
	An agent has been trained.
	Exports: character, settlement, faction, regionID, characterType, religion

	@function onGovernorAgentCreated
	@tparam eventTrigger eventData

	@usage
	function onGovernorAgentCreated(eventData)
	--something here
	end
	*/

	/***
	A building has been destroyed.
	Exports: settlement, faction, regionID, resourceDescription, religion

	@function onBuildingDestroyed
	@tparam eventTrigger eventData

	@usage
	function onBuildingDestroyed(eventData)
	--something here
	end
	*/

	/***
	A building has been added to the construction queue.
	Exports: settlement, faction, regionID, resourceDescription, religion

	@function onAddedToBuildingQueue
	@tparam eventTrigger eventData

	@usage
	function onAddedToBuildingQueue(eventData)
	--something here
	end
	*/

	/***
	A building has been completed.
	Exports: settlement, faction, priorBuild, religion

	@function onBuildingCompleted
	@tparam eventTrigger eventData

	@usage
	function onBuildingCompleted(eventData)
	--something here
	end
	*/

	/***
	The player has requested building advice.
	Exports: settlement, faction, regionID, religion

	@function onRequestBuildingAdvice
	@tparam eventTrigger eventData

	@usage
	function onRequestBuildingAdvice(eventData)
	--something here
	end
	*/

	/***
	The player has requested training advice.
	Exports: settlement, faction, regionID, religion

	@function onRequestTrainingAdvice
	@tparam eventTrigger eventData

	@usage
	function onRequestTrainingAdvice(eventData)
	--something here
	end
	*/

	/***
	A unit has been added to the training queue.
	Exports: settlement, faction, regionID, resourceDescription, religion

	@function onAddedToTrainingQueue
	@tparam eventTrigger eventData

	@usage
	function onAddedToTrainingQueue(eventData)
	--something here
	end
	*/

	/***
	An army has been entirely routed.
	Exports: army

	@function onBattleArmyRouted
	@tparam eventTrigger eventData

	@usage
	function onBattleArmyRouted(eventData)
	--something here
	end
	*/

	/***
	A reinforcing army has arrived on the battlefield.
	Exports: army

	@function onBattleReinforcementsArrive
	@tparam eventTrigger eventData

	@usage
	function onBattleReinforcementsArrive(eventData)
	--something here
	end
	*/

	/***
	The player has requested mercenaries advice.
	Exports: army

	@function onRequestMercenariesAdvice
	@tparam eventTrigger eventData

	@usage
	function onRequestMercenariesAdvice(eventData)
	--something here
	end
	*/

	/***
	The player has clicked on a button.
	Exports: resourceDescription

	@function onButtonPressed
	@tparam eventTrigger eventData

	@usage
	function onButtonPressed(eventData)
	--something here
	end
	*/

	/***
	The player triggered a keyboard shortcut.
	Exports: resourceDescription

	@function onShortcutTriggered
	@tparam eventTrigger eventData

	@usage
	function onShortcutTriggered(eventData)
	--something here
	end
	*/

	/***
	A special UI Element is visible.
	Exports: resourceDescription

	@function onUIElementVisible
	@tparam eventTrigger eventData

	@usage
	function onUIElementVisible(eventData)
	--something here
	end
	*/

	/***
	The player opened a scroll.
	Exports: resourceDescription

	@function onScrollOpened
	@tparam string eventData

	@usage
	function onScrollOpened(eventData)
	--something here
	end
	*/

	/***
	The player closed a scroll.
	Exports: resourceDescription

	@function onScrollClosed
	@tparam eventTrigger eventData

	@usage
	function onScrollClosed(eventData)
	--something here
	end
	*/

	/***
	The player has requested advice on a scroll.
	Exports: resourceDescription

	@function onScrollAdviceRequested
	@tparam eventTrigger eventData

	@usage
	function onScrollAdviceRequested(eventData)
	--something here
	end
	*/

	/***
	The player has suppressed a piece of advice.
	Exports: resourceDescription

	@function onAdviceSupressed
	@tparam eventTrigger eventData

	@usage
	function onAdviceSupressed(eventData)
	--something here
	end
	*/

	/***
	A general has been routed.
	Exports: stratCharacter, character

	@function onBattleGeneralRouted
	@tparam eventTrigger eventData

	@usage
	function onBattleGeneralRouted(eventData)
	--something here
	end
	*/

	/***
	A general has been killed.
	Exports: stratCharacter

	@function onBattleGeneralKilled
	@tparam eventTrigger eventData

	@usage
	function onBattleGeneralKilled(eventData)
	--something here
	end
	*/

	/***
	A Crusade/Jihad has been called.
	Exports: targetSettlement, targetFaction, targetRegionID, targetReligion, crusade

	@function onCrusadeCalled
	@tparam eventTrigger eventData

	@usage
	function onCrusadeCalled(eventData)
	--something here
	end
	*/

	/***
	The Pope has accepted the player's crusade target.
	Exports: targetSettlement, targetFaction, targetRegionID, targetReligion, crusade

	@function onPopeAcceptsCrusadeTarget
	@tparam eventTrigger eventData

	@usage
	function onPopeAcceptsCrusadeTarget(eventData)
	--something here
	end
	*/

	/***
	The Pope has rejected the player's crusade target.
	Exports: targetSettlement, targetFaction, targetRegionID, targetReligion, crusade

	@function onPopeRejectsCrusadeTarget
	@tparam eventTrigger eventData

	@usage
	function onPopeRejectsCrusadeTarget(eventData)
	--something here
	end
	*/

	/***
	A Crusade/Jihad has ended.
	Exports: targetSettlement, targetRegionID, crusade

	@function onCrusadeEnds
	@tparam eventTrigger eventData

	@usage
	function onCrusadeEnds(eventData)
	--something here
	end
	*/

	/***
	Called before the faction's turn starts.
	Exports: faction, religion

	@function onPreFactionTurnStart
	@tparam eventTrigger eventData

	@usage
	function onPreFactionTurnStart(eventData)
	--something here
	end
	*/

	/***
	Called at a faction's turn start.
	Exports: faction, religion

	@function onFactionTurnStart
	@tparam eventTrigger eventData

	@usage
	function onFactionTurnStart(eventData)
	--something here
	end
	*/

	/***
	Called after faction changes to a new capital.
	Exports: faction, religion

	@function onFactionNewCapital
	@tparam eventTrigger eventData

	@usage
	function onFactionNewCapital(eventData)
	newCapital=fac.capital();
	end
	*/

	/***
	Called at a faction's turn end.
	Exports: faction, religion

	@function onFactionTurnEnd
	@tparam eventTrigger eventData

	@usage
	function onFactionTurnEnd(eventData)
	--something here
	end
	*/

	/***
	A Faction has voted for the new pope.
	Exports: faction, religion

	@function onVotedForPope
	@tparam eventTrigger eventData

	@usage
	function onVotedForPope(eventData)
	--something here
	end
	*/

	/***
	A Faction has been excommunicated.
	Exports: faction, religion

	@function onFactionExcommunicated
	@tparam eventTrigger eventData

	@usage
	function onFactionExcommunicated(eventData)
	--something here
	end
	*/

	/***
	A Faction has formed a horde.
	Exports: faction, religion

	@function onHordeFormed
	@tparam eventTrigger eventData

	@usage
	function onHordeFormed(eventData)
	--something here
	end
	*/

	/***
	A cardinal has been removed from the game.
	Exports: faction, religion

	@function onCardinalRemoved
	@tparam eventTrigger eventData

	@usage
	function onCardinalRemoved(eventData)
	--something here
	end
	*/

	/***
	An inquisitor has been dispatched to a region.
	Exports: faction, religion

	@function onInquisitorAppointed
	@tparam eventTrigger eventData

	@usage
	function onInquisitorAppointed(eventData)
	--something here
	end
	*/

	/***
	An assassination mission against the pope has failed and the assassin is executed.
	Exports: faction, religion

	@function onAssassinCaughtAttackingPope
	@tparam eventTrigger eventData

	@usage
	function onAssassinCaughtAttackingPope(eventData)
	--something here
	end
	*/

	/***
	The player has opened his finances panel.
	Exports: faction, religion

	@function onFinancesPanelOpen
	@tparam eventTrigger eventData

	@usage
	function onFinancesPanelOpen(eventData)
	--something here
	end
	*/

	/***
	The player has opened the faction summary panel.
	Exports: faction, religion

	@function onFactionSummaryPanelOpen
	@tparam eventTrigger eventData

	@usage
	function onFactionSummaryPanelOpen(eventData)
	--something here
	end
	*/

	/***
	The player has opened the family tree panel.
	Exports: faction, religion

	@function onFamilyTreePanelOpen
	@tparam eventTrigger eventData

	@usage
	function onFamilyTreePanelOpen(eventData)
	--something here
	end
	*/

	/***
	The player has opened the diplomatic standing panel.
	Exports: faction, religion

	@function onDiplomacyPanelOpen
	@tparam eventTrigger eventData

	@usage
	function onDiplomacyPanelOpen(eventData)
	--something here
	end
	*/

	/***
	The player has attacked an army or settlement. The decision panel is now open.
	Exports: faction, religion

	@function onPreBattlePanelOpen
	@tparam eventTrigger eventData

	@usage
	function onPreBattlePanelOpen(eventData)
	--something here
	end
	*/

	/***
	A message has arrived for the player.
	Exports: faction, eventID, religion

	@function onIncomingMessage
	@tparam eventTrigger eventData

	@usage
	function onIncomingMessage(eventData)
	--something here
	end
	*/

	/***
	The player has opened a message.
	Exports: faction, eventID, religion

	@function onMessageOpen
	@tparam eventTrigger eventData

	@usage
	function onMessageOpen(eventData)
	--something here
	end
	*/

	/***
	The player has closed a message.
	Exports: eventID

	@function onMessageClosed
	@tparam eventTrigger eventData

	@usage
	function onMessageClosed(eventData)
	--something here
	end
	*/

	/***
	The player has declined automated settlement management.
	Exports: faction, religion

	@function onDeclineAutomatedSettlementManagement
	@tparam eventTrigger eventData

	@usage
	function onDeclineAutomatedSettlementManagement(eventData)
	--something here
	end
	*/

	/***
	The battle AI has begun processing.
	Exports: faction, religion

	@function onBattleAiCommenced
	@tparam eventTrigger eventData

	@usage
	function onBattleAiCommenced(eventData)
	--something here
	end
	*/

	/***
	The delay phase has begun.
	Exports: faction, religion

	@function onBattleDelayPhaseCommenced
	@tparam eventTrigger eventData

	@usage
	function onBattleDelayPhaseCommenced(eventData)
	--something here
	end
	*/

	/***
	The deployment phase has begun.
	Exports: faction, religion

	@function onBattleDeploymentPhaseCommenced
	@tparam eventTrigger eventData

	@usage
	function onBattleDeploymentPhaseCommenced(eventData)
	--something here
	end
	*/

	/***
	The conflict phase has begun.
	Exports: faction, religion

	@function onBattleConflictPhaseCommenced
	@tparam eventTrigger eventData

	@usage
	function onBattleConflictPhaseCommenced(eventData)
	--something here
	end
	*/

	/***
	Called after a faction declares a war.
	Exports: faction, targetFaction, religion, targetReligion

	@function onFactionWarDeclared
	@tparam eventTrigger eventData

	@usage
	function onFactionWarDeclared(eventData)
	--something
	end
	*/

	/***
	Called after a faction declares an alliance.
	Exports: faction, targetFaction, religion, targetReligion

	@function onFactionAllianceDeclared
	@tparam eventTrigger eventData

	@usage
	function onFactionAllianceDeclared(eventData)
	--something
	end
	*/

	/***
	A marriage has occured between two factions. gets fired for both factions involved.
	Exports: faction, targetFaction, religion, targetReligion

	@function onInterFactionMarriage
	@tparam eventTrigger eventData

	@usage
	function onInterFactionMarriage(eventData)
	--something
	end
	*/

	/***
	Called after a faction makes a trade agreement.
	Exports: faction, targetFaction, religion, targetReligion

	@function onFactionTradeAgreementMade
	@tparam eventTrigger eventData

	@usage
	function onFactionTradeAgreementMade(eventData)
	--something
	end
	*/

	/***
	A new pope has been elected.
	Exports: faction, targetFaction, religion, targetReligion

	@function onPopeElected
	@tparam eventTrigger eventData

	@usage
	function onPopeElected(eventData)
	--something
	end
	*/

	/***
	Called after a faction breaks an alliance.
	Exports: faction, targetFaction, religion, targetReligion

	@function onFactionBreakAlliance
	@tparam eventTrigger eventData

	@usage
	function onFactionBreakAlliance(eventData)
	--something
	end
	*/

	/***
	A faction to faction attitude update has occurred (once every round).
	Exports: faction, targetFaction, religion

	@function onUpdateAttitude
	@tparam eventTrigger eventData

	@usage
	function onUpdateAttitude(eventData)
	--something
	end
	*/

	/***
	A demeanour response has occured in diplomacy talks.
	Exports: faction, targetFaction, religion, targetReligion, amount

	@function onDemeanour
	@tparam eventTrigger eventData

	@usage
	function onDemeanour(eventData)
	--something
	end
	*/

	/***
	Called after a faction gives money to another faction.
	Exports: faction, targetFaction, religion, targetReligion, amount

	@function onGiveMoney
	@tparam eventTrigger eventData

	@usage
	function onGiveMoney(eventData)
	--something
	end
	*/

	/***
	A player unit has attacked one of the enemies units.
	Exports: attackingUnit, defendingUnit, playerUnit, enemyUnit

	@function onBattlePlayerUnitAttacksEnemyUnit
	@tparam eventTrigger eventData

	@usage
	function onBattlePlayerUnitAttacksEnemyUnit(eventData)
	--something here
	end
	*/

	/***
	An enemy unit has attacked one of the players units.
	Exports: attackingUnit, defendingUnit, playerUnit, enemyUnit

	@function onBattleEnemyUnitAttacksPlayerUnit
	@tparam eventTrigger eventData

	@usage
	function onBattleEnemyUnitAttacksPlayerUnit(eventData)
	--something here
	end
	*/

	/***
	One of the player's units has gone berserk.
	Exports: playerUnit

	@function onBattlePlayerUnitGoesBerserk
	@tparam eventTrigger eventData

	@usage
	function onBattlePlayerUnitGoesBerserk(eventData)
	--something here
	end
	*/

	/***
	One of the player's units has routed.
	Exports: playerUnit

	@function onBattlePlayerUnitRouts
	@tparam eventTrigger eventData

	@usage
	function onBattlePlayerUnitRouts(eventData)
	--something here
	end
	*/

	/***
	A siege engine belonging to the player has been destroyed.
	Exports: playerUnit

	@function onBattlePlayerSiegeEngineDestroyed
	@tparam eventTrigger eventData

	@usage
	function onBattlePlayerSiegeEngineDestroyed(eventData)
	--something here
	end
	*/

	/***
	A player's engine has started attacking a gate.
	Exports: playerUnit

	@function onBattleGatesAttackedByPlayerEngine
	@tparam eventTrigger eventData

	@usage
	function onBattleGatesAttackedByPlayerEngine(eventData)
	--something here
	end
	*/

	/***
	One of the enemy's units has gone berserk.
	Exports: enemyUnit

	@function onBattleEnemyUnitGoesBerserk
	@tparam eventTrigger eventData

	@usage
	function onBattleEnemyUnitGoesBerserk(eventData)
	--something here
	end
	*/

	/***
	One of the enemy's units has routed.
	Exports: enemyUnit

	@function onBattnemyUnitRouts
	@tparam eventTrigger eventData

	@usage
	function onBattnemyUnitRouts(eventData)
	--something here
	end
	*/

	/***
	A siege engine belonging to the enemy has been destroyed.
	Exports: enemyUnit

	@function onBattnemySiegeEngineDestroyed
	@tparam eventTrigger eventData

	@usage
	function onBattnemySiegeEngineDestroyed(eventData)
	--something here
	end
	*/

	/***
	An enemy's engine has started attacking a gate.
	Exports: enemyUnit

	@function onBattleGatesAttackedByEnemyEngine
	@tparam eventTrigger eventData

	@usage
	function onBattleGatesAttackedByEnemyEngine(eventData)
	--something here
	end
	*/

	/***
	When a particular disaster has just happened.
	Exports: resourceDescription

	@function onDisaster
	@tparam eventTrigger eventData

	@usage
	function onDisaster(eventData)
		if eventData.resourceDescription == "earthquake" then
			--something here
		end
	end
	*/

	/***
	An event counter has changed it's value.
	Exports: eventCounter

	@function onEventCounter
	@tparam eventTrigger eventData

	@usage
	function onEventCounter(eventData)
		if eventData.eventCounter == "my_event_counter" then
			--something here
		end
	end
	*/

	/***
	The last unit has been removed from a fort, agents do not count.
	Exports: fort, faction, regionID, religion

	@function onUngarrisonedFort
	@tparam eventTrigger eventData

	@usage
	function onUngarrisonedFort(eventData)
		--something here
	end
	*/

	/***
	An object of the target faction has been seen by the faction.
	Exports: faction, targetFaction, regionID, coords, religion

	@function onObjSeen
	@tparam eventTrigger eventData

	@usage
	function onObjSeen(eventData)
		--something here
	end
	*/

	/***
	The tile has been seen by the faction.
	Exports: faction, regionID, coords, religion

	@function onTileSeen
	@tparam eventTrigger eventData

	@usage
	function onTileSeen(eventData)
		--something here
	end
	*/

	/***
	A faction to faction transgression has occurred.
	Exports: faction, targetFaction, resourceDescription, religion, targetReligion
	transgressions: TC_THREATEN_WAR, TC_DECLARED_WAR, TC_MINOR_ASSASSINATION_ATTEMPT, TC_BROKE_TREATY_TERMS, TC_BROKE_ALLIANCE, TC_INVASION

	@function onTransgression
	@tparam eventTrigger eventData

	@usage
	function onTransgression(eventData)
		--something here
	end
	*/

	/***
	A faction to faction forgiveness has occurred.
	Exports: faction, targetFaction, resourceDescription, religion
	forgiveness: FC_MILITARY_ASSISTANCE, FC_OBVIOUS_BRIBE

	@function onForgiveness
	@tparam eventTrigger eventData

	@usage
	function onForgiveness(eventData)
		--something here
	end
	*/

	/***
	An army has taken a crusade or jihad target settlement.
	Exports: targetSettlement, faction, targetFaction, army, regionID, targetRegionID, coords, religion, targetReligion, crusade

	@function onArmyTakesCrusadeTarget
	@tparam eventTrigger eventData

	@usage
	function onArmyTakesCrusadeTarget(eventData)
		--something here
	end
	*/

	/***
	Units have deserted a crusade or jihad.
	Exports: targetSettlement, faction, targetFaction, targetRegionID, religion, targetReligion, crusade

	@function onUnitsDesertCrusade
	@tparam eventTrigger eventData

	@usage
	function onUnitsDesertCrusade(eventData)
		--something here
	end
	*/




	/***
	Called every time an image is rendered for display

	@function draw
	@tparam LPDIRECT3DDEVICE9 pDevice

	@usage
	function draw(device)
		ImGui.Begin("test");

		ImGui.Text("Some test text")

		ImGui.End();
	end
	*/
	drawLuaFunc = new sol::function(luaState["draw"]);
	checkLuaFunc(&drawLuaFunc);

	/***
	Called when ImGui backend reload fonts

	@function onLoadingFonts

	@usage
	function onLoadingFonts()
	--something here
	end
	*/
	onLoadingFonts = new sol::function(luaState["onLoadingFonts"]);
	checkLuaFunc(&onLoadingFonts);

	resetDXFunc = new sol::function(luaState["deinitDX"]);
	checkLuaFunc(&resetDXFunc);

	initDXFunc = new sol::function(luaState["initDX"]);
	checkLuaFunc(&initDXFunc);

	/***
	Called when a new campaign is started from the menu.

	@function onNewGameStart

	@usage
	function onNewGameStart()
	--something here
	end
	*/



	onNewGameStart = new sol::function(luaState["onNewGameStart"]);
	checkLuaFunc(&onNewGameStart);

	/***
	Called after the game loads various db`s (edu, etc) at startup.

	@function onReadGameDbsAtStart

	@usage
	function onReadGameDbsAtStart()
	--something here
	end
	*/



	onReadGameDbsAtStart = new sol::function(luaState["onReadGameDbsAtStart"]);
	checkLuaFunc(&onReadGameDbsAtStart);

	/***
	Called after the game loads various db`s (edu, etc) at startup.

	@function onGameInit

	@usage
	function onGameInit()
	--something here
	end
	*/



	onGameInit = new sol::function(luaState["onGameInit"]);
	checkLuaFunc(&onGameInit);

	/***
	Called on ai initialized on turn start.

	@function onAiTurn
	@tparam aiFaction aiFaction

	@usage
	function onAiTurn(aiFaction)
	--something here
	end
	*/

	onAiTurn = new sol::function(luaState["onAiTurn"]);
	checkLuaFunc(&onAiTurn);

	/***
	Called on clicking the stratmap.

	@function onClickAtTile
	@tparam int x
	@tparam int y
	@usage
	clickAtMap={["isClicked"]=false,["xTile"]=0,["yTile"]=0}
	function draw(device)
		if(clickAtMap.isClicked==true)
		then
			print(tostring(ImGui.IsMouseDown(ImGuiMouseButton.ImGuiMouseButton_Right)).."\n");
			print(tostring(ImGui.IsMouseDown(ImGuiMouseButton.ImGuiMouseButton_Left)).."\n");
			print(tostring(clickAtMap.xTile).." "..tostring(clickAtMap.yTile));

			clickAtMap.isClicked=false;
		end
	end

	function onClickAtTile(x,y)
		clickAtMap.isClicked=true;
		clickAtMap.xTile=x;
		clickAtMap.yTile=y;
	end
	*/



	onClickAtTile = new sol::function(luaState["onClickAtTile"]);
	checkLuaFunc(&onClickAtTile);

	/***
	Called after loading the campaign map

	@function onCampaignMapLoaded

	@usage
	function onCampaignMapLoaded()
	--something here
	end
	*/



	onCampaignMapLoaded = new sol::function(luaState["onCampaignMapLoaded"]);
	checkLuaFunc(&onCampaignMapLoaded);

	/***
	Called on plugin load (at game start).

	@function onPluginLoad

	@usage
	--An example of using this event to perform actions at the beginning of a campaign once:

	isCampaignLoadedFirstTime=false;
	function onNewGameStart()
	  isCampaignLoadedFirstTime=true;
	end

	function onCampaignMapLoaded()
	  if(isCampaignLoadedFirstTime==false)
		then
		  do return end
		end


	  --here we do our stuff, what works only one time for each campaign
	  isCampaignLoadedFirstTime=false;
	end
	*/



	onPluginLoad = new sol::function(luaState["onPluginLoad"]);
	checkLuaFunc(&onPluginLoad);


	/***
	Called on creating a new save file.

	@function onCreateSaveFile

	@usage
		function onCreateSaveFile()
			local savefiles, currentPath = {}, M2TWEOP.getPluginPath()
			savefiles[1] = currentPath.."\\testSaveFile1.ourSave"
			savefiles[2] = currentPath.."\\testSaveFile2.ourSave"

			file1 = io.open(savefiles[1], "w"); file1:write("This is save file 1!!!"); file1:close()
			file2 = io.open(savefiles[2], "w"); file2:write("This is save file 2!!!"); file2:close()

			return savefiles
		end
	*/
	onCreateSaveFile = new sol::function(luaState["onCreateSaveFile"]);
	checkLuaFunc(&onCreateSaveFile);

	/***
	Called on loading a save file.

	@function onLoadSaveFile

	@usage
	function onLoadSaveFile(paths)
		lastSaveFiles={};
		for key, value in pairs(paths) do

		print(key, " -- ", value);

		lastSaveFiles[key]={};
		lastSaveFiles[key][1]=value;
		lastSaveFiles[key][2]=readFile(value);

		end
	end


	--read file and return it`s content
	local function readFile(path)
		local file = io.open(path, "r") ;
		if (not file) then return nil end
		local content = file:read "*a";
		file:close();
		return content;
	end
	*/
	onLoadSaveFile = new sol::function(luaState["onLoadSaveFile"]);
	checkLuaFunc(&onLoadSaveFile);


	/***
	Called at the start of a new turn.

	@function onChangeTurnNum
	@tparam int turnNumber

	@usage
	function onChangeTurnNum(turnNum)
	--something here
	end
	*/
	onChangeTurnNumFunc = new sol::function(luaState["onChangeTurnNum"]);
	checkLuaFunc(&onChangeTurnNumFunc);

	/***
	Called on select worldpkgdesc for battlemap. See https://wiki.twcenter.net/index.php?title=.worldpkgdesc_-_M2TW. M2TWEOP will ignore the return value if its group does not match the required group!

	@function onSelectWorldpkgdesc
	@tparam string selectedRecordName
	@tparam string selectedRecordGroup
	@treturn string newSelectedRecordName

	@usage
	function onSelectWorldpkgdesc(selectedRecord,selectedRecordGroup)
	--something here

		return selectedRecord;
	end
	*/
	onSelectWorldpkgdesc = new sol::function(luaState["onSelectWorldpkgdesc"]);
	checkLuaFunc(&onSelectWorldpkgdesc);
	/***
	Called on specified fortificationlevel in a siege of a settlement.

	@function onfortificationlevelS
	@tparam settlementStruct siegedSettlement
	@treturn int overridedFortificationlevel
	@treturn bool isCastle override settlement type (siege equipment is slightly different between cities and castles of the same level)

	@usage
	function onfortificationlevelS(settlement)
		if settlement.xCoord == 10 and settlement.yCoord == 25 then
			return 3, false --override settlement under siege at these coordinates to level 3 city
		end
		return nil --else, do not override
	end
	*/
	onfortificationlevelS = new sol::function(luaState["onfortificationlevelS"]);
	checkLuaFunc(&onfortificationlevelS);

	/***
	Called on the completion of the siege (in any way, with any outcome).

	@function onEndSiege
	@tparam int xCoord x coordinate of siege(settlement or fort)
	@tparam int yCoord y coordinate of siege(settlement or fort)

	@usage
	function onEndSiege(x,y)
	--something here
	end
	*/
	onEndSiege = new sol::function(luaState["onEndSiege"]);
	checkLuaFunc(&onEndSiege);
	/***
	Called on the starting of the siege (in any way, with any outcome).

	@function onStartSiege
	@tparam int xCoord x coordinate of siege(settlement or fort)
	@tparam int yCoord y coordinate of siege(settlement or fort)

	@usage
	function onStartSiege(x,y)
	--something here
	end
	*/
	onStartSiege = new sol::function(luaState["onStartSiege"]);
	checkLuaFunc(&onStartSiege);


	if (onPluginLoad != nullptr)
	{
		tryLua((*onPluginLoad)());
	}
}