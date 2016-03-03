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
#include <time.h>
#include "merc.h"

const   struct  quest_data      quest_table     []              =
{
//{     "name",                         "full name",                                    vnum,         qp,     level  }
  {     "practice",                     "&PP&practic&Pe&X",                             0,           10,      1      },
  {     "bamboo training sword",        "&YB&Oa&Ym&Ob&Yo&Oo &cTraining Sword&X",        2426,        100,     1      },
  {     "tassled leather leggings",     "&cTassled &zL&We&za&Wt&zh&We&zr&c Leggings&X", 2427,        200,     5      },
  {     "ivory-cerved anklets",         "&WIvory&w-&WCarved &cAnklets&X",               2428,        300,     10     },
  {     "golwing bauble",               "&WGlow&wing &CBauble&X",                       2429,        100,     11     },
  {     "blazing flameshield",          "&RBl&raz&Oin&Yg &RFla&rmes&Ohie&Yld&X",        2430,        200,     15     },
  {     "scaly vambraces",              "&CScaly &cVambraces&X",                        2431,        300,     20     },
  {     "steel visor",                  "&zSteel &cVisor&X",                            2432,        100,     21     },
  {     "taloned gauntlets",            "&cTaloned Gauntlets&X",                        2433,        200,     25     },
  {     "brass scope",                  "&OBrass &cScope&X",                            2434,        300,     30     },
  {     "silver ear clasp",             "&wSilver &cEar Clasp&X",                       2435,        100,     31     },
  {     "heavy leather boots",          "&OHeavy Leather &cBoots&X",                    2436,        200,     35     },
  {     "wreath of illusions",          "&GW&gr&Ge&ga&Gt&gh &cof &CIllusions&X",        2437,        300,     40     },
  {     "long hafted spear",            "&OLong Hafted &zSpear&X",                      2438,        100,     41     },
  {     "arch sorcerers robe",          "&zArch Sorcerer's &cRobe&X",                   2439,        200,     45     },
  {     "engraved golden breastplate",  "&cEngraved &YGolden &cBreastplate&X",          2440,        300,     50     },
  {     "ankle sheathed dagger",        "&cAnkle-Sheathed Dagger&X",                    2441,        150,     51     },
  {     "band of winds",                "&OBand &cof &CWinds&X",                        2442,        250,     55     },
  {     "dark stormcloud",              "&zDark Stormcloud&X",                          2443,        350,     60     },
  {     "winter fur cloak",             "&WWinter Fur &cCloak&X",                       2444,        150,     61     },
  {     "travellers cloak",             "&cTraveller's Cloak&X",                        2445,        250,     65     },
  {     "demons fiery glare",           "&zDemon's &RFi&re&Or&Yy &rGlare&X",            2446,        350,     70     },
  {     "golden band of energy",        "&YGolden &cBand of &PE&Cn&Pe&Cr&Pg&Cy&X",      2447,        150,     71     },
  {     "dragon carved belt buckle",    "&RDragon-Carved &cBelt Buckle&X",              2448,        250,     75     },
  {     "elbow bladed sleeves",         "&cElbow-Bladed Sleeves&X",                     2449,        350,     80     },
  {     "heavy dwarven chainmail",      "&cHeavy Dwarven Chainmail&X",                  2450,        150,     81     },
  {     "dagger of light",              "&BD&bagge&Br &Yof &CL&ci&Cg&ch&Ct&X",          2457,        1200,    205    },
  {     "platinum platemail leggings",  "&YPlatinum Platemail &cLeggings&X",            2452,        350,     90     },
  {     "book of arcane arts",          "&OBook &cof &BArcane &RArts&X",                2453,        150,     91     },
  {     "hovering sphere of light",     "&cHovering &CSphere &cof &WLight&X",           2454,        250,     95     },
  {     "helms earguard",               "&cHelm's &BE&wa&Br&wg&Bu&wa&Br&wd&X",          2455,        350,     100    },
  {     "ivory carved mask",            "&WIvory&w-&WCarved &cMask&X",                  2456,        150,     101    },
  {     "",                              "",                                            0,           0,       0      }
};

const struct race_type race_table[MAX_RACE] = {
//{
//  "Sml", "Full Name", 
//  mstr, mint, mwis, mdex, mcon, mcha,
//  age, size
//}, 
  {  
    "Hum", "Human",
    0, 0, 0, 0, 0, 0,
    15, SIZE_MEDIUM
  },
  {  
    "Elf", "Elf",
    0, 2, 0, 2, -2, 0,
    110, SIZE_MEDIUM
  },
  {  
    "Dwa", "Dwarf",
    0, 0, 2, 0, 2, -2,
    40, SIZE_MEDIUM
  },
  {  
    "Gno", "Gnome",
    -2, 0, 0, 0, 2, 2,
    40, SIZE_SMALL
  },
  {  
    "Hlf", "Halfling",
    -2, 0, 0, 2, 0, 2,
    20, SIZE_SMALL
  }
};

const struct class_type class_table [MAX_CLASS] = {
//{ "Who",     "Long", attr_prime, skill_adept,  mbab, hitdice, d6gold, spellcaster }
  { "Cas",   "Caster",  APPLY_INT,          95,   0.5,       6,      2,        TRUE },
  { "Rog",    "Rogue",  APPLY_DEX,          85,  0.75,       8,      4,       FALSE },
  { "Fig",  "Fighter",  APPLY_STR,          85,     1,      10,      5,       FALSE },
};

const struct size_type size_table [MAX_SIZE] = {
//{        "size", mac, mstealth,  mcarry }
  {        "Fine",  -8,       16,   0.125 },
  {  "Diminutive",  -4,       12,    0.25 },
  {        "Tiny",  -2,        8,     0.5 },
  {       "Small",  -1,        4,    0.75 },
  {      "Medium",   0,        0,       0 },
  {       "Large",   1,       -4,       2 },
  {        "Huge",   2,       -8,       4 },
  {  "Gargantuan",   4,      -12,       8 },
  {    "Colossal",   8,      -16,      16 }
};

/*
 * Immort Levels
 */
#define L_HER            LEVEL_HERO

/* 
 * Wiznet table and prototype for future flag setting
 */
const struct wiznet_type wiznet_table[] = {
  { "on",         WIZ_ON,         LEVEL_HERO },
  { "prefix",     WIZ_PREFIX,     LEVEL_HERO },
  { "ticks",      WIZ_TICKS,      LEVEL_IMMORTAL },
  { "general",    WIZ_GENERAL,    LEVEL_HERO },
  { "logins",     WIZ_LOGINS,     LEVEL_HERO },
  { "sites",      WIZ_SITES,      LEVEL_IMMORTAL },
  { "links",      WIZ_LINKS,      LEVEL_IMMORTAL },
  { "newbies",    WIZ_NEWBIE,     LEVEL_HERO },
  { "spam",       WIZ_SPAM,       LEVEL_IMMORTAL },
  { "deaths",     WIZ_DEATHS,     LEVEL_HERO },
  { "resets",     WIZ_RESETS,     LEVEL_IMMORTAL },
  { "mobdeaths",  WIZ_MOBDEATHS,  LEVEL_IMMORTAL },
  { "flags",      WIZ_FLAGS,      LEVEL_IMMORTAL },
  { "penalties",  WIZ_PENALTIES,  L_GOD },
  { "saccing",    WIZ_SACCING,    L_CON },
  { "levels",     WIZ_LEVELS,     LEVEL_HERO },
  { "load",       WIZ_LOAD,       L_CON },
  { "restore",    WIZ_RESTORE,    L_CON },
  { "snoops",     WIZ_SNOOPS,     L_CON },
  { "switches",   WIZ_SWITCHES,   L_SEN },
  { "secure",     WIZ_SECURE,     L_CON },
  { "oldlog",     WIZ_OLDLOG,     L_DIR },

  { NULL,   0,    0  }
};


/*
 * Attribute bonus tables.
 */
const struct  str_app_type  str_app   [31]    =
{
  { -5, -4,   0,  0 },  /* 0  */
  { -5, -4,   3,  1 },  /* 1  */
  { -3, -2,   3,  2 },
  { -3, -1,  10,  3 },  /* 3  */
  { -2, -1,  25,  4 },
  { -2, -1,  55,  5 },  /* 5  */
  { -1,  0,  80,  6 },
  { -1,  0,  90,  7 },
  {  0,  0, 100,  8 },
  {  0,  0, 100,  9 },
  {  0,  0, 115, 10 }, /* 10  */
  {  0,  0, 115, 11 },
  {  0,  0, 140, 12 },
  {  0,  0, 140, 13 }, /* 13  */
  {  0,  1, 170, 14 },
  {  1,  1, 170, 15 }, /* 15  */
  {  1,  2, 195, 16 },
  {  2,  3, 220, 22 },
  {  2,  4, 250, 25 }, /* 18  */
  {  3,  5, 400, 30 },
  {  3,  6, 500, 35 }, /* 20  */
  {  4,  7, 600, 40 },
  {  5,  7, 700, 45 },
  {  6,  8, 800, 50 },
  {  8, 10, 900, 55 },
  { 10, 12, 999, 60 },  /* 25   */
  { 12, 14, 999, 60 },  
  { 14, 16, 999, 60 },  
  { 16, 18, 999, 60 },  /* 28 */
  { 18, 20, 999, 60 },  
  { 20, 22, 999, 60 }  /* 30   */

};



const struct  int_app_type  int_app   [31]    =
{
  {  3 }, /*  0 */
  {  5 }, /*  1 */
  {  7 },
  {  8 }, /*  3 */
  {  9 },
  { 10 }, /*  5 */
  { 11 },
  { 12 },
  { 13 },
  { 15 },
  { 17 }, /* 10 */
  { 19 },
  { 22 },
  { 25 },
  { 28 },
  { 31 }, /* 15 */
  { 34 },
  { 37 },
  { 40 }, /* 18 */
  { 44 },
  { 49 }, /* 20 */
  { 55 },
  { 60 },
  { 70 },
  { 85 },
  { 99 }, /* 25 */
  { 99 }, 
  { 99 }, 
  { 99 }, 
  { 99 }, 
  { 99 }  /* 30 */
};



const struct  wis_app_type  wis_app   [31]    =
{
  { 0 },  /*  0 */
  { 0 },  /*  1 */
  { 0 },
  { 0 },  /*  3 */
  { 0 },
  { 1 },  /*  5 */
  { 1 },
  { 1 },
  { 1 },
  { 2 },
  { 2 },  /* 10 */
  { 2 },
  { 2 },
  { 2 },
  { 2 },
  { 3 },  /* 15 */
  { 3 },
  { 4 },
  { 4 },  /* 18 */
  { 5 },
  { 5 },  /* 20 */
  { 6 },
  { 7 },
  { 7 },
  { 7 },
  { 8 },  /* 25 */
  { 8 },  
  { 8 },  
  { 9 },  
  { 9 },  
  { 10 }  /* 30 */
};



const struct  dex_app_type  dex_app   [31]    =
{
  {   60 },   /* 0 */
  {   50 },   /* 1 */
  {   50 },
  {   40 },
  {   30 },
  {   20 },   /* 5 */
  {   10 },
  {    0 },
  {    0 },
  {    0 },
  {    0 },   /* 10 */
  {    0 },
  {    0 },
  {    0 },
  {    0 },
  { - 10 },   /* 15 */
  { - 15 },
  { - 20 },
  { - 30 },
  { - 40 },
  { - 50 },   /* 20 */
  { - 65 },
  { - 75 },
  { - 90 },
  { -L_SEN },
  { -120 },    /* 25 */
  { -140 },    
  { -160 },    
  { -180 },    
  { -200 },    
  { -220 }    /* 30 */
};



const struct  con_app_type  con_app   [31]    =
{
  { -4, 20 },   /*  0 */
  { -3, 25 },   /*  1 */
  { -2, 30 },
  { -2, 35 },   /*  3 */
  { -1, 40 },
  { -1, 45 },   /*  5 */
  { -1, 50 },
  {  0, 55 },
  {  0, 60 },
  {  0, 65 },
  {  0, 70 },   /* 10 */
  {  0, 75 },
  {  0, 80 },
  {  0, 85 },
  {  0, 88 },
  {  1, 90 },   /* 15 */
  {  2, 95 },
  {  2, 97 },
  {  3, 99 },   /* 18 */
  {  3, 99 },
  {  4, 99 },   /* 20 */
  {  4, 99 },
  {  5, 99 },
  {  6, 99 },
  {  7, 99 },
  {  8, 99 },    /* 25 */
  {  8, 99 },    
  {  9, 99 },    
  {  9, 99 },    
  {  10, 99 },    
  {  10, 99 }    /* 30 */
};



/*
 * Liquid properties.
 * Used in world.obj.
 */
const struct  liq_type  liq_table [LIQ_MAX] =
{
  { "water",      "clear",  {  0, 0, 10 } },  /*  0 */
  { "beer",     "amber",  {  3, 2,  5 } },
  { "wine",     "rose",   {  4, 2,  5 } },
  { "ale",      "brown",  {  2, 2,  5 } },
  { "dark ale",   "dark",   {  1, 2,  5 } },

  { "whisky",     "golden", {  8, 1,  4 } },  /*  5 */
  { "lemonade",   "pink",   {  0, 1,  8 } },
  { "firebreather",   "boiling",  { 10, 0,  0 } },
  { "local specialty",  "everclear",  {  3, 3,  3 } },
  { "slime mold juice", "green",  {  0, 4, -8 } },

  { "milk",     "white",  {  0, 3,  6 } },  /* 10 */
  { "tea",      "tan",    {  0, 1,  6 } },
  { "coffee",     "black",  {  0, 1,  6 } },
  { "blood",      "red",    {  0, 2, -1 } },
  { "salt water",   "clear",  {  0, 1, -2 } },

  { "cola",     "cherry", {  0, 1,  5 } }   /* 15 */
};


#define SLOT(s) s
const struct  skill_type  skill_table [ MAX_SKILL ] =
{

  /*
   * Magic spells.
   */

  /*
     {
     "Name",
     {CAS,ROG,FIG}
     function,    target type,    position,
     gsn,     min mana, wait,
     "damage name",   "off name", "room see spell ends", 
     dispelable, SLOT(slot)
     },
     */

  {
    "reserved",
    { 999, 999, 999 },
    0,      TAR_IGNORE,   POS_STANDING,
    NULL,     0,   0,
    "",     "", "", FALSE, SLOT(0)
  },

  {
    "acid blast",
    { L_APP, L_APP, L_APP },
    spell_acid_blast, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     20, 12,
    "&gacid blast&X",   "!Acid Blast!", "", FALSE, SLOT(0)
  },

  {
    "animate dead",
    { L_APP, L_APP, L_APP },
    spell_animate,    TAR_OBJ_INV,    POS_STANDING,
    NULL,     100,  24,
    "",     "!Animate Dead!", "", FALSE, SLOT(0)
  },

  {
    "armor",
    { L_APP, L_APP, L_APP },
    spell_armor,    TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,     5,  12,
    "",
    "You feel less protected.",
    "$n looks less protected.",
    TRUE, SLOT(1)
  },

  {
    "astral walk",
    { L_APP, L_APP, L_APP },
    spell_astral,   TAR_IGNORE,   POS_STANDING,
    NULL,     50, 12,
    "",     "!Astral Walk!", "", FALSE, SLOT(0)
  },

  {
    "bless",
    { L_APP, L_APP, L_APP },
    spell_bless,    TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,     5,  12,
    "",
    "You feel less righteous.",
    "$n looks less righteous.",
    FALSE, SLOT(2)
  },

  {
    "blindness",
    { L_APP, L_APP, L_APP },
    spell_blindness,  TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_blindness,   5,  12,
    "",
    "You can see again.",
    "$n is no longer blinded",
    TRUE, SLOT(3)
  },

  {
    "bloodbath",
    { L_APP, L_APP, L_APP },
    spell_blood_bath, TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,           100,  12,
    "",     "!Blood Bath!", "", FALSE, SLOT(0)
  },

  {
    "burning hands",
    { L_APP, L_APP, L_APP },
    spell_burning_hands,  TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     15, 12,
    "&rbu&Yrni&Wng &rh&Oa&Yn&Wds&X","!Burning Hands!", "", FALSE, SLOT(0)
  },

  {
    "call lightning",
    { L_APP, L_APP, L_APP },
    spell_call_lightning, TAR_IGNORE,   POS_FIGHTING,
    NULL,     15, 12,
    "&Blightning bolt&X", "!Call Lightning!", "", FALSE, SLOT(0)
  },

  {
    "cause critical",
    { L_APP, L_APP, L_APP },
    spell_cause_critical, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     20, 12,
    "spell",    "!Cause Critical!", "", FALSE, SLOT(0)
  },

  {
    "cause light",
    { L_APP, L_APP, L_APP },
    spell_cause_light,  TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     15, 12,
    "&wspell&X",    "!Cause Light!", "", FALSE, SLOT(0)
  },

  {
    "cause serious",
    { L_APP, L_APP, L_APP },
    spell_cause_serious,  TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     17, 12,
    "&wspell&X",    "!Cause Serious!", "", FALSE, SLOT(0)
  },

  {
    "change sex",
    { L_APP, L_APP, L_APP },
    spell_change_sex, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
    NULL,     15, 12,
    "",
    "Your body feels familiar again.",
    "$n looks more like $mself again.",
    TRUE, SLOT(4)
  },

  {
    "charm person",
    { L_APP, L_APP, L_APP },
    spell_charm_person, TAR_CHAR_OFFENSIVE, POS_STANDING,
    &gsn_charm_person,  5,  12,
    "",
    "You feel more self-confident.",
    "$n regains $s free will.",
    TRUE, SLOT(5)
  },

  {
    "chill touch",
    { L_APP, L_APP, L_APP },
    spell_chill_touch,  TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     15, 12,
    "&Cch&wil&Cli&wng &Cto&wu&Cch&X",
    "You feel less cold.",
    "$n looks warmer.", TRUE, SLOT(6)
  },

  {
    "colour spray",
    { L_APP, L_APP, L_APP },
    spell_colour_spray, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     15, 12,
    "&rc&go&cl&Yo&Gu&Rr &rsp&gr&Ba&Gy&X","!Colour Spray!", "",
    FALSE, SLOT(0)
  },

  {
    "continual light",
    { L_APP, L_APP, L_APP },
    spell_continual_light,      TAR_IGNORE,             POS_STANDING,
    NULL,     7,  12,
    "",     "!Continual Light!", "", FALSE, SLOT(0)
  },

  {
    "control weather",
    { L_APP, L_APP, L_APP },
    spell_control_weather,      TAR_IGNORE,             POS_STANDING,
    NULL,     25, 12,
    "",     "!Control Weather!", "", FALSE, SLOT(0)
  },

  {
    "create food",
    { L_APP, L_APP, L_APP },
    spell_create_food,  TAR_IGNORE,   POS_STANDING,
    NULL,     5,  12,
    "",     "!Create Food!", "", FALSE, SLOT(0)
  },

  {
    "create spring",
    { L_APP, L_APP, L_APP },
    spell_create_spring,  TAR_IGNORE,   POS_STANDING,
    NULL,     20, 12,
    "",     "!Create Spring!", "", FALSE, SLOT(0)
  },

  {
    "create water",
    { L_APP, L_APP, L_APP },
    spell_create_water, TAR_OBJ_INV,    POS_STANDING,
    NULL,     5,  12,
    "",     "!Create Water!", "", FALSE, SLOT(0)
  },

  {
    "cure blindness",
    { L_APP, L_APP, L_APP },
    spell_cure_blindness, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
    NULL,     5,  12,
    "",     "!Cure Blindness!", "", FALSE, SLOT(0)
  },

  {
    "cure critical",
    { L_APP, L_APP, L_APP },
    spell_cure_critical,  TAR_CHAR_DEFENSIVE, POS_FIGHTING,
    NULL,     20, 12,
    "",     "!Cure Critical!", "", FALSE, SLOT(0)
  },

  {
    "cure light",
    { L_APP, L_APP, L_APP },
    spell_cure_light, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
    NULL,     10, 12,
    "",     "!Cure Light!", "", FALSE, SLOT(0)
  },

  {
    "cure poison",
    { L_APP, L_APP, L_APP },
    spell_cure_poison,  TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,     5,  12,
    "",     "!Cure Poison!", "", FALSE, SLOT(0)
  },

  {
    "cure serious",
    { L_APP, L_APP, L_APP },
    spell_cure_serious, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
    NULL,     15, 12,
    "",     "!Cure Serious!", "", FALSE, SLOT(0)
  },

  {
    "curse",
    { L_APP, L_APP, L_APP },
    spell_curse,    TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_curse,   20, 12,
    "curse",  "The curse wears off.", "$n becomes pure again.",
    TRUE, SLOT(7)
  },

  {
    "detect evil",
    { L_APP, L_APP, L_APP },
    spell_detect_evil,  TAR_CHAR_SELF,    POS_STANDING,
    NULL,     5,  12,
    "",     "The red in your vision disappears.", 
    "$n's vision is restored to normal.", TRUE, SLOT(8)
  },

  {
    "detect hidden",
    { L_APP, L_APP, L_APP },
    spell_detect_hidden,  TAR_CHAR_SELF,    POS_STANDING,
    NULL,     5,  12,
    "", "You feel less aware of your suroundings.", 
    "$n feels less aware of $s surroundings.", TRUE, SLOT(9)
  },

  {
    "detect invis",
    { L_APP, L_APP, L_APP },
    spell_detect_invis, TAR_CHAR_SELF,    POS_STANDING,
    NULL,     5,  12,
    "", "You no longer see invisible objects.", 
    "$n's vision returns to normal.", TRUE, SLOT(10)
  },

  {
    "detect magic",
    { L_APP, L_APP, L_APP },
    spell_detect_magic, TAR_CHAR_SELF,    POS_STANDING,
    NULL,     5,  12,
    "",     "The detect magic wears off.", 
    "$n's vision returns to normal.", TRUE, SLOT(11)
  },

  {
    "detect poison",
    { L_APP, L_APP, L_APP },
    spell_detect_poison,  TAR_OBJ_INV,    POS_STANDING,
    NULL,     5,  12,
    "",     "!Detect Poison!", "", TRUE, SLOT(12)
  },

  {
    "dispel evil",
    { L_APP, L_APP, L_APP },
    spell_dispel_evil,  TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     15, 12,
    "dispel &zevil&X",    "!Dispel Evil!", "", FALSE, SLOT(0)
  },

  {
    "dispel magic",
    { L_APP, L_APP, L_APP },
    spell_dispel_magic, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     15, 16,
    "",     "!Dispel Magic!", "", FALSE, SLOT(0)
  },

  {
    "earthquake",
    { L_APP, L_APP, L_APP },
    spell_earthquake, TAR_IGNORE,   POS_FIGHTING,
    NULL,     15, 12,
    "&Oearthquake&X",   "!Earthquake!", "", FALSE, SLOT(0)
  },

  {
    "enchant weapon",
    { L_APP, L_APP, L_APP },
    spell_enchant_weapon, TAR_OBJ_INV,    POS_STANDING,
    NULL,     100,  24,
    "",     "!Enchant Weapon!", "", FALSE, SLOT(0)
  },

  {
    "energy drain",
    { L_APP, L_APP, L_APP },
    spell_energy_drain, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     35, 12,
    "&zene&wrgy d&Wrain&X", "!Energy Drain!", "", FALSE, SLOT(0)
  },

  {
    "faerie fire",
    { L_APP, L_APP, L_APP },
    spell_faerie_fire,  TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     5,  12,
    "faerie fire",
    "The pink aura around you fades away.",
    "$n's outline fades.",
    TRUE, SLOT(13)
  },

  {
    "faerie fog",
    { L_APP, L_APP, L_APP },
    spell_faerie_fog, TAR_IGNORE,   POS_STANDING,
    NULL,     12, 12,
    "faerie fog",   "!Faerie Fog!", "", FALSE, SLOT(0)
  },

  {
    "fireball",
    { L_APP, L_APP, L_APP },
    spell_fireball,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     15, 12,
    "&Rfireball&X",   "!Fireball!", "", FALSE, SLOT(0)
  },

  {
    "fireshield",
    { L_APP, L_APP, L_APP },
    spell_fireshield, TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,     75, 12,
    "",
    "The flames engulfing your body burn out.",
    "The flames about $n's body burn out.",
    TRUE, SLOT(14)
  },

  {
    "flamestrike",
    { L_APP, L_APP, L_APP },
    spell_flamestrike,  TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     20, 12,
    "&rflame&Rstrike",    "!Flamestrike!", "", FALSE, SLOT(0)
  },

  {
    "fly",
    { L_APP, L_APP, L_APP },
    spell_fly,    TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,     10, 18,
    "",
    "You slowly float to the ground.",
    "$n falls to the ground!",
    TRUE, SLOT(15)
  },

  {
    "gate",
    { L_APP, L_APP, L_APP },
    spell_gate,   TAR_CHAR_DEFENSIVE, POS_FIGHTING,
    NULL,     50, 12,
    "",     "!Gate!", "", FALSE, SLOT(0)
  },

  {
    "giant strength",
    { L_APP, L_APP, L_APP },
    spell_giant_strength, TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,     20, 12,
    "",
    "You feel weaker.",
    "$n no longer looks so mighty.", TRUE, SLOT(16)
  },

  {
    "eternal intellect",
    { L_APP, L_APP, L_APP },
    spell_eternal_intellect,       TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                       20,     12,
    "",
    "You mind surges with a superior prowess.",
    "$n looks a bit duller.", TRUE, SLOT(16)
  },

  {
    "goodberry",
    { L_APP, L_APP, L_APP },
    spell_goodberry,  TAR_OBJ_INV,          POS_STANDING,
    NULL,     25, 8,
    "",     "!GOODBERRY!", "", FALSE, SLOT(0)
  },

  {
    "harm",
    { L_APP, L_APP, L_APP },
    spell_harm,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     35, 12,
    "&zharm &wspell&X",   "!Harm!", "", FALSE, SLOT(0)
  },

  {
    "heal",
    { L_APP, L_APP, L_APP },
    spell_heal,   TAR_CHAR_DEFENSIVE, POS_FIGHTING,
    NULL,     50, 12,
    "",     "!Heal!", "", FALSE, SLOT(0)
  },

  {
    "iceshield",
    { L_APP, L_APP, L_APP },
    spell_iceshield,  TAR_CHAR_SELF,    POS_STANDING,
    NULL,     150,  12,
    "",
    "The icy crust about your body melts away.",
    "The icy crust about $n's body melts to a puddle.",
    TRUE, SLOT(17)
  },

  {
    "icestorm",
    { L_APP, L_APP, L_APP },
    spell_icestorm,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     25, 8,
    "&Cicy blast&X",  "!ICESTORM!", "", FALSE, SLOT(0)
  },

  {
    "vibrate",
    { L_APP, L_APP, L_APP },
    spell_vibrate,         TAR_CHAR_DEFENSIVE,  POS_STANDING,
    NULL,     75, 0,
    "",
    "Your body stops vibrating.",
    "$n body stops vibrating.", FALSE, SLOT(18)
  },

  {
    "infravision",
    { L_APP, L_APP, L_APP },
    spell_infravision,  TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,     5,  18,
    "",     "You no longer see in the dark.", "",
    TRUE, SLOT(19)
  },

  {
    "incinerate",
    { L_APP, L_APP, L_APP },
    spell_incinerate, TAR_CHAR_OFFENSIVE, POS_STANDING,
    &gsn_incinerate,  30, 12,
    "&rflames&X","The flames have been extinguished.", 
    "The flames around $n have been extinguished.",TRUE, SLOT(20)
  },

  {
    "invis",
    { L_APP, L_APP, L_APP },
    spell_invis,    TAR_CHAR_DEFENSIVE, POS_STANDING,
    &gsn_invis,   5,  12,
    "",
    "You are no longer invisible.",
    "$n fades into existance.",
    TRUE, SLOT(21)
  },

  {
    "know alignment",
    { L_APP, L_APP, L_APP },
    spell_know_alignment, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
    NULL,     9,  12,
    "",     "!Know Alignment!", "", FALSE, SLOT(0)
  },

  {
    "lightning bolt",
    { L_APP, L_APP, L_APP },
    spell_lightning_bolt, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     15, 12,
    "&Blightning bolt&X", "!Lightning Bolt!", "", FALSE, SLOT(0)
  },

  {
    "locate object",
    { L_APP, L_APP, L_APP },
    spell_locate_object,  TAR_IGNORE,   POS_STANDING,
    NULL,     20, 18,
    "",     "!Locate Object!", "", FALSE, SLOT(0)
  },

  {
    "magic missile",
    { L_APP, L_APP, L_APP },
    spell_magic_missile,  TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     15, 12,
    "&Pmag&pic mis&Psile",  "!Magic Missile!", "", FALSE, SLOT(0)
  },

  {
    "mana",
    { L_APP, L_APP, L_APP },
    spell_mana,           TAR_CHAR_DEFENSIVE,      POS_STANDING,
    NULL,                  0, 0,
    "",                    "!Mana!", "", FALSE, SLOT(0)
  },

  {
    "mass invis",
    { L_APP, L_APP, L_APP },
    spell_mass_invis, TAR_IGNORE,   POS_STANDING,
    &gsn_mass_invis,  20, 24,
    "",
    "!Mass Invis!",
    "$n fades into existance.", TRUE, SLOT(22)
  },

  {
    "pass door",
    { L_APP, L_APP, L_APP },
    spell_pass_door,  TAR_CHAR_SELF,    POS_STANDING,
    NULL,     20, 12,
    "",
    "You feel solid again.",
    "$n becomes soild.", TRUE, SLOT(23)
  },

  {
    "aura of peace",
    { L_APP, L_APP, L_APP },
    spell_aura,       TAR_CHAR_SELF,  POS_STANDING,
    NULL,     100,  12,
    "",
    "The peace aura around your body fades.",
    "The peace aura around $n fades.", FALSE, SLOT(24)
  },

  {
    "permenancy",
    { L_APP, L_APP, L_APP },
    spell_permenancy, TAR_OBJ_INV,    POS_STANDING,
    NULL,     200,  24,
    "",     "!Permenancy!", "", FALSE, SLOT(0)
  },

  {
    "poison",
    { L_APP, L_APP, L_APP },
    spell_poison,   TAR_CHAR_OFFENSIVE, POS_STANDING,
    &gsn_poison,    10, 12,
    "&Gp&go&Gi&gs&Go&gn",
    "You feel less sick.",
    "$n looks better.", TRUE, SLOT(25)
  },

  {
    "portal",
    { L_APP, L_APP, L_APP },
    spell_portal,   TAR_IGNORE,       POS_STANDING,
    NULL,       100,  12,
    "",         "", "", FALSE, SLOT(0)
  },

  {
    "protection evil",
    { L_APP, L_APP, L_APP },
    spell_protection, TAR_CHAR_SELF,    POS_STANDING,
    NULL,     5,  12,
    "",
    "You feel less protected.",
    "$n looks less protected.",
    TRUE, SLOT(26)
  },

  {
    "refresh",
    { L_APP, L_APP, L_APP },
    spell_refresh,    TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,     12, 18,
    "refresh",    "!Refresh!", "", FALSE, SLOT(0)
  },

  {
    "remove curse",
    { L_APP, L_APP, L_APP },
    spell_remove_curse, TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,     5,  12,
    "",     "!Remove Curse!", "", FALSE, SLOT(0)
  },

  {
    "sanctuary",
    { L_APP, L_APP, L_APP },
    spell_sanctuary,  TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,     75, 12,
    "",
    "The white aura around your body fades.",
    "The white aura around $n's body vanishes.", TRUE, SLOT(27)
  },

  {
    "scry",
    { L_APP, L_APP, L_APP },
    spell_scry,   TAR_CHAR_SELF,    POS_STANDING,
    NULL,     35, 20,
    "",     "Your vision returns to normal.", 
    "$n's vision returns to normal.", TRUE, SLOT(0)
  },

  {
    "shield",
    { L_APP, L_APP, L_APP },
    spell_shield,   TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,     12, 18,
    "",
    "Your force shield shimmers then fades away.",
    "The shield protecting $n shimmers then fades away.",
    TRUE, SLOT(28)
  },

  {
    "shocking grasp",
    { L_APP, L_APP, L_APP },
    spell_shocking_grasp, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     15, 12,
    "&Ysh&Bo&Yck&Bin&Yg g&Bra&Ysp", "!Shocking Grasp!", "",
    FALSE, SLOT(0)
  },

  {
    "shockshield",
    { L_APP, L_APP, L_APP },
    spell_shockshield,  TAR_CHAR_SELF,    POS_STANDING,
    NULL,     100,  12,
    "",
    "The electricity flows into the ground.",
    "The electricity about $n's body flows into the ground.",
    TRUE, SLOT(29)
  },

  {
    "sleep",
    { L_APP, L_APP, L_APP },
    spell_sleep,    TAR_CHAR_OFFENSIVE, POS_STANDING,
    &gsn_sleep,   15, 12,
    "",     "You feel less tired.", 
    "$n looks much more refreshed.", TRUE, SLOT(30)
  },

  {
    "spell bind",
    { L_APP, L_APP, L_APP },
    spell_spell_bind, TAR_OBJ_INV,    POS_STANDING,
    NULL,     100,  24,
    "",     "!Spell Bind!", "", FALSE, SLOT(0)
  },

  {
    "stone skin",
    { L_APP, L_APP, L_APP },
    spell_stone_skin, TAR_CHAR_SELF,    POS_STANDING,
    NULL,     12, 18,
    "",
    "Your skin feels soft again.",
    "$n's skin regains its normal texture.",
    TRUE, SLOT(31)
  },

  {
    "summon",
    { L_APP, L_APP, L_APP },
    spell_summon,   TAR_IGNORE,   POS_STANDING,
    NULL,     50, 12,
    "",     "!Summon!", "", FALSE, SLOT(0)
  },

  {
    "teleport",
    { L_APP, L_APP, L_APP },
    spell_teleport,   TAR_CHAR_SELF,    POS_FIGHTING,
    NULL,     35, 12,
    "",     "!Teleport!", "", FALSE, SLOT(0)
  },

  {
    "ventriloquate",
    { L_APP, L_APP, L_APP },
    spell_ventriloquate,  TAR_IGNORE,   POS_STANDING,
    NULL,     5,  12,
    "",     "!Ventriloquate!", "", FALSE, SLOT(0)
  },

  {
    "weaken",
    { L_APP, L_APP, L_APP },
    spell_weaken,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     20, 12,
    "spell",
    "You feel stronger.",
    "$n looks stronger.",
    TRUE, SLOT(32)
  },

  {
    "word of recall",
    { L_APP, L_APP, L_APP },
    spell_word_of_recall, TAR_CHAR_SELF,    POS_RESTING,
    NULL,     5,  12,
    "",     "!Word of Recall!", "", FALSE, SLOT(0)
  },

    /*
     * Dragon breath
     */
  {
    "acid breath",
    { L_APP, L_APP, L_APP },
    spell_acid_breath,  TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     50, 12,
    "&gblast of &Gacid&X",  "!Acid Breath!", "", FALSE, SLOT(0)
  },

  {
    "fire breath",
    { L_APP, L_APP, L_APP },
    spell_fire_breath,  TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     50, 12,
    "&Rblast of &rflame&X", "!Fire Breath!", "", FALSE, SLOT(0)
  },

  {
    "frost breath",
    { L_APP, L_APP, L_APP },
    spell_frost_breath, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     50, 12,
    "&Cblast of &bfrost", "!Frost Breath!", "", FALSE, SLOT(0)
  },

  {
    "gas breath",
    { L_APP, L_APP, L_APP },
    spell_gas_breath, TAR_IGNORE,   POS_FIGHTING,
    NULL,     50, 12,
    "&zblast of &wgas",   "!Gas Breath!", "", FALSE, SLOT(0)
  },

  {
    "lightning breath", 
    { L_APP, L_APP, L_APP },
    spell_lightning_breath, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     50, 12,
    "&cblast of &Blightning", "!Lightning Breath!", "", FALSE, SLOT(0)
  },
  {
    "summon swarm",
    { L_APP, L_APP, L_APP },
    spell_summon_swarm, TAR_CHAR_SELF,    POS_FIGHTING,
    NULL,     50, 12,
    "",     "!Summon Insect!", "", FALSE, SLOT(0)
  },

  {
    "summon pack",
    { L_APP, L_APP, L_APP },
    spell_summon_pack,  TAR_CHAR_SELF,    POS_FIGHTING,
    NULL,     50, 12,
    "",     "!Summon Pack!", "", FALSE, SLOT(0)
  },

  {
    "summon demon",
    { L_APP, L_APP, L_APP },
    spell_summon_demon, TAR_CHAR_SELF,    POS_FIGHTING,
    NULL,     50, 12,
    "",     "!Summon Demon!", "", FALSE, SLOT(0)
  },

    /*
     * Fighter and Rogue skills.
     */
  {
    "backstab",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_STANDING,
    &gsn_backstab,    0,  24,
    "backstab",   "!Backstab!", "", FALSE, SLOT(0)
  },

  {
    "double backstab",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_STANDING,
    &gsn_backstab_2,    0,  24,
    "backstab",   "!Double Backstab!", "", FALSE, SLOT(0)
  },

  {
    "palm",
    { L_APP, L_APP, L_APP },
    spell_null,     TAR_IGNORE, POS_RESTING,
    &gsn_palm,      0, 0,
    "", "!palm", "", FALSE, SLOT(0)
  },

  {
    "bash",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_OFFENSIVE, POS_STANDING,
    &gsn_bash,    0,  24,
    "bash",     "!Bash!", "", FALSE, SLOT(0)
  },

  {
    "disarm",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_disarm,    0,  24,
    "",     "!Disarm!", "", FALSE, SLOT(0)
  },

  {
    "dodge",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_dodge,   0,   0,
    "",     "!Dodge!", "", FALSE, SLOT(0)
  },

  {
    "dual",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE, POS_RESTING,
    &gsn_dual,    0,   0,
    "",     "!DUAL!", "", FALSE, SLOT(0)
  },

  {
    "eighth attack",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_eighth_attack, 0,   0,
    "",     "!eighth Attack!", "", FALSE, SLOT(0)
  },

  {
    "enhanced damage",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_enhanced_damage, 0,   0,
    "",     "!Enhanced Damage!", "", FALSE, SLOT(0)
  },

  {
    "enhanced damage two",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_enhanced_two,  0,   0,
    "",     "!Enhanced Damage Two!", "", FALSE, SLOT(0)
  },

  {
    "enhanced hit",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_enhanced_hit,  0,   0,
    "",     "!Enhanced Hit!", "", FALSE, SLOT(0)
  },

  {
    "feed",     
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_feed,    0,   12,
    "bite",     "!Feed!", "", FALSE, SLOT(0)
  },

  {
    "fifth attack",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_fifth_attack,  0,   0,
    "",     "!Fifth Attack!", "", FALSE, SLOT(0)
  },

  {
    "fourth attack",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_fourth_attack, 0,   0,
    "",     "!FOURTH Attack!", "", FALSE, SLOT(0)
  },

  {
    "hide",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_RESTING,
    &gsn_hide,    0,  12,
    "",
    "You are no longer so difficult to see.",
    "$n has stopped hiding.", FALSE, SLOT(33)
  },

  {
    "iron grip",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,       POS_STANDING,
    &gsn_grip,    0,   8,
    "",
    "You loosen your hold on your weapon.",
    "$n has loosened $s hold on $s weapon.", FALSE, SLOT(34)
  },

  {
    "flury",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_flury,   0,   50,
    "flury",      "!flury!", "", FALSE, SLOT(0)
  },

  {
    "kick",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_kick,    0,   8,
    "kick",     "!Kick!", "", FALSE, SLOT(0)
  },

  {
    "high kick",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_high_kick,   0,   10,
    "high kick",  "!High Kick!", "", FALSE, SLOT(0)
  },

  {
    "jump kick",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_jump_kick,   0,   20,
    "jump kick",  "!Jump Kick!", "", FALSE, SLOT(0)
  },

  {
    "spin kick",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_spin_kick,   0,   25,
    "spin kick",  "!Spin Kick!", "", FALSE, SLOT(0)
  },

  {
    "parry",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_parry,   0,   0,
    "",     "!Parry!", "", FALSE, SLOT(0)
  },

  {
    "peek",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_STANDING,
    &gsn_peek,    0,   0,
    "",     "!Peek!", "", FALSE, SLOT(0)
  },

  {
    "pick lock",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_STANDING,
    &gsn_pick_lock,   0,  12,
    "",     "!Pick!", "", FALSE, SLOT(0)
  },

  {
    "poison weapon",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_OBJ_INV,    POS_STANDING,
    &gsn_poison_weapon, 0,  12,
    "poisonous concoction", "!Poison Weapon!", "", FALSE, SLOT(0)
  },

  {
    "punch",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_punch,   0,   10,
    "punch",    "!punch!", "", FALSE, SLOT(0)
  },

  {
    "jab punch",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_jab_punch,   0,   10,
    "jab",    "!jab!", "", FALSE, SLOT(0)
  },

  {
    "kidney punch",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_kidney_punch,    0,   10,
    "kindey punch", "!kidney punch!", "", FALSE, SLOT(0)
  },

  {
    "cross punch",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_cross_punch,   0,   10,
    "cross punch",  "!cross punch!", "", FALSE, SLOT(0)
  },

  {
    "roundhouse punch",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_roundhouse_punch,    0,   18,
    "roundhouse", "!roundhouse punch!", "", FALSE, SLOT(0)
  },

  {
    "uppercut punch",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_uppercut_punch,    0,   18,
    "uppercut", "!uppercut punch!", "", FALSE, SLOT(0)
  },

  {
    "rescue",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_rescue,    0,  12,
    "",     "!Rescue!", "", FALSE, SLOT(0)
  },

  {
    "second attack",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_second_attack, 0,   0,
    "",     "!Second Attack!", "", FALSE, SLOT(0)
  },

  {
    "seventh attack",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_seventh_attack,  0,   0,
    "",     "!seventh Attack!", "", FALSE, SLOT(0)
  },

  {
    "sixth attack",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_sixth_attack,  0,   0,
    "",     "!sixth Attack!", "", FALSE, SLOT(0)
  },

  {
    "sneak",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_STANDING,
    &gsn_sneak,   0,  12,
    "",     NULL, "", FALSE, SLOT(35)
  },

  {
    "steal",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_STANDING,
    &gsn_steal,   0,  24,
    "",     "!Steal!", "", FALSE, SLOT(0)
  },

  {
    "third attack",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_third_attack,  0,   0,
    "",     "!Third Attack!", "", FALSE, SLOT(0)
  },

    /*
     *  Spells for mega1.are from Glop/Erkenbrand.
     */
  {
    "general purpose",
    { L_APP, L_APP, L_APP },
    spell_general_purpose,  TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   0,      12,
    "general purpose ammo", "!General Purpose Ammo!", "", FALSE, SLOT(0)
  },

  {
    "high explosive",
    { L_APP, L_APP, L_APP },
    spell_high_explosive,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   0,      12,
    "high explosive ammo",  "!High Explosive Ammo!", "", FALSE, SLOT(0)
  },


    /*
     * Psionicist spell/skill definitions
     * Psi class conceived, created, and coded by Thelonius (EnvyMud)
     */

  {
    "adrenaline control",
    { L_APP, L_APP, L_APP },
    spell_adrenaline_control,       TAR_CHAR_SELF,  POS_STANDING,
    NULL,                           6,      12,
    "",
    "The adrenaline rush wears off.",
    "$n has calmed down.", FALSE, SLOT(36)
  },

  {
    "agitation",
    { L_APP, L_APP, L_APP },
    spell_agitation,        TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   10,     12,
    "agitation",            "!Agitation!", "", FALSE, SLOT(0)
  },

  {
    "aura sight",
    { L_APP, L_APP, L_APP },
    spell_aura_sight,       TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                   9,     12,
    "",                     "!Aura Sight!", "", FALSE, SLOT(0)
  },

  {
    "awe",
    { L_APP, L_APP, L_APP },
    spell_awe,              TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
    NULL,                   35,     12,
    "",                     "!Awe!", "", FALSE, SLOT(0)
  },

  {
    "ballistic attack",
    { L_APP, L_APP, L_APP },
    spell_ballistic_attack, TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   5,     12,
    "ballistic attack",     "!Ballistic Attack!", "", FALSE, SLOT(0)
  },

  {
    "biofeedback",
    { L_APP, L_APP, L_APP },
    spell_biofeedback,      TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   75,     12,
    "",
    "Your biofeedback is no longer effective.",
    "$n's body shivers and relaxes.",
    FALSE, SLOT(37)
  },

  {
    "cell adjustment",
    { L_APP, L_APP, L_APP },
    spell_cell_adjustment,  TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   8,     12,
    "",                     "!Cell Adjustment!", "", FALSE, SLOT(0)
  },

  {
    "chameleon power",
    { L_APP, L_APP, L_APP },
    spell_null,             TAR_IGNORE,             POS_STANDING,
    &gsn_chameleon,         0,     12,
    "",                     "!Chameleon Power!", "", FALSE, SLOT(38)
  },

  {
    "chaos field",
    { L_APP, L_APP, L_APP },
    spell_chaosfield, TAR_CHAR_SELF,    POS_STANDING,
    NULL,     150,  12,
    "",
    "The chaos flees from the order that surrounds you.",
    "The chaos flees from the order around $n.", FALSE, SLOT(39)
  },

  {
    "combat mind",
    { L_APP, L_APP, L_APP },
    spell_combat_mind,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                   15,     12,
    "",                     "Your battle sense has faded.",
    "$n's sense of battle has diminished.", FALSE, SLOT(40)
  },

  {
    "complete healing",
    { L_APP, L_APP, L_APP },
    spell_complete_healing, TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   200,    12,
    "",                     "!Complete Healing!", "", FALSE, SLOT(0)
  },

  {
    "control flames",
    { L_APP, L_APP, L_APP },
    spell_control_flames,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   15,     12,
    "tongue of &Rflame&X",      "!Control Flames!", "", FALSE, SLOT(0)
  },

  {
    "create sound",
    { L_APP, L_APP, L_APP },
    spell_create_sound,     TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                   5,     12,
    "",                     "!Create Sound!", "", FALSE, SLOT(0)
  },

  {
    "death field",
    { L_APP, L_APP, L_APP },
    spell_death_field,      TAR_IGNORE,             POS_FIGHTING,
    NULL,                   200,    18,
    "&zfield of death&X",       "!Death Field!", "", FALSE, SLOT(0)
  },

  {
    "detonate",
    { L_APP, L_APP, L_APP },
    spell_detonate,         TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   35,     24,
    "&zdet&Rona&rtion",           "!Detonate!", "", FALSE, SLOT(0)
  },

  {
    "disintegrate",
    { L_APP, L_APP, L_APP },
    spell_disintegrate,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   150,    18,
    "disintegration",       "!Disintegrate!", "", FALSE, SLOT(0)
  },

  {
    "displacement",
    { L_APP, L_APP, L_APP },
    spell_displacement,     TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   10,     12,
    "",
    "You are no longer displaced.",
    "$n's image fades as he appears in another spot.", FALSE, SLOT(41)
  },

  {
    "domination",
    { L_APP, L_APP, L_APP },
    spell_domination,       TAR_CHAR_OFFENSIVE,     POS_STANDING,
    &gsn_domination,        5,     12,
    "",
    "You regain control of your body.",
    "$n regains control of $m body.", FALSE, SLOT(42)
  },

  {
    "ectoplasmic form",
    { L_APP, L_APP, L_APP },
    spell_ectoplasmic_form, TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   20,     12,
    "",
    "You feel solid again.",
    "$n looks solid again.",
    FALSE, SLOT(43)
  },

  {
    "ego whip",
    { L_APP, L_APP, L_APP },
    spell_ego_whip,         TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   20,     12,
    "",
    "You feel more confident.",
    "$n looks more confident.", FALSE, SLOT(44)
  },

  {
    "energy containment",
    { L_APP, L_APP, L_APP },
    spell_energy_containment,       TAR_CHAR_SELF,  POS_STANDING,
    NULL,                           10,     12,
    "",
    "You no longer absorb energy.",
    "$n no longer absorbs energy.", FALSE, SLOT(45)
  },

  {
    "enhance armor",
    { L_APP, L_APP, L_APP },
    spell_enhance_armor,    TAR_OBJ_INV,    POS_STANDING,
    NULL,                   100,    24,
    "",                     "!Enhance Armor!", "", FALSE, SLOT(0)
  },

  {
    "enhanced strength",
    { L_APP, L_APP, L_APP },
    spell_enhanced_strength,        TAR_CHAR_SELF,  POS_STANDING,
    NULL,                           20,     12,
    "",
    "You no longer feel so HUGE.",
    "$n doesn't look so huge.",
    FALSE, SLOT(46)
  },

  {
    "flesh armor",
    { L_APP, L_APP, L_APP },
    spell_flesh_armor,      TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   15,     12,
    "",
    "Your skin returns to normal.",
    "$n's skin returns to its normal texture.", FALSE, SLOT(47)
  },

  {
    "heighten senses",
    { L_APP, L_APP, L_APP },
    spell_null,             TAR_CHAR_SELF,          POS_STANDING,
    &gsn_heighten,          0,      0,
    "",                     "Your senses return to normal.", 
    "$n's senses return to normal.", FALSE, SLOT(48)
  },

  {
    "inertial barrier",
    { L_APP, L_APP, L_APP },
    spell_inertial_barrier, TAR_IGNORE,             POS_STANDING,
    NULL,                   40,     24,
    "",
    "Your inertial barrier dissipates.",
    "$n's inertial barrier dissipates.", FALSE, SLOT(49)
  },

  {
    "inflict pain",
    { L_APP, L_APP, L_APP },
    spell_inflict_pain,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   10,     12,
    "&wmind&Wpower&X",            "!Inflict Pain!", "", FALSE, SLOT(0)
  },

  {
    "intellect fortress",
    { L_APP, L_APP, L_APP },
    spell_intellect_fortress,       TAR_IGNORE,     POS_STANDING,
    NULL,                           25,     24,
    "",
    "Your intellectual fortress crumbles.",
    "$n's intellectual fortress crumbles.",
    FALSE, SLOT(50)
  },

  {
    "lend health",
    { L_APP, L_APP, L_APP },
    spell_lend_health,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                   10,     12,
    "",                     "!Lend Health!", "", FALSE, SLOT(0)
  },

  {
    "levitation",
    { L_APP, L_APP, L_APP },
    spell_levitation,       TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                   10,     18,
    "",
    "You slowly float to the ground.",
    "$n slowly floats to the ground.",
    FALSE, SLOT(51)
  },

  {
    "mental barrier",
    { L_APP, L_APP, L_APP },
    spell_mental_barrier,   TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   8,     12,
    "",                     "Your mental barrier breaks down.", 
    "$n's mental barrier diminishes.", FALSE, SLOT(52)
  },

  {
    "mind thrust",
    { L_APP, L_APP, L_APP },
    spell_mind_thrust,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   8,     12,
    "&wmind &Wthrust&X",          "!Mind Thrust!", "", FALSE, SLOT(0)
  },

  {
    "project force",
    { L_APP, L_APP, L_APP },
    spell_project_force,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   18,     12,
    "&wprojected force&X",      "!Project Force!", "", FALSE, SLOT(0)
  },

  {
    "psionic blast",
    { L_APP, L_APP, L_APP },
    spell_psionic_blast,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   25,     12,
    "&wpsionic blast&X",        "!Psionic Blast!", "", FALSE, SLOT(0)
  },

  {
    "psychic crush",
    { L_APP, L_APP, L_APP },
    spell_psychic_crush,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   15,     18,
    "&wpsychic crush&X",        "!Psychic Crush!", "", FALSE, SLOT(0)
  },

  {
    "psychic drain",
    { L_APP, L_APP, L_APP },
    spell_psychic_drain,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   20,     12,
    "",
    "You no longer feel drained.",
    "$n looks stronger.",
    FALSE, SLOT(53)
  },

  {
    "psychic healing",
    { L_APP, L_APP, L_APP },
    spell_psychic_healing,  TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   20,      12,
    "",                     "!Psychic Healing!", "", FALSE, SLOT(0)
  },

  {
    "shadow form",
    { L_APP, L_APP, L_APP },
    spell_null,             TAR_IGNORE,             POS_STANDING,
    &gsn_shadow,            0,     12,
    "",                     "You no longer move in the shadows.", 
    "$n returns from the shadows.", FALSE, SLOT(54)
  },

  {
    "share strength",
    { L_APP, L_APP, L_APP },
    spell_share_strength,   TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                   8,     12,
    "",                 "You no longer share strength with another.", 
    "$n stops sharing $m strength.", FALSE, SLOT(55)
  },

  {
    "thought shield",
    { L_APP, L_APP, L_APP },
    spell_thought_shield,   TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   5,     12,
    "",                     "You no longer feel so protected.", 
    "$n doesn't feel very protected.", FALSE, SLOT(56)
  },

  {
    "ultrablast",
    { L_APP, L_APP, L_APP },
    spell_ultrablast,       TAR_IGNORE,             POS_FIGHTING,
    NULL,                   75,     24,
    "&zultr&wabl&Wast&X",   "!Ultrablast!" , "", FALSE, 
    SLOT(0)
  },

  {
    "cancellation",
    { L_APP, L_APP, L_APP },
    spell_cancellation, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
    NULL,       15, 16,
    "",                     "!Cancellation!", "", FALSE, SLOT(0)
  },

  {
    "throw",
    { L_APP, L_APP, L_APP },
    spell_null, TAR_CHAR_OFFENSIVE, POS_STANDING,
    &gsn_throw,   0,  0,
    "throw",   "!Throw!", "", FALSE, SLOT(0)
  },


    /*
     * Turn undead and mental block by Altrag
     */
  {
    "turn undead",
    { L_APP, L_APP, L_APP },
    spell_turn_undead, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,              100,     12,
    "",                "!Turn Undead!", "", FALSE, SLOT(0)
  },

  {
    "mental block",
    { L_APP, L_APP, L_APP },
    spell_mental_block, TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,               25,     8,
    "",                 "Your mind feels more vulnerable.", 
    "$n's mind becomes more vulnerable.", FALSE, SLOT(57)
  },

    /*
     * Patch by Altrag
     */
  {
    "patch",
    { L_APP, L_APP, L_APP },
    spell_null,     TAR_IGNORE, POS_STANDING,
    &gsn_patch,     0, 0,
    "",             "!Patch!", "", FALSE, SLOT(0)
  },

  {
    "flamesphere",
    { L_APP, L_APP, L_APP },
    gspell_flamesphere, TAR_GROUP_OFFENSIVE, POS_STANDING,
    NULL,           14, 20,
    "flamesphere",  "!Flamesphere!", "", TRUE, SLOT(58)
  },

  {
    "bash door",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_STANDING,
    &gsn_bash_door,   0,  24,
    "bash",     "!Bash Door!", "", FALSE, SLOT(0)
  },

  {
    "mental drain",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_mental_drain,  0,  12,
    "mental drain",   "!Mental Drain!", "", FALSE, SLOT(0)
  },

  {
    "drain life",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_drain_life,  0,  12,
    "&zlife &wdrain",   "!Drain Life!", "", FALSE, SLOT(0)
  },

  {
    "holy strength",
    { L_APP, L_APP, L_APP },
    spell_holy_strength,  TAR_CHAR_SELF,    POS_STANDING,
    NULL,     100,  12,
    "",     "Your divine strength fades.",  
    "The divine strength surrounding $n fades.", FALSE, SLOT(59)
  },

  {
    "curse of nature",
    { L_APP, L_APP, L_APP },
    spell_curse_of_nature,  TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     100,  12,
    "",   "You feel in sync with nature once again.", 
    "$n feels in sync with nature once again.",TRUE, SLOT(60)
  },

  {
    "detect good",
    { L_APP, L_APP, L_APP },
    spell_detect_good,  TAR_CHAR_SELF,    POS_STANDING,
    NULL,     5,  12,
    "",     "The white in your vision disappears.", 
    "$n's vision returns to normal.", TRUE, SLOT(61)
  },

  {
    "protection good",
    { L_APP, L_APP, L_APP },
    spell_protection_good,  TAR_CHAR_SELF,    POS_STANDING,
    NULL,     5,  12,
    "",     "You feel less protected.", 
    "$n looks less protected.", TRUE, SLOT(62)
  },

  {
    "shield block",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_shield_block,  0,   0,
    "",     "!Shield Block!", "", FALSE, SLOT(0)
  },

  {
    "enchantment",
    { L_APP, L_APP, L_APP },
    spell_enchanted_song, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     100,  12,
    "",     "The peace leaves you.",  
    "The peace within $n disappears.", FALSE, SLOT(0)
  },

  {
    "turn evil",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_turn_evil,   0,  12,
    "&Wdivine power&X",   "!Turn Evil!",  "", FALSE, SLOT(0)
  },

  {
    "mass shield",
    { L_APP, L_APP, L_APP },
    gspell_mass_shield,     TAR_GROUP_ALL,    POS_STANDING,
    NULL,              80,  12,
    "",     "!Mass Shield!", "", TRUE, SLOT(63)
  },

  {
    "gouge",
    { L_APP, L_APP, L_APP },
    spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_gouge,             0,      12,
    "gouge",                "!Gouge!", "", FALSE, SLOT(0)
  },

  {
    "holy sword", 
    { L_APP, L_APP, L_APP },
    spell_holysword,         TAR_OBJ_INV,        POS_STANDING,
    NULL,     100,  24,
    "",     "!Holy Sword!", "", FALSE, SLOT(0)
  },

  {
    "summon angel",
    { L_APP, L_APP, L_APP },
    spell_summon_angel, TAR_CHAR_SELF,    POS_FIGHTING,
    NULL,     100,  12,
    "",     "!Summon Angel!", "", FALSE, SLOT(0)
  },

  {
    "holy fires",
    { L_APP, L_APP, L_APP },
    spell_holy_fires, TAR_CHAR_OFFENSIVE,      POS_FIGHTING,
    NULL,     50, 12,
    "&Who&Rly fl&rames&X",    "!Holy Fires!", "", FALSE, SLOT(0)
  },

  {
    "true sight",
    { L_APP, L_APP, L_APP },
    spell_truesight,  TAR_CHAR_SELF,    POS_STANDING,
    NULL,     59, 12,
    "",     "Your vision is no longer so true.", 
    "$n's vision returns to normal.", TRUE, SLOT(64)
  },

  {
    "blade barrier",
    { L_APP, L_APP, L_APP },
    spell_bladebarrier, TAR_CHAR_SELF,    POS_STANDING,
    NULL,     150,  12,
    "",     "The phantom blades dissipate into the air.", 
    "The phantom blades surrounding $n disappear.", TRUE, SLOT(65)
  },

  { 
    "alchemy",
    { L_APP, L_APP, L_APP },
    spell_null,              TAR_IGNORE,             POS_STANDING,
    &gsn_alchemy,             0,  50,
    "",                      "!ALCHEMY!", "", FALSE, SLOT(0)
  },      

  { 
    "scribe",
    { L_APP, L_APP, L_APP },
    spell_null,              TAR_IGNORE,             POS_STANDING,
    &gsn_scribe,             0,  50,
    "",                      "!SCRIBE!", "", FALSE, SLOT(0)
  },

  {
    "flame blade", 
    { L_APP, L_APP, L_APP },
    spell_flame_blade,       TAR_OBJ_INV,        POS_STANDING,
    NULL,     100,  24,
    "",     "!Flame Blade!", "", FALSE, SLOT(0)
  },

  {
    "chaos blade", 
    { L_APP, L_APP, L_APP },
    spell_chaos_blade,       TAR_OBJ_INV,        POS_STANDING,
    NULL,     100,  24,
    "",     "!Chaos Blade!", "", FALSE, SLOT(0)
  },

  {
    "frost blade", 
    { L_APP, L_APP, L_APP },
    spell_frost_blade,       TAR_OBJ_INV,        POS_STANDING,
    NULL,     100,  24,
    "",     "!Frost Blade!", "", FALSE, SLOT(0)
  },

  {
    "stun",
    { L_APP, L_APP, L_APP },
    spell_null,           TAR_IGNORE,           POS_FIGHTING,
    &gsn_stun,            0, 0,
    "", "!Stun!", "", FALSE, SLOT(0)
  },

  {
    "berserk",
    { L_APP, L_APP, L_APP },
    spell_null,       TAR_IGNORE,       POS_FIGHTING,
    &gsn_berserk,     0, 0,
    "",        "You feel more sane.", 
    "$n has regained $s sanity.", FALSE, SLOT(0)
  },

  {
    "web", 
    { L_APP, L_APP, L_APP },
    spell_web,         TAR_CHAR_OFFENSIVE,        POS_FIGHTING,      
    NULL,     25, 14,
    "",     "The webs melt away.", 
    "The webs entangling $n melt away.", TRUE, SLOT(66)
  },

  {
    "entangle", 
    { L_APP, L_APP, L_APP },
    spell_entangle,          TAR_CHAR_OFFENSIVE,        POS_FIGHTING,
    NULL,     25, 14,
    "",     "The vines release their grasp.", 
    "The vines unwrap themselves from $n.", TRUE, SLOT(67)
  },

  {
    "dark blessing",
    { L_APP, L_APP, L_APP },
    spell_darkbless,       TAR_CHAR_DEFENSIVE,        POS_STANDING,
    NULL,     50, 24,
    "",     "The sweet caress of oblivion has lifted.", 
    "The sweet caress of oblivion rises from $n.", FALSE, SLOT(68)
  },
    /* 250 */
  {
    "bio-acceleration", 
    { L_APP, L_APP, L_APP },
    spell_bio_acceleration,  TAR_CHAR_SELF,        POS_STANDING,
    NULL,     50, 24,
    "",     "Your body structure returns to normal.",     
    "$n's body returns to a normal form.", FALSE, SLOT(69)
  },

  {
    "confusion", 
    { L_APP, L_APP, L_APP },
    spell_confusion,       TAR_CHAR_OFFENSIVE,        POS_FIGHTING,
    NULL,     50, 24,
    "",   "You become more sure of your surroundings.", 
    "$n looks less confused.", TRUE, SLOT(70)
  },

  {
    "mind probe", 
    { L_APP, L_APP, L_APP },
    spell_mind_probe,       TAR_CHAR_DEFENSIVE,        POS_STANDING,
    NULL,     50, 24,
    "",     "!MIND PROBE!", "", FALSE, SLOT(0)
  },

  {
    "haste",
    { L_APP, L_APP, L_APP },
    spell_haste,    TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,     30, 12,
    "",     "You feel yourself slow down",
    "$n slows down.", TRUE, SLOT(71)
  },

  {
    "trip",
    { L_APP, L_APP, L_APP },
    spell_null,             TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_trip,              0,      12,
    "trip",                "!Trip!", "", FALSE, SLOT(0)
  },

  {
    "dirt kick",
    { L_APP, L_APP, L_APP },
    spell_null,             TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_dirt_kick,   0,      12,
    "dirt kick",    "!Dirt Kick!", "", FALSE, SLOT(0)
  },

  {
    "chain lightning",
    { L_APP, L_APP, L_APP },
    spell_chain_lightning,  TAR_IGNORE,   POS_FIGHTING,
    NULL,     50, 22,
    "&Belectrical blast&X",     "!CHAIN LIGHTNING!", "", FALSE, SLOT(0)
  },

  {
    "meteor swarm",
    { L_APP, L_APP, L_APP },
    spell_meteor_swarm, TAR_IGNORE,   POS_FIGHTING,
    NULL,     150,  25,
    "&rm&Oe&Yt&Oe&ro&Yr&rs",        "!SWARM!", "", TRUE, SLOT(72)
  },

  {
    "psychic quake",
    { L_APP, L_APP, L_APP },
    spell_psychic_quake,  TAR_IGNORE,   POS_FIGHTING,
    NULL,     150,  25,
    "&wmental shockwave&X",     "!PSY-QUAKE!", "", FALSE, SLOT(0)
  },

  {
    "fumble", 
    { L_APP, L_APP, L_APP },
    spell_fumble,       TAR_CHAR_OFFENSIVE,        POS_STANDING,
    NULL,     50, 24,
    "",     "You feel more sure of hand.", 
    "$n looks more confident.", TRUE, SLOT(73)
  },

  {
    "dancing lights",
    { L_APP, L_APP, L_APP },
    spell_dancing_lights, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     50, 12,
    "dancing lights","The dancing lights about your body fade away.", 
    "Dancing lights about $n fade away.", FALSE, SLOT(74)
  },

  {
    "shadow walk",
    { L_APP, L_APP, L_APP },
    spell_null,             TAR_IGNORE,             POS_STANDING,
    &gsn_shadow_walk,       0,     22,
    "",                     "!SHADOW WALK!", "", FALSE, SLOT(0)
  },

  {
    "circle",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_circle,    0,   12,
    "sneak attack",   "!CIRCLE!", "", FALSE, SLOT(0)
  },

  {
    "summon shadow",
    { L_APP, L_APP, L_APP },
    spell_summon_shadow,  TAR_CHAR_SELF,    POS_FIGHTING,
    NULL,     100,  12,
    "",     "!Summon Shadow!", "", FALSE, SLOT(0)
  },

  {
    "summon beast",
    { L_APP, L_APP, L_APP },
    spell_summon_beast, TAR_CHAR_SELF,    POS_FIGHTING,
    NULL,     100,  12,
    "",     "!Summon Beast!", "", FALSE, SLOT(0)
  },

  {
    "summon treant",
    { L_APP, L_APP, L_APP },
    spell_summon_trent, TAR_CHAR_SELF,    POS_FIGHTING,
    NULL,     100,  12,
    "",     "!Summon Trent!", "", FALSE, SLOT(0)
  },

  {
    "shatter",
    { L_APP, L_APP, L_APP },
    spell_shatter,    TAR_CHAR_OFFENSIVE,   POS_FIGHTING,
    NULL,           150,  32,
    "",     "!shatter!", "", FALSE, SLOT(0)
  },

  {
    "molecular unbinding",
    { L_APP, L_APP, L_APP },
    spell_molecular_unbind, TAR_CHAR_OFFENSIVE,   POS_FIGHTING,
    NULL,     100,  32,
    "",     "!shatter!", "", FALSE, SLOT(0)
  },

  {
    "phase shift",
    { L_APP, L_APP, L_APP },
    spell_phase_shift,  TAR_CHAR_SELF,    POS_STANDING,
    NULL,     100,  12,
    "",     "You shift back into the physical plane.", 
    "$n's body shifts back into the existing world.", FALSE, SLOT(75)
  },

  {
    "disrupt",
    { L_APP, L_APP, L_APP },
    spell_disrupt,        TAR_CHAR_OFFENSIVE,       POS_FIGHTING,
    NULL,                 125,     12,
    "disruption",         "You feel less disrupted.", 
    "$n looks more stable.", TRUE, SLOT(76)
  },

  {
    "soulstrike",
    { L_APP, L_APP, L_APP },
    spell_null,           TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    &gsn_soulstrike,      0,      24,
    "soul",        "!Soulstrike!",   "", FALSE, SLOT(0)
  },

  {
    "healing hands",
    { L_APP, L_APP, L_APP },
    spell_healing_hands,      TAR_CHAR_DEFENSIVE,    POS_STANDING,
    NULL,              40,     12,
    "",         "!Healing Hands!",    "", FALSE, SLOT(0)
  },

  {
    "pray",
    { L_APP, L_APP, L_APP },
    spell_null,           TAR_CHAR_SELF,    POS_RESTING,
    &gsn_prayer,       0, 12,
    "",   "Thalador's blessing leaves your soul.",   
    "Thalador's blessing rises from $n's soul.", FALSE, SLOT(77)
  },
  {
    "spellcraft",
    { L_APP, L_APP, L_APP },
    spell_null,     TAR_IGNORE, POS_FIGHTING,
    &gsn_spellcraft,  0, 0,
    "",    "!Spellcraft!", "", FALSE, SLOT(0)
  },
  {
    "multiburst",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_multiburst,  0, 8,
    "",   "!Multiburst!", "", FALSE, SLOT(0)
  },
    /* Necromancer spells by Hannibal. */
  {
    "hex",
    { L_APP, L_APP, L_APP },
    spell_hex,     TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
    NULL,   75, 12,
    "", "The hex placed upon your soul wears off.", 
    "The hex placed upon $n's soul vanishes.", TRUE, SLOT(78)
  },
  {
    "dark ritual",
    { L_APP, L_APP, L_APP },
    spell_dark_ritual,  TAR_IGNORE,   POS_STANDING,
    NULL,   5, 12,
    "",     "!Dark Ritual!", "", FALSE, SLOT(0)
  },
  {
    "gravebind",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_OBJ_INV,    POS_RESTING,
    &gsn_gravebind,   0, 0,
    "",     "!Gravebind!", "", FALSE, SLOT(0)
  },
  {
    "field of decay",
    { L_APP, L_APP, L_APP },
    spell_field_of_decay, TAR_CHAR_SELF, POS_RESTING,
    NULL,     300, 12,
    "",   "The black haze enveloping your body dissipates.", "",
    TRUE, SLOT(79)
  },
  {
    "stench of decay",
    { L_APP, L_APP, L_APP },
    spell_stench_of_decay,  TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     15, 12,
    "&zstench of decay&X", "!Stench of Decay!", "", FALSE, SLOT(0)
  },
    /* Werewolf starts here -- Hannibal */
  {
    "enhanced claws",
    { L_APP, L_APP, L_APP },
    spell_null,     TAR_IGNORE, POS_FIGHTING,
    &gsn_enhanced_claw,   0, 0,
    "", "!Enhanced Claws!", "", FALSE, SLOT(0)
  },
  {
    "dual claws",
    { L_APP, L_APP, L_APP },
    spell_null,     TAR_IGNORE, POS_FIGHTING,
    &gsn_dualclaw,    0, 0,
    "", "!Dual Claws!", "", FALSE, SLOT(0)
  },
  {
    "fast healing",
    { L_APP, L_APP, L_APP },
    spell_null,     TAR_IGNORE, POS_SLEEPING,
    &gsn_fastheal,    0, 0,
    "", "!Fast Healing!", "", FALSE, SLOT(0)
  },
  {
    "rage",
    { L_APP, L_APP, L_APP },
    spell_null,     TAR_IGNORE, POS_RESTING,
    &gsn_rage,      0, 24,
    "", "You calm down as the rage leaves you.",
    "$n calms down as the rage leaves $m.", FALSE, SLOT(80)
  },
  {
    "bite",
    { L_APP, L_APP, L_APP },
    spell_null,     TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_bite,      0, 12,
    "vicious bite", "!Bite!", "", FALSE, SLOT(0)
  }, 
  {
    "adrenaline rush",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE, POS_RESTING,
    &gsn_rush,      0, 12,
    "",
    "You feel the rush of adrenaline wash away.", 
    "$n has calmed down.", FALSE, SLOT(81)
  },

  {
    "howl of fear",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_howlfear,    0, 24,
    "", "You shake off the terror and gather your senses.",
    "$n doesn't look as terrified anymore.", FALSE, SLOT(82)
  },

  {
    "scent",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_RESTING,
    &gsn_scent,     0, 0,
    "", "!Scent!", "", FALSE, SLOT(0)
  },
  {
    "frenzy",
    { L_APP, L_APP, L_APP },
    spell_null,       TAR_IGNORE,       POS_FIGHTING,
    &gsn_frenzy,     0, 0,
    "",        "You feel more sane.",
    "$n looks more sane.", FALSE, SLOT(82)
  },
  {
    "enhanced reflexes",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_SELF,  POS_STANDING,
    &gsn_reflex,  0, 36,
    "", "Your reflexes slow down.", "$n slows down.", FALSE, SLOT(84)
  },
  {
    "rake eyes",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_OFFENSIVE, POS_STANDING,
    &gsn_rake,    0, 36,
    "eye rake", "!rake!", "", FALSE, SLOT(0)
  },

  {
    "song of sanctuary",
    { L_APP, L_APP, L_APP },
    spell_sanctuary,    TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                       75,     12,
    "",                 "The white aura around your body fades.", 
    "The white aura surrounding $n fades away.", TRUE, SLOT(85)
  },

  {
    "missiles",
    { L_APP, L_APP, L_APP },
    spell_magic_missile,        TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                       15,     12,
    "&Pmag&pic mis&Psile",  "!Magic Missile!", "", FALSE, SLOT(0)
  },

  {
    "voices",
    { L_APP, L_APP, L_APP },
    spell_ventriloquate,        TAR_IGNORE,             POS_STANDING,
    NULL,                       5,      12,
    "",                 "!Ventriloquate!", "", FALSE, SLOT(0)
  },

  {
    "song of evil sight",
    { L_APP, L_APP, L_APP },
    spell_detect_evil,  TAR_CHAR_SELF,          POS_STANDING,
    NULL,                       5,      12,
    "",                 "The red in your vision disappears.", 
    "$n's vision returns to normal.", TRUE, SLOT(86)
  },

  {
    "icy touch",
    { L_APP, L_APP, L_APP },
    spell_chill_touch,  TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                       15,     12,
    "&Cch&wil&Cli&wng &Cto&wu&Cch&X",
    "You feel less cold.", "$n looks less cold.", TRUE, SLOT(87)
  },

  {
    "song of alignment",
    { L_APP, L_APP, L_APP },
    spell_know_alignment,       TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
    NULL,                       9,      12,
    "",                 "!Know Alignment!", "", FALSE, SLOT(0)
  },

  {
    "night vision",
    { L_APP, L_APP, L_APP },
    spell_infravision,  TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                       5,      18,
    "",                 "You no longer see in the dark.", 
    "$n's vision returns to normal.", TRUE, SLOT(88)
  },

  {
    "protection",
    { L_APP, L_APP, L_APP },
    spell_armor,                TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                       5,      12,
    "",                 "You feel less protected.", 
    "$n looks less protected.", TRUE, SLOT(89)
  },

  {
    "fiery touch",
    { L_APP, L_APP, L_APP },
    spell_burning_hands,        TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                       15,     12,
    "&rbu&Yrni&Wng &rh&Oa&Yn&Wds&X","!Burning Hands!", "", FALSE, SLOT(0)
  },

  {
    "soothing",
    { L_APP, L_APP, L_APP },
    spell_cure_light,   TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
    NULL,                       10,     12,
    "",                 "!Cure Light!", "", FALSE, SLOT(0)
  },

  {
    "sight",
    { L_APP, L_APP, L_APP },
    spell_detect_magic, TAR_CHAR_SELF,          POS_STANDING,
    NULL,                       5,      12,
    "",                 "The detect magic wears off.", 
    "$n's vision returns to normal.", TRUE, SLOT(90)
  },

  {
    "flight",
    { L_APP, L_APP, L_APP },
    spell_fly,          TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                       10,     18,
    "",                 "You slowly float to the ground.", 
    "$n slowly floats to the ground.", TRUE, SLOT(91)
  },

  {
    "shocking",
    { L_APP, L_APP, L_APP },
    spell_shocking_grasp,       TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                       15,     12,
    "&Ysh&Bo&Yck&Bin&Yg g&Bra&Ysp", "!Shocking Grasp!", "",
    FALSE, SLOT(0)
  },

  {
    "enticement",
    { L_APP, L_APP, L_APP },
    spell_charm_person, TAR_CHAR_OFFENSIVE,     POS_STANDING,
    &gsn_charm_person,  5,      12,
    "",                 "You feel more self-confident.", 
    "$n looks more confident.", TRUE, SLOT(92)
  },

  {
    "vision",
    { L_APP, L_APP, L_APP },
    spell_scry,         TAR_CHAR_SELF,          POS_STANDING,
    NULL,                       35,     20,
    "",                 "Your vision returns to normal.", 
    "$n's vision returns to normal.", TRUE, SLOT(93)
  },

  {
    "giants",
    { L_APP, L_APP, L_APP },
    spell_giant_strength,       TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                       20,     12,
    "",                 "You feel weaker.", 
    "$n looks much weaker.", TRUE, SLOT(94)
  },

  {
    "seeing",
    { L_APP, L_APP, L_APP },
    spell_detect_invis, TAR_CHAR_SELF,          POS_STANDING,
    NULL,                       5,      12,
    "",                 "You no longer see invisible objects.", 
    "$n's vision returns to normal.", TRUE, SLOT(95)
  },

  {
    "flames",
    { L_APP, L_APP, L_APP },
    spell_flamestrike,  TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                       20,     12,
    "&rflame&Rstrike",    "!Flamestrike!", "", FALSE, SLOT(0)
  },

  {
    "location",
    { L_APP, L_APP, L_APP },
    spell_locate_object,        TAR_IGNORE,             POS_STANDING,
    NULL,                       20,     18,
    "",                 "!Locate Object!", "", FALSE, SLOT(0)
  },

  {
    "invisibility",
    { L_APP, L_APP, L_APP },
    spell_invis,                TAR_CHAR_DEFENSIVE,     POS_STANDING,
    &gsn_invis,         5,      12,
    "",                 "You are no longer invisible.", 
    "$n appears from thin air.", TRUE, SLOT(96)
  },

  {
    "homeward bound",
    { L_APP, L_APP, L_APP },
    spell_word_of_recall,       TAR_CHAR_SELF,          POS_RESTING,
    NULL,                       5,      12,
    "",                 "!Word of Recall!", "", FALSE, SLOT(0)
  },

  {
    "poison sight",
    { L_APP, L_APP, L_APP },
    spell_detect_poison,        TAR_OBJ_INV,            POS_STANDING,
    NULL,                       5,      12,
    "",                 "!Detect Poison!", "", TRUE, SLOT(97)
  },

  {
    "shielding",
    { L_APP, L_APP, L_APP },
    spell_shield,               TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                       12,     18,
    "",                 "Your force shield shimmers then fades away.", 
    "The force shield surrounding $n shimmers and fades away.", FALSE, SLOT(98)
  },

  {
    "boosting",
    { L_APP, L_APP, L_APP },
    spell_adrenaline_control,       TAR_CHAR_SELF,  POS_STANDING,
    NULL,                           6,      12,
    "",                             "The adrenaline rush wears off.", 
    "$n doesn't seem so pumped up anymore.", FALSE, SLOT(99)
  },

  {
    "song of acid blast",
    { L_APP, L_APP, L_APP },
    spell_acid_blast,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                       20,     12,
    "&gacid blast&X",   "!Acid Blast!", "", FALSE, SLOT(0)
  },

  {
    "song of colour spray",
    { L_APP, L_APP, L_APP },
    spell_colour_spray, TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                       15,     12,
    "&rc&go&cl&Yo&Gu&Rr &rsp&gr&Ba&Gy&X","!Colour Spray!", "",
    FALSE, SLOT(0)
  },

  {
    "song of hidden sight",
    { L_APP, L_APP, L_APP },
    spell_detect_hidden,        TAR_CHAR_SELF,          POS_STANDING,
    NULL,                       5,      12,
    "",                 "You feel less aware of your suroundings.", 
    "$n's vision returns to normal.", TRUE, SLOT(100)
  },

  {
    "song of cursing",
    { L_APP, L_APP, L_APP },
    spell_curse,                TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    &gsn_curse,                 20,     12,
    "curse",            "The curse wears off.", 
    "The curse within $n vanishes.", TRUE, SLOT(101)
  },

  {
    "song of earthquake",
    { L_APP, L_APP, L_APP },
    spell_earthquake,           TAR_IGNORE,             POS_FIGHTING,
    NULL,                       15,     12,
    "&Oearthquake&X",   "!Earthquake!", "", FALSE, SLOT(0)
  },

  {
    "song of faerie fire",
    { L_APP, L_APP, L_APP },
    spell_faerie_fire,          TAR_CHAR_OFFENSIVE,    POS_FIGHTING,
    NULL,                       5,      12,
    "faerie fire",       "The pink aura around you fades away.", 
    "The pink aura enveloping $n slowly fades away.", TRUE, SLOT(102)
  },

  {
    "song of astral travel",
    { L_APP, L_APP, L_APP },
    spell_astral,               TAR_IGNORE,            POS_STANDING,
    NULL,                       50,     12,
    "",                  "!Astral Walk!", "", FALSE, SLOT(0)
  },

  {
    "song of the ghosts",
    { L_APP, L_APP, L_APP },
    spell_pass_door,            TAR_CHAR_SELF,         POS_STANDING,
    NULL,                       20,     12,
    "",                  "You feel solid again.", 
    "$n's body appears more solid.", TRUE, SLOT(103)
  },

  {
    "song of revealing",
    { L_APP, L_APP, L_APP },
    spell_faerie_fog,            TAR_IGNORE,           POS_STANDING,
    NULL,                        12,     12,
    "faerie fog",         "!Faerie Fog!", "", FALSE, SLOT(0)
  },

  {
    "song of stone",
    { L_APP, L_APP, L_APP },
    spell_stone_skin,            TAR_CHAR_SELF,        POS_STANDING,
    NULL,                        12,     18,
    "",                   "Your skin feels soft again.", 
    "$n's skin takes on a softer texture.", TRUE,  SLOT(104)
  },

  {
    "lullaby",
    { L_APP, L_APP, L_APP },
    spell_sleep,                 TAR_CHAR_OFFENSIVE,   POS_STANDING,
    &gsn_sleep,                  15,     12,
    "",                   "You feel less tired.",        
    "$n looks more energetic.", TRUE,  SLOT(105)
  },

  {
    "song of fiery death",
    { L_APP, L_APP, L_APP },
    spell_fireball,             TAR_CHAR_OFFENSIVE,   POS_FIGHTING,
    NULL,                       15,     12,
    "&Rfireball&X",   "!Fireball!", "", FALSE, SLOT(0)
  },

  {
    "song of cancellation",
    { L_APP, L_APP, L_APP },
    spell_cancellation,          TAR_CHAR_DEFENSIVE,   POS_FIGHTING,
    NULL,                        15,     16,
    "",                   "!Cancellation!",              "", FALSE, SLOT(0)
  },

  {
    "song of sight",
    { L_APP, L_APP, L_APP },
    spell_truesight,             TAR_CHAR_SELF,        POS_STANDING,
    NULL,                        59,     12,  
    "",                  "Your vision is no longer so true.", 
    "$n's vision returns to normal.", TRUE, SLOT(106)
  },

  {
    "song of fireshield",
    { L_APP, L_APP, L_APP },
    spell_fireshield,            TAR_CHAR_DEFENSIVE,   POS_STANDING,
    NULL,                        75,     12,
    "",               "The flames engulfing you body burn out.", 
    "The flames surrounding $n suddenly burn out.", TRUE, SLOT(107)
  },

  {
    "song of portal",
    { L_APP, L_APP, L_APP },
    spell_portal,                TAR_IGNORE,           POS_STANDING,
    NULL,                        100,    12,
    "",               "",                            "", FALSE, SLOT(0)
  },

  {
    "song of healing",
    { L_APP, L_APP, L_APP },
    spell_heal,                  TAR_CHAR_DEFENSIVE,   POS_FIGHTING,
    NULL,                        50,     12,
    "",              "!Heal!",                       "", FALSE, SLOT(0)
  },
  {
    "track",
    { L_APP, L_APP, L_APP },
    spell_null,         TAR_IGNORE,             POS_STANDING,
    &gsn_track,         0,       0,
    "",                 "!Track!",              "", FALSE, SLOT(0)
  }, 
  {
    "headbutt",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_headbutt,  0,  0,
    "headbutt",     "!Headbutt!",   "", FALSE, SLOT(0)
  },
  {
    "pass plant",
    { L_APP, L_APP, L_APP },
    spell_pass_plant,               TAR_IGNORE,             POS_STANDING,
    NULL,                       50,     12,
    "",                 "!PASS PLANT!", "", FALSE, SLOT(0)
  },

  {
    "flamehand",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_SELF,    POS_STANDING,
    &gsn_flamehand, 0,  0,
    "", "The &rflames &won your hands flicker and vanish.",
    "The &rflames &won $n's hands flicker and vanish.", FALSE, SLOT(108)
  },
  {
    "frosthand",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_SELF,    POS_STANDING,
    &gsn_frosthand, 0,  0,
    "", "The &Cfrost&w and &Cice&w melt from your hands.",
    "The &Cfrost&w and &Cice&w melt from $n's hands.", FALSE, SLOT(109)
  },
  {
    "chaoshand",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_SELF,    POS_STANDING,
    &gsn_chaoshand, 0,  0,
    "", "Your hands stop flickering, and order overtakes the &Ychaos&w.",
    "$n's hands stop flickering, and order overtakes the &Ychaos&w.",
    FALSE, SLOT(110)
  },
  {
    "bladepalm",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_bladepalm, 0,  18,
    "blade palm", "!Blade Palm!",   "", FALSE, SLOT(0)
  },
  {
    "flying kick",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_flykick, 0,  12,
    "flying kick",  "!Flying Kick!", "", FALSE, SLOT(0)
  },
  {
    "anatomy knowledge",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_STANDING,
    &gsn_anatomyknow, 0,  0,
    "",     "!Anatomy Knowledge!",    "", FALSE, SLOT(0)
  },
  {
    "blackbelt",
    { L_APP, L_APP, L_APP },
    spell_null,         TAR_IGNORE,             POS_FIGHTING,
    &gsn_blackbelt,     0,      0,
    "",        "!Blackbelt!", "", FALSE, SLOT(0)
  },
  {
    "nerve",
    { L_APP, L_APP, L_APP },
    spell_null,         TAR_CHAR_OFFENSIVE,             POS_FIGHTING,
    &gsn_nerve,     0,      24,
    "",        "You can feel your arms again.", 
    "Feeling returns to $n's arms.", FALSE, SLOT(111)
  },
  {
    "soulbind",
    { L_APP, L_APP, L_APP },
    spell_soul_bind,         TAR_CHAR_OFFENSIVE,             POS_FIGHTING,
    NULL,     200,      18,
    "",        "!SOULBIND!", "", FALSE, SLOT(0)
  },
  {
    "iron skin",
    { L_APP, L_APP, L_APP },
    spell_iron_skin,      TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   50,     12,
    "",
    "Your skin returns to normal.",
    "$n's skin returns to its normal texture.", TRUE, SLOT(112)
  },
  {
    "chi shield",
    { L_APP, L_APP, L_APP },
    spell_chi_shield,      TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   75,     12,
    "", "Your chi shield dissipates.", 
    "The chi shield surrounding $n dissipates.", FALSE, SLOT(113)
  },
  {
    "ironfist",
    { L_APP, L_APP, L_APP },
    spell_null,      TAR_CHAR_SELF,          POS_STANDING,
    &gsn_ironfist,     0,     12,
    "", "Your fists are flesh once again.", 
    "$n's fists return to a normal structure.", FALSE, SLOT(114)
  },
  {
    "Globe of Darkness",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_globedark,   50, 0,
    "&zYou raise your hand and the globe dissipates.",  
    "You are no longer able to sustain your &zglobe &cin &w:&W",
    "The globe of darkness about $n dissipates.", TRUE, SLOT(0)
  },
  {
    "drow fire",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_drowfire,  0,  12,
    "drow fire",
    "The purple aura around you fades away.",
    "$n's outline fades away.",
    TRUE, SLOT(0)
  },

  {
    "snatch",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_snatch,    0,  12,
    "",     "!Steal!", "", FALSE, SLOT(0)
  },

  {
    "enhanced damage three",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_enhanced_three,  0,   0,
    "",     "!Enhanced Damage Two!", "", FALSE, SLOT(0)
  },

  {
    "retreat",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_retreat, 0,   0,
    "",     "!Run Away:P!", "", FALSE, SLOT(0)
  },


  {
    "antidote",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_antidote,  0,   0,
    "",     "", "", FALSE, SLOT(0)
  },
  {
    "haggle",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_haggle,  0,   0,
    "",     "", "", FALSE, SLOT(0)
  },
  {
    "blind fighting",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_blindfight,  0,   0,
    "",     "", "", FALSE, SLOT(0)
  },
  {
    "enhanced dodge",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_dodge_two, 0,   0,
    "",     "", "", FALSE, SLOT(0)
  },

  {
    "ethereal mist", 
    { L_APP, L_APP, L_APP },
    spell_pass_door,    TAR_CHAR_SELF,          POS_STANDING,
    NULL,                       20,     12,
    "",                 
    "You feel solid again.",
    "$n's body becomes more solid.", TRUE, SLOT(115)
  },
  {
    "mist form",
    { L_APP, L_APP, L_APP },
    spell_mist_form,  TAR_CHAR_SELF,          POS_STANDING,
    NULL,                       100,    12,
    "",                 "A strange feeling comes over you as you solidify.",
    "$n's body returns to it's normal structure.",
    TRUE, SLOT(116)
  },
  {
    "golden aura",
    { L_APP, L_APP, L_APP },
    spell_golden_aura,  TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,     75, 12,
    "",
    "The golden aura around your body vanishes.",
    "The golden aura around $n's body vanishes.", TRUE, SLOT(117)
  },
  {
    "shriek",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_shriek,  0,   16,
    "shriek",   "", "", FALSE, SLOT(0)
  },
  {
    "acidic spit",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_IGNORE,   POS_FIGHTING,
    &gsn_spit,  0,  0,
    "&gacidic &Gspit",  "The corosive spit finally evaporates.",
    "The acidic spit burning $n evaporates.", FALSE, SLOT(118)
  },
  {
    "ward of safety", 
    { L_APP, L_APP, L_APP },
    spell_null,       TAR_IGNORE,        POS_STANDING,
    &gsn_ward_safe,     100,  12,
    "You remove the ward of safety from the room.",
    "You are no longer able to sustain your &zward of safety &cin &w:&W",
    "The wards of safety crumble.", FALSE, SLOT(0)
  },
  {
    "ward of healing", 
    { L_APP, L_APP, L_APP },
    spell_null,       TAR_IGNORE,        POS_STANDING,
    &gsn_ward_heal,     200,  12,
    "You remove the ward of healing from the room.",
    "You are no longer able to sustain your &Wward of healing &cin &w:&W",
    "The wards of healing crumble.", FALSE, SLOT(0)
  },

  {
    "thunder strike",
    { L_APP, L_APP, L_APP },
    spell_thunder_strike,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     15, 12,
    "thunder strike",   "!thunder strike!", "", SLOT(0)
  },

  {
    "inspiration",
    { L_APP, L_APP, L_APP },
    spell_inspiration,    TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,     15, 12,
    "",   "The inspiration you once felt vanishes.", 
    "The inspiration within $n disappears.", FALSE, SLOT(0)
  },

  {
    "warcry",
    { L_APP, L_APP, L_APP },
    spell_war_cry,    TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     15, 12,
    "war cry",      "!War cry!", "", SLOT(0)
  },  

  {
    "group healing",
    { L_APP, L_APP, L_APP },
    spell_group_healing,       TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
    NULL,                       50,     12,
    "",                 "!Group Heal!", "", FALSE, SLOT(0)  
  },

  {
    "chant",
    { L_APP, L_APP, L_APP },
    spell_chant,    TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,     50, 12,
    "",   "!Chant!", "", FALSE, SLOT(0)
  },

  {
    "aid",
    { L_APP, L_APP, L_APP },
    spell_aid,    TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,     30, 12,
    "",
    "You feel divine aid disipate.",
    "$n looks less righteous.",
    FALSE, SLOT(199)
  },

  {
    "bark skin",
    { L_APP, L_APP, L_APP },
    spell_bark_skin,  TAR_CHAR_SELF,    POS_STANDING,
    NULL,     8,  18,
    "",
    "Your skin returns to normal.",
    "$n's skin returns to normal.",
    TRUE, SLOT(200)
  },

  {
    "blur",
    { L_APP, L_APP, L_APP },
    spell_blur,     TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   15,     12,
    "",
    "You are no longer blured.",
    "$n's blured form focused suddenly.", FALSE, SLOT(201)
  },

  {
    "firewall",
    { L_APP, L_APP, L_APP },
    spell_firewall, TAR_IGNORE,   POS_FIGHTING,
    NULL,     55, 22,
    "&Rroaring flames&X",     "!FIRWALL!", "", FALSE, SLOT(0)
  },

  {
    "draw strength", 
    { L_APP, L_APP, L_APP },
    spell_draw_strength,  TAR_CHAR_SELF,        POS_STANDING,
    NULL,     80, 40,
    "",   "Strength returns to your body.",     
    "$n's doesn't look so weak anymore.", FALSE, SLOT(202)
  },

  {
    "scrolls",
    { L_APP, L_APP, L_APP },
    spell_null,           TAR_IGNORE,   POS_STANDING,
    &gsn_scrolls,    0, 0,
    "&Wfailure",    "!Scrolls!", "", FALSE, SLOT(0)
  },

  {
    "wands",
    { L_APP, L_APP, L_APP },
    spell_null,           TAR_IGNORE,   POS_STANDING,
    &gsn_wands,    0, 0,
    "&Wfailure",    "!Wands!", "", FALSE, SLOT(0)
  },

  {
    "staves",
    { L_APP, L_APP, L_APP },
    spell_null,           TAR_IGNORE,   POS_STANDING,
    &gsn_staves,    0, 0,
    "&Wfailure",    "!Staves!", "", FALSE, SLOT(0)
  },

  {
    "purify",
    { L_APP, L_APP, L_APP },
    spell_purify, TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL, 70, 12, "", "!purify!", FALSE,  SLOT(0)
  },
  {
    "lure",
    { L_APP, L_APP, L_APP },
    spell_null, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_lure, 0,   32, "", "!lure!", FALSE, SLOT(0)
  },
  {
    "image",
    { L_APP, L_APP, L_APP },
    spell_image, TAR_CHAR_DEFENSIVE, POS_STANDING,
    &gsn_image, 50, 12, "",
    "Your images fade.",
    "$n's multiple images fade.", TRUE, SLOT(203)
  },
  {
    "silence",
    { L_APP, L_APP, L_APP },
    spell_silence,  TAR_CHAR_OFFENSIVE, POS_STANDING,
    NULL, 60, 12, "", "You may cast spells again.", 
    "$n is somatic once again.", TRUE, SLOT(24)
  },

  {
    "flip",
    { L_APP, L_APP, L_APP },
    spell_null, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_flip, 0,  32, "flip", "!flip!", FALSE, SLOT(0)
  },
  {
    "hallucinate",
    { L_APP, L_APP, L_APP },
    spell_hallucinate,  TAR_CHAR_OFFENSIVE, POS_STANDING,
    NULL, 160,  12, "", "You regain your sanity.", 
    "$n looks better.", TRUE, SLOT(205)
  },
  {
    "unholystrength",
    { L_APP, L_APP, L_APP },
    spell_unholystrength, TAR_CHAR_DEFENSIVE, POS_STANDING,
    &gsn_unholystrength, 50, 12, "",
    "The unholy strength of the plague leaves you.",
    "$n's aura of disease evaporates.", TRUE, SLOT(206)
  },
  {
    "plague",
    { L_APP, L_APP, L_APP },
    spell_null,   TAR_CHAR_OFFENSIVE, POS_STANDING,
    &gsn_plague,    10, 12,
    "&zp&wl&za&wg&zu&we",
    "You feel less sick.",
    "$n looks better.", TRUE, SLOT(207)
  },
  {
    "manabomb",
    { L_APP, L_APP, L_APP },
    spell_manabomb,   TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,     500,  12,
    "&RManaBomb&X",   "!ManaBomb!", "", FALSE, SLOT(0)
  },

  {
    "vaccinate",
    { L_APP, L_APP, L_APP },
    spell_vaccinate,  TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,     5,  12,
    "",     "You are no longer vaccinated.",
    "$n is no longer vaccinated.", FALSE, SLOT(208)
  },

    /*
     * Place all new spells/skills BEFORE this one.  It is used as an index marker
     * in the same way that theres a blank entry at the end of the command table.
     * (in interp.c)
     * -- Altrag
     */
  {
    "",
    {999,999,999},
    spell_null,     0, 0,
    NULL, 0, 0,
    "", "", "", SLOT(0)
  }
};

const struct gskill_type gskill_table [MAX_GSPELL] =
{
  /*
   * The globals for group spells..
   * -- Altrag
   */
  /*{wait,SLOT(slot),{CAS,ROG,FIG},*/
  { 3, SLOT(221), {0,0,0} },
  { 2, SLOT(227), {2,0,0} },
};
