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

void do_drowfire( CHAR_DATA *ch, char *argument )
{
	AFFECT_DATA af;
	CHAR_DATA   *victim;

	if ( ch->race != RACE_DROW )
	{
		send_to_char( AT_GREY, "Drowfire by a non-drow?\n\r", ch );
		return;
	}

	if(argument[0] != '\0')
	{
		if(!(victim = get_char_room(ch, argument)))
		{
			send_to_char(C_DEFAULT, "They aren't here.\n\r", ch);
			return;
		}
	}
	else
	{
		if(!(victim = ch->fighting))
		{
			send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch);
			return;
		}
	}
	if ( is_affected( victim, gsn_drowfire ) )
		return;

	WAIT_STATE( ch, skill_table[gsn_drowfire].beats );
	af.type	= gsn_drowfire;
	af.level	= ch->level;
	af.duration	= ch->level / 10;
	af.location	= APPLY_AC;
	af.modifier	= 5 * ch->level;
	af.bitvector	= 0;
	affect_to_char( victim, &af );
	af.location	= APPLY_HITROLL;
	af.modifier	= 0 - ch->level / 5;
	af.bitvector	= 0;
	affect_to_char( victim, &af );

	send_to_char(AT_PINK, "You are surrounded by a purple outline.\n\r", victim );
	act(AT_PINK, "$n is surrounded by a purple outline.", victim, NULL, NULL, TO_ROOM );
	if ( !victim->fighting )
		set_fighting( victim, ch );

	return;
}

void do_globedarkness( CHAR_DATA *ch, char *argument )
{
	ROOM_AFFECT_DATA raf;
	POWERED_DATA *pd;
	OBJ_DATA *globe;
	char buf[MAX_STRING_LENGTH];
	bool found = FALSE;
	if ( ch->race != RACE_DROW )
	{
		send_to_char(AT_GREY, "Huh?\n\r", ch );
		return;
	}
	if ( argument[0] == '\0' )
	{
		send_to_char( AT_DGREY, "Syntax: globe <create|dissipate|locate>\n\r", ch );
		return;
	}
	if ( !str_cmp( argument, "locate" ) )
	{
		for ( pd = ch->powered; pd; pd = pd->next )
		{
			if ( pd->type == gsn_globedark )
			{
				sprintf( buf, "Globe of Darkness&w, &W%s&w; &cCost&w: &R%d&w.\n\r", 
						pd->room->name, pd->cost );
				send_to_char( AT_DGREY, buf, ch );
				found = TRUE;
			}
		}
		if ( !found )
			send_to_char( AT_CYAN, "You are not sustaining any &zGlobes&w.\n\r", ch );
		return;
	}
	if ( !str_cmp( argument, "dissipate" ) )
	{
		if ( !is_raffected( ch->in_room, gsn_globedark ) )
		{
			send_to_char( AT_CYAN, "There is no &zGlobe &cin this room&w.\n\r", ch );
			return;
		}
		for ( pd = ch->powered; pd; pd = pd->next )
		{
			if ( !pd )
				break;
			if ( pd->type == gsn_globedark )
			{
				found = TRUE;
				if ( pd->room == ch->in_room )
				{
					send_to_char( AT_DGREY, "You wave your hand and the globe dissipates.\n\r", ch );
					act( AT_DGREY, "The globe of darkness dissipates.", 
							ch, NULL, NULL, TO_ROOM );
					raffect_remove( ch->in_room, ch, pd->raf );
					return;
				}
			}
		}
		if ( !found )
		{
			send_to_char( AT_CYAN, "You are not sustaining any &zGlobes&w.\n\r", ch );
			return;
		}
		send_to_char( AT_CYAN, "You are not powering the &zGlobe&c in this room.\n\r", ch ); 
		return;
	}
	if ( !str_cmp( argument, "create" ) )
	{
		if ( is_raffected( ch->in_room, gsn_globedark ) )
		{
			send_to_char(AT_GREY, "A globe of darkness already sits in this room.\n\r", ch );
			return;
		}
		globe = create_object( get_obj_index( OBJ_VNUM_WARD_PHYS ), 1 );
		free_string( globe->name );
		free_string( globe->short_descr );
		free_string( globe->description );
		globe->name = str_dup( "globe darkness" );
		globe->short_descr = str_dup( "&za Globe of Darkness" );
		globe->description = str_dup( "A &zGlobe of Darkness &Xsits upon the room." );
		raf.type	= gsn_globedark;
		raf.location	= ROOM_NONE;
		raf.room	= ch->in_room;
		raf.material	= globe;
		raf.powered_by = ch;
		raffect_to_room( ch->in_room, ch, &raf );
		send_to_char( AT_DGREY, "You raise your hand and place of globe of darkness upon the area!\n\r", ch );
		act( AT_DGREY, "$n raises his hand and everything goes pitch black!",
				ch, NULL, NULL, TO_ROOM );
		return;
	}
	do_globedarkness( ch, "" );
}
void do_forge( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char buf[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj, *hammer;
	int wear, lvl;
	long group1 = ITEM_ANTI_HUMAN + ITEM_ANTI_DROW + ITEM_ANTI_ELF;
	long group2 = ITEM_ANTI_DWARF + ITEM_ANTI_HALFLING;
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
	if ( !str_prefix( arg2, "elf" ) || !str_prefix( arg2, "drow" ) || !str_prefix( arg2, "elder" ) || !str_prefix( arg2, "human" ) )
		antirace = group2;
	if ( !str_prefix( arg2, "dwarf" ) || !str_prefix( arg2, "halfling" ) )
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