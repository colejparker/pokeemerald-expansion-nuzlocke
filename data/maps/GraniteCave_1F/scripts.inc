GraniteCave_1F_MapScripts::
	.byte 0

GraniteCave_1F_EventScript_Hiker::
	lock
	faceplayer
	msgbox GraniteCave_1F_Text_ExplainFlash, MSGBOX_DEFAULT
	release
	end

GraniteCave_1F_Text_ExplainFlash:
	.string "It gets awfully dark ahead.\n"
	.string "It'll be tough trying to explore.\p"
	.string "You'll want a FLASH TOOL to light\n"
	.string "up the darkness.\p"
	.string "I hear DEWFORD's GYM LEADER hands\n"
	.string "them out.$"

