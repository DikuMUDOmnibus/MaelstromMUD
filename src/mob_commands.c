/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
 *  The MOBprograms have been contributed by N'Atas-ha.  Any support for   *
 *  these routines should not be expected from Merc Industries.  However,  *
 *  under no circumstances should the blame for bugs, etc be placed on     *
 *  Merc Industries.  They are not guaranteed to work on all systems due   *
 *  to their frequent use of strxxx functions.  They are also not the most *
 *  efficient way to perform their tasks, but hopefully should be in the   *
 *  easiest possible way to install and begin using. Documentation for     *
 *  such installation can be found in INSTALL.  Enjoy........    N'Atas-Ha *
 ***************************************************************************/

#define unix 1
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"

/*
 * Local functions.
 */

char *			mprog_type_to_name	args( ( int type ) );

/* This routine transfers between alpha and numeric forms of the
 *  mob_prog bitvector types. It allows the words to show up in mpstat to
 *  make it just a hair bit easier to see what a mob should be doing.
 */

char *mprog_type_to_name( int type )
{
	switch ( type )
	{
		case IN_FILE_PROG:          return "in_file_prog";
		case ACT_PROG:              return "act_prog";
		case SPEECH_PROG:           return "speech_prog";
		case RAND_PROG:             return "rand_prog";
		case FIGHT_PROG:            return "fight_prog";
		case HITPRCNT_PROG:         return "hitprcnt_prog";
		case DEATH_PROG:            return "death_prog";
		case ENTRY_PROG:            return "entry_prog";
		case GREET_PROG:            return "greet_prog";
		case ALL_GREET_PROG:        return "all_greet_prog";
		case GIVE_PROG:             return "give_prog";
		case BRIBE_PROG:            return "bribe_prog";
		default:                    return "ERROR_PROG";
	}
}

/* A trivial rehack of do_mstat.  This doesnt show all the data, but just
 * enough to identify the mob and give its basic condition.  It does however,
 * show the MOBprograms which are set.
 */

void do_mpstat( CHAR_DATA *ch, char *argument )
{
	char        buf[ MAX_STRING_LENGTH ];
	char        arg[ MAX_INPUT_LENGTH  ];
	MPROG_DATA *mprg;
	CHAR_DATA  *victim;

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( C_DEFAULT, "MobProg stat whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
		send_to_char( C_DEFAULT,"They aren't here.\n\r", ch );
		return;
	}

	if ( !IS_NPC( victim ) )
	{
		send_to_char(C_DEFAULT, "Only Mobiles can have Programs!\n\r", ch);
		return;
	}

	if ( !( victim->pIndexData->progtypes ) )
	{
		send_to_char(C_DEFAULT, "That Mobile has no Programs set.\n\r", ch);
		return;
	}

	sprintf( buf, "Name: %s.  Vnum: %d.\n\r",
			victim->name, victim->pIndexData->vnum );
	send_to_char(C_DEFAULT, buf, ch );

	sprintf( buf, "Short description: %s.\n\rLong  description: %s",
			victim->short_descr,
			victim->long_descr[0] != '\0' ?
			victim->long_descr : "(none).\n\r" );
	send_to_char(C_DEFAULT, buf, ch );

	sprintf( buf, "Hp: %d/%d(%d).  Mana: %d/%d(%d).  Move: %d/%d(%d).\n\r",
			victim->hit,         victim->perm_hit, MAX_HIT(victim),
			victim->mana,        victim->perm_mana, MAX_MANA(victim),
			victim->move,        victim->perm_move, MAX_MOVE(victim) );
	send_to_char(C_DEFAULT, buf, ch );

#ifdef NEW_MONEY
	sprintf( buf,
			"Lv: %d.  Class: %s.  Align: %d.  AC: %d.  Exp: %d.\n\r",
			victim->level,  class_numbers( victim, FALSE ), victim->alignment,
			GET_AC( victim ),    victim->exp );
	send_to_char(C_DEFAULT, buf, ch );
	sprintf( buf, "Gold: %d   Silver: %d   Copper: %d.\n\r",
			victim->money.gold, victim->money.silver, victim->money.copper );
	send_to_char(C_DEFAULT, buf, ch ); 
#else
	sprintf( buf,
			"Lv: %d.  Class: %s.  Align: %d.  AC: %d.  Gold: %d.  Exp: %d.\n\r",
			victim->level,  class_numbers( victim, FALSE ), victim->alignment,
			GET_AC( victim ),    victim->gold,         victim->exp );
	send_to_char(C_DEFAULT, buf, ch );
#endif
	for ( mprg = victim->pIndexData->mobprogs; mprg != NULL;
			mprg = mprg->next )
	{
		sprintf( buf, ">%s %s\n\r%s\n\r\n\r",
				mprog_type_to_name( mprg->type ),
				mprg->arglist,
				mprg->comlist );
		send_to_char(C_DEFAULT, buf, ch );
	}

	return;

}

/* prints the argument to all the rooms aroud the mobile */

void do_mpasound( CHAR_DATA *ch, char *argument )
{

	ROOM_INDEX_DATA *was_in_room;
	int              door;

	if ( !IS_NPC( ch ) )
	{
		send_to_char(C_DEFAULT, "Huh?\n\r", ch );
		return;
	}

	if ( IS_SET( ch->act , ACT_PET ) || IS_AFFECTED( ch, AFF_CHARM ) )
		return;

	if ( argument[0] == '\0' )
	{
		/*bug( "Mpasound - No argument: vnum %d.", ch->pIndexData->vnum );*/
		sprintf( log_buf, "Mpasound - No argument: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );

		return;
	}

	was_in_room = ch->in_room;
	for ( door = 0; door <= 5; door++ )
	{
		EXIT_DATA       *pexit;

		if ( ( pexit = was_in_room->exit[door] ) != NULL
				&&   pexit->to_room != NULL
				&&   pexit->to_room != was_in_room )
		{
			ch->in_room = pexit->to_room;
			MOBtrigger  = FALSE;
			act( C_DEFAULT, argument, ch, NULL, NULL, TO_ROOM );
		}
	}

	ch->in_room = was_in_room;
	return;

}

/* lets the mobile kill any player or mobile without murder*/

void do_mpkill( CHAR_DATA *ch, char *argument )
{
	char      arg[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;

	if ( !IS_NPC( ch ) )
	{
		send_to_char(C_DEFAULT, "Huh?\n\r", ch );
		return;
	}

	if ( IS_SET( ch->act , ACT_PET ) || IS_AFFECTED( ch, AFF_CHARM ) )
		return;

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		/*bug( "MpKill - no argument: vnum %d.",
		  ch->pIndexData->vnum );*/
		sprintf( log_buf, "MpKill - no argument: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
	}

	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
		/*bug( "MpKill - Victim not in room: vnum %d.",
		  ch->pIndexData->vnum );*/
		sprintf( log_buf, "MpKill - Victim not in room: vnum %d name %s short %s .",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	if ( victim == ch )
	{
		/*bug( "MpKill - Bad victim to attack: vnum %d.",
		  ch->pIndexData->vnum );*/
		sprintf( log_buf, "MpKill - Bad victim to attack: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
	{
		/*bug( "MpKill - Charmed mob attacking master: vnum %d.",
		  ch->pIndexData->vnum );*/
		sprintf( log_buf, "MpKill - Charmed mob attacking master: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	if ( ch->position == POS_FIGHTING )
	{	
		/*bug( "MpKill - Already fighting: vnum %d",
		  ch->pIndexData->vnum );*/
		sprintf( log_buf, "MpKill - Already fighting: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	multi_hit( ch, victim, TYPE_UNDEFINED );
	return;
}


/* lets the mobile destroy an object in its inventory
   it can also destroy a worn object and it can destroy 
   items using all.xxxxx or just plain all of them */

void do_mpjunk( CHAR_DATA *ch, char *argument )
{
	char      arg[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	if ( !IS_NPC( ch ) )
	{
		send_to_char(C_DEFAULT, "Huh?\n\r", ch );
		return;
	}

	if ( IS_SET( ch->act , ACT_PET ) || IS_AFFECTED( ch, AFF_CHARM ) )
		return;

	one_argument( argument, arg );

	if ( arg[0] == '\0')
	{
		/*bug( "Mpjunk - No argument: vnum %d.", ch->pIndexData->vnum );*/
		sprintf( log_buf, "Mpjunk - No argument: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
	{
		if ( ( obj = get_obj_wear( ch, arg ) ) != NULL )
		{
			unequip_char( ch, obj );
			extract_obj( obj );
			return;
		}
		if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
			return; 
		extract_obj( obj );
	}
	else
		for ( obj = ch->carrying; obj != NULL; obj = obj_next )
		{
			obj_next = obj->next_content;
			if ( arg[3] == '\0' || is_name( ch, &arg[4], obj->name ) )
			{
				if ( obj->wear_loc != WEAR_NONE)
					unequip_char( ch, obj );
				extract_obj( obj );
			} 
		}

	return;

}

/* prints the message to everyone in the room other than the mob and victim */

void do_mpechoaround( CHAR_DATA *ch, char *argument )
{
	char       arg[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;

	if ( !IS_NPC( ch ) )
	{
		send_to_char(C_DEFAULT, "Huh?\n\r", ch );
		return;
	}

	if ( IS_SET( ch->act , ACT_PET ) || IS_AFFECTED( ch, AFF_CHARM ) )
		return;

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		/*bug( "Mpechoaround - No argument:  vnum %d.", ch->pIndexData->vnum);*/
		sprintf( log_buf, "Mpechoaround - No argument: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	if ( !( victim=get_char_room( ch, arg ) ) )
	{
		/*bug( "Mpechoaround - victim does not exist: vnum %d.",
		  ch->pIndexData->vnum );*/
		sprintf( log_buf, "Mpechoaround - victim does not exist: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	act(C_DEFAULT,  argument, ch, NULL, victim, TO_NOTVICT );
	return;
}

/* prints the message to only the victim */

void do_mpechoat( CHAR_DATA *ch, char *argument )
{
	char       arg[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;

	if ( !IS_NPC( ch ) )
	{
		send_to_char(C_DEFAULT, "Huh?\n\r", ch );
		return;
	}

	if ( IS_SET( ch->act , ACT_PET ) || IS_AFFECTED( ch, AFF_CHARM ) )
		return;

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' || argument[0] == '\0' )
	{
		/*bug( "Mpechoat - No argument:  vnum %d.",
		  ch->pIndexData->vnum );*/
		sprintf( log_buf, "Mpechoat - No argument: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	if ( !( victim = get_char_room( ch, arg ) ) )
	{
		/*        bug( "Mpechoat - victim does not exist: vnum %d.",
				  ch->pIndexData->vnum ); */
		sprintf( log_buf, "Mpechoat - victim does not exist: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	act(C_DEFAULT,  argument, ch, NULL, victim, TO_VICT );
	return;
}

/* prints the message to the room at large */

void do_mpecho( CHAR_DATA *ch, char *argument )
{
	if ( !IS_NPC(ch) )
	{
		send_to_char(C_DEFAULT,  "Huh?\n\r", ch );
		return;
	}

	if ( IS_SET( ch->act , ACT_PET ) || IS_AFFECTED( ch, AFF_CHARM ) )
		return;

	if ( argument[0] == '\0' )
	{
		/*bug( "Mpecho - called w/o argument: vnum %d.",
		  ch->pIndexData->vnum );*/
		sprintf( log_buf, "Mpecho - called w/o argument: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	act(C_DEFAULT,  argument, ch, NULL, NULL, TO_ROOM );
	return;

}

/* lets the mobile load an item or mobile.  All items
   are loaded into inventory.  you can specify a level with
   the load object portion as well. */

void do_mpmload( CHAR_DATA *ch, char *argument )
{
	char            arg[ MAX_INPUT_LENGTH ];
	MOB_INDEX_DATA *pMobIndex;
	CHAR_DATA      *victim;

	if ( !IS_NPC( ch ) )
	{
		send_to_char( C_DEFAULT, "Huh?\n\r", ch );
		return;
	}

	if ( IS_SET( ch->act , ACT_PET ) || IS_AFFECTED( ch, AFF_CHARM ) )
		return;

	one_argument( argument, arg );

	if ( arg[0] == '\0' || !is_number(arg) )
	{
		/*bug( "Mpmload - Bad vnum as arg: vnum %d.", ch->pIndexData->vnum );*/
		sprintf( log_buf, "Mpmload - Bad vnum as arg: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	if ( ( pMobIndex = get_mob_index( atoi( arg ) ) ) == NULL )
	{
		/*bug( "Mpmload - Bad mob vnum: vnum %d.", ch->pIndexData->vnum );*/
		sprintf( log_buf, "Mpmload - Bad mob vnum: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	victim = create_mobile( pMobIndex );
	char_to_room( victim, ch->in_room );
	return;
}

void do_mpoload( CHAR_DATA *ch, char *argument )
{
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	OBJ_INDEX_DATA *pObjIndex;
	OBJ_DATA       *obj;
	int             level;

	if ( !IS_NPC( ch ) )
	{
		send_to_char( C_DEFAULT, "Huh?\n\r", ch );
		return;
	}

	if ( IS_SET( ch->act , ACT_PET ) || IS_AFFECTED( ch, AFF_CHARM ) )
		return;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[0] == '\0' || !is_number( arg1 ) )
	{
		/*bug( "Mpoload - Bad syntax: vnum %d.",
		  ch->pIndexData->vnum );*/
		sprintf( log_buf, "Mpoload - Bad syntax: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
	{
		/*bug( "Mpoload - Bad vnum arg: vnum %d.", ch->pIndexData->vnum );*/
		sprintf( log_buf, "Mpoload - Bad vnum arg: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}


	if ( arg2[0] == '\0' )
	{
		level = pObjIndex->level;
		/* level = get_trust( ch ); */
	}
	else
	{
		/*
		 * New feature from Alander.
		 */
		if ( !is_number( arg2 ) )
		{
			/*bug( "Mpoload - Bad syntax: vnum %d.", ch->pIndexData->vnum );*/
			sprintf( log_buf, "Mpoload - Bad syntax: vnum %d name %s short %s.",
					ch->pIndexData->vnum, ch->name, ch->short_descr );
			bug( log_buf, -1 );
			return;
		}
		level = atoi( arg2 );
		if ( level < 0 || level > get_trust( ch ) )
		{
			/*bug( "Mpoload - Bad level: vnum %d.", ch->pIndexData->vnum );*/
			sprintf( log_buf, "Mpoload - Bad level: vnum %d name %s short %s.",
					ch->pIndexData->vnum, ch->name, ch->short_descr );
			bug( log_buf, -1 );
			return;
		}
	}

	obj = create_object( pObjIndex, level );
	if ( CAN_WEAR(obj, ITEM_TAKE) )
	{
		obj_to_char( obj, ch );
	}
	else
	{
		obj_to_room( obj, ch->in_room );
	}

	return;
}

/* lets the mobile purge all objects and other npcs in the room,
   or purge a specified object or mob in the room.  It can purge
   itself, but this had best be the last command in the MOBprogram
   otherwise ugly stuff will happen */

void do_mppurge( CHAR_DATA *ch, char *argument )
{
	char       arg[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;
	OBJ_DATA  *obj;

	if ( !IS_NPC( ch ) )
	{
		send_to_char( C_DEFAULT, "Huh?\n\r", ch );
		return;
	}

	if ( IS_SET( ch->act , ACT_PET ) || IS_AFFECTED( ch, AFF_CHARM ) )
		return;

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		/* 'purge' */
		CHAR_DATA *vnext;
		OBJ_DATA  *obj_next;

		for ( victim = ch->in_room->people; victim != NULL; victim = vnext )
		{
			vnext = victim->next_in_room;
			if ( IS_NPC( victim ) && victim != ch )
				extract_char( victim, TRUE );
		}

		for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
		{
			obj_next = obj->next_content;
			extract_obj( obj );
		}

		return;
	}

	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
		if ( ( obj = get_obj_here( ch, arg ) ) )
		{
			extract_obj( obj );
		}
		else
		{
			/*bug( "Mppurge - Bad argument: vnum %d.", ch->pIndexData->vnum );*/
			sprintf( log_buf, "Mppurge - Bad argument: vnum %d name %s short %s.",
					ch->pIndexData->vnum, ch->name, ch->short_descr );
			bug( log_buf, -1 );
		}
		return;
	}

	if ( !IS_NPC( victim ) )
	{
		/*bug( "Mppurge - Purging a PC: vnum %d.", ch->pIndexData->vnum );*/
		sprintf( log_buf, "Mppurge - Purging a PC: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	extract_char( victim, TRUE );
	return;
}


/* lets the mobile goto any location it wishes that is not private */

void do_mpgoto( CHAR_DATA *ch, char *argument )
{
	char             arg[ MAX_INPUT_LENGTH ];
	ROOM_INDEX_DATA *location;

	if ( !IS_NPC( ch ) )
	{
		send_to_char( C_DEFAULT, "Huh?\n\r", ch );
		return;
	}

	if ( IS_SET( ch->act , ACT_PET ) || IS_AFFECTED( ch, AFF_CHARM ) )
		return;

	one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		/*bug( "Mpgoto - No argument: vnum %d.", ch->pIndexData->vnum );*/
		sprintf( log_buf, "Mpgoto - No argument: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	if ( ( location = find_location( ch, arg ) ) == NULL )
	{
		/*bug( "Mpgoto - No such location: vnum %d.", ch->pIndexData->vnum );*/
		sprintf( log_buf, "Mpgoto - No such location: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	if ( location == ch->in_room )
		return;

	if ( ch->fighting != NULL )
		stop_fighting( ch, TRUE );

	char_from_room( ch );
	char_to_room( ch, location );

	return;
}

/* lets the mobile do a command at another location. Very useful */

void do_mpat( CHAR_DATA *ch, char *argument )
{
	char             arg[ MAX_INPUT_LENGTH ];
	ROOM_INDEX_DATA *location;
	ROOM_INDEX_DATA *original;
	CHAR_DATA       *wch;

	if ( !IS_NPC( ch ) )
	{
		send_to_char( C_DEFAULT, "Huh?\n\r", ch );
		return;
	}

	if ( IS_SET( ch->act , ACT_PET ) || IS_AFFECTED( ch, AFF_CHARM ) )
		return;

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' || argument[0] == '\0' )
	{
		/*bug( "Mpat - Bad argument: vnum %d.", ch->pIndexData->vnum );*/
		sprintf( log_buf, "Mpat - Bad argument: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	if ( ( location = find_location( ch, arg ) ) == NULL )
	{
		/*bug( "Mpat - No such location: vnum %d.", ch->pIndexData->vnum );*/
		sprintf( log_buf, "Mpat - No such location: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	original = ch->in_room;
	char_from_room( ch );
	char_to_room( ch, location );
	interpret( ch, argument );

	/*
	 * See if 'ch' still exists before continuing!
	 * Handles 'at XXXX quit' case.
	 */
	for ( wch = char_list; wch != NULL; wch = wch->next )
	{
		if ( wch == ch )
		{
			char_from_room( ch );
			char_to_room( ch, original );
			break;
		}
	}

	return;
}

/* lets the mobile transfer people.  the all argument transfers
   everyone in the current room to the specified location 
CORRECTION: ALL transfers all in game */

void do_mptransfer( CHAR_DATA *ch, char *argument )
{
	char             arg1[ MAX_INPUT_LENGTH ];
	char             arg2[ MAX_INPUT_LENGTH ];
	ROOM_INDEX_DATA *location;
	DESCRIPTOR_DATA *d;
	CHAR_DATA       *victim;

	if ( !IS_NPC( ch ) )
	{
		send_to_char( C_DEFAULT, "Huh?\n\r", ch );
		return;
	}

	if ( IS_SET( ch->act , ACT_PET ) || IS_AFFECTED( ch, AFF_CHARM ) )
		return;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[0] == '\0' )
	{
		/*bug( "Mptransfer - Bad syntax: vnum %d.", ch->pIndexData->vnum );*/
		sprintf( log_buf, "Mptransfer - Bad syntax: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	if ( !str_cmp( arg1, "all" ) )
	{
		for ( d = descriptor_list; d != NULL; d = d->next )
		{
			if ( d->connected == CON_PLAYING
					&&   d->character != ch
					&&   d->character->in_room != NULL
					&&   can_see( ch, d->character ) )
			{
				char buf[MAX_STRING_LENGTH];
				sprintf( buf, "%s %s", d->character->name, arg2 );
				do_transfer( ch, buf );
			}
		}
		return;
	}

	if ( !str_cmp( arg1, "room" ) )
	{
		for ( d = descriptor_list; d != NULL; d = d->next )
		{
			if ( d->connected == CON_PLAYING
					&&   d->character != ch
					&&   d->character->in_room != NULL
					&&   can_see( ch, d->character ) 
					&&   d->character->in_room == ch->in_room )
			{
				char buf[MAX_STRING_LENGTH];
				sprintf( buf, "%s %s", d->character->name, arg2 );
				do_transfer( ch, buf );
			}
		}
		return;
	}

	/*
	 * Thanks to Grodyn for the optional location parameter.
	 */
	if ( arg2[0] == '\0' )
	{
		location = ch->in_room;
	}
	else
	{
		if ( ( location = find_location( ch, arg2 ) ) == NULL )
		{
			/*bug( "Mptransfer - No such location: vnum %d.",
			  ch->pIndexData->vnum );*/
			sprintf( log_buf, "Mptransfer - No such location: vnum %d name %s short %s.",
					ch->pIndexData->vnum, ch->name, ch->short_descr );
			bug( log_buf, -1 );
			return;
		}

		if ( room_is_private( location ) )
		{
			/*	    bug( "Mptransfer - Private room: vnum %d.",
					ch->pIndexData->vnum );*/
			sprintf( log_buf, "Mptransfer - Private room: vnum %d name %s short %s.",
					ch->pIndexData->vnum, ch->name, ch->short_descr );
			bug( log_buf, -1 );
			return;
		}
	}

	if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
	{
		/*bug( "Mptransfer - No such person: vnum %d.",
		  ch->pIndexData->vnum );*/
		sprintf( log_buf, "Mptransfer - No such person: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	if ( victim->in_room == NULL )
	{
		/*bug( "Mptransfer - Victim in Limbo: vnum %d.",
		  ch->pIndexData->vnum );*/
		sprintf( log_buf, "Mptransfer - Victim in Limbo: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	if ( victim->in_room == location )
		return;

	if ( victim->fighting != NULL )
		stop_fighting( victim, TRUE );

	char_from_room( victim );
	char_to_room( victim, location );

	return;
}

/* lets the mobile force someone to do something.  must be mortal level
   and the all argument only affects those in the room with the mobile */

void do_mpforce( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];

	if ( !IS_NPC( ch ) )
	{
		send_to_char( C_DEFAULT, "Huh?\n\r", ch );
		return;
	}

	if ( IS_SET( ch->act , ACT_PET ) || IS_AFFECTED( ch, AFF_CHARM ) )
		return;

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' || argument[0] == '\0' )
	{
		/*bug( "Mpforce - Bad syntax: vnum %d.", ch->pIndexData->vnum );*/
		sprintf( log_buf, "Mpforce - Bad syntax: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	if ( !str_cmp( arg, "all" ) )
	{
		CHAR_DATA *vch;
		CHAR_DATA *vch_next;

		for ( vch = char_list; vch != NULL; vch = vch_next )
		{
			vch_next = vch->next;

			if ( vch->in_room == ch->in_room
					&& get_trust( vch ) < get_trust( ch ) 
					&& can_see( ch, vch ) )
			{
				interpret( vch, argument );
			}
		}
	}
	else
	{
		CHAR_DATA *victim;

		if ( ( victim = get_char_room( ch, arg ) ) == NULL )
		{
			/*bug( "Mpforce - No such victim: vnum %d.", 
			  ch->pIndexData->vnum );*/
			sprintf( log_buf, "Mpforce - No such victim: vnum %d name %s short %s.",
					ch->pIndexData->vnum, ch->name, ch->short_descr );
			bug( log_buf, -1 );
			return;
		}

		if ( victim == ch )
		{
			/*bug( "Mpforce - Forcing oneself: vnum %d.",
			  ch->pIndexData->vnum );*/
			sprintf( log_buf, "Mpforce - Forcing oneself: vnum %d name %s short %s.",
					ch->pIndexData->vnum, ch->name, ch->short_descr );
			bug( log_buf, -1 );
			return;
		}

		interpret( victim, argument );
	}

	return;
}

/* Specialized teleport, only teleports to another room in SAME area.
 * -- Altrag */
void do_mpteleport( CHAR_DATA *ch, char *argument )
{
	int vnum;
	ROOM_INDEX_DATA *to_room = NULL;
	AREA_DATA *pArea;

	if ( !IS_NPC(ch) )
	{
		send_to_char(C_DEFAULT, "Huh?\n\r",ch);
		return;
	}

	if ( IS_SET( ch->act , ACT_PET ) || IS_AFFECTED( ch, AFF_CHARM ) )
		return;

	pArea = ch->in_room->area;

	if ( !pArea )
	{
		/*bug( "Mpteleport: not in area - %d.", ch->pIndexData->vnum );*/
		sprintf( log_buf, "Mpteleport - not in area: vnum %d name %s short %s.",
				ch->pIndexData->vnum, ch->name, ch->short_descr );
		bug( log_buf, -1 );
		return;
	}

	/* Stop attempts that take forever.. -- Altrag */
	for ( vnum = 0; vnum < (pArea->uvnum - pArea->lvnum) / 2; vnum++ )
	{
		int to_vnum;

		to_vnum = number_range( pArea->lvnum, pArea->uvnum );
		if ( ( to_room = get_room_index( to_vnum ) ) != NULL )
			break;
	}

	if ( !to_room )
		return;

	char_from_room(ch);
	char_to_room(ch, to_room);
	return;
}

