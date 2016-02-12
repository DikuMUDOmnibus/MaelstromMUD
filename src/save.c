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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

extern	int	_filbuf		args( (FILE *) );

/*
 * Array of containers read for proper re-nesting of objects.
 */
#define MAX_NEST	100
static	OBJ_DATA *	rgObjNest	[ MAX_NEST ];

/*
 *  playerlist function  --Decklarean
 */

void update_playerlist  args( ( CHAR_DATA *ch ));

/*
 * Local functions.
 */
void fwrite_char		args( ( CHAR_DATA *ch,  FILE *fp ) );
void fwrite_obj			args( ( CHAR_DATA *ch,  OBJ_DATA  *obj, FILE *fp, int iNest, bool storage ) );
void fread_char			args( ( CHAR_DATA *ch,  FILE *fp ) );
void fread_obj 			args( ( CHAR_DATA *ch,  FILE *fp, bool storage ) );
void fread_pet 			args( ( CHAR_DATA *ch, FILE *fp ) );
void save_pet 			args( ( CHAR_DATA *ch, FILE *fp, CHAR_DATA *pet ) );
void add_alias 			args( ( CHAR_DATA *ch, ALIAS_DATA *pAl, char *old, char *new ) );
void fwrite_alias		args( ( CHAR_DATA *ch, FILE *fp ) );
void fread_alias		args( ( CHAR_DATA *ch, FILE *fp ) );
void fread_finger		args( ( CHAR_DATA *ch, FILE *fp, char *name ) );

/* Check to see if a player exists */
bool pstat( char *name ) {
	bool found;
	char strsave[256];
	FILE *fp;

	sprintf( strsave, "%s%c/%s", PLAYER_DIR, LOWER(name[0]), capitalize( name ) );

	found = ( fp = fopen( strsave, "r" ) ) != NULL;
	fclose( fp );
	fpReserve = fopen( NULL_FILE, "r" );

	return found;
}

/*
 *   Save a character and inventory.
 *   Would be cool to save NPC's too for quest purposes,
 *   some of the infrastructure is provided.
 */
void save_char_obj( CHAR_DATA *ch )
{
	FILE *fp;
	CHAR_DATA *pet;
	char  buf     [ MAX_STRING_LENGTH ];
	char  strsave [ MAX_INPUT_LENGTH  ];

	if ( IS_NPC( ch ) ) {
		return;
	}

	if ( ch->desc && ch->desc->original ) {
		ch = ch->desc->original;
	}

	update_playerlist( ch );

	ch->save_time = current_time;
	fclose( fpReserve );

	/* player files parsed directories by Yaz 4th Realm */
	sprintf( strsave, "%s%c/%s", PLAYER_DIR, LOWER(ch->name[0]), capitalize( ch->name ) );

	if ( !( fp = fopen( strsave, "w" ) ) ) {
		sprintf( buf, "Save_char_obj: fopen %s: ", ch->name );
		bug( buf, 0 );
		perror( strsave );
	} else {
		fwrite_char( ch, fp );

		if ( ch->carrying ) {
			fwrite_obj( ch, ch->carrying, fp, 0, FALSE );
		}

		if ( !IS_NPC( ch ) && ch->pcdata->storage ) {
			fwrite_obj( ch, ch->pcdata->storage, fp, 0, TRUE );
		}

		for ( pet = ch->in_room->people; pet; pet = pet->next_in_room ) {
			if (IS_NPC( pet ) ) {
				if ( IS_SET( pet->act, ACT_PET ) && ( pet->master == ch ) ) {
					save_pet( ch, fp, pet );
					break;
				}
			}
		}

		tail_chain();
		fprintf( fp, "#END\n" );
	}

	fclose( fp );

	fpReserve = fopen( NULL_FILE, "r" );
	return;
}



/*
 * Write the char.
 */
void fwrite_char( CHAR_DATA *ch, FILE *fp )
{
	AFFECT_DATA *paf;
	int          sn;

	fprintf( fp, "#%s\n", IS_NPC( ch ) ? "MOB" : "PLAYER"	);

	fprintf( fp, "Nm          %s~\n",	ch->name		);
	if ( ch->pcdata->lname && ch->pcdata->lname[0] != '\0' ) 
		fprintf( fp, "Lnm         %s~\n",   ch->pcdata->lname       );
	fprintf( fp, "ShtDsc      %s~\n",	ch->short_descr		);
	fprintf( fp, "LngDsc      %s~\n",	ch->long_descr		);
	fprintf( fp, "Dscr        %s~\n",	ch->description		);
	fprintf( fp, "Prmpt       %s~\n",	ch->prompt		);
	fprintf( fp, "Sx          %d\n",	ch->sex			);
	fprintf( fp, "Cla         %s\n",	class_numbers( ch, TRUE ));
	fprintf( fp, "Rce         %d\n",	ch->race		);
	fprintf( fp, "Wiznet      %d\n",    ch->wiznet		);
	fprintf( fp, "Clan        %d\n",    ch->clan                );
	fprintf( fp, "Clvl        %d\n",    ch->clev                );
	fprintf( fp, "Ctmr        %d\n",    ch->ctimer              );
	fprintf( fp, "Stun        %d %d %d %d %d\n",
			ch->stunned[0], ch->stunned[1], ch->stunned[2],
			ch->stunned[3], ch->stunned[4] );
	fprintf( fp, "WizLev      %d\n",    ch->wizinvis            );
	fprintf( fp, "ClkLev      %d\n",    ch->cloaked		);
	fprintf( fp, "Lvl         %d\n",	ch->level		);
	fprintf( fp, "Antidisarm  %d\n",    ch->antidisarm          );
	fprintf( fp, "Trst        %d\n",	ch->trust		);
	fprintf( fp, "Security    %d\n",    ch->pcdata->security	);  /* OLC */
	fprintf( fp, "Wizbt       %d\n",	ch->wizbit		);
	fprintf( fp, "Playd       %d\n",
			ch->played + (int) ( current_time - ch->logon )		);
	fprintf( fp, "Note        %ld\n",   ch->last_note           );
	fprintf( fp, "Room        %d\n",
			(  ch->in_room == get_room_index( ROOM_VNUM_LIMBO )
			   && ch->was_in_room )
			? ch->was_in_room->vnum
			: ch->in_room->vnum );

	fprintf( fp, "HpMnMv    %d %d %d %d %d %d %d %d %d\n",
			ch->hit, ch->perm_hit, ch->mod_hit, ch->mana, ch->perm_mana,
			ch->mod_mana, ch->move, ch->perm_move, ch->mod_move );
	fprintf( fp, "Charisma    %d\n",    ch->charisma		);
	fprintf( fp, "Gold	      %d\n",	ch->money.gold		);
	fprintf( fp, "Silver      %d\n",  	ch->money.silver	);
	fprintf( fp, "Copper      %d\n",	ch->money.copper	);
	fprintf( fp, "GRank       %d\n",	ch->guild_rank		);
	fprintf( fp, "Guild       %s~\n",
			(ch->guild != NULL) ? ch->guild->name : "\0"		);
	fprintf( fp, "Exp         %d\n",	ch->exp			);
	if ( ch->race == RACE_GHOUL && !IS_SET( ch->act, PLR_UNDEAD ) )
		SET_BIT( ch->act, PLR_UNDEAD );
	fprintf( fp, "Act         %d\n",    ch->act				);
	fprintf( fp, "Act2        %d\n",    ch->act2			);
	fprintf( fp, "AffdBy      %ld\n",	ch->affected_by		);
	fprintf( fp, "AffdBy2     %ld\n",    ch->affected_by2        ); 
	fprintf( fp, "ImmBits     %ld\n",    ch->imm_flags		);
	fprintf( fp, "ResBits     %ld\n",	ch->res_flags		);
	fprintf( fp, "VulBits     %ld\n",	ch->vul_flags		);
	/* Bug fix from Alander */
	fprintf( fp, "Pos         %d\n",
			ch->position == POS_FIGHTING ? POS_STANDING : ch->position );

	fprintf( fp, "Prac        %d\n",	ch->practice		);
	fprintf( fp, "SavThr      %d\n",	ch->saving_throw	);
	fprintf( fp, "Align       %d\n",	ch->alignment		);
	fprintf( fp, "SAlign      %c\n",	ch->start_align		); 
	fprintf( fp, "Hit         %d\n",	ch->hitroll		);
	fprintf( fp, "Dam         %d\n",	ch->damroll		);
	fprintf( fp, "Armr        %d\n",	ch->armor		);
	fprintf( fp, "Wimp        %d\n",	ch->wimpy		);
	fprintf( fp, "Deaf        %d\n",	ch->deaf		);
	fprintf( fp, "Email	      %s~\n",   ch->pcdata->email	);
	fprintf( fp, "Plan	      %s~\n",   ch->pcdata->plan	);
	fprintf( fp, "Corpses     %d\n",    ch->pcdata->corpses	);

	if ( ch->cquestpnts  != 0 )
		fprintf( fp, "Cquestpnts  %d\n",        ch->cquestpnts          );
	if ( ch->questpoints != 0 )
		fprintf( fp, "QuestPnts   %d\n",        ch->questpoints         );
	if ( ch->nextquest != 0 )
		fprintf( fp, "QuestNext   %d\n",        ch->nextquest           );  
	else if ( ch->countdown != 0 )
		fprintf( fp, "QuestNext   %d\n",        30                    );
	/* 30 */
	if ( IS_NPC( ch ) )
	{
		fprintf( fp, "Vnum        %d\n",	ch->pIndexData->vnum	);
	}
	else
	{
		fprintf( fp, "Paswd       %s~\n",	ch->pcdata->pwd		);
		fprintf( fp, "Bmfin       %s~\n",	ch->pcdata->bamfin	);
		fprintf( fp, "Bmfout      %s~\n",	ch->pcdata->bamfout	);
		fprintf( fp, "Bmfsee      %s~\n",       ch->pcdata->bamfusee	);
		fprintf( fp, "Trnsto      %s~\n",       ch->pcdata->transto	);
		fprintf( fp, "Trnsfrom    %s~\n",       ch->pcdata->transfrom	);
		fprintf( fp, "Trnvict     %s~\n",       ch->pcdata->transvict	);
		fprintf( fp, "Slyuc       %s~\n",       ch->pcdata->slayusee	);
		fprintf( fp, "Slyrm       %s~\n",       ch->pcdata->slayroom	);
		fprintf( fp, "Slyvict     %s~\n",       ch->pcdata->slayvict  	);
		fprintf( fp, "Afkmes      %s~\n",       ch->pcdata->afkchar     );
		/* EotS stats tracking */
		fprintf( fp, "Warpts      %d\n",		ch->warpts				);
		fprintf( fp, "Warkills    %d\n",		ch->warkills			);
		fprintf( fp, "Wardeaths   %d\n",		ch->wardeaths			);
		fprintf( fp, "Pkills      %d\n",		ch->pkills				);
		fprintf( fp, "pkilled     %d\n",		ch->pkilled				);
		fprintf( fp, "Arenawon    %d\n",		ch->arenawon			);
		fprintf( fp, "Arenalost   %d\n",		ch->arenalost			);
		fprintf( fp, "Incarnations %d\n",		ch->incarnations		);
		fprintf( fp, "Raisepts    %d\n",		ch->raisepts				);

		fprintf( fp, "Bank   	  %d %d %d\n",	
				ch->pcdata->bankaccount.gold,
				ch->pcdata->bankaccount.silver,
				ch->pcdata->bankaccount.copper );
		fprintf( fp, "Ttle        %s~\n",	ch->pcdata->title	);
		fprintf( fp, "Whotype	  %s~\n",	ch->pcdata->whotype	);
		fprintf( fp, "AtrPrm      %d %d %d %d %d\n",
				ch->pcdata->perm_str,
				ch->pcdata->perm_int,
				ch->pcdata->perm_wis,
				ch->pcdata->perm_dex,
				ch->pcdata->perm_con );

		fprintf( fp, "AtrMd       %d %d %d %d %d\n",
				ch->pcdata->mod_str, 
				ch->pcdata->mod_int, 
				ch->pcdata->mod_wis,
				ch->pcdata->mod_dex, 
				ch->pcdata->mod_con );

		fprintf( fp, "Cond        %d %d %d\n",
				ch->pcdata->condition[0],
				ch->pcdata->condition[1],
				ch->pcdata->condition[2] );

		fprintf( fp, "Pglen       %d\n",   ch->pcdata->pagelen     );
		fprintf( fp, "Empower     %s~\n",  ch->pcdata->empowerments);
		fprintf( fp, "Detract     %s~\n",  ch->pcdata->detractments);	
		for ( sn = 0; skill_table[sn].name[0] != '\0'; sn++ )
		{
			if ( skill_table[sn].name && ch->pcdata->learned[sn] > 0 )
			{
				fprintf( fp, "Skll        %d '%s'\n",
						ch->pcdata->learned[sn], skill_table[sn].name );
			}
		}
	}

	for ( paf = ch->affected; paf; paf = paf->next )
	{
		if ( paf->deleted )
			continue;

		fprintf( fp, "Aff       %3d %3d %3d %3d %10d %3d\n",
				skill_table[paf->type].slot,
				paf->duration,
				paf->modifier,
				paf->location,
				paf->bitvector,
				paf->level );
	}
	for ( paf = ch->affected2; paf; paf = paf->next )
	{
		if ( paf->deleted )
			continue;

		fprintf( fp, "Aff2       %3d %3d %3d %3d %10d %3d\n",
				skill_table[paf->type].slot,
				paf->duration,
				paf->modifier,
				paf->location,
				paf->bitvector,
				paf->level );
	}

	if ( ch->pcdata && ch->pcdata->alias_list )
	{
		ALIAS_DATA *pAl;

		for ( pAl = ch->pcdata->alias_list; pAl; pAl = pAl->next )
			fprintf( fp, "Alias      %s~ %s~\n",
					pAl->old, pAl->new );
	}

	fprintf( fp, "End\n\n" );
	return;
}



/*
 * Write an object and its contents.
 */
void fwrite_obj( CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int iNest,
		bool storage )
{
	AFFECT_DATA      *paf;
	EXTRA_DESCR_DATA *ed;

	/*
	 *   Slick recursion to write lists backwards,
	 *   so loading them will load in forwards order.
	 */
	if ( obj->next_content )
		fwrite_obj( ch, obj->next_content, fp, iNest, storage );

	/*
	 * Castrate storage characters.
	 */
	if ( obj->item_type == ITEM_KEY
			|| obj->deleted )
		return;

	if ( storage )
		fprintf( fp, "#STORAGE\n" );
	else
		fprintf( fp, "#OBJECT\n" );
	fprintf( fp, "Nest         %d\n",	iNest			     );
	fprintf( fp, "Name         %s~\n",	obj->name		     );
	fprintf( fp, "ShortDescr   %s~\n",	obj->short_descr	     );
	fprintf( fp, "Description  %s~\n",	obj->description	     );
	fprintf( fp, "Vnum         %d\n",	obj->pIndexData->vnum	     );
	fprintf( fp, "ExtraFlags   %d\n",	obj->extra_flags	     );
	fprintf( fp, "AntiClassF   %d\n",	obj->anti_class_flags	     );
	fprintf( fp, "AntiRaceF    %d\n",	obj->anti_race_flags	     );
	fprintf( fp, "WearFlags    %d\n",	obj->wear_flags		     );
	fprintf( fp, "WearLoc      %d\n",	obj->wear_loc		     );
	fprintf( fp, "ItemType     %d\n",	obj->item_type		     );
	fprintf( fp, "Weight       %d\n",	obj->weight		     );
	fprintf( fp, "Level        %d\n",	obj->level		     );
	fprintf( fp, "Timer        %d\n",	obj->timer		     );
	fprintf( fp, "Gold         %d\n",	obj->cost.gold		     );
	fprintf( fp, "Silver       %d\n",   obj->cost.silver	     );
	fprintf( fp, "Copper       %d\n",   obj->cost.copper	     );
	fprintf( fp, "Values       %d %d %d %d\n",
			obj->value[0], obj->value[1], obj->value[2], obj->value[3]   );
	fprintf( fp, "Activates    %d %d %d %d\n",
			obj->ac_type, obj->ac_vnum, obj->ac_charge[0], obj->ac_charge[1] );
	fprintf( fp, "AcSpell      %s~\n",  obj->ac_spell                );
	switch ( obj->item_type )
	{
		case ITEM_POTION:
		case ITEM_SCROLL:
			if ( obj->value[1] > 0 )
			{
				fprintf( fp, "Spell 1      '%s'\n", 
						skill_table[obj->value[1]].name );
			}

			if ( obj->value[2] > 0 )
			{
				fprintf( fp, "Spell 2      '%s'\n", 
						skill_table[obj->value[2]].name );
			}

			if ( obj->value[3] > 0 )
			{
				fprintf( fp, "Spell 3      '%s'\n", 
						skill_table[obj->value[3]].name );
			}

			break;

		case ITEM_PILL:
		case ITEM_STAFF:
		case ITEM_LENSE:
		case ITEM_WAND:
			if ( obj->value[3] > 0 )
			{
				fprintf( fp, "Spell 3      '%s'\n", 
						skill_table[obj->value[3]].name );
			}

			break;
	}

	for ( paf = obj->affected; paf; paf = paf->next )
	{
		fprintf( fp, "Affect       %d %d %d %d %d\n",
				skill_table[paf->type].slot,
				paf->duration,
				paf->modifier,
				paf->location,
				paf->bitvector );
	}

	for ( ed = obj->extra_descr; ed; ed = ed->next )
	{
		fprintf( fp, "ExtraDescr   %s~ %s~\n",
				ed->keyword, ed->description );
	}

	fprintf( fp, "End\n\n" );

	if ( obj->contains )
		fwrite_obj( ch, obj->contains, fp, iNest + 1, storage );

	tail_chain();
	return;
}



/*
 * Load a char and inventory into a new ch structure.
 */
bool load_char_obj( DESCRIPTOR_DATA *d, char *name )
{
	FILE      *fp;
	static PC_DATA    pcdata_zero;
	CHAR_DATA *ch;
	char       strsave [ MAX_INPUT_LENGTH ];
	bool       found;

	if ( !char_free )
	{
		ch				= alloc_perm( sizeof( *ch ));
	}
	else
	{
		ch				= char_free;
		char_free			= char_free->next;
	}
	clear_char( ch );

	if ( !pcdata_free )
	{
		ch->pcdata			= alloc_perm( sizeof( *ch->pcdata ));
	}
	else
	{
		ch->pcdata			= pcdata_free;
		pcdata_free			= pcdata_free->next;
	}
	*ch->pcdata				= pcdata_zero;

	d->character			= ch;
	ch->desc				= d;

	ch->name				= str_dup( name );

	ch->prompt                          = str_dup( "<%hhp %mm %vmv> " );
	ch->last_note                       = 0;
	ch->wiznet				= WIZ_PREFIX;
	ch->act				= PLR_BLANK
		| PLR_COMBINE
		| PLR_PROMPT
		| PLR_ANSI
		| PLR_COMBAT;
	ch->pcdata->lname			= NULL;        /* str_dup( "" );   &str_empty[0]; */ 
	ch->pcdata->pwd			= str_dup( "" );
	ch->pcdata->bamfin			= str_dup( "" );
	ch->pcdata->bamfout			= str_dup( "" );
	ch->pcdata->bamfusee                = str_dup( "" );
	ch->pcdata->transto                 = str_dup( "" );
	ch->pcdata->transfrom               = str_dup( "" );
	ch->pcdata->transvict               = str_dup( "" );
	ch->pcdata->slayusee                = str_dup( "" );
	ch->pcdata->slayroom                = str_dup( "" );
	ch->pcdata->slayvict                = str_dup( "" );
	ch->pcdata->afkchar                 = str_dup( "" );
	ch->pcdata->title			= str_dup( "" );
	ch->pcdata->perm_str		= 13;
	ch->pcdata->perm_int		= 13; 
	ch->pcdata->perm_wis		= 13;
	ch->pcdata->perm_dex		= 13;
	ch->pcdata->perm_con		= 13;
	ch->pcdata->condition[COND_THIRST]	= MAX_THIRST;  /*  48  */
	ch->pcdata->condition[COND_FULL]	= MAX_FULL;    /*  48  */
	ch->pcdata->pagelen                 = 60;
	ch->pcdata->security		= 0;	/* OLC */

	ch->pcdata->switched                = FALSE;
	ch->combat_timer			= 0;	/* XOR */
	ch->summon_timer			= 0;	/* XOR */
	ch->imm_flags			= 0;	/* XOR */
	ch->res_flags			= 0;	/* XOR */
	ch->vul_flags			= 0;	/* XOR */
	ch->guild				= NULL;	/* XOR */
	ch->pcdata->bankaccount.gold	= 0;
	ch->pcdata->bankaccount.silver	= 0;
	ch->pcdata->bankaccount.copper	= 0;
	ch->pcdata->alias_list		= NULL; /* TRI */
	ch->pcdata->corpses			= 0;
	ch->pcdata->empowerments		= str_dup("");
	ch->pcdata->detractments		= str_dup("");

	found = FALSE;
	fclose( fpReserve );

	sprintf( strsave, "%s%c/%s", PLAYER_DIR, LOWER(ch->name[0]), capitalize( name ) );

	if ( ( fp = fopen( strsave, "r" ) ) )
	{
		int iNest;

		for ( iNest = 0; iNest < MAX_NEST; iNest++ ) {
			rgObjNest[iNest] = NULL;
		}

		found = TRUE;

		for ( ; ; )
		{
			char  letter;
			char *word;

			letter = fread_letter( fp );
			if ( letter == '*' )
			{
				fread_to_eol( fp );
				continue;
			}

			if ( letter != '#' )
			{
				bug( "Load_char_obj: # not found.", 0 );
				break;
			}

			word = fread_word( fp );
			if ( !str_cmp( word, "PLAYER" ) ) fread_char ( ch, fp );
			else if ( !str_cmp( word, "OBJECT" ) ) fread_obj ( ch, fp, FALSE );
			else if ( !str_cmp( word, "PET"    ) ) fread_pet  ( ch, fp );
			else if ( !str_cmp( word, "STORAGE" ) ) fread_obj ( ch, fp, TRUE );
			/*  	    else if ( !str_cmp( word, "ALIAS"  ) ) fread_alias ( ch, fp );  */
			else if ( !str_cmp( word, "END"    ) ) break;
			else
			{
				bug( "Load_char_obj: bad section.", 0 );
				break;
			}
		}

		fclose( fp );
	}

	fpReserve = fopen( NULL_FILE, "r" );
	return found;
}



/*
 * Read in a char.
 */

#if defined( KEY )
#undef KEY
#endif

#define KEY( literal, field, value )					\
	if ( !str_cmp( word, literal ) )	\
{					\
	field  = value;			\
	fMatch = TRUE;			\
	break;				\
}

void fread_char( CHAR_DATA *ch, FILE *fp )
{
	char *word;
	char  buf [ MAX_STRING_LENGTH ];
	bool  fMatch;

	for ( ; ; )
	{
		word   = feof( fp ) ? "End" : fread_word( fp );
		fMatch = FALSE;

		switch ( UPPER( word[0] ) )
		{
			case '*':
				fMatch = TRUE;
				fread_to_eol( fp );
				break;

			case 'A':
				KEY( "Act",		ch->act,		fread_number( fp ) );
				KEY( "Act2",	ch->act2,		fread_number( fp ) ); 
				KEY( "AffdBy",	ch->affected_by,	fread_number( fp ) );
				KEY( "AffdBy2",     ch->affected_by2,       fread_number( fp ) );
				KEY( "Afkmes",      ch->pcdata->afkchar,    fread_string( fp ) );
				KEY( "Align",	ch->alignment,		fread_number( fp ) );
				KEY( "Antidisarm",  ch->antidisarm,         fread_number( fp ) );
				KEY( "Arenalost",	ch->arenalost,			fread_number( fp ) );
				KEY( "Arenawon",	ch->arenawon,			fread_number( fp ) );
				KEY( "Armr",	ch->armor,		fread_number( fp ) );

				if ( !str_cmp( word, "Aff" ) )
				{
					AFFECT_DATA *paf;

					if ( !affect_free )
					{
						paf		= alloc_perm( sizeof( *paf ) );
					}
					else
					{
						paf		= affect_free;
						affect_free	= affect_free->next;
					}

					paf->type	= fread_number( fp );
					paf->type       = slot_lookup(paf->type);
					paf->duration	= fread_number( fp );
					paf->modifier	= fread_number( fp );
					paf->location	= fread_number( fp );
					paf->bitvector	= fread_number( fp );
					paf->level	= fread_number( fp );
					paf->deleted    = FALSE;
					paf->next	= ch->affected;
					ch->affected	= paf;
					if ( !is_sn(paf->type) || paf->type == 0 )
						paf->deleted = TRUE;
					fMatch = TRUE;
					break;
				}
				if ( !str_cmp( word, "Aff2" ) )
				{
					AFFECT_DATA *paf;

					if ( !affect_free )
					{
						paf		= alloc_perm( sizeof( *paf ));
					}
					else
					{
						paf		= affect_free;
						affect_free	= affect_free->next;
					}

					paf->type	= fread_number( fp );
					paf->type       = slot_lookup(paf->type);
					paf->duration	= fread_number( fp );
					paf->modifier	= fread_number( fp );
					paf->location	= fread_number( fp );
					paf->bitvector	= fread_number( fp );
					paf->level	= fread_number( fp );
					paf->deleted    = FALSE;
					paf->next	= ch->affected2;
					ch->affected2	= paf;
					if ( !is_sn(paf->type) || paf->type == 0 )
						paf->deleted = TRUE;
					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "Alias" ) )
				{
					ALIAS_DATA *pAl;

					fMatch = TRUE;
					if ( !ch->pcdata )
					{
						bug("Fread_char: Alias without pcdata",0);
						fread_string(fp);
						fread_string(fp);
						break;
					}
					pAl = alloc_perm( sizeof( *pAl ) );
					pAl->old = fread_string(fp);
					pAl->new = fread_string(fp);
					pAl->next = ch->pcdata->alias_list;
					ch->pcdata->alias_list = pAl;
					break;
				}

				if ( !str_cmp( word, "AtrMd"  ) )
				{
					ch->pcdata->mod_str  = fread_number( fp );
					ch->pcdata->mod_int  = fread_number( fp );
					ch->pcdata->mod_wis  = fread_number( fp );
					ch->pcdata->mod_dex  = fread_number( fp );
					ch->pcdata->mod_con  = fread_number( fp );
					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "AtrPrm" ) )
				{
					ch->pcdata->perm_str = fread_number( fp );
					ch->pcdata->perm_int = fread_number( fp );
					ch->pcdata->perm_wis = fread_number( fp );
					ch->pcdata->perm_dex = fread_number( fp );
					ch->pcdata->perm_con = fread_number( fp );
					fMatch = TRUE;
					break;
				}
				break;

			case 'B':
				KEY( "Bmfin",	ch->pcdata->bamfin,	fread_string( fp ) );
				KEY( "Bmfout",	ch->pcdata->bamfout,	fread_string( fp ) );
				KEY( "Bmfsee",      ch->pcdata->bamfusee,   fread_string( fp ) );
				if ( !str_cmp( word, "Bank" ) )
				{
					ch->pcdata->bankaccount.gold   = fread_number( fp );
					ch->pcdata->bankaccount.silver = fread_number( fp );
					ch->pcdata->bankaccount.copper = fread_number( fp );
					fMatch = TRUE;
					break;
				}
				break;

			case 'C':
				KEY( "Charisma",    ch->charisma,           fread_number( fp ) );
				KEY( "Corpses",	ch->pcdata->corpses,	fread_number( fp ) );
				KEY( "Clan",        ch->clan,               fread_number( fp ) );
				KEY( "Clvl",        ch->clev,               fread_number( fp ) );
				KEY( "Copper",	ch->money.copper,	fread_number( fp ) );
				KEY( "Ctmr",        ch->ctimer,             fread_number( fp ) );
				if ( !str_cmp( word, "Cla" ) )
				{
					int iClass=0;
					fMatch = TRUE;
					for ( iClass=0; iClass < MAX_CLASS; iClass++ )
						ch->class[ iClass ] = -1;
					for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
					{
						ch->class[ iClass ] = fread_number( fp );
						if ( ch->class[ iClass ] == -1 )
							break;
					}
				}
				KEY( "ClkLev",      ch->cloaked,            fread_number( fp ) );
				KEY( "Cquestpnts",  ch->cquestpnts,         fread_number( fp ) );            

				if ( !str_cmp( word, "Cond" ) )
				{
					ch->pcdata->condition[0] = fread_number( fp );
					ch->pcdata->condition[1] = fread_number( fp );
					ch->pcdata->condition[2] = fread_number( fp );
					fMatch = TRUE;
					break;
				}
				break;

			case 'D':
				KEY( "Dam", 	ch->damroll,		fread_number( fp ) );
				KEY( "Deaf",	ch->deaf,		fread_number( fp ) );
				KEY( "Detract",	ch->pcdata->detractments,fread_string(fp ) );
				KEY( "Dscr",	ch->description,	fread_string( fp ) );
				break;

			case 'E':
				KEY( "Email",	ch->pcdata->email,	fread_string( fp ) );
				KEY( "Empower",	ch->pcdata->empowerments,fread_string(fp ) );
				if ( !str_cmp( word, "End" ) )
				{
					/* Coders have "insane" trusts for a reason ELVIS *
					 * -- Altrag                                      */
					/*	        if ( ch->trust > 107 && ch->level < 108 && !IS_CODER(ch) )
								ch->trust = 0;*/
					if ( IS_CODER(ch) )
						REMOVE_BIT(ch->affected_by2, CODER);
					return;
				}
				KEY( "Exp",		ch->exp,		fread_number( fp ) );
				break;

			case 'G':
				KEY( "Gold",	ch->money.gold,		fread_number( fp ) );
				KEY( "GRank",	ch->guild_rank,		fread_number( fp ) );
				if(!str_cmp(word, "Guild"))
				{
					int i;
					char *guild;
					guild = fread_string(fp);
					fMatch = TRUE;
					for(i = 0;guild_table[i].name[0] != '\0';i++)
					{
						if(!strcmp(guild_table[i].name, guild))
						{
							ch->guild = (GUILD_DATA *)&(guild_table[i]);
							break;
						}
					}
				}
				break;

			case 'H':
				KEY( "Hit", 	ch->hitroll,		fread_number( fp ) );

				if ( !str_cmp( word, "HpMnMv" ) )
				{
					ch->hit		= fread_number( fp );
					ch->perm_hit	= fread_number( fp );
					ch->mod_hit	= fread_number( fp );
					ch->mana	= fread_number( fp );
					ch->perm_mana	= fread_number( fp );
					ch->mod_mana	= fread_number( fp );
					ch->move	= fread_number( fp );
					ch->perm_move	= fread_number( fp );
					ch->mod_move	= fread_number( fp );
					fMatch = TRUE;
					break;
				}
				break;
			case 'I':
				KEY( "ImmBits",	ch->imm_flags,		fread_number( fp ) );
				KEY( "Incarnations",	ch->incarnations,	fread_number( fp ) );
				break;
			case 'L':
				KEY( "Lnm",         ch->pcdata->lname,       fread_string( fp ) );
				KEY( "Lvl", 	ch->level,		fread_number( fp ) );
				if ( !str_cmp( word, "LngDsc" ) )
				{
					fread_to_eol( fp );
					fMatch = TRUE;
					break;
				}
				break;

			case 'N':
				if ( !str_cmp( word, "Nm" ) )
				{
					/*
					 * Name already set externally.
					 */
					fread_to_eol( fp );
					fMatch = TRUE;
					break;
				}
				KEY( "Note",        ch->last_note,          fread_number( fp ) );
				break;

			case 'P':
				KEY( "Pglen",       ch->pcdata->pagelen,    fread_number( fp ) );
				KEY( "Paswd",	ch->pcdata->pwd,	fread_string( fp ) );
				KEY( "Pkilled",	ch->pkilled,			fread_number( fp ) );
				KEY( "Pkills",	ch->pkills,			fread_number( fp ) );
				KEY( "Playd",	ch->played,		fread_number( fp ) );
				KEY( "Plan",	ch->pcdata->plan,	fread_string( fp ) );
				KEY( "Pos", 	ch->position,		fread_number( fp ) );
				KEY( "Prac",	ch->practice,		fread_number( fp ) );
				KEY( "Prmpt",	ch->prompt,		fread_string( fp ) );
				break;

			case 'Q':
				KEY( "QuestPnts",   ch->questpoints,        fread_number( fp ) );
				KEY( "QuestNext",   ch->nextquest,          fread_number( fp ) );
				break;

			case 'R':
				KEY( "Raisepts",	ch->raisepts,	fread_number( fp ) );
				KEY( "Rce",         ch->race,		fread_number( fp ) );
				KEY( "ResBits",	ch->res_flags,		fread_number( fp ) );

				if ( !str_cmp( word, "Room" ) )
				{
					ch->in_room = get_room_index( fread_number( fp ) );
					if ( !ch->in_room )
						ch->in_room = get_room_index( ROOM_VNUM_LIMBO );
					fMatch = TRUE;
					break;
				}

				break;

			case 'S':
				KEY( "SavThr",	ch->saving_throw,	fread_number( fp ) );
				KEY( "SAlign",	ch->start_align,	fread_letter( fp ) );
				KEY( "Sx",		ch->sex,		fread_number( fp ) );
				KEY( "Silver",	ch->money.silver,	fread_number( fp ) );
				KEY( "Slyuc",	ch->pcdata->slayusee,	fread_string( fp ) );
				KEY( "Slyrm",	ch->pcdata->slayroom,	fread_string( fp ) );
				KEY( "Slyvict",	ch->pcdata->slayvict,	fread_string( fp ) );
				if ( !str_cmp( word, "Stun" ) )
				{
					ch->stunned[0] = fread_number( fp );
					ch->stunned[1] = fread_number( fp );
					ch->stunned[2] = fread_number( fp );
					ch->stunned[3] = fread_number( fp );
					ch->stunned[4] = fread_number( fp );
					fMatch = TRUE;
					break;
				}
				if ( !str_cmp( word, "ShtDsc" ) )
				{
					fread_to_eol( fp );
					fMatch = TRUE;
					break;
				}
				KEY( "Security",    ch->pcdata->security,	fread_number( fp ) );	/* OLC */

				if ( !str_cmp( word, "Skll" ) )
				{
					int sn;
					int value;

					value = fread_number( fp );
					sn    = skill_lookup( fread_word( fp ) );
					if ( sn < 0 )
						bug( "Fread_char: unknown skill.", 0 );
					else
						ch->pcdata->learned[sn] = value;
					fMatch = TRUE;
				}

				break;

			case 'T':
				KEY( "Trst",	ch->trust,		fread_number( fp ) );
				KEY( "Trnsto",	ch->pcdata->transto,	fread_string( fp ) );
				KEY( "Trnsfrom",	ch->pcdata->transfrom,	fread_string( fp ) );
				KEY( "Trnvict",	ch->pcdata->transvict, 	fread_string( fp ) );

				if ( !str_cmp( word, "Ttle" ) )
				{
					ch->pcdata->title = fread_string( fp );
					if (   isalpha( ch->pcdata->title[0] )
							|| isdigit( ch->pcdata->title[0] ) )
					{
						sprintf( buf, " %s", ch->pcdata->title );
						free_string( ch->pcdata->title );
						ch->pcdata->title = str_dup( buf );
					}
					fMatch = TRUE;
					break;
				}

				break;

			case 'V':
				KEY( "VulBits",	ch->vul_flags,	fread_number( fp ) );

				if ( !str_cmp( word, "Vnum" ) )
				{
					ch->pIndexData = get_mob_index( fread_number( fp ) );
					fMatch = TRUE;
					break;
				}
				break;

			case 'W':
				KEY( "Wardeaths",	ch->wardeaths,		fread_number( fp ) );
				KEY( "Warkills",	ch->warkills,		fread_number( fp ) );
				KEY( "Warpts",	ch->warpts,		fread_number( fp ) );
				KEY( "Wiznet",	ch->wiznet,		fread_number( fp ) );
				KEY( "Wimp",	ch->wimpy,		fread_number( fp ) );
				KEY( "Wizbt",	ch->wizbit,		fread_number( fp ) );
				KEY( "WizLev",      ch->wizinvis,           fread_number( fp ) );
				KEY( "Whotype",	ch->pcdata->whotype,	fread_string( fp ) );
				break;
		}

		/* Make sure old chars have this field - Kahn */
		if ( !ch->pcdata->pagelen ) {
			ch->pcdata->pagelen = 60;
		}

		if ( !ch->prompt || ch->prompt == '\0' ) {
			ch->prompt = str_dup ( "<%hhp %mm %vmv> " );
		}

		if ( !fMatch ) {
			sprintf( buf, "Fread_char: %s -> no match.", word );
			bug( buf, 0 );
			fread_to_eol( fp );
		}
	}
}



void fread_obj( CHAR_DATA *ch, FILE *fp, bool storage )
{
	static OBJ_DATA  obj_zero;
	OBJ_DATA *obj;
	char     *word;
	int       iNest;
	bool      fMatch;
	bool      fNest;
	bool      fVnum;

	if ( !obj_free )
	{
		obj		= alloc_perm( sizeof( *obj ) );
	}
	else
	{
		obj		= obj_free;
		obj_free	= obj_free->next;
	}

	*obj		= obj_zero;
	obj->name		= str_dup( "" );
	obj->short_descr	= str_dup( "" );
	obj->description	= str_dup( "" );
	obj->deleted        = FALSE;

	fNest		= FALSE;
	fVnum		= TRUE;
	iNest		= 0;

	for ( ; ; )
	{
		word   = feof( fp ) ? "End" : fread_word( fp );
		fMatch = FALSE;

		switch ( UPPER( word[0] ) )
		{
			case '*':
				fMatch = TRUE;
				fread_to_eol( fp );
				break;

			case 'A':

				KEY( "AntiClassF",	obj->anti_class_flags,	fread_number( fp ) );
				KEY( "AntiRaceF",	obj->anti_race_flags,	fread_number( fp ) );
				if ( !str_cmp( word, "Activates") )
				{	
					obj->ac_type	  = fread_number( fp );
					obj->ac_vnum	  = fread_number( fp );
					obj->ac_charge[0] = fread_number( fp );
					obj->ac_charge[1] = fread_number( fp );
					fMatch		= TRUE;
					break;
				}

				if ( !str_cmp( word, "AcSpell" ) )
				{
					obj->ac_spell     = fread_string( fp ); 
					fMatch            = TRUE;
					break;
				}
				if ( !str_cmp( word, "Affect" ) )
				{
					AFFECT_DATA *paf;

					if ( !affect_free )
					{
						paf		= alloc_perm( sizeof( *paf ));
					}
					else
					{
						paf		= affect_free;
						affect_free	= affect_free->next;
					}

					paf->type	= fread_number( fp );
					paf->type       = slot_lookup(paf->type);
					paf->duration	= fread_number( fp );
					paf->modifier	= fread_number( fp );
					paf->location	= fread_number( fp );
					paf->bitvector	= fread_number( fp );
					paf->next	= obj->affected;
					obj->affected	= paf;
					fMatch		= TRUE;
					break;
				}
				break;

			case 'C':
				KEY( "Copper", 	obj->cost.copper,	fread_number( fp ) ); 
				KEY( "Cost",	obj->cost.copper,	fread_number( fp ) );
				break;

			case 'D':
				KEY( "Description",	obj->description,	fread_string( fp ) );
				break;

			case 'E':
				KEY( "ExtraFlags",	obj->extra_flags,	fread_number( fp ) );
				if ( !str_cmp( word, "ExtraDescr" ) )
				{
					EXTRA_DESCR_DATA *ed;

					if ( !extra_descr_free )
					{
						ed			= alloc_perm( sizeof( *ed ) );
					}
					else
					{
						ed			= extra_descr_free;
						extra_descr_free	= extra_descr_free->next;
					}

					ed->keyword		= fread_string( fp );
					ed->description		= fread_string( fp );
					ed->next		= obj->extra_descr;
					obj->extra_descr	= ed;
					fMatch = TRUE;
				}

				if ( !str_cmp( word, "End" ) )
				{
					if ( !fNest || !fVnum )
					{
						bug( "Fread_obj: incomplete object.", 0 );
						free_string( obj->name        );
						free_string( obj->description );
						free_string( obj->short_descr );
						/*		    obj->next = obj_free;
									obj_free  = obj;*/
						free_mem( obj, sizeof( *obj ) );
						return;
					}
					else
					{
						obj->next	= object_list;
						object_list	= obj;
						obj->pIndexData->count++;
						if ( iNest == 0 || !rgObjNest[iNest] )
						{
							if ( storage && !IS_NPC( ch ) )
								obj_to_storage( obj, ch );
							else
								obj_to_char( obj, ch );
						}
						else
							obj_to_obj( obj, rgObjNest[iNest-1] );
						if ( obj->item_type == ITEM_POTION )
							SET_BIT( obj->extra_flags, ITEM_NO_LOCATE );
						if ( obj->item_type == ITEM_NOTEBOARD
								|| obj->item_type == ITEM_PORTAL )
							extract_obj( obj );
						return;
					}
				}
				break;
			case 'G':
				KEY( "Gold",	obj->cost.gold,		fread_number( fp ) );
				break; 
			case 'I':
				KEY( "ItemType",	obj->item_type,		fread_number( fp ) );
				break;

			case 'L':
				KEY( "Level",	obj->level,		fread_number( fp ) );
				break;

			case 'N':
				KEY( "Name",	obj->name,		fread_string( fp ) );

				if ( !str_cmp( word, "Nest" ) )
				{
					iNest = fread_number( fp );
					if ( iNest < 0 || iNest >= MAX_NEST )
					{
						bug( "Fread_obj: bad nest %d.", iNest );
					}
					else
					{
						rgObjNest[iNest] = obj;
						fNest = TRUE;
					}
					fMatch = TRUE;
				}
				break;

			case 'S':
				KEY( "ShortDescr",	obj->short_descr,	fread_string( fp ) );
				KEY( "Silver", 	obj->cost.silver,	fread_number( fp ) ); 
				if ( !str_cmp( word, "Spell" ) )
				{
					int iValue;
					int sn;

					iValue = fread_number( fp );
					sn     = skill_lookup( fread_word( fp ) );
					if ( iValue < 0 || iValue > 3 )
					{
						bug( "Fread_obj: bad iValue %d.", iValue );
					}
					else if ( sn < 0 )
					{
						bug( "Fread_obj: unknown skill.", 0 );
					}
					else
					{
						obj->value[iValue] = sn;
					}
					fMatch = TRUE;
					break;
				}

				break;

			case 'T':
				KEY( "Timer",	obj->timer,		fread_number( fp ) );
				break;

			case 'V':
				if ( !str_cmp( word, "Values" ) )
				{
					obj->value[0]	= fread_number( fp );
					obj->value[1]	= fread_number( fp );
					obj->value[2]	= fread_number( fp );
					obj->value[3]	= fread_number( fp );
					fMatch		= TRUE;
					break;
				}

				if ( !str_cmp( word, "Vnum" ) )	/* OLC */
				{
					int vnum;

					vnum = fread_number( fp );
					if ( !( obj->pIndexData = get_obj_index( vnum ) ) )
						obj->pIndexData = get_obj_index( OBJ_VNUM_DUMMY );

					fVnum = TRUE;
					fMatch = TRUE;
					break;
				}
				break;

			case 'W':
				KEY( "WearFlags",	obj->wear_flags,	fread_number( fp ) );
				KEY( "WearLoc",	obj->wear_loc,		fread_number( fp ) );
				KEY( "Weight",	obj->weight,		fread_number( fp ) );
				KEY( "Wiznet",	ch->wiznet,		fread_number( fp ) );
				break;

		}

		if ( !fMatch )
		{
			bug( "Fread_obj: no match.", 0 );
			fread_to_eol( fp );
		}
	}
}

void save_pet( CHAR_DATA *ch, FILE *fp, CHAR_DATA *pet )
{
	AFFECT_DATA  *paf;
	fprintf( fp, "#PET\n" );
	fprintf( fp, "%d %d %d %d %d %ld %ld %d %s~ %s~\n", pet->pIndexData->vnum, 
			pet->hit, pet->perm_hit, pet->mod_hit, pet->act, pet->affected_by,pet->affected_by2,
			pet->level, pet->short_descr, pet->long_descr );  

	for ( paf = pet->affected; paf; paf = paf->next )
	{
		if ( paf->deleted )
			continue;

		fprintf( fp, "Aff       %3d %3d %3d %3d %10d %3d\n",
				skill_table[paf->type].slot,
				paf->duration,
				paf->modifier,
				paf->location,
				paf->bitvector,
				paf->level );
	}
	for ( paf = pet->affected2; paf; paf = paf->next )
	{
		if ( paf->deleted )
			continue;

		fprintf( fp, "Aff2       %3d %3d %3d %3d %10d %3d\n",
				skill_table[paf->type].slot,
				paf->duration,
				paf->modifier,
				paf->location,
				paf->bitvector,
				paf->level );
	}

	fprintf( fp, "EndOfPet" );

	fprintf( fp, "\n" );
	return;
}  

void fread_pet( CHAR_DATA *ch, FILE *fp )
{
	MOB_INDEX_DATA *pMob;
	CHAR_DATA *pet;
	int vnum;
	char * word;  
	vnum = fread_number( fp );
	if ( ( pMob = get_mob_index( vnum ) ) == NULL ||
			( pet = create_mobile( pMob ) ) == NULL )
	{
		for ( vnum = 0; vnum < 5; vnum++ )
			fread_number( fp );
		return;
	}
	char_to_room( pet, ch->in_room );
	pet->master = ch;
	pet->hit = fread_number( fp );
	pet->perm_hit = fread_number( fp );
	pet->mod_hit = fread_number( fp );
	pet->act = fread_number( fp );
	pet->affected_by = fread_number( fp ) | AFF_CHARM;
	pet->affected_by2 = fread_number( fp );
	pet->level = fread_number( fp );
	free_string(pet->short_descr); 
	pet->short_descr = str_dup(fread_string( fp ));
	free_string(pet->long_descr);
	pet->long_descr = str_dup(fread_string( fp ));

	for ( ; ; )
	{
		word   =  fread_word( fp );

		if ( !str_cmp( word, "EndOfPet" ) )
			break;

		else if ( !str_cmp( word, "Aff" ) )
		{
			AFFECT_DATA *paf;

			if ( !affect_free )
			{
				paf		= alloc_perm( sizeof( *paf ) );
			}
			else
			{
				paf		= affect_free;
				affect_free	= affect_free->next;
			}

			paf->type	= fread_number( fp );
			paf->type       = slot_lookup(paf->type);
			paf->duration	= fread_number( fp );
			paf->modifier	= fread_number( fp );
			paf->location	= fread_number( fp );
			paf->bitvector	= fread_number( fp );
			paf->level	= fread_number( fp );
			paf->deleted    = FALSE;
			paf->next	= pet->affected;
			pet->affected	= paf;
			if ( !is_sn(paf->type) || paf->type == 0 )
				paf->deleted = TRUE;
		}
		else if ( !str_cmp( word, "Aff2" ) )
		{
			AFFECT_DATA *paf;

			if ( !affect_free )
			{
				paf		= alloc_perm( sizeof( *paf ));
			}
			else
			{
				paf		= affect_free;
				affect_free	= affect_free->next;
			}

			paf->type	= fread_number( fp );
			paf->type       = slot_lookup(paf->type);
			paf->duration	= fread_number( fp );
			paf->modifier	= fread_number( fp );
			paf->location	= fread_number( fp );
			paf->bitvector	= fread_number( fp );
			paf->level	= fread_number( fp );
			paf->deleted    = FALSE;
			paf->next	= pet->affected2;
			pet->affected2	= paf;
			if ( !is_sn(paf->type) || paf->type == 0 )
				paf->deleted = TRUE;
		}
	}
	return;
}

void fread_alias( CHAR_DATA *ch, FILE *fp )
{
	ALIAS_DATA *iAl;

	iAl = ch->pcdata->alias_list;

	for ( ; ; )
	{
		char *word = NULL;
		char *word1 = NULL;
		char letter;

		letter = fread_letter( fp );
		bug("letter: %c", letter );

		switch( letter )
		{
			case '~' :
				fread_to_eol( fp );
				return;
			default:
				ungetc( letter, fp );
				word = fread_string( fp );
				word1 = fread_string( fp );
				add_alias( ch, iAl, word, word1 );
				fread_to_eol( fp );
				sprintf( log_buf, "%s %s", word, word1 );
				bug( log_buf, 0 );
				break;
		}
	}
	return;
}

void fwrite_alias( CHAR_DATA *ch, FILE *fp )
{
	ALIAS_DATA *pAl;

	fprintf(fp, "#ALIAS\n");
	for ( pAl = ch->pcdata->alias_list; pAl; pAl = pAl->next )
	{
		fprintf( fp, "%s~ %s~\n", pAl->old, pAl->new );
		bug( "Writing alias...", 0 );
	}
	fprintf( fp, "~\n\n" );
	return;
}

/*
 * Assumes ch->pcdata->corpses & that it is initialized to 0.
 * This routine WILL NOT work as written without it.
 * checks to make sure corpse is not empty before
 * reading/writing the corpse file.
 */
void corpse_back( CHAR_DATA *ch, OBJ_DATA *corpse )
{
	FILE      *fp; 	
	OBJ_DATA  *obj, *obj_next;
	OBJ_DATA  *obj_nest,  *objn_next;
	char       strsave[MAX_INPUT_LENGTH ];
	char       buf    [MAX_STRING_LENGTH]; 	 	
	int        corpse_cont[1024];
	int        item_level [1024];
	int        c =1; 
	int        checksum1 =0;
	int        checksum2 =0;

	/* Don't do anything if the corpse is empty */
	if (!corpse->contains)
		return;

	if ( IS_NPC( ch ) )
		return;         

	/* Ok, it isn't empty determine the # of items in the corpse. 
	 * Store the items in the LAST number of the array to  write 
	 * it backwards.  Easiest way to do it.
	 */
	for ( obj = corpse->contains; obj; obj = obj_next )
	{
		obj_next = obj->next_content;
		if ( obj->item_type == ITEM_POTION )
			continue;
		corpse_cont[c] = obj->pIndexData->vnum;
		item_level[c] = obj->level;
		checksum1 += corpse_cont[c];        
		checksum2 +=  item_level[c];        
		++c;
		if ( obj->contains) /* get stuff in containers */
		{
			for ( obj_nest = obj->contains; obj_nest; obj_nest = objn_next )
			{
				objn_next = obj_nest->next_content;
				if ( obj_nest->item_type == ITEM_POTION )
					continue;
				corpse_cont[c] = obj_nest->pIndexData->vnum;
				item_level[c] = obj_nest->level;
				checksum1 += corpse_cont[c];        
				checksum2 +=  item_level[c];        
				++c;
			}     
		}
	}
	/* Check the corpse for only one item. Assumes if true the the player
	 * died trying to retrieve their corpse. Change it if you like.
	 */
	if (c <= 2 ) 
		return;

	/* Add in the number of items and checksum for validation check */
	corpse_cont[0] = c -1;
	item_level[0] = c -1;
	corpse_cont[c+1] = checksum1;
	item_level[c+1] = checksum2;

	/* Ok now we have a corpse to save, is it the first one? */

	fclose( fpReserve );

	/* player files parsed directories by Yaz 4th Realm */
	sprintf( strsave, "%s%c/%s.cps", PLAYER_DIR, LOWER(ch->name[0]), capitalize( ch->name ) );

	if ( !( fp = fopen( strsave, "w" ) ) )
	{
		sprintf( buf, "Corpse back: fopen %s: ", ch->name );
		bug( buf, 0 );
		perror( strsave );
	}
	else
	{
		int i;
		for ( i = 0 ; i < c ; i++ )
		{
			fprintf( fp, "%d ", corpse_cont[i] );
			fprintf( fp, "%d ",  item_level[i] );
		}
		fprintf( fp, "%d ",  corpse_cont[i+1] );      
		fprintf( fp, "%d\n",  item_level[i+1] );      
	}
	fclose( fp );        
	fpReserve = fopen( NULL_FILE, "r" );
	ch->pcdata->corpses = 1;
	return;

	fclose( fpReserve );

	/* Okay, it isn't the first corpse, read the rest */
	/*
	   if ( !( fp = fopen( strsave, "r" ) ) )
	   {
	   sprintf( buf, "Corpse back: fopen %s: ", ch->name );
	   bug( buf, 0 );
	   perror( strsave );
	   }
	   else
	   {
	   for ( i=4 ; i > 0 ; i-- )
	   {
	   corpse_cont[i][0] = fread_number( fp );
	   item_level[i][0]  = fread_number( fp );

	   if ( corpse_cont[i][0] == 99 )
	   break;

	   for ( c = 1 ; c < corpse_cont[i][0] +2 ; c++ )
	   {    
	   corpse_cont[i][c] = fread_number ( fp );            
	   item_level[i][c]  = fread_number ( fp );        
	   }
	   }

	   }
	   fclose( fp );


	   if ( !( fp = fopen( strsave, "w" ) ) )
	   {
	   sprintf( buf, "Corpse back: fopen %s: ", ch->name );
	   bug( buf, 0 );
	   perror( strsave );
	   }
	   else
	   {
	   for ( i=5 ; i > 0 ; i-- )
	   {
	   if ( corpse_cont[i][0] == 99 )
	   break;

	   fprintf( fp, "%d ", corpse_cont[i][0] );
	   fprintf( fp, "%d ",  item_level[i][0] );
	   checksum1 = 0;
	   checksum2 = 0;

	   for ( c = 1 ; c < corpse_cont[i][0] +1 ; c++ )
	   {    
	   fprintf( fp, "%d " , corpse_cont[i][c]  );            
	   fprintf( fp, "%d " ,  item_level[i][c]  );
	   checksum1 += corpse_cont[i][c];
	   checksum2 +=  item_level[i][c];
	   }
	   fprintf( fp, "%d ", checksum1  );
	   fprintf( fp, "%d\n", checksum2 );
	   }
	   fprintf( fp, "99 99" );      

	   }
	   fclose( fp );
	   fpReserve = fopen( NULL_FILE, "r" );
	   return;
	   */
}            


void save_finger ( CHAR_DATA *ch )
{
	// busted, disabling for now -- Ristowe
	return;

	FILE *fp;
	char buf      [ MAX_STRING_LENGTH ];
	char fingsave [ MAX_INPUT_LENGTH  ];

	if ( IS_NPC( ch ) || ch->level < 2 )
		return;

	if ( ch->desc && ch->desc->original )
		ch = ch->desc->original;

	sprintf( fingsave, "%s%c/%s.fng", PLAYER_DIR, LOWER(ch->name[0]), capitalize( ch->name ) );

	fclose(fpReserve);
	if ( !( fp = fopen ( fingsave, "w" ) ) )
	{
		sprintf( buf, "Save_finger_Info: fopen %s ", ch->name );
		bug( buf, 0 );
		perror( fingsave );
	}
	else
		fwrite_finger( ch, fp );

	fclose( fp );
	fpReserve = fopen( NULL_FILE, "r" );
	return;
}

void fwrite_finger( CHAR_DATA *ch, FILE *fp )
{

	fprintf( fp, "&CName: &W%s\n",		ch->name	);
	fprintf( fp, "&CMud Age: &W%-13d",		get_age( ch )	);
	fprintf( fp, "&CClan: &W%s\n", get_clan_index( ch->clan )->name );
	fprintf( fp, "&CLevel: &W%-16d",		ch->level	);
	fprintf( fp, "&CSex: &W%s\n",	  ch->sex == SEX_MALE ? "male" :
			ch->sex == SEX_FEMALE ? "female" : "neutral" );
	fprintf( fp, "&CClass: &W%-15s",	class_short( ch ) );
	fprintf( fp, "&CRace: &W%s\n",	(get_race_data(ch->race))->race_name );
	if ( is_class( ch, CLASS_THIEF ) )
		fprintf( fp, "&CThief: &W%s\n", ( IS_SET( ch->act, PLR_THIEF ) ) ? "Yes"
				: "No" ); 
	/*  fprintf( fp, "Mobs Killed: %-10d",   ch->pcdata->kills	);
		fprintf( fp, "Deaths: %d\n\n", 	   ch->pcdata->died	); */
	fprintf( fp, "&CTitle: &W%s\n",			ch->pcdata->title );
	fprintf( fp, "&CEmail: &W%s\n",			ch->pcdata->email );
	fprintf( fp, "&CPlan: &W%s\n",			ch->pcdata->plan );
	fprintf( fp, "&CPkills: &W%-15d",			ch->pkills );
	fprintf( fp, "&CPkilled: &W%d\n",			ch->pkilled );
	fprintf( fp, "&CArena Wins: &W%-11d",		ch->arenawon );
	fprintf( fp, "&CArena Losses: &W%d\n",	ch->arenalost );
	fprintf( fp, "&CWar Kills: &W%-12d",			ch->warkills );
	fprintf( fp, "&CWar Deaths: &W%-11d",			ch->wardeaths );
	fprintf( fp, "&CWar Points: &W%d\n",			ch->warpts );
	fprintf( fp, "&CLast On: &W%s~\n", 		(char * ) ctime(&ch->logon) );
	return;
}
/*
   void read_finger ( CHAR_DATA *ch, char *argument )
   {
   FILE *fp;
   char buf [ MAX_STRING_LENGTH ];
   char fingload [ MAX_INPUT_LENGTH ];
   char arg [ MAX_INPUT_LENGTH ];
   char arg2 [ MAX_INPUT_LENGTH ];

   argument = one_argument( argument, arg );
   argument = one_argument( argument, arg2 );

   sprintf( fingload, "%s%c/%s.fng", PLAYER_DIR, LOWER(arg[0]),
   capitalize( arg ) );

   fclose(fpReserve);
   if ( !( fp = fopen ( fingload, "r" ) ) )
   {
   sprintf( buf, "Load_finger_Info: fopen %s ", arg );
   bug( buf, 0 );
   perror( fingload );
   sprintf( buf, "The character %s doesn't exist.\n\r", arg );
   send_to_char( AT_WHITE, buf, ch );
   }
   else
   fread_finger ( ch, fp );

   fclose( fp );
   fpReserve = fopen( NULL_FILE, "r" );
   return;
   }

   void fread_finger ( CHAR_DATA *ch, FILE *fp )
   {
   char *finger;
   char  buf[MAX_STRING_LENGTH];

   sprintf( buf, "          Finger Info             \n\r" );
   send_to_char( AT_WHITE, buf, ch );

   sprintf( buf, "          ------ ----\n\r\n\r" );
   send_to_char( AT_WHITE, buf, ch );

   finger = fread_string( fp );
   sprintf( buf, "%s", finger );
   send_to_char( AT_WHITE, buf, ch );
   send_to_char( AT_WHITE, "\n\r", ch );
   return;
   } 
   */
void read_finger ( CHAR_DATA *ch, char *argument )
{
	FILE *fp;
	char buf [ MAX_STRING_LENGTH ];
	char fingload [ MAX_INPUT_LENGTH ];
	char arg [ MAX_INPUT_LENGTH ];
	char arg2 [ MAX_INPUT_LENGTH ];
	argument = one_argument( argument, arg );
	argument = one_argument(argument, arg2 );
	sprintf( fingload, "%s%c/%s.fng", PLAYER_DIR, LOWER(arg[0]), capitalize( arg ) );
	fclose(fpReserve);
	if ( !( fp = fopen ( fingload, "r" ) ) )
	{
		sprintf( buf, "Load_finger_Info: fopen %s ", arg );
		bug( buf, 0 );
		perror( fingload );
		sprintf( buf, "The character %s doesn't exist.\n\r", arg );
		send_to_char( AT_WHITE, buf, ch );
	}  else  {
		sprintf( buf, "%s", capitalize( arg ) );
		fread_finger ( ch, fp, buf );
	}
	if(fp)
		fclose( fp );
	fpReserve = fopen( NULL_FILE, "r" );
	return;
}
void fread_finger ( CHAR_DATA *ch, FILE *fp, char *name )
{
	char     *finger;
	char  buf[MAX_STRING_LENGTH];
	/*CHAR_DATA  *wch;*/
	/*CHAR_DATA *victim;*/
	/* sprintf( buf, "          Finger Info\n\r" );
	   send_to_char( AT_WHITE, buf, ch );
	   sprintf( buf,
	   "          ------ ----\n\r\n\r" );  send_to_char( AT_WHITE, buf, ch );  */
	sprintf( buf, "\n\r      &CInfo on &W%s\n\r\n\r", name);
	send_to_char( AT_WHITE, buf, ch );
	finger = fread_string( fp );
	sprintf( buf, "%s", finger );
	send_to_char( AT_WHITE, buf, ch );
	send_to_char( AT_WHITE, "\n\r", ch );
	return;
}


void save_banlist( BAN_DATA *banlist )
{
	BAN_DATA *ban;
	FILE *fp;
	char buf [ MAX_INPUT_LENGTH ];

	if ( !(fp = fopen( BAN_LIST, "w" )) )
	{
		bug( "Unable to open BAN_LIST", 0 );
		return;
	}
	for ( ban = ban_list; ban; ban = ban->next )
	{
		if ( ban->user )
			sprintf( buf, "%c %s@%s", ban->type, ban->user, ban->name );
		else
			sprintf( buf, "%c %s", ban->type, ban->name );
		fprintf( fp, "%s~\n", buf );
	}
	fprintf( fp, "$\n" );
	fclose( fp );
	return;
}
