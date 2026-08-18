VictoryRoad_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, VictoryRoad_1F_OnTransition
	.byte 0

VictoryRoad_1F_OnTransition:
	call_if_eq VAR_VICTORY_ROAD_1F_STATE, 1, VictoryRoad_1F_EventScript_SetEntranceWallyPos1
	call_if_eq VAR_VICTORY_ROAD_1F_STATE, 2, VictoryRoad_1F_EventScript_SetEntranceWallyPos2
	end

VictoryRoad_1F_EventScript_SetEntranceWallyPos1::
	setobjectxyperm LOCALID_VICTORY_ROAD_ENTRANCE_WALLY, 2, 24
	setobjectmovementtype LOCALID_VICTORY_ROAD_ENTRANCE_WALLY, MOVEMENT_TYPE_FACE_DOWN
	return

VictoryRoad_1F_EventScript_SetEntranceWallyPos2::
	setobjectxyperm LOCALID_VICTORY_ROAD_ENTRANCE_WALLY, 3, 24
	setobjectmovementtype LOCALID_VICTORY_ROAD_ENTRANCE_WALLY, MOVEMENT_TYPE_FACE_DOWN
	return

VictoryRoad_1F_EventScript_WallyBattleTrigger1::
	lockall
	setvar VAR_0x8008, 1
	addobject LOCALID_VICTORY_ROAD_ENTRANCE_WALLY
	applymovement LOCALID_VICTORY_ROAD_ENTRANCE_WALLY, VictoryRoad_1F_Movement_WallyApproachPlayer1
	waitmovement 0
	goto VictoryRoad_1F_EventScript_WallyEntranceBattle
	end

VictoryRoad_1F_EventScript_WallyBattleTrigger2::
	lockall
	setvar VAR_0x8008, 2
	addobject LOCALID_VICTORY_ROAD_ENTRANCE_WALLY
	applymovement LOCALID_VICTORY_ROAD_ENTRANCE_WALLY, VictoryRoad_1F_Movement_WallyApproachPlayer2
	waitmovement 0
	goto VictoryRoad_1F_EventScript_WallyEntranceBattle
	end

VictoryRoad_1F_EventScript_WallyEntranceBattle::
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	msgbox VictoryRoad_1F_Text_WallyNotGoingToLoseAnymore, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_WALLY_VR_1, VictoryRoad_1F_Text_WallyEntranceDefeat
	msgbox VictoryRoad_1F_Text_WallyPostEntranceBattle, MSGBOX_DEFAULT
	clearflag FLAG_HIDE_VICTORY_ROAD_ENTRANCE_WALLY
	copyobjectxytoperm LOCALID_VICTORY_ROAD_ENTRANCE_WALLY
	setflag FLAG_DEFEATED_WALLY_VICTORY_ROAD
	copyvar VAR_VICTORY_ROAD_1F_STATE, VAR_0x8008
	releaseall
	end

VictoryRoad_1F_Movement_WallyApproachPlayer1:
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_up
	step_end

VictoryRoad_1F_Movement_WallyApproachPlayer2:
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_up
	step_end

@ This Wally appears near the entrance once his battle is triggered and remains there until the Hall of Fame is entered
VictoryRoad_1F_EventScript_EntranceWally::
	msgbox VictoryRoad_1F_Text_WallyPostEntranceBattle, MSGBOX_NPC
	end

@ This Wally appears and remains at the exit after the Hall of Fame is entered
VictoryRoad_1F_EventScript_ExitWally::
	trainerbattle_single TRAINER_WALLY_VR_2, VictoryRoad_1F_Text_WallyIntro, VictoryRoad_1F_Text_WallyDefeat
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, VictoryRoad_1F_EventScript_RematchWally
	msgbox VictoryRoad_1F_Text_WallyPostBattle, MSGBOX_AUTOCLOSE
	end

VictoryRoad_1F_EventScript_RematchWally::
	trainerbattle_rematch TRAINER_WALLY_VR_2, VictoryRoad_1F_Text_WallyIntro, VictoryRoad_1F_Text_WallyDefeat
	msgbox VictoryRoad_1F_Text_WallyPostBattle, MSGBOX_AUTOCLOSE
	end

VictoryRoad_1F_EventScript_Edgar::
	trainerbattle_single TRAINER_EDGAR, VictoryRoad_1F_Text_EdgarIntro, VictoryRoad_1F_Text_EdgarDefeat
	msgbox VictoryRoad_1F_Text_EdgarPostBattle, MSGBOX_AUTOCLOSE
	end

VictoryRoad_1F_EventScript_Albert::
	trainerbattle_single TRAINER_ALBERT, VictoryRoad_1F_Text_AlbertIntro, VictoryRoad_1F_Text_AlbertDefeat
	msgbox VictoryRoad_1F_Text_AlbertPostBattle, MSGBOX_AUTOCLOSE
	end

VictoryRoad_1F_EventScript_Hope::
	trainerbattle_single TRAINER_HOPE, VictoryRoad_1F_Text_HopeIntro, VictoryRoad_1F_Text_HopeDefeat
	msgbox VictoryRoad_1F_Text_HopePostBattle, MSGBOX_AUTOCLOSE
	end

VictoryRoad_1F_EventScript_Quincy::
	trainerbattle_single TRAINER_QUINCY, VictoryRoad_1F_Text_QuincyIntro, VictoryRoad_1F_Text_QuincyDefeat
	msgbox VictoryRoad_1F_Text_QuincyPostBattle, MSGBOX_AUTOCLOSE
	end

VictoryRoad_1F_EventScript_Katelynn::
	trainerbattle_single TRAINER_KATELYNN, VictoryRoad_1F_Text_KatelynnIntro, VictoryRoad_1F_Text_KatelynnDefeat
	msgbox VictoryRoad_1F_Text_KatelynnPostBattle, MSGBOX_AUTOCLOSE
	end

VictoryRoad_1F_Text_WallyNotGoingToLoseAnymore:
	.string "WALLY: Hi! {PLAYER}!\p"
	.string "I bet you're surprised to see me here!\p"
	.string "I made it all the way here, and it's\n"
	.string "all thanks to you!\p"
	.string "{PLAYER}, losing to you that time\n"
	.string "made me stronger!\p"
	.string "But I'm not going to lose anymore!\p"
	.string "I'm going to win! For the POKéMON who\n"
	.string "gave me courage and strength!\p"
	.string "Okay… Here I come!$"

VictoryRoad_1F_Text_WallyEntranceDefeat:
	.string "Wow!\n"
	.string "{PLAYER}, you are strong, after all!$"

VictoryRoad_1F_Text_WallyPostEntranceBattle:
	.string "WALLY: I couldn't beat you today,\n"
	.string "{PLAYER}, but one of these days, I'll\l"
	.string "catch up to you!$"

VictoryRoad_1F_Text_WallyIntro:
	.string "WALLY: Hi! {PLAYER}!\p"
	.string "I've gotten stronger since that last\n"
	.string "time! I wanted to show you, {PLAYER}!\p"
	.string "Okay… Here I come!$"

VictoryRoad_1F_Text_WallyDefeat:
	.string "Wow!\n"
	.string "{PLAYER}, you are strong, after all!$"

VictoryRoad_1F_Text_WallyPostBattle:
	.string "WALLY: I couldn't beat you this time,\n"
	.string "too… But one of these days, {PLAYER},\l"
	.string "I'm going to catch up to you…\p"
	.string "And challenge the POKéMON LEAGUE!$"

VictoryRoad_1F_Text_EdgarIntro:
	.string "I've made it this far a couple times,\n"
	.string "but the last stretch is so long…$"

VictoryRoad_1F_Text_EdgarDefeat:
	.string "My dream ends here again…$"

VictoryRoad_1F_Text_EdgarPostBattle:
	.string "You've made it this far. Keep the\n"
	.string "momentum going and become the\l"
	.string "CHAMPION! If anyone can, it's you!$"

VictoryRoad_1F_Text_AlbertIntro:
	.string "I didn't come all this way to lose now.\n"
	.string "That possibility doesn't exist!$"

VictoryRoad_1F_Text_AlbertDefeat:
	.string "Impossible…\n"
	.string "I lost?$"

VictoryRoad_1F_Text_AlbertPostBattle:
	.string "I lost here…\p"
	.string "That means I lack the qualifications\n"
	.string "to become the CHAMPION…$"

VictoryRoad_1F_Text_HopeIntro:
	.string "This seemingly infinite and harsh road\n"
	.string "lives up to its name of VICTORY.$"

VictoryRoad_1F_Text_HopeDefeat:
	.string "Your battle style is fantastic…$"

VictoryRoad_1F_Text_HopePostBattle:
	.string "You seem to have the potential for\n"
	.string "becoming the CHAMPION.$"

VictoryRoad_1F_Text_QuincyIntro:
	.string "What is the VICTORY ROAD?\n"
	.string "I'll tell you if you win!$"

VictoryRoad_1F_Text_QuincyDefeat:
	.string "Okay!\n"
	.string "Well done!$"

VictoryRoad_1F_Text_QuincyPostBattle:
	.string "Getting through here safely--that's\n"
	.string "the final test for any TRAINER aiming\l"
	.string "to become the POKéMON CHAMPION.\p"
	.string "That's why it's called the VICTORY\n"
	.string "ROAD.$"

VictoryRoad_1F_Text_KatelynnIntro:
	.string "I have nothing to say to anyone\n"
	.string "that's come this far. Come on!$"

VictoryRoad_1F_Text_KatelynnDefeat:
	.string "This is a disgrace…$"

VictoryRoad_1F_Text_KatelynnPostBattle:
	.string "Humph, go right on ahead.\n"
	.string "See if I care.$"

VictoryRoad_1F_EventScript_Red::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_RED, VictoryRoad_1F_EventScript_RedAlreadyDefeated
	msgbox VictoryRoad_1F_Text_RedChallenge, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, VictoryRoad_1F_EventScript_RedDeclined
	trainerbattle_single TRAINER_RED, VictoryRoad_1F_Text_RedIntro, VictoryRoad_1F_Text_RedDefeat, VictoryRoad_1F_EventScript_RedVictory
	end

VictoryRoad_1F_EventScript_RedDeclined::
	release
	end

VictoryRoad_1F_EventScript_RedAlreadyDefeated::
	msgbox VictoryRoad_1F_Text_RedAlreadyDefeated, MSGBOX_DEFAULT
	release
	end

VictoryRoad_1F_EventScript_RedVictory::
	msgbox VictoryRoad_1F_Text_RedPostBattle, MSGBOX_DEFAULT
	goto VictoryRoad_1F_EventScript_RedRewardChoice
	end

VictoryRoad_1F_EventScript_RedRewardChoice::
	dynmultipush VictoryRoad_1F_Text_RewardMegaStone, 0
	dynmultipush VictoryRoad_1F_Text_RewardGoldBottleCap, 1
	dynmultipush VictoryRoad_1F_Text_RewardNatureMint, 2
	dynmultistack 0, 0, TRUE, 3, FALSE, 0, DYN_MULTICHOICE_CB_NONE
	closemessage
	switch VAR_RESULT
	case 0, VictoryRoad_1F_EventScript_RedRewardMegaStoneMenu
	case 1, VictoryRoad_1F_EventScript_RedRewardGoldBottleCap
	case 2, VictoryRoad_1F_EventScript_RedRewardMintMenu
	end

VictoryRoad_1F_EventScript_RedRewardGoldBottleCap::
	setorcopyvar VAR_0x8000, ITEM_GOLD_BOTTLE_CAP
	setorcopyvar VAR_0x8001, 1
	call VictoryRoad_1F_EventScript_GiveRewardItemNoRandomize
	setflag FLAG_DEFEATED_RED
	release
	end

VictoryRoad_1F_EventScript_GiveRewardItemNoRandomize::
	copyvar VAR_0x8006, VAR_0x8000
	additem VAR_0x8000, VAR_0x8001
	copyvar VAR_0x8007, VAR_RESULT
	call EventScript_ObtainItemMessage
	return

VictoryRoad_1F_Text_RedChallenge:
	.string "…\p"
	.string "So, you made it all the way here.\p"
	.string "I am RED.\p"
	.string "Do you want to battle?$"

VictoryRoad_1F_Text_RedIntro:
	.string "…$"

VictoryRoad_1F_Text_RedDefeat:
	.string "…!$"

VictoryRoad_1F_Text_RedPostBattle:
	.string "…\p"
	.string "Take what you want.$"

VictoryRoad_1F_Text_RedAlreadyDefeated:
	.string "…$"

VictoryRoad_1F_Text_RewardMegaStone:
	.string "A MEGA STONE$"

VictoryRoad_1F_Text_RewardGoldBottleCap:
	.string "A GOLD BOTTLE CAP$"

VictoryRoad_1F_Text_RewardNatureMint:
	.string "A NATURE MINT$"

VictoryRoad_1F_EventScript_RedRewardMegaStoneMenu::
	dynmultipush VictoryRoad_1F_Text_RedMS_ABOMASITE, ITEM_ABOMASITE
	dynmultipush VictoryRoad_1F_Text_RedMS_ABSOLITE, ITEM_ABSOLITE
	dynmultipush VictoryRoad_1F_Text_RedMS_ABSOLITE_Z, ITEM_ABSOLITE_Z
	dynmultipush VictoryRoad_1F_Text_RedMS_AERODACTYLITE, ITEM_AERODACTYLITE
	dynmultipush VictoryRoad_1F_Text_RedMS_AGGRONITE, ITEM_AGGRONITE
	dynmultipush VictoryRoad_1F_Text_RedMS_ALAKAZITE, ITEM_ALAKAZITE
	dynmultipush VictoryRoad_1F_Text_RedMS_ALTARIANITE, ITEM_ALTARIANITE
	dynmultipush VictoryRoad_1F_Text_RedMS_AMPHAROSITE, ITEM_AMPHAROSITE
	dynmultipush VictoryRoad_1F_Text_RedMS_AUDINITE, ITEM_AUDINITE
	dynmultipush VictoryRoad_1F_Text_RedMS_BANETTITE, ITEM_BANETTITE
	dynmultipush VictoryRoad_1F_Text_RedMS_BARBARACITE, ITEM_BARBARACITE
	dynmultipush VictoryRoad_1F_Text_RedMS_BAXCALIBRITE, ITEM_BAXCALIBRITE
	dynmultipush VictoryRoad_1F_Text_RedMS_BEEDRILLITE, ITEM_BEEDRILLITE
	dynmultipush VictoryRoad_1F_Text_RedMS_BLASTOISINITE, ITEM_BLASTOISINITE
	dynmultipush VictoryRoad_1F_Text_RedMS_BLAZIKENITE, ITEM_BLAZIKENITE
	dynmultipush VictoryRoad_1F_Text_RedMS_BUTTERFREEITE, ITEM_BUTTERFREEITE
	dynmultipush VictoryRoad_1F_Text_RedMS_CAMERUPTITE, ITEM_CAMERUPTITE
	dynmultipush VictoryRoad_1F_Text_RedMS_CHANDELURITE, ITEM_CHANDELURITE
	dynmultipush VictoryRoad_1F_Text_RedMS_CHARIZARDITE_X, ITEM_CHARIZARDITE_X
	dynmultipush VictoryRoad_1F_Text_RedMS_CHARIZARDITE_Y, ITEM_CHARIZARDITE_Y
	dynmultipush VictoryRoad_1F_Text_RedMS_CHESNAUGHTITE, ITEM_CHESNAUGHTITE
	dynmultipush VictoryRoad_1F_Text_RedMS_CHIMECHITE, ITEM_CHIMECHITE
	dynmultipush VictoryRoad_1F_Text_RedMS_CLEFABLITE, ITEM_CLEFABLITE
	dynmultipush VictoryRoad_1F_Text_RedMS_CRABOMINITE, ITEM_CRABOMINITE
	dynmultipush VictoryRoad_1F_Text_RedMS_DARKRANITE, ITEM_DARKRANITE
	dynmultipush VictoryRoad_1F_Text_RedMS_DELPHOXITE, ITEM_DELPHOXITE
	dynmultipush VictoryRoad_1F_Text_RedMS_DIANCITE, ITEM_DIANCITE
	dynmultipush VictoryRoad_1F_Text_RedMS_DRAGALGITE, ITEM_DRAGALGITE
	dynmultipush VictoryRoad_1F_Text_RedMS_DRAGONINITE, ITEM_DRAGONINITE
	dynmultipush VictoryRoad_1F_Text_RedMS_DRAMPANITE, ITEM_DRAMPANITE
	dynmultipush VictoryRoad_1F_Text_RedMS_EELEKTROSSITE, ITEM_EELEKTROSSITE
	dynmultipush VictoryRoad_1F_Text_RedMS_EMBOARITE, ITEM_EMBOARITE
	dynmultipush VictoryRoad_1F_Text_RedMS_EXCADRITE, ITEM_EXCADRITE
	dynmultipush VictoryRoad_1F_Text_RedMS_FALINKSITE, ITEM_FALINKSITE
	dynmultipush VictoryRoad_1F_Text_RedMS_FERALIGITE, ITEM_FERALIGITE
	dynmultipush VictoryRoad_1F_Text_RedMS_FLOETTITE, ITEM_FLOETTITE
	dynmultipush VictoryRoad_1F_Text_RedMS_FROSLASSITE, ITEM_FROSLASSITE
	dynmultipush VictoryRoad_1F_Text_RedMS_GALLADITE, ITEM_GALLADITE
	dynmultipush VictoryRoad_1F_Text_RedMS_GARCHOMPITE, ITEM_GARCHOMPITE
	dynmultipush VictoryRoad_1F_Text_RedMS_GARCHOMPITE_Z, ITEM_GARCHOMPITE_Z
	dynmultipush VictoryRoad_1F_Text_RedMS_GARDEVOIRITE, ITEM_GARDEVOIRITE
	dynmultipush VictoryRoad_1F_Text_RedMS_GENGARITE, ITEM_GENGARITE
	dynmultipush VictoryRoad_1F_Text_RedMS_GLALITITE, ITEM_GLALITITE
	dynmultipush VictoryRoad_1F_Text_RedMS_GLIMMORANITE, ITEM_GLIMMORANITE
	dynmultipush VictoryRoad_1F_Text_RedMS_GOLISOPITE, ITEM_GOLISOPITE
	dynmultipush VictoryRoad_1F_Text_RedMS_GOLURKITE, ITEM_GOLURKITE
	dynmultipush VictoryRoad_1F_Text_RedMS_GRENINJITE, ITEM_GRENINJITE
	dynmultipush VictoryRoad_1F_Text_RedMS_GYARADOSITE, ITEM_GYARADOSITE
	dynmultipush VictoryRoad_1F_Text_RedMS_HAWLUCHANITE, ITEM_HAWLUCHANITE
	dynmultipush VictoryRoad_1F_Text_RedMS_HEATRANITE, ITEM_HEATRANITE
	dynmultipush VictoryRoad_1F_Text_RedMS_HERACRONITE, ITEM_HERACRONITE
	dynmultipush VictoryRoad_1F_Text_RedMS_HOUNDOOMINITE, ITEM_HOUNDOOMINITE
	dynmultipush VictoryRoad_1F_Text_RedMS_KANGASKHANITE, ITEM_KANGASKHANITE
	dynmultipush VictoryRoad_1F_Text_RedMS_KINGLERITE, ITEM_KINGLERITE
	dynmultipush VictoryRoad_1F_Text_RedMS_LAPRASITE, ITEM_LAPRASITE
	dynmultipush VictoryRoad_1F_Text_RedMS_LATIASITE, ITEM_LATIASITE
	dynmultipush VictoryRoad_1F_Text_RedMS_LATIOSITE, ITEM_LATIOSITE
	dynmultipush VictoryRoad_1F_Text_RedMS_LOPUNNITE, ITEM_LOPUNNITE
	dynmultipush VictoryRoad_1F_Text_RedMS_LUCARIONITE, ITEM_LUCARIONITE
	dynmultipush VictoryRoad_1F_Text_RedMS_LUCARIONITE_Z, ITEM_LUCARIONITE_Z
	dynmultipush VictoryRoad_1F_Text_RedMS_MACHAMPITE, ITEM_MACHAMPITE
	dynmultipush VictoryRoad_1F_Text_RedMS_MAGEARNITE, ITEM_MAGEARNITE
	dynmultipush VictoryRoad_1F_Text_RedMS_MALAMARITE, ITEM_MALAMARITE
	dynmultipush VictoryRoad_1F_Text_RedMS_MANECTITE, ITEM_MANECTITE
	dynmultipush VictoryRoad_1F_Text_RedMS_MAWILITE, ITEM_MAWILITE
	dynmultipush VictoryRoad_1F_Text_RedMS_MEDICHAMITE, ITEM_MEDICHAMITE
	dynmultipush VictoryRoad_1F_Text_RedMS_MEGANIUMITE, ITEM_MEGANIUMITE
	dynmultipush VictoryRoad_1F_Text_RedMS_MEOWSTICITE, ITEM_MEOWSTICITE
	dynmultipush VictoryRoad_1F_Text_RedMS_METAGROSSITE, ITEM_METAGROSSITE
	dynmultipush VictoryRoad_1F_Text_RedMS_PIDGEOTITE, ITEM_PIDGEOTITE
	dynmultipush VictoryRoad_1F_Text_RedMS_PINSIRITE, ITEM_PINSIRITE
	dynmultipush VictoryRoad_1F_Text_RedMS_PYROARITE, ITEM_PYROARITE
	dynmultipush VictoryRoad_1F_Text_RedMS_RAICHUNITE_X, ITEM_RAICHUNITE_X
	dynmultipush VictoryRoad_1F_Text_RedMS_RAICHUNITE_Y, ITEM_RAICHUNITE_Y
	dynmultipush VictoryRoad_1F_Text_RedMS_SABLENITE, ITEM_SABLENITE
	dynmultipush VictoryRoad_1F_Text_RedMS_SALAMENCITE, ITEM_SALAMENCITE
	dynmultipush VictoryRoad_1F_Text_RedMS_SCEPTILITE, ITEM_SCEPTILITE
	dynmultipush VictoryRoad_1F_Text_RedMS_SCIZORITE, ITEM_SCIZORITE
	dynmultipush VictoryRoad_1F_Text_RedMS_SCOLIPITE, ITEM_SCOLIPITE
	dynmultipush VictoryRoad_1F_Text_RedMS_SCOVILLAINITE, ITEM_SCOVILLAINITE
	dynmultipush VictoryRoad_1F_Text_RedMS_SCRAFTINITE, ITEM_SCRAFTINITE
	dynmultipush VictoryRoad_1F_Text_RedMS_SHARPEDONITE, ITEM_SHARPEDONITE
	dynmultipush VictoryRoad_1F_Text_RedMS_SKARMORITE, ITEM_SKARMORITE
	dynmultipush VictoryRoad_1F_Text_RedMS_SLOWBRONITE, ITEM_SLOWBRONITE
	dynmultipush VictoryRoad_1F_Text_RedMS_SNORLAXITE, ITEM_SNORLAXITE
	dynmultipush VictoryRoad_1F_Text_RedMS_STARAPTITE, ITEM_STARAPTITE
	dynmultipush VictoryRoad_1F_Text_RedMS_STARMINITE, ITEM_STARMINITE
	dynmultipush VictoryRoad_1F_Text_RedMS_STEELIXITE, ITEM_STEELIXITE
	dynmultipush VictoryRoad_1F_Text_RedMS_SWAMPERTITE, ITEM_SWAMPERTITE
	dynmultipush VictoryRoad_1F_Text_RedMS_TATSUGIRINITE, ITEM_TATSUGIRINITE
	dynmultipush VictoryRoad_1F_Text_RedMS_TYRANITARITE, ITEM_TYRANITARITE
	dynmultipush VictoryRoad_1F_Text_RedMS_VENUSAURITE, ITEM_VENUSAURITE
	dynmultipush VictoryRoad_1F_Text_RedMS_VICTREEBELITE, ITEM_VICTREEBELITE
	dynmultipush VictoryRoad_1F_Text_RedMS_ZERAORITE, ITEM_ZERAORITE
	dynmultistack 0, 0, FALSE, 8, FALSE, 0, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, VictoryRoad_1F_EventScript_RedRewardChoice
	setorcopyvar VAR_0x8000, VAR_RESULT
	setorcopyvar VAR_0x8001, 1
	call VictoryRoad_1F_EventScript_GiveRewardItemNoRandomize
	setflag FLAG_DEFEATED_RED
	release
	end

VictoryRoad_1F_EventScript_RedRewardMintMenu::
	dynmultipush VictoryRoad_1F_Text_RedMint_ADAMANT_MINT, ITEM_ADAMANT_MINT
	dynmultipush VictoryRoad_1F_Text_RedMint_BOLD_MINT, ITEM_BOLD_MINT
	dynmultipush VictoryRoad_1F_Text_RedMint_BRAVE_MINT, ITEM_BRAVE_MINT
	dynmultipush VictoryRoad_1F_Text_RedMint_CALM_MINT, ITEM_CALM_MINT
	dynmultipush VictoryRoad_1F_Text_RedMint_CAREFUL_MINT, ITEM_CAREFUL_MINT
	dynmultipush VictoryRoad_1F_Text_RedMint_GENTLE_MINT, ITEM_GENTLE_MINT
	dynmultipush VictoryRoad_1F_Text_RedMint_HASTY_MINT, ITEM_HASTY_MINT
	dynmultipush VictoryRoad_1F_Text_RedMint_IMPISH_MINT, ITEM_IMPISH_MINT
	dynmultipush VictoryRoad_1F_Text_RedMint_JOLLY_MINT, ITEM_JOLLY_MINT
	dynmultipush VictoryRoad_1F_Text_RedMint_LAX_MINT, ITEM_LAX_MINT
	dynmultipush VictoryRoad_1F_Text_RedMint_LONELY_MINT, ITEM_LONELY_MINT
	dynmultipush VictoryRoad_1F_Text_RedMint_MILD_MINT, ITEM_MILD_MINT
	dynmultipush VictoryRoad_1F_Text_RedMint_MODEST_MINT, ITEM_MODEST_MINT
	dynmultipush VictoryRoad_1F_Text_RedMint_NAIVE_MINT, ITEM_NAIVE_MINT
	dynmultipush VictoryRoad_1F_Text_RedMint_NAUGHTY_MINT, ITEM_NAUGHTY_MINT
	dynmultipush VictoryRoad_1F_Text_RedMint_QUIET_MINT, ITEM_QUIET_MINT
	dynmultipush VictoryRoad_1F_Text_RedMint_RASH_MINT, ITEM_RASH_MINT
	dynmultipush VictoryRoad_1F_Text_RedMint_RELAXED_MINT, ITEM_RELAXED_MINT
	dynmultipush VictoryRoad_1F_Text_RedMint_SASSY_MINT, ITEM_SASSY_MINT
	dynmultipush VictoryRoad_1F_Text_RedMint_SERIOUS_MINT, ITEM_SERIOUS_MINT
	dynmultipush VictoryRoad_1F_Text_RedMint_TIMID_MINT, ITEM_TIMID_MINT
	dynmultistack 0, 0, FALSE, 8, FALSE, 0, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, VictoryRoad_1F_EventScript_RedRewardChoice
	setorcopyvar VAR_0x8000, VAR_RESULT
	setorcopyvar VAR_0x8001, 1
	call VictoryRoad_1F_EventScript_GiveRewardItemNoRandomize
	setflag FLAG_DEFEATED_RED
	release
	end

@ === Mega Stone name labels ===
VictoryRoad_1F_Text_RedMS_ABOMASITE:
	.string "Abomasite$"

VictoryRoad_1F_Text_RedMS_ABSOLITE:
	.string "Absolite$"

VictoryRoad_1F_Text_RedMS_ABSOLITE_Z:
	.string "Absolite Z$"

VictoryRoad_1F_Text_RedMS_AERODACTYLITE:
	.string "Aerodactylite$"

VictoryRoad_1F_Text_RedMS_AGGRONITE:
	.string "Aggronite$"

VictoryRoad_1F_Text_RedMS_ALAKAZITE:
	.string "Alakazite$"

VictoryRoad_1F_Text_RedMS_ALTARIANITE:
	.string "Altarianite$"

VictoryRoad_1F_Text_RedMS_AMPHAROSITE:
	.string "Ampharosite$"

VictoryRoad_1F_Text_RedMS_AUDINITE:
	.string "Audinite$"

VictoryRoad_1F_Text_RedMS_BANETTITE:
	.string "Banettite$"

VictoryRoad_1F_Text_RedMS_BARBARACITE:
	.string "Barbaracite$"

VictoryRoad_1F_Text_RedMS_BAXCALIBRITE:
	.string "Baxcalibrite$"

VictoryRoad_1F_Text_RedMS_BEEDRILLITE:
	.string "Beedrillite$"

VictoryRoad_1F_Text_RedMS_BLASTOISINITE:
	.string "Blastoisinite$"

VictoryRoad_1F_Text_RedMS_BLAZIKENITE:
	.string "Blazikenite$"

VictoryRoad_1F_Text_RedMS_BUTTERFREEITE:
	.string "Butterfreeite$"

VictoryRoad_1F_Text_RedMS_CAMERUPTITE:
	.string "Cameruptite$"

VictoryRoad_1F_Text_RedMS_CHANDELURITE:
	.string "Chandelurite$"

VictoryRoad_1F_Text_RedMS_CHARIZARDITE_X:
	.string "Charizardite X$"

VictoryRoad_1F_Text_RedMS_CHARIZARDITE_Y:
	.string "Charizardite Y$"

VictoryRoad_1F_Text_RedMS_CHESNAUGHTITE:
	.string "Chesnaughtite$"

VictoryRoad_1F_Text_RedMS_CHIMECHITE:
	.string "Chimechite$"

VictoryRoad_1F_Text_RedMS_CLEFABLITE:
	.string "Clefablite$"

VictoryRoad_1F_Text_RedMS_CRABOMINITE:
	.string "Crabominite$"

VictoryRoad_1F_Text_RedMS_DARKRANITE:
	.string "Darkranite$"

VictoryRoad_1F_Text_RedMS_DELPHOXITE:
	.string "Delphoxite$"

VictoryRoad_1F_Text_RedMS_DIANCITE:
	.string "Diancite$"

VictoryRoad_1F_Text_RedMS_DRAGALGITE:
	.string "Dragalgite$"

VictoryRoad_1F_Text_RedMS_DRAGONINITE:
	.string "Dragoninite$"

VictoryRoad_1F_Text_RedMS_DRAMPANITE:
	.string "Drampanite$"

VictoryRoad_1F_Text_RedMS_EELEKTROSSITE:
	.string "Eelektrossite$"

VictoryRoad_1F_Text_RedMS_EMBOARITE:
	.string "Emboarite$"

VictoryRoad_1F_Text_RedMS_EXCADRITE:
	.string "Excadrite$"

VictoryRoad_1F_Text_RedMS_FALINKSITE:
	.string "Falinksite$"

VictoryRoad_1F_Text_RedMS_FERALIGITE:
	.string "Feraligite$"

VictoryRoad_1F_Text_RedMS_FLOETTITE:
	.string "Floettite$"

VictoryRoad_1F_Text_RedMS_FROSLASSITE:
	.string "Froslassite$"

VictoryRoad_1F_Text_RedMS_GALLADITE:
	.string "Galladite$"

VictoryRoad_1F_Text_RedMS_GARCHOMPITE:
	.string "Garchompite$"

VictoryRoad_1F_Text_RedMS_GARCHOMPITE_Z:
	.string "Garchompite Z$"

VictoryRoad_1F_Text_RedMS_GARDEVOIRITE:
	.string "Gardevoirite$"

VictoryRoad_1F_Text_RedMS_GENGARITE:
	.string "Gengarite$"

VictoryRoad_1F_Text_RedMS_GLALITITE:
	.string "Glalitite$"

VictoryRoad_1F_Text_RedMS_GLIMMORANITE:
	.string "Glimmoranite$"

VictoryRoad_1F_Text_RedMS_GOLISOPITE:
	.string "Golisopite$"

VictoryRoad_1F_Text_RedMS_GOLURKITE:
	.string "Golurkite$"

VictoryRoad_1F_Text_RedMS_GRENINJITE:
	.string "Greninjite$"

VictoryRoad_1F_Text_RedMS_GYARADOSITE:
	.string "Gyaradosite$"

VictoryRoad_1F_Text_RedMS_HAWLUCHANITE:
	.string "Hawluchanite$"

VictoryRoad_1F_Text_RedMS_HEATRANITE:
	.string "Heatranite$"

VictoryRoad_1F_Text_RedMS_HERACRONITE:
	.string "Heracronite$"

VictoryRoad_1F_Text_RedMS_HOUNDOOMINITE:
	.string "Houndoominite$"

VictoryRoad_1F_Text_RedMS_KANGASKHANITE:
	.string "Kangaskhanite$"

VictoryRoad_1F_Text_RedMS_KINGLERITE:
	.string "Kinglerite$"

VictoryRoad_1F_Text_RedMS_LAPRASITE:
	.string "Laprasite$"

VictoryRoad_1F_Text_RedMS_LATIASITE:
	.string "Latiasite$"

VictoryRoad_1F_Text_RedMS_LATIOSITE:
	.string "Latiosite$"

VictoryRoad_1F_Text_RedMS_LOPUNNITE:
	.string "Lopunnite$"

VictoryRoad_1F_Text_RedMS_LUCARIONITE:
	.string "Lucarionite$"

VictoryRoad_1F_Text_RedMS_LUCARIONITE_Z:
	.string "Lucarionite Z$"

VictoryRoad_1F_Text_RedMS_MACHAMPITE:
	.string "Machampite$"

VictoryRoad_1F_Text_RedMS_MAGEARNITE:
	.string "Magearnite$"

VictoryRoad_1F_Text_RedMS_MALAMARITE:
	.string "Malamarite$"

VictoryRoad_1F_Text_RedMS_MANECTITE:
	.string "Manectite$"

VictoryRoad_1F_Text_RedMS_MAWILITE:
	.string "Mawilite$"

VictoryRoad_1F_Text_RedMS_MEDICHAMITE:
	.string "Medichamite$"

VictoryRoad_1F_Text_RedMS_MEGANIUMITE:
	.string "Meganiumite$"

VictoryRoad_1F_Text_RedMS_MEOWSTICITE:
	.string "Meowsticite$"

VictoryRoad_1F_Text_RedMS_METAGROSSITE:
	.string "Metagrossite$"

VictoryRoad_1F_Text_RedMS_PIDGEOTITE:
	.string "Pidgeotite$"

VictoryRoad_1F_Text_RedMS_PINSIRITE:
	.string "Pinsirite$"

VictoryRoad_1F_Text_RedMS_PYROARITE:
	.string "Pyroarite$"

VictoryRoad_1F_Text_RedMS_RAICHUNITE_X:
	.string "Raichunite X$"

VictoryRoad_1F_Text_RedMS_RAICHUNITE_Y:
	.string "Raichunite Y$"

VictoryRoad_1F_Text_RedMS_SABLENITE:
	.string "Sablenite$"

VictoryRoad_1F_Text_RedMS_SALAMENCITE:
	.string "Salamencite$"

VictoryRoad_1F_Text_RedMS_SCEPTILITE:
	.string "Sceptilite$"

VictoryRoad_1F_Text_RedMS_SCIZORITE:
	.string "Scizorite$"

VictoryRoad_1F_Text_RedMS_SCOLIPITE:
	.string "Scolipite$"

VictoryRoad_1F_Text_RedMS_SCOVILLAINITE:
	.string "Scovillainite$"

VictoryRoad_1F_Text_RedMS_SCRAFTINITE:
	.string "Scraftinite$"

VictoryRoad_1F_Text_RedMS_SHARPEDONITE:
	.string "Sharpedonite$"

VictoryRoad_1F_Text_RedMS_SKARMORITE:
	.string "Skarmorite$"

VictoryRoad_1F_Text_RedMS_SLOWBRONITE:
	.string "Slowbronite$"

VictoryRoad_1F_Text_RedMS_SNORLAXITE:
	.string "Snorlaxite$"

VictoryRoad_1F_Text_RedMS_STARAPTITE:
	.string "Staraptite$"

VictoryRoad_1F_Text_RedMS_STARMINITE:
	.string "Starminite$"

VictoryRoad_1F_Text_RedMS_STEELIXITE:
	.string "Steelixite$"

VictoryRoad_1F_Text_RedMS_SWAMPERTITE:
	.string "Swampertite$"

VictoryRoad_1F_Text_RedMS_TATSUGIRINITE:
	.string "Tatsugirinite$"

VictoryRoad_1F_Text_RedMS_TYRANITARITE:
	.string "Tyranitarite$"

VictoryRoad_1F_Text_RedMS_VENUSAURITE:
	.string "Venusaurite$"

VictoryRoad_1F_Text_RedMS_VICTREEBELITE:
	.string "Victreebelite$"

VictoryRoad_1F_Text_RedMS_ZERAORITE:
	.string "Zeraorite$"

@ === Nature Mint name labels ===
VictoryRoad_1F_Text_RedMint_ADAMANT_MINT:
	.string "Adamant Mint$"

VictoryRoad_1F_Text_RedMint_BOLD_MINT:
	.string "Bold Mint$"

VictoryRoad_1F_Text_RedMint_BRAVE_MINT:
	.string "Brave Mint$"

VictoryRoad_1F_Text_RedMint_CALM_MINT:
	.string "Calm Mint$"

VictoryRoad_1F_Text_RedMint_CAREFUL_MINT:
	.string "Careful Mint$"

VictoryRoad_1F_Text_RedMint_GENTLE_MINT:
	.string "Gentle Mint$"

VictoryRoad_1F_Text_RedMint_HASTY_MINT:
	.string "Hasty Mint$"

VictoryRoad_1F_Text_RedMint_IMPISH_MINT:
	.string "Impish Mint$"

VictoryRoad_1F_Text_RedMint_JOLLY_MINT:
	.string "Jolly Mint$"

VictoryRoad_1F_Text_RedMint_LAX_MINT:
	.string "Lax Mint$"

VictoryRoad_1F_Text_RedMint_LONELY_MINT:
	.string "Lonely Mint$"

VictoryRoad_1F_Text_RedMint_MILD_MINT:
	.string "Mild Mint$"

VictoryRoad_1F_Text_RedMint_MODEST_MINT:
	.string "Modest Mint$"

VictoryRoad_1F_Text_RedMint_NAIVE_MINT:
	.string "Naive Mint$"

VictoryRoad_1F_Text_RedMint_NAUGHTY_MINT:
	.string "Naughty Mint$"

VictoryRoad_1F_Text_RedMint_QUIET_MINT:
	.string "Quiet Mint$"

VictoryRoad_1F_Text_RedMint_RASH_MINT:
	.string "Rash Mint$"

VictoryRoad_1F_Text_RedMint_RELAXED_MINT:
	.string "Relaxed Mint$"

VictoryRoad_1F_Text_RedMint_SASSY_MINT:
	.string "Sassy Mint$"

VictoryRoad_1F_Text_RedMint_SERIOUS_MINT:
	.string "Serious Mint$"

VictoryRoad_1F_Text_RedMint_TIMID_MINT:
	.string "Timid Mint$"

