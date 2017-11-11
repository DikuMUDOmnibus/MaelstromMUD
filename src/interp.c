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

bool check_social( CHAR_DATA * ch, char * command, char * argument );
bool check_alias( CHAR_DATA * ch, char * command, char * argument );
void add_alias( CHAR_DATA * ch, ALIAS_DATA * pAl, char * old, char * new );
bool can_use_cmd( int cmd, CHAR_DATA * ch, int trust );

/*
 * Command logging types.
 */
#define LOG_NORMAL 0
#define LOG_ALWAYS 1
#define LOG_NEVER  2
#define LOG_BUILD  3

/*
 * Log-all switch.
 */
bool fLogAll = FALSE;

/*
 * Command table.
 */
struct cmd_type cmd_table [] = {
  { "north",           do_north,             POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "east",            do_east,              POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "south",           do_south,             POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "west",            do_west,              POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "up",              do_up,                POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "down",            do_down,              POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "northwest",       do_northwest,         POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "northeast",       do_northeast,         POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "southwest",       do_southwest,         POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "southeast",       do_southeast,         POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "nw",              do_northwest,         POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "ne",              do_northeast,         POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "sw",              do_southwest,         POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "se",              do_southeast,         POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "push",            do_push,              POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "drag",            do_drag,              POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "accept",          do_accept,            POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "buy",             do_buy,               POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "bug",             do_bug,               POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "declare",         do_declare,           POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "cast",            do_cast,              POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "challenge",       do_challenge,         POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "deposit",         do_deposit,           POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "dual",            do_dual,              POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "exits",           do_exits,             POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "flip",            do_flip,              POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "get",             do_get,               POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "palm",            do_palm,              POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "inventory",       do_inventory,         POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "identify",        do_identify,          POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "invoke",          do_invoke,            POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "kill",            do_kill,              POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "look",            do_look,              POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "lure",            do_lure,              POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "order",           do_order,             POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "rest",            do_rest,              POS_SLEEPING, 0,     LOG_NORMAL, TRUE    },
  { "repair",          do_repair,            POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "sleep",           do_sleep,             POS_SLEEPING, 0,     LOG_NORMAL, TRUE    },
  { "stand",           do_stand,             POS_SLEEPING, 0,     LOG_NORMAL, TRUE    },
  { "tell",            do_tell,              POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "track",           do_track,             POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "war ",            do_war,               POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "wield",           do_wear,              POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "wizhelp",         do_wizhelp,           POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "mpasound",        do_mpasound,          POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "mpjunk",          do_mpjunk,            POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "mpecho",          do_mpecho,            POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "mpechoat",        do_mpechoat,          POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "mpechoaround",    do_mpechoaround,      POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "mpkill",          do_mpkill,            POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "mpmload",         do_mpmload,           POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "mpoload",         do_mpoload,           POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "mppurge",         do_mppurge,           POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "mpgoto",          do_mpgoto,            POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "mpat",            do_mpat,              POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "mptransfer",      do_mptransfer,        POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "mpforce",         do_mpforce,           POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "mpteleport",      do_mpteleport,        POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "affected",        do_affectedby,        POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "raffected",       do_raffect,           POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "areas",           do_areas,             POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "commands",        do_commands,          POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "compare",         do_compare,           POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "consider",        do_consider,          POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "credits",         do_credits,           POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "enter",           do_enter,             POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "equipment",       do_equipment,         POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "examine",         do_examine,           POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "finger",          do_finger,            POS_DEAD,     0,     LOG_ALWAYS, TRUE    },
  { "forge",           do_forge,             POS_STANDING, 30,    LOG_ALWAYS, TRUE    },
  { "help",            do_help,              POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "idea",            do_idea,              POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "report",          do_report,            POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "pagelength",      do_pagelen,           POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "read",            do_look,              POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "scan",            do_scan,              POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "score",           do_score,             POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "scent",           do_scent,             POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "slist",           do_slist,             POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "socials",         do_socials,           POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "time",            do_time,              POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "typo",            do_typo,              POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "weather",         do_weather,           POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "worth",           do_worth,             POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "who",             do_who,               POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "wizlist",         do_immlist,           POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "champlist",       do_champlist,         POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "newbie",          do_newbie,            POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "induct",          do_induct,            POS_STANDING, 0,     LOG_ALWAYS, TRUE    },
  { "outcast",         do_outcast,           POS_STANDING, 0,     LOG_ALWAYS, TRUE    },
  { "setlev",          do_setlev,            POS_STANDING, 0,     LOG_ALWAYS, TRUE    },
  { "cinfo",           do_cinfo,             POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "cquest",          do_clanquest,         POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "alias",           do_alias,             POS_DEAD,     0,     LOG_NORMAL, FALSE   },
  { "auto",            do_auto,              POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "autoexit",        do_autoexit,          POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "autoloot",        do_autoloot,          POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "autocoins",       do_autocoins,         POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "autosac",         do_autosac,           POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "blank",           do_blank,             POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "brief",           do_brief,             POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "channels",        do_channels,          POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "combine",         do_combine,           POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "autosplit",       do_autosplit,         POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "ansi",            do_ansi,              POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "fullname",        do_fullname,          POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "combat",          do_combat,            POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "telnetga",        do_telnetga,          POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "config",          do_config,            POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "description",     do_description,       POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "email",           do_email,             POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "password",        do_password,          POS_DEAD,     0,     LOG_NEVER,  TRUE    },
  { "plan",            do_plan,              POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "prompt",          do_prompt,            POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "title",           do_title,             POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "wimpy",           do_wimpy,             POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "answer",          do_answer,            POS_SLEEPING, 0,     LOG_NORMAL, TRUE    },
  { "auction",         do_auction,           POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "bid",             do_bid,               POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "chat",            do_chat,              POS_SLEEPING, 0,     LOG_NORMAL, TRUE    },
  { "emote",           do_emote,             POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "gtell",           do_gtell,             POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "music",           do_music,             POS_SLEEPING, 0,     LOG_NORMAL, TRUE    },
  { "note",            do_note,              POS_SLEEPING, 0,     LOG_NORMAL, TRUE    },
  { "quest",           do_quest,             POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "question",        do_question,          POS_SLEEPING, 0,     LOG_NORMAL, TRUE    },
  { "reply",           do_reply,             POS_SLEEPING, 0,     LOG_NORMAL, TRUE    },
  { "say",             do_say,               POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "shout",           do_shout,             POS_RESTING,  3,     LOG_NORMAL, TRUE    },
  { "yell",            do_yell,              POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "clantalk",        do_clan,              POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "clans",           do_clans,             POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "class",           do_class,             POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "ooc",             do_ooc,               POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "guard",           do_guard,             POS_DEAD,     L_CON, LOG_NORMAL, TRUE    },
  { "conference",      do_conference,        POS_DEAD,     L_IMP, LOG_ALWAYS, TRUE    },
  { "info",            do_info,              POS_DEAD,     L_DIR, LOG_ALWAYS, TRUE    },
  { "vent",            do_vent,              POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "clear_alias",     do_clear,             POS_DEAD,     0,     LOG_NORMAL, FALSE   },
  { "brandish",        do_brandish,          POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "close",           do_close,             POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "donate",          do_donate,            POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "drink",           do_drink,             POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "drop",            do_drop,              POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "eat",             do_eat,               POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "fill",            do_fill,              POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "give",            do_give,              POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "hold",            do_wear,              POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "join",            do_join,              POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "list",            do_list,              POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "lock",            do_lock,              POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "open",            do_open,              POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "patch",           do_patch,             POS_STANDING, 26,    LOG_NORMAL, TRUE    },
  { "pick",            do_pick,              POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "put",             do_put,               POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "quaff",           do_quaff,             POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "raise",           do_raise,             POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "recall",          do_lowrecall,         POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "recite",          do_recite,            POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "remove",          do_remove,            POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "remor",           do_remor,             POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "remort",          do_remort,            POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "retrieve",        do_retrieve,          POS_STANDING, 2,     LOG_NORMAL, TRUE    },
  { "sell",            do_sell,              POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "take",            do_get,               POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "sacrifice",       do_sacrifice,         POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "separate",        do_separate,          POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "store",           do_store,             POS_STANDING, 2,     LOG_NORMAL, TRUE    },
  { "unlock",          do_unlock,            POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "value",           do_value,             POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "wear",            do_wear,              POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "zap",             do_zap,               POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "stare",           do_stare,             POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "alchemy",         do_alchemy,           POS_RESTING,  0,     LOG_ALWAYS, TRUE    },
  { "antidote",        do_antidote,          POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "scribe",          do_scribe,            POS_RESTING,  0,     LOG_ALWAYS, TRUE    },
  { "gravebind",       do_gravebind,         POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "flamehand",       do_flamehand,         POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "frosthand",       do_frosthand,         POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "chaoshand",       do_chaoshand,         POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "backstab",        do_backstab,          POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "bladepalm",       do_bladepalm,         POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "bs",              do_backstab,          POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "disarm",          do_disarm,            POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "flee",            do_flee,              POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "retreat",         do_retreat,           POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "flury",           do_flury,             POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "kick",            do_kick,              POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "high kick",       do_high_kick,         POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "jump kick",       do_jump_kick,         POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "spin kick",       do_spin_kick,         POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "flying kick",     do_flyingkick,        POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "punch",           do_punch,             POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "jab punch",       do_jab_punch,         POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "kidney punch",    do_kidney_punch,      POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "cross punch",     do_cross_punch,       POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "roundhouse punch", do_roundhouse_punch, POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "uppercut punch",  do_uppercut_punch,    POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "murde",           do_murde,             POS_FIGHTING, 5,     LOG_NORMAL, TRUE    },
  { "murder",          do_murder,            POS_FIGHTING, 5,     LOG_ALWAYS, TRUE    },
  { "nerve",           do_nerve,             POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "rescue",          do_rescue,            POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "throw",           do_throw,             POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "trip",            do_trip,              POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "drain life",      do_drain_life,        POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "mental drain",    do_mental_drain,      POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "shriek",          do_shriek,            POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "snatch",          do_snatch,            POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "stun",            do_stun,              POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "berserk",         do_berserk,           POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "soulstrike",      do_soulstrike,        POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "multiburst",      do_multiburst,        POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "bite",            do_bite,              POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "howl of fear",    do_howl_of_fear,      POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "frenzy",          do_frenzy,            POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "account",         do_account,           POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "adrenaline rush", do_rush,              POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "afk",             do_afk,               POS_DEAD,     0,     LOG_NORMAL, FALSE   },
  { "afkmes",          do_afkmes,            POS_DEAD,     0,     LOG_NORMAL, FALSE   },
  { "bash door",       do_bash,              POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "chameleon power", do_chameleon,         POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "follow",          do_follow,            POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "group",           do_group,             POS_SLEEPING, 0,     LOG_NORMAL, TRUE    },
  { "heighten senses", do_heighten,          POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "enhance reflexes", do_reflex,           POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "hide",            do_hide,              POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "iron grip",       do_irongrip,          POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "ironfist",        do_ironfist,          POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "poison weapon",   do_poison_weapon,     POS_SLEEPING, 0,     LOG_NORMAL, TRUE    },
  { "practice",        do_practice,          POS_SLEEPING, 0,     LOG_NORMAL, TRUE    },
  { "qui",             do_qui,               POS_DEAD,     0,     LOG_NORMAL, FALSE   },
  { "quit",            do_quit,              POS_DEAD,     0,     LOG_NORMAL, FALSE   },
  { "rage",            do_rage,              POS_DEAD,     0,     LOG_NORMAL, TRUE    },
  { "remote",          do_remote,            POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "save",            do_save,              POS_DEAD,     1,     LOG_NORMAL, FALSE   },
  { "shadow",          do_shadow_walk,       POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "sleep",           do_sleep,             POS_SLEEPING, 0,     LOG_NORMAL, TRUE    },
  { "sneak",           do_sneak,             POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "spells",          do_spells,            POS_SLEEPING, L_IMP, LOG_NORMAL, TRUE    },
  { "split",           do_split,             POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "steal",           do_steal,             POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "train",           do_train,             POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "visible",         do_visible,           POS_SLEEPING, 0,     LOG_NORMAL, TRUE    },
  { "wake",            do_wake,              POS_SLEEPING, 0,     LOG_NORMAL, TRUE    },
  { "ward",            do_ward,              POS_STANDING, 1,     LOG_ALWAYS, TRUE    },
  { "where",           do_where,             POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "withdraw",        do_withdraw,          POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "pray",            do_pray,              POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "delet",           do_delet,             POS_STANDING, 0,     LOG_NORMAL, FALSE   },
  { "delete",          do_delete,            POS_STANDING, 0,     LOG_ALWAYS, FALSE   },
  { "pkill",           do_pkill,             POS_DEAD,     0,     LOG_NORMAL, FALSE   },
  { "indestructable",  do_indestructable,    POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "remake",          do_remake,            POS_STANDING, 0,     LOG_NORMAL, TRUE    },
  { "advance",         do_advance,           POS_DEAD,     L_IMP, LOG_ALWAYS, TRUE    },
  { "allow",           do_allow,             POS_DEAD,     L_DIR, LOG_ALWAYS, TRUE    },
  { "astat",           do_astat,             POS_DEAD,     L_JUN, LOG_NORMAL, TRUE    },
  { "astrip",          do_astrip,            POS_DEAD,     L_JUN, LOG_NORMAL, TRUE    },
  { "at",              do_at,                POS_DEAD,     L_APP, LOG_NORMAL, TRUE    },
  { "authorize",       do_authorize,         POS_DEAD,     L_APP, LOG_ALWAYS, TRUE    },
  { "bamfmes",         do_bamf,              POS_DEAD,     L_APP, LOG_NORMAL, TRUE    },
  { "banlist",         do_banlist,           POS_DEAD,     L_GOD, LOG_NORMAL, TRUE    },
  { "bodybag",         do_bodybag,           POS_DEAD,     L_APP, LOG_ALWAYS, TRUE    },
  { "bugs",            do_bugs,              POS_DEAD,     L_APP, LOG_NORMAL, TRUE    },
  { "changes",         do_changes,           POS_DEAD,     L_DIR, LOG_NORMAL, TRUE    },
  { "clandesc",        do_clandesc,          POS_DEAD,     L_SEN, LOG_NORMAL, TRUE    },
  { "cloak",           do_cloak,             POS_DEAD,     L_DIR, LOG_NORMAL, TRUE    },
  { "clone",           do_clone,             POS_DEAD,     L_SEN, LOG_NORMAL, TRUE    },
  { "countcommands",   do_countcommands,     POS_DEAD,     L_IMP, LOG_NORMAL, FALSE   },
  { "cset",            do_cset,              POS_DEAD,     L_IMP, LOG_BUILD,  TRUE    },
  { "deny",            do_deny,              POS_DEAD,     L_IMP, LOG_ALWAYS, TRUE    },
  { "desc_check",      do_desc_check,        POS_DEAD,     L_IMP, LOG_NORMAL, TRUE    },
  { "desc_clean",      do_descript_clean,    POS_DEAD,     L_IMP, LOG_NORMAL, TRUE    },
  { "detract",         do_detract,           POS_DEAD,     L_IMP, LOG_ALWAYS, TRUE    },
  { "disconnect",      do_disconnect,        POS_DEAD,     L_GOD, LOG_ALWAYS, TRUE    },
  { "echo",            do_echo,              POS_DEAD,     L_JUN, LOG_ALWAYS, TRUE    },
  { "empower",         do_empower,           POS_DEAD,     L_IMP, LOG_ALWAYS, TRUE    },
  { "force",           do_force,             POS_DEAD,     L_DIR, LOG_ALWAYS, TRUE    },
  { "freeze",          do_freeze,            POS_DEAD,     L_DIR, LOG_ALWAYS, TRUE    },
  { "goto",            do_goto,              POS_DEAD,     L_APP, LOG_NORMAL, TRUE    },
  { "hero",            do_hero,              POS_DEAD,     100,   LOG_NORMAL, TRUE    },
  { "hlist",           do_hlist,             POS_DEAD,     L_APP, LOG_NORMAL, TRUE    },
  { "holylight",       do_holylight,         POS_DEAD,     L_APP, LOG_NORMAL, TRUE    },
  { "hotreboo",        do_hotreboo,          POS_DEAD,     L_DIR, LOG_ALWAYS, TRUE    },
  { "hotreboot",       do_hotreboot,         POS_DEAD,     L_DIR, LOG_ALWAYS, TRUE    },
  { "ideas",           do_ideas,             POS_DEAD,     L_DIR, LOG_ALWAYS, TRUE    },
  { "immtalk",         do_immtalk,           POS_DEAD,     L_APP, LOG_NORMAL, TRUE    },
  { "shieldify",       do_shieldify,         POS_STANDING, L_CON, LOG_NORMAL, TRUE    },
  { "doubleexp",       do_doubleexp,         POS_DEAD,     L_DIR, LOG_NORMAL, TRUE    },
  { "log",             do_log,               POS_DEAD,     L_CON, LOG_ALWAYS, TRUE    },
  { "memory",          do_memory,            POS_DEAD,     L_GOD, LOG_NORMAL, TRUE    },
  { "mfind",           do_mfind,             POS_DEAD,     L_JUN, LOG_NORMAL, TRUE    },
  { "mload",           do_mload,             POS_DEAD,     L_DEM, LOG_BUILD,  TRUE    },
  { "mpstat",          do_mpstat,            POS_DEAD,     L_APP, LOG_NORMAL, TRUE    },
  { "mpcommands",      do_mpcommands,        POS_DEAD,     L_APP, LOG_NORMAL, TRUE    },
  { "mset",            do_mset,              POS_DEAD,     L_DIR, LOG_BUILD,  TRUE    },
  { "mstat",           do_mstat,             POS_DEAD,     L_JUN, LOG_NORMAL, TRUE    },
  { "mwhere",          do_mwhere,            POS_DEAD,     L_JUN, LOG_NORMAL, TRUE    },
  { "newban",          do_newban,            POS_DEAD,     L_GOD, LOG_ALWAYS, TRUE    },
  { "newcorpse",       do_newcorpse,         POS_DEAD,     L_GOD, LOG_ALWAYS, TRUE    },
  { "newlock",         do_newlock,           POS_DEAD,     L_IMP, LOG_ALWAYS, TRUE    },
  { "noemote",         do_noemote,           POS_DEAD,     L_GOD, LOG_NORMAL, TRUE    },
  { "notell",          do_notell,            POS_DEAD,     L_GOD, LOG_NORMAL, TRUE    },
  { "nukerep",         do_nukerep,           POS_DEAD,     L_GOD, LOG_NORMAL, TRUE    },
  { "numlock",         do_numlock,           POS_DEAD,     L_CON, LOG_ALWAYS, TRUE    },
  { "ofind",           do_ofind,             POS_DEAD,     L_JUN, LOG_NORMAL, TRUE    },
  { "oload",           do_oload,             POS_DEAD,     L_JUN, LOG_BUILD,  TRUE    },
  { "opstat",          do_opstat,            POS_DEAD,     L_APP, LOG_NORMAL, TRUE    },
  { "oset",            do_oset,              POS_DEAD,     L_DIR, LOG_BUILD,  TRUE    },
  { "ostat",           do_ostat,             POS_DEAD,     L_JUN, LOG_NORMAL, TRUE    },
  { "owhere",          do_owhere,            POS_DEAD,     L_JUN, LOG_NORMAL, TRUE    },
  { "pardon",          do_pardon,            POS_DEAD,     L_IMP, LOG_ALWAYS, TRUE    },
  { "peace",           do_peace,             POS_DEAD,     L_DEM, LOG_NORMAL, TRUE    },
  { "permban",         do_permban,           POS_DEAD,     L_DIR, LOG_ALWAYS, TRUE    },
  { "plist",           do_playerlist,        POS_DEAD,     L_APP, LOG_NORMAL, TRUE    },
  { "pload",           do_pload,             POS_DEAD,     L_DIR, LOG_ALWAYS, TRUE    },
  { "purge",           do_purge,             POS_DEAD,     L_DEM, LOG_NORMAL, TRUE    },
  { "qecho",           do_qecho,             POS_DEAD,     L_GOD, LOG_ALWAYS, TRUE    },
  { "qset",            do_qset,              POS_DEAD,     L_GOD, LOG_ALWAYS, TRUE    },
  { "rebuild",         do_rebuild,           POS_DEAD,     L_DIR, LOG_ALWAYS, TRUE    },
  { "reboo",           do_reboo,             POS_DEAD,     L_DIR, LOG_NORMAL, TRUE    },
  { "reboot",          do_reboot,            POS_DEAD,     L_DIR, LOG_ALWAYS, TRUE    },
  { "recho",           do_recho,             POS_DEAD,     L_JUN, LOG_ALWAYS, TRUE    },
  { "restore",         do_restore,           POS_DEAD,     L_GOD, LOG_ALWAYS, TRUE    },
  { "restrict",        do_restrict,          POS_DEAD,     L_GOD, LOG_NORMAL, TRUE    },
  { "rpstat",          do_rpstat,            POS_DEAD,     L_APP, LOG_NORMAL, TRUE    },
  { "rset",            do_rset,              POS_DEAD,     L_DIR, LOG_BUILD,  TRUE    },
  { "rstat",           do_rstat,             POS_DEAD,     L_JUN, LOG_NORMAL, TRUE    },
  { "seize",           do_seize,             POS_DEAD,     L_DIR, LOG_ALWAYS, TRUE    },
  { "shutdow",         do_shutdow,           POS_DEAD,     L_CON, LOG_NORMAL, TRUE    },
  { "shutdown",        do_shutdown,          POS_DEAD,     L_CON, LOG_ALWAYS, TRUE    },
  { "silence",         do_silence,           POS_DEAD,     L_GOD, LOG_NORMAL, TRUE    },
  { "sla",             do_sla,               POS_DEAD,     L_GOD, LOG_NORMAL, TRUE    },
  { "slay",            do_slay,              POS_DEAD,     L_GOD, LOG_ALWAYS, TRUE    },
  { "slaymes",         do_slaymes,           POS_DEAD,     L_GOD, LOG_NORMAL, TRUE    },
  { "slookup",         do_slookup,           POS_DEAD,     L_DIR, LOG_NORMAL, TRUE    },
  { "snoop",           do_snoop,             POS_DEAD,     L_DIR, LOG_ALWAYS, TRUE    },
  { "sset",            do_sset,              POS_DEAD,     L_DIR, LOG_ALWAYS, TRUE    },
  { "sstat",           do_sstat,             POS_DEAD,     L_DIR, LOG_NORMAL, TRUE    },
  { "sstime",          do_sstime,            POS_DEAD,     L_CON, LOG_ALWAYS, TRUE    },
  { "tempban",         do_tempban,           POS_DEAD,     L_GOD, LOG_ALWAYS, TRUE    },
  { "todo",            do_todo,              POS_DEAD,     L_DIR, LOG_ALWAYS, TRUE    },
  { "transfer",        do_transfer,          POS_DEAD,     L_JUN, LOG_ALWAYS, TRUE    },
  { "transmes",        do_trmes,             POS_DEAD,     L_JUN, LOG_NORMAL, TRUE    },
  { "trust",           do_trust,             POS_DEAD,     L_IMP, LOG_ALWAYS, TRUE    },
  { "typos",           do_typos,             POS_DEAD,     L_SEN, LOG_ALWAYS, TRUE    },
  { "users",           do_users,             POS_DEAD,     L_DIR, LOG_NORMAL, TRUE    },
  { "vused",           do_vused,             POS_DEAD,     L_DIR, LOG_NORMAL, TRUE    },
  { "whotype",         do_whotype,           POS_DEAD,     L_CON, LOG_NORMAL, TRUE    },
  { "wizify",          do_wizify,            POS_DEAD,     L_CON, LOG_ALWAYS, TRUE    },
  { "wizlock",         do_wizlock,           POS_DEAD,     L_IMP, LOG_ALWAYS, TRUE    },
  { "wizinvis",        do_invis,             POS_DEAD,     L_DIR, LOG_NORMAL, TRUE    },
  { "wiznet",          do_wiznet,            POS_DEAD,     L_JUN, LOG_NORMAL, TRUE    },
  { "wrlist",          do_wrlist,            POS_DEAD,     L_JUN, LOG_NORMAL, TRUE    },
  { "switch",          do_switch,            POS_DEAD,     L_DIR, LOG_ALWAYS, TRUE    },
  { "return",          do_return,            POS_DEAD,     L_DIR, LOG_NORMAL, TRUE    },
  { "gouge",           do_gouge,             POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "circle",          do_circle,            POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "rake",            do_rake,              POS_FIGHTING, 0,     LOG_NORMAL, TRUE    },
  { "transport",       do_transport,         POS_RESTING,  0,     LOG_NORMAL, TRUE    },
  { "aedit",           do_aedit,             POS_DEAD,     L_IMP, LOG_BUILD,  TRUE    },
  { "cedit",           do_cedit,             POS_DEAD,     L_CON, LOG_BUILD,  TRUE    },
  { "hedit",           do_hedit,             POS_DEAD,     L_APP, LOG_BUILD,  TRUE    },
  { "redit",           do_redit,             POS_DEAD,     L_CON, LOG_BUILD,  TRUE    },
  { "oedit",           do_oedit,             POS_DEAD,     L_IMP, LOG_BUILD,  TRUE    },
  { "medit",           do_medit,             POS_DEAD,     L_IMP, LOG_BUILD,  TRUE    },
  { "asave",           do_asave,             POS_DEAD,     L_APP, LOG_NORMAL, TRUE    },
  { "alist",           do_alist,             POS_DEAD,     L_CON, LOG_NORMAL, TRUE    },
  { "resets",          do_resets,            POS_DEAD,     L_CON, LOG_NORMAL, TRUE    },
  { "mreset",          do_mreset,            POS_DEAD,     L_CON, LOG_NORMAL, TRUE    },
  { "sedit",           do_sedit,             POS_DEAD,     L_CON, LOG_BUILD,  TRUE    },
  { "spedit",          do_spedit,            POS_DEAD,     L_IMP, LOG_ALWAYS, TRUE    },
  { "rename_obj",      do_rename_obj,        POS_DEAD,     L_APP, LOG_ALWAYS, TRUE    },
  { "nedit",           do_nedit,             POS_DEAD,     L_CON, LOG_BUILD,  TRUE    },
  // fin.
  { "",                0,                    POS_DEAD,     0,     LOG_NORMAL, FALSE   }
};

/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
void interpret( CHAR_DATA * ch, char * argument ) {
  char command[ MAX_INPUT_LENGTH ];
  char logline[ MAX_INPUT_LENGTH ];
  int  cmd;
  int  trust;
  bool found;

  if ( ch->wait > 0 ) {
    return;
  }

  // strip leading spaces
  while ( isspace( *argument ) ) {
    argument++;
  }

  if ( argument[ 0 ] == '\0' ) {
    return;
  }

  // implement freeze command
  if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_FREEZE ) ) {
    send_to_char( AT_LBLUE, "You're totally frozen!\n\r", ch );
    return;
  }

  /*
   * Implement stun.
   */
  if ( IS_STUNNED( ch, STUN_COMMAND ) || IS_STUNNED( ch, STUN_TOTAL ) ) {
    send_to_char( AT_LBLUE, "You're too stunned to do anything!\n\r", ch );
    return;
  }

  // grab the command word
  // NOTE: special parsing so ' can be a command, also no spaces needed after punctuation
  strcpy( logline, argument );

  if ( !isalpha( argument[ 0 ] ) && !isdigit( argument[ 0 ] ) ) {
    command[ 0 ] = argument[ 0 ];
    command[ 1 ] = '\0';
    argument++;

    while ( isspace( *argument ) ) {
      argument++;
    }
  } else {
    argument = one_argument( argument, command );
  }

  // look for command in command table
  found = FALSE;
  trust = get_trust( ch );

  for ( cmd = 0; cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ ) {
    if ( command[ 0 ] == cmd_table[ cmd ].name[ 0 ] && !str_prefix( command, cmd_table[ cmd ].name ) && can_use_cmd( cmd, ch, trust ) ) {
      found = TRUE;
      break;
    }
  }

  // log and snoop
  if ( cmd_table[ cmd ].log == LOG_NEVER ) {
    strcpy( logline, "XXXXXXXX XXXXXXXX XXXXXXXX" );
  }

  // builder logs
  if ( cmd_table[ cmd ].log == LOG_BUILD ) {
    sprintf( log_buf, "%s: %s", ch->name, logline );
    log_string( log_buf, CHANNEL_BUILD, get_trust( ch ) );
    wiznet( log_buf, ch, NULL, WIZ_SECURE, 0, get_trust( ch ) );
  }

  if ( ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_LOG ) ) || fLogAll || cmd_table[ cmd ].log == LOG_ALWAYS ) {
    sprintf( log_buf, "%s: %s", ch->name, logline );
    log_string( log_buf, ( ch->level == L_IMP ? CHANNEL_NONE : CHANNEL_GOD ), ( ( ch->level > 100 ) ? get_trust( ch ) : -1 ) );
    wiznet( log_buf, ch, NULL, WIZ_SECURE, 0, get_trust( ch ) );
  }

  if ( ch->desc && ch->desc->snoop_by ) {
    sprintf( log_buf, "%s%%", ch->name );
    write_to_buffer( ch->desc->snoop_by, log_buf, 0 );
    write_to_buffer( ch->desc->snoop_by, logline, 0 );
    write_to_buffer( ch->desc->snoop_by, "\n\r", 2 );
  }

  // hiding stuff...
  if ( IS_AFFECTED( ch, AFF_HIDE ) ) {
    // things you can do while hiding
    if ( str_cmp( cmd_table[ cmd ].name, "pick" ) &&
         str_cmp( cmd_table[ cmd ].name, "chameleon power" ) &&
         str_cmp( cmd_table[ cmd ].name, "save" ) &&
         str_cmp( cmd_table[ cmd ].name, "visible" ) &&
         str_cmp( cmd_table[ cmd ].name, "inventory" ) &&
         str_cmp( cmd_table[ cmd ].name, "equipment" ) &&
         str_cmp( cmd_table[ cmd ].name, "where" ) &&
         str_cmp( cmd_table[ cmd ].name, "who" ) &&
         str_cmp( cmd_table[ cmd ].name, "open" ) &&
         str_cmp( cmd_table[ cmd ].name, "hide" ) &&
         str_cmp( cmd_table[ cmd ].name, "sleep" ) &&
         str_cmp( cmd_table[ cmd ].name, "stand" ) &&
         str_cmp( cmd_table[ cmd ].name, "rest" ) &&
         str_cmp( cmd_table[ cmd ].name, "peek" ) &&
         str_cmp( cmd_table[ cmd ].name, "shadow" ) &&
         str_cmp( cmd_table[ cmd ].name, "sneak" ) &&
         str_cmp( cmd_table[ cmd ].name, "worth" ) &&
         str_cmp( cmd_table[ cmd ].name, "affected" ) &&
         str_cmp( cmd_table[ cmd ].name, "look" ) &&
         str_cmp( cmd_table[ cmd ].name, "tell" ) &&
         str_cmp( cmd_table[ cmd ].name, "score" ) ) {
      // if you are sneaking you can move while hiding
      if ( IS_AFFECTED( ch, AFF_SNEAK ) ) {
        // check and see if they are chameleon and remove it if they are trying to move
        if ( is_affected( ch, gsn_chameleon ) ) {
          send_to_char( AT_BLUE, "You return to your normal coloration.\n\r", ch );
          affect_strip( ch, gsn_chameleon );
        }

        // if they aren't hidding remove the hide bit
        if ( !is_affected( ch, gsn_hide ) ) {
          REMOVE_BIT( ch->affected_by, AFF_HIDE );
        }
      } else {
        if ( is_affected( ch, gsn_chameleon ) ) {
          send_to_char( AT_BLUE, "You return to your normal coloration.\n\r", ch );
          affect_strip( ch, gsn_chameleon );
        }

        if ( is_affected( ch, gsn_hide ) ) {
          send_to_char( AT_BLUE, "You stop hiding.\n\r", ch );
          affect_strip( ch, gsn_hide );
        }

        REMOVE_BIT( ch->affected_by, AFF_HIDE );
      }
    }
  }

  if ( IS_AFFECTED2( ch, AFF_BERSERK ) && trust < LEVEL_IMMORTAL ) {
    if ( !found || ( str_cmp( cmd_table[ cmd ].name, "flee" ) &&
                     str_cmp( cmd_table[ cmd ].name, "look" ) &&
                     str_cmp( cmd_table[ cmd ].name, "get" ) &&
                     str_cmp( cmd_table[ cmd ].name, "wield" ) &&
                     str_cmp( cmd_table[ cmd ].name, "dual" ) &&
                     str_cmp( cmd_table[ cmd ].name, "score" ) ) ) {
      send_to_char( AT_BLOOD, "You cannot do that in such a rage!\n\r", ch );
      return;
    }
  }

  if ( !found ) {
    // look for command in socials table
    if ( IS_NPC( ch ) || !check_alias( ch, command, argument ) ) {
      if ( !check_social( ch, command, argument ) ) {
        send_to_char( C_DEFAULT, "Huh?\n\r", ch );
      }
    }

    return;
  }

  // character not in position for command?
  if ( ch->position < cmd_table[ cmd ].position ) {
    switch ( ch->position ) {
      case POS_DEAD:
        send_to_char( AT_RED, "Lie still; you are DEAD.\n\r", ch );
        break;
      case POS_MORTAL:
      case POS_INCAP:
        send_to_char( AT_RED, "You are hurt far too bad for that.\n\r", ch );
        break;
      case POS_STUNNED:
        send_to_char( AT_WHITE, "You are too stunned to do that.\n\r", ch );
        break;
      case POS_SLEEPING:
        send_to_char( AT_BLUE, "In your dreams, or what?\n\r", ch );
        break;
      case POS_RESTING:
        send_to_char( AT_BLUE, "Nah... You feel too relaxed...\n\r", ch );
        break;
      case POS_FIGHTING:
        send_to_char( AT_BLOOD, "No way!  You are still fighting!\n\r", ch );
        break;
    }

    return;
  }

  // dispatch the command.
  if ( IS_SET( ch->act, PLR_AFK ) ) {
    // if player was afk, remove afk
    REMOVE_BIT( ch->act, PLR_AFK );
    send_to_char( AT_YELLOW, "You are no longer AFK.\n\r", ch );
  }

  ( *cmd_table[ cmd ].do_fun )( ch, argument );
  tail_chain();
  return;
}

bool can_use_cmd( int cmd, CHAR_DATA * ch, int trust ) {
  if ( IS_NPC( ch ) && !cmd_table[ cmd ].npc ) {
    return FALSE;
  }

  if ( trust <= 0 ) {
    trust = get_trust( ch );
  }

  if ( !IS_NPC( ch ) ) {
    if ( is_name( NULL, cmd_table[ cmd ].name, ch->pcdata->empowerments ) ) {
      return TRUE;
    }

    if ( is_name( NULL, cmd_table[ cmd ].name, ch->pcdata->detractments ) ) {
      return FALSE;
    }
  }

  return ( cmd_table[ cmd ].level <= trust );
}

bool check_alias( CHAR_DATA * ch, char * command, char * argument ) {
  ALIAS_DATA * al;
  char         arg[ MAX_STRING_LENGTH ];
  char         newarg[ MAX_STRING_LENGTH ];
  bool         found;

  arg[ 0 ]    = '\0';
  newarg[ 0 ] = '\0';

  while ( isspace( *argument ) ) {
    argument++;
  }

  strcpy( arg, argument );

  if ( IS_NPC( ch ) ) {
    return FALSE;
  }

  if ( !( ch->pcdata->alias_list ) ) {
    return FALSE;
  }

  found = FALSE;

  for ( al = ch->pcdata->alias_list; al; al = al->next ) {
    if ( !str_cmp( al->old, command ) ) {
      strcpy( newarg, al->new );
      strcat( newarg, " " );
      strcat( newarg, arg );
      interpret( ch, newarg );

      found = TRUE;
      break;
    }
  }

  return found;
}

bool check_social( CHAR_DATA * ch, char * command, char * argument ) {
  CHAR_DATA   * victim;
  SOCIAL_DATA * pSocial;
  char          arg[ MAX_INPUT_LENGTH ];
  char          buf[ MAX_STRING_LENGTH ];
  bool          found;

  found = FALSE;

  for ( pSocial = social_first; pSocial; pSocial = pSocial->next ) {
    if ( command[ 0 ] == pSocial->name[ 0 ] && !str_prefix( command, pSocial->name ) ) {
      found = TRUE;
      break;
    }
  }

  if ( !found ) {
    return FALSE;
  }

  if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_NO_EMOTE ) ) {
    send_to_char( AT_LBLUE, "You are anti-social!\n\r", ch );
    return TRUE;
  }

  switch ( ch->position ) {
    case POS_DEAD:
      send_to_char( AT_RED, "Lie still; you are DEAD.\n\r", ch );
      return TRUE;
    case POS_INCAP:
    case POS_MORTAL:
      send_to_char( AT_RED, "You are hurt far too badly for that.\n\r", ch );
      return TRUE;
    case POS_STUNNED:
      send_to_char( AT_WHITE, "You are too stunned to do that.\n\r", ch );
      return TRUE;
    case POS_SLEEPING:

      if ( !str_cmp( pSocial->name, "snore" ) ) {
        break;
      }

      send_to_char( AT_BLUE, "In your dreams, or what?\n\r", ch );
      return TRUE;
  }

  one_argument( argument, arg );
  victim = NULL;

  if ( arg[ 0 ] == '\0' ) {
    act( AT_PINK, pSocial->char_no_arg, ch, NULL, victim, TO_CHAR );
    act( AT_PINK, pSocial->others_no_arg, ch, NULL, victim, TO_ROOM );
  } else if ( !( victim = get_char_room( ch, arg ) ) ) {
    if ( !( victim = get_char_world( ch, arg ) ) ) {
      send_to_char( AT_WHITE, "They aren't here.\n\r", ch );
    } else {
      if ( ( ch->in_room->vnum == ROOM_VNUM_HELL ) && ( ch->level < LEVEL_IMMORTAL ) ) {
        send_to_char( AT_WHITE, "What do you think this is, a vacation?\n\r", ch );
      } else {
        sprintf( buf, "From a great distance, %s", pSocial->char_found );
        act( AT_PINK, buf, ch, NULL, victim, TO_CHAR );

        sprintf( buf, "From a great distance, %s", pSocial->vict_found );
        act( AT_PINK, buf, ch, NULL, victim, TO_VICT );

        sprintf( buf, "From a great distance, %s", pSocial->others_found );
        act( AT_PINK, buf, ch, NULL, victim, TO_NOTVICT );
      }
    }
  } else if ( victim == ch ) {
    act( AT_PINK, pSocial->char_auto, ch, NULL, victim, TO_CHAR );
    act( AT_PINK, pSocial->others_auto, ch, NULL, victim, TO_ROOM );
  } else {
    act( AT_PINK, pSocial->char_found, ch, NULL, victim, TO_CHAR );
    act( AT_PINK, pSocial->vict_found, ch, NULL, victim, TO_VICT );
    act( AT_PINK, pSocial->others_found, ch, NULL, victim, TO_NOTVICT );

    if ( !IS_NPC( ch ) && IS_NPC( victim ) && !IS_AFFECTED( victim, AFF_CHARM ) && IS_AWAKE( victim ) && ( !victim->pIndexData->mobprogs ) ) {
      switch ( number_bits( 4 ) ) {
        case 0:

          if ( ( victim->level < ch->level ) && !( victim->fighting ) ) {
            multi_hit( victim, ch, TYPE_UNDEFINED );
          }

          break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
          act( AT_PINK, pSocial->char_found, victim, NULL, ch, TO_CHAR );
          act( AT_PINK, pSocial->vict_found, victim, NULL, ch, TO_VICT );
          act( AT_PINK, pSocial->others_found, victim, NULL, ch, TO_NOTVICT );
          break;
        case 9:
        case 10:
        case 11:
        case 12:
          act( AT_PINK, "You slap $N.", victim, NULL, ch, TO_CHAR );
          act( AT_PINK, "$n slaps you.", victim, NULL, ch, TO_VICT );
          act( AT_PINK, "$n slaps $N.", victim, NULL, ch, TO_NOTVICT );
          break;
      }
    }
  }

  return TRUE;
}

/*
 * Return true if an argument is completely numeric.
 */
bool is_number( char * arg ) {
  if ( *arg == '\0' ) {
    return FALSE;
  }

  if ( *arg == '+' || *arg == '-' ) {
    arg++;
  }

  for (; *arg != '\0'; arg++ ) {
    if ( !isdigit( *arg ) ) {
      return FALSE;
    }
  }

  return TRUE;
}

/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument( char * argument, char * arg ) {
  char * pdot;
  int    number;

  for ( pdot = argument; *pdot != '\0'; pdot++ ) {
    if ( *pdot == '.' ) {
      *pdot  = '\0';
      number = atoi( argument );
      *pdot  = '.';
      strcpy( arg, pdot + 1 );
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
char * one_argument( char * argument, char * arg_first ) {
  char cEnd;

  while ( isspace( *argument ) ) {
    argument++;
  }

  cEnd = ' ';

  if ( *argument == '\'' || *argument == '"' ) {
    cEnd = *argument++;
  }

  while ( *argument != '\0' ) {
    if ( *argument == cEnd ) {
      argument++;
      break;
    }

    *arg_first = LOWER( *argument );
    arg_first++;
    argument++;
  }

  *arg_first = '\0';

  while ( isspace( *argument ) ) {
    argument++;
  }

  return argument;
}

void do_clear( CHAR_DATA * ch, char * argument ) {
  ALIAS_DATA * pA1;
  ALIAS_DATA * pA1_next;

  if ( !ch->pcdata->alias_list ) {
    send_to_char( AT_WHITE, "You have no alias' to clear.\n\r", ch );
    return;
  }

  for ( pA1 = ch->pcdata->alias_list; pA1; pA1 = pA1_next ) {
    pA1_next = pA1->next;
    free_alias( pA1 );
  }

  send_to_char( AT_WHITE, "All aliases have been erased.\n\r", ch );
  ch->pcdata->alias_list = NULL;
  return;
}

void do_alias( CHAR_DATA * ch, char * argument ) {
  char         arg[ MAX_STRING_LENGTH ];
  char         arg1[ MAX_STRING_LENGTH ];
  ALIAS_DATA * al;
  ALIAS_DATA * oAl;

  smash_tilde( argument );
  argument = one_argument( argument, arg );
  strcpy( arg1, argument );

  if ( arg[ 0 ] == '\0' ) {
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

  if ( arg1[ 0 ] == '\0' && ( !( ch->pcdata->alias_list ) ) ) {
    send_to_char( AT_WHITE, "You have no command alias' to remove!\n\r", ch );
    return;
  }

  if ( arg1[ 0 ] != '\0' ) {
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

  if ( arg1[ 0 ] == '\0' ) {
    if ( !al ) {
      send_to_char( C_DEFAULT, "Alias does not exist.\n\r", ch );
      return;
    }

    if ( al == ch->pcdata->alias_list ) {
      ch->pcdata->alias_list = al->next;
    } else {
      for ( oAl = ch->pcdata->alias_list; oAl; oAl = oAl->next ) {
        if ( oAl->next == al ) {
          break;
        }
      }

      if ( !oAl ) {
        sprintf( log_buf, "Do_alias: bad alias - ch (%s) - [%s]", ch->name, arg );
        bug( log_buf, 0 );
        send_to_char( C_DEFAULT, "Alias does not exist.\n\r", ch );
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

void add_alias( CHAR_DATA * ch, ALIAS_DATA * pAl, char * old, char * new ) {
  pAl                    = alloc_perm( sizeof( *pAl ) );
  pAl->old               = str_dup( old );
  pAl->new               = str_dup( new );
  pAl->next              = ch->pcdata->alias_list;
  ch->pcdata->alias_list = pAl;
  return;
}

void do_countcommands( CHAR_DATA * ch, char * argument ) {
  char buf[ MAX_STRING_LENGTH ];
  int  scnt;
  int  skill[ MAX_CLASS ];
  int  spell[ MAX_CLASS ];
  int  iclass;

  for ( iclass = 0; iclass < MAX_CLASS; iclass++ ) {
    skill[ iclass ] = 0;
    spell[ iclass ] = 0;
  }

  for ( scnt = 0; skill_table[ scnt ].name[ 0 ] != '\0'; scnt++ ) {
    for ( iclass = 0; iclass < MAX_CLASS; iclass++ ) {
      if ( skill_table[ scnt ].skill_level[ iclass ] < L_APP ) {
        if ( skill_table[ scnt ].spell_fun == spell_null ) {
          skill[ iclass ]++;
        } else {
          spell[ iclass ]++;
        }
      }
    }
  }

  sprintf( buf, "Command table size: %d\n\r" "Social table size:  %d\n\r" "GSkill table size:  %d\n\r" "Skill table size:   %d\n\r",
           (int) ( sizeof(   cmd_table ) / sizeof(   cmd_table[ 0 ] ) - 1 ),
           top_social,
           (int) ( sizeof( gskill_table ) / sizeof( gskill_table[ 0 ] ) ),
           scnt
           );

  send_to_char( AT_PINK, buf, ch );

  for ( iclass = 0; iclass < MAX_CLASS; iclass++ ) {
    sprintf( buf, "%-15s: Skills %2d + Spells %2d = Total %2d\n\r",
             class_table[ iclass ].who_long,
             skill[ iclass ], spell[ iclass ], skill[ iclass ] + spell[ iclass ] );
    send_to_char( AT_PINK, buf, ch );
  }

  return;
}

void do_afk( CHAR_DATA * ch, char * argument ) {
  send_to_char( AT_YELLOW, "You announce that you are going AFK.\n\r", ch );
  act( AT_YELLOW, "$n announces, 'I am going AFK.'", ch, NULL, NULL, TO_ROOM );
  SET_BIT( ch->act, PLR_AFK );
  return;
}

void do_pkill( CHAR_DATA * ch, char * argument ) {
  if ( argument[ 0 ] == '\0' ) {
    send_to_char( AT_WHITE, "Syntax: &Rpkill yes&W    To choose to become a PKILLER forever!\n\r", ch );
    send_to_char( AT_WHITE, "This is NOT a reversible choice. Type HELP PK.\n\r", ch );
    return;
  }

  if ( !str_cmp( argument, "yes" ) ) {
    if ( ch->level < 20 ) {
      send_to_char( AT_WHITE, "You must be at least level 20 to choose the path of PKILL.\n\r", ch );
      return;
    }

    if ( IS_SET( ch->act, PLR_PKILLER ) ) {
      send_to_char( AT_WHITE, "You have already chosen the path of pkilling.\n\r", ch );
      return;
    }

    send_to_char( AT_RED, "You are now a PKILLER! Beware, the path you have chosen is not one of the weak minded.\n\r", ch );
    act( AT_RED, "$n has made a lifelong decision, to join forces with Pkillers throughout the Storm!", ch, NULL, NULL, TO_ROOM );
    SET_BIT( ch->act, PLR_PKILLER );
    do_help( ch, "pk" );

    return;
  } else {
    send_to_char( AT_WHITE, "Syntax: &Rpkill yes&W    To choose to become a PKILLER forever!\n\r", ch );
    send_to_char( AT_WHITE, "This is NOT a reversible choice. Type HELP PK.\n\r", ch );
    return;
  }
}
