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



bool	check_social	args( ( CHAR_DATA *ch, char *command,
			char *argument ) );
bool    check_alias     args( ( CHAR_DATA *ch, char *command,
			char *argument ) );
void    add_alias   args( ( CHAR_DATA *ch, ALIAS_DATA *pAl,
			char *old, char *new ) );
bool	can_use_cmd	args( ( int cmd, CHAR_DATA *ch, int trust ) );

/*
 * Command logging types.
 */
#define LOG_NORMAL              0
#define LOG_ALWAYS              1
#define LOG_NEVER               2
/*
 * Altrag was here.
 */
#define LOG_BUILD               3


/*
 * God Levels - Check them out in merc.h
 */

#define L_HER                   LEVEL_HERO

/*
 * Log-all switch.
 */
bool				fLogAll		    = FALSE;



/*
 * Command table.
 */
struct	cmd_type	cmd_table	[ ] =
{
	/*
	 * Common movement commands.
	 */
	{ "north",		do_north,	POS_STANDING,	 0,  LOG_NORMAL	},
	{ "east",		do_east,	POS_STANDING,	 0,  LOG_NORMAL	},
	{ "south",		do_south,	POS_STANDING,	 0,  LOG_NORMAL	},
	{ "west",		do_west,	POS_STANDING,	 0,  LOG_NORMAL },
	{ "up",		do_up,		POS_STANDING,	 0,  LOG_NORMAL },
	{ "down",		do_down,	POS_STANDING,	 0,  LOG_NORMAL },
	{ "push",		do_push,	POS_STANDING,	 0,  LOG_NORMAL },
	{ "drag",		do_drag,	POS_STANDING,	 0,  LOG_NORMAL },

	/*
	 * Common other commands.
	 * Placed here so one and two letter abbreviations work.
	 */
	{ "accept",		do_accept,	POS_STANDING,	 0,  LOG_NORMAL },
	{ "buy",		do_buy,		POS_STANDING,	 0,  LOG_NORMAL	},
	{ "bug",		do_bug,		POS_DEAD,	 0,  LOG_NORMAL	},
	{ "declare",	do_declare,		POS_STANDING,	 0,  LOG_NORMAL	},
	{ "cast",		do_cast,	POS_FIGHTING,	 0,  LOG_NORMAL	},
	{ "challenge",	do_challenge,	POS_STANDING,	 0,  LOG_NORMAL },
	{ "deposit",	do_deposit,     POS_RESTING,     0, LOG_NORMAL  },
	{ "dual",           do_dual,        POS_RESTING,     0,  LOG_NORMAL },
	{ "exits",		do_exits,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "flip",		do_flip,	POS_FIGHTING,	0,	LOG_NORMAL },
	{ "get",		do_get,		POS_RESTING,	 0,  LOG_NORMAL	},
	{ "palm",		do_palm,	POS_RESTING,	 0,  LOG_NORMAL },
	{ "inventory",	do_inventory,	POS_DEAD,	 0,  LOG_NORMAL	},
	{ "identify", do_identify, POS_RESTING, 0, LOG_NORMAL },
	{ "invoke",         do_invoke,      POS_RESTING,     0,  LOG_NORMAL },
	{ "kill",		do_kill,	POS_FIGHTING,	 0,  LOG_NORMAL	},
	{ "look",		do_look,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "lure",		do_lure,	POS_FIGHTING,	 0,  LOG_NORMAL	},
	{ "order",		do_order,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "rest",		do_rest,	POS_SLEEPING,	 0,  LOG_NORMAL	},
	{ "repair",         do_repair,      POS_RESTING,     0,  LOG_NORMAL },
	// { "sing",		do_cast,	POS_FIGHTING,	 0,  LOG_NORMAL },
	{ "sleep",		do_sleep,	POS_SLEEPING,	 0,  LOG_NORMAL	},
	{ "stand",		do_stand,	POS_SLEEPING,	 0,  LOG_NORMAL	},
	{ "tell",		do_tell,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "track",          do_track,       POS_STANDING,    0,  LOG_NORMAL },
	{ "war	",		do_war,		POS_STANDING,	0,	LOG_NORMAL	},
	{ "wield",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "wizhelp",	do_wizhelp,	POS_DEAD,        0,  LOG_NORMAL	},

	{ "mpasound",       do_mpasound,    POS_DEAD,        0,  LOG_NORMAL },
	{ "mpjunk",         do_mpjunk,      POS_DEAD,        0,  LOG_NORMAL },
	{ "mpecho",         do_mpecho,      POS_DEAD,        0,  LOG_NORMAL },
	{ "mpechoat",       do_mpechoat,    POS_DEAD,        0,  LOG_NORMAL },
	{ "mpechoaround",   do_mpechoaround,POS_DEAD,        0,  LOG_NORMAL },
	{ "mpkill",         do_mpkill      ,POS_DEAD,        0,  LOG_NORMAL },
	{ "mpmload",        do_mpmload     ,POS_DEAD,        0,  LOG_NORMAL },
	{ "mpoload",        do_mpoload     ,POS_DEAD,        0,  LOG_NORMAL },
	{ "mppurge",        do_mppurge     ,POS_DEAD,        0,  LOG_NORMAL },
	{ "mpgoto",         do_mpgoto      ,POS_DEAD,        0,  LOG_NORMAL },
	{ "mpat",           do_mpat        ,POS_DEAD,        0,  LOG_NORMAL },
	{ "mptransfer",     do_mptransfer  ,POS_DEAD,        0,  LOG_NORMAL },
	{ "mpforce",        do_mpforce     ,POS_DEAD,        0,  LOG_NORMAL },
	{ "mpteleport",     do_mpteleport,  POS_DEAD,        0,  LOG_NORMAL },

	/*
	 * Informational commands.
	 */
	{ "affected",       do_affectedby,  POS_DEAD,        0,  LOG_NORMAL },
	{ "raffected",	do_raffect,	POS_DEAD,	 0,  LOG_NORMAL },
	{ "areas",		do_areas,	POS_DEAD,	 0,  LOG_NORMAL	},
	{ "commands",	do_commands,	POS_DEAD,	 0,  LOG_NORMAL },
	{ "compare",	do_compare,	POS_RESTING,	 0,  LOG_NORMAL },
	{ "consider",	do_consider,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "credits",	do_credits,	POS_DEAD,	 0,  LOG_NORMAL	},
	{ "enter",          do_enter,       POS_STANDING,    0,  LOG_NORMAL },
	{ "equipment",	do_equipment,	POS_DEAD,	 0,  LOG_NORMAL	},
	{ "examine",	do_examine,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "finger",         do_finger,      POS_DEAD,        0,  LOG_ALWAYS },
	{ "forge",		do_forge,	POS_STANDING,   30,  LOG_ALWAYS },
	{ "help",		do_help,	POS_DEAD,	 0,  LOG_NORMAL	},
	{ "idea",		do_idea,	POS_DEAD,	 0,  LOG_NORMAL	},
	{ "report",		do_report,	POS_DEAD,	 0,  LOG_NORMAL	},
	{ "pagelength",     do_pagelen,     POS_DEAD,        0,  LOG_NORMAL },
	{ "read",		do_look,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "scan",		do_scan,	POS_STANDING,	0,	LOG_NORMAL	},
	{ "score",		do_score,	POS_DEAD,	 0,  LOG_NORMAL	},
	{ "scent",		do_scent,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "slist",          do_slist,       POS_DEAD,        0,  LOG_NORMAL },
	{ "socials",	do_socials,	POS_DEAD,	 0,  LOG_NORMAL },
	{ "time",		do_time,	POS_DEAD,	 0,  LOG_NORMAL	},
	{ "typo",		do_typo,	POS_DEAD,	 0,  LOG_NORMAL	},
	{ "weather",	do_weather,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "worth",          do_worth,       POS_DEAD,        0,  LOG_NORMAL },
	{ "who",		do_who,		POS_DEAD,	 0,  LOG_NORMAL	},
	{ "wizlist",        do_immlist,     POS_DEAD,        0,  LOG_NORMAL },
	{ "champlist",	do_champlist,	POS_DEAD,	 0,  LOG_NORMAL },
	{ "newbie",		do_newbie,	POS_DEAD,        0,  LOG_NORMAL },

	/*
	 * Clan commands.
	 */
	{ "induct",        do_induct,       POS_STANDING,    0,  LOG_ALWAYS },
	{ "outcast",       do_outcast,      POS_STANDING,    0,  LOG_ALWAYS },
	{ "setlev",        do_setlev,       POS_STANDING,    0,  LOG_ALWAYS },
	{ "cinfo",         do_cinfo,        POS_DEAD,        0,  LOG_NORMAL },
	// { "assasinate",    do_assasinate,   POS_STANDING,   30,  LOG_ALWAYS },
	{ "farsight",      do_farsight,     POS_SLEEPING,   30,  LOG_NORMAL },
	{ "cquest",        do_clanquest,    POS_DEAD,        0,  LOG_NORMAL },

	/*
	 * Configuration commands.
	 */
	{ "alias",          do_alias,       POS_DEAD,        0,  LOG_NORMAL },
	{ "auto",           do_auto,        POS_DEAD,        0,  LOG_NORMAL },
	{ "autoexit",       do_autoexit,    POS_DEAD,        0,  LOG_NORMAL },
	{ "autoloot",       do_autoloot,    POS_DEAD,        0,  LOG_NORMAL },
	{ "autocoins",      do_autocoins,   POS_DEAD,        0,  LOG_NORMAL },
	{ "autosac",        do_autosac,     POS_DEAD,        0,  LOG_NORMAL },
	{ "blank",          do_blank,       POS_DEAD,        0,  LOG_NORMAL },
	{ "brief",          do_brief,       POS_DEAD,        0,  LOG_NORMAL },
	{ "channels",	do_channels,	POS_DEAD,	 0,  LOG_NORMAL	},
	{ "combine",        do_combine,     POS_DEAD,        0,  LOG_NORMAL },
	{ "autosplit",      do_autosplit,   POS_DEAD,        0,  LOG_NORMAL },
	{ "ansi",           do_ansi,    	POS_DEAD,        0,  LOG_NORMAL },
	{ "fullname",       do_fullname,    POS_DEAD,        0,  LOG_NORMAL },
	{ "combat",         do_combat,      POS_DEAD,        0,  LOG_NORMAL },
	{ "telnetga",       do_telnetga,    POS_DEAD,        0,  LOG_NORMAL },
	{ "config",		do_config,	POS_DEAD,	 0,  LOG_NORMAL	},
	{ "description",	do_description,	POS_DEAD,	 0,  LOG_NORMAL	},
	{ "email",		do_email,	POS_DEAD,	 0,  LOG_NORMAL },
	{ "password",	do_password,	POS_DEAD,	 0,  LOG_NEVER	},
	{ "plan",		do_plan,	POS_DEAD,	 0,  LOG_NORMAL },
	{ "pr",     	do_practice,	POS_SLEEPING,	 0,  LOG_NORMAL	},
	{ "prompt",         do_prompt,      POS_DEAD,        0,  LOG_NORMAL },
	{ "title",		do_title,	POS_DEAD,	 0,  LOG_NORMAL	},
	{ "wimpy",		do_wimpy,	POS_DEAD,	 0,  LOG_NORMAL	},
	// { "qjoin",		do_questflag,   POS_DEAD, 0,LOG_NORMAL },
	/*
	 * Communication commands.
	 */
	{ "answer",		do_answer,	POS_SLEEPING,	 0,  LOG_NORMAL },
	{ "auction",	do_auction,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "bid",            do_bid,         POS_RESTING,     0,  LOG_NORMAL },
	{ "chat",		do_chat,	POS_SLEEPING,	 0,  LOG_NORMAL	},
	{ "emote",		do_emote,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "gtell",		do_gtell,	POS_DEAD,	 0,  LOG_NORMAL	},
	{ "music",		do_music,	POS_SLEEPING,	 0,  LOG_NORMAL },
	{ "note",		do_note,	POS_SLEEPING,	 0,  LOG_NORMAL	},
	// { "pose",		do_pose,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "quest",          do_quest,       POS_RESTING,     0,  LOG_NORMAL },
	{ "question",	do_question,	POS_SLEEPING,	 0,  LOG_NORMAL },
	{ "reply",		do_reply,	POS_SLEEPING,	 0,  LOG_NORMAL },
	{ "say",		do_say,		POS_RESTING,	 0,  LOG_NORMAL	},
	{ "shout",		do_shout,	POS_RESTING,	 3,  LOG_NORMAL	},
	{ "yell",		do_yell,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "clantalk",       do_clan,        POS_DEAD,        0,  LOG_NORMAL },
	{ "clans",          do_clans,       POS_DEAD,        0,  LOG_NORMAL },
	{ "class",          do_class,       POS_DEAD,        0,  LOG_NORMAL },
	{ "ooc",		do_ooc,		POS_DEAD,	 0,  LOG_NORMAL },
	{ "guard",          do_guard,       POS_DEAD,    L_CON,  LOG_NORMAL },
	{ "conference",     do_conference,  POS_DEAD,    L_IMP,  LOG_ALWAYS },
	{ "info",		do_info,	POS_DEAD,    L_DIR,  LOG_ALWAYS },
	{ "vent",           do_vent,        POS_DEAD,        0,  LOG_NORMAL },
	{ "clear_alias",    do_clear,       POS_DEAD,        0,  LOG_NORMAL },

	/*
	 * Object manipulation commands.
	 */
	{ "brandish",	do_brandish,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "close",		do_close,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "donate",		do_donate,	POS_RESTING,	 0,  LOG_NORMAL },
	{ "drink",		do_drink,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "drop",		do_drop,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "eat",		do_eat,		POS_RESTING,	 0,  LOG_NORMAL	},
	{ "fill",		do_fill,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "give",		do_give,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "hold",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "join",           do_join,        POS_RESTING,     0,  LOG_NORMAL },
	{ "list",		do_list,	POS_STANDING,	 0,  LOG_NORMAL	},
	{ "lock",		do_lock,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "open",		do_open,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "patch",          do_patch,       POS_STANDING,    26, LOG_NORMAL },
	{ "pick",		do_pick,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "put",		do_put,		POS_RESTING,	 0,  LOG_NORMAL },
	{ "quaff",		do_quaff,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "raise",		do_raise,	POS_STANDING,	0,	LOG_NORMAL },
	{ "recall",         do_lowrecall,   POS_STANDING,    0,  LOG_NORMAL },
	{ "recite",		do_recite,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "remove",		do_remove,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "remor",		do_remor,	POS_STANDING,	0,	LOG_NORMAL },
	{ "remort",		do_remort,	POS_STANDING,	0,	LOG_NORMAL },
	{ "retrieve",       do_retrieve,    POS_STANDING,    2,  LOG_NORMAL },
	{ "sell",		do_sell,	POS_STANDING,	 0,  LOG_NORMAL	},
	{ "take",		do_get,		POS_RESTING,	 0,  LOG_NORMAL	},
	{ "sacrifice",	do_sacrifice,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "separate",       do_separate,    POS_RESTING,     0,  LOG_NORMAL },
	{ "store",          do_store,       POS_STANDING,    2,  LOG_NORMAL },
	{ "unlock",		do_unlock,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "value",		do_value,	POS_STANDING,	 0,  LOG_NORMAL	},
	{ "wear",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "zap",		do_zap,		POS_RESTING,	 0,  LOG_NORMAL	},
	{ "stare",          do_stare,       POS_RESTING,     0,  LOG_NORMAL },
	{ "alchemy",        do_alchemy,     POS_RESTING,     0,  LOG_ALWAYS },
	{ "antidote",	do_antidote,	POS_RESTING,	 0,  LOG_NORMAL },
	{ "scribe",         do_scribe,      POS_RESTING,     0,  LOG_ALWAYS },
	{ "gravebind",	do_gravebind,	POS_RESTING,	 0,  LOG_NORMAL },
	{ "flamehand",	do_flamehand,	POS_RESTING,	 0,  LOG_NORMAL },
	{ "frosthand",	do_frosthand,	POS_RESTING,	 0,  LOG_NORMAL },
	{ "chaoshand",	do_chaoshand,	POS_RESTING,	 0,  LOG_NORMAL },
	/*
	 * Combat commands.
	 */
	{ "backstab",	do_backstab,	POS_STANDING,	 0,  LOG_NORMAL	},
	{ "bladepalm",	do_bladepalm,	POS_FIGHTING,	 0,  LOG_NORMAL },
	{ "bs",		do_backstab,	POS_STANDING,	 0,  LOG_NORMAL	},
	{ "disarm",		do_disarm,	POS_FIGHTING,	 0,  LOG_NORMAL	},
	{ "flee",		do_flee,	POS_FIGHTING,	 0,  LOG_NORMAL	},
	{ "retreat",	do_retreat,	POS_FIGHTING,	 0,  LOG_NORMAL },
	{ "flury",		do_flury,	POS_FIGHTING,	 0,  LOG_NORMAL	},
	{ "kick",		do_kick,	POS_FIGHTING,	 0,  LOG_NORMAL	},
	{ "high kick",	do_high_kick,	POS_FIGHTING,	 0,  LOG_NORMAL	},
	{ "jump kick",	do_jump_kick,	POS_FIGHTING,	 0,  LOG_NORMAL	},
	{ "spin kick",	do_spin_kick,	POS_FIGHTING,	 0,  LOG_NORMAL	},
	{ "flying kick",	do_flyingkick,	POS_FIGHTING,	 0,  LOG_NORMAL },
	{ "punch",          do_punch,       POS_FIGHTING,    0,  LOG_NORMAL },
	{ "jab punch",      do_jab_punch,   POS_FIGHTING,    0,  LOG_NORMAL },
	{ "kidney punch",   do_kidney_punch,POS_FIGHTING,    0,  LOG_NORMAL },
	{ "cross punch",    do_cross_punch, POS_FIGHTING,    0,  LOG_NORMAL },
	{ "roundhouse punch",do_roundhouse_punch,POS_FIGHTING,0, LOG_NORMAL },
	{ "uppercut punch", do_uppercut_punch,POS_FIGHTING,  0,  LOG_NORMAL },
	{ "murde",		do_murde,	POS_FIGHTING,	 5,  LOG_NORMAL	},
	{ "murder",		do_murder,	POS_FIGHTING,	 5,  LOG_ALWAYS	},
	{ "nerve",		do_nerve,	POS_FIGHTING,	 0,  LOG_NORMAL },
	{ "rescue",		do_rescue,	POS_FIGHTING,	 0,  LOG_NORMAL	},
	{ "throw",	        do_throw,	POS_STANDING,    0,  LOG_NORMAL },
	{ "trip",		do_trip,	POS_FIGHTING,	 0,  LOG_NORMAL },
	{ "drain life",	do_drain_life,	POS_FIGHTING,	 0,  LOG_NORMAL	},
	{ "mental drain",   do_mental_drain,POS_FIGHTING,    0,  LOG_NORMAL },
	{ "shriek",		do_shriek,	POS_FIGHTING,	 0,  LOG_NORMAL },
	{ "snatch",		do_snatch,	POS_FIGHTING,	 0,  LOG_NORMAL },
	{ "stun",           do_stun,        POS_FIGHTING,    0,  LOG_NORMAL },
	{ "berserk",        do_berserk,     POS_FIGHTING,    0,  LOG_NORMAL },
	{ "soulstrike",     do_soulstrike,  POS_FIGHTING,    0,  LOG_NORMAL },
	{ "multiburst",	do_multiburst,	POS_FIGHTING,	 0,  LOG_NORMAL },
	{ "bite",		do_bite,	POS_FIGHTING,	 0,  LOG_NORMAL },
	{ "howl of fear",	do_howl_of_fear,POS_FIGHTING,	 0,  LOG_NORMAL },
	{ "frenzy",		do_frenzy,	POS_FIGHTING,	 0,  LOG_NORMAL },

	/*
	 * Miscellaneous commands.
	 */
	{ "account",        do_account,     POS_RESTING,     0,  LOG_NORMAL },
	{ "adrenaline rush",do_rush,	POS_RESTING,	 0,  LOG_NORMAL },
	{ "afk",		do_afk,		POS_DEAD,	 0,  LOG_NORMAL },
	{ "afkmes",         do_afkmes,      POS_DEAD,        0,  LOG_NORMAL },
	{ "bash door",      do_bash,        POS_STANDING,    0,  LOG_NORMAL },
	{ "chameleon power",do_chameleon,   POS_STANDING,    0,  LOG_NORMAL },
	{ "follow",		do_follow,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "group",		do_group,	POS_SLEEPING,	 0,  LOG_NORMAL	},
	{ "heighten senses",do_heighten,    POS_STANDING,    0,  LOG_NORMAL },
	{ "enhance reflexes",do_reflex,	POS_STANDING,	 0,  LOG_NORMAL },
	{ "hide",		do_hide,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "iron grip",      do_irongrip,    POS_STANDING,    0,  LOG_NORMAL },
	{ "ironfist",	do_ironfist,	POS_STANDING,	 0,  LOG_NORMAL },
	{ "poison weapon",	do_poison_weapon,POS_SLEEPING,	 0,  LOG_NORMAL	},
	{ "practice",	do_practice,	POS_SLEEPING,	 0,  LOG_NORMAL	},
	{ "qui",		do_qui,		POS_DEAD,	 0,  LOG_NORMAL	},
	{ "quit",		do_quit,	POS_DEAD,	 0,  LOG_NORMAL	},
	{ "rage",		do_rage,	POS_DEAD,	 0,  LOG_NORMAL },
	{ "remote",		do_remote,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "<",		do_remote,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "save",		do_save,	POS_DEAD,	 1,  LOG_NORMAL	},
	{ "shadow",         do_shadow_walk, POS_STANDING,    0,  LOG_NORMAL },
	{ "sleep",		do_sleep,	POS_SLEEPING,	 0,  LOG_NORMAL	},
	{ "sneak",		do_sneak,	POS_STANDING,	 0,  LOG_NORMAL	},
	{ "spells",         do_spells,      POS_SLEEPING,    L_IMP, LOG_NORMAL},
	{ "split",		do_split,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "steal",		do_steal,	POS_STANDING,	 0,  LOG_NORMAL	},
	{ "train",		do_train,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "visible",	do_visible,	POS_SLEEPING,	 0,  LOG_NORMAL },
	{ "wake",		do_wake,	POS_SLEEPING,	 0,  LOG_NORMAL	},
	{ "ward",		do_ward,    POS_STANDING, 1,  LOG_ALWAYS },
	{ "where",		do_where,	POS_RESTING,	 0,  LOG_NORMAL	},
	{ "withdraw",       do_withdraw,    POS_RESTING,     0,  LOG_NORMAL },
	{ "pray",           do_pray,        POS_RESTING,     0,  LOG_NORMAL },
	{ "delet",		do_delet,	POS_STANDING,    0,  LOG_NORMAL },
	{ "delete",		do_delete,	POS_STANDING,    0,  LOG_ALWAYS },
	{ "pkill",		do_pkill,	POS_DEAD,	 0,  LOG_NORMAL },

	/*
	 * Artifactor commands
	 */
	{ "indestructable", do_indestructable, POS_STANDING, 0,  LOG_NORMAL },
	{ "remake",		do_remake,  	POS_STANDING,	 0,  LOG_NORMAL },

	/*
	 * Immortal commands.
	 */

	// { "vnum",           do_vnum,        POS_DEAD,  L_DIR,  LOG_NORMAL },
	// { "load",           do_load,        POS_DEAD,  L_DIR,  LOG_NORMAL },
	{ "advance",        do_advance,     POS_DEAD,  L_IMP,  LOG_ALWAYS },
	{ "allow",          do_allow,       POS_DEAD,  L_DIR,  LOG_ALWAYS },
	{ "astat",          do_astat,       POS_DEAD,  L_JUN,  LOG_NORMAL },
	{ "astrip",         do_astrip,      POS_DEAD,  L_JUN,  LOG_NORMAL },
	{ "at",             do_at,          POS_DEAD,  L_APP,  LOG_NORMAL },
	{ "authorize",      do_authorize,   POS_DEAD,  L_APP,  LOG_ALWAYS },
	{ "bamfmes",        do_bamf,        POS_DEAD,  L_APP,  LOG_NORMAL },
	{ "banlist",	do_banlist,     POS_DEAD,  L_GOD,  LOG_NORMAL },
	{ "bodybag",        do_bodybag,     POS_DEAD,  L_APP,  LOG_ALWAYS },
	{ "bugs", 					do_bugs, 				POS_DEAD,  L_APP,	 LOG_NORMAL },
	{ "changes",	do_changes,	POS_DEAD,  L_DIR,  LOG_NORMAL },
	{ "clandesc",       do_clandesc,    POS_DEAD,  L_SEN,  LOG_NORMAL },
	{ "cloak",          do_cloak,       POS_DEAD,  L_DIR,  LOG_NORMAL },
	{ "clone",		do_clone,       POS_DEAD,  L_SEN,  LOG_NORMAL },
	{ "countcommands",  do_countcommands,POS_DEAD, L_IMP,  LOG_NORMAL },
	{ "cset",           do_cset,        POS_DEAD,  L_IMP,  LOG_BUILD  },
	{ "deny",           do_deny,        POS_DEAD,  L_IMP,  LOG_ALWAYS },
	{ "desc_check",	do_desc_check,	POS_DEAD,  L_IMP,  LOG_NORMAL },
	{ "desc_clean",     do_descript_clean, POS_DEAD, L_IMP,LOG_NORMAL },
	{ "detract",	do_detract,	POS_DEAD,  L_IMP,  LOG_ALWAYS },
	{ "disconnect",     do_disconnect,  POS_DEAD,  L_GOD,  LOG_ALWAYS },
	{ "echo",           do_echo,        POS_DEAD,  L_JUN,  LOG_ALWAYS },
	{ "empower",	do_empower,     POS_DEAD,  L_IMP,  LOG_ALWAYS },
	{ "force",          do_force,       POS_DEAD,  L_DIR,  LOG_ALWAYS },
	{ "freeze",         do_freeze,      POS_DEAD,  L_DIR,  LOG_ALWAYS },
	{ "goto",           do_goto,        POS_DEAD,  L_APP,  LOG_NORMAL },
	{ "hero",           do_hero,        POS_DEAD,    100,    LOG_NORMAL },
	{ "hlist",          do_hlist,       POS_DEAD,    L_APP,  LOG_NORMAL },
	{ "holylight",      do_holylight,   POS_DEAD,  L_APP,  LOG_NORMAL },
	{ "hotreboo",      	do_hotreboo,    POS_DEAD,  L_DIR,  LOG_ALWAYS },
	{ "hotreboot",      do_hotreboot,   POS_DEAD,  L_DIR,  LOG_ALWAYS },
	{ "ideas",          do_ideas,       POS_DEAD,  L_DIR,  LOG_ALWAYS },
	{ "imc",		do_imc,         POS_DEAD,  L_APP,  LOG_NORMAL },
	{ "immtalk",        do_immtalk,     POS_DEAD,  L_APP,  LOG_NORMAL },
	{ "shieldify",      do_shieldify,   POS_STANDING,  L_CON, LOG_NORMAL },
	{ "doubleexp",      do_doubleexp,   POS_DEAD,  L_DIR,  LOG_NORMAL },
	{ "log",            do_log,         POS_DEAD,  L_CON,  LOG_ALWAYS },
	{ "memory",         do_memory,      POS_DEAD,  L_GOD,  LOG_NORMAL },
	{ "mfind",          do_mfind,       POS_DEAD,  L_JUN,  LOG_NORMAL },
	{ "mload",          do_mload,       POS_DEAD,  L_DEM,  LOG_BUILD  },
	{ "mpstat",         do_mpstat,      POS_DEAD,  L_APP,  LOG_NORMAL },
	{ "mpcommands",     do_mpcommands,  POS_DEAD,  L_APP,  LOG_NORMAL },
	{ "mset",           do_mset,        POS_DEAD,  L_DIR,  LOG_BUILD  },
	{ "mstat",          do_mstat,       POS_DEAD,  L_JUN,  LOG_NORMAL },
	{ "mwhere",         do_mwhere,      POS_DEAD,  L_JUN,  LOG_NORMAL },
	{ "newban",		do_newban,      POS_DEAD,  L_GOD,  LOG_ALWAYS },
	{ "newcorpse",	do_newcorpse,   POS_DEAD,  L_GOD,  LOG_ALWAYS },
	{ "newlock",        do_newlock,     POS_DEAD,  L_IMP,  LOG_ALWAYS },
	{ "noemote",        do_noemote,     POS_DEAD,  L_GOD,  LOG_NORMAL },
	{ "notell",         do_notell,      POS_DEAD,  L_GOD,  LOG_NORMAL },
	{ "nukerep",	do_nukerep,	POS_DEAD,  L_GOD,  LOG_NORMAL },
	{ "numlock",        do_numlock,     POS_DEAD,  L_CON,  LOG_ALWAYS },
	{ "ofind",          do_ofind,       POS_DEAD,  L_JUN,  LOG_NORMAL },
	{ "oload",          do_oload,       POS_DEAD,  L_JUN,  LOG_BUILD  },
	{ "opstat",		do_opstat,	POS_DEAD,    L_APP,  LOG_NORMAL },
	{ "oset",           do_oset,        POS_DEAD,  L_DIR,  LOG_BUILD  },
	{ "ostat",          do_ostat,       POS_DEAD,  L_JUN,  LOG_NORMAL },
	{ "owhere",         do_owhere,      POS_DEAD,  L_JUN,  LOG_NORMAL },
	{ "pardon",         do_pardon,      POS_DEAD,  L_IMP,  LOG_ALWAYS },
	{ "peace",          do_peace,       POS_DEAD,  L_DEM,  LOG_NORMAL },
	{ "permban",	do_permban,	    POS_DEAD,  L_DIR,  LOG_ALWAYS },
	{ "plist",	        do_playerlist,  POS_DEAD,    L_APP,  LOG_NORMAL },
	{ "pload",          do_pload,       POS_DEAD,  L_DIR,  LOG_ALWAYS },
	{ "purge",          do_purge,       POS_DEAD,  L_DEM,  LOG_NORMAL },
	{ "qecho",          do_qecho,       POS_DEAD,  L_GOD,  LOG_ALWAYS },
	{ "qset",           do_qset,        POS_DEAD,  L_GOD,  LOG_ALWAYS },
	{ "rebuild",        do_rebuild,	    POS_DEAD,  L_DIR,  LOG_ALWAYS },
	{ "reboo",          do_reboo,       POS_DEAD,  L_DIR,  LOG_NORMAL },
	{ "reboot",         do_reboot,      POS_DEAD,  L_DIR,  LOG_ALWAYS },
	{ "recho",          do_recho,       POS_DEAD,  L_JUN,  LOG_ALWAYS },
	{ "restore",        do_restore,     POS_DEAD,  L_GOD,  LOG_ALWAYS },
	{ "restrict",       do_restrict,    POS_DEAD,  L_GOD,  LOG_NORMAL },
	{ "rpstat",		do_rpstat,	POS_DEAD,  L_APP,  LOG_NORMAL },
	{ "rset",           do_rset,        POS_DEAD,  L_DIR,  LOG_BUILD  },
	{ "rstat",          do_rstat,       POS_DEAD,  L_JUN,  LOG_NORMAL },
	{ "seize",		do_seize,       POS_DEAD,  L_DIR,  LOG_ALWAYS },
	{ "shutdow",        do_shutdow,     POS_DEAD,  L_CON,  LOG_NORMAL },
	{ "shutdown",       do_shutdown,    POS_DEAD,  L_CON,  LOG_ALWAYS },
	{ "silence",        do_silence,     POS_DEAD,  L_GOD,  LOG_NORMAL },
	{ "sla",            do_sla,         POS_DEAD,  L_GOD,  LOG_NORMAL },
	{ "slay",           do_slay,        POS_DEAD,  L_GOD,  LOG_ALWAYS },
	{ "slaymes",        do_slaymes,     POS_DEAD,  L_GOD,  LOG_NORMAL },
	{ "slookup",        do_slookup,     POS_DEAD,  L_DIR,  LOG_NORMAL },
	{ "snoop",          do_snoop,       POS_DEAD,  L_DIR,  LOG_ALWAYS },
	{ "sset",           do_sset,        POS_DEAD,  L_DIR,  LOG_ALWAYS },
	{ "sstat",          do_sstat,       POS_DEAD,  L_DIR,  LOG_NORMAL },
	{ "sstime",         do_sstime,      POS_DEAD,  L_CON,  LOG_ALWAYS },
	{ "tempban",	do_tempban,     POS_DEAD,  L_GOD,  LOG_ALWAYS },
	{ "todo",           do_todo,        POS_DEAD,  L_DIR,  LOG_ALWAYS },
	{ "transfer",       do_transfer,    POS_DEAD,  L_JUN,  LOG_ALWAYS },
	{ "transmes",       do_trmes,       POS_DEAD,  L_JUN,  LOG_NORMAL },
	{ "trust",          do_trust,       POS_DEAD,  L_IMP,  LOG_ALWAYS },
	{ "typos",          do_typos,       POS_DEAD,  L_SEN,  LOG_ALWAYS },
	{ "users",          do_users,       POS_DEAD,  L_DIR,  LOG_NORMAL },
	{ "vused",          do_vused,       POS_DEAD,  L_DIR,  LOG_NORMAL },
	{ "whotype",	do_whotype,	POS_DEAD,    L_CON,  LOG_NORMAL },
	{ "wizify",         do_wizify,      POS_DEAD,  L_CON,  LOG_ALWAYS },
	{ "wizlock",        do_wizlock,     POS_DEAD,  L_IMP,  LOG_ALWAYS },
	{ "wizinvis",       do_invis,       POS_DEAD,  L_DIR,  LOG_NORMAL },
	{ "wiznet",		do_wiznet,      POS_DEAD,  L_JUN,  LOG_NORMAL },
	{ "wrlist",         do_wrlist,      POS_DEAD,  L_JUN,  LOG_NORMAL },
	{ ":",              do_immtalk,     POS_DEAD,  L_APP,  LOG_NORMAL },
	{ "=",              do_imc,         POS_DEAD,  L_APP,  LOG_NORMAL },
	{ "-",              do_hero,        POS_DEAD,    100,    LOG_NORMAL },

	// { "fighting",        do_fighting,    POS_DEAD,  L_GOD,  LOG_NORMAL },
	{ "switch",       do_switch,      POS_DEAD,  L_DIR,  LOG_ALWAYS },
	{ "return",       do_return,      POS_DEAD,  L_DIR,  LOG_NORMAL },
	// { "pwhere",           do_pwhere,      POS_DEAD,    105,  LOG_NORMAL },
	// { "bamfin",         do_bamfin,      POS_DEAD,  L_APP,  LOG_NORMAL },
	// { "bamfout",        do_bamfout,     POS_DEAD,  L_APP,  LOG_NORMAL },

	/*
	 * Here because they interefeared with something
	 */
	{ "gouge",          do_gouge,      POS_FIGHTING, 0,      LOG_NORMAL },
	{ "circle",         do_circle,     POS_FIGHTING, 0,      LOG_NORMAL },
	{ "rake",           do_rake,       POS_FIGHTING, 0,      LOG_NORMAL },
	// { "transport",      do_transport,  POS_RESTING,      0,  LOG_NORMAL },
	// { "immlist",	do_immlist,	POS_DEAD, 	 0,  LOG_NORMAL },

	/*
	 * OLC 1.1b
	 */
	{ "aedit",		do_aedit,	POS_DEAD, L_IMP,  LOG_BUILD  },
	{ "cedit",          do_cedit,       POS_DEAD, L_CON,  LOG_BUILD  },
	{ "hedit",          do_hedit,       POS_DEAD, L_APP,  LOG_BUILD  },
	{ "redit",		do_redit,	POS_DEAD, L_CON,  LOG_BUILD  },
	{ "oedit",		do_oedit,	POS_DEAD, L_IMP,  LOG_BUILD  },
	{ "medit",		do_medit,	POS_DEAD, L_IMP,  LOG_BUILD  },
	{ "asave",          do_asave,	POS_DEAD, L_APP,  LOG_NORMAL },
	{ "alist",		do_alist,	POS_DEAD, L_CON,  LOG_NORMAL },
	{ "resets",		do_resets,	POS_DEAD, L_CON,  LOG_NORMAL },
	{ "mreset",		do_mreset,	POS_DEAD, L_CON,  LOG_NORMAL },

	{ "sedit",		do_sedit,	POS_DEAD, L_CON,  LOG_BUILD  },
	{ "spedit",		do_spedit,	POS_DEAD, L_IMP,  LOG_ALWAYS },
	{ "rename_obj",	do_rename_obj,	POS_DEAD, L_APP,  LOG_ALWAYS },
	{ "nedit",		do_nedit,	POS_DEAD, L_CON,  LOG_BUILD  },
	/*
	 * End of list.
	 */
	{ "",		0,		POS_DEAD,	 0,  LOG_NORMAL	}
};

/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
void interpret( CHAR_DATA *ch, char *argument )
{
	char command [ MAX_INPUT_LENGTH ];
	char logline [ MAX_INPUT_LENGTH ];
	int  cmd;
	int  trust;
	bool found;

	if ( ch->wait > 0 )
		return;

	/*
	 * Strip leading spaces.
	 */
	while ( isspace( *argument ) )
		argument++;
	if ( argument[0] == '\0' )
		return;

	/*
	 * Implement freeze command.
	 */
	if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_FREEZE ) )
	{
		send_to_char(AT_LBLUE, "You're totally frozen!\n\r", ch );
		return;
	}

	/*
	 * Implement stun.
	 */
	if ( IS_STUNNED( ch, STUN_COMMAND ) ||
			IS_STUNNED( ch, STUN_TOTAL ) )
	{
		send_to_char(AT_LBLUE, "You're too stunned to do anything!\n\r", ch );
		return;
	}

	/*
	 * Grab the command word.
	 * Special parsing so ' can be a command,
	 *   also no spaces needed after punctuation.
	 */
	strcpy( logline, argument );
	if ( !isalpha( argument[0] ) && !isdigit( argument[0] ) )
	{
		command[0] = argument[0];
		command[1] = '\0';
		argument++;
		while ( isspace( *argument ) )
			argument++;
	}
	else
	{
		argument = one_argument( argument, command );
	}

	/*
	 * Look for command in command table.
	 */
	found = FALSE;
	trust = get_trust( ch );
	for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
	{
		if ( command[0] == cmd_table[cmd].name[0]
				&& !str_prefix( command, cmd_table[cmd].name )
				&& can_use_cmd(cmd, ch, trust) )
		{
			found = TRUE;
			break;
		}
	}

	/*
	 * Log and snoop.
	 */
	if ( cmd_table[cmd].log == LOG_NEVER )
		strcpy( logline, "XXXXXXXX XXXXXXXX XXXXXXXX" );


	/*
	 * Builder Logs.
	 * Added by Altrag.
	 */
	if ( cmd_table[cmd].log == LOG_BUILD )
	{
		sprintf( log_buf, "%s: %s", ch->name, logline );
		log_string( log_buf, CHANNEL_BUILD , get_trust( ch ) );
		wiznet( log_buf, ch, NULL, WIZ_SECURE, 0, get_trust( ch ) );
	}

	if ( ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_LOG ) )
			|| fLogAll
			|| cmd_table[cmd].log == LOG_ALWAYS )
	{
		sprintf( log_buf, "%s: %s", ch->name, logline );
		log_string( log_buf, ( ch->level == L_IMP ? CHANNEL_NONE : CHANNEL_GOD ),
				( ( ch->level > 100 ) ? get_trust( ch ) : -1 ) );
		wiznet( log_buf, ch, NULL, WIZ_SECURE, 0, get_trust( ch ) );
	}

	if ( ch->desc && ch->desc->snoop_by )
	{
		sprintf( log_buf, "%s%%", ch->name );
		write_to_buffer( ch->desc->snoop_by, log_buf,    0 );
		write_to_buffer( ch->desc->snoop_by, logline, 0 );
		write_to_buffer( ch->desc->snoop_by, "\n\r",  2 );
	}

	/*
	 *  Hiding stuff...  - Decklarean
	 */
	if ( IS_AFFECTED(ch,AFF_HIDE) )
	{     /*Things you can do while hiding*/
		if (            str_cmp( cmd_table[cmd].name, "pick"     ) &&
				str_cmp( cmd_table[cmd].name, "chameleon power") &&
				str_cmp( cmd_table[cmd].name, "save"     ) &&
				str_cmp( cmd_table[cmd].name, "visible"  ) &&
				str_cmp( cmd_table[cmd].name, "inventory") &&
				str_cmp( cmd_table[cmd].name, "equipment") &&
				str_cmp( cmd_table[cmd].name, "where"    ) &&
				str_cmp( cmd_table[cmd].name, "who"      ) &&
				str_cmp( cmd_table[cmd].name, "open"     ) &&
				str_cmp( cmd_table[cmd].name, "hide"     ) &&
				str_cmp( cmd_table[cmd].name, "sleep"    ) &&
				str_cmp( cmd_table[cmd].name, "stand"    ) &&
				str_cmp( cmd_table[cmd].name, "rest"     ) &&
				str_cmp( cmd_table[cmd].name, "peek"     ) &&
				str_cmp( cmd_table[cmd].name, "shadow"   ) &&
				str_cmp( cmd_table[cmd].name, "sneak"    ) &&
				str_cmp( cmd_table[cmd].name, "worth"    ) &&
				str_cmp( cmd_table[cmd].name, "affected" ) &&
				str_cmp( cmd_table[cmd].name, "look"     ) &&
				str_cmp( cmd_table[cmd].name, "tell"     ) &&
				str_cmp( cmd_table[cmd].name, "score"    )    )
				{
					/*If you are sneaking you can move these directions while hiding*/
					if ( IS_AFFECTED(ch,AFF_SNEAK) &&
							( !str_cmp( cmd_table[cmd].name, "north" ) ||
							  !str_cmp( cmd_table[cmd].name, "east"  ) ||
							  !str_cmp( cmd_table[cmd].name, "south" ) ||
							  !str_cmp( cmd_table[cmd].name, "west"  ) ||
							  !str_cmp( cmd_table[cmd].name, "up"    ) ||
							  !str_cmp( cmd_table[cmd].name, "down"  )    ) )
					{
						/* check and see if they are chameleon and remove it if they
						   are trying to move*/
						if ( is_affected(ch, gsn_chameleon) )
						{
							send_to_char(AT_BLUE, "You return to your normal coloration.\n\r", ch );
							affect_strip ( ch, gsn_chameleon );
						}
						/* If they aren't hidding remove the hide bit */
						if ( !is_affected(ch, gsn_hide ) )
							REMOVE_BIT( ch->affected_by, AFF_HIDE );
					}
					else
					{  /* remove all the hidding stuff */
						if ( is_affected(ch, gsn_chameleon) )
						{
							send_to_char(AT_BLUE, "You return to your normal coloration.\n\r", ch );
							affect_strip ( ch, gsn_chameleon);
						}
						if ( is_affected(ch, gsn_hide) )
						{
							send_to_char(AT_BLUE, "You stop hiding.\n\r", ch);
							affect_strip ( ch, gsn_hide );
						}
						REMOVE_BIT( ch->affected_by, AFF_HIDE );
					}
				}
	}


	if ( IS_AFFECTED2(ch,AFF_BERSERK) && trust < LEVEL_IMMORTAL)
	{
		if ( !found || (str_cmp( cmd_table[cmd].name, "flee" ) &&
					str_cmp( cmd_table[cmd].name, "look" ) &&
					str_cmp( cmd_table[cmd].name, "get"  ) &&
					str_cmp( cmd_table[cmd].name, "wield") &&
					str_cmp( cmd_table[cmd].name, "dual" ) &&
					str_cmp( cmd_table[cmd].name, "score")) )
		{
			send_to_char( AT_BLOOD, "You cannot do that in such a rage!\n\r",ch);
			return;
		}
	}

	if ( !found )
	{
		/*
		 * Look for command in socials table.
		 */
		if ( IS_NPC(ch) || !check_alias( ch, command, argument ) )
			if ( !check_social( ch, command, argument ) )
				send_to_char(C_DEFAULT, "Huh?\n\r", ch );
		return;
	}

	/*
	 * Character not in position for command?
	 */
	if ( ch->position < cmd_table[cmd].position )
	{
		switch( ch->position )
		{
			case POS_DEAD:
				send_to_char(AT_RED, "Lie still; you are DEAD.\n\r", ch );
				break;

			case POS_MORTAL:
			case POS_INCAP:
				send_to_char(AT_RED, "You are hurt far too bad for that.\n\r", ch );
				break;

			case POS_STUNNED:
				send_to_char(AT_WHITE, "You are too stunned to do that.\n\r",    ch );
				break;

			case POS_SLEEPING:
				send_to_char(AT_BLUE, "In your dreams, or what?\n\r",           ch );
				break;

			case POS_RESTING:
				send_to_char(AT_BLUE, "Nah... You feel too relaxed...\n\r",     ch );
				break;

			case POS_FIGHTING:
				send_to_char(AT_BLOOD, "No way!  You are still fighting!\n\r",   ch );
				break;

		}
		return;
	}
	/*
	   if ( IS_NPC(ch) )
	   {
	   char combuf [MAX_STRING_LENGTH];
	   char filname[MAX_INPUT_LENGTH];
	   extern int port;

	   sprintf(combuf, "%s: [%d] %s %s\n\r", ctime( &current_time), ch->pIndexData->vnum, command, argument);
	   sprintf(filname, "comlog%d.txt", port );
	   append_file( ch, filname, combuf );
	   }
	   */
	/*
	 * Dispatch the command.
	 */
	if ( IS_SET( ch->act, PLR_AFK ) )  /* if player was afk, remove afk */
	{
		REMOVE_BIT( ch->act, PLR_AFK );
		send_to_char( AT_YELLOW, "You are no longer AFK.\n\r", ch );
	}

	(*cmd_table[cmd].do_fun) ( ch, argument );
	tail_chain( );
	return;
}

bool can_use_cmd( int cmd, CHAR_DATA *ch, int trust )
{
	if ( trust <= 0 )
		trust = get_trust(ch);
	if ( !IS_NPC(ch) )
	{
		if ( is_name(NULL, cmd_table[cmd].name, ch->pcdata->empowerments) )
			return TRUE;
		if ( is_name(NULL, cmd_table[cmd].name, ch->pcdata->detractments) )
			return FALSE;
	}
	return (cmd_table[cmd].level <= trust);
}

bool check_alias( CHAR_DATA *ch, char *command, char *argument )
{
	ALIAS_DATA *al;
	char arg[MAX_STRING_LENGTH];
	char newarg[MAX_STRING_LENGTH];
	bool found;

	arg[0] = '\0';
	newarg[0] = '\0';

	/*   if ( argument[0] != '\0' )
		 argument = one_argument( argument, arg );*/
	while ( isspace(*argument) )
		argument++;
	strcpy( arg, argument );

	if ( IS_NPC( ch ) )
		return FALSE;

	if ( !(ch->pcdata->alias_list) )
		return FALSE;

	found = FALSE;

	for ( al = ch->pcdata->alias_list; al; al = al->next )
	{
		if ( !str_cmp( al->old, command ) )
		{
			/*       if ( arg[0] != '\0' )
					 {*/
			strcpy( newarg, al->new );
			strcat( newarg, " " );
			strcat( newarg, arg );
			interpret( ch, newarg );
			/*       }
					 else
					 interpret( ch, al->new );*/

			found = TRUE;
			break;
		}
	}
	return found;
}


bool check_social( CHAR_DATA *ch, char *command, char *argument )
{
	CHAR_DATA *victim;
	char       arg [ MAX_INPUT_LENGTH ];
	char       buf [ MAX_STRING_LENGTH ];

	/*    int        cmd;*/
	bool       found;
	SOCIAL_DATA *pSocial;

	found  = FALSE;
	/*
	   for ( cmd = 0; social_table[cmd].name[0] != '\0'; cmd++ )
	   {
	   if ( command[0] == social_table[cmd].name[0]
	   && !str_prefix( command, social_table[cmd].name ) )
	   {
	   found = TRUE;
	   break;
	   }
	   } */

	for(pSocial = social_first;pSocial;pSocial = pSocial->next)
	{
		if ( command[0] == pSocial->name[0]
				&& !str_prefix( command, pSocial->name) )
		{
			found = TRUE;
			break;
		}
	}

	if ( !found )
		return FALSE;

	if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_NO_EMOTE ) )
	{
		send_to_char(AT_LBLUE, "You are anti-social!\n\r", ch );
		return TRUE;
	}

	switch ( ch->position )
	{
		case POS_DEAD:
			send_to_char(AT_RED, "Lie still; you are DEAD.\n\r",             ch );
			return TRUE;

		case POS_INCAP:
		case POS_MORTAL:
			send_to_char(AT_RED, "You are hurt far too badly for that.\n\r", ch );
			return TRUE;

		case POS_STUNNED:
			send_to_char(AT_WHITE, "You are too stunned to do that.\n\r",      ch );
			return TRUE;

		case POS_SLEEPING:
			/*
			 * I just know this is the path to a 12" 'if' statement.  :(
			 * But two players asked for it already!  -- Furey
			 */
			if ( !str_cmp( pSocial->name, "snore" ) )
				break;
			send_to_char(AT_BLUE, "In your dreams, or what?\n\r",             ch );
			return TRUE;

	}

	one_argument( argument, arg );
	victim = NULL;
	if ( arg[0] == '\0' )
	{
		act(AT_PINK, pSocial->char_no_arg,   ch, NULL, victim, TO_CHAR);
		act(AT_PINK, pSocial->others_no_arg, ch, NULL, victim, TO_ROOM);
	}
	else if ( !( victim = get_char_room( ch, arg ) ) )
	{
		if ( !( victim = get_char_world( ch, arg ) ) )
			send_to_char(AT_WHITE, "They aren't here.\n\r",                    ch );
		else {
			if ((ch->in_room->vnum == ROOM_VNUM_HELL) &&
					(ch->level < LEVEL_IMMORTAL))
			{
				send_to_char(AT_WHITE, "What do you think this is, a vacation?\n\r" ,ch);
			}
			else
			{
				sprintf( buf, "From a great distance, %s", pSocial->char_found);
				act(AT_PINK, buf, ch, NULL, victim, TO_CHAR);
				sprintf( buf, "From a great distance, %s", pSocial->vict_found);
				act(AT_PINK, buf, ch, NULL, victim, TO_VICT);
				sprintf( buf, "From a great distance, %s", pSocial->others_found );
				act(AT_PINK, buf, ch, NULL, victim, TO_NOTVICT);
			}

		}

	}
	else if ( victim == ch )
	{
		act(AT_PINK, pSocial->char_auto,     ch, NULL, victim, TO_CHAR);
		act(AT_PINK, pSocial->others_auto,   ch, NULL, victim, TO_ROOM);
	}
	else
	{
		act(AT_PINK, pSocial->char_found,    ch, NULL, victim, TO_CHAR);
		act(AT_PINK, pSocial->vict_found,    ch, NULL, victim, TO_VICT);
		act(AT_PINK, pSocial->others_found,  ch, NULL, victim, TO_NOTVICT);

		if ( !IS_NPC( ch )
				&& IS_NPC( victim )
				&& !IS_AFFECTED( victim, AFF_CHARM )
				&& IS_AWAKE( victim )
				&& ( !victim->pIndexData->mobprogs ) )

		{
			switch ( number_bits( 4 ) )
			{
				case 0:
					if ( ( victim->level < ch->level )
							&& !( victim->fighting ) )
						multi_hit( victim, ch, TYPE_UNDEFINED );
					break;

				case 1: case 2: case 3: case 4:
				case 5: case 6: case 7: case 8:
					act(AT_PINK, pSocial->char_found,
							victim, NULL, ch, TO_CHAR    );
					act(AT_PINK, pSocial->vict_found,
							victim, NULL, ch, TO_VICT    );
					act(AT_PINK, pSocial->others_found,
							victim, NULL, ch, TO_NOTVICT );
					break;

				case 9: case 10: case 11: case 12:
					act(AT_PINK, "You slap $N.",  victim, NULL, ch, TO_CHAR    );
					act(AT_PINK, "$n slaps you.", victim, NULL, ch, TO_VICT    );
					act(AT_PINK, "$n slaps $N.",  victim, NULL, ch, TO_NOTVICT );
					break;
			}
		}
	}

	return TRUE;
}



/*
 * Return true if an argument is completely numeric.
 */
bool is_number( char *arg )
{
	if ( *arg == '\0' )
		return FALSE;

	if ( *arg == '+' || *arg == '-' )
		arg++;

	for ( ; *arg != '\0'; arg++ )
	{
		if ( !isdigit( *arg ) )
			return FALSE;
	}

	return TRUE;
}



/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument( char *argument, char *arg )
{
	char *pdot;
	int   number;

	for ( pdot = argument; *pdot != '\0'; pdot++ )
	{
		if ( *pdot == '.' )
		{
			*pdot = '\0';
			number = atoi( argument );
			*pdot = '.';
			strcpy( arg, pdot+1 );
			return number;
		}
	}

	strcpy( arg, argument );
	return 1;
}



/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_argument( char *argument, char *arg_first )
{
	char cEnd;

	while ( isspace( *argument ) )
		argument++;

	cEnd = ' ';

	if ( *argument == '\'' ||
			*argument == '"' )
		cEnd = *argument++;

	while ( *argument != '\0' )
	{
		if ( *argument == cEnd )
		{
			argument++;
			break;
		}
		*arg_first = LOWER( *argument );
		arg_first++;
		argument++;
	}
	*arg_first = '\0';

	while ( isspace( *argument ) )
		argument++;

	return argument;
}

void do_clear( CHAR_DATA *ch, char *argument )
{
	ALIAS_DATA *pA1;
	ALIAS_DATA *pA1_next;

	if( !ch->pcdata->alias_list ) {
		send_to_char( AT_WHITE, "You have no alias' to clear.\n\r", ch );
		return;
	}

	for ( pA1 = ch->pcdata->alias_list; pA1; pA1 = pA1_next ) {
		pA1_next = pA1->next;
		free_alias(pA1);
	}
	send_to_char( AT_WHITE, "All aliases have been erased.\n\r", ch );
	ch->pcdata->alias_list = NULL;
	return;

}

void do_alias( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_STRING_LENGTH];
	char arg1[MAX_STRING_LENGTH];
	ALIAS_DATA *al;
	ALIAS_DATA *oAl;

	smash_tilde( argument );
	argument = one_argument( argument, arg );
	strcpy( arg1, argument );

	if ( arg[0] == '\0' ) {
		if ( !ch->pcdata->alias_list ) {
			send_to_char( AT_WHITE, "You have no alias' set.\n\r", ch );
			return;
		}

		send_to_char( AT_WHITE, "Your currently set alias' are:\n\r", ch );

		for ( al = ch->pcdata->alias_list; al; al = al->next ) {
			sprintf( log_buf, "  %-10s   &B%s&w\n\r", al->old, al->new );
			send_to_char( AT_RED, log_buf, ch );
		}
		send_to_char( AT_WHITE, "\n\r", ch );
		return;
	}

	if ( arg1[0] == '\0' && ( !( ch->pcdata->alias_list ) ) ) {
		send_to_char( AT_WHITE, "You have no command alias' to remove!\n\r", ch );
		return;
	}

	if ( arg1[0] != '\0' ) {
		for ( oAl = ch->pcdata->alias_list; oAl; oAl = oAl->next ) {
			if ( is_name( NULL, arg1, oAl->old ) || !str_prefix( arg, arg1 ) ) {
				send_to_char( AT_WHITE, "You cannot alias other alias' into your [new] string.\n\r", ch );
				return;
			}
		}
	}

	for ( al = ch->pcdata->alias_list; al; al = al->next ) {
		if ( !str_cmp( al->old, arg ) ) {
			break;
		}
	}

	if ( arg1[0] == '\0' ) {
		if ( !al ) {
			send_to_char( C_DEFAULT, "Alias does not exist.\n\r", ch );
			return;
		}

		if ( al == ch->pcdata->alias_list ) {
			ch->pcdata->alias_list = al->next;
		} else {
			for ( oAl = ch->pcdata->alias_list; oAl; oAl = oAl->next ){
				if ( oAl->next == al ) {
					break;
				}
			}

			if ( !oAl ) {
				sprintf( log_buf, "Do_alias: bad alias - ch (%s) - [%s]", ch->name, arg );
				bug( log_buf, 0 );
				send_to_char(C_DEFAULT, "Alias does not exist.\n\r", ch );
				return;
			}

			oAl->next = al->next;
		}

		free_alias( al );
		act( AT_WHITE, "Alias '$t' cleared.\n\r", ch, arg, NULL, TO_CHAR );
		return;
	}

	if ( al ) {
		free_string( al->new );
		al->new = str_dup( arg1 );
		sprintf( log_buf, "Alias '%s' remade.\n\r", arg );
		send_to_char( AT_WHITE, log_buf, ch );
		return;
	}

	add_alias( ch, al, arg, arg1 );
	sprintf( log_buf, "Alias '%s' added.\n\r", arg );
	send_to_char( AT_WHITE, log_buf, ch );
	return;
}

void add_alias( CHAR_DATA *ch, ALIAS_DATA *pAl, char *old, char *new )
{
	pAl = alloc_perm( sizeof( *pAl ) );
	pAl->old = str_dup( old );
	pAl->new = str_dup( new );
	pAl->next = ch->pcdata->alias_list;  ch->pcdata->alias_list = pAl;
	return;
}

void do_countcommands( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];
	int scnt;
	int skill[MAX_CLASS];
	int spell[MAX_CLASS];
	int iclass;

	for ( iclass = 0; iclass < MAX_CLASS; iclass++ )
	{
		skill[iclass] = 0;
		spell[iclass] = 0;
	}

	for ( scnt = 0; skill_table[scnt].name[0] != '\0'; scnt++ )
	{
		for ( iclass = 0; iclass < MAX_CLASS; iclass++ )
		{
			if ( skill_table[scnt].skill_level[iclass] < L_APP )
			{
				if ( skill_table[scnt].spell_fun == spell_null )
					skill[iclass]++;
				else
					spell[iclass]++;
			}
		}
	}

	sprintf(buf, "Command table size: %d\n\r"
			"Social table size:  %d\n\r"
			"GSkill table size:  %d\n\r"
			"Skill table size:   %d\n\r",
			/* -1 from each for blank index entry at end. */
			(int) (sizeof(   cmd_table) / sizeof(   cmd_table[0]) - 1),
			top_social,
			/*sizeof(social_table) / sizeof(social_table[0]) -1,*/
			(int) (sizeof(gskill_table) / sizeof(gskill_table[0])),
			scnt); /* Someone wanna explain why theres an error with
					  sizeof(skill_table)?? */
	send_to_char(AT_PINK, buf, ch );
	for ( iclass = 0; iclass < MAX_CLASS; iclass++ )
	{
		sprintf( buf, "%-15s: Skills %2d + Spells %2d = Total %2d\n\r",
				class_table[iclass].who_long,
				skill[iclass], spell[iclass], skill[iclass]+spell[iclass] );
		send_to_char(AT_PINK, buf, ch );
	}
	return;
}
void do_afk( CHAR_DATA *ch, char *argument )
{
	if ( IS_NPC( ch ) )
		return;
	send_to_char( AT_YELLOW, "You announce that you are going AFK.\n\r",
			ch );
	act( AT_YELLOW, "$n announces, 'I am going AFK.'", ch, NULL, NULL,
			TO_ROOM );
	SET_BIT( ch->act, PLR_AFK );
	return;
}

void do_pkill( CHAR_DATA *ch, char *argument )
{
	if ( IS_NPC( ch ) )
		return;

	if(argument[0] == '\0' )
	{
		send_to_char( AT_WHITE,
				"Syntax: &Rpkill yes&W    To choose to become a PKILLER forever!\n\r", ch );
		send_to_char( AT_WHITE, "This is NOT a reversible choice. Type HELP PK.\n\r", ch );
		return;
	}
	if (!str_cmp( argument, "yes" ) )
	{
		if ( ch->level < 20 )
		{
			send_to_char( AT_WHITE, "You must be at least level 20 to choose the path of PKILL.\n\r", ch );
			return;
		}
		if ( IS_SET( ch->act, PLR_PKILLER ) )
		{
			send_to_char( AT_WHITE, "You have already chosen the path of pkilling.\n\r", ch );
			return;
		}
		send_to_char( AT_RED,
				"You are now a PKILLER! Beware, the path you have chosen is not one of the weak minded.\n\r", ch);
		act( AT_RED,
				"$n has made a lifelong decision, to join forces with Pkillers throughout the Storm!",
				ch , NULL, NULL, TO_ROOM );
		SET_BIT( ch->act, PLR_PKILLER );
		do_help( ch, "pk" );
		return;
	}
	else
	{
		send_to_char( AT_WHITE,
				"Syntax: &Rpkill yes&W    To choose to become a PKILLER forever!\n\r", ch );
		send_to_char( AT_WHITE, "This is NOT a reversible choice. Type HELP PK.\n\r", ch );
		return;
	}

}
