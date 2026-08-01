@ Shared move-tutor box-selection framework, still referenced by src/chooseboxmon.c's
@ SELECT_PC_MON_MOVE_TUTOR table entry even though no in-game NPC uses the move_tutor macro anymore.
MoveTutor_EventScript_OpenPartyMenu::
	special ChooseMonForMoveTutor
	lock
	faceplayer
	return

MoveTutor_EventScript_OpenBox::
	chooseboxmon SELECT_PC_MON_MOVE_TUTOR
	waitstate
	lock
	faceplayer
	return

MoveTutor_EventScript_CanOnlyBeLearnedOnce::
	msgbox MoveTutor_Text_ThisMoveCanOnlyBeLearnedOnce, MSGBOX_YESNO
	return

MoveTutor_AfterChooseBoxMon::
	waitstate
	special CanTeachMoveBoxMon
	waitstate
	return
