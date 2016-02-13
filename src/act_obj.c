/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  Envy Diku Mud improvements copyright (C) 1994 by Michael Quan, David   *
 *  Love, Guilherme 'Willie' Arnold, and Mitchell Tse.                     *
 *                                                                         *
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#define unix 1
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


/*
 * External functions.
 */
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing );

/*
 * Local functions.
 */
void	get		args( ( CHAR_DATA *ch, char *argument, bool palming ) );
bool	get_obj		args( ( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container, bool palming ) );
bool	remove_obj	args( ( CHAR_DATA *ch, int iWear, bool fReplace ) );
void obj_random_apply args(( OBJ_DATA *obj, int total, char *buf ));
void	wear_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace ) );
CHAR_DATA *	find_keeper	args( ( CHAR_DATA *ch ) );

MONEY_DATA *get_cost    args( ( CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy ) );

void    do_acoload      args( ( CHAR_DATA *ch, OBJ_DATA *obj, int vnum ) );
void    do_acmload      args( ( CHAR_DATA *ch, OBJ_DATA *obj, int vnum ) );
void    do_actrans      args( ( CHAR_DATA *ch, OBJ_DATA *obj, int vnum ) );
void    do_acmorph      args( ( CHAR_DATA *ch, OBJ_DATA *obj, int vnum ) );



bool get_obj( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container, bool palming ) {
	MONEY_DATA amount;
	/*  int tot_coins; */

	if ( !CAN_WEAR( obj, ITEM_TAKE ) && obj->item_type != ITEM_CORPSE_PC )
	{
		send_to_char(AT_WHITE, "You can't take that.\n\r", ch );
		oprog_get_trigger( obj, ch ); /* So items w/o take flag can 
										 still have obj progs */
		return 0;
	}

	if ( obj->item_type == ITEM_CORPSE_PC && get_trust(ch) < LEVEL_IMMORTAL )
	{
		send_to_char(AT_WHITE, "You can't take that.\n\r", ch );
		return 0;
	}

	if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
	{
		act(AT_WHITE, "$d: you can't carry that many items.",
				ch, NULL, obj->name, TO_CHAR );
		return 0;
	}

	if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
	{
		act(AT_WHITE, "$d: you can't carry that much weight.",
				ch, NULL, obj->name, TO_CHAR );
		return 0;
	}

	if ( container )
	{   if (!palming)
		{	
			act(AT_GREEN, "You get $p from $P.", ch, obj, container, TO_CHAR );
			act(AT_GREEN, "$n gets $p from $P.", ch, obj, container, TO_ROOM );
		}
		if ( palming == 1 )
			act(AT_GREEN, "You palm $p from $P.", ch, obj, container, TO_CHAR );
		oprog_get_from_trigger( obj, ch, container );
		obj_from_obj( obj );
		if ( ch->in_room->vnum == ROOM_VNUM_DON_1
				|| ch->in_room->vnum == ROOM_VNUM_DON_2
				|| ch->in_room->vnum == ROOM_VNUM_DON_3
				|| ch->in_room->vnum == ROOM_VNUM_DON_4
				|| obj->item_type == ITEM_CORPSE_PC )
			obj->timer = -1;
	}
	else
	{
		if ( !palming ) 
		{
			act(AT_GREEN, "You get $p.", ch, obj, container, TO_CHAR );
			act(AT_GREEN, "$n gets $p.", ch, obj, container, TO_ROOM );
		}
		if ( palming == 1 ) 
			act(AT_GREEN, "You palm $p.", ch, obj, container, TO_CHAR );
		oprog_get_trigger( obj, ch );
		obj_from_room( obj );
		if ( ch->in_room->vnum == ROOM_VNUM_DON_1
				|| ch->in_room->vnum == ROOM_VNUM_DON_2
				|| ch->in_room->vnum == ROOM_VNUM_DON_3
				|| ch->in_room->vnum == ROOM_VNUM_DON_4
				|| obj->item_type == ITEM_CORPSE_PC )
			obj->timer = -1;
	}

	if ( obj->item_type == ITEM_MONEY )
	{
		char buf [ MAX_STRING_LENGTH ];
		amount.gold   = obj->value[0];
		amount.silver = obj->value[1];
		amount.copper = obj->value[2];

		/*	tot_coins = amount.gold + amount.silver + amount.copper;
			if ( ( ch->carry_weight + tot_coins/100 ) > can_carry_w( ch ) )
			{
			act(AT_WHITE, "$d: you can't carry that much weight.",
			ch, NULL, obj->name, TO_CHAR );
			return 0;
			}
			ch->carry_weight += tot_coins/100; 
			*/
		add_money( &ch->money, &amount );
		sprintf( buf, "You counted %s\n\r", money_string( &amount ) );
		send_to_char( AT_YELLOW, buf, ch );
		if ( IS_SET( ch->act, PLR_AUTOSPLIT ) && ( amount.gold +
					amount.silver + amount.copper > 1 ) )
		{
			if ( amount.gold > 1 )
			{
				sprintf( buf, "%d gold", amount.gold );
				do_split( ch, buf );
			}
			if ( amount.silver > 1 )
			{
				sprintf( buf, "%d silver", amount.silver );
				do_split( ch, buf );
			}
			if ( amount.copper > 1 )
			{
				sprintf( buf, "%d copper", amount.copper );
				do_split( ch, buf );
			}
		}

		extract_obj( obj );
		return 0;
	}
	else
	{
		obj_to_char( obj, ch );
		if ( ch->in_room->vnum == ROOM_VNUM_DON_1
				|| ch->in_room->vnum == ROOM_VNUM_DON_2
				|| ch->in_room->vnum == ROOM_VNUM_DON_3
				|| ch->in_room->vnum == ROOM_VNUM_DON_4
				|| obj->item_type == ITEM_CORPSE_PC )
			obj->timer = -1;
	}

	return 1;
}

void do_get( CHAR_DATA *ch, char *argument )
{
	if ( is_affected( ch, gsn_nerve ) )
	{
		send_to_char( AT_YELLOW, "You cannot feel your arms.\n\r", ch );
		return;
	}
	get( ch, argument, FALSE );
	return;
}
void do_palm( CHAR_DATA *ch, char *argument )
{
	char arg [ MAX_INPUT_LENGTH ];
	if ( IS_NPC( ch ) )
		return;
	if ( !can_use_skpell( ch, gsn_palm ) )
	{
		send_to_char( AT_WHITE, "You don't know how to palm objects",
				ch );
		return;
	}
	one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		send_to_char( AT_WHITE, "Palm what?\n\r", ch );
		return;
	}
	if ( get_curr_dex( ch ) < 22 )
	{
		send_to_char( AT_WHITE, 
				"You aren't coordinated enough to pull off this sleight of hand.\n\r", ch );
		return;
	}
	if ( number_percent( ) > ch->pcdata->learned[gsn_palm] )
		get( ch, argument, FALSE ); /* fail */
	else
	{
		get( ch, argument, TRUE );  /* succeed */
		update_skpell( ch, gsn_palm );
	}

	return;

}


void get( CHAR_DATA *ch, char *argument, bool palming )
{
	OBJ_DATA *obj;
	OBJ_DATA *container;
	char      arg1 [ MAX_INPUT_LENGTH ];
	char      arg2 [ MAX_INPUT_LENGTH ];
	char      buf [ MAX_STRING_LENGTH ];
	bool      found;
	int	      ObjCount=0;
	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	/* Get type. */
	if ( arg1[0] == '\0' )
	{
		if ( palming )
			send_to_char(AT_WHITE, "Palm what?\n\r", ch );
		else
			send_to_char(AT_WHITE, "Get what?\n\r", ch );
		return;
	}

	if ( arg2[0] == '\0' )
	{
		if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
		{
			/* 'get obj' */
			obj = get_obj_list( ch, arg1, ch->in_room->contents );
			if ( !obj )
			{
				act(AT_WHITE, "I see no $T here.", ch, NULL, arg1, TO_CHAR );
				return;
			}

			get_obj( ch, obj, NULL, palming );
		}
		else
		{
			/* 'get all' or 'get all.obj' */
			OBJ_DATA *obj_next;
			if ( palming )
			{
				send_to_char( AT_WHITE, 
						"No one can palm that much and not be noticed.\n\r", ch );
				return;
			}
			found = FALSE;
			for ( obj = ch->in_room->contents; obj; obj = obj_next )
			{
				obj_next = obj->next_content; 

				if ( ( arg1[3] == '\0' || is_name(ch, &arg1[4], obj->name ) )
						&& can_see_obj( ch, obj ) )
				{
					found = TRUE;
					ObjCount+=get_obj( ch, obj, NULL, 2);

					if ((!obj_next || obj->pIndexData->vnum != 
								obj_next->pIndexData->vnum)&& ObjCount)
					{
						sprintf(buf,"You get %d $p%s.",
								ObjCount, ObjCount > 1 ? "s" : "" );
						act(AT_WHITE, buf,ch,obj,NULL,TO_CHAR);
						sprintf(buf,"$n gets %d $p%s.",
								ObjCount, ObjCount > 1 ? "s" : "" );
						act(AT_WHITE, buf,ch,obj,NULL,TO_ROOM);
						ObjCount=0;
					}

				}
			}

			if ( !found ) 
			{
				if ( arg1[3] == '\0' )
					send_to_char(AT_WHITE, "I see nothing here.\n\r", ch );
				else
					act(AT_WHITE, "I see no $T here.", ch, NULL, &arg1[4], TO_CHAR );
			}

		}
	}
	else
	{
		/* 'get ... container' */
		if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
		{
			send_to_char(AT_WHITE, "You can't do that.\n\r", ch );
			return;
		}

		if ( !( container = get_obj_here( ch, arg2 ) ) )
		{
			act(AT_WHITE, "I see no $T here.", ch, NULL, arg2, TO_CHAR );
			return;
		}

		switch ( container->item_type )
		{
			default:
				send_to_char(AT_WHITE, "That's not a container.\n\r", ch );
				return;

			case ITEM_CONTAINER:
			case ITEM_CORPSE_NPC:
				break;

			case ITEM_CORPSE_PC:
				{
					CHAR_DATA *gch;
					char      *pd;
					char       name[ MAX_INPUT_LENGTH ];

					if ( IS_NPC( ch ) )
					{
						send_to_char(AT_WHITE, "You can't do that.\n\r", ch );
						return;
					}

					pd = container->short_descr;
					pd = one_argument( pd, name );
					pd = one_argument( pd, name );
					pd = one_argument( pd, name );

					if ( str_cmp( name, ch->name ) && !IS_IMMORTAL( ch ) )
					{
						bool fGroup;

						fGroup = FALSE;
						for ( gch = char_list; gch; gch = gch->next )
						{
							if ( !IS_NPC( gch )
									&& is_same_group( ch, gch )
									&& !str_cmp( name, gch->name ) )
							{
								fGroup = TRUE;
								break;
							}
						}
						if ( ch->clan != 0 )
							fGroup = TRUE;

						if ( !fGroup )
						{
							send_to_char(AT_WHITE, "You can't do that.\n\r", ch );
							return;
						}
					}
				}
		}

		if ( IS_SET( container->value[1], CONT_CLOSED ) )
		{
			act(AT_GREEN, "The $d is closed.", ch, NULL, container->name, TO_CHAR );
			return;
		}

		if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
		{
			/* 'get obj container' */
			obj = get_obj_list( ch, arg1, container->contains );
			if ( !obj )
			{
				act(AT_GREEN, "I see nothing like that in the $T.",
						ch, NULL, arg2, TO_CHAR );
				return;
			}
			get_obj( ch, obj, container, palming );
		}
		else
		{
			/* 'get all container' or 'get all.obj container' */
			OBJ_DATA *obj_next;
			if ( palming )
			{
				send_to_char( AT_WHITE,
						"You can't palm that much stuff at once from it.\n\r", ch );
				return;
			}
			found = FALSE;
			for ( obj = container->contains; obj; obj = obj_next )
			{
				obj_next = obj->next_content;

				if ( ( arg1[3] == '\0' || is_name(ch, &arg1[4], obj->name ) )
						&& can_see_obj( ch, obj ) )
				{
					found = TRUE;
					ObjCount+=get_obj( ch, obj, container, 2);

					if ((!obj_next || obj->pIndexData->vnum !=
								obj_next->pIndexData->vnum)&& ObjCount)
					{
						sprintf(buf,"You get %d $ps from $P.",ObjCount);
						act(AT_WHITE, buf,ch,obj,container,TO_CHAR);
						sprintf(buf,"$n gets %d $ps from $P.",ObjCount);
						act(AT_WHITE, buf,ch,obj,container,TO_ROOM);

						ObjCount=0;
					}

				}
			}

			if ( !found )
			{
				if ( arg1[3] == '\0' )
					act(AT_GREEN, "I see nothing in the $T.",
							ch, NULL, arg2, TO_CHAR );
				else
					act(AT_GREEN, "I see nothing like that in the $T.",
							ch, NULL, arg2, TO_CHAR );
			}
		}
	}

	return;
}



void do_put( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *container;
	OBJ_DATA *obj;
	char      arg1 [ MAX_INPUT_LENGTH ];
	char      arg2 [ MAX_INPUT_LENGTH ];
	char      buf [ MAX_STRING_LENGTH ];
	int	      ObjCount=0;
	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	if ( is_affected( ch, gsn_nerve ) )
	{
		send_to_char( AT_YELLOW, "You can't feel your arms.\n\r", ch );
		return;
	}
	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char(AT_DGREEN, "Put what in what?\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
	{
		send_to_char(AT_DGREEN, "You can't do that.\n\r", ch );
		return;
	}

	if ( !( container = get_obj_here( ch, arg2 ) ) )
	{
		act(AT_DGREEN, "I see no $T here.", ch, NULL, arg2, TO_CHAR );
		return;
	}

	if ( container->item_type != ITEM_CONTAINER )
	{
		send_to_char(AT_DGREEN, "That's not a container.\n\r", ch );
		return;
	}

	if ( IS_SET( container->value[1], CONT_CLOSED ) )
	{
		act(AT_DGREEN, "The $d is closed.", ch, NULL, container->name, TO_CHAR );
		return;
	}

	if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
	{
		/* 'put obj container' */
		if ( !( obj = get_obj_carry( ch, arg1 ) ) )
		{
			send_to_char(AT_DGREEN, "You do not have that item.\n\r", ch );
			return;
		}

		if ( obj == container )
		{
			send_to_char(AT_DGREEN, "You can't fold it into itself.\n\r", ch );
			return;
		}

		if ( !can_drop_obj( ch, obj ) )
		{
			send_to_char(AT_DGREEN, "You can't let go of it.\n\r", ch );
			return;
		}

		if ( get_obj_weight( obj ) + get_obj_weight( container )
				> container->value[0] )
		{
			send_to_char(AT_DGREEN, "It won't fit.\n\r", ch );
			return;
		}

		obj_from_char( obj );
		obj_to_obj( obj, container );
		act(AT_GREEN, "You put $p in $P.", ch, obj, container, TO_CHAR );
		act(AT_GREEN, "$n puts $p in $P.", ch, obj, container, TO_ROOM );
		oprog_put_trigger( obj, ch, container );
	}
	else
	{
		/* 'put all container' or 'put all.obj container' */
		OBJ_DATA *obj_next;

		for ( obj = ch->carrying; obj; obj = obj_next )
		{
			obj_next = obj->next_content;

			if ( ( arg1[3] == '\0' || is_name(ch, &arg1[4], obj->name ) )
					&& can_see_obj( ch, obj )
					&& obj->wear_loc == WEAR_NONE
					&& obj != container
					&& can_drop_obj( ch, obj )
					&& get_obj_weight( obj ) + get_obj_weight( container )
					<= container->value[0] )
			{
				obj_from_char( obj );
				obj_to_obj( obj, container );
				oprog_put_trigger( obj, ch, container );
				ObjCount++;

				if (!obj_next || obj->pIndexData->vnum !=
						obj_next->pIndexData->vnum)
				{
					sprintf(buf,"You put %d $ps in $P.",ObjCount);
					act(AT_WHITE, buf,ch,obj,container,TO_CHAR);
					sprintf(buf,"$n puts %d $ps in $P.",ObjCount);
					act(AT_WHITE, buf,ch,obj,container,TO_ROOM);
					ObjCount=0;
				}

			}
		}
	}

	return;
}



void do_drop( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	char      arg [ MAX_INPUT_LENGTH ];
	char      arg2 [ MAX_INPUT_LENGTH ];
	char      buf [ MAX_STRING_LENGTH ];
	bool      found;
	int	      ObjCount=0;
	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg2 );

	if ( arg[0] == '\0' )
	{
		send_to_char(AT_DGREEN, "Drop what?\n\r", ch );
		return;
	}

	if ( is_number( arg ) )
	{
		/* 'drop NNNN coins' */
		OBJ_DATA *obj_next;

		MONEY_DATA *howmuch;
		int amount;

		/* drop <amount> <currency type> */

		amount  = atoi( arg );
		howmuch = take_money( ch, amount, arg2, "drop" );

		if ( !howmuch )
		{
			sprintf( buf, "You don't have that much %s.\n\r", arg2 );
			send_to_char( AT_WHITE, buf, ch );
			return;
		}

		for ( obj = ch->in_room->contents; obj; obj = obj_next )
		{
			obj_next = obj->next_content;

			if ( obj->deleted )
				continue;

			switch ( obj->pIndexData->vnum )
			{
				case OBJ_VNUM_MONEY_ONE:
					if ( obj->value[0] == 1 )
						howmuch->gold   += 1;
					if ( obj->value[1] == 1 )
						howmuch->silver += 1;
					if ( obj->value[2] == 1 )
						howmuch->copper += 1;
					extract_obj( obj );
					break;
				case OBJ_VNUM_MONEY_SOME:
					howmuch->gold   += obj->value[0];
					howmuch->silver += obj->value[1];
					howmuch->copper += obj->value[2];
					extract_obj( obj );
					break;
			}
		}             

		/* Change the parameters of create_money to MONEY_DATA *amount */
		obj_to_room( create_money( howmuch ), ch->in_room );
		send_to_char(AT_YELLOW, "OK.\n\r", ch );
		act(AT_YELLOW, "$n drops some coins.", ch, NULL, NULL, TO_ROOM );
		return;
	}                

if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
{
	/* 'drop obj' */
	if ( !( obj = get_obj_carry( ch, arg ) ) )
	{
		send_to_char(AT_DGREEN, "You do not have that item.\n\r", ch );	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
		send_to_char(AT_DGREEN, "You can't let go of it.\n\r", ch );
		return;
	}

	obj_from_char( obj );
	if ( ch->in_room->vnum == ROOM_VNUM_DON_1
			|| ch->in_room->vnum == ROOM_VNUM_DON_2
			|| ch->in_room->vnum == ROOM_VNUM_DON_3
			|| ch->in_room->vnum == ROOM_VNUM_DON_4
			|| obj->item_type == ITEM_CORPSE_PC )
		obj->timer = 30;
	obj_to_room( obj, ch->in_room );
	act(AT_GREEN, "You drop $p.", ch, obj, NULL, TO_CHAR );
	act(AT_GREEN, "$n drops $p.", ch, obj, NULL, TO_ROOM );
	oprog_drop_trigger( obj, ch );
}
else
{
	/* 'drop all' or 'drop all.obj' */
	OBJ_DATA *obj_next;

	found = FALSE;
	for ( obj = ch->carrying; obj; obj = obj_next )
	{
		obj_next = obj->next_content;

		if ( ( arg[3] == '\0' || is_name(ch, &arg[4], obj->name ) )
				&& can_see_obj( ch, obj )
				&& obj->wear_loc == WEAR_NONE
				&& can_drop_obj( ch, obj ) )
		{
			found = TRUE;
			obj_from_char( obj );
			if ( ch->in_room->vnum == ROOM_VNUM_DON_1
					|| ch->in_room->vnum == ROOM_VNUM_DON_2
					|| ch->in_room->vnum == ROOM_VNUM_DON_3
					|| ch->in_room->vnum == ROOM_VNUM_DON_4
					|| obj->item_type == ITEM_CORPSE_PC )
				obj->timer = 20;

			obj_to_room( obj, ch->in_room );
			oprog_drop_trigger( obj, ch );
			ObjCount++;

			if (!obj_next || obj->pIndexData->vnum !=
					obj_next->pIndexData->vnum)
			{
				sprintf(buf,"You drop %d $ps.",ObjCount);
				act(AT_WHITE, buf, ch, obj, NULL, TO_CHAR);
				sprintf(buf,"$n drops %d $ps.", ObjCount);
				act(AT_WHITE, buf, ch, obj, NULL, TO_ROOM);
				ObjCount=0;
			}

		}
	}

	if ( !found )
	{
		if ( arg[3] == '\0' )
			send_to_char(AT_DGREEN, "You are not carrying anything.", ch );
		else
			act(AT_DGREEN, "You are not carrying any $T.",
					ch, NULL, &arg[4], TO_CHAR );
	}
}

return;
}



void do_give( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *victim;
	OBJ_DATA  *obj;
	char       buf [ MAX_STRING_LENGTH ];
	char       arg1 [ MAX_INPUT_LENGTH ];
	char       arg2 [ MAX_INPUT_LENGTH ];
	MONEY_DATA *howmuch;
	char       arg3 [ MAX_INPUT_LENGTH ];
	int        amount;
	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char(AT_DGREEN, "Give what to whom?\n\r", ch );
		return;
	}

	if ( is_number( arg1 ) )
	{
		/* 'give NNNN coins victim' */
		/* 'give NNNN type victim' */

		amount = atoi( arg1 );
		argument = one_argument( argument, arg3 );

		if ( arg3[0] == '\0' )
		{
			send_to_char(AT_DGREEN, "Give what to whom?\n\r", ch );
			return;
		}

		if ( !( victim = get_char_room( ch, arg3 ) ) )
		{
			send_to_char(AT_DGREEN, "They aren't here.\n\r", ch );
			return;
		}      

		howmuch = take_money( ch, amount, arg2, "give" );

		if ( !howmuch )
		{
			send_to_char(AT_YELLOW, "You haven't got that much money.\n\r", ch );
			return; 
		}

		add_money( &victim->money, howmuch );

		sprintf( buf, "You give %s %s", victim->name, money_string( howmuch ) );
		act(AT_YELLOW, buf, ch, NULL, victim, TO_CHAR );
		sprintf( buf, "%s gives you %s",
				can_see( victim, ch ) ? ch->name : "someone", money_string( howmuch ) );
		act(AT_YELLOW, buf, ch, NULL, victim, TO_VICT );
		act(AT_YELLOW, "$n gives $N some coins.",  ch, NULL, victim, TO_NOTVICT );

		/* Change mprog_bribe_trigger to  
		   ( CHAR_DATA *mob, CHAR_DATA *ch, MONEY_DATA *amount ) */
		mprog_bribe_trigger( victim, ch, howmuch ); 

		return;
	}

if ( !( obj = get_obj_carry( ch, arg1 ) ) )
{
	send_to_char(AT_DGREEN, "You do not have that item.\n\r", ch );
	return;
}

if ( obj->wear_loc != WEAR_NONE )
{
	send_to_char(AT_DGREEN, "You must remove it first.\n\r", ch );
	return;
}

if ( !( victim = get_char_room( ch, arg2 ) ) )
{
	send_to_char(AT_DGREEN, "They aren't here.\n\r", ch );
	return;
}

if ( !can_drop_obj( ch, obj ) )
{
	send_to_char(AT_DGREEN, "You can't let go of it.\n\r", ch );
	return;
}

if ( victim->carry_number + get_obj_number( obj ) > can_carry_n( victim ) )
{
	act(AT_DGREEN, "$N has $S hands full.", ch, NULL, victim, TO_CHAR );
	return;
}

if ( victim->carry_weight + get_obj_weight( obj ) > can_carry_w( victim ) )
{
	act(AT_DGREEN, "$N can't carry that much weight.", ch, NULL, victim, TO_CHAR );
	return;
}

if ( !can_see_obj( victim, obj ) )
{
	act(AT_DGREEN, "$N can't see it.", ch, NULL, victim, TO_CHAR );
	return;
}

obj_from_char( obj );
obj_to_char( obj, victim );
act(AT_DGREEN, "You give $p to $N.", ch, obj, victim, TO_CHAR    );
act(AT_DGREEN, "$n gives you $p.",   ch, obj, victim, TO_VICT    );
act(AT_DGREEN, "$n gives $p to $N.", ch, obj, victim, TO_NOTVICT );
oprog_give_trigger( obj, ch, victim );
if ( !obj || !ch || !victim )
	return;
	mprog_give_trigger( victim, ch, obj );
	return;
	}

/* Transport item skill here */

void do_transport( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *victim;
	OBJ_DATA  *obj;
	char       arg1 [ MAX_INPUT_LENGTH ];
	char       arg2 [ MAX_INPUT_LENGTH ];

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char(AT_DGREEN, "Transport what to whom?\n\r", ch );
		return;
	}


	if ( !( obj = get_obj_carry( ch, arg1 ) ) )
	{
		send_to_char(AT_DGREEN, "You do not have that item.\n\r", ch );
		return;
	}

	if ( obj->wear_loc != WEAR_NONE )
	{
		send_to_char(AT_DGREEN, "You must remove it first.\n\r", ch );
		return;
	}

	if ( !( victim = get_pc_world( ch, arg2 ) ) )
	{
		send_to_char(AT_DGREEN, "They aren't here.\n\r", ch );
		return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
		send_to_char(AT_DGREEN, "You can't let go of it.\n\r", ch );
		return;
	}

	if ( victim->carry_number + get_obj_number( obj ) > can_carry_n( victim ) )
	{
		act(AT_DGREEN, "$N has $S hands full.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( victim->carry_weight + get_obj_weight( obj ) > can_carry_w( victim ) )
	{
		act(AT_DGREEN, "$N can't carry that much weight.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( !can_see_obj( victim, obj ) )
	{
		act(AT_DGREEN, "$N can't see it.", ch, NULL, victim, TO_CHAR );
		return;
	}

	obj_from_char( obj );
	obj_to_char( obj, victim );
	act(AT_DGREEN, "You transport $p to $N.", ch, obj, victim, TO_CHAR    );
	act(AT_DGREEN, "$p appears magically in your hands from $n.",   ch, obj, victim, TO_VICT );
	act(AT_DGREEN, "Something appears magically in $n's hands from $N.", victim , NULL, ch, 
			TO_NOTVICT );
	return;
}

void do_fill( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	OBJ_DATA *fountain;
	char      arg [ MAX_INPUT_LENGTH ];
	bool      found;

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char(AT_BLUE, "Fill what?\n\r", ch );
		return;
	}

	if ( !( obj = get_obj_carry( ch, arg ) ) )
	{
		send_to_char(AT_BLUE, "You do not have that item.\n\r", ch );
		return;
	}

	found = FALSE;
	for ( fountain = ch->in_room->contents; fountain;
			fountain = fountain->next_content )
	{
		if ( fountain->item_type == ITEM_FOUNTAIN )
		{
			found = TRUE;
			break;
		}
	}

	if ( !found )
	{
		send_to_char(AT_BLUE, "There is no fountain here!\n\r", ch );
		return;
	}

	if ( obj->item_type != ITEM_DRINK_CON )
	{
		send_to_char(AT_BLUE, "You can't fill that.\n\r", ch );
		return;
	}

	if ( obj->value[1] != 0 && obj->value[2] != 0 )
	{
		send_to_char(AT_BLUE, "There is already another liquid in it.\n\r", ch );
		return;
	}

	if ( obj->value[1] >= obj->value[0] )
	{
		send_to_char(AT_BLUE, "Your container is full.\n\r", ch );
		return;
	}

	act(AT_LBLUE, "You fill $p.", ch, obj, NULL, TO_CHAR );
	obj->value[2] = 0;
	obj->value[1] = obj->value[0];
	oprog_fill_trigger( obj, ch, fountain );
	return;
}

void do_drink( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	char      arg [ MAX_INPUT_LENGTH ];
	int       amount;
	int       liquid;

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
		{
			if ( obj->item_type == ITEM_FOUNTAIN ) 
				break;
			if ( obj->item_type == ITEM_LIQUID    )
				break;
		}

		if ( !obj )
		{
			send_to_char(AT_BLUE, "Drink what?\n\r", ch );
			return;
		}
	}
	else
	{
		if ( !( obj = get_obj_here( ch, arg ) ) )
		{
			send_to_char(AT_BLUE, "You can't find it.\n\r", ch );
			return;
		}
	}

	if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_DRUNK] >= 90 )
	{
		send_to_char(AT_BLUE, "You fail to reach your mouth.  *Hic*\n\r", ch );
		return;
	}

	switch ( obj->item_type )
	{
		default:
			send_to_char(AT_BLUE, "You can't drink from that.\n\r", ch );
			break;

		case ITEM_FOUNTAIN:
			if ( !IS_NPC( ch ) )
				ch->pcdata->condition[COND_THIRST] = 58;  /*  48  */
			act(AT_LBLUE, "You drink from the $p.\n\r", ch, obj, NULL, TO_CHAR );
			send_to_char(AT_BLUE, "You are not thirsty.\n\r", ch );
			act(AT_LBLUE, "$n drinks from the $p.", ch, obj, NULL, TO_ROOM );
			break;

		case ITEM_LIQUID:
		case ITEM_DRINK_CON:
			if ( obj->value[1] <= 0 ) {
				if (obj->item_type == ITEM_DRINK_CON) {
					send_to_char(AT_BLUE, "It is already empty.\n\r", ch );
					return;
				} else if (obj->item_type == ITEM_LIQUID) {
					send_to_char(AT_BLUE, "There doesn't seem to be any left.\n\r", ch );
					return;
				}
			}

			if ( ( liquid = obj->value[2] ) >= LIQ_MAX ) {
				bug( "Do_drink: bad liquid number %d.", liquid );
				liquid = obj->value[2] = 0;
			}

			act(AT_LBLUE, "You drink $T from $p.", ch, obj, liq_table[liquid].liq_name, TO_CHAR );
			act(AT_LBLUE, "$n drinks $T from $p.", ch, obj, liq_table[liquid].liq_name, TO_ROOM );

			amount = number_range( 3, 8 );
			amount = UMIN( amount, obj->value[1] );

			gain_condition( ch, COND_DRUNK, liq_table[liquid].liq_affect[COND_DRUNK  ] );
			gain_condition( ch, COND_FULL, amount * liq_table[liquid].liq_affect[COND_FULL   ] );
			gain_condition( ch, COND_THIRST, amount * liq_table[liquid].liq_affect[COND_THIRST ] );

			if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_DRUNK ] > 100 ) {
				ch->pcdata->condition[COND_DRUNK ] = 100;
			}

			if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_DRUNK ] > 10 ) {
				send_to_char(AT_ORANGE, "You feel drunk.\n\r", ch );
			}

			if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_FULL  ] > MAX_FULL ) {
				send_to_char(AT_BLUE, "You are full.\n\r", ch );
			}

			if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_THIRST] > MAX_THIRST ) {
				send_to_char(AT_BLUE, "You do not feel thirsty.\n\r", ch );
			}

			if ( obj->value[3] != 0 ) {
				/* The shit was poisoned ! */
				AFFECT_DATA af;

				send_to_char(AT_GREEN, "You choke and gag.\n\r", ch );
				act(AT_GREEN, "$n chokes and gags.", ch, NULL, NULL, TO_ROOM );
				af.type      = gsn_poison;
				af.duration  = 3 * amount;
				af.location  = APPLY_STR;
				af.modifier  = -2;
				af.bitvector = AFF_POISON;
				affect_join( ch, &af );
			}

			obj->value[1] -= amount;

			if ( obj->value[1] <= 0 ) {
				if (obj->item_type == ITEM_DRINK_CON) {
					send_to_char(AT_BLUE, "The empty container vanishes.\n\r", ch );
				} else if (obj->item_type == ITEM_LIQUID) {
					act(AT_LBLUE, "You drink the last of the $T.", ch, obj, liq_table[liquid].liq_name, TO_CHAR );
					act(AT_LBLUE, "$n drinks the last of the $T.", ch, obj, liq_table[liquid].liq_name, TO_ROOM );
				}

				extract_obj( obj );
			}
			break;
	}

	return;
}



void do_eat( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	char      arg [ MAX_INPUT_LENGTH ];
	int       amnt;
	one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		send_to_char(AT_ORANGE, "Eat what?\n\r", ch );
		return;
	}

	if ( !( obj = get_obj_carry( ch, arg ) ) )
	{
		send_to_char(AT_ORANGE, "You do not have that item.\n\r", ch );
		return;
	}

	if ( !IS_IMMORTAL( ch ) )
	{
		if ( obj->item_type != ITEM_FOOD 
				&& obj->item_type != ITEM_PILL 
				&& obj->item_type != ITEM_BERRY
				&& obj->item_type != ITEM_CORPSE_NPC )
		{
			send_to_char(AT_ORANGE, "That's not edible.\n\r", ch );
			return;
		}

		if ( obj->item_type == ITEM_CORPSE_NPC ) {
			send_to_char(AT_ORANGE, "Your stomach turns at the thought of eating that.\n\r", ch );
			return;
		}

		if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_FULL] > MAX_FULL ) {   
			send_to_char(AT_ORANGE, "You are too full to eat more.\n\r", ch );
			return;
		}
	}

	act(AT_ORANGE, "You eat $p.", ch, obj, NULL, TO_CHAR );
	act(AT_ORANGE, "$n eats $p.", ch, obj, NULL, TO_ROOM );

	switch ( obj->item_type )
	{

		case ITEM_FOOD:
			if ( !IS_NPC( ch ) )
			{
				int condition;

				condition = ch->pcdata->condition[COND_FULL];
				gain_condition( ch, COND_FULL, obj->value[0] );
				if ( ch->pcdata->condition[COND_FULL] > MAX_FULL )
					send_to_char(AT_ORANGE, "You are full.\n\r", ch );
				else if ( condition == 0 && ch->pcdata->condition[COND_FULL] > 0 )
					send_to_char(AT_ORANGE, "You are no longer hungry.\n\r", ch );
			}

			if ( obj->value[3] != 0 )
			{
				/* The shit was poisoned! */
				AFFECT_DATA af;

				act(AT_GREEN, "$n chokes and gags.", ch, 0, 0, TO_ROOM );
				send_to_char(AT_GREEN, "You choke and gag.\n\r", ch );

				af.type      = gsn_poison;
				af.level     = obj->level;
				af.duration  = 2 * obj->value[0];
				af.location  = APPLY_STR;
				af.modifier  = -2;
				af.bitvector = AFF_POISON;
				affect_join( ch, &af );
			}
			break;
		case ITEM_BERRY:
			amnt = number_range( obj->value[0], obj->value[1] );
			ch->hit = UMIN( ch->hit + amnt, MAX_HIT(ch) );
			update_pos( ch );
			send_to_char(AT_ORANGE, "You feel warm all over.\n\r", ch);
			break;        
		case ITEM_PILL:
			if ( ch->level >= obj->level )
			{
				obj_cast_spell( obj->value[1], obj->value[0], ch, ch, NULL );
				obj_cast_spell( obj->value[2], obj->value[0], ch, ch, NULL );
				obj_cast_spell( obj->value[3], obj->value[0], ch, ch, NULL );
			}
			else
			{
				act( AT_ORANGE, "$p is too high level for you.", ch, obj, NULL,
						TO_CHAR );
				return;
			}
			break;
	}

	extract_obj( obj );
	return;
}



/*
 * Remove an object.
 */
bool remove_obj( CHAR_DATA *ch, int iWear, bool fReplace )
{
	OBJ_DATA *obj;

	if ( !( obj = get_eq_char( ch, iWear ) ) )
		return TRUE;

	if ( !fReplace )
		return FALSE;

	if ( IS_SET( obj->extra_flags, ITEM_NOREMOVE ) )
	{
		act(AT_RED, "You can't remove $p.", ch, obj, NULL, TO_CHAR );
		return FALSE;
	}

	/*    unequip_char( ch, obj ); */
	act(AT_WHITE, "$n stops using $p.", ch, obj, NULL, TO_ROOM );
	act(AT_WHITE, "You stop using $p.", ch, obj, NULL, TO_CHAR );

	unequip_char( ch, obj );

	return TRUE;
}



/*
 * Wear one object.
 * Optional replacement of existing objects.
 * Big repetitive code, ick.
 */
void wear_obj( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace )
{
	char buf [ MAX_STRING_LENGTH ];
	int weapon_type = 0;

	if ( ch->level < obj->level )
	{
		sprintf( buf, "You must be level %d to use this object.\n\r",
				obj->level );
		send_to_char(AT_BLUE, buf, ch );
		act(AT_BLUE, "$n tries to use $p, but is too inexperienced.",
				ch, obj, NULL, TO_ROOM );
		return;
	}

	if ( obj->item_type == ITEM_LIGHT )
	{
		if ( !remove_obj( ch, WEAR_LIGHT, fReplace ) )
			return;
		act(AT_WHITE, "You light $p and hold it.",  ch, obj, NULL, TO_CHAR );
		act(AT_WHITE, "$n lights $p and holds it.", ch, obj, NULL, TO_ROOM );
		equip_char( ch, obj, WEAR_LIGHT );
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_FINGER ) )
	{
		if ( get_eq_char( ch, WEAR_FINGER_L )
				&&   get_eq_char( ch, WEAR_FINGER_R )
				&&   !remove_obj( ch, WEAR_FINGER_L, fReplace )
				&&   !remove_obj( ch, WEAR_FINGER_R, fReplace ) )
			return;

		if ( !get_eq_char( ch, WEAR_FINGER_L ) )
		{
			act(AT_BLUE, "You wear $p on your left finger.",  ch, obj, NULL, TO_CHAR );
			act(AT_BLUE, "$n wears $p on $s left finger.",    ch, obj, NULL, TO_ROOM );
			equip_char( ch, obj, WEAR_FINGER_L );
			return;
		}

		if ( !get_eq_char( ch, WEAR_FINGER_R ) )
		{
			act(AT_BLUE, "You wear $p on your right finger.", ch, obj, NULL, TO_CHAR );
			act(AT_BLUE, "$n wears $p on $s right finger.",   ch, obj, NULL, TO_ROOM );
			equip_char( ch, obj, WEAR_FINGER_R );
			return;
		}

		bug( "Wear_obj: no free finger.", 0 );
		send_to_char(AT_BLUE, "You already wear two rings.\n\r", ch );
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_NECK ) )
	{
		if ( get_eq_char( ch, WEAR_NECK_1 )
				&&   get_eq_char( ch, WEAR_NECK_2 )
				&&   !remove_obj( ch, WEAR_NECK_1, fReplace )
				&&   !remove_obj( ch, WEAR_NECK_2, fReplace ) )
			return;

		if ( !get_eq_char( ch, WEAR_NECK_1 ) )
		{
			act(AT_BLUE, "You wear $p around your neck.", ch, obj, NULL, TO_CHAR );
			act(AT_BLUE, "$n wears $p around $s neck.",   ch, obj, NULL, TO_ROOM );
			equip_char( ch, obj, WEAR_NECK_1 );
			return;
		}

		if ( !get_eq_char( ch, WEAR_NECK_2 ) )
		{
			act(AT_BLUE, "You wear $p around your neck.", ch, obj, NULL, TO_CHAR );
			act(AT_BLUE, "$n wears $p around $s neck.",   ch, obj, NULL, TO_ROOM );
			equip_char( ch, obj, WEAR_NECK_2 );
			return;
		}

		bug( "Wear_obj: no free neck.", 0 );
		send_to_char(AT_BLUE, "You already wear two neck items.\n\r", ch );
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_BODY ) )
	{
		if ( !remove_obj( ch, WEAR_BODY, fReplace ) )
			return;
		act(AT_BLUE, "You wear $p on your body.", ch, obj, NULL, TO_CHAR );
		act(AT_BLUE, "$n wears $p on $s body.",   ch, obj, NULL, TO_ROOM );
		equip_char( ch, obj, WEAR_BODY );
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_HEAD ) )
	{
		if ( !remove_obj( ch, WEAR_HEAD, fReplace ) )
			return;
		act(AT_BLUE, "You wear $p on your head.", ch, obj, NULL, TO_CHAR );
		act(AT_BLUE, "$n wears $p on $s head.",   ch, obj, NULL, TO_ROOM );
		equip_char( ch, obj, WEAR_HEAD );
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_ORBIT ) )
	{
		if ( get_eq_char( ch, WEAR_ORBIT )
				&&   get_eq_char( ch, WEAR_ORBIT_2 )
				&&   !remove_obj( ch, WEAR_ORBIT, fReplace )
				&&   !remove_obj( ch, WEAR_ORBIT_2, fReplace ) )
			return;

		if ( !get_eq_char( ch, WEAR_ORBIT ) )
		{
			act(AT_BLUE, "You start $p spinning about your head.",
					ch, obj, NULL, TO_CHAR );
			act(AT_BLUE, "$n starts $p spinning around $s head.",
					ch, obj, NULL, TO_ROOM );
			equip_char( ch, obj, WEAR_ORBIT );
			return;
		}

		if ( !get_eq_char( ch, WEAR_ORBIT_2 ) )
		{
			act(AT_BLUE, "You starts $p spinning about your head.",
					ch, obj, NULL, TO_CHAR );
			act(AT_BLUE, "$n starts $p spinning around $s head.",
					ch, obj, NULL, TO_ROOM );
			equip_char( ch, obj, WEAR_ORBIT_2 );
			return;
		}

		bug( "Wear_obj: no free wrist.", 0 );
		send_to_char(AT_BLUE, "There are already two things spinning about your head.\n\r", ch );
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_FACE ) )
	{
		if ( !remove_obj( ch, WEAR_ON_FACE, fReplace ) )
			return;
		act(AT_BLUE, "You place $p on your face.", ch, obj, NULL, TO_CHAR );
		act(AT_BLUE, "$n places $p on $s face.",   ch, obj, NULL, TO_ROOM );
		equip_char( ch, obj, WEAR_ON_FACE );
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_CONTACT ) )
	{
		if ( !remove_obj( ch, WEAR_IN_EYES, fReplace ) )
			return;
		act(AT_BLUE, "You stick $p into your eyes.", ch, obj, NULL, TO_CHAR );
		act(AT_BLUE, "$n sticks $p into $s eyes.",   ch, obj, NULL, TO_ROOM );
		equip_char( ch, obj, WEAR_IN_EYES );
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_LEGS ) )
	{
		if ( !remove_obj( ch, WEAR_LEGS, fReplace ) )
			return;
		act(AT_BLUE, "You wear $p on your legs.", ch, obj, NULL, TO_CHAR );
		act(AT_BLUE, "$n wears $p on $s legs.",   ch, obj, NULL, TO_ROOM );
		equip_char( ch, obj, WEAR_LEGS );
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_FEET ) )
	{
		if ( !remove_obj( ch, WEAR_FEET, fReplace ) )
			return;
		act(AT_BLUE, "You wear $p on your feet.", ch, obj, NULL, TO_CHAR );
		act(AT_BLUE, "$n wears $p on $s feet.",   ch, obj, NULL, TO_ROOM );
		equip_char( ch, obj, WEAR_FEET );
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_HANDS ) )
	{
		if ( !remove_obj( ch, WEAR_HANDS, fReplace ) )
			return;
		act(AT_BLUE, "You wear $p on your hands.", ch, obj, NULL, TO_CHAR );
		act(AT_BLUE, "$n wears $p on $s hands.",   ch, obj, NULL, TO_ROOM );
		equip_char( ch, obj, WEAR_HANDS );
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_ARMS ) )
	{
		if ( !remove_obj( ch, WEAR_ARMS, fReplace ) )
			return;
		act(AT_BLUE, "You wear $p on your arms.", ch, obj, NULL, TO_CHAR );
		act(AT_BLUE, "$n wears $p on $s arms.",   ch, obj, NULL, TO_ROOM );
		equip_char( ch, obj, WEAR_ARMS );
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_ABOUT ) )
	{
		if ( !remove_obj( ch, WEAR_ABOUT, fReplace ) )
			return;
		act(AT_BLUE, "You wear $p about your body.", ch, obj, NULL, TO_CHAR );
		act(AT_BLUE, "$n wears $p about $s body.",   ch, obj, NULL, TO_ROOM );
		equip_char( ch, obj, WEAR_ABOUT );
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_WAIST ) )
	{
		if ( !remove_obj( ch, WEAR_WAIST, fReplace ) )
			return;
		act(AT_BLUE, "You wear $p about your waist.", ch, obj, NULL, TO_CHAR );
		act(AT_BLUE, "$n wears $p about $s waist.",   ch, obj, NULL, TO_ROOM );
		equip_char( ch, obj, WEAR_WAIST );
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_WRIST ) )
	{
		if ( get_eq_char( ch, WEAR_WRIST_L )
				&&   get_eq_char( ch, WEAR_WRIST_R )
				&&   !remove_obj( ch, WEAR_WRIST_L, fReplace )
				&&   !remove_obj( ch, WEAR_WRIST_R, fReplace ) )
			return;

		if ( !get_eq_char( ch, WEAR_WRIST_L ) )
		{
			act(AT_BLUE, "You wear $p around your left wrist.",
					ch, obj, NULL, TO_CHAR );
			act(AT_BLUE, "$n wears $p around $s left wrist.",
					ch, obj, NULL, TO_ROOM );
			equip_char( ch, obj, WEAR_WRIST_L );
			return;
		}

		if ( !get_eq_char( ch, WEAR_WRIST_R ) )
		{
			act(AT_BLUE, "You wear $p around your right wrist.",
					ch, obj, NULL, TO_CHAR );
			act(AT_BLUE, "$n wears $p around $s right wrist.",
					ch, obj, NULL, TO_ROOM );
			equip_char( ch, obj, WEAR_WRIST_R );
			return;
		}

		bug( "Wear_obj: no free wrist.", 0 );
		send_to_char(AT_BLUE, "You already wear two wrist items.\n\r", ch );
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_SHIELD ) )
	{
		if ( get_eq_char( ch, WEAR_WIELD_2 ) )
		{
			send_to_char( AT_BLUE, "You cannot use a shield while dual wielding!\n\r", ch );
			return;
		}

		if ( !remove_obj( ch, WEAR_SHIELD, fReplace ) )
			return;

		act(AT_BLUE, "You wear $p as a shield.", ch, obj, NULL, TO_CHAR );
		act(AT_BLUE, "$n wears $p as a shield.", ch, obj, NULL, TO_ROOM );
		equip_char( ch, obj, WEAR_SHIELD );
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WIELD ) )
	{
		if ( !remove_obj( ch, WEAR_WIELD, fReplace ) )
			return;

		if ( get_obj_weight( obj ) > str_app[get_curr_str( ch )].wield )
		{
			send_to_char(AT_BLUE, "It is too heavy for you to wield.\n\r", ch );
			return;
		}
		weapon_type = obj->value[3];
		if((strcmp(flag_string(weapon_flags,weapon_type),"none")))
		{
			if ( !(class_table[prime_class(ch)].objtype[weapon_type]) )
			{
				sprintf(buf, "%s's cannot use weapons that %s.\n\r", 
						class_table[prime_class(ch)].who_long,
						flag_string(weapon_flags,weapon_type) );
				send_to_char(AT_YELLOW, buf, ch );
				return;
			}
		}
		act(AT_BLUE, "You wield $p.", ch, obj, NULL, TO_CHAR );
		act(AT_BLUE, "$n wields $p.", ch, obj, NULL, TO_ROOM );
		equip_char( ch, obj, WEAR_WIELD );
		return;
	}


	if ( CAN_WEAR( obj, ITEM_HOLD ) )
	{
		if ( get_eq_char( ch, WEAR_WIELD_2 ) )
		{
			send_to_char( AT_BLUE, "You cannot hold something while dual wielding!\n\r", ch );
			return;
		}	

		if ( !remove_obj( ch, WEAR_HOLD, fReplace ) )
			return;
		act(AT_BLUE, "You hold $p in your hands.", ch, obj, NULL, TO_CHAR );
		act(AT_BLUE, "$n holds $p in $s hands.",   ch, obj, NULL, TO_ROOM );
		equip_char( ch, obj, WEAR_HOLD );
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_EARS ) )
	{
		if ( get_eq_char( ch, WEAR_EAR_L )
				&&   get_eq_char( ch, WEAR_EAR_R )
				&&   !remove_obj( ch, WEAR_EAR_L, fReplace )
				&&   !remove_obj( ch, WEAR_EAR_R, fReplace ) )
			return;

		if ( !get_eq_char( ch, WEAR_EAR_L ) )
		{
			act(AT_BLUE, "You place $p on your left ear.", ch, obj, NULL, TO_CHAR );
			act(AT_BLUE, "$n places $p on $s left ear.", ch, obj, NULL, TO_ROOM );
			equip_char( ch, obj, WEAR_EAR_L );
			return;
		}

		if ( !get_eq_char( ch, WEAR_EAR_R ) )
		{
			act(AT_BLUE, "You place $p on your right ear.", ch, obj, NULL, TO_CHAR );
			act(AT_BLUE, "$n placess $p on $s right ear.", ch, obj, NULL, TO_ROOM );
			equip_char( ch, obj, WEAR_EAR_R );
			return;
		}

		bug( "Wear_obj: no free ear.", 0 );
		send_to_char(AT_BLUE, "You already have items on both ears.\n\r", ch );
		return;
	}


	if ( CAN_WEAR( obj, ITEM_WEAR_ANKLE ) )
	{
		if ( get_eq_char( ch, WEAR_ANKLE_L )
				&&   get_eq_char( ch, WEAR_ANKLE_R )
				&&   !remove_obj( ch, WEAR_ANKLE_L, fReplace )
				&&   !remove_obj( ch, WEAR_ANKLE_R, fReplace ) )
			return;

		if ( !get_eq_char( ch, WEAR_ANKLE_L ) )
		{
			act(AT_BLUE, "You wear $p on your left ankle.", ch, obj, NULL, TO_CHAR );
			act(AT_BLUE, "$n wears $p on $s left ankle.", ch, obj, NULL, TO_ROOM );
			equip_char( ch, obj, WEAR_ANKLE_L );
			return;
		}

		if ( !get_eq_char( ch, WEAR_ANKLE_R ) )
		{
			act(AT_BLUE, "You wear $p on your right ankle.", ch, obj, NULL, TO_CHAR );
			act(AT_BLUE, "$n wears $p on $s right ankle.", ch, obj, NULL, TO_ROOM );
			equip_char( ch, obj, WEAR_ANKLE_R );
			return;
		}

		bug( "Wear_obj: no free ankle.", 0 );
		send_to_char(AT_BLUE, "You already have items on both ankles.\n\r", ch );
		return;
	}

	if ( fReplace )
		send_to_char(AT_BLUE, "You can't wear, wield, or hold that.\n\r", ch );

	return;
}

void do_dual( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	char      arg [ MAX_INPUT_LENGTH ];
	char      buf [ MAX_STRING_LENGTH ];
	bool      fReplace;
	int       weapon_type = 0;

	fReplace = TRUE;

	one_argument( argument, arg );

	if ( IS_NPC( ch ) )
		return;
	if ( IS_AFFECTED2( ch, AFF_RAGE ) )
	{
		send_to_char( AT_RED, "You are too enraged to dual wield.\n\r", ch );
		return;
	}
	if ( ( get_eq_char ( ch, WEAR_SHIELD ) ) || ( get_eq_char ( ch, WEAR_HOLD ) ) )
	{
		send_to_char( AT_BLUE, "You cannot dual wield while you hold something in your hands.\n\r", ch );
		return;
	}

	if ( arg[0] == '\0' )
	{
		send_to_char(AT_BLUE, "Dual wield what?\n\r", ch );
		return;
	}
	if ( !( obj = get_obj_carry( ch, arg ) ) )
	{
		send_to_char(AT_BLUE, "You do not have that item.\n\r", ch );
		return;
	}
	if ( ch->level < obj->level )
	{
		send_to_char( AT_WHITE, "You are to inexperienced.\n\r", ch );
		return;
	}
	if ( !IS_NPC( ch )
			&& !can_use_skpell( ch, gsn_dual ) )
	{
		send_to_char(AT_WHITE, "You cannot.\n\r", ch );
		return;
	}
	if ( ch->pcdata->learned[gsn_dual] == 0 )
	{   
		send_to_char( AT_WHITE, "You cannot.\n\r", ch );
		return;
	}
	if ( CAN_WEAR( obj, ITEM_WIELD ) )
	{ 
		if ( !remove_obj( ch, WEAR_WIELD_2, fReplace ) )
			return;

		if ( get_obj_weight( obj ) > str_app[get_curr_str( ch )].wield )
		{
			send_to_char(AT_BLUE, "It is too heavy for you to dual wield.\n\r", ch );
			return;
		}

		/* Won't put update_skpell here since it is in fight.c */

		weapon_type = obj->value[3];
		if((strcmp(flag_string(weapon_flags,weapon_type),"none")))
		{
			if ( !(class_table[prime_class(ch)].objtype[weapon_type]) )
			{
				sprintf(buf, "%s's cannot use weapons that %s.\n\r", 
						class_table[prime_class(ch)].who_long, 
						flag_string(weapon_flags,weapon_type) );
				send_to_char(AT_YELLOW, buf, ch );
				return;
			}
		}
		act(AT_BLUE, "You dual wield $p.", ch, obj, NULL, TO_CHAR );
		act(AT_BLUE, "$n dual wields $p.", ch, obj, NULL, TO_ROOM );
		equip_char( ch, obj, WEAR_WIELD_2 );
	}  
	return;
}

void do_triple( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	char      arg [ MAX_INPUT_LENGTH ];
	char      buf [ MAX_STRING_LENGTH ];
	bool      fReplace;
	int       weapon_type = 0;

	fReplace = TRUE;

	one_argument( argument, arg );

	if ( IS_NPC( ch ) )
		return;
	if ( IS_AFFECTED2( ch, AFF_RAGE ) )
	{
		send_to_char( AT_RED, "You are too enraged to triple wield.\n\r", ch );
		return;
	}
	if ( get_eq_char( ch, WEAR_WIELD_2 ) && ( get_eq_char ( ch, WEAR_SHIELD ) 
				||  get_eq_char( ch, WEAR_HOLD ) ) )
	{
		send_to_char( AT_BLUE, "You cannot triple wield while you hold something in your hands.\n\r", ch );
		return;
	}

	if ( !get_eq_char( ch, WEAR_WIELD_2 ))
	{
		send_to_char( AT_BLUE, "You must be dual wielding to triple wield.\n\r", ch);
		return;
	}

	if ( arg[0] == '\0' )
	{
		send_to_char(AT_BLUE, "Triple wield what?\n\r", ch );
		return;
	}
	if ( !( obj = get_obj_carry( ch, arg ) ) )
	{
		send_to_char(AT_BLUE, "You do not have that item.\n\r", ch );
		return;
	}
	if ( ch->level < obj->level )
	{
		send_to_char( AT_WHITE, "You are too inexperienced.\n\r", ch );
		return;
	}
	if ( IS_NPC( ch ) )
	{
		send_to_char(AT_WHITE, "You cannot.\n\r", ch );
		return;
	}
	// if ( ch->race != RACE_KREEN )
	// {   
	// 	send_to_char( AT_WHITE, "You aren't a Kreen.\n\r", ch );
	// 	return;
	// }

	if ( CAN_WEAR( obj, ITEM_WIELD ) )
	{ 
		if ( !remove_obj( ch, WEAR_WIELD_3, fReplace ) )
			return;

		if ( get_obj_weight( obj ) > str_app[get_curr_str( ch )].wield )
		{
			send_to_char(AT_BLUE, "It is too heavy for you to triple wield.\n\r", ch );
			return;
		}

		/* Won't put update_skpell here since it is in fight.c */

		weapon_type = obj->value[3];
		if((strcmp(flag_string(weapon_flags,weapon_type),"none")))
		{
			if ( !(class_table[prime_class(ch)].objtype[weapon_type]) )
			{
				sprintf(buf, "%s's cannot use weapons that %s.\n\r", 
						class_table[prime_class(ch)].who_long, 
						flag_string(weapon_flags,weapon_type) );
				send_to_char(AT_YELLOW, buf, ch );
				return;
			}
		}
		act(AT_BLUE, "You triple wield $p.", ch, obj, NULL, TO_CHAR );
		act(AT_BLUE, "$n triple wields $p.", ch, obj, NULL, TO_ROOM );
		equip_char( ch, obj, WEAR_WIELD_3 );
	}  
	return;
}

void do_wear( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	char      arg [ MAX_INPUT_LENGTH ];

	one_argument( argument, arg );
	if ( is_affected( ch, gsn_nerve ) )
	{
		send_to_char( AT_YELLOW, "You cannot feel your arms.\n\r", ch );
		return;
	}

	if ( arg[0] == '\0' )
	{
		send_to_char(AT_BLUE, "Wear, wield, or hold what?\n\r", ch );
		return;
	}

	if ( !str_cmp( arg, "all" ) )
	{
		OBJ_DATA *obj_next;

		for ( obj = ch->carrying; obj; obj = obj_next )
		{
			obj_next = obj->next_content;

			if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) 
					&& !( IS_AFFECTED2( ch, AFF_RAGE ) 
						&& obj->item_type == ITEM_WEAPON ) )
				wear_obj( ch, obj, FALSE );
		}
		return;
	}
	else
	{
		if ( !( obj = get_obj_carry( ch, arg ) ) )
		{
			send_to_char(AT_BLUE, "You do not have that item.\n\r", ch );
			return;
		}
		if ( IS_AFFECTED2( ch, AFF_RAGE ) && obj->item_type == ITEM_WEAPON )
		{
			send_to_char( AT_RED, "You are too enraged to wield a weapon.\n\r", ch );
			return;
		}
		wear_obj( ch, obj, TRUE );
	}

	return;
}



void do_remove( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	char      arg [ MAX_INPUT_LENGTH ];

	one_argument( argument, arg );
	if ( is_affected( ch, gsn_nerve ) )
	{
		send_to_char( AT_YELLOW, "You cannot feel your arms.\n\r", ch );
		return;
	}

	if ( arg[0] == '\0' )
	{
		send_to_char(AT_BLUE, "Remove what?\n\r", ch );
		return;
	}

	if ( !str_cmp( arg, "all" ) )
	{
		OBJ_DATA *obj_next;
		for ( obj = ch->carrying; obj != NULL; obj = obj_next )
		{
			obj_next = obj->next_content;
			if ( obj->wear_loc != WEAR_NONE && can_see_obj( ch, obj ) )
				remove_obj( ch, obj->wear_loc, TRUE );
		}
		return;
	}
	if ( !( obj = get_obj_wear( ch, arg ) ) )
	{
		send_to_char(AT_BLUE, "You do not have that item.\n\r", ch );
		return;
	}

	remove_obj( ch, obj->wear_loc, TRUE );
	return;
}


void do_sacrifice( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	char      	arg  [ MAX_INPUT_LENGTH ];
	char      	buf  [ MAX_STRING_LENGTH];
	char      	buf1 [ MAX_STRING_LENGTH];
	int		iname;
	int		award;
	int		chance;
	int		diff;		

	/* immortal names go here */
	char * msgbuf[]=	{
		"Kjodo", "Torann","Shock","Malum","Zeriara","Rakuum","Xree","Kliq" };

	iname = number_range( 0 , 7 ); /* change second # if you add/delete
									  immortal names */
	one_argument( argument, arg );

	if ( arg[0] == '\0' || !str_cmp( arg, ch->name ) )
	{
		send_to_char( AT_WHITE,
				"The gods appreciates your offer and may accept it later.", ch );
		act( AT_WHITE, 
				"$n offers $mself to the gods, who graciously decline.",
				ch, NULL, NULL, TO_ROOM );
		return;
	}
	if ( !str_cmp( arg, "all"  ) )
	{
		OBJ_DATA *obj_next;
		for ( obj = ch->in_room->contents; obj; obj = obj_next )
		{
			obj_next = obj->next_content;
			if ( obj->deleted )
				continue;
			if ( CAN_WEAR( obj, ITEM_TAKE ) 
					&& obj->item_type != ITEM_MONEY)
				do_sacrifice( ch, strdup( obj->name ) );
		}
		return;
	}

	obj = get_obj_list( ch, arg, ch->in_room->contents );
	if ( !obj )
	{
		send_to_char( AT_WHITE, "You can't find it.\n\r", ch );
		return;
	}

	if ( !CAN_WEAR( obj, ITEM_TAKE ) )
	{
		act( AT_WHITE,
				"$p is not an acceptable sacrifice.", ch, obj, NULL, TO_CHAR );
		return;
	}

	if ( obj->item_type == ITEM_MONEY )
	{
		send_to_char( AT_WHITE, "Sorry, you cannot sacrifice money!\n\r", ch );
		return;
	}

	chance = number_range (1 , 4 );
	switch ( chance )
	{
		case 1:
			award = number_range ( 1, 6 );
			sprintf( buf1, "%d copper coin%s", award, (award != 1) ? "s" : "" );
			ch->money.copper += award;
			break;
		case 2:	
			if ( ch->hit >= MAX_HIT(ch) )
				diff = 0;
			else
				diff = 6; 
			award = number_range ( 0 , diff );
			sprintf( buf1, "%d hit point%s", award, (award != 1) ? "s" : "" );
			ch->hit += award;
			break;
		case 3:	
			if ( ch->mana >= MAX_MANA(ch))
				diff = 0;
			else
				diff = 6;

			award = number_range ( 0 , diff );
			sprintf( buf1, "%d mana", award );
			ch->mana += award;

			break;

		case 4:	
			if ( ch->move >= MAX_MOVE(ch) )
				diff = 0;
			else
				diff = 6;
			award = number_range ( 0 , diff );
			sprintf( buf1, "%d movement point%s", award, (award != 1) ? "s" : "" );
			ch->move += award;
			break;
		default:
			sprintf (buf1, "3 silver coins" );
			ch->money.silver += 3;
			break;
	}	   

	sprintf(buf,  "%s gives you %s for your sacrifice.\n\r",
			msgbuf[iname], buf1 );

	send_to_char( AT_WHITE, buf , ch );

	act( AT_WHITE, "$n sacrifices $p to the gods.", ch, obj, NULL, TO_ROOM );
	wiznet( "$N sends up $p as a burnt offering.", ch, obj, WIZ_SACCING, 0, 0 );
	extract_obj( obj );
	return;
}  




void do_quaff( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	char      arg [ MAX_INPUT_LENGTH ];

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char(AT_BLUE, "Quaff what?\n\r", ch );
		return;
	}

	if ( !( obj = get_obj_carry( ch, arg ) ) )
	{
		send_to_char(AT_BLUE, "You do not have that potion.\n\r", ch );
		return;
	}

	if ( obj->item_type != ITEM_POTION )
	{
		send_to_char(AT_BLUE, "You can quaff only potions.\n\r", ch );
		return;
	}

	act(AT_BLUE, "You quaff $p.", ch, obj, NULL ,TO_CHAR );
	act(AT_BLUE, "$n quaffs $p.", ch, obj, NULL, TO_ROOM );
	if ( obj->value[1] == skill_lookup ( "aura of peace" )
			|| obj->value[2] == skill_lookup ( "aura of peace" )
			|| obj->value[3] == skill_lookup ( "aura of peace" ) )
	{
		extract_obj ( obj );
		return;
	}
	if ( obj->value[1] == skill_lookup ( "chaos field" )
			|| obj->value[2] == skill_lookup ( "chaos field" )
			|| obj->value[3] == skill_lookup ( "chaos field" ) )
	{
		if ( obj->value[0] > 50 )
		{
			extract_obj ( obj );
			return;
		}
	}
	if ( obj->value[1] == skill_lookup ( "blade barrier" )
			|| obj->value[2] == skill_lookup ( "blade barrier" )
			|| obj->value[3] == skill_lookup ( "blade barrier" ) )
	{
		if ( obj->value[0] > 50 )
		{
			extract_obj ( obj );
			return;
		}
	}
	if ( obj->value[1] == skill_lookup ( "vibrate" )
			|| obj->value[2] == skill_lookup ( "vibrate" )
			|| obj->value[3] == skill_lookup ( "vibrate" ) )
	{
		if ( obj->value[0] > 50 )
		{
			extract_obj ( obj );
			return;
		}
	}
	if ( obj->value[1] == skill_lookup ( "iceshield" )
			|| obj->value[2] == skill_lookup ( "iceshield" )
			|| obj->value[3] == skill_lookup ( "iceshield" ) )
	{
		if ( obj->value[0] > 50 )
		{
			extract_obj ( obj );
			return;
		}
	}
	if ( obj->level > ch->level ) 
		act(AT_BLUE, "$p is too high level for you.", ch, obj, NULL, TO_CHAR );
	else
	{
		/* obj->value[0] is not used for potions */
		obj_cast_spell( obj->value[1], obj->value[0], ch, ch, NULL );
		obj_cast_spell( obj->value[2], obj->value[0], ch, ch, NULL );
		obj_cast_spell( obj->value[3], obj->value[0], ch, ch, NULL );
		oprog_use_trigger( obj, ch, ch );
	}

	extract_obj( obj );
	return;
}



void do_recite( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA  *scroll;
	OBJ_DATA  *obj;
	CHAR_DATA *victim;
	char       arg1 [ MAX_INPUT_LENGTH ];
	char       arg2 [ MAX_INPUT_LENGTH ];

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( !( scroll = get_obj_carry( ch, arg1 ) ) )
	{
		send_to_char(AT_BLUE, "You do not have that scroll.\n\r", ch );
		return;
	}

	if ( scroll->item_type != ITEM_SCROLL )
	{
		send_to_char(AT_BLUE, "You can recite only scrolls.\n\r", ch );
		return;
	}

	obj = NULL;
	if ( arg2[0] == '\0' )
	{
		victim = ch;
	}
	else
	{
		if ( !( victim = get_char_room ( ch, arg2 ) )
				&& !( obj  = get_obj_here  ( ch, arg2 ) ) )
		{
			send_to_char(AT_BLUE, "You can't find it.\n\r", ch );
			return;
		}
	}

	act(AT_BLUE, "You recite $p.", ch, scroll, NULL, TO_CHAR );
	act(AT_BLUE, "$n recites $p.", ch, scroll, NULL, TO_ROOM );

	/* Scrolls skill Thalador */
	if ( !IS_NPC( ch )
			&& !( number_percent( ) < ch->pcdata->learned[gsn_scrolls] ) )
	{
		switch ( number_bits( 3 ) )
		{
			case 0: 
			case 1:                      
			case 2:
			case 3:
				act( AT_WHITE, "You can't understand $p at all.",
						ch, scroll, NULL, TO_CHAR );
				act( AT_WHITE, "$n can't understand $p at all.",
						ch, scroll, NULL, TO_ROOM );
				return;                    
			case 4:                
			case 5:                      
			case 6:                      
				send_to_char(AT_CYAN, "You must have said something incorrectly.\n\r",
						ch );
				act(AT_CYAN,  "$n must have said something incorrectly.",
						ch, NULL,   NULL, TO_ROOM );
				act(AT_CYAN,  "$p blazes brightly, then is gone.",
						ch, scroll, NULL, TO_CHAR );
				act(AT_CYAN,  "$p blazes brightly and disappears.",
						ch, scroll, NULL, TO_ROOM );
				extract_obj( scroll );
				return;
			case 7:
				act(AT_RED,  
						"You completely botch the recitation, and $p bursts into flames!!", 
						ch, scroll, NULL, TO_CHAR );
				act(AT_RED,  "$p glows and then bursts into flame!", 
						ch, scroll, NULL, TO_ROOM );
				/*
				 * damage( ) call after extract_obj in case the damage would
				 * have extracted ch.  This is okay because we merely mark
				 * obj->deleted; it still retains all values until list_update.
				 * Sloppy?  Okay, create another integer variable. ---Thelonius
				 */
				extract_obj( scroll );
				damage( ch, ch, scroll->level, gsn_scrolls );
				return;
		}
	}


	if ( scroll->level > ch->level ) 
		act(AT_BLUE, "$p is too high level for you.", ch, scroll, NULL, TO_CHAR );
	else
	{
		/* scroll->value[0] is not used for scrolls */
		obj_cast_spell( scroll->value[1], scroll->level, ch, victim, obj );
		obj_cast_spell( scroll->value[2], scroll->level, ch, victim, obj );
		obj_cast_spell( scroll->value[3], scroll->level, ch, victim, obj );
		if ( victim )
			oprog_use_trigger( scroll, ch, victim );
		else
			oprog_use_trigger( scroll, ch, obj );
	}
	update_skpell(ch, gsn_scrolls);
	extract_obj( scroll );
	return;
}



void do_brandish( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA  *staff;
	CHAR_DATA *vch;
	int        sn;

	if ( !( staff = get_eq_char( ch, WEAR_HOLD ) ) )
	{
		send_to_char(AT_BLUE, "You hold nothing in your hand.\n\r", ch );
		return;
	}

	if ( staff->item_type != ITEM_STAFF )
	{
		send_to_char(AT_BLUE, "You can brandish only with a staff.\n\r", ch );
		return;
	}

	if ( ( sn = staff->value[3] ) < 0
			|| !is_sn(sn)
			|| skill_table[sn].spell_fun == 0 )
	{
		bug( "Do_brandish: bad sn %d.", sn );
		return;
	}

	WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

	if ( ( staff->value[2] > 0 ) || ( staff->value[1] == -1 ) )
	{
		CHAR_DATA *vch_next;

		act(AT_BLUE, "You brandish $p.",  ch, staff, NULL, TO_CHAR );
		act(AT_BLUE, "$n brandishes $p.", ch, staff, NULL, TO_ROOM );

		/* Staves skill by Thalador */
		if ( !IS_NPC( ch ) 
				&& !( number_percent( ) < ch->pcdata->learned[gsn_staves] ) )
		{ 
			switch ( number_bits( 3 ) )
			{
				case 0: 
				case 1:                      
				case 2:                      
				case 3: 
					act( AT_CYAN, "You are unable to invoke the power of $p.",
							ch, staff, NULL, TO_CHAR );
					act( AT_CYAN, "$n is unable to invoke the power of $p.",
							ch, staff, NULL, TO_ROOM );
					return;                    
				case 4:                
				case 5:                      
				case 6:                      
					act( AT_CYAN, "You summon the power of $p, but it fizzles away.",
							ch, staff, NULL, TO_CHAR );
					act( AT_CYAN, "$n summons the power of $p, but it fizzles away.",
							ch, staff, NULL, TO_ROOM );
					if ( --staff->value[2] <= 0 )
					{
						act( AT_CYAN, "$p blazes bright and is gone.",
								ch, staff, NULL, TO_CHAR );
						act( AT_CYAN, "$p blazes bright and is gone.",
								ch, staff, NULL, TO_ROOM );
						extract_obj( staff );
					}
					return;
				case 7:
					act( AT_CYAN, "You can't control the power of $p, and it shatters!",
							ch, staff, NULL, TO_CHAR );
					act( AT_CYAN, "$p shatters into tiny pieces!", 
							ch, staff, NULL, TO_ROOM );
					/*
					 * damage( ) call after extract_obj in case the damage would
					 * have extracted ch.  This is okay because we merely mark
					 * obj->deleted; it still retains all values until list_update.
					 * Sloppy?  Okay, create another integer variable. ---Thelonius
					 */
					extract_obj( staff );
					damage( ch, ch, staff->level, gsn_staves );
					return;
			}
		}
		for ( vch = ch->in_room->people; vch; vch = vch_next )
		{
			vch_next = vch->next_in_room;

			if ( vch->deleted )
				continue;

			switch ( skill_table[sn].target )
			{
				default:
					bug( "Do_brandish: bad target for sn %d.", sn );
					return;

				case TAR_IGNORE:
					if ( vch != ch )
						continue;
					break;

				case TAR_CHAR_OFFENSIVE:
					if ( IS_NPC( ch ) ? IS_NPC( vch ) : !IS_NPC( vch ) )
						continue;
					break;

				case TAR_CHAR_DEFENSIVE:
					if ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
						continue;
					break;

				case TAR_CHAR_SELF:
					if ( vch != ch )
						continue;
					break;
			}

			/* staff->value[0] is not used for staves */
			obj_cast_spell( staff->value[3], staff->level, ch, vch, NULL );
			oprog_use_trigger( staff, ch, vch );
		}
	}
	if (!(staff->value[1] == -1 ))
		if ( --staff->value[2] <= 0 )
		{
			act(AT_WHITE, "Your $p blazes bright and is gone.", ch, staff, NULL, TO_CHAR );
			act(AT_WHITE, "$n's $p blazes bright and is gone.", ch, staff, NULL, TO_ROOM );
			extract_obj( staff );
		}
	update_skpell(ch,gsn_staves);
	return;
}


void do_stare ( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA  *wand;
	OBJ_DATA  *obj;
	CHAR_DATA *victim;
	char       arg [ MAX_INPUT_LENGTH ];

	one_argument( argument, arg );
	if ( arg[0] == '\0' && !ch->fighting )
	{
		send_to_char(AT_BLUE, "Stare at whom or what?\n\r", ch );
		return;
	}

	if ( !( wand = get_eq_char( ch, WEAR_IN_EYES ) ) )
	{
		send_to_char(AT_BLUE, "You have no lenses in your eyes.\n\r", ch );
		return;
	}

	if ( wand->item_type != ITEM_LENSE )
	{
		send_to_char(AT_BLUE, "You can only stare with magical lenses.\n\r", ch );
		return;
	}

	obj = NULL;
	if ( arg[0] == '\0' )
	{
		if ( ch->fighting )
		{
			victim = ch->fighting;
		}
		else
		{
			send_to_char(AT_BLUE, "Stare at whom or what?\n\r", ch );
			return;
		}
	}
	else
	{
		if ( !( victim = get_char_room ( ch, arg ) )
				&& !( obj  = get_obj_here  ( ch, arg ) ) )
		{
			send_to_char(AT_BLUE, "You can't find it.\n\r", ch );
			return;
		}
	}

	WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

	if ( ( wand->value[2] > 0 ) || ( wand->value[1] == -1 ) )
	{
		if ( victim )
		{
			act(AT_BLUE, "You stare at $N with $p.", ch, wand, victim, TO_CHAR );
			act(AT_BLUE, "$n stares at $N with $p.", ch, wand, victim, TO_ROOM );
		}
		else
		{
			act(AT_BLUE, "You stare at $P with $p.", ch, wand, obj, TO_CHAR );
			act(AT_BLUE, "$n stares at $P with $p.", ch, wand, obj, TO_ROOM );
		}

		/* wand->value[0] is not used for lenses */
		obj_cast_spell( wand->value[3], wand->level, ch, victim, obj );
		if ( victim )
			oprog_use_trigger( wand, ch, victim );
		else
			oprog_use_trigger( wand, ch, obj );
	}

	if (!(wand->value[1] == -1 ) )
		if ( --wand->value[2] <= 0 )
		{
			act(AT_WHITE, "Your $p melts in your eyes.", ch, wand, NULL, TO_CHAR );
			act(AT_WHITE, "$n's $p melts in $s eyes.", ch, wand, NULL, TO_ROOM );
			extract_obj( wand );
		}

	return;
}

void do_zap( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA  *wand;
	OBJ_DATA  *obj;
	CHAR_DATA *victim;
	char       arg [ MAX_INPUT_LENGTH ];

	one_argument( argument, arg );
	if ( arg[0] == '\0' && !ch->fighting )
	{
		send_to_char(AT_BLUE, "Zap whom or what?\n\r", ch );
		return;
	}

	if ( !( wand = get_eq_char( ch, WEAR_HOLD ) ) )
	{
		send_to_char(AT_BLUE, "You hold nothing in your hand.\n\r", ch );
		return;
	}

	if ( wand->item_type != ITEM_WAND )
	{
		send_to_char(AT_BLUE, "You can zap only with a wand.\n\r", ch );
		return;
	}

	obj = NULL;
	if ( arg[0] == '\0' )
	{
		if ( ch->fighting )
		{
			victim = ch->fighting;
		}
		else
		{
			send_to_char(AT_BLUE, "Zap whom or what?\n\r", ch );
			return;
		}
	}
	else
	{
		if ( !( victim = get_char_room ( ch, arg ) )
				&& !( obj  = get_obj_here  ( ch, arg ) ) )
		{
			send_to_char(AT_BLUE, "You can't find it.\n\r", ch );
			return;
		}
	}

	WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

	if ( ( wand->value[2] > 0 ) || ( wand->value[1] == -1 ) )
	{
		if ( victim )
		{
			act(AT_BLUE, "You zap $N with $p.", ch, wand, victim, TO_CHAR );
			act(AT_BLUE, "$n zaps $N with $p.", ch, wand, victim, TO_ROOM );
		}
		else
		{
			act(AT_BLUE, "You zap $P with $p.", ch, wand, obj, TO_CHAR );
			act(AT_BLUE, "$n zaps $P with $p.", ch, wand, obj, TO_ROOM );
		}

		/* Wands skill by Thalador */
		if ( !IS_NPC( ch ) 
				&& !( number_percent( ) < ch->pcdata->learned[gsn_wands] ) )
		{ 
			switch ( number_bits( 3 ) )
			{
				case 0: 
				case 1:                      
				case 2:                      
				case 3: 
					act( AT_CYAN, "You are unable to invoke the power of $p.",
							ch, wand, NULL, TO_CHAR );
					act( AT_CYAN, "$n is unable to invoke the power of $p.",
							ch, wand, NULL, TO_ROOM );
					return;                    
				case 4:                
				case 5:                      
				case 6:                      
					act( AT_CYAN, "You summon the power of $p, but it fizzles away.",
							ch, wand, NULL, TO_CHAR );
					act( AT_CYAN, "$n summons the power of $p, but it fizzles away.",
							ch, wand, NULL, TO_ROOM );
					if ( --wand->value[2] <= 0 )
					{
						act( AT_CYAN, "$p blazes bright and is gone.",
								ch, wand, NULL, TO_CHAR );
						act( AT_CYAN, "$p blazes bright and is gone.",
								ch, wand, NULL, TO_ROOM );
						extract_obj( wand );
					}
					return;
				case 7:
					act( AT_CYAN, "You can't control the power of $p, and it explodes!",
							ch, wand, NULL, TO_CHAR );
					act( AT_CYAN, "$p explodes into fragments!", 
							ch, wand, NULL, TO_ROOM );
					extract_obj( wand );
					damage( ch, ch, wand->level, gsn_wands );
					return;
			}
		}


		/* wand->value[0] is not used for wands */
		obj_cast_spell( wand->value[3], wand->level, ch, victim, obj );
		if ( victim )
			oprog_use_trigger(wand, ch, victim );
		else
			oprog_use_trigger( wand, ch, obj );
	}

	if (!(wand->value[1] == -1 ) )
		if ( --wand->value[2] <= 0 )
		{
			act(AT_WHITE, "Your $p explodes into fragments.", ch, wand, NULL, TO_CHAR );
			act(AT_WHITE, "$n's $p explodes into fragments.", ch, wand, NULL, TO_ROOM );
			extract_obj( wand );
		}
	update_skpell(ch, gsn_wands);
	return;
}



void do_steal( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA  *obj;
	CHAR_DATA *victim;
	char       buf  [ MAX_STRING_LENGTH ];
	char       arg1 [ MAX_INPUT_LENGTH  ];
	char       arg2 [ MAX_INPUT_LENGTH  ];
	int        percent;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char(AT_BLOOD, "Steal what from whom?\n\r", ch );
		return;
	}

	if ( !( victim = get_char_room( ch, arg2 ) ) )
	{
		send_to_char(AT_BLOOD, "They aren't here.\n\r", ch );
		return;
	}
	if ( IS_SET(victim->in_room->room_flags, ROOM_SAFE) )
	{
		send_to_char(AT_BLOOD, "You cannot steal in a safe room.\n\r", ch );
		return;
	}
	if ( abs(ch->level - victim->level) > 5 && ( !IS_NPC(victim) ) )
	{
		send_to_char(AT_WHITE, "That is not in the pkill range... valid range is +/- 5 levels.\n\r", ch );
		return;
	}
	if ( victim == ch )
	{
		send_to_char(AT_BLOOD, "That's pointless.\n\r", ch );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_steal].beats );
	percent  = number_percent( ) + ( IS_AWAKE( victim ) ? 10 : -50 );

	if ( ch->level + 5 < victim->level
			|| ( victim->position == POS_FIGHTING 
				&& victim->fighting != ch )
			|| ( !IS_NPC( ch ) && percent > ch->pcdata->learned[gsn_steal] ) )
	{
		/*
		 * Failure.
		 */
		send_to_char(AT_RED, "Oops.\n\r", ch );
		if ( ch->guild && !strcmp( ch->guild->name, "MERCENARY" ) )
		{
			send_to_char( AT_RED, "You are kicked out of &rMERCENARY&R for treason!\n\r", ch );
			ch->guild = NULL;
			ch->guild_rank = 0;
		}
		act(AT_RED, "$n tried to steal from you.\n\r", ch, NULL, victim, TO_VICT    );
		act(AT_RED, "$n tried to steal from $N.\n\r",  ch, NULL, victim, TO_NOTVICT );
		sprintf( buf, "%s is a bloody thief!", ch->name );
		do_shout( victim, buf );
		if ( !IS_NPC( ch ) )
		{
			if ( IS_NPC( victim ) )
			{
				multi_hit( victim, ch, TYPE_UNDEFINED );
			}
			else
			{
				sprintf( buf, "$N tried to steal from %s.", victim->name );
				wiznet( buf, ch, NULL, WIZ_FLAGS, 0, 0 );
				if ( !IS_SET( ch->act, PLR_THIEF ) )
				{
					SET_BIT( ch->act, PLR_THIEF );
					send_to_char(AT_RED, "*** You are now a THIEF!! ***\n\r", ch );
					save_char_obj( ch );
				}
			}
		}

		return;
	}

	if (   !str_prefix( arg1, "coins" ) )
		/*	|| !str_cmp   ( arg1, "gold"  ) )  */
	{
		MONEY_DATA amount;

		amount.gold = victim->money.gold * number_range( 1, 10 ) / 100;
		if ( amount.gold <= 0 )
		{
			amount.silver = victim->money.silver * number_range( 1, 10 ) / 100;
			if ( amount.silver <= 0 )
				amount.copper = victim->money.copper * number_range( 1, 10 ) / 100;	
			if ( amount.copper <= 0 )
			{
				send_to_char(AT_BLOOD, "You couldn't get any coins.\n\r", ch );
				return;
			}
		}
		add_money( &ch->money, &amount );  
		sub_money( &victim->money, &amount );

		sprintf( buf, "Jackpot!  You got %s\n\r", money_string( &amount ) );
		send_to_char(AT_RED, buf, ch );
		return;
	}

if ( !( obj = get_obj_carry( victim, arg1 ) ) )
{
	send_to_char(AT_BLOOD, "You can't find it.\n\r", ch );
	return;
}

	if ( !can_drop_obj( ch, obj )
			|| IS_SET( obj->extra_flags, ITEM_INVENTORY )
			|| obj->level > ch->level )
{
	send_to_char(AT_BLOOD, "You can't pry it away.\n\r", ch );
	return;
}

if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
{
	send_to_char(AT_BLOOD, "You have your hands full.\n\r", ch );
	return;
}

if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
{
	send_to_char(AT_BLOOD, "You can't carry that much weight.\n\r", ch );
	return;
}

obj_from_char( obj );
obj_to_char( obj, ch );
send_to_char(AT_RED, "Ok.\n\r", ch );

update_skpell( ch, gsn_steal );

return;
}



/*
 * Shopping commands.
 */
CHAR_DATA *find_keeper( CHAR_DATA *ch )
{
	CHAR_DATA *keeper;
	SHOP_DATA *pShop;
	char       buf [ MAX_STRING_LENGTH ];

	pShop = NULL;
	for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
	{
		if ( IS_NPC( keeper ) && ( pShop = keeper->pIndexData->pShop ) )
			break;
	}

	if ( !pShop || IS_AFFECTED( keeper, AFF_CHARM ) )
	{
		send_to_char(C_DEFAULT, "You can't do that here.\n\r", ch );
		return NULL;
	}

	/*
	 * Undesirables.
	 */
	if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_KILLER ) )
	{
		do_say( keeper, "Killers are not welcome!" );
		sprintf( buf, "%s the KILLER is over here!\n\r", ch->name );
		do_shout( keeper, buf );
		return NULL;
	}

	if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_THIEF ) )
	{
		do_say( keeper, "Thieves are not welcome!" );
		sprintf( buf, "%s the THIEF is over here!\n\r", ch->name );
		do_shout( keeper, buf );
		return NULL;
	}

	/*
	 * Shop hours.
	 */
	if ( time_info.hour < pShop->open_hour )
	{
		do_say( keeper, "Sorry, come back later." );
		return NULL;
	}

	if ( time_info.hour > pShop->close_hour )
	{
		do_say( keeper, "Sorry, come back tomorrow." );
		return NULL;
	}

	/*
	 * Invisible or hidden people.
	 */
	if ( !can_see( keeper, ch ) )
	{
		do_say( keeper, "I don't trade with folks I can't see." );
		return NULL;
	}

	return keeper;
}


MONEY_DATA *get_cost( CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy )
{
	SHOP_DATA *pShop;
	static MONEY_DATA new_cost;

	new_cost.gold = new_cost.silver = new_cost.copper = 0;

	if ( !obj || !( pShop = keeper->pIndexData->pShop ) )
		return NULL;


	if ( fBuy )
	{
		new_cost.gold   = obj->cost.gold   * pShop->profit_buy / 100;
		new_cost.silver = obj->cost.silver * pShop->profit_buy / 100;
		new_cost.copper = obj->cost.copper * pShop->profit_buy / 100;
	}
	else 
	{
		OBJ_DATA *obj2;
		int       itype;

		for ( itype = 0; itype < MAX_TRADE; itype++ )
		{
			if ( obj->item_type == pShop->buy_type[itype] )
			{
				new_cost.gold   = obj->cost.gold   * pShop->profit_sell / 100;
				new_cost.silver = obj->cost.silver * pShop->profit_sell / 100;
				new_cost.copper = obj->cost.copper * pShop->profit_sell / 100;
				break;
			}
		}

		for ( obj2 = keeper->carrying; obj2; obj2 = obj2->next_content )
		{
			if ( obj->pIndexData == obj2->pIndexData )
			{
				new_cost.gold   = ( new_cost.gold   > 0 ) ? new_cost.gold / 2 : 0;
				new_cost.silver = ( new_cost.silver > 0 ) ? new_cost.silver / 2 : 0;
				new_cost.copper = ( new_cost.copper > 0 ) ? new_cost.copper / 2 : 0;
			}
		}
	}

	if ( obj->item_type == ITEM_STAFF || obj->item_type == ITEM_WAND )
	{
		new_cost.gold   *= ( obj->value[2] / obj->value[1] );
		new_cost.silver *= ( obj->value[2] / obj->value[1] );
		new_cost.copper *= ( obj->value[2] / obj->value[1] );
	}

	/* Check for negative costs */  
	if ( new_cost.gold < 0 || new_cost.silver < 0 || new_cost.copper < 0 )
		return NULL;

	return &new_cost;
}

void do_buy( CHAR_DATA *ch, char *argument )
{
	char arg [ MAX_INPUT_LENGTH ];
	char arg1[MAX_STRING_LENGTH];
	int noi = 1;
	int in = 1;
	MONEY_DATA pet_cost;
	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg1 );

	if ( arg[0] == '\0' )
	{
		send_to_char(AT_CYAN, "Buy what?\n\r", ch );
		return;
	}

	if ( arg1[0] == '\0' )
		noi = 1;
	else
		noi = atoi( arg1 );

	if ( IS_SET( ch->in_room->room_flags, ROOM_PET_SHOP ) )
	{
		CHAR_DATA       *pet;
		ROOM_INDEX_DATA *pRoomIndexNext;
		ROOM_INDEX_DATA *in_room;
		char             buf [ MAX_STRING_LENGTH ];

		if ( IS_NPC( ch ) )
			return;

		if ( noi > 1 )
		{
			send_to_char( AT_CYAN, "You can only buy one pet at a time.\n\r",ch);
			return;
		}

		pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );
		if ( !pRoomIndexNext )
		{
			bug( "Do_buy: bad pet shop at vnum %d.", ch->in_room->vnum );
			send_to_char(AT_CYAN, "Sorry, you can't buy that here.\n\r", ch );
			return;
		}

		in_room     = ch->in_room;
		ch->in_room = pRoomIndexNext;
		pet         = get_char_room( ch, arg );
		ch->in_room = in_room;

		if ( !pet || !IS_SET( pet->act, ACT_PET ) )
		{
			send_to_char(AT_CYAN, "Sorry, you can't buy that here.\n\r", ch );
			return;
		}

		if ( IS_SET( ch->act, PLR_BOUGHT_PET ) )
		{
			send_to_char(AT_CYAN, "You already bought one pet this level.\n\r", ch );
			return;
		}

		pet_cost.silver = pet_cost.copper = 0;	

		pet_cost.gold = ( 10 * pet->level * pet->level );

		/* Convert ch coins and pet cost to copper to compare */
		if ( ( ch->money.gold*C_PER_G + ch->money.silver*S_PER_G + 
					ch->money.copper ) < (pet_cost.gold*100) )
		{
			send_to_char(AT_CYAN, "You can't afford it.\n\r", ch );
			return;  
		}

		if ( ch->level < pet->level )
		{
			send_to_char(AT_CYAN, "You're not ready for this pet.\n\r", ch );
			return;
		}

		spend_money( &ch->money, &pet_cost );

		pet	  = create_mobile( pet->pIndexData );
		if ( ch->level < LEVEL_HERO )
			SET_BIT( ch->act,          PLR_BOUGHT_PET );
		SET_BIT( pet->act,         ACT_PET        );
		SET_BIT( pet->affected_by, AFF_CHARM      );

		argument = one_argument( argument, arg );
		if ( arg[0] != '\0' )
		{
			sprintf( buf, "%s %s", pet->name, arg );
			free_string( pet->name );
			pet->name = str_dup( buf );
		}

		sprintf( buf, "%sA neck tag says 'I belong to %s'.\n\r",
				pet->description, ch->name );
		free_string( pet->description );
		pet->description = str_dup( buf );

		char_to_room( pet, ch->in_room );
		add_follower( pet, ch );
		send_to_char(AT_WHITE, "Enjoy your pet.\n\r", ch );
		act(AT_WHITE, "$n bought $N as a pet.", ch, NULL, pet, TO_ROOM );
		return;
	}
	else
	{
		OBJ_DATA  *obj;
		CHAR_DATA *keeper;
		MONEY_DATA *cost;
		bool	   haggled = FALSE;
		if ( !( keeper = find_keeper( ch ) ) )
			return;   

		obj  = get_obj_carry( keeper, arg );
		cost = get_cost( keeper, obj, TRUE );

		if ( !cost || ( cost->gold == 0 && cost->silver == 0 && cost->copper == 0 ) ||
				!can_see_obj( ch, obj ) )
		{
			act(AT_CYAN, "$n tells you 'I don't sell that -- try 'list''.",
					keeper, NULL, ch, TO_VICT );
			ch->reply = keeper;
			return;
		}

		cost->gold   *= noi;
		cost->silver *= noi;
		cost->copper *= noi; 

		if ( !IS_NPC( ch )
				&& ch->pcdata->learned[gsn_haggle] > 0
				&& number_percent( ) < 75 )
		{                    
			cost->copper *= 0.85;
			cost->copper += (cost->silver * 0.15); 
			cost->silver *= 0.85;
			cost->silver += (cost->gold   * 0.15); 
			cost->gold   *= 0.85;
			haggled = TRUE;
			update_skpell( ch, gsn_haggle );
		}         

		if ( ch->charisma > 24 )
		{
			cost->copper *= 0.80;
			cost->silver *= 0.80;
			cost->gold   *= 0.80;
		}
		else if ( ch->charisma > 19 && ch->charisma < 25 ) 
		{
			cost->copper *= 0.85;
			cost->silver *= 0.85;
			cost->gold   *= 0.85;
		}
		else if ( ch->charisma < 15 )
		{
			cost->copper += (cost->copper * 0.20);
			cost->silver += (cost->silver * 0.20);
			cost->gold   += (cost->gold   * 0.20);
		}
		else if ( ch->charisma < 20 )
		{
			cost->copper += (cost->copper * 0.15);
			cost->silver += (cost->silver * 0.15);
			cost->gold   += (cost->gold   * 0.15);
		}

		if ( !IS_SET( obj->extra_flags, ITEM_INVENTORY ) && noi > 1 )
		{
			send_to_char( AT_WHITE, "You can only buy one of those at a time.\n\r", ch );
			return;
		}

		if ( noi < 1 )
		{
			send_to_char( AT_WHITE, "Buy how many?\n\r", ch );
			return;
		}  

		if ( ( ch->money.gold*C_PER_G + ch->money.silver*S_PER_G +
					ch->money.copper ) < ( cost->gold*C_PER_G + cost->silver*S_PER_G +
						cost->copper ) )
		{
			if ( noi == 1 )
				sprintf( log_buf, "$n tells you 'You can't afford to buy $p." );
			else
				sprintf( log_buf, "$n tells you 'You can't afford to buy %d $ps.",
						noi );
			act(AT_CYAN, log_buf, keeper, obj, ch, TO_VICT );
			ch->reply = keeper;
			return;  
		}

		if ( obj->level > ch->level )
		{
			act(AT_CYAN, "$n tells you 'You can't use $p yet'.",
					keeper, obj, ch, TO_VICT );
			ch->reply = keeper;
			return;
		}          
		if ( ch->carry_number + ( get_obj_number( obj ) * noi ) > can_carry_n( ch ) )
		{
			send_to_char(AT_CYAN, "You can't carry that many items.\n\r", ch );
			return;
		}

		if ( ch->carry_weight + ( get_obj_weight( obj ) * noi ) > can_carry_w( ch ) )
		{
			send_to_char(AT_CYAN, "You can't carry that much weight.\n\r", ch );
			return;   
		}


		if ( haggled )
		{
			sprintf( log_buf, "You haggle with $N and pay %s", money_string( cost ) );
			act( AT_GREY, log_buf, ch, NULL, keeper, TO_CHAR );
			act( AT_GREY, "$n haggles with $N.", ch , NULL, keeper, TO_ROOM );
		}

		if ( noi == 1 )
		{                
			act(AT_WHITE, "You buy $p.", ch, obj, NULL, TO_CHAR );
			act(AT_WHITE, "$n buys $p.", ch, obj, NULL, TO_ROOM );
		}     
		else
		{
			sprintf( log_buf, "You buy %d $p%s.", noi, ( noi > 1 ) ? "s" : "" );
			act(AT_WHITE, log_buf, ch, obj, NULL, TO_CHAR );
			sprintf( log_buf, "$n buys %d $p%s.", noi, ( noi > 1 ) ? "s" : "" );
			act(AT_WHITE, log_buf, ch, obj, NULL, TO_ROOM );
		}
		add_money( &keeper->money, cost );
		spend_money( &ch->money, cost );	

		if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
		{
			for ( in = 1; in <= noi; in++ )
			{
				obj = create_object( obj->pIndexData, obj->level );
				obj_to_char( obj, ch );
			}
		}
		else
		{
			obj_from_char( obj );
			obj_to_char( obj, ch );
		}

		oprog_buy_trigger( obj, ch, keeper );
		return;
	}
}

void do_list( CHAR_DATA *ch, char *argument )
{
	char buf  [ MAX_STRING_LENGTH   ];
	char buf1 [ MAX_STRING_LENGTH*4 ];

	buf1[0] = '\0';

	if ( ch->in_room->vnum == ROOM_VNUM_ARTIFACTOR )
	{
		send_to_char( AT_YELLOW, "ARTIFACTOR PRICES\n\r", ch);
		send_to_char( AT_WHITE, "\n\rMake item Indestructable:\n\r", ch );
		send_to_char( AT_BLUE, "25000 gold coins per item level.\n\r", ch );
		send_to_char( C_DEFAULT, "Indestructable <item>\n\r", ch );
		send_to_char( AT_WHITE, "\n\rRemake item:\n\r", ch );
		send_to_char( AT_BLUE, "100000 gold coins.\n\r", ch);
		send_to_char( C_DEFAULT, "Remake <item>\n\r", ch );
		return;
	}
	else if ( IS_SET( ch->in_room->room_flags, ROOM_PET_SHOP ) )
	{
		CHAR_DATA       *pet;
		ROOM_INDEX_DATA *pRoomIndexNext;
		bool             found;

		pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );
		if ( !pRoomIndexNext )
		{
			bug( "Do_list: bad pet shop at vnum %d.", ch->in_room->vnum );
			send_to_char(AT_CYAN, "You can't do that here.\n\r", ch );
			return;
		}

		found = FALSE;
		for ( pet = pRoomIndexNext->people; pet; pet = pet->next_in_room )
		{
			if ( IS_SET( pet->act, ACT_PET ) )
			{
				if ( !found )
				{
					found = TRUE;
					strcat( buf1, "Pets for sale:\n\r" );
				}
				sprintf( buf, "[%2d] %5d Gold - %s\n\r",
						pet->level,
						10 * pet->level * pet->level,
						pet->short_descr );
				strcat( buf1, buf );
			}
		}
		if ( !found )
			send_to_char(AT_CYAN, "Sorry, we're out of pets right now.\n\r", ch );

		send_to_char(AT_CYAN, buf1, ch );
		return;
	}
	else
	{
		OBJ_DATA  *obj;
		CHAR_DATA *keeper;
		char       arg [ MAX_INPUT_LENGTH ];
		bool       found;
		MONEY_DATA *amt;
		one_argument( argument, arg );

		if ( !( keeper = find_keeper( ch ) ) )
			return;

		found = FALSE;
		for ( obj = keeper->carrying; obj; obj = obj->next_content )
		{
			amt = get_cost( keeper, obj, TRUE );

			if ( obj->wear_loc != WEAR_NONE || !amt )
				continue;
			if ( can_see_obj( ch, obj )
					&& ( arg[0] == '\0' || is_name(ch, arg, obj->name ) ) )
			{
				if ( !found )
				{
					found = TRUE;
					strcat( buf1, "[Lv Gold Silv Copp] Item\n\r" );
				}
				sprintf( buf, "[%2d %4d %4d %4d] %s.\n\r",
						obj->level, amt->gold, amt->silver, amt->copper,
						capitalize( obj->short_descr ) );
				strcat( buf1, buf );
			}
		}

		if ( !found )
		{
			if ( arg[0] == '\0' )
				send_to_char(AT_CYAN, "You can't buy anything here.\n\r", ch );
			else
				send_to_char(AT_CYAN, "You can't buy that here.\n\r", ch );
			return;
		}

		send_to_char(AT_CYAN, buf1, ch );
		return;
	}
}



void do_sell( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA  *obj;
	CHAR_DATA *keeper;
	char       buf [ MAX_STRING_LENGTH ];
	char       arg [ MAX_INPUT_LENGTH  ];
	MONEY_DATA *cost;

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char(AT_CYAN, "Sell what?\n\r", ch );
		return;
	}

	if ( !( keeper = find_keeper( ch ) ) )
		return;

	if ( !( obj = get_obj_carry( ch, arg ) ) )
	{
		act(AT_CYAN, "$n tells you 'You don't have that item'.",
				keeper, NULL, ch, TO_VICT );
		ch->reply = keeper;
		return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
		send_to_char(AT_CYAN, "You can't let go of it.\n\r", ch );
		return;
	}

	if ( !can_see_obj( keeper, obj ) )
	{
		act(AT_CYAN, "$n tells you 'I can't see that item'.",
				keeper, NULL, ch, TO_VICT );
		ch->reply = keeper;
		return;
	}

	cost = get_cost( keeper, obj, FALSE );

	if ( ( !cost ) || ( cost->gold == 0 && cost->silver == 0 && cost->copper == 0 ) ||
			obj->level >= LEVEL_IMMORTAL )
	{
		act(AT_CYAN, "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
		return;
	}

	if ( IS_SET( obj->extra_flags, ITEM_POISONED ) )
	{
		act(AT_CYAN, "$n tells you 'I won't buy that!  It's poisoned!'",
				keeper, NULL, ch, TO_VICT );
		ch->reply = keeper;
		return;
	}
	if ( !IS_NPC( ch )
			&& ch->pcdata->learned[gsn_haggle] > 0 )
	{
		cost->gold   = obj->cost.gold;
		cost->silver = obj->cost.silver;
		cost->copper = obj->cost.copper;
		update_skpell( ch, gsn_haggle );
	}

	if ( ch->charisma > 24 )
	{
		cost->gold   += ( cost->gold * 0.05 );
		cost->silver += ( cost->silver * 0.05 );
		cost->copper += ( cost->copper * 0.05 );
	}   
	if ( ch->charisma > 19 && ch->charisma < 24 )
	{
		cost->gold   += ( cost->gold * 0.02 );
		cost->silver += ( cost->silver * 0.02 );
		cost->copper += ( cost->copper * 0.02 );
	}
	if ( ch->charisma < 15 )
	{
		cost->gold   -= ( cost->gold * 0.20 );
		cost->silver -= ( cost->silver * 0.20 );
		cost->copper -= ( cost->copper * 0.20 );
	}
	if ( ch->charisma < 20 )
	{
		cost->gold   -= ( cost->gold * 0.15 );
		cost->silver -= ( cost->silver * 0.15 );
		cost->copper -= ( cost->copper * 0.15 );
	}

	sprintf( buf, "You sell $p for %s", money_string( cost ) );
	act(AT_WHITE, buf, ch, obj, NULL, TO_CHAR );
	act(AT_WHITE, "$n sells $p.", ch, obj, NULL, TO_ROOM );

	add_money( &ch->money, cost );
	sub_money( &keeper->money, cost );
	if ( keeper->money.gold < 0 ) 
		keeper->money.gold = 0;
	if ( keeper->money.silver < 0 )
		keeper->money.silver = 0;
	if ( keeper->money.copper < 0 )
		keeper->money.copper = 0;

	oprog_sell_trigger( obj, ch, keeper );

	if ( obj->item_type == ITEM_TRASH )
	{
		extract_obj( obj );
	}
	else
	{
		obj_from_char( obj );
		obj_to_char( obj, keeper );
	}

	return;
}



void do_value( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA  *obj;
	CHAR_DATA *keeper;
	char       buf [ MAX_STRING_LENGTH ];
	char       arg [ MAX_INPUT_LENGTH  ];
	MONEY_DATA *cost;

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char(AT_CYAN, "Value what?\n\r", ch );
		return;
	}

	if ( !( keeper = find_keeper( ch ) ) )
		return;

	if ( !( obj = get_obj_carry( ch, arg ) ) )
	{
		act(AT_CYAN, "$n tells you 'You don't have that item'.",
				keeper, NULL, ch, TO_VICT );
		ch->reply = keeper;
		return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
		send_to_char(AT_CYAN, "You can't let go of it.\n\r", ch );
		return;
	}

	if ( !can_see_obj( keeper, obj ) )
	{
		act(AT_CYAN, "$n tells you 'You are offering me an imaginary object!?!?'.",
				keeper, NULL, ch, TO_VICT );
		ch->reply = keeper;
		return;
	}
	cost = get_cost( keeper, obj, FALSE );

	if ( ( cost->gold + cost->silver + cost->copper ) <= 0 )
	{
		act(AT_CYAN, "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
		return;
	}

	if ( IS_SET( obj->extra_flags, ITEM_POISONED ) )
	{
		act(AT_CYAN, "$n tells you 'I won't buy that!  It's poisoned!'",
				keeper, NULL, ch, TO_VICT );
		ch->reply = keeper;
		return;
	}
	sprintf( buf, "$n tells you 'I'll buy $p for %s'", money_string( cost ) );
	act(AT_WHITE, buf, keeper, obj, ch, TO_VICT );
	ch->reply = keeper;

	return;
}

/* Poison weapon by Thelonius for EnvyMud */
void do_poison_weapon( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	char      arg [ MAX_INPUT_LENGTH ];

	if ( !IS_NPC( ch )                                                  
			&& !can_use_skpell( ch, gsn_poison_weapon ) )
	{                                          
		send_to_char(AT_DGREEN, "What do you think you are, a thief?\n\r", ch );
		return;
	}

	one_argument( argument, arg );

	if ( arg[0] == '\0' )                                              
	{ send_to_char(AT_DGREEN, "What are you trying to poison?\n\r",    ch ); return; }
	if ( ch->fighting )                                       
	{ send_to_char(AT_DGREEN, "While you're fighting?  Nice try.\n\r", ch ); return; }
	if ( !( obj = get_obj_carry( ch, arg ) ) )
	{ send_to_char(AT_DGREEN, "You do not have that weapon.\n\r",      ch ); return; }
	if ( obj->item_type != ITEM_WEAPON )
	{ send_to_char(AT_DGREEN, "That item is not a weapon.\n\r",        ch ); return; }
	if ( IS_OBJ_STAT( obj, ITEM_POISONED ) )
	{ send_to_char(AT_DGREEN, "That weapon is already poisoned.\n\r",  ch ); return; }

	/* Now we have a valid weapon...check to see if we have the powder. */

	/* Great, we have the ingredients...but is the thief smart enough? */
	if ( !IS_NPC( ch ) && get_curr_wis( ch ) < 19 )
	{
		send_to_char(AT_DGREEN, "You can't quite remember what to do...\n\r", ch );
		return;
	}
	/* And does the thief have steady enough hands? */
	if ( !IS_NPC( ch )
			&& ( get_curr_dex( ch ) < 20
				|| ch->pcdata->condition[COND_DRUNK] > 0 ) )
	{
		send_to_char(AT_DGREEN,
				"Your hands aren't steady enough to properly mix the poison.\n\r",
				ch );
		return;
	}

	WAIT_STATE( ch, skill_table[gsn_poison_weapon].beats );

	/* Check the skill percentage */
	if ( !IS_NPC( ch )
			&& number_percent( ) > ch->pcdata->learned[gsn_poison_weapon] )
	{
		send_to_char(AT_DGREEN, "You failed and spill some on yourself.  Ouch!\n\r",
				ch );
		damage( ch, ch, ch->level, gsn_poison_weapon );
		act(AT_DGREEN, "$n spills the poison all over!", ch, NULL, NULL, TO_ROOM );
		return;
	}

	/* Well, I'm tired of waiting.  Are you? */
	act(AT_GREEN, "You mix a deadly poison!",
			ch, NULL, NULL, TO_CHAR );
	act(AT_GREEN, "$n mixes a deadly poison!",
			ch, NULL, NULL, TO_ROOM );
	act(AT_GREEN, "You pour the poison over $p, which glistens wickedly!",
			ch, obj, NULL, TO_CHAR  );
	act(AT_GREEN, "$n pours the poison over $p, which glistens wickedly!",
			ch, obj, NULL, TO_ROOM  );
	SET_BIT( obj->extra_flags, ITEM_POISONED );
	obj->cost.gold   *= ch->level;
	obj->cost.silver *= ch->level;
	obj->cost.copper *= ch->level;

	/* WHAT?  All of that, just for that one bit?  How lame. ;) */
	update_skpell( ch, gsn_poison_weapon );

	return;
}

void do_acmorph ( CHAR_DATA *ch, OBJ_DATA *obj, int  vnum )
{
	OBJ_INDEX_DATA *pObjIndex;
	OBJ_DATA       *nObj;
	int             level;

	level = 0;
	act( AT_BLUE, "You invoke $p.", ch, obj, NULL, TO_CHAR );
	act( AT_BLUE, "$n invokes $p.", ch, obj, NULL, TO_ROOM ); 

	if ( !(pObjIndex = get_obj_index( vnum ) ) )
	{
		act( AT_BLUE, "$p whines and sparks, but nothing happens", ch, obj, NULL, TO_CHAR );
		return;
	}
	level = pObjIndex->level;
	nObj = create_object( pObjIndex, level );
	if ( CAN_WEAR( nObj, ITEM_TAKE ) )
	{
		obj_to_char( nObj, ch );
	}
	else
	{
		obj_to_room( nObj, ch->in_room );
	}

	act(AT_BLUE, "$p's form wavers, then solidifies as $P.", ch, obj, nObj, TO_CHAR );
	act(AT_BLUE, "$n's $p wavers in form. then solidifies as $P.", ch, obj, nObj, TO_ROOM );
	oprog_invoke_trigger( obj, ch, nObj );
	extract_obj( obj );
	return;    
}        

void do_acoload( CHAR_DATA *ch, OBJ_DATA *obj, int  vnum )
{
	OBJ_INDEX_DATA *pObjIndex;
	OBJ_DATA       *nObj;
	int             level;

	level = 0;
	act( AT_BLUE, "You invoke $p.", ch, obj, NULL, TO_CHAR );
	act( AT_BLUE, "$n invokes $p.", ch, obj, NULL, TO_ROOM ); 

	if ( !(pObjIndex = get_obj_index( vnum ) ) )
	{
		act( AT_BLUE, "$p whines and sparks, but nothing happens", ch, obj, NULL, TO_CHAR );
		return;
	}
	level = pObjIndex->level;
	nObj = create_object( pObjIndex, level );
	if ( CAN_WEAR( nObj, ITEM_TAKE ) )
	{
		obj_to_char( nObj, ch );
	}
	else
	{
		obj_to_room( nObj, ch->in_room );
	}
	act(AT_BLUE, "$p spawns $P.", ch, obj, nObj, TO_CHAR );
	act(AT_BLUE, "$n's $p spawns $P.", ch, obj, nObj, TO_ROOM );
	oprog_invoke_trigger( obj, ch, nObj );

	return;    
}        

void do_acmload( CHAR_DATA *ch, OBJ_DATA *obj, int vnum )
{
	CHAR_DATA      *victim;
	MOB_INDEX_DATA *pMobIndex;
	AFFECT_DATA af;

	act( AT_BLUE, "You invoke $p.", ch, obj, NULL, TO_CHAR );
	act( AT_BLUE, "$n invokes $p.", ch, obj, NULL, TO_ROOM ); 

	if ( !( pMobIndex = get_mob_index( vnum ) ) )
	{
		act( AT_BLUE, "$p whines and sparks, but nothing happens", ch, obj, NULL, TO_CHAR );
		return;
	}
	victim = create_mobile( pMobIndex );
	char_to_room( victim, ch->in_room );

	act(AT_BLUE, "$p spawns $N.", ch, obj, victim, TO_CHAR );
	act(AT_BLUE, "$n's $p spawns $N.", ch, obj, victim, TO_ROOM );
	if ( victim->master )
		stop_follower( victim );
	add_follower( victim, ch );
	af.type      = skill_lookup( "charm person" );
	af.duration  = 50;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_CHARM;
	affect_to_char( victim, &af );
	oprog_invoke_trigger( obj, ch, victim );

	return;
}

void do_actrans( CHAR_DATA *ch, OBJ_DATA *obj, int vnum )
{
	ROOM_INDEX_DATA *location;

	act( AT_BLUE, "You invoke $p.", ch, obj, NULL, TO_CHAR );
	act( AT_BLUE, "$n invokes $p.", ch, obj, NULL, TO_ROOM ); 

	if ( ch->in_room->vnum == 8 )
	{
		send_to_char(AT_RED, "Such items do not work for those sent to Hell.\n\r", ch);
		return;
	}

	if ( !( location = get_room_index( vnum ) ) )
	{
		act(AT_BLUE, "$p whines and sparks, but nothing happens.", ch, obj, NULL, TO_CHAR );
		return;
	}

	if ( room_is_private( location ) )
	{
		send_to_char(AT_BLUE, "That room is private right now.\n\r", ch );
		return;
	}

	if ( ch->fighting )
	{
		act( AT_BLUE, "$p pulses lightly, but fail to function.", ch, obj, NULL, TO_CHAR );
		return;
	} 
	act(AT_BLUE, "Everything begins to spin, when it clears you are elsewhere.", ch, obj, NULL, TO_CHAR );
	act(AT_BLUE, "$n invokes $p and vanishes in a swirling red mist.", ch, obj, NULL, TO_ROOM );
	char_from_room( ch );
	char_to_room( ch, location );
	act(AT_BLUE, "$n arrives in a swirling red mist.", ch, obj, NULL, TO_ROOM);
	do_look( ch, "auto" );
	oprog_invoke_trigger( obj, ch, ch );
	return;
}

void do_invoke( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA       *obj;
	char            arg1 [ MAX_INPUT_LENGTH ];
	char            arg2 [ MAX_INPUT_LENGTH ];
	char            spellarg [ MAX_INPUT_LENGTH ];

	if ( IS_NPC(ch) ) {
		return;
	}

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( !(obj = get_obj_carry( ch, arg1 ) ) && ( !(obj = get_obj_wear( ch, arg1 ) ) ) )
	{
		send_to_char( AT_WHITE, "You can't find it.\n\r", ch );
		return;
	}

	if ( ch->level < obj->level )
	{
		send_to_char(AT_BLUE, "You have not attained the level of mastery to use this item", ch );
		act(AT_BLUE, "$n tries to use $p, but is too inexperienced.",
				ch, obj, NULL, TO_ROOM );
		return;
	}

	if ( obj->ac_type <= 0 || obj->ac_type >= 6 )
	{
		act( AT_WHITE, "$p cannot be invoked.", ch, obj, NULL, TO_CHAR );
		return;
	}
	if ( obj->ac_type == 5 && !obj->ac_spell )
	{
		sprintf( log_buf, "Obj[%d] AcType Spell with no Spellname",
				obj->pIndexData->vnum );
		bug( log_buf, 0 );
		act( AT_WHITE, "$p cannot be invoked.", ch, obj, NULL, TO_CHAR );
		return;
	}

	if ( gets_zapped( ch, obj ) )
	{
		act(AT_BLUE, "You are zapped by $p and drop it.", ch, obj, NULL, TO_CHAR );
		act(AT_BLUE, "$n is zapped by $p and drops it.",  ch, obj, NULL, TO_ROOM );
		obj_from_char( obj );
		obj_to_room( obj, ch->in_room );
		return;
	}

	switch ( obj->ac_type )
	{
		default:   break;
		case 1:    do_acoload( ch, obj, obj->ac_vnum ); break;
		case 2:    do_acmload( ch, obj, obj->ac_vnum ); break;
		case 3:    do_actrans( ch, obj, obj->ac_vnum ); break;
		case 4:    do_acmorph( ch, obj, obj->ac_vnum ); break;
		case 5:    
				   {
					   spellarg[0] = '\0';
					   sprintf( spellarg, "'%s' %s", obj->ac_spell, arg2 );
					   do_acspell( ch, obj, spellarg );
					   break;
				   }
	}
	if ( obj->ac_charge[1] != -1 )
		if ( -- obj->ac_charge[0] <= 0 )
		{
			act(AT_WHITE, "Your $p sputters and sparks.", ch, obj, NULL, TO_CHAR );
			act(AT_WHITE, "$n's $p sputters and sparks..", ch, obj, NULL, TO_ROOM );
			obj->ac_type = 0;
			free_string( obj->ac_spell );
			obj->ac_spell = &str_empty[0];
			obj->ac_vnum = 0;
		}

	return;
}      

void do_voodo ( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA   *obj;
	CHAR_DATA  *victim;
	char        buf [MAX_STRING_LENGTH];
	char        arg [MAX_INPUT_LENGTH];
	char       *name;

	if ( ch->clan != 5 )
	{
		send_to_char(C_DEFAULT, "Huh?\n\r", ch);
		return;
	}
	buf[0] = '\0';
	one_argument( argument, arg );
	if ( !(victim = get_char_world( ch, arg ) ) )
	{
		send_to_char( AT_RED, "No such person exists.", ch );
		return;
	}
	if ( saves_spell( ch->level, victim ) )
	{
		send_to_char( AT_RED, "You failed.\n\r", ch );
		return;
	} 
	if ( ( !IS_NPC(victim) ) && ( victim->clan == 0 ) )
	{
		send_to_char( AT_RED, "Not on the unclanned.\n\r", ch );
		return;
	} 
	if (IS_NPC(victim))
		name	  = victim->short_descr;
	else
		name       = victim->name;
	obj = create_object ( get_obj_index( OBJ_VNUM_DOLL ), 0 );
	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );
	free_string( obj->name );
	obj->name = str_dup( arg );
	obj->timer = 10;
	obj_to_char(obj, ch);
	act(AT_RED, "You call upon the dark forces of Retribution to create $p.", ch, obj, NULL, TO_CHAR );
	act(AT_RED, "$n calls upon the dark forces of Retribution to create $p.", ch, obj, NULL, TO_ROOM );
	return;

}

void do_deposit( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_STRING_LENGTH];
	CLAN_DATA *pClan;
	bool clan_bank = FALSE;
	char arg2 [ MAX_STRING_LENGTH ];
	char arg3 [ MAX_STRING_LENGTH ];
	char buf  [ MAX_STRING_LENGTH ];
	MONEY_DATA amount;

	if (IS_NPC( ch ) )
		return;

	if ( !IS_SET( ch->in_room->room_flags, ROOM_BANK ) )
	{
		send_to_char( AT_WHITE, "You are not in a bank!\n\r", ch );
		return;
	}

	pClan = get_clan_index( ch->clan );  
	argument = one_argument( argument, arg );
	/* deposit gold, silver, copper <amount> or deposits all */

	amount.gold = amount.silver = amount.copper = 0;
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );

	if ( is_number( arg2 ) )
	{
		if ( !str_cmp( arg, "gold" ) )
			amount.gold = atoi( arg2 );
		else if ( !str_cmp( arg, "silver" ) )
			amount.silver = atoi( arg2 );
		else if ( !str_cmp( arg, "copper" ) )
			amount.copper = atoi( arg2 );
		else if ( str_cmp( arg, "all" ) )
		{
			send_to_char( AT_WHITE, "Invalid amount of money.\n\r", ch );
			send_to_char( AT_WHITE, "&WSyntax: deposit <currency_type> <amount>\n\r", ch );
			send_to_char( AT_WHITE, "&W    or: deposit <currency_type> <amount> clan\n\r", ch );
			return;
		}
	}  

	if ( ( amount.gold   > ch->money.gold   ) ||
			( amount.silver > ch->money.silver ) ||
			( amount.copper > ch->money.copper ) )
	{
		send_to_char( AT_WHITE, "You don't have that much money.\n\r", ch );
		return;
	}

	if ( ( !is_number( arg2 ) && ( str_cmp( arg, "all" ) ) ) ||
			( atoi( arg2 ) < 0 ) )
	{
		send_to_char( AT_WHITE, "Invalid amount of money.\n\r", ch );
		send_to_char( AT_WHITE, "&WSyntax: deposit <currency_type> <amount>\n\r", ch );
		send_to_char( AT_WHITE, "&W    or: deposit <currency_type> <amount> clan\n\r", ch );
		return;
	}
	else
	{
		if ( !str_cmp( arg, "all" ) )
		{
			amount.gold   = ch->money.gold;
			amount.silver = ch->money.silver;
			amount.copper = ch->money.copper;
		}

		if ( !str_cmp( arg3, "clan" ) )
		{
			if (  ( IS_SET( ch->in_room->area->area_flags, AREA_CLAN_HQ ) ) &&
					( pClan != 0 ) )
			{
				add_money( &pClan->bankaccount, &amount );
				clan_bank = TRUE;
			}
			else
			{
				send_to_char( AT_WHITE, 
						"You can only deposit into your clan bankaccount while you are at your clan head quarters' bank.\n\r",
						ch );
				return;
			} 
		}
		else  add_money( &ch->pcdata->bankaccount, &amount );

		sub_money( &ch->money, &amount );     
		if ( clan_bank )
		{
			sprintf( buf, "You deposit into your clan bankaccount %s\n\r", money_string( &amount ) );
			sprintf( buf+strlen( buf ), "Your current clan balance is %s\n\r",
					money_string( &pClan->bankaccount ) );
			save_clans( );
		}
		else
		{
			sprintf( buf, "You deposit %s\n\r", money_string( &amount ) );
			sprintf( buf+strlen( buf ), "Your current balance is %s\n\r",
					money_string( &ch->pcdata->bankaccount ) );
		}   
		send_to_char( AT_WHITE, buf, ch );

		if ( clan_bank )
			sprintf( buf, "&w$n deposits into the clan bankaccount %s", money_string( &amount ) );
		else
			sprintf( buf, "&w$n deposits %s", money_string( &amount ) );

		act( AT_WHITE, buf, ch, NULL, NULL, TO_ROOM );

		return;

	}

	return;
}            

void do_withdraw( CHAR_DATA *ch, char *argument )
{
	char arg2 [ MAX_STRING_LENGTH ];
	char arg3 [ MAX_STRING_LENGTH ];
	char buf  [ MAX_STRING_LENGTH ];
	MONEY_DATA amount;
	CLAN_DATA *pClan;
	bool clan_bank = FALSE;
	char arg[MAX_STRING_LENGTH];

	if (IS_NPC( ch ) )
		return;

	if ( !IS_SET( ch->in_room->room_flags, ROOM_BANK ) )
	{
		send_to_char(AT_WHITE, "You are not in a bank!\n\r", ch );
		return;
	}

	pClan = get_clan_index( ch->clan );  
	argument = one_argument( argument, arg );
	/* Withdraw gold, silver, copper <amount> or withdraw all */

	amount.gold = amount.silver = amount.copper = 0;
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );

	if ( is_number( arg2 ) )
	{
		if ( !str_cmp( arg, "gold" ) )
			amount.gold = atoi( arg2 );
		else if ( !str_cmp( arg, "silver" ) )
			amount.silver = atoi( arg2 );
		else if ( !str_cmp( arg, "copper" ) )
			amount.copper = atoi( arg2 );
		else if ( str_cmp( arg, "all" ) )
		{
			send_to_char( AT_WHITE, "Invalid amount of money.\n\r", ch );
			send_to_char( AT_WHITE, "&WSyntax: withdraw <currency_type> <amount>\n\r", ch );
			send_to_char( AT_WHITE, "&W    or: withdraw <currency_type> <amount> clan\n\r", ch );
			return;
		}
	}

	if ( !str_cmp( arg3, "clan" ) )
	{
		if ( ( IS_SET( ch->in_room->area->area_flags, AREA_CLAN_HQ ) ) &&
				( pClan != 0 ) )
		{
			if ( ( amount.gold   > pClan->bankaccount.gold ) ||
					( amount.silver > pClan->bankaccount.silver ) ||
					( amount.copper > pClan->bankaccount.copper ) )
			{
				send_to_char( AT_WHITE, "You clan bankaccount doesn't hold that much money.\n\r", ch );
				return;
			}
			else clan_bank = TRUE;
		}
		else
		{
			send_to_char( AT_WHITE, 
					"You must be at your clan head quarters to withdraw money from the clan bankaccount.\n\r", ch );
			return;
		}
	}
	else
	{
		if ( ( amount.gold   > ch->pcdata->bankaccount.gold   ) ||
				( amount.silver > ch->pcdata->bankaccount.silver ) ||
				( amount.copper > ch->pcdata->bankaccount.copper ) )
		{
			send_to_char( AT_WHITE, "Your bank account doesn't hold that much money.\n\r", ch );
			return;
		}
	}

	if ( ( !is_number( arg2 ) && ( str_cmp( arg, "all" ) ) ) ||
			( atoi( arg2 ) < 0 ) )
	{
		send_to_char( AT_WHITE, "Invalid amount of money.\n\r", ch );
		send_to_char( AT_WHITE, "&WSyntax: withdraw <currency_type> <amount>\n\r", ch );
		send_to_char( AT_WHITE, "&W    or: withdraw <currency_type> <amount> clan\n\r", ch );
		return;
	}
	else
	{
		if ( ( !str_cmp( arg, "all" ) ) && ( !clan_bank ) )
		{
			amount.gold   = ch->pcdata->bankaccount.gold;
			amount.silver = ch->pcdata->bankaccount.silver;
			amount.copper = ch->pcdata->bankaccount.copper;
		}
		else if ( ( !str_cmp( arg, "all" ) ) && ( clan_bank ) )
		{
			amount.gold   = pClan->bankaccount.gold;
			amount.silver = pClan->bankaccount.silver;
			amount.copper = pClan->bankaccount.copper;
		}

		add_money( &ch->money, &amount );
		if ( clan_bank )
		{
			sub_money( &pClan->bankaccount, &amount );
			sprintf( buf, "You withdraw from the clan bankaccount %s\n\r", money_string( &amount ) );
			send_to_char( AT_WHITE, buf, ch );
			sprintf( buf, "Your current clan balance is %s\n\r", money_string( &pClan->bankaccount ) );
			send_to_char( AT_WHITE, buf, ch );
			sprintf( buf, "&w$n withdraws from the clan bankaccount %s\n\r", money_string( &amount ) );
			act( AT_WHITE, buf, ch, NULL, NULL, TO_ROOM );
			save_clans( );
		}
		else
		{
			sub_money( &ch->pcdata->bankaccount, &amount );
			sprintf( buf, "You withdraw %s\n\r", money_string( &amount ) );
			send_to_char( AT_WHITE, buf, ch );
			sprintf( buf, "Your current balance is %s\n\r", 
					money_string( &ch->pcdata->bankaccount ) ); 
			send_to_char( AT_WHITE, buf, ch );
			sprintf( buf, "&w$n withdraws %s\n\r", money_string( &amount ) );
			act( AT_WHITE, buf, ch, NULL, NULL, TO_ROOM );
		}
		return;
	}

	return;
}

void do_repair ( CHAR_DATA *ch, char *argument )
{

	char       arg[MAX_STRING_LENGTH];
	OBJ_DATA  *pObj;
	MONEY_DATA amount;   

	if (IS_NPC(ch))
		return;
	if ( !IS_SET( ch->in_room->room_flags, ROOM_SMITHY ) )
	{
		send_to_char(AT_WHITE, "You are not within a smithy.\n\r", ch );
		return;
	}
	one_argument( argument, arg );

	if ( !str_cmp( arg, "all" ) )
	{
		char buf[MAX_STRING_LENGTH];
		for ( pObj = ch->carrying; pObj; pObj = pObj->next_content )
		{
			if ( pObj->wear_loc == WEAR_NONE )
				continue;

			if ( ( (pObj->cost.gold*100) + (pObj->cost.silver*10) +
						(pObj->cost.copper) ) >= ( (pObj->pIndexData->cost.gold*100) +
							(pObj->pIndexData->cost.silver*10) +
							(pObj->pIndexData->cost.copper) ) )
				continue;
			amount.gold   = (pObj->pIndexData->cost.gold   - pObj->cost.gold);
			amount.silver = (pObj->pIndexData->cost.silver - pObj->cost.silver);
			amount.copper = (pObj->pIndexData->cost.copper - pObj->cost.copper);
			switch (pObj->item_type)
			{
				case ITEM_WEAPON:
					amount.gold   = (amount.gold * pObj->value[2]) / (!pObj->value[1] ? 1 :
							pObj->value[1]);
					amount.silver = (amount.silver * pObj->value[2]) / (!pObj->value[1] ? 1 :
							pObj->value[1]);
					amount.copper = (amount.copper * pObj->value[2]) / (!pObj->value[1] ? 1 :
							pObj->value[1]);
					break;
				case ITEM_ARMOR:
					amount.gold   = (amount.gold * pObj->level) / (!pObj->value[0] ? 1 :
							pObj->value[0]);
					amount.silver = (amount.silver * pObj->level) / (!pObj->value[0] ? 1 :
							pObj->value[0]);
					amount.copper = (amount.copper * pObj->level) / (!pObj->value[0] ? 1 :
							pObj->value[0]);
					break;
				default:
					bug("Do_repair: Item not weapon or armor.",0);
					break;
			}
			if ( ( amount.gold <= 0 ) && ( amount.silver <= 0 ) && ( amount.copper <= 0 ) )
				amount.gold = 1;

			if ( ( (amount.gold*100) + (amount.silver*10) + (amount.copper) ) >
					( (ch->money.gold*100) + (ch->money.silver*10) + (ch->money.copper) ) )
			{
				sprintf(buf, "$p will cost %s You lack the funds.", money_string( &amount ) );
				act(AT_WHITE,buf,ch,pObj,NULL,TO_CHAR);
				continue;
			}

			pObj->cost.gold   = pObj->pIndexData->cost.gold;
			pObj->cost.silver = pObj->pIndexData->cost.silver;
			pObj->cost.copper = pObj->pIndexData->cost.copper;
			REMOVE_BIT(pObj->extra_flags, ITEM_PATCHED);
			sprintf(buf, "To repair $p, you are charged %s", money_string( &amount ) );
			act(AT_WHITE,buf,ch,pObj,NULL,TO_CHAR);
			spend_money( &ch->money, &amount );
		}
		return;
	}

	if (!( pObj = get_obj_carry ( ch, arg ) ) )
	{
		send_to_char (AT_WHITE, "You do not see that here.\n\r", ch );
		return;
	}

	if ( ( (pObj->cost.gold*100) + (pObj->cost.silver*10) + (pObj->cost.copper) ) >=
			( (pObj->pIndexData->cost.gold*100) + (pObj->pIndexData->cost.silver*10) +
			  (pObj->pIndexData->cost.copper) ) )
	{
		send_to_char(AT_WHITE, "That item is not damaged.\n\r", ch );
		return;
	}
	amount.gold   = (pObj->pIndexData->cost.gold   - pObj->cost.gold);
	amount.silver = (pObj->pIndexData->cost.silver - pObj->cost.silver);
	amount.copper = (pObj->pIndexData->cost.copper - pObj->cost.copper);

	switch( pObj->item_type )
	{
		case ITEM_WEAPON:
			amount.gold   = (amount.gold * pObj->value[2]) / (!pObj->value[1] ? 1 :
					pObj->value[1]);
			amount.silver = (amount.silver * pObj->value[2]) / (!pObj->value[1] ? 1 :
					pObj->value[1]);
			amount.copper = (amount.copper * pObj->value[2]) / (!pObj->value[1] ? 1 :
					pObj->value[1]);
			break;
		case ITEM_ARMOR:
			amount.gold   = (amount.gold * pObj->level) / (!pObj->value[0] ? 1 :
					pObj->value[0]);
			amount.silver = (amount.silver * pObj->level) / (!pObj->value[0] ? 1 :
					pObj->value[0]);
			amount.copper = (amount.copper * pObj->level) / (!pObj->value[0] ? 1 :
					pObj->value[0]);
			break;
		default:
			bug("Do_repair: Item not weapon or armor.",0);
			break;
	}
	if ( ( amount.gold <= 0 ) && ( amount.silver <= 0 ) && ( amount.copper <= 0 ) )
		amount.gold = 1;

	if ( ( (ch->money.gold*100) + (ch->money.silver*10) + (ch->money.copper) ) <
			( (amount.gold*100) + (amount.silver*10) + (amount.copper) ) )
	{
		char    buf[MAX_STRING_LENGTH];

		sprintf(buf, "That item will cost %s You lack the funds.\n\r", money_string( &amount ) );
		send_to_char( AT_WHITE, buf, ch );
		return;
	}
	else
	{
		char     buf[MAX_STRING_LENGTH];

		sprintf( buf, "To repair %s, you are charged %s\n\r", 
				pObj->short_descr, money_string( &amount ) );
		sub_money( &ch->money, &amount );
		send_to_char ( AT_WHITE, buf, ch );
		pObj->cost.gold   = pObj->pIndexData->cost.gold;
		pObj->cost.silver = pObj->pIndexData->cost.silver;
		pObj->cost.copper = pObj->pIndexData->cost.copper;
		REMOVE_BIT(pObj->extra_flags, ITEM_PATCHED);
		return;
	}

	return;

}

void do_account( CHAR_DATA *ch, char *argument )
{
	/*  char arg [ MAX_STRING_LENGTH ]; */
	char buf [ MAX_STRING_LENGTH ];
	CLAN_DATA *clanacct;

	if (IS_NPC( ch ) )
		return;
	else
	{
		if ( !IS_SET( ch->in_room->room_flags, ROOM_BANK ) )
		{
			send_to_char(AT_WHITE, "You are not in a bank!\n\r", ch );
			return;
		}

		/* Don't need to convert coins, just checking if char has any money */

		if ( ( ch->pcdata->bankaccount.gold + ch->pcdata->bankaccount.silver +
					ch->pcdata->bankaccount.copper ) > 0 )
		{
			sprintf( buf, "Your account holds %s\n\r", money_string( &ch->pcdata->bankaccount ) ); 
			send_to_char( AT_WHITE, buf, ch );
			/*       return; */
		}
		else
		{
			int len = 0;
			len = strlen( ch->name );
			send_to_char(AT_WHITE, "You have nothing in your account!\n\r", ch );
			sprintf( buf, "&wFrom the shocked look on $n'%s face, you can tell that they have nothing in their account.",
					ch->name[len] == 's' ? "" : "s" );
			act(AT_WHITE, buf, ch, NULL, NULL, TO_ROOM );
			/*      return; */
		}
	}

	clanacct = get_clan_index( ch->clan );

	if ( ( IS_SET( ch->in_room->area->area_flags, AREA_CLAN_HQ ) )
			&& ( clanacct != 0 ) )
	{
		if ( ( clanacct->bankaccount.gold + clanacct->bankaccount.silver +
					clanacct->bankaccount.copper ) > 0 )
		{
			sprintf( buf, "%s's account holds %s\n\r", clanacct->name, 
					money_string( &clanacct->bankaccount ) );
			send_to_char( AT_WHITE, buf, ch );
			return;
		}
		else
		{
			sprintf( buf, "%s's account is empty.\n\r", clanacct->name );
			send_to_char( AT_WHITE, buf, ch );
			return;
		} 
	}

return;
}         

void do_separate( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *Obj;
	OBJ_DATA *aObj;
	OBJ_DATA *bObj;
	OBJ_INDEX_DATA *pIndex;

	if ( !( Obj = get_obj_carry( ch, argument ) ) )
	{
		send_to_char( AT_WHITE, "You are not carrying that item.\n\r", ch );
		return;
	}

	if ( !get_obj_index( Obj->pIndexData->sep_one ) ||
			!get_obj_index( Obj->pIndexData->sep_two ) )
	{
		send_to_char( AT_WHITE, "It cannot be separated.\n\r", ch );
		return;
	}

	pIndex = get_obj_index( Obj->pIndexData->sep_one );
	aObj = create_object( pIndex, pIndex->level );

	pIndex = get_obj_index( Obj->pIndexData->sep_two );
	bObj = create_object( pIndex, pIndex->level );
	sprintf( log_buf, "$n separates $p into %s and %s.\n\r",
			aObj->name, bObj->name );
	act( AT_WHITE, log_buf, ch, Obj, NULL, TO_ROOM );

	oprog_separate_trigger( Obj, ch );
	obj_from_char( Obj );
	extract_obj( Obj );
	obj_to_char( aObj, ch );
	obj_to_char( bObj, ch );
	send_to_char( AT_WHITE, "The object is now separated.\n\r", ch );
}

void do_join( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *aObj;
	OBJ_DATA *bObj;
	OBJ_DATA *Obj;
	OBJ_INDEX_DATA *pIndex;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( !(aObj = get_obj_carry(ch, arg1)) )
	{
		char buf[MAX_STRING_LENGTH];

		sprintf( buf, "You are not carrying any %s.\n\r", arg1 );
		send_to_char( AT_WHITE, buf, ch );
		return;
	}

	if ( !(bObj = get_obj_carry(ch, arg2)) )
	{
		char buf[MAX_STRING_LENGTH];

		if (strlen( arg2 ) > 0 )
		{
			sprintf( buf, "You are not carrying any %s.\n\r", arg2 );
			send_to_char( AT_WHITE, buf, ch );
			return;
		}
		else 
			if (strlen( arg2 ) <= 0 )
			{
				send_to_char( AT_WHITE, "What's that?\n\r", ch );
				return;
			}
	}

	if ( aObj->pIndexData->join != bObj->pIndexData->join ||
			aObj->pIndexData == bObj->pIndexData || 
			!get_obj_index( aObj->pIndexData->join ) )
	{
		char buf[MAX_STRING_LENGTH];

		sprintf( buf, "%s cannot be joined with %s.\n\r",
				capitalize( aObj->short_descr ), bObj->short_descr );
		send_to_char( AT_WHITE, buf, ch );
		return;
	}
	oprog_join_trigger( aObj, ch, bObj );
	pIndex = get_obj_index( aObj->pIndexData->join );
	Obj = create_object( pIndex, pIndex->level );
	obj_to_char( Obj, ch );
	sprintf( log_buf, "$n joins $p to $P to create %s.\n\r", Obj->short_descr );
	act( AT_WHITE, log_buf, ch, aObj, bObj, TO_ROOM );

	obj_from_char( aObj );
	extract_obj( aObj );
	obj_from_char( bObj );
	extract_obj( bObj );
	send_to_char( AT_WHITE, "Objects joined.\n\r", ch );
}

/*
 * -- Altrag
 */
void do_store( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	MONEY_DATA amt;
	int storage = 1000;


	if ( IS_NPC( ch ) )
		return;

	if ( !IS_SET( ch->in_room->room_flags, ROOM_BANK ) )
	{
		send_to_char( AT_WHITE, "You must be in a bank to store items.\n\r", ch );
		return;
	}
	if ( IS_SET( ch->act, PLR_THIEF ) )
	{
		send_to_char( AT_WHITE, "This Bank refuses to transact with thieves!\n\r", ch );
		return;
	}
	if ( argument[0] == '\0' )
	{
		send_to_char( AT_WHITE, "Your storage box contains:\n\r", ch );
		show_list_to_char( ch->pcdata->storage, ch, TRUE, TRUE );
		return;
	}

	if ( !( obj = get_obj_carry( ch, argument ) ) )
	{
		send_to_char( AT_WHITE, "You are not carrying that item.\n\r", ch );
		return;
	}

	if ( ch->pcdata->storcount >= 9 )
	{
		send_to_char( AT_WHITE,
				"You may only have 10 items in your storage box.\n\r", ch );
		return;
	}

	if ( ( (ch->pcdata->bankaccount.gold*100) + (ch->pcdata->bankaccount.silver*10) +
				(ch->pcdata->bankaccount.copper) ) < storage*100 )
		{
			send_to_char( AT_WHITE, 
					"Storing costs 1000 gold coins, which you do not have in your bank account.\n\r",
					ch );
			return;
		}

	amt.silver = amt.copper = 0;
	amt.gold = 1000;
	spend_money( &ch->pcdata->bankaccount, &amt );  
	oprog_store_trigger( obj, ch );

	obj_from_char( obj );
	obj_to_storage( obj, ch );
	send_to_char( AT_WHITE, "The bank deducts 1000 gold coins from your account.\n\r", ch );
}

void do_retrieve( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;

	if ( IS_NPC( ch ) )
		return;

	if ( argument[0] == '\0' )
	{
		send_to_char( AT_WHITE, "Retrieve what?\n\r", ch );
		return;
	}

	if ( !IS_SET( ch->in_room->room_flags, ROOM_BANK ) )
	{
		send_to_char(AT_WHITE, "You must be in a bank to retrieve items.\n\r", ch);
		return;
	}

	if ( !( obj = get_obj_storage( ch, argument ) ) )
	{
		send_to_char(AT_WHITE, "You do not have that object in storage.\n\r", ch);
		return;
	}
	if ( IS_SET( ch->act, PLR_THIEF ) )
	{
		send_to_char( AT_WHITE, "This Bank refuses to transact with thieves!\n\r", ch );
		return;
	}
	obj_from_storage( obj );
	obj_to_char( obj, ch );
	oprog_retrieve_trigger( obj, ch );
	send_to_char( AT_WHITE, "You retrieve it from storage.\n\r", ch );
}

/*
 * -- Altrag Dalosein
 */
void do_patch( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	int ammount = 0;
	char arg[MAX_STRING_LENGTH];

	MONEY_DATA amt;
	amt.gold = amt.silver = amt.copper = 0;

	if ( IS_NPC( ch ) )
		return;

	if ( ch->pcdata->learned[gsn_patch] <= 0 )
	{
		send_to_char(C_DEFAULT, "You don't know how to patch equipment.\n\r",ch);
		return;
	}

	one_argument( argument, arg );
	/* check if they want to repair everything they are wearing */
	if ( !str_cmp( arg, "all" ) )  
	{
		/* loop through all the items you are wearing */
		for ( obj = ch->carrying; obj; obj = obj->next_content )
		{
			/* If you are not wearing something in the location skip it */
			if ( obj->wear_loc == WEAR_NONE )
				continue;

			/* If the item is undamged skip it */
			if ( ( (obj->cost.gold*100) + (obj->cost.silver*10) + (obj->cost.copper) ) >=
					( (obj->pIndexData->cost.gold*100) + (obj->pIndexData->cost.silver*10) +
					  (obj->pIndexData->cost.copper) ) )
				continue;
			/* If the item is not armor or a weapon skip it */
			if ( obj->item_type != ITEM_WEAPON && obj->item_type != ITEM_ARMOR )
			{
				act(AT_WHITE,"You can't repair $p it's not a weapon or armor.",ch,obj,NULL,TO_CHAR);
				continue;
			}

			/* If the item is already patched skip it */
			if ( IS_SET(obj->extra_flags, ITEM_PATCHED) )
			{
				act(AT_WHITE,"You can't do anything more for $p.",ch,obj,NULL,TO_CHAR);
				continue;
			}

			/* patch the item */
			ammount = ch->pcdata->learned[gsn_patch] / 2;
			amt.gold   = (ammount * (obj->pIndexData->cost.gold   - obj->cost.gold)) / 100;
			amt.silver = (ammount * (obj->pIndexData->cost.silver - obj->cost.silver)) / 100;
			amt.copper = (ammount * (obj->pIndexData->cost.copper - obj->cost.copper)) / 100;
			obj->cost.gold   += amt.gold;
			obj->cost.silver += amt.silver;
			obj->cost.copper += amt.copper;
			SET_BIT(obj->extra_flags, ITEM_PATCHED);

			act(AT_WHITE,"You do your best to repair $p.",ch,obj,NULL,TO_CHAR);
		}
		return;
	}

	if ( !( obj = get_obj_carry( ch, argument ) ) )
	{
		send_to_char(C_DEFAULT, "You do not have that item.\n\r",ch);
		return;
	}

	if ( obj->item_type != ITEM_WEAPON && obj->item_type != ITEM_ARMOR )
	{
		send_to_char(C_DEFAULT, "You may only repair weapons and armor.\n\r",ch);
		return;
	}

	if ( IS_SET(obj->extra_flags, ITEM_PATCHED) )
	{
		send_to_char(C_DEFAULT, "You can't do much more for it.\n\r",ch);
		return;
	}

	if ( ( (obj->cost.gold*100) + (obj->cost.silver*10) + (obj->cost.copper) ) >=
			( (obj->pIndexData->cost.gold*100) + (obj->pIndexData->cost.silver*10) +
			  (obj->pIndexData->cost.copper) ) )
	{
		send_to_char(C_DEFAULT, "It already looks like new.\n\r",ch);
		return;
	}

	ammount = ch->pcdata->learned[gsn_patch] / 2;
	amt.gold   = (ammount * (obj->pIndexData->cost.gold   - obj->cost.gold)) / 100;
	amt.silver = (ammount * (obj->pIndexData->cost.silver - obj->cost.silver)) / 100;
	amt.copper = (ammount * (obj->pIndexData->cost.copper - obj->cost.copper)) / 100;
	obj->cost.gold   += amt.gold;
	obj->cost.silver += amt.silver;
	obj->cost.copper += amt.copper;

	SET_BIT(obj->extra_flags, ITEM_PATCHED);

	act(AT_WHITE,"$n repairs his $p a bit.",ch,obj,NULL,TO_ROOM);
	act(AT_WHITE,"You do your best to repair your $p.",ch,obj,NULL,TO_CHAR);
	update_skpell( ch, gsn_patch );
	return;
}

void do_alchemy ( CHAR_DATA *ch, char *argument )
{
	char       buf[MAX_STRING_LENGTH];
	AFFECT_DATA af;
	int        sn;
	OBJ_DATA  *pobj;
	char       arg1[MAX_INPUT_LENGTH];
	OBJ_DATA  *cobj;
	OBJ_DATA  *fobj;
	int        mana;
	int        dam;
	int        chance;


	if ( IS_NPC(ch) )
		return;
	if ( !IS_NPC( ch )                                                  
			&& !can_use_skpell( ch, gsn_alchemy ) )
	{                                          
		send_to_char(AT_WHITE, "What do you think you are, a cleric?\n\r", ch );
		return;
	}
	one_argument( argument, arg1 );
	if ( arg1[0] == '\0' )                                              
	{ send_to_char(AT_WHITE, "What spell do you wish to alchemy?\n\r",    ch ); return; }
	if ( ch->fighting )                                       
	{ send_to_char(AT_WHITE, "While you're fighting?  Nice try.\n\r", ch ); return; }

	for ( pobj = ch->carrying; pobj; pobj = pobj->next_content )
	{
		if ( ( (pobj->pIndexData->vnum == OBJ_VNUM_FLASK) ||
					(pobj->pIndexData->vnum == OBJ_VNUM_RWFLASK) )
				&& pobj->value[1] == skill_lookup("reserved") )
			break;
	}
	if ( !pobj )
	{
		send_to_char(AT_WHITE, "You do not have the empty flask.\n\r", ch );
		return;
	}
	for ( cobj = ch->carrying; cobj; cobj = cobj->next_content )
	{
		if ( ( cobj->pIndexData->vnum == OBJ_VNUM_CAULDRON ) ||
				( cobj->pIndexData->vnum == OBJ_VNUM_RWCAULDRON ) )
			break;
	}
	if ( !cobj )
	{
		send_to_char(AT_WHITE, "You do not have the cauldron.\n\r", ch );
		return;
	}
	for ( fobj = ch->carrying; fobj; fobj = fobj->next_content )
	{
		if ( ( fobj->pIndexData->vnum == OBJ_VNUM_MFIRE ) ||
				( fobj->pIndexData->vnum == OBJ_VNUM_RWFIRE ) )
			break;
	}
	if ( !fobj )
	{
		send_to_char(AT_WHITE, "You do not have the magical fire.\n\r", ch );
		return;
	}

	/* K, now we have all the stuff... check to see if the cleric can cast
	   the spell he wants.
	   */

	if ( ( sn = skill_lookup( arg1 ) ) < 0
			|| !can_use_skpell( ch, sn ) 
			|| sn == skill_lookup( "true sight" ) )
	{
		send_to_char(AT_BLUE, "You can't do that.\n\r", ch );
		return;
	}
	mana = MANA_COST( ch, sn );
	mana *= 2;
	if ( ch->mana < mana )
	{
		send_to_char(AT_WHITE, "You lack the mana to bind the elixer.\n\r", ch );
		return;
	}
	ch->mana -= mana;
	dam = ch->level * ( skill_table[sn].skill_level[prime_class(ch)] / 9 );
	chance = ch->pcdata->learned[gsn_alchemy] 
		- ( skill_table[sn].skill_level[prime_class(ch)] / 6 );

	if ( sn == skill_lookup( "aura of peace" ) )
		chance = 0;
	if ( sn == skill_lookup( "iceshield" ) )
		chance = 15;
	if ( number_percent( ) > chance )
	{
		sprintf( buf, "The %s potion explodes! causing %d damage!",
				skill_table[sn].name, dam );
		send_to_char(AT_RED, buf, ch );
		act(AT_RED, "$n's elixer explodes!", ch, NULL, NULL, TO_ROOM );
		extract_obj (pobj);
		extract_obj (cobj);
		extract_obj (fobj);
		send_to_char(AT_RED, "All your alchemy equipment is ruined!\n\r", ch );
		damage ( ch, ch, dam, gsn_incinerate );
		af.type      = gsn_incinerate;
		af.level	   = skill_table[sn].skill_level[prime_class(ch)];
		af.duration  = 5;
		af.location  = APPLY_NONE;
		af.modifier  = 0;
		af.bitvector = AFF_FLAMING;
		affect_join( ch, &af );
		return;
	}

	pobj->value[1] = sn;
	pobj->value[0] = ch->level/2 - 1; 
	pobj->timer = 60;
	pobj->level = ch->level/2 - 1;
	pobj->cost.gold = ch->level * skill_table[sn].skill_level[prime_class(ch)];
	sprintf( buf, "%s potion", skill_table[sn].name );
	free_string( pobj->short_descr );
	pobj->short_descr = str_dup (buf );
	sprintf( buf, "A potion of %s has been left here.", skill_table[sn].name );
	free_string( pobj->description );
	pobj->description = str_dup ( buf );
	if ( !is_name(NULL, pobj->name, skill_table[sn].name ) )
	{
		sprintf( buf, "%s %s", pobj->name, skill_table[sn].name );
		free_string( pobj->name );
		pobj->name = str_dup( buf );
	}
	act( AT_RED, "You deftly mix a $p.", ch, pobj, NULL, TO_CHAR );
	act( AT_RED, "$n mixes a $p.", ch, pobj, NULL, TO_ROOM );
	update_skpell( ch, gsn_alchemy );
	return;
}

void do_scribe( CHAR_DATA *ch, char *argument )
{
	char       buf[MAX_STRING_LENGTH];
	AFFECT_DATA af;
	int        sn;
	OBJ_DATA  *pobj;
	char       arg1[MAX_INPUT_LENGTH];
	OBJ_DATA  *cobj;
	OBJ_DATA  *fobj;
	int        mana;
	int        dam;
	int        chance;


	if ( IS_NPC(ch) )
		return;
	if ( !IS_NPC( ch )                                                  
			&& !can_use_skpell( ch, gsn_scribe ) )
	{                                          
		send_to_char(AT_WHITE, "What do you think you are, a mage?\n\r", ch );
		return;
	}
	one_argument( argument, arg1 );
	if ( arg1[0] == '\0' )                                              
	{ send_to_char(AT_WHITE, "What spell do you wish to scribe?\n\r",    ch ); return; }
	if ( ch->fighting )                                       
	{ send_to_char(AT_WHITE, "While you're fighting?  Nice try.\n\r", ch ); return; }

	for ( pobj = ch->carrying; pobj; pobj = pobj->next_content )
	{
		if ( ( (pobj->pIndexData->vnum == OBJ_VNUM_PARCHMENT) ||
					(pobj->pIndexData->vnum == OBJ_VNUM_RWPARCHMENT) )
				&& pobj->value[1] == skill_lookup("reserved") )
			break;
	}
	if ( !pobj )
	{
		send_to_char(AT_WHITE, "You do not have the blank parchment.\n\r", ch );
		return;
	}
	for ( cobj = ch->carrying; cobj; cobj = cobj->next_content )
	{
		if ( ( cobj->pIndexData->vnum == OBJ_VNUM_QUILL ) ||
				( cobj->pIndexData->vnum == OBJ_VNUM_RWQUILL ) )
			break;
	}
	if ( !cobj )
	{
		send_to_char(AT_WHITE, "You do not have the quill.\n\r", ch );
		return;
	}
	for ( fobj = ch->carrying; fobj; fobj = fobj->next_content )
	{
		if ( ( fobj->pIndexData->vnum == OBJ_VNUM_MINK ) ||
				( fobj->pIndexData->vnum == OBJ_VNUM_RWINK ) )
			break;
	}
	if ( !fobj )
	{
		send_to_char(AT_WHITE, "You do not have the magical ink.\n\r", ch );
		return;
	}

	/* K, now we have all the stuff... check to see if the mage can cast
	   the spell he wants.
	   */

	if ( ( sn = skill_lookup( arg1 ) ) < 0
			|| !can_use_skpell( ch, sn ) 
			|| sn == skill_lookup( "true sight" ) )
	{
		send_to_char(AT_BLUE, "You can't do that.\n\r", ch );
		return;
	}
	mana = MANA_COST( ch, sn );
	mana *= 2;
	if ( ch->mana < mana )
	{
		send_to_char(AT_WHITE, "You lack the mana to bind the words to the parchment.\n\r", ch );
		return;
	}
	ch->mana -= mana;
	dam = ch->level * ( skill_table[sn].skill_level[prime_class( ch )] / 9 );
	chance = ch->pcdata->learned[gsn_scribe] 
		- ( skill_table[sn].skill_level[prime_class( ch )] / 6 );

	if ( sn == skill_lookup( "shatter" ) )
		chance = 0;
	if ( number_percent( ) > chance )
	{
		sprintf( buf, "The %s scroll explodes! causing %d damage!",
				skill_table[sn].name, dam );
		send_to_char(AT_RED, buf, ch );
		act(AT_RED, "$n's scroll explodes!", ch, NULL, NULL, TO_ROOM );
		extract_obj (pobj);
		extract_obj (cobj);
		extract_obj (fobj);
		send_to_char(AT_RED, "All your scribery equipment is ruined!\n\r", ch );
		damage ( ch, ch, dam, gsn_incinerate );
		af.type      = gsn_incinerate;
		af.level	   = skill_table[sn].skill_level[prime_class(ch)];
		af.duration  = 5;
		af.location  = APPLY_NONE;
		af.modifier  = 0;
		af.bitvector = AFF_FLAMING;
		affect_join( ch, &af );
		return;
	}

	pobj->value[1] = sn;
	pobj->value[0] = ch->level/2 - 1; 
	pobj->timer = 60;
	pobj->level = ch->level/2 - 1;
	pobj->cost.gold = ch->level * skill_table[sn].skill_level[prime_class(ch)];
	sprintf( buf, "scroll of %s", skill_table[sn].name );
	free_string( pobj->short_descr );
	pobj->short_descr = str_dup (buf );
	sprintf( buf, "A scroll of %s has been left here.", skill_table[sn].name );
	free_string( pobj->description );
	pobj->description = str_dup ( buf );
	if ( !is_name(NULL, pobj->name, skill_table[sn].name ) )
	{
		sprintf( buf, "scroll %s", skill_table[sn].name );
		free_string( pobj->name );
		pobj->name = str_dup( buf );
	}
	act( AT_RED, "You neatly scribe a $p.", ch, pobj, NULL, TO_CHAR );
	act( AT_RED, "$n scribes a $p.", ch, pobj, NULL, TO_ROOM );
	send_to_char(AT_RED, "You toss aside the empty ink container.\n\r", ch);
	extract_obj(fobj);
	update_skpell( ch, gsn_scribe );
	return;
}

void do_donate( CHAR_DATA *ch,char *argument )
{
	char arg         [MAX_INPUT_LENGTH];
	ROOM_INDEX_DATA  *donation_room;
	OBJ_DATA         *obj;

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( AT_WHITE, "Donate What?\n\r", ch );
		return;
	}


	if ( !( obj = get_obj_carry( ch, argument ) ) )
	{
		send_to_char( AT_WHITE, "You do not have that item. \n\r", ch );
		return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
		send_to_char( AT_WHITE, "You can't let go of it.\n\r", ch );
		return;
	}

	if ( obj->item_type == ITEM_SCROLL || obj->item_type == ITEM_STAFF
			|| obj->item_type == ITEM_WAND   || obj->item_type == ITEM_POTION
			|| obj->item_type == ITEM_PILL   || obj->item_type == ITEM_LENSE )
	{
		if ( ( donation_room = get_room_index( ROOM_VNUM_DON_1 ) ) == NULL )
		{
			bug( "Do_donate: invalid vnum for donation room.\n\r", 0 );
			send_to_char( AT_WHITE, "Donation failed.\n\r",ch);
			return;
		}
	}
	else
		if ( obj->item_type == ITEM_WEAPON )
		{
			if ( ( donation_room = get_room_index( ROOM_VNUM_DON_2 ) ) == NULL )
			{
				bug( "Do_donate: invalid vnum for donation room.\n\r", 0 );
				send_to_char( AT_WHITE, "Donation failed.\n\r",ch);
				return;
			}
		}
		else
			if ( obj->item_type == ITEM_ARMOR )
			{
				if ( ( donation_room = get_room_index( ROOM_VNUM_DON_3 ) ) == NULL )
				{
					bug( "Do_donate: invalid vnum for donation room.\n\r", 0 );
					send_to_char( AT_WHITE, "Donation failed.\n\r",ch);
					return;
				}
			}
			else
				if ( ( donation_room = get_room_index( ROOM_VNUM_DON_4 ) ) == NULL )
				{
					bug( "Do_donate: invalid vnum for donation room.\n\r", 0 );
					send_to_char( AT_WHITE, "Donation failed.\n\r",ch);
					return;
				}

	obj_from_char( obj );
	obj->timer = 20;
	obj_to_room( obj, donation_room );
	act( AT_WHITE, "$n donates $p.", ch, obj, NULL, TO_ROOM );
	act( AT_WHITE, "You donate $p.", ch, obj, NULL, TO_CHAR );
	return;
}
void do_gravebind( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA	*obj;
	char		arg [ MAX_INPUT_LENGTH ];
	int		hp;
	if ( !IS_NPC( ch )
			&& !can_use_skpell( ch, gsn_gravebind ) )
	{
		send_to_char( C_DEFAULT,
				"You do not know how to gravebind corpses.\n\r", ch );
		return;
	}
	one_argument( argument, arg );
	if ( arg == '\0' )
	{
		send_to_char( AT_DGREY, "You must gravebind something.\n\r", ch );
		return;
	}
	obj = get_obj_list( ch, arg, ch->in_room->contents );
	if ( !obj )
	{
		send_to_char( AT_DGREY, "You can't find it.\n\r", ch );
		return;
	}
	if ( obj->item_type != ITEM_CORPSE_NPC )
	{
		send_to_char( AT_DGREY, "You can only gravebind corpses.\n\r", ch );
		return;
	}
	if ( ch->pcdata->learned[gsn_gravebind] < number_percent() )
	{
		send_to_char( AT_DGREY, 
				"You gravebind the corpse incorrectly and destroy it.\n\r", ch );
		act( AT_DGREY, "$n's gravebind of the $p fails and it is destroyed.",
				ch, obj, NULL, TO_ROOM );
		extract_obj( obj );
		return;
	}
	hp = UMAX( 20, ch->level + ch->level / 2 );
	ch->hit += hp;
	ch->hit = UMIN( MAX_HIT(ch), ch->hit );
	send_to_char( AT_DGREY, 
			"You gravebind the corpse, sucking away it's remaining life force.\n\r",
			ch );
	act( AT_DGREY,
			"The last of the $p's life force is sucked away by $n and then decays.",
			ch, obj, NULL, TO_ROOM );
	extract_obj( obj );
	update_skpell( ch, gsn_gravebind );
	return;
}


void do_indestructable( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA  *obj;
	CHAR_DATA *gch;
	MONEY_DATA amount;  

	if ( ch->in_room->vnum != ROOM_VNUM_ARTIFACTOR )
	{ 
		send_to_char( AT_RED, "You can't do that here.\n\r", ch );
		return;
	} 

	if ( argument[0] == '\0' )
	{
		send_to_char(AT_WHITE,
				"The artifactor says, 'What do you want to make indestructable?'\n\r",
				ch);
		return;
	}

	if ( !( obj = get_obj_carry( ch, argument ) ) )
	{
		send_to_char(AT_WHITE,
				"The artifactor says, 'You don't have that item.'\n\r",
				ch);
		return;
	}

	amount.gold = amount.silver = amount.copper = 0;
	if ( obj->level )
		amount.gold = obj->level * 25000;
	else
		amount.gold = 25000;

	if ( ( (ch->money.gold*C_PER_G) + (ch->money.silver*S_PER_G) + 
				(ch->money.copper) ) < amount.gold*C_PER_G )
	{
		send_to_char(AT_CYAN,
				"The artifactor says, 'You can't afford to make that indestructale.'\n\r",
				ch);
		return;
	}
	spend_money( &ch->money, &amount );

	act(AT_WHITE,
			"The artifactor takes $p from you and waves a glowing wand over it...",
			ch, obj, NULL, TO_CHAR );
	act(AT_WHITE,
			"The artifactor takes $p from $n and waves a glowing wand over it...",
			ch, obj, NULL,TO_ROOM);

	if ( number_percent( ) < (85-(obj->level * .35)) )
	{
		obj->extra_flags |= ITEM_NO_DAMAGE;
		act(AT_WHITE, 
				"$p rises from the artifactors hand and begins to vibrate...", 
				ch, obj, NULL, TO_CHAR ); 
		act(AT_WHITE, 
				"$p rises from the artifactors hand and begins to vibrate...", 
				ch, obj, NULL, TO_ROOM ); 

		act(AT_WHITE,
				"with a blinding flash of light $p drops back into the artifactors hand.",
				ch, obj, NULL, TO_CHAR );
		act(AT_WHITE,
				"with a blinding flash of light $p drops back into the artifactors hand.",
				ch, obj, NULL, TO_ROOM );

		act(AT_WHITE,
				"The artifactor hands you back $p.",
				ch, obj, NULL, TO_CHAR );
		act(AT_WHITE,
				"The artifactor hands $n back $p.",
				ch, obj, NULL, TO_ROOM );

		return;
	}

	act(AT_WHITE,
			"$p rises from the artifactors hand and begins to vibrate violently...",
			ch, obj, NULL, TO_CHAR );
	act(AT_WHITE,
			"$p rises from the artifactors hand and begins to vibrate violently...",
			ch, obj, NULL, TO_ROOM );

	act(AT_RED,
			"with a blinding flash of flame the $p explodes.",
			ch, obj, NULL, TO_CHAR );
	act(AT_RED,
			"with a blinding flash of flame the $p explodes.",
			ch, obj, NULL, TO_ROOM );

	for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
	{
		damage( gch, gch, (obj->level*10) , TYPE_UNDEFINED );           
	}


	act(AT_RED,
			"The artifactor curses as he sucks on his burned fingers.",
			ch, NULL, NULL, TO_CHAR );
	act(AT_RED,
			"The artifactor curses as he sucks on his burned fingers.",
			ch, NULL, NULL, TO_ROOM );

	extract_obj(obj);
	return;
}

void do_remake( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	MONEY_DATA amount;

	if ( ch->in_room->vnum != ROOM_VNUM_ARTIFACTOR )
	{ 
		send_to_char( AT_RED, "You can't do that here.\n\r", ch );
		return;
	} 

	if ( argument[0] == '\0' )
	{
		send_to_char(AT_WHITE,
				"The artifactor says, 'What do you want to remake?'\n\r",
				ch);
		return;
	}

	if ( !(obj = get_obj_carry( ch, argument ) ) )
	{
		send_to_char(AT_WHITE,
				"The artifactor says, 'You don't have that item.'\n\r",
				ch);
		return;
	}


	amount.silver = amount.copper = 0;
	amount.gold = 100000;

	if ( ( ch->money.gold*C_PER_G + ch->money.silver*S_PER_G +
				ch->money.copper ) < amount.gold*C_PER_G )
	{
		send_to_char(AT_CYAN,
				"The artifactor says, 'You can't afford to remake that.'\n\r",
				ch);
		return;
	}
	spend_money( &ch->money, &amount );
	act(AT_WHITE,
			"The artifactor waves a glowing rod over your $p.",
			ch, obj, NULL, TO_CHAR );
	act(AT_WHITE,
			"The artifactor waves a glowing rod over $n's $p.",
			ch, obj, NULL,TO_ROOM);

	act(AT_WHITE,
			"The artifactor says, '$n now visualize what you want your $p to become.'",
			ch, obj, NULL, TO_CHAR );
	act(AT_WHITE,
			"The artifactor says, '$n now visualize what you want your $p to become.'",
			ch, obj, NULL,TO_ROOM);

	act(AT_YELLOW, 
			"The $p begins to flow and reshape in your hand.",
			ch, obj, NULL, TO_CHAR );

	act(AT_YELLOW, 
			"The $p begins to flow and reshape in $n's hand.",
			ch, obj, NULL, TO_ROOM );

	do_rename_obj( ch, argument );

	return;
}

void do_flamehand( CHAR_DATA *ch, char *argument )
{
	AFFECT_DATA af;
	if ( is_affected( ch, gsn_flamehand ) )
		return;
	if ( IS_NPC( ch ) || 
			!can_use_skpell( ch, gsn_flamehand ) )
	{
		send_to_char( AT_GREY, "Huh?\n\r", ch );
		return;
	}

	if ( ch->pcdata->learned[gsn_flamehand] < number_percent( ) )
	{
		send_to_char( AT_GREY, "You cannot seem to control your chi.\n\r",
				ch );
		return;
	}
	af.type	 = gsn_flamehand;
	af.level	 = ch->level;
	af.duration	 = 20 + ( 5 * (ch->level > 75) ) + ( 10 * (ch->level >95) );
	af.location	 = APPLY_NONE;
	af.modifier	 = 0;
	af.bitvector	 = 0;
	affect_to_char( ch, &af );
	send_to_char( AT_GREY, "You tap deep down into your chi.\n\r", ch );
	act( AT_GREY, "$n goes into a trance, $s palms facing to the sky.",
			ch, NULL, NULL, TO_ROOM );
	send_to_char( AT_GREY, "Your hands burst into &rflames&w.\n\r", ch );
	act( AT_GREY, "$n's hands suddenly burst into &rflames&w!",
			ch, NULL, NULL, TO_ROOM );
	update_skpell( ch, gsn_flamehand );
	return;
}
void do_frosthand( CHAR_DATA *ch, char *argument )
{
	AFFECT_DATA af;
	if ( is_affected( ch, gsn_frosthand ) )
		return;
	if ( IS_NPC( ch ) || 
			!can_use_skpell( ch, gsn_frosthand ) )
	{
		send_to_char( AT_GREY, "Huh?\n\r", ch );
		return;
	}

	if ( ch->pcdata->learned[gsn_frosthand] < number_percent( ) )
	{
		send_to_char( AT_GREY, "You cannot seem to control your chi.\n\r",
				ch );
		return;
	}
	af.type	 = gsn_frosthand;
	af.level	 = ch->level;
	af.duration	 = 25 + ( 5 * (ch->level > 85) ) + ( 10 * (ch->level > 95) );
	af.location	 = APPLY_NONE;
	af.modifier	 = 0;
	af.bitvector	 = 0;
	affect_to_char( ch, &af );
	send_to_char( AT_GREY, "You tap deep down into your chi.\n\r", ch );
	act( AT_GREY, "$n goes into a trance, $s palms facing to the sky.",
			ch, NULL, NULL, TO_ROOM );
	send_to_char( AT_GREY, "Your hands become freezing and are covered with &Cfrost &wand &Cice&w.\n\r", ch );
	act( AT_GREY, "$n's hands slowly seem to turn to &Cice&w.",
			ch, NULL, NULL, TO_ROOM );
	update_skpell( ch, gsn_frosthand );
	return;
}
void do_chaoshand( CHAR_DATA *ch, char *argument )
{
	AFFECT_DATA af;
	if ( is_affected( ch, gsn_chaoshand ) )
		return;
	if ( IS_NPC( ch ) || 
			!can_use_skpell( ch, gsn_chaoshand ) )
	{
		send_to_char( AT_GREY, "Huh?\n\r", ch );
		return;
	}

	if ( ch->pcdata->learned[gsn_chaoshand] < number_percent( ) )
	{
		send_to_char( AT_GREY, "You cannot seem to control your chi.\n\r",
				ch );
		return;
	}
	af.type	 = gsn_chaoshand;
	af.level	 = ch->level;
	af.duration	 = 20 + ( 5 * (ch->level > 90) ) + ( 10 * (ch->level >= 100) );
	af.location	 = APPLY_NONE;
	af.modifier	 = 0;
	af.bitvector	 = 0;
	affect_to_char( ch, &af );
	send_to_char( AT_GREY, "You tap deep down into your chi.\n\r", ch );
	act( AT_GREY, "$n goes into a trance, $s palms facing to the sky.",
			ch, NULL, NULL, TO_ROOM );
	send_to_char( AT_GREY, "Your hands sparkle &Ychaoticaly&w.\n\r", ch );
	act( AT_GREY, "$n's hands start to sparkle in a &Ychaotic &wfashion.",
			ch, NULL, NULL, TO_ROOM );
	update_skpell( ch, gsn_chaoshand );
	return;
}
void do_ironfist( CHAR_DATA *ch, char *argument )
{
	AFFECT_DATA af;
	if ( is_affected( ch, gsn_ironfist ) )
		return;
	if ( IS_NPC( ch ) || 
			!can_use_skpell( ch, gsn_ironfist ) )
	{
		send_to_char( AT_GREY, "Huh?\n\r", ch );
		return;
	}
	WAIT_STATE( ch, skill_table[gsn_ironfist].beats );
	if ( ch->pcdata->learned[gsn_ironfist] < number_percent( ) )
	{
		send_to_char( AT_GREY, "You failed.\n\r",
				ch );
		return;
	}
	send_to_char( AT_GREY, "Your fists turn to &Oiron&w.\n\r", ch );
	act( AT_GREY, "$n's fists turn to &Oiron&w.", ch, NULL, NULL, TO_ROOM );
	af.type	 = gsn_ironfist;
	af.level	 = ch->level;
	af.duration	 = ch->level / 2;
	af.location	 = APPLY_HITROLL;
	af.modifier	 = ch->level / 2;
	af.bitvector	 = 0;
	affect_to_char( ch, &af );
	af.location	 = APPLY_DAMROLL;
	af.modifier	 = ch->level / 3;
	af.bitvector	 = 0;
	affect_to_char( ch, &af );
	update_skpell( ch, gsn_ironfist );
	return;
}
void do_snatch( CHAR_DATA *ch, char *argument )
{
	if ( !can_use_skpell( ch, gsn_snatch )
			|| ch->pcdata->learned[gsn_snatch] < number_percent() )
	{
		send_to_char(C_DEFAULT, "You failed.\n\r", ch );
		return;
	}
	do_steal( ch, argument );
	ch->wait = 0;
	WAIT_STATE( ch, skill_table[gsn_snatch].beats );
	update_skpell( ch, gsn_snatch );
	return;
}
void do_antidote( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *flask;
	if ( IS_NPC( ch ) 
			|| !can_use_skpell( ch, gsn_antidote ) )
	{
		send_to_char(C_DEFAULT, "Huh?\n\r", ch );
		return;
	}
	for ( flask = ch->carrying; flask; flask = flask->next_content )
	{
		if ( ( (flask->pIndexData->vnum == OBJ_VNUM_FLASK) ||
					(flask->pIndexData->vnum == OBJ_VNUM_RWFLASK) )
				&& flask->value[1] == skill_lookup("reserved") )
			break;
	}
	if ( !flask )
	{
		send_to_char( AT_GREY, "You need a flask to put your antidote in.\n\r", ch );
		return;
	}
	flask->value[1] = skill_lookup( "cure poison" );
	flask->value[0] = ch->level;
	flask->cost.gold = 1000;
	free_string( flask->name );
	flask->name	  = str_dup( "antidote" );
	free_string( flask->short_descr );
	flask->short_descr = str_dup( "A flask of antidote" );
	free_string( flask->description );
	flask->description  = str_dup( "A small flask filled with a greenish liquid sets upon the floor." );
	send_to_char( AT_GREY, "You poor the antidote in the empty flask.\n\r", ch );
	act( AT_GREY, "$n poors some liquid into an empty flask.\n\r",
			ch, NULL, NULL, TO_ROOM );
	update_skpell( ch, gsn_antidote );
	return;
}

void do_identify( CHAR_DATA *ch, char *argument ) {
	OBJ_DATA    *obj;
	AFFECT_DATA *paf;
	char         buf [ MAX_STRING_LENGTH ];
	int          spn;
	char 				 arg [ MAX_STRING_LENGTH ];

	one_argument( argument, arg );

	if ( arg[0] == '\0' ) {
		send_to_char(AT_CYAN, "You can't find it.\n\r", ch );
		return;
	}

	if ( !( obj = get_obj_here( ch, arg ) ) ) {
		send_to_char(AT_CYAN, "You do not have that item.\n\r", ch );
		return;
	}

	sprintf( buf, "Object '%s' is type %s, extra flags %s %s %s.\n\r",
			obj->name,
			item_type_name( obj ),
			obj->extra_flags ? extra_bit_name( obj->extra_flags ) : "",
			obj->anti_race_flags ? antirace_bit_name(obj->anti_race_flags) : "",
			obj->anti_class_flags ? anticlass_bit_name(obj->anti_class_flags) : "");
	send_to_char(AT_CYAN, buf, ch );

	sprintf( buf, "Weight : %d, level : %d.\n\r", obj->weight, obj->level );
	send_to_char( AT_CYAN, buf, ch );

	sprintf( buf, "Gold Value: %d  Silver Value: %d  Copper Value: %d\n\r", obj->cost.gold, obj->cost.silver, obj->cost.copper );
	send_to_char(AT_CYAN, buf, ch );

	switch ( obj->item_type ) {
		case ITEM_PILL:  
		case ITEM_SCROLL: 
		case ITEM_POTION:
			sprintf( buf, "Level %d spells of:", obj->value[0] );
			send_to_char(AT_CYAN, buf, ch );

			if ( is_sn(obj->value[1]) ) {
				send_to_char(AT_CYAN, " '", ch );
				send_to_char(AT_WHITE, skill_table[obj->value[1]].name, ch );
				send_to_char(AT_CYAN, "'", ch );
			}

			if ( is_sn(obj->value[2]) ) {
				send_to_char(AT_CYAN, " '", ch );
				send_to_char(AT_WHITE, skill_table[obj->value[2]].name, ch );
				send_to_char(AT_CYAN, "'", ch );
			}

			if ( is_sn(obj->value[3]) ) {
				send_to_char(AT_CYAN, " '", ch );
				send_to_char(AT_WHITE, skill_table[obj->value[3]].name, ch );
				send_to_char(AT_CYAN, "'", ch );
			}

			send_to_char(AT_CYAN, ".\n\r", ch );
			break;

		case ITEM_WAND: 
		case ITEM_LENSE:
		case ITEM_STAFF: 
			if (!(obj->value[1] == -1 ) ) {
				sprintf( buf, "Has %d(%d) charges of level %d", obj->value[1], obj->value[2], obj->value[0] );
			} else {
				sprintf( buf, "Has unlimited charges of level %d", obj->value[0] );
			}

			send_to_char(AT_CYAN, buf, ch );

			if ( is_sn(obj->value[3]) ) {
				send_to_char(AT_CYAN, " '", ch );
				send_to_char(AT_WHITE, skill_table[obj->value[3]].name, ch );
				send_to_char(AT_CYAN, "'", ch );
			}

			send_to_char(AT_CYAN, ".\n\r", ch );
			break;

		case ITEM_WEAPON:
			sprintf( buf, "Damage is %d to %d (average %d).\n\r", obj->value[1], obj->value[2], ( obj->value[1] + obj->value[2] ) / 2 );
			send_to_char(AT_RED, buf, ch );
			break;

		case ITEM_ARMOR:
			sprintf( buf, "Armor class is %d.\n\r", obj->value[0] );
			send_to_char(AT_CYAN, buf, ch );
			break;
	}

	if ( obj->ac_type != 0 ) {
		switch( obj->ac_type ) {
			default:
				send_to_char(AT_CYAN, "Invoke Type Unknown.\n\r", ch );
				break;
			case 1 :
					  {
						  if ( obj->ac_charge[1] != -1 ) {
							  sprintf( buf, "Object creation invoke, with [%d/%d] charges.\n\r", obj->ac_charge[0], obj->ac_charge[1] );
						  } else {
							  sprintf( buf, "Object creation invoke, with unlimited charges.\n\r" );
						  }

						  send_to_char(AT_CYAN, buf, ch );

						  break;
					  }
			case 2 :
					  {
						  if ( obj->ac_charge[1] != -1 ) {
							  sprintf( buf, "Monster creation invoke, with [%d/%d] charges.\n\r", obj->ac_charge[0], obj->ac_charge[1] );
						  } else {
							  sprintf( buf, "Monster creation invoke, with unlimited charges.\n\r" );
						  }

						  send_to_char(AT_CYAN, buf, ch );
						  break;        
					  }
			case 3 :
					  {
						  if ( obj->ac_charge[1] != -1 ) {
							  sprintf( buf, "Transfer invoke, with [%d/%d] charges.\n\r", obj->ac_charge[0], obj->ac_charge[1] );
						  } else {
							  sprintf( buf, "Transfer invoke, with unlimited charges.\n\r" );
						  }

						  send_to_char(AT_CYAN, buf, ch );
						  break;
					  }
			case 4 :
					  {
						  if ( obj->ac_charge[1] != -1 ) {
							  sprintf( buf, "Object morph invoke, with [%d/%d] charges.\n\r", obj->ac_charge[0], obj->ac_charge[1] );
						  } else {
							  sprintf( buf, "Object morph invoke, with unlimited charges.\n\r" );
						  }

						  send_to_char(AT_CYAN, buf, ch );
						  break;
					  }
			case 5 :
					  {
						  if ( obj->ac_charge[1] != -1 ) {
							  sprintf( buf, "Spell invoke, has [%d/%d] charges of ", obj->ac_charge[0], obj->ac_charge[1] );
						  } else {
							  sprintf( buf, "Spell invoke, with unlimited charges of " );
						  }

						  send_to_char(AT_CYAN, buf, ch );

						  spn = skill_lookup( obj->ac_spell );

						  if ( is_sn(spn) ) {
							  send_to_char(AT_CYAN, " '", ch );
							  send_to_char(AT_WHITE, spn ? obj->ac_spell : "(none)", ch );
							  send_to_char(AT_CYAN, "'\n\r", ch );
						  }

						  break;
					  }
		}   
	} 

	for ( paf = obj->pIndexData->affected; paf; paf = paf->next ) {
		if ( paf->location != APPLY_NONE && paf->modifier != 0 ) {
			sprintf( buf, "Affects %s by %d.\n\r", affect_loc_name( paf->location ), paf->modifier );
			send_to_char(AT_BLUE, buf, ch );
		}
	}

	for ( paf = obj->affected; paf; paf = paf->next ) {
		if ( paf->location != APPLY_NONE && paf->modifier != 0 ) {
			sprintf( buf, "Affects %s by %d.\n\r", affect_loc_name( paf->location ), paf->modifier );
			send_to_char(AT_BLUE, buf, ch );
		}
	}

	return;
}

