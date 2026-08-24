Underwater_SealedChamber_MapScripts::
	map_script MAP_SCRIPT_ON_DIVE_WARP, Underwater_SealedChamber_OnDive
	.byte 0

Underwater_SealedChamber_OnDive:
	goto Underwater_SealedChamber_EventScript_SurfaceRoute134

Underwater_SealedChamber_EventScript_SurfaceRoute134::
	setdivewarp MAP_ROUTE134, 60, 31
	end

Underwater_SealedChamber_EventScript_SurfaceSealedChamber::
	setdivewarp MAP_SEALED_CHAMBER_OUTER_ROOM, 10, 19
	end

Underwater_SealedChamber_EventScript_Braille::
	lockall
	braillemsgbox Underwater_SealedChamber_Braille_GoUpHere
	releaseall
	end

