/***********************************
 * Do NOT delete this file!        *
 * -- Hannibal			   *
 ***********************************/

bool can_use_skpell( CHAR_DATA *ch, int sn )
{
	if ( IS_NPC( ch ) ) {
		return TRUE;
	}

	if ( ch->pcdata->learned[sn] > 0 ) {
		return TRUE;
	}

	return FALSE;
}

bool can_practice_skpell( CHAR_DATA *ch, int sn )
{
	int iClass = 0;
	if ( IS_NPC( ch ) ) {
		return TRUE;
	}

	if ( ch->pcdata->learned[sn] > 0 ) {
		return TRUE;
	}

	for ( iClass = 0; ch->class[iClass] != -1; iClass++ ) {
		if ( ch->level >= skill_table[sn].skill_level[ch->class[iClass]] ) {
			return TRUE;
		}
	}

	return FALSE;
}

bool has_spells( CHAR_DATA *ch )
{
	int iClass;

	if ( IS_NPC( ch ) ) {
		return FALSE;
	}

	for ( iClass = 0; ch->class[iClass] != -1; iClass++ ) {
		if ( class_table[iClass].spellcaster ) {
			return TRUE;
		}
	}
	
	return FALSE;
}

bool is_class( CHAR_DATA *ch, int class )
{
	int iClass; 

	if ( IS_NPC( ch ) ) {
		return FALSE;
	}

	for ( iClass = 0; ch->class[iClass] != -1; iClass++ )
	{
		if ( ch->class[iClass] == class ) {
			return TRUE;
		}
	}

	return FALSE;
}

int prime_class( CHAR_DATA *ch )
{
	return ch->class[0];
}

int number_classes( CHAR_DATA *ch )
{
	int iClass;
	if ( IS_NPC( ch ) )
		return 0;
	for ( iClass = 0; ch->class[iClass] != -1; iClass++ )
		; 
	return iClass;
}

char *class_long( CHAR_DATA *ch )
{
	static char buf [ 512 ];
	int iClass;
	buf[0] = '\0';
	if ( IS_NPC( ch ) )
		return "Mobile";
	for ( iClass = 0; ch->class[iClass] != -1 ; iClass++ )
	{
		strcat( buf, "/" );
		strcat( buf, class_table[ch->class[iClass]].who_long );
	}
	return buf+1;
}

char *class_short( CHAR_DATA *ch )
{
	static char buf [ 512 ];
	int iClass;
	buf[0] = '\0';
	if ( IS_NPC( ch ) )
		return "Mob";
	for ( iClass = 0; ch->class[iClass] != -1 ; iClass++ )
	{
		strcat( buf, "/" );
		strcat( buf, class_table[ch->class[iClass]].who_name );
	}
	return buf+1;
}

char *class_numbers( CHAR_DATA *ch, bool pSave )
{
	static char buf[ 512 ];
	char buf2[ 10 ];
	int iClass;
	buf[0] = '\0';
	if ( IS_NPC( ch ) )
		return "0";
	for ( iClass = 0; ch->class[ iClass ] != -1; iClass++ )
	{
		strcat( buf, " " );
		sprintf( buf2, "%d", ch->class[iClass] );
		strcat( buf, buf2 );
	}
	if ( pSave )
		strcat( buf, " -1" );
	return buf+1;
}
bool gets_zapped( CHAR_DATA *ch, OBJ_DATA *obj )
{
	if ( !IS_NPC( ch ) )
		if ( ( IS_OBJ_STAT( obj, ITEM_ANTI_EVIL   ) && IS_EVIL   ( ch ) )
				|| ( IS_OBJ_STAT( obj, ITEM_ANTI_GOOD   ) && IS_GOOD   ( ch ) )
				|| ( IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL( ch ) )
				|| ( IS_ANTI_CLASS( obj, ITEM_ANTI_CASTER   ) && is_class( ch, CLASS_CASTER ) )
				|| ( IS_ANTI_CLASS( obj, ITEM_ANTI_ROGUE  ) && is_class( ch, CLASS_ROGUE ) )
				|| ( IS_ANTI_CLASS( obj, ITEM_ANTI_FIGHTER) && is_class( ch, CLASS_FIGHTER ) )
				|| ( IS_ANTI_RACE( obj, ITEM_ANTI_HUMAN     ) && ch->race == RACE_HUMAN )
				|| ( IS_ANTI_RACE( obj, ITEM_ANTI_ELF       ) && ch->race == RACE_ELF )
				|| ( IS_ANTI_RACE( obj, ITEM_ANTI_DWARF     ) && ch->race == RACE_DWARF )
				|| ( IS_ANTI_RACE( obj, ITEM_ANTI_GNOME     ) && ch->race == RACE_GNOME )
				|| ( IS_ANTI_RACE( obj, ITEM_ANTI_HALFLING  ) && ch->race == RACE_HALFLING ) )
				return TRUE;
	return FALSE;
}
