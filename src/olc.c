/***************************************************************************
 *  File: olc.c                                                            *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This code was freely distributed with the The Isles 1.1 source code,   *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 *                                                                         *
 ***************************************************************************/


#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "olc.h"


/*
 * Local functions.
 */
void	save_area		args( ( AREA_DATA *pArea ) );
void	forge_pay_cost		args( ( CHAR_DATA *ch ) );
/* AREA_DATA  *get_area_data	args( ( int vnum ) ); */
MPROG_DATA *get_mprog_data      args( ( MOB_INDEX_DATA *pMob, int vnum ) );
/*
   TRAP_DATA  *get_trap_data       args( ( void *vo, int vnum, int type ) );
   */
/* Executed from comm.c.  Minimizes compiling when changes are made. */
bool run_olc_editor( DESCRIPTOR_DATA *d )
{
	switch ( d->editor )
	{
		case ED_AREA:
			aedit( d->character, d->incomm );
			break;
		case ED_ROOM:
			redit( d->character, d->incomm );
			break;
		case ED_OBJECT:
			oedit( d->character, d->incomm );
			break;
		case ED_MOBILE:
			medit( d->character, d->incomm );
			break;
		case ED_MPROG:
			mpedit( d->character, d->incomm );
			break;
		case ED_CLAN:
			cedit( d->character, d->incomm );
			break;
		case ED_HELP:
			hedit( d->character, d->incomm );	/* XOR */
			break;
		case ED_OPROG:
		case ED_RPROG:
		case ED_EPROG:
			tedit( d->character, d->incomm );
			break;
		case ED_SOCIAL:
			sedit( d->character, d->incomm );
			break;
		case RENAME_OBJECT:
			rename_object( d->character, d->incomm );
			break;
		case FORGE_OBJECT:
			forge_object( d->character, d->incomm );
			break;
		case ED_MRESET:
			mreset( d->character, d->incomm );
			break;
		case ED_NEWBIE:
			nedit( d->character, d->incomm );
			break;
		case ED_SPEDIT:
			spedit( d->character, d->incomm );
			break;
		default:
			return FALSE;
	}
	return TRUE;
}



char *olc_ed_name( CHAR_DATA *ch )
{
	static char buf[11];

	buf[0] = '\0';
	switch (ch->desc->editor)
	{
		case ED_AREA:
			sprintf( buf, "AEdit" );
			break;
		case ED_ROOM:
			sprintf( buf, "REdit" );
			break;
		case ED_OBJECT:
			sprintf( buf, "OEdit" );
			break;
		case ED_MOBILE:
			sprintf( buf, "MEdit" );
			break;
		case ED_MPROG:
			sprintf( buf, "MPEdit" );
			break;
		case ED_CLAN:
			sprintf( buf, "CEdit" );
			break;
		case ED_HELP:
			sprintf( buf, "HEdit" );
			break;
		case ED_OPROG:
			sprintf( buf, "OPEdit" );
			break;
		case ED_RPROG:
			sprintf( buf, "RPEdit" );
			break;
		case ED_EPROG:
			sprintf( buf, "EPEdit" );
			break;
		case ED_SOCIAL:
			sprintf( buf, "SEdit"  );
			break;
		case ED_MRESET:
			sprintf( buf, "MResetEdit" );
			break;
		case RENAME_OBJECT:
			sprintf( buf, "Rename_Obj" );
			break;
		case FORGE_OBJECT:
			sprintf( buf, "Forge_Obj" );
			break;
		case ED_NEWBIE:
			sprintf( buf, "NEdit" );
			break;
		case ED_SPEDIT:
			sprintf( buf, "SPEdit" );
			break;
		default:
			sprintf( buf, " " );
			break;
	}
	return buf;
}



char *olc_ed_vnum( CHAR_DATA *ch )
{
	AREA_DATA *pArea;
	ROOM_INDEX_DATA *pRoom;
	OBJ_INDEX_DATA *pObj;
	MOB_INDEX_DATA *pMob;
	CLAN_DATA      *pClan;
	static char buf[10];

	buf[0] = '\0';
	switch ( ch->desc->editor )
	{
		case ED_AREA:
			pArea = (AREA_DATA *)ch->desc->pEdit;
			sprintf( buf, "%d", pArea ? pArea->vnum : 0 );
			break;
		case ED_ROOM:
			pRoom = ch->in_room;
			sprintf( buf, "%d", pRoom ? pRoom->vnum : 0 );
			break;
		case ED_OBJECT:
			pObj = (OBJ_INDEX_DATA *)ch->desc->pEdit;
			sprintf( buf, "%d", pObj ? pObj->vnum : 0 );
			break;
		case ED_MOBILE:
			pMob = (MOB_INDEX_DATA *)ch->desc->pEdit;
			sprintf( buf, "%d", pMob ? pMob->vnum : 0 );
			break;
		case ED_MPROG:
			pMob = (MOB_INDEX_DATA *)ch->desc->inEdit;
			sprintf( buf, "%d", pMob ? pMob->vnum : 0 );
			break;
		case ED_CLAN:
			pClan = (CLAN_DATA *)ch->desc->pEdit;
			sprintf( buf, "%d", pClan ? pClan->vnum : 0 );
			break;
		case ED_OPROG:
			pObj = (OBJ_INDEX_DATA *)ch->desc->inEdit;
			sprintf( buf, "%d", pObj ? pObj->vnum : 0 );
			break;
		case ED_RPROG:
			pRoom = ch->in_room;
			sprintf( buf, "%d", pRoom ? pRoom->vnum : 0 );
			break;
		case ED_EPROG:
			{
				/* Woah.. this is a long way around nothing.. :) */
				int dir;
				EXIT_DATA *pExit;

				pRoom = ch->in_room;

				for ( dir = 0; dir < 6; dir++ ) {
					if ( (pExit = pRoom->exit[dir]) )
					{
						TRAP_DATA *pTrap;

						for ( pTrap = pExit->traps; pTrap; pTrap = pTrap->next_here )
							if ( pTrap == (TRAP_DATA *)ch->desc->pEdit)
								break;
						if ( pTrap )
							break;
					}
				}

				if ( dir < 6 ) {
					sprintf( buf, "%s", capitalize( dir_name[dir] ) );
				} else {
					sprintf( buf, "Unknown" );
				}

				break;
			}
		default:
			sprintf( buf, " " );
			break;
	}

	return buf;
}



/*****************************************************************************
Name:		show_olc_cmds
Purpose:	Format up the commands from given table.
Called by:	show_commands(olc_act.c).
 ****************************************************************************/
void show_olc_cmds( CHAR_DATA *ch, const struct olc_cmd_type *olc_table )
{
	char buf  [ MAX_STRING_LENGTH ];
	char buf1 [ MAX_STRING_LENGTH ];
	int  cmd;
	int  col;

	buf1[0] = '\0';
	col = 0;
	for (cmd = 0; olc_table[cmd].name[0] != '\0'; cmd++)
	{
		sprintf( buf, "%-15.15s", olc_table[cmd].name );
		strcat( buf1, buf );
		if ( ++col % 5 == 0 )
			strcat( buf1, "\n\r" );
	}

	if ( col % 5 != 0 )
		strcat( buf1, "\n\r" );

	send_to_char(C_DEFAULT, buf1, ch );
	return;
}



/*****************************************************************************
Name:		show_commands
Purpose:	Display all olc commands.
Called by:	olc interpreters.
 ****************************************************************************/
bool show_commands( CHAR_DATA *ch, char *argument )
{
	switch (ch->desc->editor)
	{
		case ED_AREA:
			show_olc_cmds( ch, aedit_table );
			break;
		case ED_ROOM:
			show_olc_cmds( ch, redit_table );
			break;
		case ED_OBJECT:
			show_olc_cmds( ch, oedit_table );
			break;
		case ED_MOBILE:
			show_olc_cmds( ch, medit_table );
			break;
		case ED_MPROG:
			show_olc_cmds( ch, mpedit_table );
			break;
		case ED_CLAN:
			show_olc_cmds( ch, cedit_table );
			break;
		case ED_HELP:
			show_olc_cmds( ch, hedit_table );
			break;
		case ED_OPROG:
		case ED_RPROG:
		case ED_EPROG:
			show_olc_cmds( ch, tedit_table );
			break;
		case ED_SOCIAL:
			show_olc_cmds( ch, sedit_table );
			break;
		case RENAME_OBJECT:
			show_olc_cmds( ch, rename_obj_table );
			break;
		case FORGE_OBJECT:
			show_olc_cmds( ch, forge_obj_table );
			break;
		case ED_MRESET:
			show_olc_cmds( ch, mreset_table );
			break;
		case ED_SPEDIT:
			show_olc_cmds( ch, spedit_table );
			break;
		case ED_NEWBIE:
			show_olc_cmds( ch, nedit_table );
			break;
	}

	return FALSE;
}



/*****************************************************************************
 *                           Interpreter Tables.                             *
 *****************************************************************************/
const struct olc_cmd_type aedit_table[] =
{
	/*  {   command		function		}, */

	{   "age",		aedit_age		},
	{   "builders",	aedit_builder		},
	{   "commands",	show_commands		},
	{   "create",	aedit_create		},
	{   "filename",	aedit_file		},
	{   "name",		aedit_name		},
	{   "recall",	aedit_recall		},
	{	"reset",	aedit_reset		},
	{   "security",	aedit_security		},
	{	"show",		aedit_show		},
	{   "links",	aedit_links		},
	{   "vnum",		aedit_vnum		},
	{   "lvnum",	aedit_lvnum		},
	{   "uvnum",	aedit_uvnum		},
	{   "sounds",       aedit_sounds            },
	{   "prototype",    aedit_prototype         },
	{   "clan_hq",	aedit_clan_hq		},
	{   "?",		show_help		},
	{   "version",	show_version		},
	{	"llevel",	aedit_llevel		},
	{	"ulevel",	aedit_ulevel		},
	{   "noquest",	aedit_noquest		},
	{   "mudschool",	aedit_mudschool		},
	{   "color",	aedit_color		},

	{	"",		0,			}
};

const struct olc_cmd_type cedit_table[]=
{
	/*  {   command         function                }, */

	{   "commands",     show_commands           },
	{   "create",       cedit_create            },
	{   "clist",        cedit_clist             },
	{   "deity",        cedit_diety             },
	{   "members",      cedit_members           },
	{   "mkills",       cedit_mkills            },
	{   "civil",        cedit_civil		},
	{   "pkill",        cedit_pkill             },
	{   "pkilled",      cedit_pkilled           },
	{   "pkills",       cedit_pkills            },
	{   "name",         cedit_name              },
	{   "object",       cedit_object            },
	{   "recall",       cedit_recall            },
	{   "description",  cedit_desc              },
	{   "power",        cedit_power             },
	{    "induct",      cedit_induct		},
	{   "",             0,                      }
};


const struct olc_cmd_type redit_table[] =
{
	/*  {   command		function		}, */

	{   "commands",	show_commands		},
	{   "create",	redit_create		},
	{   "delet",	redit_delet		},
	{   "delete",	redit_delete		},
	{   "desc",		redit_desc		},
	{   "ed",		redit_ed		},
	{   "format",	redit_format		},
	{   "name",		redit_name		},
	{	"show",		redit_show		},

	{   "north",	redit_north		},
	{   "south",	redit_south		},
	{   "east",		redit_east		},
	{   "west",		redit_west		},
	{   "up",		redit_up		},
	{   "down",		redit_down		},
	{   "walk",		redit_move		},

	/* New reset commands. */
	{	"mreset",	redit_mreset		},
	{	"oreset",	redit_oreset		},
	{   "rreset",	redit_rreset		},
	{	"mlist",	redit_mlist		},
	{	"olist",	redit_olist		},
	{   "rlist",        redit_rlist             },
	{	"mshow",	redit_mshow		},
	{	"oshow",	redit_oshow		},
	{   "proglist",     redit_proglist		},
	{   "rdamage",      redit_rdamage           },
	{   "rpedit",       redit_rpedit            },
	{   "rplist",       redit_rplist            },
	{   "rpremove",     redit_rpremove          },
	{   "epedit",       redit_epedit            },
	{   "eplist",       redit_eplist            },
	{   "epremove",     redit_epremove          },

	{   "?",		show_help		},
	{   "version",	show_version		},

	{	"",		0,			}
};



const struct olc_cmd_type oedit_table[] =
{
	/*  {   command		function		}, */

	{   "addaffect",	oedit_addaffect		},
	{   "commands",	show_commands		},
	{   "cost",		oedit_cost		},
	{   "level",        oedit_level             },
	{   "ac_type",      set_ac_type             },
	{   "ac_vnum",      set_ac_vnum             },
	{   "ac_v1",        set_ac_v1               },
	{   "ac_v2",        set_ac_v2               },
	{   "ac_setspell",  set_ac_setspell         },
	{   "create",	oedit_create		},
	{   "delaffect",	oedit_delaffect		},
	{   "delet",	oedit_delet		},
	{   "delete",	oedit_delete		},
	{   "ed",		oedit_ed		},
	{   "long",		oedit_long		},
	{   "name",		oedit_name		},
	{   "short",	oedit_short		},
	{	"show",		oedit_show		},
	{   "v0",		oedit_value0		},
	{   "v1",		oedit_value1		},
	{   "v2",		oedit_value2		},
	{   "v3",		oedit_value3		},
	{   "weight",	oedit_weight		},
	{   "ojoin",        oedit_join		},
	{   "osepone",      oedit_sepone		},
	{   "oseptwo",      oedit_septwo		},
	{   "opedit",       oedit_opedit            },
	{   "oplist",       oedit_oplist            },
	{   "opremove",     oedit_opremove          },

	{	"mlist",	redit_mlist		},
	{	"olist",	redit_olist		},
	{   "rlist",        redit_rlist             },

	{   "?",		show_help		},
	{   "version",	show_version		},

	{	"",		0,			}
};



const struct olc_cmd_type medit_table[] =
{
	/*  {   command		function		}, */

	{   "alignment",	medit_align		},
	{   "commands",	show_commands		},
	{   "copper",	medit_copper		},
	{   "create",	medit_create		},
	{   "desc",		medit_desc		},
	{   "delet",	medit_delet		},
	{   "delete",	medit_delete		},
	{   "level",	medit_level		},
	{   "hp",           medit_hitpoint          },
	{   "gold",         medit_gold              },
	{   "long",		medit_long		},
	{   "name",		medit_name		},
	{   "shop",		medit_shop		},
	{   "class",	medit_class	        },
	{   "short",	medit_short		},
	{	"show",		medit_show		},
	{   "silver", 	medit_silver		},
	{   "spec",		medit_spec		},
	{   "immune",	medit_immune		},	/* XOR */
	{   "mpedit",       medit_mpedit            },      /* Altrag */
	{   "mplist",       medit_mplist            },      /* Altrag */
	{   "mpremove",     medit_mpremove          },      /* Altrag */

	{	"mlist",	redit_mlist		},
	{	"olist",	redit_olist		},
	{   "rlist",        redit_rlist             },

	{   "?",		show_help		},
	{   "version",	show_version		},

	{	"",		0,			}
};


const struct olc_cmd_type mpedit_table[] =
{
	/*  {   command         function                }, */

	{   "arglist",      mpedit_arglist,         },
	{   "comlist",      mpedit_comlist,         },

	{   "?",            show_help,              },
	{   "version",      show_version,           },
	{   "commands",     show_commands,          },

	{   "",             0,                      }
};

const struct olc_cmd_type tedit_table[] =
{
	/*  {   command         function                }, */

	{   "arglist",     tedit_arglist,           },
	{   "comlist",     tedit_comlist,           },
	/*    {   "disarmable",  tedit_disarmable,        },*/

	{   "?",           show_help,               },
	{   "version",     show_version,            },
	{   "commands",    show_commands,           },

	{   "",            0,                       }
};

const struct olc_cmd_type hedit_table[] =
{
	/*  {   command		function		}, */

	{   "commands",	show_commands		},
	{   "delet",	edit_delet		},
	{   "delete",	hedit_delete		},
	{   "keyword",	hedit_name		},
	{   "name",		hedit_name		},
	{   "level",	hedit_level		},
	{   "show",		hedit_show		},
	{   "desc",		hedit_desc		},
	{   "text",		hedit_desc		},

	{   "?",            show_help               },
	{   "version",      show_version            },

	{   "",		0,			}
};

const struct olc_cmd_type mreset_table[] =
{
	/*  {   command		function		}, */

	/*    {	"add",		mreset_add		},
		  {	"delet",	mreset_delet		},
		  {	"delete",	mreset_delete		},
		  {   "max_in_room",  mreset_max_in_room	}, */

	{	"?",		show_help		},
	{   "commands",	show_commands		},

	{   "",            	0,                      }
};

const struct olc_cmd_type spedit_table[] =
{
	/*  {   command         function                }, */

	{   "commands",     show_commands           },
	{   "1",         	spedit_damage_msg       },
	{   "2",        	spedit_spell_ends       },
	{   "3",         	spedit_spell_ends_room  },
	{   "dispelable",   spedit_dispelable       },
	{   "min",         	spedit_min              },
	{   "mana",         spedit_mana             },
	{   "name",       	spedit_name            	},
	{   "show",         spedit_show             },
	{   "wait",      	spedit_wait             },

	{   "",             0,                      }
};

const struct olc_cmd_type sedit_table[] =
{
	/*  {   command         function                }, */

	{   "keyword",	sedit_name		},

	{ 	"char_no_arg",	sedit_char_no_arg	},
	{ 	"others_no_arg",sedit_others_no_arg	},
	{ 	"char_found",	sedit_char_found	},
	{   "others_found",	sedit_others_found	},
	{   "vict_found",	sedit_vict_found	},
	{	"char_auto",	sedit_char_auto		},
	{	"others_auto",	sedit_others_auto	},

	{   "1",  		sedit_char_no_arg       },
	{   "2",		sedit_others_no_arg     },
	{   "3",   		sedit_char_found        },
	{   "4", 		sedit_others_found      },
	{   "5",   		sedit_vict_found        },
	{   "6",   	 	sedit_char_auto         },
	{   "7",  		sedit_others_auto       },


	{   "delet",	edit_delet		},
	{   "delete",	sedit_delete		},


	{	"?",		show_help		},
	{   "commands",	show_commands		},

	{   "",            	0,                      }
};

const struct olc_cmd_type nedit_table[] =
{
	/* { command		function		}, */

	{ "keyword",		nedit_keyword		},
	{ "answer1",		nedit_answer1		},
	{ "answer2",		nedit_answer2		},
	// TODO add delete

	{ "1",		nedit_answer1		},
	{ "2", 		nedit_answer2		},

	{ "?",		show_help		},
	{ "commands",	show_commands		},

	{ "",		0,			}
};

const struct olc_cmd_type rename_obj_table[] =
{
	/*  {   command         function                }, */

	{   "keyword",	rename_keyword		},
	{	"short",	rename_short		},
	{	"long",		rename_long		},

	{	"1",		rename_keyword		},
	{	"2",		rename_short		},
	{ 	"3",		rename_long		},

	{   "commands",	show_commands		},

	{   "",            	0,                      }
};
const struct olc_cmd_type forge_obj_table[] =
{

	{   "keyword",	rename_keyword,		},
	{	"short",	rename_short,		},
	{	"long",		rename_long,		},
	{	"type", 	forge_type		},

	{	"1",		forge_addaffect,	},
	{	"2",		forge_addaffect,	},
	{	"3",		forge_addaffect,	},
	{	"4",		forge_addaffect,	},
	{	"5",		forge_addaffect,	},
	{	"6",		forge_addaffect,	},
	{	"7",		forge_addaffect,	},
	{	"8",		forge_addaffect,	},
	{	"9",		forge_addaffect,	},

	{   "commands",	show_commands,		},

	{   "",            	0,                      }
};

/*****************************************************************************
 *                          End Interpreter Tables.                          *
 *****************************************************************************/



/*****************************************************************************
Name:		get_area_data
Purpose:	Returns pointer to area with given vnum.
Called by:	do_aedit(olc.c).
 ****************************************************************************/
AREA_DATA *get_area_data( int vnum )
{
	AREA_DATA *pArea;

	for (pArea = area_first; pArea; pArea = pArea->next )
	{
		if (pArea->vnum == vnum)
			return pArea;
	}

	return 0;
}

/*
 * Get data for a MobProg -- Altrag
 */
MPROG_DATA *get_mprog_data( MOB_INDEX_DATA *pMob, int vnum )
{
	int value = 0;
	MPROG_DATA *pMProg;

	for ( pMProg = pMob->mobprogs; pMProg; pMProg = pMProg->next, value++ )
	{
		if ( value == vnum )
			return pMProg;
	}
	return NULL;
}

TRAP_DATA *get_trap_data( void *vo, int vnum, int type )
{
	int value = 0;
	OBJ_INDEX_DATA *obj = (OBJ_INDEX_DATA *)vo;
	ROOM_INDEX_DATA *room = (ROOM_INDEX_DATA *)vo;
	EXIT_DATA *pExit = (EXIT_DATA *)vo;
	TRAP_DATA *pTrap;

	switch(type)
	{
		case ED_OPROG:
			for (pTrap = obj->traps; pTrap; pTrap = pTrap->next_here, value++)
				if ( value == vnum )
					return pTrap;
			return NULL;
		case ED_RPROG:
			for (pTrap = room->traps; pTrap; pTrap = pTrap->next_here, value++)
				if ( value == vnum )
					return pTrap;
			return NULL;
		case ED_EPROG:
			for (pTrap = pExit->traps; pTrap; pTrap = pTrap->next_here, value++)
				if ( value == vnum )
					return pTrap;
			return NULL;
		default:
			bug( "Get_trap_index: Invalid type %d", type );
			return NULL;
	}
	return NULL;
}

/*****************************************************************************
Name:		edit_done
Purpose:	Resets builder information on completion.
Called by:	aedit, redit, oedit, medit(olc.c), mpedit(Altrag)
 ****************************************************************************/
bool edit_done( CHAR_DATA *ch )
{
	/*
	 * Well, since i have the inEdit for mpedit, why not make it usable for
	 * all nested editors..?
	 * -- Altrag
	 */
	if ( ch->desc->editin || ch->desc->inEdit )
	{
		ch->desc->pEdit = ch->desc->inEdit;
		ch->desc->inEdit = NULL;
		ch->desc->editor = ch->desc->editin;
		ch->desc->editin = 0;
		return FALSE;
	}
	if ( ch->desc->editor == FORGE_OBJECT )
		forge_pay_cost( ch );
	ch->desc->pEdit = NULL;
	ch->desc->editor = 0;
	return FALSE;
}



/*****************************************************************************
 *                              Interpreters.                                *
 *****************************************************************************/


/* Area Interpreter, called by do_aedit. */
void aedit( CHAR_DATA *ch, char *argument )
{
	AREA_DATA *pArea;
	char command[MAX_INPUT_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	int  cmd;
	int  value;

	EDIT_AREA(ch, pArea);
	smash_tilde( argument );
	strcpy( arg, argument );
	argument = one_argument( argument, command );

	if ( !IS_BUILDER( ch, pArea ) ) /*  && ch->level < L_CON ) */
		send_to_char(C_DEFAULT, "AEdit:  Insufficient security to modify area.\n\r", ch );
	/*    else if ( IS_SWITCHED( ch ) )
		  send_to_char( C_DEFAULT, "AEdit: Cannot edit while switched.\n\r",
		  ch ); */

	if ( command[0] == '\0' )
	{
		aedit_show( ch, argument );
		return;
	}

	if ( !str_cmp(command, "done") )
	{
		edit_done( ch );
		return;
	}

	if ( !IS_BUILDER( ch, pArea ) ) /* && ch->level < L_CON ) ||
									   IS_SWITCHED( ch ) ) */
	{
		interpret( ch, arg );
		return;
	}

	/* Search Table and Dispatch Command. */
	for ( cmd = 0; *aedit_table[cmd].name; cmd++ )
	{
		if ( !str_prefix( command, aedit_table[cmd].name ) )
		{
			if ( (*aedit_table[cmd].olc_fun) ( ch, argument ) )
				SET_BIT( pArea->area_flags, AREA_CHANGED );
			return;
		}
	}

	/* Take care of flags. */
	if ( ( value = flag_value( area_flags, arg ) ) != NO_FLAG )
	{
		TOGGLE_BIT(pArea->area_flags, value);

		send_to_char(C_DEFAULT, "Flag toggled.\n\r", ch );
		return;
	}

	/* Default to Standard Interpreter. */
	interpret( ch, arg );
	return;
}



/* Room Interpreter, called by do_redit. */
void redit( CHAR_DATA *ch, char *argument )
{
	ROOM_INDEX_DATA *pRoom;
	AREA_DATA *pArea;
	char arg[MAX_STRING_LENGTH];
	char command[MAX_INPUT_LENGTH];
	int  cmd;
	int  value;

	EDIT_ROOM(ch, pRoom);
	pArea = pRoom->area;

	smash_tilde( argument );
	strcpy( arg, argument );
	argument = one_argument( argument, command );

	if ( !IS_BUILDER( ch, pArea ) ) /* && ch->level < L_CON ) */
	{
		send_to_char(C_DEFAULT, "REdit:  Insufficient security to modify room.\n\r", ch );
	}
	/*    else if ( IS_SWITCHED( ch ) )
		  send_to_char( C_DEFAULT, "REdit: Cannot edit while switched.\n\r",
		  ch ); */

	if ( command[0] == '\0' )
	{
		redit_show( ch, argument );
		return;
	}

	if ( !str_cmp(command, "done") )
	{
		edit_done( ch );
		return;
	}

	if ( !IS_BUILDER( ch, pArea ) ) /* && ch->level < L_CON ) ||
									   IS_SWITCHED( ch ) ) */
	{
		interpret( ch, arg );
		return;
	}

	/* Search Table and Dispatch Command. */
	for ( cmd = 0; *redit_table[cmd].name; cmd++ )
	{
		if ( !str_prefix( command, redit_table[cmd].name ) )
		{
			if ( (*redit_table[cmd].olc_fun) ( ch, argument ) )
				SET_BIT( pArea->area_flags, AREA_CHANGED );
			return;
		}
	}

	/* Take care of flags. */
	if ( ( value = flag_value( room_flags, arg ) ) != NO_FLAG )
	{
		TOGGLE_BIT(pRoom->room_flags, value);

		SET_BIT( pArea->area_flags, AREA_CHANGED );
		send_to_char(C_DEFAULT, "Room flag toggled.\n\r", ch );
		return;
	}

	if ( ( value = flag_value( sector_flags, arg ) ) != NO_FLAG )
	{
		pRoom->sector_type  = value;

		SET_BIT( pArea->area_flags, AREA_CHANGED );
		send_to_char(C_DEFAULT, "Sector type set.\n\r", ch );
		return;
	}

	/* Default to Standard Interpreter. */
	interpret( ch, arg );
	return;
}

void cedit( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_STRING_LENGTH];
	char command[MAX_INPUT_LENGTH];
	int  cmd;

	smash_tilde( argument );
	strcpy( arg, argument );
	argument = one_argument( argument, command );

	if ( !str_cmp( "done", command ) )
	{
		edit_done( ch );
		return;
	}

	if ( get_trust(ch) < L_CON )
	{
		send_to_char(C_DEFAULT, "You may not edit clans.\n\r", ch );
		return;
	}

	if ( command[0] == '\0' )
	{
		cedit_show( ch, argument );
		return;
	}

	/* Search Table and Dispatch Command. */
	for ( cmd = 0; *cedit_table[cmd].name; cmd++ )
	{
		if ( !str_prefix( command, cedit_table[cmd].name ) )
		{
			(*cedit_table[cmd].olc_fun) ( ch, argument );
			return;
		}
	}

	/* Default to Standard Interpreter. */
	interpret( ch, arg );
	return;
}



/* Object Interpreter, called by do_oedit. */
void oedit( CHAR_DATA *ch, char *argument )
{
	AREA_DATA *pArea;
	OBJ_INDEX_DATA *pObj;
	char arg[MAX_STRING_LENGTH];
	char command[MAX_INPUT_LENGTH];
	int  cmd;
	int  value;

	smash_tilde( argument );
	strcpy( arg, argument );
	argument = one_argument( argument, command );

	EDIT_OBJ(ch, pObj);
	pArea = pObj->area;

	if ( !IS_BUILDER( ch, pArea ) ) /*  && ch->level < L_CON ) */
		send_to_char(C_DEFAULT, "OEdit: Insufficient security to modify area.\n\r", ch );
	/*    else if ( IS_SWITCHED( ch ) )
		  send_to_char( C_DEFAULT, "OEdit: Cannot edit while switched.\n\r",
		  ch ); */

	if ( command[0] == '\0' )
	{
		oedit_show( ch, argument );
		return;
	}

	if ( !str_cmp(command, "done") )
	{
		edit_done( ch );
		return;
	}

	if ( !IS_BUILDER( ch, pArea ) ) /* && ch->level < L_CON ) ||
									   IS_SWITCHED( ch ) ) */
	{
		interpret( ch, arg );
		return;
	}

	/* Search Table and Dispatch Command. */
	for ( cmd = 0; *oedit_table[cmd].name; cmd++ )
	{
		if ( !str_prefix( command, oedit_table[cmd].name ) )
		{
			if ( (*oedit_table[cmd].olc_fun) ( ch, argument ) )
				SET_BIT( pArea->area_flags, AREA_CHANGED );
			return;
		}
	}

	/* Take care of flags. */
	if ( ( value = flag_value( type_flags, arg ) ) != NO_FLAG )
	{
		pObj->item_type = value;

		SET_BIT( pArea->area_flags, AREA_CHANGED );
		send_to_char(C_DEFAULT, "Type set.\n\r", ch);

		/*
		 * Clear the values.
		 */
		pObj->value[0] = 0;
		pObj->value[1] = 0;
		pObj->value[2] = 0;
		pObj->value[3] = 0;

		return;
	}

	if ( ( value = flag_value( extra_flags, arg ) ) != NO_FLAG )
	{
		TOGGLE_BIT(pObj->extra_flags, value);

		SET_BIT( pArea->area_flags, AREA_CHANGED );
		send_to_char(C_DEFAULT, "Extra flag toggled.\n\r", ch);
		return;
	}

	if ( ( value = flag_value( wear_flags, arg ) ) != NO_FLAG )
	{
		TOGGLE_BIT(pObj->wear_flags, value);

		SET_BIT( pArea->area_flags, AREA_CHANGED );
		send_to_char(C_DEFAULT, "Wear flag toggled.\n\r", ch);
		return;
	}
	/* FOR NEW FLAGS */

	if ( ( value = flag_value( anti_class_flags, arg ) ) != NO_FLAG )
	{
		TOGGLE_BIT(pObj->anti_class_flags, value);

		SET_BIT( pArea->area_flags, AREA_CHANGED );
		send_to_char(C_DEFAULT, "Anti class flag toggled.\n\r", ch);
		return;
	}
	if ( ( value = flag_value( anti_race_flags, arg ) ) != NO_FLAG )
	{
		TOGGLE_BIT(pObj->anti_race_flags, value);

		SET_BIT( pArea->area_flags, AREA_CHANGED );
		send_to_char(C_DEFAULT, "Anti race flag toggled.\n\r", ch);
		return;
	}

	/* Default to Standard Interpreter. */
	interpret( ch, arg );
	return;
}



/* Mobile Interpreter, called by do_medit. */
void medit( CHAR_DATA *ch, char *argument )
{
	AREA_DATA *pArea;
	MOB_INDEX_DATA *pMob;
	char command[MAX_INPUT_LENGTH];
	char arg[MAX_STRING_LENGTH];
	int  cmd;
	int  value;


	smash_tilde( argument );
	strcpy( arg, argument );
	argument = one_argument( argument, command );

	EDIT_MOB(ch, pMob);
	pArea = pMob->area;

	if ( !IS_BUILDER( ch, pArea ) ) /*  && ch->level < L_CON )  */
		send_to_char(C_DEFAULT, "MEdit: Insufficient security to modify area.\n\r", ch );
	/*    else if ( IS_SWITCHED( ch ) )
		  send_to_char( C_DEFAULT, "MEdit: Cannot edit while switched.\n\r",
		  ch ); */

	if ( command[0] == '\0' )
	{
		medit_show( ch, argument );
		return;
	}

	if ( !str_cmp(command, "done") )
	{
		edit_done( ch );
		return;
	}

	if ( !IS_BUILDER( ch,  pArea ) ) /* && ch->level < L_CON ) ||
										IS_SWITCHED( ch ) ) */
	{
		interpret( ch, arg );
		return;
	}

	/* Search Table and Dispatch Command. */
	for ( cmd = 0; *medit_table[cmd].name; cmd++ )
	{
		if ( !str_prefix( command, medit_table[cmd].name ) )
		{
			if ( (*medit_table[cmd].olc_fun) ( ch, argument ) )
				SET_BIT( pArea->area_flags, AREA_CHANGED );
			return;
		}
	}

	/* Take care of flags. */
	if ( ( value = flag_value( sex_flags, arg ) ) != NO_FLAG )
	{
		pMob->sex = value;

		SET_BIT( pArea->area_flags, AREA_CHANGED );
		send_to_char(C_DEFAULT, "Sex set.\n\r", ch);
		return;
	}


	if ( ( value = flag_value( act_flags, arg ) ) != NO_FLAG )
	{
		TOGGLE_BIT(pMob->act, value);

		SET_BIT( pArea->area_flags, AREA_CHANGED );
		send_to_char(C_DEFAULT, "Act flag toggled.\n\r", ch);
		return;
	}


	if ( ( value = flag_value( affect_flags, arg ) ) != NO_FLAG )
	{
		TOGGLE_BIT(pMob->affected_by, value);

		SET_BIT( pArea->area_flags, AREA_CHANGED );
		send_to_char(C_DEFAULT, "Affect flag toggled.\n\r", ch);
		return;
	}

	if ( ( value = flag_value( affect2_flags, arg ) ) != NO_FLAG
			&& value != AFF_GOLDEN )
	{
		TOGGLE_BIT(pMob->affected_by2, value);

		SET_BIT( pArea->area_flags, AREA_CHANGED );
		send_to_char(C_DEFAULT, "Affect flag toggled.\n\r", ch);
		return;
	}

	/* Default to Standard Interpreter. */
	interpret( ch, arg );
	return;
}


/*
 * Editor for MobPrograms.
 * -- Altrag
 */
void mpedit( CHAR_DATA *ch, char *argument )
{
	AREA_DATA *pArea;
	MPROG_DATA *pMProg;
	char command[MAX_INPUT_LENGTH];
	char arg[MAX_STRING_LENGTH];
	int  cmd;
	int  value;

	smash_tilde( argument );
	strcpy( arg, argument );
	argument = one_argument( argument, command );

	EDIT_MPROG(ch, pMProg);
	{
		MOB_INDEX_DATA *pMob;

		pMob = (MOB_INDEX_DATA *)ch->desc->inEdit;
		pArea = pMob->area;
	}

	if ( command[0] == '\0' )
	{
		mpedit_show( ch, "" );
		return;
	}

	if ( !str_cmp(command, "done") )
	{
		edit_done( ch );
		return;
	}

	if ( !IS_BUILDER( ch, pArea ) ) /*  && ch->level < L_CON ) ||
										IS_SWITCHED( ch ) ) */
	{
		interpret( ch, arg );
		return;
	}

	/* Search Table and Dispatch Command. */
	for ( cmd = 0; *mpedit_table[cmd].name; cmd++ )
	{
		if ( !str_prefix( command, mpedit_table[cmd].name ) )
		{
			if ( (*mpedit_table[cmd].olc_fun) ( ch, argument ) )
				SET_BIT( pArea->area_flags, AREA_CHANGED );
			return;
		}
	}

	if ( ( value = flag_value( mprog_types, arg ) ) != NO_FLAG )
	{
		MOB_INDEX_DATA *pMob;
		MPROG_DATA *prog;

		pMProg->type = value;
		pMob = (MOB_INDEX_DATA *)ch->desc->inEdit;
		/*
		 * Full list search again in case theres more than one of the type
		 * -- Altrag
		 */
		pMob->progtypes = 0;
		for ( prog = pMob->mobprogs; prog; prog = prog->next )
			SET_BIT( pMob->progtypes, prog->type );
		SET_BIT( pArea->area_flags, AREA_CHANGED );
		send_to_char(C_DEFAULT, "MobProg type set.\n\r", ch);
		return;
	}

	/* Default to Standard Interpreter. */
	interpret( ch, arg );
	return;
}
/*
 * Very klutzy global trap editor.. Main cause is that exits are handled
 * differently from the other items with traps.
 * -- Altrag
 */
void tedit( CHAR_DATA *ch, char *argument )
{
	AREA_DATA *pArea;
	TRAP_DATA *pTrap;
	EXIT_DATA *pExit = NULL;
	char command[MAX_INPUT_LENGTH];
	char arg[MAX_STRING_LENGTH];
	int cmd;
	int value;

	switch (ch->desc->editor)
	{
		case ED_OPROG:
			pArea = ((OBJ_INDEX_DATA *)ch->desc->inEdit)->area;
			break;
		case ED_RPROG:
			{
				TRAP_DATA *trap;

				for ( trap = ch->in_room->traps; trap; trap = trap->next_here )
					if ( trap == (TRAP_DATA *)ch->desc->pEdit )
						break;
				if ( !trap )
				{
					send_to_char(C_DEFAULT, "Room changed.  Returning to REditor.\n\r",ch);
					edit_done( ch );
					/*	redit( ch, arg );*/
					return;
				}
			}
			pArea = ch->in_room->area;
			break;
		case ED_EPROG:
			{
				int dir;

				for ( dir = 0; dir < 6; dir++ )
					if ( (pExit = ch->in_room->exit[dir]) )
					{
						TRAP_DATA *trap = (TRAP_DATA *)ch->desc->pEdit;

						/*	  for ( trap = pExit->traps; trap; trap = trap->next_here )
							  if ( trap == (TRAP_DATA *)ch->desc->pEdit )
							  break;
							  if ( trap )
							  break;*/
						if ( trap->on_exit == pExit )
							break;
					}
				if ( dir == 6 )
				{
					send_to_char(C_DEFAULT, "Room changed.  Returning to REditor.\n\r",ch);
					edit_done( ch );
					/*	redit(ch, arg);*/
					return;
				}
				pArea = ch->in_room->area;
				break;
			}
		default:
			bug("Tedit: Invalid editor type %d", ch->desc->editor);
			return;
	}

	smash_tilde(argument);
	strcpy(arg, argument);
	argument = one_argument(argument, command);

	EDIT_TRAP(ch, pTrap);

	if ( command[0] == '\0' )
	{
		tedit_show( ch, "" );
		return;
	}

	if ( !str_cmp(command, "done") )
	{
		edit_done( ch );
		return;
	}

	if ( !IS_BUILDER( ch, pArea ) ) /*  && ch->level < L_CON ) ||
										IS_SWITCHED( ch ) ) */
	{
		interpret( ch, arg );
		return;
	}

	/* Search table and dispatch command. */
	for ( cmd = 0; *tedit_table[cmd].name; cmd++ )
	{
		if ( !str_prefix( command, tedit_table[cmd].name ) )
		{
			if ( (*tedit_table[cmd].olc_fun) ( ch, argument ) )
				SET_BIT( pArea->area_flags, AREA_CHANGED );
			return;
		}
	}

	switch (ch->desc->editor)
	{
		TRAP_DATA *trap;

		case ED_OPROG:
		if ( ( value = flag_value( oprog_types, arg ) ) != NO_FLAG )
		{
			OBJ_INDEX_DATA *obj = (OBJ_INDEX_DATA *)ch->desc->inEdit;

			pTrap->type = value;
			obj->traptypes = 0;

			for ( trap = obj->traps; trap; trap = trap->next_here )
				SET_BIT( obj->traptypes, trap->type );
			SET_BIT( pArea->area_flags, AREA_CHANGED );
			send_to_char(C_DEFAULT, "ObjProg type set.\n\r", ch );
			return;
		}
		break;
		case ED_RPROG:
		if ( ( value = flag_value( rprog_types, arg ) ) != NO_FLAG )
		{
			ROOM_INDEX_DATA *room = ch->in_room;

			pTrap->type = value;
			room->traptypes = 0;

			for ( trap = room->traps; trap; trap = trap->next_here )
				SET_BIT( room->traptypes, trap->type );
			SET_BIT( pArea->area_flags, AREA_CHANGED );
			send_to_char( C_DEFAULT, "RoomProg type set.\n\r",ch);
			return;
		}
		break;
		case ED_EPROG:
		if ( !pExit )
		{
			send_to_char(C_DEFAULT, "Exit not found.  Returning to REditor.\n\r",ch);
			bug("Tedit: NULL pExit in %d", ch->in_room->vnum);
			edit_done( ch );
			redit( ch, arg );
			return;
		}

		if ( ( value = flag_value( eprog_types, arg ) ) != NO_FLAG )
		{
			TRAP_DATA *trap;

			pTrap->type = value;
			pExit->traptypes = 0;

			for ( trap = pExit->traps; trap; trap = trap->next_here )
				SET_BIT( pExit->traptypes, trap->type );
			SET_BIT( pArea->area_flags, AREA_CHANGED );
			send_to_char( C_DEFAULT, "ExitProg type set.\n\r",ch);
			return;
		}
		break;
		default:
		bug("Tedit: Invalid editor %d", ch->desc->editor);
		break;
	}

	/* Default to Standard Interpreter. */
	interpret( ch, arg );
	return;
}


void purge_area( AREA_DATA *pArea )
{
	ROOM_INDEX_DATA *pRoom;
	int  vnum;
	CHAR_DATA *victim;
	CHAR_DATA *vnext;
	OBJ_DATA *obj_next;
	OBJ_DATA *obj;

	for ( vnum = pArea->lvnum; vnum <= pArea->uvnum; vnum++ )
	{
		if ( ( pRoom = get_room_index(vnum) ) )
		{
			for ( victim = pRoom->people; victim; victim = vnext )
			{
				vnext = victim->next_in_room;
				if ( victim->deleted ) continue;
				if ( IS_NPC( victim ) )
					extract_char( victim, TRUE );
			}
			for (obj = pRoom->contents; obj; obj = obj_next )
			{
				obj_next = obj->next_content;
				if ( obj->deleted )
					continue;
				extract_obj( obj );
			}
		}
	}
	return;

}
void do_aedit( CHAR_DATA *ch, char *argument )
{
	AREA_DATA *pArea;
	char command[MAX_INPUT_LENGTH];

	argument = one_argument( argument, command );
	pArea = ch->in_room->area;

	if ( command[0] == 'r' && !str_prefix( command, "reset" ) )
	{
		if ( ch->desc->editor == ED_AREA )
			reset_area( (AREA_DATA *)ch->desc->pEdit );
		else
			reset_area( pArea );
		send_to_char(C_DEFAULT, "Area reset.\n\r", ch );
		return;
	}

	if ( command[0] == 'p' && !str_prefix( command, "purge" ) )
	{
		if ( ch->desc->editor == ED_AREA )
			purge_area( (AREA_DATA *)ch->desc->pEdit );
		else
			purge_area( pArea );
		send_to_char(C_DEFAULT, "Area purged.\n\r", ch );
		return;
	}

	if ( command[0] == 'c' && !str_prefix( command, "create" ) )
	{
		if ( aedit_create( ch, argument ) )
		{
			ch->desc->editor = ED_AREA;
			pArea = (AREA_DATA *)ch->desc->pEdit;
			SET_BIT( pArea->area_flags, AREA_CHANGED );
			aedit_show( ch, "" );
		}
		return;
	}

	if ( is_number( command ) )
	{
		if ( !( pArea = get_area_data( atoi(command) ) ) )
		{
			send_to_char(C_DEFAULT, "No such area vnum exists.\n\r", ch );
			return;
		}
	}

	/*
	 * Builder defaults to editing current area.
	 */
	ch->desc->pEdit = (void *)pArea;
	ch->desc->editor = ED_AREA;
	aedit_show( ch, "" );
	return;
}



/* Entry point for editing room_index_data. */
void do_redit( CHAR_DATA *ch, char *argument )
{
	ROOM_INDEX_DATA *pRoom;
	char command[MAX_INPUT_LENGTH];
	int players = 0;

	argument = one_argument( argument, command );
	pRoom = ch->in_room;

	if ( command[0] == 'r' && !str_prefix( command, "reset" ) )
	{
		players = pRoom->area->nplayer;
		pRoom->area->nplayer = 0;
		/* When doing redit reset, make amt of players in area 0
		   so objects reset on the ground --Angi */
		reset_room( pRoom );
		send_to_char(C_DEFAULT, "Room reset.\n\r", ch );
		/* Set amt of players in area back to original number */
		pRoom->area->nplayer = players;
		return;
	}

	if ( command[0] == 'c' && !str_prefix( command, "create" ) )
	{
		if ( redit_create( ch, argument ) )
		{
			char_from_room( ch );
			char_to_room( ch, ch->desc->pEdit );
			SET_BIT( pRoom->area->area_flags, AREA_CHANGED );
		}
	}

	/*
	 * Builder defaults to editing current room.
	 */
	ch->desc->editor = ED_ROOM;
	redit_show( ch, "" );
	return;
}



/* Entry point for editing obj_index_data. */
void do_oedit( CHAR_DATA *ch, char *argument )
{
	OBJ_INDEX_DATA *pObj;
	AREA_DATA *pArea;
	char command[MAX_INPUT_LENGTH];

	argument = one_argument( argument, command );

	if ( is_number( command ) )
	{
		if ( !( pObj = get_obj_index( atoi( command ) ) ) )
		{
			send_to_char(C_DEFAULT, "OEdit:  That vnum does not exist.\n\r", ch );
			return;
		}

		ch->desc->pEdit = (void *)pObj;
		ch->desc->editor = ED_OBJECT;
		oedit_show( ch, "" );
		return;
	}

	if ( command[0] == 'c' && !str_prefix( command, "create" ) )
	{
		if ( oedit_create( ch, argument ) )
		{
			if ( argument[0] == '\0')
				pArea = ch->in_room->area;
			else
				pArea = get_vnum_area( atoi( argument ) );
			SET_BIT( pArea->area_flags, AREA_CHANGED );
			ch->desc->editor = ED_OBJECT;
			oedit_show( ch, "" );
		}
		return;
	}

	send_to_char(C_DEFAULT, "OEdit:  There is no default object to edit.\n\r", ch );
	return;
}



/* Entry point for editing mob_index_data. */
void do_medit( CHAR_DATA *ch, char *argument )
{
	MOB_INDEX_DATA *pMob;
	AREA_DATA *pArea;
	char command[MAX_INPUT_LENGTH];

	argument = one_argument( argument, command );

	if ( is_number( command ) )
	{
		if ( !( pMob = get_mob_index( atoi( command ) ) ))
		{
			send_to_char(C_DEFAULT, "MEdit:  That vnum does not exist.\n\r", ch );
			return;
		}

		ch->desc->pEdit = (void *)pMob;
		ch->desc->editor = ED_MOBILE;
		medit_show( ch, "" );
		return;
	}

	if ( command[0] == 'c' && !str_prefix( command, "create" ) )
	{
		if ( medit_create( ch, argument ) )
		{
			if ( argument[0] == '\0')
				pArea = ch->in_room->area;
			else
				pArea = get_vnum_area( atoi( argument ) );
			SET_BIT( pArea->area_flags, AREA_CHANGED );
			ch->desc->editor = ED_MOBILE;
			medit_show( ch, "" );
		}
		return;
	}

	send_to_char(C_DEFAULT, "MEdit:  There is no default mobile to edit.\n\r", ch );
	return;
}

void do_cedit( CHAR_DATA *ch, char *argument )
{
	CLAN_DATA *pClan;
	char command[MAX_INPUT_LENGTH];

	argument = one_argument( argument, command );

	if ( is_number( command ) )
	{
		if ( !( pClan = get_clan_index( atoi( command ) ) ))
		{
			send_to_char(C_DEFAULT, "CEdit:  That clan does not exist.\n\r", ch );
			return;
		}

		ch->desc->pEdit = (void *)pClan;
		ch->desc->editor = ED_CLAN;
		cedit_show( ch, "" );
		return;
	}

	if ( command[0] == 'c' && !str_prefix( command, "create" ) )
	{
		if ( cedit_create( ch, argument ) )
		{
			ch->desc->editor = ED_CLAN;
			cedit_show( ch, "" );
		}
		return;
	}

	send_to_char(C_DEFAULT, "CEdit:  There is no default clan to edit.\n\r", ch );
	return;
}

/*
 * The last part needed in this file.. :)
 * -- Altrag
 */
bool medit_mpedit( CHAR_DATA *ch, char *argument )
{
	MPROG_DATA *pMProg;
	char command[MAX_INPUT_LENGTH];

	argument = one_argument( argument, command );

	if ( is_number( command ) )
	{
		if ( !( pMProg = get_mprog_data( (MOB_INDEX_DATA *)ch->desc->pEdit,
						atoi( command ) ) ) )
		{
			send_to_char( C_DEFAULT, "MPEdit:  Mobile has no such MobProg.\n\r", ch );
			return FALSE;
		}
		ch->desc->editin = ch->desc->editor;
		ch->desc->editor = ED_MPROG;
		ch->desc->inEdit = ch->desc->pEdit;
		ch->desc->pEdit  = (void *)pMProg;
		mpedit_show( ch, "" );
		return TRUE;
	}

	if ( command[0] == 'c' && !str_prefix( command, "create" ) )
	{
		if ( mpedit_create( ch, argument ) )
		{
			ch->desc->editin = ch->desc->editor;
			ch->desc->editor = ED_MPROG;

			mpedit_show( ch, "" );
			return TRUE;
		}
		return FALSE;
	}

	send_to_char( C_DEFAULT, "MPEdit:  There is no default MobProg to edit.\n\r", ch );
	return FALSE;
}

bool oedit_opedit( CHAR_DATA *ch, char *argument )
{
	TRAP_DATA *pTrap;
	char command[MAX_INPUT_LENGTH];

	argument = one_argument( argument, command );

	if ( is_number( command ) )
	{
		if ( !( pTrap = get_trap_data( ch->desc->pEdit, atoi(command),
						ED_OPROG ) ) )
		{
			send_to_char( C_DEFAULT, "OPEdit:  Object has no such ObjProg.\n\r", ch );
			return FALSE;
		}
		ch->desc->editin = ch->desc->editor;
		ch->desc->editor = ED_OPROG;
		ch->desc->inEdit = ch->desc->pEdit;
		ch->desc->pEdit  = (void *)pTrap;
		tedit_show( ch, "" );
		return TRUE;
	}

	if ( command[0] == 'c' && !str_prefix( command, "create" ) )
	{
		ch->desc->editin = ch->desc->editor;
		ch->desc->editor = ED_OPROG;
		if ( tedit_create( ch, argument ) )
		{
			tedit_show( ch, "" );
			return TRUE;
		}
		ch->desc->editor = ch->desc->editin;
		ch->desc->editin = 0;
		return FALSE;
	}

	send_to_char( C_DEFAULT, "OPEdit:  There is no default ObjProg to edit.\n\r", ch );
	return FALSE;
}

bool redit_rpedit( CHAR_DATA *ch, char *argument )
{
	TRAP_DATA *pTrap;
	char command[MAX_INPUT_LENGTH];

	argument = one_argument( argument, command );

	if ( is_number( command ) )
	{
		if ( !( pTrap = get_trap_data( ch->in_room, atoi(command), ED_RPROG ) ) )
		{
			send_to_char(C_DEFAULT, "RPEdit:  Room has no such RoomProg.\n\r",ch);
			return FALSE;
		}
		ch->desc->editin = ch->desc->editor;
		ch->desc->editor = ED_RPROG;
		ch->desc->pEdit  = (void *)pTrap;
		tedit_show( ch, argument );
		return TRUE;
	}

	if ( command[0] == 'c' && !str_prefix( command, "create" ) )
	{
		ch->desc->editin = ch->desc->editor;
		ch->desc->editor = ED_RPROG;
		if ( tedit_create( ch, argument ) )
		{
			tedit_show( ch, argument );
			return TRUE;
		}
		ch->desc->editor = ch->desc->editin;
		ch->desc->editin = 0;
		return FALSE;
	}

	send_to_char(C_DEFAULT, "RPEdit:  There is no default RoomProg to edit.\n\r",ch);
	return FALSE;
}

bool redit_epedit( CHAR_DATA *ch, char *argument )
{
	TRAP_DATA *pTrap;
	char command[MAX_INPUT_LENGTH];
	int dir;

	argument = one_argument( argument, command );
	for ( dir = 0; dir < 6; dir++ )
		if ( !str_prefix( command, dir_name[dir] ) && ch->in_room->exit[dir] )
			break;
	if ( dir == 6 )
	{
		send_to_char(C_DEFAULT, "EPEdit:  Room has no such exit.\n\r", ch);
		return FALSE;
	}

	argument = one_argument( argument, command );

	if ( is_number( command ) )
	{
		if ( !( pTrap = get_trap_data( ch->in_room->exit[dir], atoi(command),
						ED_EPROG ) ) )
		{
			send_to_char(C_DEFAULT, "EPEdit:  Exit has no such ExitProg.\n\r", ch);
			return FALSE;
		}
		ch->desc->editin = ch->desc->editor;
		ch->desc->editor = ED_EPROG;
		ch->desc->pEdit  = pTrap;
		tedit_show( ch, "" );
		return TRUE;
	}

	if ( command[0] == 'c' && !str_prefix( command, "create" ) )
	{
		char buf[MAX_INPUT_LENGTH];

		ch->desc->editin = ch->desc->editor;
		ch->desc->editor = ED_EPROG;
		strcpy( buf, dir_name[dir] );
		if ( tedit_create( ch, buf ) )
		{
			tedit_show( ch, "" );
			return TRUE;
		}
		ch->desc->editor = ch->desc->editin;
		ch->desc->editin = 0;
		return FALSE;
	}

	send_to_char(C_DEFAULT, "EPEdit:  There is no default ExitProg to edit.\n\r",ch);
	return FALSE;
}

void display_resets( CHAR_DATA *ch )
{
	ROOM_INDEX_DATA	*pRoom;
	RESET_DATA		*pReset;
	MOB_INDEX_DATA	*pMob = NULL;
	char 		buf   [ MAX_STRING_LENGTH ];
	char 		final [ MAX_STRING_LENGTH ];
	int 		iReset = 0;

	EDIT_ROOM(ch, pRoom);
	final[0]  = '\0';

	send_to_char ( C_DEFAULT,
			" No.  Loads    Description       Location         Vnum    Max  Description"
			"\n\r"
			"==== ======== ============= =================== ======== ===== ==========="
			"\n\r", ch );

	for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
	{
		OBJ_INDEX_DATA  *pObj;
		MOB_INDEX_DATA  *pMobIndex;
		OBJ_INDEX_DATA  *pObjIndex;
		OBJ_INDEX_DATA  *pObjToIndex;
		ROOM_INDEX_DATA *pRoomIndex;

		final[0] = '\0';
		sprintf( final, "&R[&w%2d&R]&w ", ++iReset );

		switch ( pReset->command )
		{
			default:
				sprintf( buf, "Bad reset command: %c.", pReset->command );
				strcat( final, buf );
				break;

			case 'M':
				if ( !( pMobIndex = get_mob_index( pReset->arg1 ) ) )
				{
					sprintf( buf, "Load Mobile - Bad Mob %d\n\r", pReset->arg1 );
					strcat( final, buf );
					continue;
				}

				if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
				{
					sprintf( buf, "Load Mobile - Bad Room %d\n\r", pReset->arg3 );
					strcat( final, buf );
					continue;
				}

				pMob = pMobIndex;
				sprintf( buf, "&BM&R[&B%5d&R]&B %-13.13s in room             R[%5d] [%3d] %-15.15s&w\n\r",
						pReset->arg1, pMob->short_descr, pReset->arg3,
						pReset->arg2, pRoomIndex->name );
				strcat( final, buf );

				/*
				 * Check for pet shop.
				 * -------------------
				 */
				{
					ROOM_INDEX_DATA *pRoomIndexPrev;

					pRoomIndexPrev = get_room_index( pRoomIndex->vnum - 1 );
					if ( pRoomIndexPrev
							&& IS_SET( pRoomIndexPrev->room_flags, ROOM_PET_SHOP ) )
						final[5] = 'P';
				}

				break;

			case 'O':
				if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
				{
					sprintf( buf, "Load Object - Bad Object %d\n\r",
							pReset->arg1 );
					strcat( final, buf );
					continue;
				}

				pObj       = pObjIndex;

				if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
				{
					sprintf( buf, "Load Object - Bad Room %d\n\r", pReset->arg3 );
					strcat( final, buf );
					continue;
				}

				sprintf( buf, "&WO&R[&W%5d&R]&W %-13.13s in room             "
						"R[%5d]       %-15.15s&w\n\r",
						pReset->arg1, pObj->short_descr,
						pReset->arg3, pRoomIndex->name );
				strcat( final, buf );

				break;

			case 'P':
				if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
				{
					sprintf( buf, "Put Object - Bad Object %d\n\r",
							pReset->arg1 );
					strcat( final, buf );
					continue;
				}

				pObj       = pObjIndex;

				if ( !( pObjToIndex = get_obj_index( pReset->arg3 ) ) )
				{
					sprintf( buf, "Put Object - Bad To Object %d\n\r",
							pReset->arg3 );
					strcat( final, buf );
					continue;
				}

				sprintf( buf,
						"O[%5d] %-13.13s inside              O[%5d]       %-15.15s\n\r",
						pReset->arg1,
						pObj->short_descr,
						pReset->arg3,
						pObjToIndex->short_descr );
				strcat( final, buf );

				break;

			case 'G':
			case 'E':
				if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
				{
					sprintf( buf, "Give/Equip Object - Bad Object %d\n\r",
							pReset->arg1 );
					strcat( final, buf );
					continue;
				}

				pObj       = pObjIndex;

				if ( !pMob )
				{
					sprintf( buf, "Give/Equip Object - No Previous Mobile\n\r" );
					strcat( final, buf );
					break;
				}

				/*	    if ( pMob->pShop ) */
				if ( ( pMob->pShop ) && ( pReset->arg2 == WEAR_NONE ) ) /* Angi */
				{
					sprintf( buf,
							"O[%5d] %-13.13s in the inventory of S[%5d]       %-15.15s\n\r",
							pReset->arg1,
							pObj->short_descr,
							pMob->vnum,
							pMob->short_descr  );
				}
				else
					sprintf( buf,
							"O[%5d] %-13.13s %-19.19s M[%5d]       %-15.15s\n\r",
							pReset->arg1,
							pObj->short_descr,
							(pReset->command == 'G') ?
							flag_string( wear_loc_strings, WEAR_NONE )
							: flag_string( wear_loc_strings, pReset->arg3 ),
							pMob->vnum,
							pMob->short_descr );
				strcat( final, buf );

				break;

				/*
				 * Doors are set in rs_flags don't need to be displayed.
				 * If you want to display them then uncomment the new_reset
				 * line in the case 'D' in load_resets in db.c and here.
				 *
				 case 'D':
				 pRoomIndex = get_room_index( pReset->arg1 );
				 sprintf( buf, "R[%5d] %s door of %-19.19s reset to %s\n\r",
				 pReset->arg1,
				 capitalize( dir_name[ pReset->arg2 ] ),
				 pRoomIndex->name,
				 flag_string( door_resets, pReset->arg3 ) );
				 strcat( final, buf );

				 break;
				 *
				 * End Doors Comment.
				 */
			case 'R':
				if ( !( pRoomIndex = get_room_index( pReset->arg1 ) ) )
				{
					sprintf( buf, "Randomize Exits - Bad Room %d\n\r",
							pReset->arg1 );
					strcat( final, buf );
					continue;
				}

				sprintf( buf, "R[%5d] Exits (0) to (%d) are randomized in %s\n\r",
						pReset->arg1, pReset->arg2, pRoomIndex->name );
				strcat( final, buf );

				break;
		}
		send_to_char(C_DEFAULT, final, ch );
	}

	return;
}



/*****************************************************************************
Name:		add_reset
Purpose:	Inserts a new reset in the given index slot.
Called by:	do_resets(olc.c).
 ****************************************************************************/
void add_reset( ROOM_INDEX_DATA *room, RESET_DATA *pReset, int index )
{
	RESET_DATA *reset;
	int iReset = 0;

	if ( !room->reset_first )
	{
		room->reset_first	= pReset;
		room->reset_last	= pReset;
		pReset->next		= NULL;
		return;
	}

	index--;

	if ( index == 0 )	/* First slot (1) selected. */
	{
		pReset->next = room->reset_first;
		room->reset_first = pReset;
		return;
	}

	/*
	 * If negative slot( <= 0 selected) then this will find the last.
	 */
	for ( reset = room->reset_first; reset->next; reset = reset->next )
	{
		if ( ++iReset == index )
			break;
	}

	pReset->next	= reset->next;
	reset->next		= pReset;
	if ( !pReset->next )
		room->reset_last = pReset;
	return;
}



void do_resets( CHAR_DATA *ch, char *argument )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char arg4[MAX_INPUT_LENGTH];
	char arg5[MAX_INPUT_LENGTH];
	RESET_DATA *pReset = NULL;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );
	argument = one_argument( argument, arg4 );
	argument = one_argument( argument, arg5 );

	/*
	 * Display resets in current room.
	 * -------------------------------
	 */
	if ( arg1[0] == '\0' )
	{
		if ( ch->in_room->reset_first )
		{
			send_to_char(C_DEFAULT,
					"Resets: M = mobile, R = room, O = object, "
					"P = pet, S = shopkeeper\n\r", ch );
			display_resets( ch );
		}
		else
			send_to_char(C_DEFAULT, "No resets in this room.\n\r", ch );
		return;
	}

	if ( !IS_BUILDER( ch, ch->in_room->area ) )
	{
		send_to_char(C_DEFAULT, "Resets: Invalid security for editing this area.\n\r", ch );
		return;
	}

	/*
	 * Take index number and search for commands.
	 * ------------------------------------------
	 */
	if ( is_number( arg1 ) )
	{
		ROOM_INDEX_DATA *pRoom = ch->in_room;

		/*
		 * Delete a reset.
		 * ---------------
		 */
		if ( !str_cmp( arg2, "delete" ) )
		{
			int insert_loc = atoi( arg1 );

			if ( !ch->in_room->reset_first )
			{
				send_to_char(C_DEFAULT, "No resets in this room.\n\r", ch );
				return;
			}

			if ( insert_loc-1 <= 0 )
			{
				pReset = pRoom->reset_first;
				pRoom->reset_first = pRoom->reset_first->next;
				if ( !pRoom->reset_first )
					pRoom->reset_last = NULL;
			}
			else
			{
				int iReset = 0;
				RESET_DATA *prev = NULL;

				for ( pReset = pRoom->reset_first;
						pReset;
						pReset = pReset->next )
				{
					if ( ++iReset == insert_loc )
						break;
					prev = pReset;
				}

				if ( !pReset )
				{
					send_to_char(C_DEFAULT, "Reset not found.\n\r", ch );
					return;
				}

				if ( prev )
					prev->next = prev->next->next;
				else
					pRoom->reset_first = pRoom->reset_first->next;

				for ( pRoom->reset_last = pRoom->reset_first;
						pRoom->reset_last->next;
						pRoom->reset_last = pRoom->reset_last->next );
			}

			free_reset_data( pReset );
			send_to_char(C_DEFAULT, "Reset deleted.\n\r", ch );
		}
		/*      else if ( (!str_cmp( arg2, "mob" ) && is_number( arg3 ))
				|| (!str_cmp( arg2, "obj" ) && is_number( arg3 ))
				|| (!str_cmp( arg2, "ran" ) && is_number( arg3 )) ) */
		/*
		 * Add a reset.
		 * ------------
		 */
		/*      { */
		/*
		 * Check for Mobile reset.
		 * -----------------------
		 */
		/*	if ( !str_cmp( arg2, "mob" ) )
			{
			pReset = new_reset_data();
			pReset->command = 'M';
			pReset->arg1    = atoi( arg3 );
			pReset->arg2    = is_number( arg4 ) ? atoi( arg4 ) : 1 ;*/ /* Max # */
		/*	  pReset->arg3    = ch->in_room->vnum;
			  }
			  else if ( !str_cmp( arg2, "obj" ) ) */
		/*
		 * Check for Object reset.
		 * -----------------------
		 */
		/*	{
			pReset = new_reset_data();
			pReset->arg1    = atoi( arg3 ); */
		/*
		 * Inside another object.
		 * ----------------------
		 */
		/*	  if ( !str_prefix( arg4, "inside" ) )
			  {
			  pReset->command = 'P';
			  pReset->arg2    = 0;
			  pReset->arg3    = is_number( arg5 ) ? atoi( arg5 ) : 1;
			  }
			  else if ( !str_cmp( arg4, "room" ) ) */
		/*
		 * Inside the room.
		 * ----------------
		 */
		/*	  {
			  pReset = new_reset_data();
			  pReset->command = 'O';
			  pReset->arg2     = 0;
			  pReset->arg3     = ch->in_room->vnum;
			  }
			  else */
		/*
		 * Into a Mobile's inventory.
		 * --------------------------
		 */
		/*	  {
			  if ( flag_value( wear_loc_flags, arg4 ) == NO_FLAG )
			  {
			  send_to_char(C_DEFAULT, "Resets: '? wear-loc'\n\r", ch );
			  return;
			  }
			  pReset = new_reset_data();
			  pReset->arg3 = flag_value( wear_loc_flags, arg4 );
			  if ( pReset->arg2 == WEAR_NONE )
			  pReset->command = 'G';
			  else
			  pReset->command = 'E';
			  }
			  }
			  else if ( !str_cmp( arg2, "ran" ) ) */
		/*
		 * Random Exit Resets.
		 * Added By Altrag.
		 */
		/*	{
			pReset = new_reset_data();
			pReset->command = 'R';
			pReset->arg1    = atoi( arg3 );
			pReset->arg2    = ch->in_room->vnum;
			}

			add_reset( ch->in_room, pReset, atoi( arg1 ) );
			send_to_char(C_DEFAULT, "Reset added.\n\r", ch );
			} */
			  else
			  {
				  /*	send_to_char(C_DEFAULT, "Syntax: RESET <number> OBJ <vnum> <wear_loc>\n\r", ch );
						send_to_char(C_DEFAULT, "        RESET <number> OBJ <vnum> in <vnum>\n\r", ch );
						send_to_char(C_DEFAULT, "        RESET <number> OBJ <vnum> room\n\r", ch );
						send_to_char(C_DEFAULT, "        RESET <number> MOB <vnum> [<max #>]\n\r", ch );
						send_to_char(C_DEFAULT, "        RESET <number> RAN <last-door>\n\r", ch ); */
				  send_to_char(C_DEFAULT, "Syntax: &BRESET &W(displays resets in room)", ch );
				  send_to_char(C_DEFAULT, "        &BRESET <number> DELETE\n\r", ch );
			  }
	}

	return;
}



/*****************************************************************************
Name:		do_alist
Purpose:	Normal command to list areas and display area information.
Called by:	interpreter(interp.c)
 ****************************************************************************/
void do_alist( CHAR_DATA *ch, char *argument )
{
	char buf    [ MAX_STRING_LENGTH ];
	char result [ MAX_STRING_LENGTH*2 ];	/* May need tweaking. */
	AREA_DATA *pArea;
	char *prot = "";

	/*    sprintf( result, "[%3s] [%-28s] (%-5s-%5s) [%-10s]  %3s  [%-8s]\n\r",*/
	sprintf( result, "&z[&W%3s&z] &z[&B%-28s&z] &z(&W%-5s-%5s&z) &z[&G%-10s&z] &z[&W%3s&z][&W%-8s&z]\n\r",
			"Num", "Area Name", "lvnum", "uvnum", "Filename", "Sec", "Builders" );

	for ( pArea = area_first; pArea; pArea = pArea->next )
	{
		if ( IS_SET( pArea->area_flags, AREA_PROTOTYPE ) )
			prot = "*";
		else
			prot = " ";
		/*	sprintf( buf, "[%3d] %s%-29.29s (%-5d-%5d) %-12.12s [%3d] [%-8.8s]\n\r",*/
		sprintf( buf, "&z[&W%3d&z] &R%s&B%-29.29s &z(&W%-5d-%5d&z) &G%-12.12s &z[&w%3d&z][&W%-8.8s&z]\n\r",
				pArea->vnum,
				prot,
				&pArea->name[8],
				pArea->lvnum,
				pArea->uvnum,
				pArea->filename,
				pArea->security,
				pArea->builders );
		strcat( result, buf );
	}

	send_to_char(C_DEFAULT, result, ch );
	return;
}

/* XOR */
void hedit(CHAR_DATA *ch, char *argument)
{
	char command[MAX_INPUT_LENGTH];
	char arg[MAX_STRING_LENGTH];
	int cmd;

	smash_tilde(argument);
	strcpy(arg, argument);
	argument = one_argument(argument, command);

	if(command[0] == '\0')
	{
		hedit_show(ch, argument);
		return;
	}

	if(!str_cmp(command, "done"))
	{
		/*    HELP_DATA *pHelp;
			  static int counter;

			  counter++;
			  if ( counter >= 5 )
			  {
			  EDIT_HELP(ch, pHelp);
			  save_area(pHelp->area);
			  counter = 0;
			  } */
		save_helps();
		edit_done( ch );
		return;
	}

	for(cmd = 0;*hedit_table[cmd].name;cmd++)
	{
		if(!str_prefix(command, hedit_table[cmd].name))
		{
			(*hedit_table[cmd].olc_fun) (ch, argument);
			return;
		}
	}

	/* Default to Standard Interpreter. */
	interpret(ch, arg);
	return;
}

HELP_DATA *get_help(char *argument)
{
	HELP_DATA *pHelp;
	for(pHelp = help_first;pHelp;pHelp = pHelp->next)
	{
		if(is_name(NULL, argument, pHelp->keyword))
		{
			return pHelp;
		}
	}
	return NULL;
}

void do_hedit(CHAR_DATA *ch, char *argument)
{
	HELP_DATA *pHelp;
	char command[MAX_INPUT_LENGTH];
	char arg[MAX_STRING_LENGTH];

	strcpy( arg, argument );
	argument = one_argument(argument, command);

	if(command[0] == 'c' && !str_prefix(command, "create"))
	{
		if(get_help(argument) != NULL)
		{
			send_to_char(C_DEFAULT, "Help entry already exist.\n\r", ch);
			return;
		}
		/*    if(help_free)
			  {
			  pHelp = help_free;
			  help_free = help_free->next;
			  }
			  else*/
		pHelp = alloc_perm(sizeof(*pHelp));


		if(!help_first)
			help_first = pHelp;
		if(help_last)
			help_last->next = pHelp;

		/*    pHelp->area = help_last->area; */
		help_last	= pHelp;
		pHelp->next	= NULL;
		top_help++;

		pHelp->level   = L_SEN;
		if(pHelp->keyword)
			free_string(pHelp->keyword);
		pHelp->keyword = str_dup(argument);
		if(pHelp->text)
			free_string(pHelp->text);
	}
	else
	{
		if((pHelp = get_help(arg)) == NULL)
		{
			send_to_char(C_DEFAULT, "Help entry not found.\n\r", ch);
			return;
		}
	}
	ch->desc->pEdit = (void *) pHelp;
	ch->desc->editor = ED_HELP;
	ch->desc->inEdit = NULL;
	ch->desc->editin = 0;
	hedit_show(ch, "");
	return;
}
/* END */

void mreset(CHAR_DATA *ch, char *argument)
{
	char command[MAX_INPUT_LENGTH];
	char arg[MAX_STRING_LENGTH];

	smash_tilde(argument);
	strcpy(arg, argument);
	argument = one_argument(argument, command);

	if(command[0] == '\0')
	{
		mreset_show(ch, argument);
		return;
	}

	if(!str_cmp(command, "done"))
	{
		edit_done( ch );
		return;
	}

	/* Call editor function */
	/*  for(cmd = 0;*reset_table[cmd].name;cmd++)
		{
		if(!str_prefix(command, reset_table[cmd].name))
		{
		(*reset_table[cmd].olc_fun) (ch, argument);
		return;
		}
		}*/

	/* Default to Standard Interpreter. */
	interpret(ch, arg);
	return;

}

void do_mreset(CHAR_DATA *ch, char *argument)
{

	ch->desc->pEdit = NULL;
	ch->desc->editor = ED_MRESET;
	ch->desc->inEdit = NULL;
	ch->desc->editin = 0;
	mreset_show(ch, "");
	return;
}

void sedit(CHAR_DATA *ch, char *argument)
{
	char command[MAX_INPUT_LENGTH];
	char arg[MAX_STRING_LENGTH];
	int cmd;

	smash_tilde(argument);
	strcpy(arg, argument);
	argument = one_argument(argument, command);

	if(command[0] == '\0')
	{
		sedit_show(ch, argument);
		return;
	}

	if(!str_cmp(command, "credit"))
	{
		send_to_char( AT_YELLOW, "Made by Decklarean, 1996.\n\r", ch );
		return;
	}

	if(!str_cmp(command, "done"))
	{
		save_social( );
		edit_done( ch );
		return;
	}

	/* Call editor function */
	for(cmd = 0;*sedit_table[cmd].name;cmd++)
	{
		if(!str_prefix(command, sedit_table[cmd].name))
		{
			(*sedit_table[cmd].olc_fun) (ch, argument);
			return;
		}
	}

	/* Default to Standard Interpreter. */
	interpret(ch, arg);
	return;
}

/* get a social out of the social list */

SOCIAL_DATA *get_social(char *argument)
{
	SOCIAL_DATA *pSocial;
	for(pSocial = social_first;pSocial;pSocial = pSocial->next)
	{
		if(is_name(NULL, argument, pSocial->name))
		{
			return pSocial;
		}
	}
	return NULL;
}

void do_sedit(CHAR_DATA *ch, char *argument)
{
	SOCIAL_DATA *pSocial;
	char command[MAX_INPUT_LENGTH];
	char arg[MAX_STRING_LENGTH];

	strcpy( arg, argument );
	argument = one_argument(argument, command);

	if(command[0] == 'c' && !str_prefix(command, "create"))
	{
		if(argument[0] == '\0')
		{
			send_to_char(C_DEFAULT, "SEdit:  Syntax: sedit create <social name>\n\r", ch);
			return;
		}

		if(get_social(argument) != NULL)
		{
			send_to_char(C_DEFAULT, "SEdit:  Social entry already exists.\n\r", ch);
			return;
		}

		pSocial = new_social_index( );

		if(!social_first)
			social_first = pSocial;
		if(social_last)
			social_last->next = pSocial;

		social_last	  = pSocial;
		/*    pSocial->next = NULL;
			  top_social++;

			  free_string( pSocial->name		);
			  free_string( pSocial->char_no_arg	);
			  free_string( pSocial->others_no_arg	);
			  free_string( pSocial->char_found	);
			  free_string( pSocial->others_found	);
			  free_string( pSocial->vict_found	);
			  free_string( pSocial->char_auto	);
			  free_string( pSocial->others_auto	); */

		pSocial->name = str_dup(argument);

	}
	else
	{
		if((pSocial = get_social(arg)) == NULL)
		{
			send_to_char(C_DEFAULT, "Social entry not found.\n\r", ch);
			return;
		}
	}
	ch->desc->pEdit = (void *) pSocial;
	ch->desc->editor = ED_SOCIAL;
	ch->desc->inEdit = NULL;
	ch->desc->editin = 0;
	sedit_show(ch, "");
	return;
}

void rename_object(CHAR_DATA *ch, char *argument)
{
	char command[MAX_INPUT_LENGTH];
	char arg[MAX_STRING_LENGTH];
	int cmd;

	smash_tilde(argument);
	strcpy(arg, argument);
	argument = one_argument(argument, command);

	if(command[0] == '\0')
	{
		rename_show(ch, argument);
		return;
	}

	if(!str_cmp(command, "credit"))
	{
		send_to_char( AT_YELLOW, "Made by Decklarean, 1996.\n\r", ch );
		return;
	}

	if(!str_cmp(command, "done"))
	{
		edit_done( ch );
		save_char_obj( ch );
		return;
	}

	/* Call editor function */
	for(cmd = 0;*rename_obj_table[cmd].name;cmd++)
	{
		if(!str_prefix(command, rename_obj_table[cmd].name))
		{
			(*rename_obj_table[cmd].olc_fun) (ch, argument);
			return;
		}
	}

	/* Default to Standard Interpreter. */
	interpret(ch, arg);
	return;
}

void do_rename_obj( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *pObj;
	char command[MAX_INPUT_LENGTH];

	argument = one_argument( argument, command );

	if ( command[0] == '\0' )
	{
		send_to_char(C_DEFAULT, "RENAME_OBJ:  Syntax: rename <obj name>.\n\r", ch );
		return;
	}

	if ( !( pObj = get_obj_carry( ch, command ) ) )
	{
		send_to_char(C_DEFAULT, "RENAME_OBJ:  You don't have that obj.\n\r", ch );
		return;
	}
	else
	{
		ch->desc->pEdit = (void *)pObj;
		ch->desc->editor = RENAME_OBJECT;
		rename_show( ch, "" );
		return;
	}

}

void forge_obj( CHAR_DATA *ch, OBJ_DATA *to_forge )
{
	ch->desc->pEdit = (void *)to_forge;
	ch->desc->editor = FORGE_OBJECT;
	forge_show( ch, "" );
	return;
}
void forge_object(CHAR_DATA *ch, char *argument)
{
	char command[MAX_INPUT_LENGTH];
	char arg[MAX_STRING_LENGTH];
	int cmd;

	smash_tilde(argument);
	strcpy(arg, argument);
	argument = one_argument(argument, command);

	if(command[0] == '\0')
	{
		forge_show(ch, argument);
		return;
	}

	if(!str_cmp(command, "done"))
	{
		edit_done( ch );
		save_char_obj( ch );
		return;
	}

	/* Call editor function */
	for(cmd = 0;*forge_obj_table[cmd].name;cmd++)
	{
		if(!str_prefix(command, forge_obj_table[cmd].name))
		{
			if ( is_number( command )
					&& atoi( command ) > ch->level / 10 )
			{
				send_to_char( AT_GREY, "You cannot forge so much at your level.\n\r", ch );
				return;
			}
			(*forge_obj_table[cmd].olc_fun) (ch, argument);
			return;
		}
	}

	/* Default to Standard Interpreter. */
	interpret(ch, arg);
	return;
}
void forge_pay_cost( CHAR_DATA *ch )
{
	OBJ_DATA *obj;

	FORGE_OBJ(ch, obj);


	if ( ( (ch->money.gold*C_PER_G) + (ch->money.silver*S_PER_G) +
				(ch->money.copper) ) < ( (obj->cost.gold*C_PER_G) + (obj->cost.silver*S_PER_G) +
					(obj->cost.copper) ) )
	{
		send_to_char( AT_GREY, "You cannot afford to forge this object.\n\r", ch );
		extract_obj( obj );
		return;
	}
	obj_to_char( obj, ch );
	spend_money( &ch->money, &obj->cost );
	return;
}

void spedit(CHAR_DATA *ch, char *argument)
{
	char command[MAX_INPUT_LENGTH];
	char arg[MAX_STRING_LENGTH];
	int cmd;

	smash_tilde(argument);
	strcpy(arg, argument);
	argument = one_argument(argument, command);

	if(command[0] == '\0')
	{
		spedit_show(ch, argument);
		return;
	}

	if(!str_cmp(command, "credit"))
	{
		send_to_char( AT_YELLOW, "Made by Decklarean, 1997.\n\r", ch );
		return;
	}

	if(!str_cmp(command, "done"))
	{
		/*save_spellskill( );*/
		edit_done( ch );
		return;
	}

	/* Call editor function */
	for(cmd = 0;*spedit_table[cmd].name;cmd++)
	{
		if(!str_prefix(command, spedit_table[cmd].name))
		{
			(*spedit_table[cmd].olc_fun) (ch, argument);
			return;
		}
	}

	/* Default to Standard Interpreter. */
	interpret(ch, arg);
	return;
}

void do_spedit(CHAR_DATA *ch, char *argument)
{
	SKILL_TYPE *pSpell;
	char command[MAX_INPUT_LENGTH];
	char arg[MAX_STRING_LENGTH];
	int sn;

	strcpy( arg, argument );
	argument = one_argument(argument, command);

	if(command[0] == 'c' && !str_prefix(command, "create"))
	{
		if(argument[0] == '\0')
		{
			send_to_char(C_DEFAULT, "SPedit:  Syntax: spedit create <spell name>\n\r", ch);
			return;
		}

		if( skill_lookup(argument) < 0 )
		{
			send_to_char(C_DEFAULT, "SPEdit:  Spell/Skill entry already exist.\n\r", ch);
			return;
		}

		send_to_char(C_DEFAULT, "SPEdit:  Spell/Skill online creation not avable.\n\r", ch);
		return;
	}
	else
	{
		if( (sn = skill_lookup(arg)) < 0)
		{
			send_to_char(C_DEFAULT, "Spell/skill entry not found.\n\r", ch);
			return;
		}
		pSpell = (SKILL_TYPE *)&skill_table[sn];
	}
	ch->desc->pEdit = (void *) pSpell;
	ch->desc->editor = ED_SPEDIT;
	ch->desc->inEdit = NULL;
	ch->desc->editin = 0;
	spedit_show(ch, "");
	return;
}

void nedit( CHAR_DATA *ch, char *argument )
{
	char command [ MAX_INPUT_LENGTH ];
	char arg [ MAX_INPUT_LENGTH ];
	int cmd;

	smash_tilde( argument );
	strcpy( arg, argument );
	argument = one_argument( argument, command );

	if( command[0] == '\0' )
	{
		nedit_show( ch, argument );
		return;
	}

	if( !str_cmp( command, "done" ) )
	{
		save_newbie( );
		edit_done( ch );
		return;
	}

	for( cmd = 0; *nedit_table[cmd].name; cmd++ )
	{
		if( !str_prefix( command, nedit_table[cmd].name ) )
		{
			( *nedit_table[cmd].olc_fun ) ( ch, argument );
			return;
		}
	}

	interpret( ch, arg );
	return;

}

NEWBIE_DATA *get_newbie_data( char *argument ) {
	NEWBIE_DATA *pNewbie;

	for( pNewbie = newbie_first; pNewbie; pNewbie = pNewbie->next ) {
		if( is_name( NULL, argument, pNewbie->keyword ) ) {
			return pNewbie;
		}
	}

	return NULL;
}

void do_nedit( CHAR_DATA *ch, char *argument ) {
	NEWBIE_DATA *pNewbie;
	char command [ MAX_INPUT_LENGTH ];
	char arg     [ MAX_STRING_LENGTH ];

	strcpy( arg, argument );
	argument = one_argument(argument, command);

	if ( command[0] == 'c' && !str_prefix( command, "create" ) ) {
		if( argument[0] == '\0' ) {
			send_to_char( C_DEFAULT, "Syntax: nedit create <keyword>\n\r", ch );
			return;
		}

		if( get_newbie_data( argument ) != NULL ) {
			send_to_char( C_DEFAULT, "Keyword already exists.\n\r", ch );
			return;
		}

		pNewbie = new_newbie_index();

		pNewbie->keyword = str_dup( argument );

		newbie_sort(pNewbie);
	} else if ( !(pNewbie = get_newbie_data( arg ) ) ) {
		send_to_char( C_DEFAULT, "Keyword doesn't exist.\n\r", ch );
		return;
	}

	ch->desc->pEdit = (void *)pNewbie;
	ch->desc->editor = ED_NEWBIE;
	nedit_show( ch, "" );

	return;
}
