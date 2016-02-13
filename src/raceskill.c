/*****************************************************************************
 * All the racial skills		                                     *
 * -- Hannibal                                                               *
 *****************************************************************************/

#define unix 1
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"
/*
 * External functions.
 */
extern void    set_fighting     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
extern void    forge_obj	args( ( CHAR_DATA *ch, OBJ_DATA *to_forge ) );

void do_forge( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char buf[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj, *hammer;
	int wear, lvl;
	long group1 = ITEM_ANTI_HUMAN + ITEM_ANTI_ELF;
	long group2 = ITEM_ANTI_DWARF + ITEM_ANTI_HALFLING + ITEM_ANTI_GNOME;
	long antirace = 0;
	wear = 0;
	if ( ch->race != RACE_DWARF )
	{
		send_to_char( AT_GREY, "You aren't a dwarf!\n\r", ch );
		return;
	}
	if ( argument[0] == '\0' )
	{
		send_to_char( AT_WHITE, "Syntax: Forge <obj> <race> <lvl>\n\r", ch );
		send_to_char( AT_WHITE, "  obj = ring necklace armor helm\n\r", ch );
		send_to_char( AT_WHITE, "        mask leggings boots gauntlets\n\r", ch );
		send_to_char( AT_WHITE, "        gauntlets armplates shield\n\r", ch );
		send_to_char( AT_WHITE, "        belt bracer anklet weapon\n\r", ch );
		send_to_char( AT_WHITE, "  race= any valid race. HELP FORGE RACES\n\r", ch );
		send_to_char( AT_WHITE, "        to see race groupings.\n\r", ch );
		sprintf( buf, "  lvl = minimum 30, maximum %d.\n\r", ch->level );
		send_to_char( AT_WHITE, buf, ch );
		send_to_char( AT_WHITE, "  BASE cost to make item is: 100 gold * lvl\n\r", ch );
		return;
	}
	if ( ch->in_room->vnum != ROOM_VNUM_SMITHY )
	{
		send_to_char( AT_GREY, "You cannot forge equipment here.\n\r", ch );
		return;
	}
	for ( hammer = ch->carrying; hammer; hammer = hammer->next )
	{
		if ( hammer->pIndexData->vnum == OBJ_VNUM_SMITHY_HAMMER
				&& hammer->wear_loc == WEAR_HOLD )
			break;
	}
	if ( !hammer )
	{
		send_to_char( AT_GREY, "You must hold a smithy hammer to forge something.\n\r", ch );  
		return;
	}
	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg2 );
	if ( !str_prefix( arg, "ring" ) )
		wear = ITEM_WEAR_FINGER;
	if ( !str_prefix( arg, "necklace" ) )
		wear = ITEM_WEAR_NECK;
	if ( !str_prefix( arg, "armor" ) )
		wear = ITEM_WEAR_BODY;
	if ( !str_prefix( arg, "helm" ) )
		wear = ITEM_WEAR_HEAD;
	if ( !str_prefix( arg, "mask" ) )
		wear = ITEM_WEAR_FACE;
	if ( !str_prefix( arg, "leggings" ) )
		wear = ITEM_WEAR_LEGS;
	if ( !str_prefix( arg, "boots" ) )
		wear = ITEM_WEAR_FEET;
	if ( !str_prefix( arg, "gauntlets" ) )
		wear = ITEM_WEAR_HANDS;
	if ( !str_prefix( arg, "armplates" ) )
		wear = ITEM_WEAR_ARMS;
	if ( !str_prefix( arg, "shield" ) )
		wear = ITEM_WEAR_SHIELD;
	if ( !str_prefix( arg, "belt" ) )
		wear = ITEM_WEAR_WAIST;
	if ( !str_prefix( arg, "bracer" ) )
		wear = ITEM_WEAR_WRIST;
	if ( !str_prefix( arg, "anklet" ) )
		wear = ITEM_WEAR_ANKLE;
	if ( !str_prefix( arg, "weapon" ) )
		wear = ITEM_WIELD;
	if ( !str_prefix( arg2, "elf" ) || !str_prefix( arg2, "human" ) )
		antirace = group2;
	if ( !str_prefix( arg2, "dwarf" ) || !str_prefix( arg2, "halfling" ) || !str_prefix( arg2, "gnome" ) )
		antirace = group1;
	if ( is_number( argument ) )
		lvl = atoi( argument );	
	else
		lvl = 0;
	if ( wear && antirace && ( lvl < 30 || lvl > ch->level ) )
	{
		sprintf( buf, "Illegal level.  Valid levels are 30 to %d.\n\r", ch->level );
		send_to_char( AT_GREY, buf, ch );
		return;
	}
	if ( wear && antirace == 0 )
	{
		send_to_char( AT_GREY, "Illegal race.  Help RACE for valid race list.\n\r", ch );
		return;
	}
	if ( wear )
	{
		if ( (ch->money.gold + (ch->money.silver/SILVER_PER_GOLD) +
					(ch->money.copper/COPPER_PER_GOLD) ) < (lvl * 100) )
		{
				send_to_char( AT_GREY, "You do not have enough money to create the base item of this level.\n\r", ch );
				return;
			}
			else
				if ( wear == ITEM_WIELD )
					obj = create_object( get_obj_index( OBJ_VNUM_TO_FORGE_W ), lvl );
				else
					obj = create_object( get_obj_index( OBJ_VNUM_TO_FORGE_A ), lvl );
			obj->cost.silver = obj->cost.copper = 0;
			obj->cost.gold = lvl * 100;
			obj->weight = lvl * 0.15;
			obj->level = lvl;
			obj->anti_race_flags = antirace;
			if ( obj->level >= 101 )
				obj->extra_flags += ITEM_NO_DAMAGE;
			obj->wear_flags += wear;
			forge_obj( ch, obj );
		}
		else
			do_forge( ch, "" );
		return;
	}