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
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/*
 * This file contains all of the OS-dependent stuff:
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.
 *
 * The data flow for input is:
 *    Game_loop ---> Read_from_descriptor ---> Read
 *    Game_loop ---> Read_from_buffer
 *
 * The data flow for output is:
 *    Game_loop ---> Process_Output ---> Write_to_descriptor -> Write
 *
 * The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
 * -- Furey  26 Jan 1993
 */

#define unix 1

#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/telnet.h>
#include <unistd.h>
#include "merc.h"

/*
 * Malloc debugging stuff.
 */
#if defined( MALLOC_DEBUG )
#include <malloc.h>
extern	int	malloc_debug	args( ( int  ) );
extern	int	malloc_verify	args( ( void ) );
#endif

/*
 * Socket and TCP/IP stuff.
 */
const	char	echo_off_str	[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const	char	echo_on_str		[] = { IAC, WONT, TELOPT_ECHO, '\0' };
const	char 	go_ahead_str	[] = { IAC, GA, '\0' };

int	close									args( ( int fd ) );
int	gettimeofday					args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen								args( ( int s, int backlog ) );
int	select								args( ( int width, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout ) );
int	socket								args( ( int domain, int type, int protocol ) );
int	init_socket						args( ( int port ) );

bool check_ban						args( ( struct descriptor_data *dnew, bool loggedin ) );
bool check_newban					args( ( struct descriptor_data *dnew, bool loggedin ) );
bool start_auth						args( ( DESCRIPTOR_DATA *d ) );
bool read_from_descriptor	args( ( DESCRIPTOR_DATA *d ) );
bool write_to_descriptor	args( ( char *txt, int length, DESCRIPTOR_DATA *d ) );

void nanny								args( ( DESCRIPTOR_DATA *d, char *argument ) );
void read_from_buffer			args( ( DESCRIPTOR_DATA *d ) );
void stop_idling					args( ( CHAR_DATA *ch ) );
void bust_a_prompt				args( ( DESCRIPTOR_DATA *d ) );
void auth_maxdesc					args( ( int *maxdesc, fd_set *in_set, fd_set *out_set, fd_set *exc_set ) );
void auth_update					args( ( fd_set *in_set, fd_set *out_set, fd_set *exc_set ) );
void auth_check						args( ( DESCRIPTOR_DATA *d ) );
void game_loop_unix				args( ( int control ) );
void new_descriptor				args( ( int control ) );
void note_cleanup					args( ( void ) );

bool check_parse_name			args( ( char *name ) );
bool check_reconnect			args( ( DESCRIPTOR_DATA *d, char *name, bool fConn ) );
bool check_playing				args( ( DESCRIPTOR_DATA *d, char *name ) );
bool process_output				args( ( DESCRIPTOR_DATA *d, bool fPrompt ) );

char *set_str_color				args( ( int AType, const char *txt) );

/* void    bust_a_color_prompt     args( ( DESCRIPTOR_DATA *d ) ); */

void reset_builder_levels ( );
void hotreboot_recover ( );

/*
 * Global variables.
 */
DESCRIPTOR_DATA *   descriptor_free;											/* Free list for descriptors	*/
DESCRIPTOR_DATA *   descriptor_list;											/* All open descriptors		*/
DESCRIPTOR_DATA *   d_next;																/* Next descriptor in loop	*/
FILE *		    			fpReserve;														/* Reserved file handle		*/
bool		    				merc_down;														/* Shutdown                     */
bool		    				wizlock;															/* Game is wizlocked		*/
bool		    				auth_off;															/* Authorization turned off	*/
time_t		    			current_time;													/* Time of this pulse		*/
time_t		    			exe_comp_time;												/* Time the executable was compiled */
char *		    			exe_file;															/* Name of the executable	*/
char		    				str_boot_time [ MAX_INPUT_LENGTH ];
int                 numlock = 0;      										/* Game is numlocked at <level> */
int 								port;
int 								control;
int 								maxdesc;

/* IMC functions */
#include <arpa/inet.h>

#define IMC_PORT	1046
#define IMC_HOST	(205 << 24)+(134 << 16)+(192 << 8)+31
#define TIMEOFS		(5739 * 86400) + (7 * 3600) + (24 * 60)

void imc_setup		args( ( void ) );
void imc_accept		args( ( void ) );
void imc_update		args( ( fd_set *ins, fd_set *outs, fd_set *excs ) );
bool imc_read			args( ( void ) );
bool imc_write		args( ( const char *txt ) );
void imc_channel	args( ( const char *buf, int len ) );

int imcport = -1;
int imcdesc = -1;

int game_main( int argc, char **argv )
{
	struct timeval now_time;
	bool fHotReboot = FALSE;

	/*
	 * Memory debugging if needed.
	 */
#if defined( MALLOC_DEBUG )
	malloc_debug( 2 );
#endif

	/*
	 * Init time.
	 */

	exe_file = str_dup( argv[0] );
	{
		struct stat statis;
		if ( !stat( exe_file, &statis ) )
			exe_comp_time = statis.st_mtime;
	}

	gettimeofday( &now_time, NULL );
	current_time = (time_t) now_time.tv_sec;

	if ( current_time < TIMEOFS ) {
		current_time += TIMEOFS;
	}

	strcpy( str_boot_time, ctime( &current_time ) );

	/*
	 * Reserve one channel for our use.
	 */
	if ( !( fpReserve = fopen( NULL_FILE, "r" ) ) )
	{
		perror( NULL_FILE );
		exit( 0 );
	}

	/*
	 * Get the port number.
	 */
	port = 1234;
	if ( argc > 1 )
	{
		if ( !is_number( argv[1] ) )
		{
			fprintf( stderr, "Usage: %s [port #]\n", argv[0] );
			exit( 0 );
		}
		else if ( ( port = atoi( argv[1] ) ) <= 1024 )
		{
			fprintf( stderr, "Port number must be above 1024.\n" );
			exit( 0 );
		}
	}

	/* Checks what port -- note Angi */
	if ( port == 2222 ) reset_builder_levels();

	if (argv[2] && argv[2][0]) {
		fHotReboot = TRUE;
		control = atoi(argv[3]);
	} else {
		fHotReboot = FALSE;
	}

	/*
	 * Run the game.
	 */

	/*    control = init_socket( port ); */
	boot_db( );
	note_cleanup( );

	/* We already have the port if hot rebooting */
	if ( !fHotReboot ) {
		if ( (control = init_socket( port )) < 0 ) {
			exit(1);
		}
	}

	sprintf( log_buf, "EnvyMud is ready to rock on port %d.", port );
	log_string( log_buf, CHANNEL_NONE, -1 );
	{
		FILE *fp;
		char new_comlog[MAX_INPUT_LENGTH];
		char old_comlog[MAX_INPUT_LENGTH];

		sprintf(new_comlog, "comlog%d.txt", port );
		sprintf(old_comlog, "comlog%d.old", port );

		unlink(old_comlog);
		rename(new_comlog, old_comlog);

		fclose(fpReserve);

		if ( !(fp = fopen( new_comlog, "w" ) ) ) {
			perror( new_comlog );
		} else {
			fprintf( fp, "%s: %s\n", ctime( &current_time ), log_buf );
			fclose(fp);
		}

		fpReserve = fopen(NULL_FILE, "r" );
	}

	/* Recover the players after a hot reboot */
	if (fHotReboot) {
		hotreboot_recover( );
	}

	game_loop_unix( control );
	close( control );

	/*
	 * That's all, folks.
	 */
	log_string( "Normal termination of game.", CHANNEL_NONE, -1 );
	exit( 0 );
	return 0;
}



int init_socket( int port )
{
	static struct sockaddr_in sa_zero;
	struct sockaddr_in sa;
	int         x        = 1;
	int         fd;

	if ( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
	{
		perror( "Init_socket: socket" );
		return -1;
	}

	if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,
				(char *) &x, sizeof( x ) ) < 0 )
	{
		perror( "Init_socket: SO_REUSEADDR" );
		close( fd );
		return -1;
	}

	/*
#if defined( SO_DONTLINGER ) && !defined( SYSV )
	{
	struct	linger	ld;

	ld.l_onoff  = 1;
	ld.l_linger = 1000;

	if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER,
	(char *) &ld, sizeof( ld ) ) < 0 )
	{
	perror( "Init_socket: SO_DONTLINGER" );
	close( fd );
	return -1;
	}
	}
#endif
	*/

	sa		    = sa_zero;
	sa.sin_family   = AF_INET;
	sa.sin_port	    = htons( port );

	if ( bind( fd, (struct sockaddr *) &sa, sizeof( sa ) ) < 0 )
	{
		if ( port != IMC_PORT || errno != EADDRINUSE )
			perror( "Init_socket: bind" );
		close( fd );
		return (errno == EADDRINUSE ? -2 : -1);
	}

	if ( listen( fd, 3 ) < 0 )
	{
		perror( "Init_socket: listen" );
		close( fd );
		return -1;
	}

	return fd;
}

void game_loop_unix( int control )
{
	static struct timeval null_time;
	struct timeval last_time;

	signal( SIGPIPE, SIG_IGN );
	signal( SIGCHLD, SIG_IGN );

	gettimeofday( &last_time, NULL );
	current_time = (time_t) last_time.tv_sec;
	if ( current_time < TIMEOFS ) current_time += TIMEOFS;

	/* Main loop */
	while ( !merc_down )
	{
		DESCRIPTOR_DATA *d;
		fd_set           in_set;
		fd_set           out_set;
		fd_set           exc_set;
		/*	int              maxdesc;*/

#if defined( MALLOC_DEBUG )
		if ( malloc_verify( ) != 1 )
			abort( );
#endif

		/*
		 * Poll all active descriptors.
		 */
		FD_ZERO( &in_set  );
		FD_ZERO( &out_set );
		FD_ZERO( &exc_set );
		FD_SET( control, &in_set );
		if ( imcport > 0 )
			FD_SET(imcport, &in_set);
		maxdesc	= control;
		for ( d = descriptor_list; d; d = d->next )
		{
			maxdesc = UMAX( maxdesc, d->descriptor );
			FD_SET( d->descriptor, &in_set  );
			FD_SET( d->descriptor, &out_set );
			FD_SET( d->descriptor, &exc_set );
		}
		auth_maxdesc(&maxdesc, &in_set, &out_set, &exc_set);

		if ( imcdesc > 0 )
		{
			maxdesc = UMAX(maxdesc, imcdesc);
			FD_SET(imcdesc, &in_set );
			FD_SET(imcdesc, &out_set);
			FD_SET(imcdesc, &exc_set);
		}

		if ( select( maxdesc+1, &in_set, &out_set, &exc_set, &null_time ) < 0 )
		{
			perror( "Game_loop: select: poll" );
			exit( 0 );
		}

		/*
		 * New connection?
		 */
		if ( FD_ISSET( control, &in_set ) ) {
			new_descriptor( control );
		}

		/*	if ( port != 1045 )
			imc_update(&in_set, &out_set, &exc_set); */
		auth_update(&in_set, &out_set, &exc_set);

		/*
		 * Kick out the freaky folks.
		 */
		for ( d = descriptor_list; d; d = d_next )
		{
			d_next = d->next;
			if ( FD_ISSET( d->descriptor, &exc_set ) )
			{
				FD_CLR( d->descriptor, &in_set  );
				FD_CLR( d->descriptor, &out_set );

				if ( d->character ) {
					save_char_obj( d->character);
				}

				d->outtop	= 0;
				close_socket( d );
				continue;
			}
		}
		/*
		 * Process input.
		 */
		for ( d = descriptor_list; d; d = d_next )
		{
			d_next	= d->next;
			d->fcommand	= FALSE;

			if ( FD_ISSET( d->descriptor, &in_set ) )
			{
				if ( d->character )
					d->character->timer = 0;
				if ( !read_from_descriptor( d ) )
				{
					FD_CLR( d->descriptor, &out_set );
					if ( d->character )
						/*			save_char_obj( d->character ); */
						d->outtop	= 0;
					close_socket( d );
					continue;
				}
			}
			if ( d->character && d->character->race_wait > 0 )
				--d->character->race_wait;

			if ( d->character && d->character->wait > 0 )
			{
				--d->character->wait;
				continue;
			}

			read_from_buffer( d );
			if ( d->incomm[0] != '\0' )
			{
				d->fcommand	= TRUE;
				stop_idling( d->character );

				/* OLC */
				if ( d->showstr_point )
					show_string( d, d->incomm );
				else
					if ( d->pString )
						string_add( d->character, d->incomm );
					else
					{
						char buf[MAX_INPUT_LENGTH];
						char buf2[MAX_STRING_LENGTH];
						FILE *fp;

						sprintf( buf, "comlog%d.txt", port );
						sprintf( buf2, "%s: %s: %s\n", ctime( &current_time ),
								d->character ? d->character->name : "(Unknown)",
								d->incomm );
						fclose(fpReserve);
						if ( (fp = fopen( buf, "a" ) ) )
						{
							fprintf( fp, "%s: %s: %s\n", ctime( &current_time ),
									d->character ? d->character->name : "(Unknown)",
									d->incomm );
							fclose(fp);
						}
						fpReserve = fopen( NULL_FILE, "r" );

						if ( d->connected == CON_PLAYING )
						{
							if ( !run_olc_editor( d ) )
								interpret( d->character, d->incomm );
						}
						else if ( d->connected == CON_CHATTING )
						{
							chat_interp( d->character, d->incomm );
						}
						else
							nanny( d, d->incomm );
					}
				d->incomm[0]	= '\0';
			}
		}
		/*
		 * Autonomous game motion.
		 */
		update_handler( );
		/*
		 * Output.
		 */
		for ( d = descriptor_list; d; d = d_next )
		{
			d_next = d->next;

			if ( ( d->fcommand || d->outtop > 0 )
					&& FD_ISSET( d->descriptor, &out_set ) )
			{
				if ( !process_output( d, TRUE ) )
				{
					if ( d->character ) {
						save_char_obj( d->character );
					}

					d->outtop	= 0;
					close_socket( d );
				}
			}
		}

		/*
		 * Synchronize to a clock.
		 * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
		 * Careful here of signed versus unsigned arithmetic.
		 */
		{
			struct timeval now_time;
			long secDelta;
			long usecDelta;

			gettimeofday( &now_time, NULL );
			usecDelta	= ( (int) last_time.tv_usec )
				- ( (int)  now_time.tv_usec )
				+ 1000000 / PULSE_PER_SECOND;
			secDelta	= ( (int) last_time.tv_sec )
				- ( (int)  now_time.tv_sec );
			while ( usecDelta < 0 )
			{
				usecDelta += 1000000;
				secDelta  -= 1;
			}

			while ( usecDelta >= 1000000 )
			{
				usecDelta -= 1000000;
				secDelta  += 1;
			}

			if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
			{
				struct timeval stall_time;

				stall_time.tv_usec = usecDelta;
				stall_time.tv_sec  = secDelta;
				if ( select( 0, NULL, NULL, NULL, &stall_time ) < 0 )
				{
					perror( "Game_loop: select: stall" );
					exit( 0 );
				}
			}
		}
		gettimeofday( &last_time, NULL );
		current_time = (time_t) last_time.tv_sec;
		if ( current_time < TIMEOFS ) current_time += TIMEOFS;
	}

	return;
}

void init_descriptor (DESCRIPTOR_DATA *dnew, int desc)
{
	static DESCRIPTOR_DATA d_zero;

	*dnew	= d_zero;

	dnew->descriptor		= desc;
	dnew->character     = NULL;
	dnew->connected     = CON_GET_ANSI;
	dnew->showstr_head  = str_dup( "" );
	dnew->showstr_point = 0;
	dnew->showstr_point = NULL;
	dnew->pEdit					= NULL;			/* OLC */
	dnew->pString				= NULL;			/* OLC */
	dnew->editor				= 0;			/* OLC */
	dnew->outsize				= 1;
	dnew->outbuf				= alloc_mem( dnew->outsize );
	dnew->ansi          = FALSE;
	dnew->user          = str_dup( "(unknown)" );
}

void new_descriptor( int control )
{

	DESCRIPTOR_DATA *dnew;
	struct sockaddr_in      sock;
	struct hostent         *from;
	char                    buf [ MAX_STRING_LENGTH ];
	int                     desc;
	socklen_t               size;

	size = sizeof( sock );
	if ( ( desc = accept( control, (struct sockaddr *) &sock, &size) ) < 0 )
	{
		perror( "New_descriptor: accept" );
		return;
	}

#if !defined( FNDELAY )
#define FNDELAY O_NDELAY
#endif

	if ( fcntl( desc, F_SETFL, FNDELAY ) == -1 ) {
		perror( "New_descriptor: fcntl: FNDELAY" );
		close(desc);
		return;
	}

	/*
	 * Cons a new descriptor.
	 */
	if ( !descriptor_free ) {
		dnew		= alloc_perm( sizeof( *dnew ) );
	} else {
		dnew		= descriptor_free;
		descriptor_free	= descriptor_free->next;
	}

	init_descriptor (dnew, desc);

	size = sizeof(sock);

	if ( getpeername( desc, (struct sockaddr *) &sock, &size ) < 0 ) {
		perror( "New_descriptor: getpeername" );
		dnew->host = str_dup( "(unknown)" );
	} else {
		/*
		 * Would be nice to use inet_ntoa here but it takes a struct arg,
		 * which ain't very compatible between gcc and system libraries.
		 */
		int addr;

		addr = ntohl( sock.sin_addr.s_addr );

		sprintf( buf, "%d.%d.%d.%d", ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF, ( addr >>  8 ) & 0xFF, ( addr ) & 0xFF);
		sprintf( log_buf, "Sock.sinaddr:  %s", buf );

		log_string( log_buf, CHANNEL_GOD, -1 );

		from = gethostbyaddr( (char *) &sock.sin_addr, sizeof(sock.sin_addr), AF_INET );
		dnew->host = str_dup( from ? from->h_name : buf );
	}

	if ( check_ban( dnew, FALSE ) ) return;

	dnew->next			= descriptor_list;
	descriptor_list		= dnew;

	write_to_buffer( dnew, "Do you wish to use ANSI?  (Yes/No): ", 0 );
	start_auth(dnew);
	return;
}

void close_socket( DESCRIPTOR_DATA *dclose )
{
	CHAR_DATA *ch;

	if ( dclose->outtop > 0 )
		process_output( dclose, FALSE );

	if ( dclose->snoop_by )
	{
		write_to_buffer( dclose->snoop_by,
				"Your victim has left the game.\n\r", 0 );
	}

	{
		DESCRIPTOR_DATA *d;

		for ( d = descriptor_list; d; d = d->next )
		{
			if ( d->snoop_by == dclose )
				d->snoop_by = NULL;
		}
	}

	if ( ( ch = dclose->original ) )
	{
		if ( !dclose->character )
		{
			sprintf( log_buf, "Close_socket: original without character: %s",
					(dclose->original->name ? dclose->original->name
					 : "(unknown)" ) );
			bug( log_buf, 0);
			dclose->original->desc = NULL;
		}
		else
		{
			if ( IS_NPC(dclose->character) )
				do_return( dclose->character, "" );
			else
				dclose->original->desc = NULL;
		}
	}

	if ( ( ch = dclose->character ) )
	{
		sprintf( log_buf, "Closing link to %s.", (ch->name ? ch->name : "(unknown)") );
		log_string( log_buf, CHANNEL_LOG, -1 );
		if ( (dclose->connected == CON_PLAYING || dclose->connected == CON_CHATTING) && ch->in_room )
		{
			if ( ch->name )
				act(AT_GREEN, "$n has lost $s link.", ch, NULL, NULL, TO_ROOM );
			else
				act(AT_GREEN, "Someone has lost their link.", ch, NULL, NULL, TO_ROOM );
			wiznet( "Netdeath has claimed $N.", ch, NULL, WIZ_LINKS, 0, 0 );
			if ( ch->in_room->vnum == ROOM_VNUM_SMITHY )
			{
				char_from_room( ch );
				char_to_room( ch, get_room_index( 768 ) );
			}
			if (    !IS_SET( ch->act, PLR_WIZINVIS )
					&& !IS_SET( ch->act, PLR_CLOAKED ) )
				info( "The link between %s and the storm has been torn.",
						(int)(ch->name), 0 );
			/*	  {
				  CHAR_DATA *PeT;
				  for ( PeT = ch->in_room->people; PeT; PeT = PeT->next_in_room )
				  {
				  if ( IS_NPC( PeT ) )
				  if ( IS_SET( PeT->act, ACT_PET ) && PeT->master == ch )
				  {

				  REMOVE_BIT( PeT->act, ACT_PET );
				  break;

				  }
				  }
				  }*/
			ch->desc = NULL;
		}
		else
		{
			free_char( dclose->character );
		}
	}

	if ( d_next == dclose )
		d_next = d_next->next;

	auth_check(dclose);

	if ( dclose == descriptor_list )
	{
		descriptor_list = descriptor_list->next;
	}
	else
	{
		DESCRIPTOR_DATA *d;

		for ( d = descriptor_list; d && d->next != dclose; d = d->next )
			;
		if ( d )
			d->next = dclose->next;
		else
			bug( "Close_socket: dclose not found.", 0 );
	}

	close( dclose->descriptor );
	free_string( dclose->host );
	free_string( dclose->user );
	free_string( dclose->showstr_head );
	/* RT socket leak fix */
	free_mem( dclose->outbuf, dclose->outsize );

	/*    dclose->next	= descriptor_free;
		  descriptor_free	= dclose;*/
	free_mem( dclose, sizeof( *dclose ) );
	return;
}



bool read_from_descriptor( DESCRIPTOR_DATA *d )
{
	int iStart;

	/* Hold horses if pending command already. */
	if ( d->incomm[0] != '\0' )
		return TRUE;

	/* Check for overflow. */
	iStart = strlen( d->inbuf );
	if ( iStart >= sizeof( d->inbuf ) - 10 )
	{
		sprintf( log_buf, "%s input overflow!", d->host );
		log_string( log_buf, CHANNEL_GOD, -1 );
		wiznet("Spam spam spam $N spam spam spam spam spam!",
				d->character,NULL,WIZ_SPAM,0,get_trust(d->character));
		write_to_descriptor(
				"\n\r*** PUT A LID ON IT!!! ***\n\r", 0, d );
		return FALSE;
	}

	/* Snarf input. */

	for ( ; ; )
	{
		int nRead;

		nRead = read( d->descriptor, d->inbuf + iStart,
				sizeof( d->inbuf ) - 10 - iStart );
		if ( nRead > 0 )
		{
			iStart += nRead;
			if ( d->inbuf[iStart-1] == '\n' || d->inbuf[iStart-1] == '\r' )
				break;
		}
		else if ( nRead == 0 )
		{
			log_string( "EOF encountered on read.", CHANNEL_GOD, -1 );
			return FALSE;
		}
		else if ( errno == EWOULDBLOCK || errno == EAGAIN )
			break;
		else
		{
			perror( "Read_from_descriptor" );
			return FALSE;
		}
	}

	d->inbuf[iStart] = '\0';
	return TRUE;
}



/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer( DESCRIPTOR_DATA *d )
{
	int i;
	int j;
	int k;

	/*
	 * Hold horses if pending command already.
	 */
	if ( d->incomm[0] != '\0' ) {
		return;
	}

	/*
	 * Look for at least one new line.
	 */
	for ( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
	{
		if ( d->inbuf[i] == '\0' )
			return;
	}

	/*
	 * Canonical input processing.
	 */
	for ( i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
	{
		if ( k >= MAX_INPUT_LENGTH - 2 )
		{
			write_to_descriptor("Line too long.\n\r", 0, d );

			/* skip the rest of the line */
			for ( ; d->inbuf[i] != '\0'; i++ )
			{
				if ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
					break;
			}
			d->inbuf[i]   = '\n';
			d->inbuf[i+1] = '\0';
			break;
		}

		if ( d->inbuf[i] == '\b' && k > 0 )
			--k;
		else if ( isascii( d->inbuf[i] ) && isprint( d->inbuf[i] ) )
			d->incomm[k++] = d->inbuf[i];
	}

	/*
	 * Finish off the line.
	 */
	if ( k == 0 )
		d->incomm[k++] = ' ';
	d->incomm[k] = '\0';

	/*
	 * Deal with bozos with #repeat 1000 ...
	 */
	if ( k > 1 || d->incomm[0] == '!' )
	{
		if ( d->incomm[0] != '!' && strcmp( d->incomm, d->inlast ) )
		{
			d->repeat = 0;
		}
		else
		{
			if ( ++d->repeat >= 20 )
			{
				sprintf( log_buf, "%s input spamming!", d->host );
				log_string( log_buf, CHANNEL_GOD, -1 );
				wiznet("Spam spam spam $N spam spam spam spam spam!",
						d->character,NULL,WIZ_SPAM,0,get_trust(d->character));
				write_to_descriptor(
						"\n\r*** PUT A LID ON IT!!! ***\n\r", 0, d );
				/*                if ( d->descriptor ) */
				strcpy( d->incomm, "quit" );


			}
		}
	}

	/*
	 * Do '!' substitution.
	 */
	if ( d->incomm[0] == '!' ) {
		strcpy( d->incomm, d->inlast );
	} else {
		strcpy( d->inlast, d->incomm );
	}

	/*
	 * Shift the input buffer.
	 */
	while ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
		i++;
	for ( j = 0; ( d->inbuf[j] = d->inbuf[i+j] ) != '\0'; j++ )
		;
	return;
}



/*
 * Low level output function.
 */
bool process_output( DESCRIPTOR_DATA *d, bool fPrompt )
{
	extern bool merc_down;

	/*
	 * Bust a prompt.
	 */
	if ( fPrompt && !merc_down && d->connected == CON_PLAYING )	/* OLC */
		if ( d->showstr_point )
			write_to_buffer( d,
					"[Please type (c)ontinue, (r)efresh, (b)ack, (h)elp, (q)uit, or RETURN]:  ",
					0 );
		else
			if ( d->pString )
				write_to_buffer( d, "> ", 2 );
			else
			{
				CHAR_DATA *ch;
				CHAR_DATA *victim;

				/* XOR */
				/* battle prompt from Rom2 */
				ch = d->character;

				if ((victim = ch->fighting) != NULL)
				{
					int percent;
					/*	  int iHealth;
						  char buf2[MAX_STRING_LENGTH];
						  char health[20];*/
					char wound[100];
					char buf[MAX_STRING_LENGTH];

					if (MAX_HIT(victim) > 0)
						percent = victim->hit * 100 / MAX_HIT(victim);
					else
						percent = -1;
					if (percent >= 100)
						sprintf(wound,"is in excellent condition. &B[&G%d&B]",
								percent);
					else if (percent >= 90)
						sprintf(wound,"has a few scratches. &B[&G%d&B]", percent);
					else if (percent >= 75)
						sprintf(wound,"has some small wounds and bruises. &B[&Y%d&B]",
								percent);
					else if (percent >= 50)
						sprintf(wound,"has quite a few wounds. &B[&Y%d&B]", percent);
					else if (percent >= 30)
						sprintf(wound,"has some big nasty wounds and scratches. &B[&R%d&B]", percent);
					else if (percent >= 15)
						sprintf(wound,"looks pretty hurt. &B[&R%d&B]", percent);
					else if (percent >= 0)
						sprintf(wound,"is in awful condition. &B[&R%d&B]", percent);
					else
						sprintf(wound,"is bleeding to death. [%d]", percent);
					/*
					   strcpy( buf2, "&z[" );
					   if ( percent >= 66 )
					   strcat( buf2, "&G" );
					   else if( percent >= 33 )
					   strcat( buf2, "&Y" );
					   else
					   strcat( buf2, "&r" );
					   health[0] = '+';
					   for( iHealth = 1; iHealth < 20; iHealth++ )
					   if ( percent >= iHealth * 5 )
					   health[iHealth] = '+';
					   else
					   health[iHealth] = ' ';
					   health[iHealth] = '\0';
					   strcat( buf2, health );
					   strcat( buf2, "&z]" );
					   sprintf( wound, "%s", buf2 );
					   */
					sprintf(buf,"\n\r&R%s %s",
							IS_NPC(victim) ? victim->short_descr : victim->name,wound);
					buf[0] = UPPER(buf[0]);

					write_to_buffer( d, buf, 0);
				}
				else if ( (victim = ch->hunting) && IS_NPC(victim) )
				{
					hunt_victim( ch );
				}

				ch = d->original ? d->original : d->character;
				if ( IS_SET( ch->act, PLR_BLANK     ) )
					write_to_buffer( d, "\n\r", 2 );

				if ( IS_SET( ch->act, PLR_PROMPT    ) )
					/*	        if ( IS_SET( ch->act, PLR_ANSI ) )
								bust_a_color_prompt( d );
								else */
					bust_a_prompt( d );

				if ( IS_SET( ch->act, PLR_TELNET_GA ) )
					write_to_buffer( d, go_ahead_str, 0 );
			}
	else if ( d->connected == CON_CHATTING )
		write_to_buffer( d, "&w: ", 0);
	/*
	 * Short-circuit if nothing to write.
	 */
	if ( d->outtop == 0 )
		return TRUE;

	/*
	 * Snoop-o-rama.
	 */
	if ( d->snoop_by )
	{
		write_to_buffer( d->snoop_by, "% ", 2 );
		write_to_buffer( d->snoop_by, d->outbuf, d->outtop );
	}

	/*
	 * OS-dependent output.
	 */
	if ( !write_to_descriptor(d->outbuf, d->outtop, d ) )
	{
		d->outtop = 0;
		free_mem(d->outbuf, d->outsize);
		d->outsize = 1;
		d->outbuf = alloc_mem(d->outsize );
		return FALSE;
	}
	else
	{
		d->outtop = 0;
		free_mem(d->outbuf, d->outsize);
		d->outsize = 1;
		d->outbuf = alloc_mem(d->outsize);
		return TRUE;
	}
}

/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 */
void bust_a_prompt( DESCRIPTOR_DATA *d )
{
	CHAR_DATA *ch;
	const char      *str;
	const char      *i;
	char      *point;
	char       buf  [ MAX_STRING_LENGTH ];
	char       buf2 [ MAX_STRING_LENGTH ];

	ch = d->character;
	if( !ch->prompt || ch->prompt[0] == '\0' )
	{
		send_to_char(C_DEFAULT, "\n\r\n\r", ch );
		return;
	}
	i = 0;
	point = buf;
	str = d->original ? d->original->prompt : d->character->prompt;
	send_to_char(C_DEFAULT, "", ch);
	while( *str != '\0' )
	{
		if( *str != '%' )
		{
			*point++ = *str++;
			continue;
		}
		++str;
		switch( *str )
		{
			default :
				i = " "; break;
			case 'h' :
				sprintf( buf2, "%d", ch->hit                               );
				i = buf2; break;
			case 'H' :
				sprintf( buf2, "%d", MAX_HIT(ch) );
				i = buf2; break;
			case 'm' :
				sprintf( buf2, "%d", ch->mana);
				i = buf2; break;
			case 'M' :
				sprintf( buf2, "%d", MAX_MANA( ch ) );
				i = buf2; break;
			case 'v' :
				sprintf( buf2, "%d", ch->move                              );
				i = buf2; break;
			case 'V' :
				sprintf( buf2, "%d", MAX_MOVE(ch));
				i = buf2; break;
			case 'x' :
				sprintf( buf2, "%d", ch->exp                               );
				i = buf2; break;
			case 'X' :
				sprintf( buf2, "%d", xp_tolvl( ch ) - ch->exp             );
				i = buf2; break;
			case 'g' :
				sprintf( buf2, "%d", ch->money.gold			       );
				i = buf2; break;
			case 'a' :
				if( ch->level >= 5 )
					sprintf( buf2, "%d", ch->alignment                      );
				else
					sprintf( buf2, "%s", IS_GOOD( ch ) ? "good"
							: IS_EVIL( ch ) ? "evil" : "neutral" );
				i = buf2; break;
			case 'r' :
				if( ch->in_room )
					sprintf( buf2, "%s", ch->in_room->name                  );
				else
					sprintf( buf2, " "                                      );
				i = buf2; break;
			case 'R' :
				if( IS_IMMORTAL( ch ) && ch->in_room )
					sprintf( buf2, "%d", ch->in_room->vnum                  );
				else
					sprintf( buf2, " "                                      );
				i = buf2; break;
			case 'z' :
				if( IS_IMMORTAL( ch ) && ch->in_room )
					sprintf( buf2, "%s", ch->in_room->area->name            );
				else
					sprintf( buf2, " "                                      );
				i = buf2; break;
			case '%' :
				sprintf( buf2, "%%"                                        );
				i = buf2; break;
			case 'c' :		/* OLC */
				i = olc_ed_name( ch );
				break;
			case 'C' :		/* OLC */
				i = olc_ed_vnum( ch );
				break;
			case 'p' :
				{
					if ( IS_IMMORTAL( ch ) )
					{
						extern int sAllocPerm;

						sprintf( buf2, "%d", (sAllocPerm * 10) / (1024 * 1024) );
					}
					else
						sprintf( buf2, " " );
					i = buf2; break;
				}
			case 'l' :
				if ( IS_SET( ch->act, PLR_WIZINVIS ) )
					sprintf( buf2, "Invisible" );
				else
					sprintf( buf2, "Visible" );
				i = buf2;break;
			case 'd' :
				if ( IS_SET( ch->act, PLR_CLOAKED ) )
					sprintf( buf2, "Cloaked" );
				else
					sprintf( buf2, "Visible" );
				i = buf2;break;
		}
		++str;
		while( ( *point = *i ) != '\0' )
			++point, ++i;
	}
	/*log_string("first",0);
	  while( ( *point = *i ) != '\0' )
	  {
	  log_string("next",0);
	  ++point, ++i;
	  }
	 *point = '\0';
	 log_string("last",0);*/
	write_to_buffer( d, buf, point - buf );
	return;
}

/*
 * Append onto an output buffer.
 */
void write_to_buffer( DESCRIPTOR_DATA *d, const char *txt, int length )
{
	/*
	 * Find length in case caller didn't.
	 */
	if ( length <= 0 )
		length = strlen( txt );

	/*
	 * Initial \n\r if needed.
	 */
	if ( d->outtop == 0 && !d->fcommand )
	{
		d->outbuf[0]	= '\n';
		d->outbuf[1]	= '\r';
		d->outtop	= 2;
	}

	/*
	 * Expand the buffer as needed.
	 */
	while ( d->outtop + length >= d->outsize )
	{
		char *outbuf;

		outbuf      = alloc_mem( d->outtop + length + 1 );
		strncpy( outbuf, d->outbuf, d->outtop );
		free_mem( d->outbuf, d->outsize );
		d->outbuf  = outbuf;
		d->outsize = d->outtop + length + 1;
	}

	/*
	 * Copy.
	 */
	strncpy( d->outbuf + d->outtop, txt, length );
	d->outtop += length;
	/* Modded below... nothing critical, just a check */
	d->outbuf[d->outtop]='\0';
	return;
}



/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
const char colors[16] = ".rgObpcwzRGYBPCW";
bool write_to_descriptor( char *txt, int length, DESCRIPTOR_DATA *d )
{
	char *ntxt;
	char *str;
	int nl = 0;
	int cl;
	int cn = 0;

	if ( length <= 0 )
		length = strlen(txt);

	for ( str = txt; (str-txt) < length; str++ )
	{
		if ( *str != '&' )
		{
			nl++;
			continue;
		}
		str++;
		if ( *str == '\0' )
		{
			nl++;
			break;
		}
		if ( *str == '&' )
		{
			nl++;
			continue;
		}
		for ( cl = 0; cl < 16; cl++ )
			if ( *str == colors[cl] )
				break;
		if ( cl < 16 )
		{
			if ( d->ansi )
				nl += 9;
		}
		else
			nl += 2;
	}
	nl++; /* \0 */
	ntxt = alloc_mem(nl );
	for ( str = txt; (str-txt) < length; str++ )
	{
		if ( *str != '&' )
		{
			ntxt[cn++] = *str;
			continue;
		}
		str++;
		if ( *str == '\0' )
		{
			ntxt[cn++] = '&';
			break;
		}
		if ( *str == '&' )
		{
			ntxt[cn++] = '&';
			continue;
		}
		for ( cl = 0; cl < 16; cl++ )
			if ( *str == colors[cl] )
				break;
		if ( cl < 16 )
		{
			if ( d->ansi )
			{
				if ( cl == 0 )
					cl = number_range(1, 15);
				sprintf(&ntxt[cn], "\033[0;%d;%dm", (cl > 7), (cl & 7) + 30);
				cn += 9;
			}
			continue;
		}
		sprintf(&ntxt[cn], "&%c", *str);
		cn += 2;
	}

	ntxt[cn] = '\0';

	/* Hmm.. reuse all our int vars.. grin.. */
	for ( cl = 0; cl < nl; cl += cn )
	{
		length = UMIN( nl - cl, 4096 );
		if ( ( cn = write( d->descriptor, ntxt + cl, length ) ) < 0 )
		{
			perror( "Write_to_descriptor" );
			free_mem(ntxt, nl);
			return FALSE;
		}
	}

	free_mem(ntxt, nl);
	return TRUE;
}

/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny( DESCRIPTOR_DATA *d, char *argument )
{
	CHAR_DATA *ch;
	NOTE_DATA *pnote;
	char      *pwdnew;
	char      *p;
	char       buf [ MAX_STRING_LENGTH ];
	char       buf2[ MAX_STRING_LENGTH ];
	int        iClass;
	int        iRace;
	int	       iCount;
	int        notes;
	bool       fOld;
	MONEY_DATA amt;

	while ( isspace( *argument ) )
		argument++;

	/* This is here so we wont get warnings.  ch = NULL anyways - Kahn */
	ch          = d->character;

	switch ( d->connected )
	{

		default:
			bug( "Nanny: bad d->connected %d.", d->connected );
			close_socket( d );
			return;

		case CON_GET_ANSI:
			if ( argument[0] == '\0' || (str_prefix( argument, "yes" ) && str_prefix( argument, "no" )) ) {
				write_to_buffer( d, "If you don't know what ANSI is, choose NO.\n\r", 0 );

				if ( d->repeat > 10 ) {
					close_socket( d ); /* this is cause of them jerks  */
					return;
				} else {
					write_to_buffer( d, "Do you wish to use ANSI?  (Yes/No): ", 0 );
					return;
				}
			}

			if (!str_prefix( argument, "yes" ) ) {
				d->ansi = TRUE;
				write_to_buffer( d, "ANSI set.\n\r", 0 );
			} else if (!str_prefix( argument, "no" ) ) {
				write_to_buffer( d, "ANSI not set.\n\r", 0 );
			} else {
				return;
			}

			d->connected = CON_GET_NAME;

			{
				extern char *help_greeting;

				if ( help_greeting[0] == '.' ) {
					write_to_buffer( d, help_greeting + 1, 0 );
				} else {
					write_to_buffer( d, help_greeting, 0 );
				}

				write_to_buffer( d, "&WWhat is your name? ", 0 );
			}

			break;

		case CON_GET_NAME:
			if ( argument[0] == '\0' ) {
				write_to_buffer( d, "&WWhat is your name? ", 0 );
				return;
			}

			argument[0] = UPPER( argument[0] );

			log_string( argument, CHANNEL_NONE , -1 );

			fOld = load_char_obj( d, argument );

			ch   = d->character;

			if ( !check_parse_name( argument ) ) {
				if ( !fOld ) {
					write_to_buffer( d, "&cIllegal name, try another.\n\r&WName&w: ", 0 );

					if ( d->character ) {
						free_char( d->character );
					}

					d->character = NULL;
					return;
				} else {
					sprintf( buf, "Illegal name:  %s", argument );
					bug( buf, 0 );
				}
			}

			if ( IS_SET( ch->act, PLR_DENY ) ) {
				sprintf( log_buf, "Denying access to %s@%s.", argument, d->host );
				log_string( log_buf, CHANNEL_GOD , -1 );
				write_to_buffer( d, "&cYou are denied access.\n\r", 0 );
				close_socket( d );

				return;
			}

			if ( check_reconnect( d, argument, FALSE ) ) {
				fOld = TRUE;
			} else {
				/* Must be immortal with wizbit in order to get in */
				if ( wizlock && !IS_HERO( ch ) && !ch->wizbit ) {
					write_to_buffer( d, "&cThe game is open for Imms only right now. Come back later..\n\r", 0 );
					close_socket( d );
					return;
				}

				if ( ch->level <= numlock && !ch->wizbit && numlock != 0 ) {
					write_to_buffer( d, "&cThe game is locked to your level character.\n\r\n\r", 0 );
					if ( ch->level == 0 ) {
						write_to_buffer( d, "New characters are now temporarily in email ", 0 );
						write_to_buffer( d, "registration mode.\n\r\n\r", 0 );
						write_to_buffer( d, "Please email storm@netway.com to ", 0 );
						write_to_buffer( d, "register your character.\n\r\n\r", 0 );
						write_to_buffer( d, "One email address per character please.\n\r", 0 );
						write_to_buffer( d, "Thank you, EnvyMud Staff.\n\r\n\r", 0 );
					}

					close_socket( d ) ;
					return;
				}
			}

			if ( fOld ) {
				/* Old player */
				write_to_buffer( d, "&WPassword&w: ", 0 );
				write_to_buffer( d, echo_off_str, 0 );

				d->connected = CON_GET_OLD_PASSWORD;

				return;
			} else {
				/* New player */
				if ( check_playing( d, ch->name ) ) {
					return;
				}

				if ( check_newban( d, FALSE ) ) {
					close_socket( d );
					return;
				}

				sprintf( buf, "&cDid I get that right&w, &W%s &w(&WY&w/&WN&w)? ", argument );
				write_to_buffer( d, buf, 0 );

				d->connected = CON_CONFIRM_NEW_NAME;

				return;
			}

			break;

		case CON_GET_OLD_PASSWORD:
			write_to_buffer( d, "\n\r", 2 );

			if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
			{
				write_to_buffer( d, "&cWrong password.\n\r", 0 );
				close_socket( d );
				return;
			}

			write_to_buffer( d, echo_on_str, 0 );

			if ( check_playing( d, ch->name ) )
				return;

			if ( check_reconnect( d, ch->name, TRUE ) )
				return;

			if ( ch->level != L_IMP )
			{
				sprintf( log_buf, "%s!%s@%s has connected.",
						ch->name, d->user, d->host );
				wiznet(log_buf,NULL,NULL,WIZ_SITES,0,get_trust(ch));
			}
			if ( IS_HERO( ch ) )
				do_help( ch, "imotd" );
			do_help( ch, "motd" );
			d->connected = CON_READ_MOTD;
			break;

		case CON_CONFIRM_NEW_NAME:
			switch ( *argument )
			{
				case 'y': case 'Y':
					sprintf( buf,
							"&cNew character&w.\n\r&cGive me a &Wpassword&c for %s&w: %s",
							ch->name, echo_off_str );
					write_to_buffer( d, buf, 0 );
					d->connected = CON_GET_NEW_PASSWORD;
					break;

				case 'n': case 'N':
					write_to_buffer( d, "&WOk&w, &Wwhat IS it&w, &Wthen&w? ", 0 );
					free_char( d->character );
					d->character = NULL;
					d->connected = CON_GET_NAME;
					break;

				default:
					write_to_buffer( d, "&cPlease type &WYes &cor &WNo&w: ", 0 );
					break;
			}
			break;

		case CON_GET_NEW_PASSWORD:
			write_to_buffer( d, "\n\r", 2 );

			if ( strlen( argument ) < 5 )
			{
				write_to_buffer( d,
						"&cPassword must be at least five characters long&w.\n\r&WPassword&w: ",
						0 );
				return;
			}

			pwdnew = crypt( argument, ch->name );
			for ( p = pwdnew; *p != '\0'; p++ )
			{
				if ( *p == '~' )
				{
					write_to_buffer( d,
							"&cNew password not acceptable&w, &ctry again&w.\n\r&WPassword&w: ",
							0 );
					return;
				}
			}

			free_string( ch->pcdata->pwd );
			ch->pcdata->pwd = str_dup( pwdnew );
			write_to_buffer( d, "&cPlease retype &Wpassword&w: ", 0 );
			d->connected = CON_CONFIRM_NEW_PASSWORD;
			break;

		case CON_CONFIRM_NEW_PASSWORD:
			write_to_buffer( d, "\n\r", 2 );

			if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
			{
				write_to_buffer( d,
						"&cPasswords don't match&w.\n\r&cRetype &Wpassword&w: ",
						0 );
				d->connected = CON_GET_NEW_PASSWORD;
				return;
			}

			write_to_buffer( d, echo_on_str, 0 );
			write_to_buffer( d, "&cWhat is your sex &w(&WM&w/&WF&w/&WN&w)? ", 0 );
			d->connected = CON_GET_NEW_SEX;
			break;

		case CON_GET_NEW_SEX:
			switch ( argument[0] )
			{
				case 'm': case 'M': ch->sex = SEX_MALE;    break;
				case 'f': case 'F': ch->sex = SEX_FEMALE;  break;
				case 'n': case 'N': ch->sex = SEX_NEUTRAL; break;
				default:
									write_to_buffer( d,
											"&cThat's not a sex&w.\n\r&WWhat IS your sex&w? ", 0 );
									return;
			}
			buf2[0] = '\0';
			strcpy( buf2,
					"\n\r                  &z-= &RSelect a race from the list below &z=-\n\r\n\r" );
			for ( iRace = 0; iRace < MAX_RACE; iRace++ )
			{
				sprintf( buf, "&z[&W%-12s&z]%s",
						race_table[iRace].race_full,
						(( iRace+1) % 5) == 0 ? "\n\r" : "" );
				strcat( buf2, buf );
			}
			if ( iRace % 5 != 0 )
				strcat( buf2, "\n\r" );
			strcat( buf2, "\n\r&RRACE &w-> " );
			write_to_buffer( d, buf2, 0 );
			wiznet("Newbie alert!  $N sighted.",ch,NULL,WIZ_NEWBIE,0,0);
			d->connected = CON_GET_NEW_RACE;
			break;
		case CON_GET_NEW_RACE:
			for ( iRace = 0; iRace < MAX_RACE; iRace++ )
			{
				if ( ( !str_prefix( argument, race_table[iRace].race_name ) )
						|| ( !str_cmp( argument, race_table[iRace].race_full ) ) )
				{
					ch->race = iRace;
					break;
				}
			}

			if ( iRace == MAX_RACE )
			{
				write_to_buffer( d,
						"&cThat's not a race&w.\n\r&WWhat IS your race&w? ", 0 );
				return;
			}
			sprintf( buf, "%s %s", race_table[iRace].race_full,
					race_table[iRace].race_name );
			do_help( ch, buf );
			write_to_buffer( d, "&WIs this the race you desire&w? ", 0 );
			d->connected = CON_CONFIRM_RACE;
			break;

		case CON_CONFIRM_RACE:
			switch ( *argument )
			{
				case 'y': case 'Y':
					/* booger */
					if (IS_SET(ch->act2, PLR_REMORT ))
					{
						ch->pcdata->mod_str = race_table[d->character->race].mstr;
						ch->pcdata->mod_int = race_table[d->character->race].mint;
						ch->pcdata->mod_wis = race_table[d->character->race].mwis;
						ch->pcdata->mod_dex = race_table[d->character->race].mdex;
						ch->pcdata->mod_con = race_table[d->character->race].mcon;
					}
					buf2[0]='\0';
					strcpy( buf2,
							"\n\r                 &z-= &RSelect a class from the list below &z=-"
							"\n\r           &cYou may type &RBACK &cto return to previous selection.\n\r\n\r"
						  );
					iCount = 0;
					for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
					{
						if ( class_table[iClass].races[ch->race] )
						{
							iCount++;
							sprintf( buf, "&z[&W%-12s&z]%s",
									class_table[iClass].who_long,
									iCount % 5 == 0 ? "\n\r" : "" );
							strcat( buf2, buf );
						}
					}
					if ( iCount % 5 != 0 )
						strcat( buf2, "\n\r" );
					strcat( buf2, "\n\r&R1ST CLASS &w-> " );
					write_to_buffer( d, buf2, 0 );
					d->connected = CON_GET_NEW_CLASS;
					break;
				case 'n': case 'N':
					strcpy( buf2,
							"\n\r                  &z-= &RSelect a race from the list below &z=-\n\r\n\r" );
					for ( iRace = 0; iRace < MAX_RACE; iRace++ )
					{
						sprintf( buf, "&z[&W%-12s&z]%s",
								race_table[iRace].race_full,
								(( iRace+1) % 5) == 0 ? "\n\r" : "" );
						strcat( buf2, buf );
					}
					if ( iRace % 5 != 0 )
						strcat( buf2, "\n\r" );
					strcat( buf2, "\n\r&RRACE &w-> " );
					write_to_buffer( d, buf2, 0 );
					d->connected = CON_GET_NEW_RACE;
					break;

				default:
					write_to_buffer( d,
							"&cPlease type &WYes &cor &WNo&w: ", 0 );
					break;
			}
			break;

		case CON_GET_NEW_CLASS:
			if ( !str_cmp( argument, "back" ) )
			{
				strcpy( buf2,
						"\n\r                  &z-= &RSelect a race from the list below &z=-\n\r\n\r" );
				for ( iRace = 0; iRace < MAX_RACE; iRace++ )
				{
					sprintf( buf, "&z[&W%-12s&z]%s",
							race_table[iRace].race_full,
							(( iRace+1) % 5) == 0 ? "\n\r" : "" );
					strcat( buf2, buf );
				}
				if ( iRace % 5 != 0 )
					strcat( buf2, "\n\r" );
				strcat( buf2, "\n\r&RRACE &w-> " );
				write_to_buffer( d, buf2, 0 );
				d->connected = CON_GET_NEW_RACE;
				return;
			}
			for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
			{
				if ( !str_prefix( argument, class_table[iClass].who_long )
						&& class_table[iClass].races[ch->race] )
				{
					ch->class[0] = iClass;
					break;
				}
			}

			if ( iClass == MAX_CLASS )
			{
				write_to_buffer( d,
						"That's not a class.\n\rWhat IS your class? ", 0 );
				return;
			}

			do_help( ch, (char *)class_table[iClass].who_long );
			write_to_buffer( d, "&WIs this the class you desire&w? ", 0 );
			d->connected = CON_CONFIRM_CLASS;
			break;

		case CON_CONFIRM_CLASS:
			switch ( *argument )
			{
				case 'y': case 'Y':
					/* booger */
					if (IS_SET( ch->act2, PLR_REMORT ))
					{
						switch ( class_table[prime_class(ch)].attr_prime )
						{
							case APPLY_STR: ch->pcdata->perm_str = 16; break;
							case APPLY_INT: ch->pcdata->perm_int = 16; break;
							case APPLY_WIS: ch->pcdata->perm_wis = 16; break;
							case APPLY_DEX: ch->pcdata->perm_dex = 16; break;
							case APPLY_CON: ch->pcdata->perm_con = 16; break;
						}
					}
					argument[0] = '\0';
					do_help( ch, "MULTICLASS" );
					sprintf( buf, "&WDo you wish to be multi-classed&w? " );
					write_to_buffer( d, buf, 0 );
					d->connected = CON_WANT_MULTI;
					break;

				case 'n': case 'N':
					strcpy( buf2,
							"\n\r                 &z-= &RSelect a class from the list below &z=-"
							"\n\r           &cYou may type &RBACK &cto return to previous selection.\n\r\n\r"
						  );
					iCount = 0;
					for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
					{
						if ( class_table[iClass].races[ch->race] )
						{
							iCount++;
							sprintf( buf, "&z[&W%-12s&z]%s",
									class_table[iClass].who_long,
									iCount % 5 == 0 ? "\n\r" : "" );
							strcat( buf2, buf );
						}
					}
					if ( iCount % 5 != 0 )
						strcat( buf2, "\n\r" );
					strcat( buf2, "\n\r&R1ST CLASS &w-> " );
					write_to_buffer( d, buf2, 0 );
					d->connected = CON_GET_NEW_CLASS;
			}
			break;
		case CON_WANT_MULTI:
			switch ( *argument )
			{
				case 'y': case 'Y':
					strcpy( buf2,
							"\n\r                 &z-= &RSelect a class from the list below &z=-"
							"\n\r           &cYou may type &RBACK &cto return to previous selection.\n\r\n\r"
						  );
					iCount = 0;
					for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
					{
						if ( class_table[iClass].races[ch->race]
								&& prime_class( ch ) != iClass
								&& class_table[prime_class(ch)].multi[iClass] )
						{
							iCount++;
							sprintf( buf, "&z[&W%-12s&z]%s",
									class_table[iClass].who_long,
									iCount % 5 == 0 ? "\n\r" : "" );
							strcat( buf2, buf );
						}
					}
					if ( iCount % 5 != 0 )
						strcat( buf2, "\n\r" );
					strcat( buf2, "\n\r&R2ND CLASS &w-> " );
					write_to_buffer( d, buf2, 0 );
					d->connected = CON_GET_2ND_CLASS;
					break;
				case 'n': case 'N':
					if (IS_SET(ch->act2, PLR_REMORT ))
						ch->exp = xp_tolvl(ch)/2;
					ch->class[1] = -1;
					write_to_buffer( d, "&cPress &W'y' &cto enter a description or &WEnter &cto continue&w ", 0 );
					do_help( ch, "motd" );
					d->connected = CON_READ_MOTD;
					break;
				default:
					write_to_buffer( d, "&cPlease type &WYes &cor &WNo&w: ", 0 );
					break;
			}
			break;
		case CON_GET_2ND_CLASS:
			if ( !str_cmp( argument, "back" ) )
			{
				strcpy( buf2,
						"\n\r                 &z-= &RSelect a class from the list below &z=-"
						"\n\r           &cYou may type &RBACK &cto return to previous selection.\n\r\n\r"
					  );
				iCount = 0;
				for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
				{
					if ( class_table[iClass].races[ch->race] )
					{
						iCount++;
						sprintf( buf, "&z[&W%-12s&z]%s",
								class_table[iClass].who_long,
								iCount % 5 == 0 ? "\n\r" : "" );
						strcat( buf2, buf );
					}
				}
				if ( iCount % 5 != 0 )
					strcat( buf2, "\n\r" );
				strcat( buf2, "\n\r&R1ST CLASS &w-> " );
				write_to_buffer( d, buf2, 0 );
				d->connected = CON_GET_NEW_CLASS;
				return;
			}
			for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
			{
				if ( !str_prefix( argument, class_table[iClass].who_long )
						&& class_table[iClass].races[ch->race]
						&& prime_class( ch ) != iClass
						&& class_table[prime_class(ch)].multi[iClass] )
				{
					ch->class[1] = iClass;
					break;
				}
			}

			if ( iClass == MAX_CLASS )
			{
				write_to_buffer( d,
						"&cThat's not a class&w.\n\r&WWhat IS your class&w? ", 0 );
				return;
			}

			do_help( ch, (char *)class_table[iClass].who_long );
			write_to_buffer( d, "&WIs this the class you desire&w? ", 0 );
			d->connected = CON_CONFIRM_2ND_CLASS;
			break;

		case CON_CONFIRM_2ND_CLASS:
			switch ( *argument )
			{
				case 'y': case 'Y':
					if (IS_SET(ch->act2, PLR_REMORT )) {
						ch->exp = xp_tolvl(ch)/2;
					}

					ch->class[2] = -1;

					write_to_buffer( d, "&cPress &W'y' &cto enter a description or &WEnter &cto continue&w: ", 0 );
					do_help(ch, "motd");
					
					d->connected = CON_READ_MOTD;

					break;
				case 'n': case 'N':
					strcpy( buf2,
							"\n\r                 &z-= &RSelect a class from the list below &z=-"
							"\n\r           &cYou may type &RBACK &cto return to previous selection.\n\r\n\r"
						  );
					iCount = 0;
					for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
					{
						if ( class_table[iClass].races[ch->race]
								&& prime_class( ch ) != iClass
								&& class_table[prime_class(ch)].multi[iClass] )
						{
							iCount++;
							sprintf( buf, "&z[&W%-12s&z]%s",
									class_table[iClass].who_long,
									iCount % 5 == 0 ? "\n\r" : "" );
							strcat( buf2, buf );
						}
					}
					if ( iCount % 5 != 0 )
						strcat( buf2, "\n\r" );
					strcat( buf2, "\n\r&R2ND CLASS &w-> " );
					write_to_buffer( d, buf2, 0 );
					d->connected = CON_GET_2ND_CLASS;
			}
			break;

		case CON_CHECK_AUTHORIZE:
			/*switch ( *argument )
			  {
			  case 'y': case 'Y':
			//   string_append( ch, &ch->description );
			}*/
			ch->start_align = 'N';
			sprintf( log_buf, "%s!%s@%s new player.", ch->name, d->user,d->host );
			log_string( log_buf, CHANNEL_LOG, -1 );
			write_to_buffer( d, "\n\r", 2 );
			ch->pcdata->pagelen = 60;
			do_help( ch, "motd" );

			/* if authorize is on:  */

			if (!auth_off)
			{
				d->connected = CON_AUTHORIZE_NAME;
				sprintf(log_buf, "%s!%s@%s a %s %s %s %s needs be to AUTHORIZED.",
						ch->name, d->user, d->host,
						IS_GOOD( ch ) ? "good" :
						IS_EVIL( ch ) ? "evil" : "neutral",
						ch->sex == SEX_MALE    ? "male"   :
						ch->sex == SEX_FEMALE  ? "female" : "neuter",
						race_table[ch->race].race_full,
						class_short( ch ) );
				log_string(log_buf, CHANNEL_LOG, -1 );
				wiznet(log_buf, ch, NULL, WIZ_GENERAL, 0, 0 );
				write_to_buffer(d,
						"Now, please wait for an immortal to authorize you.\n\r", 0);
			}
			else
				d->connected = CON_READ_MOTD;
			break;
			/* XOR authorize */
		case CON_AUTHORIZE_NAME:
		case CON_AUTHORIZE_NAME1:
		case CON_AUTHORIZE_NAME2:
		case CON_AUTHORIZE_NAME3:
			write_to_buffer(d, "Please wait for an immortal to authorize you.\n\r", 0 );
			sprintf(log_buf, "%s!%s@%s a %s %s %s %s needs be to AUTHORIZED.",
					ch->name, d->user, d->host,
					IS_GOOD( ch ) ? "good" :
					IS_EVIL( ch ) ? "evil" : "neutral",
					ch->sex == SEX_MALE    ? "male"   :
					ch->sex == SEX_FEMALE  ? "female" : "neuter",
					race_table[ch->race].race_full,
					class_short( ch ) );
			log_string(log_buf, CHANNEL_LOG, -1 );
			wiznet( log_buf, ch, NULL, WIZ_GENERAL, 0, 0);
			d->connected++;
			if(d->connected == CON_AUTHORIZE_LOGOUT)
			{
				write_to_buffer( d, "Auto exit to prevent spam.\n\r", 0 );
				sprintf(log_buf, "%s!%s@%s auto logged off.",
						ch->name, d->user, d->host);
				log_string(log_buf, CHANNEL_LOG, -1 );
				wiznet( log_buf, ch, NULL, WIZ_GENERAL, 0, 0);
				close_socket( d );
				return;
			}
			break;
			/* END */
		case CON_BEGIN_REMORT:
			buf2[0] = '\0';
			strcpy( buf2,
					"\n\r                  &z-= &RSelect a race from the list below &z=-\n\r\n\r" );
			for ( iRace = 0; iRace < MAX_RACE; iRace++ )
			{
				sprintf( buf, "&z[&W%-12s&z]%s",
						race_table[iRace].race_full,
						(( iRace+1) % 5) == 0 ? "\n\r" : "" );
				strcat( buf2, buf );
			}
			if ( iRace % 5 != 0 )
				strcat( buf2, "\n\r" );
			strcat( buf2, "\n\r&RRACE &w-> " );
			write_to_buffer( d, buf2, 0 );
			wiznet("Remort reconnected!  $N sighted.",ch,NULL,WIZ_NEWBIE,0,0);
			d->connected = CON_GET_NEW_RACE;
			break;
		case CON_READ_MOTD:
			ch->next    = char_list;
			char_list   = ch;
			d->connected    = CON_PLAYING;

			/*    send_to_char(AT_GREEN,
				  "\n\rWelcome to The Eye of the &BS&Ct&Wo&Cr&Bm&G."
				  "  May your visit here be tempest tossed.\n\r",
				  ch ); */

			if ( ch->level == 0 )
			{
				switch ( class_table[prime_class( ch )].attr_prime )
				{
					case APPLY_STR: ch->pcdata->perm_str = 16; break;
					case APPLY_INT: ch->pcdata->perm_int = 16; break;
					case APPLY_WIS: ch->pcdata->perm_wis = 16; break;
					case APPLY_DEX: ch->pcdata->perm_dex = 16; break;
					case APPLY_CON: ch->pcdata->perm_con = 16; break;
				}

				ch->pcdata->mod_str += race_table[ch->race].mstr;
				ch->pcdata->mod_int += race_table[ch->race].mint;
				ch->pcdata->mod_wis += race_table[ch->race].mwis;
				ch->pcdata->mod_dex += race_table[ch->race].mdex;
				ch->pcdata->mod_con += race_table[ch->race].mcon;
				SET_BIT( ch->act, PLR_AUTOEXIT + PLR_AUTOCOINS );
				ch->level   = 1;
				ch->exp = number_classes( ch ) == 1 ? 1000
					: number_classes( ch ) * 1500;
				ch->hit     = MAX_HIT(ch);
				ch->mana    = MAX_MOVE(ch);
				ch->move    = MAX_MOVE(ch);
				/* charisma */
				if ( ( ch->race == RACE_HUMAN ) ||
						( ch->race == RACE_DWARF ) )
					ch->charisma = number_range( 20, 24 );
				if ( ( ch->race == RACE_GNOME )  ||
						( ch->race == RACE_ELF )    ||
						( ch->race == RACE_HALFLING ) )
					ch->charisma = number_range( 25, 30 );

				sprintf( buf, "the %s %s",
						race_table[ch->race].race_full,
						class_table[prime_class(ch)].who_long );
				set_title( ch, buf );
				ch->prompt = str_dup( "<&Y%hhp &C%mm &G%vmv&w> " );
				if ( ch->start_align == 'E' )
					char_to_room( ch, get_room_index( ROOM_VNUM_RW_SCHOOL ) );
				else
					char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
			}
			else if ( ch->in_room )
			{
				char_to_room( ch, ch->in_room );
			}
			else if ( IS_IMMORTAL( ch ) )
			{
				char_to_room( ch, get_room_index( ROOM_VNUM_CHAT ) );
			}
			else
			{
				char_to_room( ch, get_room_index( ROOM_VNUM_TEMPLE ) );
			}

			if ( !IS_SET( ch->act, PLR_WIZINVIS )
					&& !IS_AFFECTED( ch, AFF_INVISIBLE ) )
				act(AT_GREEN, "$n has returned to the storm.",
						ch, NULL, NULL, TO_ROOM );
			else
			{
				CHAR_DATA *gch;
				for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
				{
					if ( ch != gch )
						if ( get_trust(gch) >= ch->wizinvis )
						{
							sprintf (buf,
									"%s slightly phased has returned to the storm.\n\r",
									ch->name);
							send_to_char ( AT_GREEN, buf, gch );
						}
				}
			}

			if ( !IS_SET( ch->act, PLR_WIZINVIS ) && !IS_SET ( ch->act, PLR_CLOAKED) ) {
				info( "%s has returned to the storm.", (int)(ch->name), 0 );
			}

			/* Log player login */
			json_t *obj = json_object();
			json_object_set_new(obj, "player_name", json_string( ch->name ));
			write_analytics(obj, "logins");

			if ( ch->pcdata->corpses >= 2 )
			{
				char strfng [ MAX_INPUT_LENGTH ];
				ch->pcdata->corpses = 0;
				/* player files parsed directories by Yaz 4th Realm */
				sprintf( strfng, "%s%c/%s.cps", PLAYER_DIR, LOWER( ch->name[0] ), capitalize( ch->name ) );
						if ( remove( strfng ) != 0 )
							perror( strfng );
						}

						sprintf(log_buf,"%s!%s@%s has connected.", ch->name, d->user, d->host);
						log_string(log_buf, (ch->level == L_IMP ? 1 : CHANNEL_LOG), ch->level - 1 );

						if ( !IS_NPC( ch ) && ch->pcdata->storage )
						{
						OBJ_DATA *obj;
						OBJ_DATA *obj_next;
						int count = 1;
						char buf[MAX_STRING_LENGTH];

						for ( obj = ch->pcdata->storage; obj; obj = obj_next, count++ )
						{
						obj_next = obj->next_content;
						if ( (ch->pcdata->bankaccount.gold +
									(ch->pcdata->bankaccount.silver/S_PER_G) +
									(ch->pcdata->bankaccount.copper/C_PER_G) ) < count * 1000 )
							{
								sprintf( buf,
										"The bank has repossessed %s from your storage.\n\r",
										obj->short_descr );
								send_to_char( AT_RED, buf, ch );
								obj_from_storage( obj );
								extract_obj( obj );
							}
						}

				sprintf( buf,
						"The bank deducts %dgp from your account for the storage of %d items.\n\r",
						ch->pcdata->storcount * 1000, ch->pcdata->storcount );
				send_to_char( AT_PINK, buf, ch );
				amt.silver = amt.copper = 0;
				amt.gold = ch->pcdata->storcount * 1000;
				spend_money( &ch->pcdata->bankaccount, &amt );
						}

				do_look( ch, "auto" );

				/* Here to make sure ch isn't switched --Angi */
				ch->pcdata->switched = FALSE;

				/* check for new notes */
				notes = 0;

				for ( pnote = note_list; pnote; pnote = pnote->next )
					if ( is_note_to( ch, pnote ) && str_cmp( ch->name, pnote->sender )
							&& pnote->date_stamp > ch->last_note && pnote->on_board == 0 )
						notes++;

				if ( notes == 1 )
					send_to_char(AT_WHITE,
							"\n\rYou have one new note waiting.\n\r", ch );
				else
					if ( notes > 1 )
					{
						sprintf( buf, "\n\rYou have %d new notes waiting.\n\r",
								notes );
						send_to_char(AT_WHITE, buf, ch );
					}
				break;
	}
	return;
}

/*
 * Parse a name for acceptability.
 */
bool check_parse_name( char *name )
{
	/*
	 * Reserved words.	OLC 1.1b
	 */
	if ( is_exact_name( name, "all auto immortal self someone none" ) )
		return FALSE;

	/*
	 * Length restrictions.
	 */
	if ( strlen( name ) <  3 )
		return FALSE;

	if ( strlen( name ) > 12 )
		return FALSE;

	/*
	 * Alphanumerics only.
	 * Lock out IllIll twits.
	 */
	{
		char *pc;
		bool fIll;

		fIll = TRUE;
		for ( pc = name; *pc != '\0'; pc++ )
		{
			if ( !isalpha( *pc ) )
				return FALSE;
			if ( LOWER( *pc ) != 'i' && LOWER( *pc ) != 'l' )
				fIll = FALSE;
		}

		if ( fIll )
			return FALSE;
	}

	/*
	 * Prevent players from naming themselves after mobs.
	 */
	{
		extern MOB_INDEX_DATA *mob_index_hash [ MAX_KEY_HASH ];
		MOB_INDEX_DATA *pMobIndex;
		int             iHash;

		for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
		{
			for ( pMobIndex  = mob_index_hash[iHash];
					pMobIndex;
					pMobIndex  = pMobIndex->next )
			{
				if ( is_exact_name( name, pMobIndex->player_name ) )
					return FALSE;
			}
		}
	}

	return TRUE;
}



/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect( DESCRIPTOR_DATA *d, char *name, bool fConn )
{
	OBJ_DATA  *obj;
	CHAR_DATA *ch;

	for ( ch = char_list; ch; ch = ch->next )
	{
		if ( !IS_NPC( ch ) && !ch->deleted
				&& ( !fConn || !ch->desc )
				&& !str_cmp( d->character->name, ch->name ) )
		{
			if ( fConn == FALSE )
			{
				free_string( d->character->pcdata->pwd );
				d->character->pcdata->pwd = str_dup( ch->pcdata->pwd );
			}
			else
			{
				free_char( d->character );
				d->character = ch;
				ch->desc     = d;
				ch->timer    = 0;
				send_to_char(AT_GREEN, "Reconnecting.\n\r", ch );
				act(AT_GREEN, "$n has reconnected.", ch, NULL, NULL, TO_ROOM );
				sprintf( log_buf, "%s!%s@%s reconnected.", ch->name, d->user, d->host );
				wiznet( log_buf,ch,NULL,WIZ_LINKS,0,0);
				if ( !IS_SET( ch->act, PLR_WIZINVIS )
						&& !IS_SET( ch->act, PLR_CLOAKED ) )
					info( "%s has re-established a link to the storm.",
							(int)(ch->name), 0 );
				if ( IS_AFFECTED2( ch, AFF_PLOADED ) )
					REMOVE_BIT( ch->affected_by2, AFF_PLOADED );
				d->connected = CON_PLAYING;
				/*
				 * Contributed by Gene Choi
				 */
				if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) )
						&& obj->item_type == ITEM_LIGHT
						&& obj->value[2] != 0
						&& ch->in_room )
					++ch->in_room->light;

			}
			return TRUE;
		}
	}

	return FALSE;
}



/*
 * Check if already playing.
 */
bool check_playing( DESCRIPTOR_DATA *d, char *name )
{
	DESCRIPTOR_DATA *dold;

	for ( dold = descriptor_list; dold; dold = dold->next )
	{
		if ( dold != d
				&& dold->character
				&& dold->connected != CON_GET_NAME
				&& dold->connected != CON_GET_OLD_PASSWORD
				&& !str_cmp( name, dold->original
					? dold->original->name : dold->character->name ) )
		{
			if ( dold->connected == CON_PLAYING )
			{
				free_char(d->character);
				d->character = (dold->original ? dold->original :
						dold->character );
				if ( dold->original )
					dold->original->desc = NULL;
				dold->character->desc = NULL;
				d->character->desc = d;
				dold->original = NULL;
				dold->character = NULL;
				write_to_buffer(dold, "Kicking off old link.\n\r", 0);
				close_socket(dold);

				if ( !IS_SET( d->character->act, PLR_WIZINVIS ) )
					act( AT_GREEN, "A ghostly aura briefly embodies $n.",
							d->character, NULL, NULL, TO_ROOM );
				send_to_char( AT_GREEN, "You arise from netdeath and continue"
						" playing.\n\r", d->character );
				sprintf(log_buf, "%s connects, kicking off old link.",
						d->character->name);
				log_string(log_buf, CHANNEL_LOG, d->character->level);
				wiznet(log_buf, NULL, NULL, WIZ_LINKS, 0, 0);
				if ( !IS_SET( d->character->act, PLR_WIZINVIS )
						&& !IS_SET( d->character->act, PLR_CLOAKED) )
					info(log_buf, 0, 0);
				d->connected = CON_PLAYING;
				return TRUE;
			}
			write_to_buffer( d, "Already playing.\n\r Name: ", 0 );
			d->connected = CON_GET_NAME;
			if ( d->character )
			{
				free_char( d->character );
				d->character = NULL;
			}
			return TRUE;
		}
	}

	return FALSE;
}



void stop_idling( CHAR_DATA *ch )
{
	if (   !ch
			|| !ch->desc
			||  ch->desc->connected != CON_PLAYING
			|| !ch->was_in_room
			||  ch->in_room != get_room_index( ROOM_VNUM_LIMBO ) )
		return;

	ch->timer = 0;
	char_from_room( ch );
	char_to_room( ch, ch->was_in_room );
	ch->was_in_room	= NULL;
	act(AT_GREEN, "$n has returned from the void.", ch, NULL, NULL, TO_ROOM );
	return;
}

/*
 * Write to all characters.
 */
void send_to_all_char( const char *text ) {
	DESCRIPTOR_DATA *d;

	if ( !text ) {
		return;
	}

	for ( d = descriptor_list; d; d = d->next ) {
		if ( d->connected == CON_PLAYING ) {
			send_to_char(AT_YELLOW, text, d->character );
		}
	}

	return;
}

/*
 * Write to one char.
 */
void send_to_char(int AType, const char *txt, CHAR_DATA *ch ) {
	if ( !ch || !txt || !ch->desc ) {
		return;
	}

	free_string( ch->desc->showstr_head );

	if ( strstr( txt, "&X" ) ) {
		set_str_color(AType, txt);
	}

	ch->desc->showstr_head  = str_dup(txt);
	ch->desc->showstr_point = ch->desc->showstr_head;
	set_char_color( AType, ch );
	show_string( ch->desc, "" );
}

void set_char_color( int AType, CHAR_DATA *ch )
{
	char buf[32];

	if ( ch && ch->desc && ch->desc->ansi )
	{
		if (AType>15)
			sprintf( buf,"\033[0;%d;5;%dm",(((AType-16) & 8)==8),30+((AType-16) & 7) );
		else
			sprintf( buf,"\033[0;%d;%dm",((AType & 8)==8),30+(AType & 7) );
		write_to_buffer( ch->desc, buf, strlen(buf) );
	}
	return;
}

/* Fixed a couple things.. 1) it didnt need to str_dup its return text, and
   2) killed the second color table.. colors from above will work just fine
   -- Alty
   */
char *set_str_color( int AType, const char *txt )
{
	char *ptr, *str = "&X";
	for ( ; ; )
	{
		ptr = strstr(txt, str);
		if (!ptr)
			break;
		*(ptr+1) = colors[AType];
	}
	return (char *)txt;
}


/* OLC, new pager for editing long descriptions. */ /*
													   ========================================================================= */ /* - The heart of the
																																	   pager.  Thanks to N'Atas-Ha, ThePrincedom for porting */ /* this SillyMud code for MERC 2.0 and
																																																	laying down the groundwork.  */ /* - Thanks to Blackstar, hopper.cs.uiowa.edu 4000 for which the
																																																									   improvements*/ /* to the pager was modeled from.  - Kahn */ /* - Safer, allows very large pagelen
																																																																									  now, and allows to page while switched */ /* Zavod of jcowan.reslife.okstate.edu 4000.  */ /*
																																																																																																	 ========================================================================= */

void show_string( DESCRIPTOR_DATA *d, char *input )  {
	char               *start, *end;
	char                arg[MAX_INPUT_LENGTH];
	int                 lines = 0, pagelen;

	/* Set the page length */
	/* ------------------- */

	pagelen = d->original ? d->original->pcdata->pagelen
		: d->character->pcdata->pagelen;

	/* Check for the command entered */
	/* ----------------------------- */

	one_argument( input, arg );

	switch( UPPER( *arg ) )
	{
		/* Show the next page */

		case '\0':
		case 'C': lines = 0;
				  break;

				  /* Scroll back a page */

		case 'B': lines = -2 * pagelen;
				  break;

				  /* Help for show page */

		case 'H': write_to_buffer( d, "B     - Scroll back one page.\n\r", 0 );
				  write_to_buffer( d, "C     - Continue scrolling.\n\r", 0 );
				  write_to_buffer( d, "H     - This help menu.\n\r", 0 );
				  write_to_buffer( d, "R     - Refresh the current page.\n\r",
						  0 );
				  write_to_buffer( d, "Enter - Continue Scrolling.\n\r", 0 );
				  return;

				  /* refresh the current page */

		case 'R': lines = -1 - pagelen;
				  break;

				  /* stop viewing */

		default:  free_string( d->showstr_head );
				  d->showstr_head  = NULL;
				  d->showstr_point = NULL;
				  return;
	}

	/* do any backing up necessary to find the starting point */
	/* ------------------------------------------------------ */

	if ( lines < 0 )
	{
		for( start= d->showstr_point; start > d->showstr_head && lines < 0;
				start-- )
			if ( *start == '\r' )
				lines++;
	}
	else
		start = d->showstr_point;

	/* Find the ending point based on the page length */
	/* ---------------------------------------------- */

	lines  = 0;

	for ( end= start; *end && lines < pagelen; end++ )
		if ( *end == '\r' )
			lines++;

	d->showstr_point = end;

	if ( end - start )
		write_to_buffer( d, start, end - start );

	/* See if this is the end (or near the end) of the string */
	/* ------------------------------------------------------ */

	for ( ; isspace( *end ); end++ );

	if ( !*end )
	{
		free_string( d->showstr_head );
		d->showstr_head  = NULL;
		d->showstr_point = NULL;
	}

	return;
}


/*
 * The primary output interface for formatted output.
 */
void act(int AType, const char *format, CHAR_DATA *ch, const void *arg1,
		const void *arg2, int type )
{
	OBJ_DATA        *obj1        = (OBJ_DATA  *) arg1;
	OBJ_DATA        *obj2        = (OBJ_DATA  *) arg2;
	CHAR_DATA       *to;
	CHAR_DATA       *vch         = (CHAR_DATA *) arg2;
	static char *    const  he_she  [ ] = { "it",  "he",  "she" };
	static char *    const  him_her [ ] = { "it",  "him", "her" };
	static char *    const  his_her [ ] = { "its", "his", "her" };
	const  char            *str;
	const  char            *i;
	char            *point;
	char             buf     [ MAX_STRING_LENGTH ];
	char             buf2    [ MAX_STRING_LENGTH ];
	char             fname   [ MAX_INPUT_LENGTH  ];

	/*
	 * Discard null and zero-length messages.
	 */
	if ( !format || format[0] == '\0' )
		return;
	set_str_color(AType, format);
	to = ch->in_room->people;
	if ( type == TO_VICT )
	{
		if ( !vch || !vch->in_room)
		{
			bug( "Act: null vch (or vch->in_room) with TO_VICT.", 0 );
			return;
		}
		to = vch->in_room->people;
	}

	for ( ; to; to = to->next_in_room )
	{
		if ( ( to->deleted )
				/*	    || ( !to->desc && IS_NPC( to ) )*/
				|| !IS_AWAKE( to ) )
			continue;

		if ( type == TO_CHAR    && to != ch )
			continue;
		if ( type == TO_VICT    && ( to != vch || to == ch ) )
			continue;
		if ( type == TO_ROOM    && to == ch )
			continue;
		if ( type == TO_NOTVICT && (to == ch || to == vch) )
			continue;
		if ( type == TO_COMBAT &&
				(to == ch || to == vch || !IS_SET(to->act, PLR_COMBAT ) ) )
			continue;

		point	= buf;
		str	= format;
		while ( *str != '\0' )
		{
			if ( *str != '$' )
			{
				*point++ = *str++;
				continue;
			}
			++str;

			if ( !arg2 && *str >= 'A' && *str <= 'Z' )
			{
				bug( "Act: missing arg2 for code %d.", *str );
				sprintf( log_buf, "String: %s.", format );
				bug( log_buf, 0 );
				i = " <@@@> ";
			}
			else
			{
				switch ( *str )
				{
					default:  bug( "Act: bad code %d.", *str );
							  i = " <@@@> ";				break;
							  /* Thx alex for 't' idea */
					case 't': i = (char *) arg1; set_str_color(AType, i);   break;
					case 'T': i = (char *) arg2; set_str_color(AType, i);   break;
					case 'n': i = PERS( ch,  to  );	set_str_color(AType, i);break;
					case 'N': i = PERS( vch, to  );	set_str_color(AType, i);break;
					case 'e': i = he_she  [URANGE( 0, ch  ->sex, 2 )];	break;
					case 'E': i = he_she  [URANGE( 0, vch ->sex, 2 )];	break;
					case 'm': i = him_her [URANGE( 0, ch  ->sex, 2 )];	break;
					case 'M': i = him_her [URANGE( 0, vch ->sex, 2 )];	break;
					case 's': i = his_her [URANGE( 0, ch  ->sex, 2 )];	break;
					case 'S': i = his_her [URANGE( 0, vch ->sex, 2 )];	break;

					case 'p':
							  i = can_see_obj( to, obj1 )
								  ? obj1->short_descr
								  : "something";
							  set_str_color(AType, i);
							  break;

					case 'P':
							  i = can_see_obj( to, obj2 )
								  ? obj2->short_descr
								  : "something";
							  set_str_color(AType, i);
							  break;

					case 'd':
							  if ( !arg2 || ( (char *) arg2 )[0] == '\0' )
							  {
								  i = "door";
							  }
							  else
							  {
								  one_argument( (char *) arg2, fname );
								  i = fname;
							  }
							  set_str_color(AType, i);
							  break;
				}
			}

			++str;
			while ( ( *point = *i ) != '\0' )
				++point, ++i;
		}

		*point++ = '\n';
		*point++ = '\r';
		*point++ = '\0';

		buf[0]   = UPPER( buf[0] );
		if (to->desc && to->desc->ansi)
		{
			if (AType>15)
				sprintf(buf2,"\033[0;%d;5;%dm",(((AType-16) & 8)==8),30+((AType-16) & 7));
			else
				sprintf(buf2,"\033[0;%d;%dm",((AType & 8)==8),30+(AType & 7));
			strcat(buf2, buf);
		}
		else
			strcpy(buf2, buf);
		if ( to->desc )
			write_to_buffer( to->desc, buf2, strlen(buf2) );
		if (MOBtrigger)
			mprog_act_trigger( buf, to, ch, obj1, vch );
	}
	MOBtrigger = TRUE;
	return;
}

void do_authorize(CHAR_DATA *ch, char *argument)
{
	DESCRIPTOR_DATA *d;
	char arg1[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int mode = 0;

	if(argument[0] != '\0')
	{
		argument = one_argument(argument, arg1);
		if (auth_off && !str_cmp(arg1, "on"))
		{
			if(ch->level > L_DIR) /* Level 112+ */
			{
				auth_off = FALSE;
				send_to_char(AT_YELLOW, "Authorization is now ON.\n\r", ch );
			}
			else
				send_to_char(C_DEFAULT, "You are not authorized to issue this command.\n\r", ch );
			return;
		}
		else if (!auth_off && !str_cmp(arg1, "off"))
		{
			if(ch->level > L_DIR) /* Level 112+ */
			{
				auth_off = TRUE;
				send_to_char(AT_YELLOW, "Authorization is now OFF.\n\r", ch );
			}
			else
				send_to_char(C_DEFAULT, "You are not authorized to issue this command.\n\r", ch );
			return;
		}
		argument = one_argument(argument, arg2);
		if(arg2[0] == '\0' || !str_prefix(arg2, "yes"))
			mode = 1;
		else if(!str_prefix(arg2, "no"))
			mode = 2;
		/*    else if(!strcmp(arg2, "m") || !strcmp(arg2, "message"))
			  mode = 3;*/
		else
			mode = 3;
	}
	else
	{
		send_to_char(AT_DGREY,"People waiting for authorization:\n\r",ch);
		send_to_char(AT_DGREY,"---------------------------------\n\r",ch);
	}
	for(d = descriptor_list;d;d = d_next)
	{
		d_next	= d->next;
		if(!d->character) continue;
		if(!d->character->name) continue;
		if(mode == 0)
		{
			if(d->connected >= CON_AUTHORIZE_NAME
					&& d->connected <= CON_AUTHORIZE_LOGOUT)
			{
				sprintf(buf, "  %s!%s@%s a %s %s %s %s\n\r",
						d->character->name, d->user, d->host,
						IS_GOOD( d->character ) ? "good" :
						IS_EVIL( d->character ) ? "evil" : "neutral",
						d->character->sex == SEX_MALE    ? "male"   :
						d->character->sex == SEX_FEMALE  ? "female" : "neuter",
						race_table[d->character->race].race_full,
						class_short( d->character ) );
				send_to_char(C_DEFAULT, buf, ch);
			}
		}
		else if(d->connected >= CON_AUTHORIZE_NAME
				&& d->connected <= CON_AUTHORIZE_LOGOUT
				&& is_exact_name(d->character->name, arg1))
		{
			if(mode == 1)
			{
				send_to_char(C_DEFAULT, "Character authorized.\n\r", ch);
				d->connected = CON_READ_MOTD;
				write_to_buffer(d, "You have been authorized.\n\r", 0 );
				sprintf(buf, "%s!%s@%s AUTHORIZED by %s.",
						d->character->name, d->user, d->host, ch->name);
				log_string(buf, CHANNEL_LOG, -1 );
				append_file( ch, AUTH_LIST, buf );
				wiznet( buf, ch, NULL, WIZ_GENERAL, 0, 0 );
				return;
			}
			else
				if(mode == 2)
				{
					send_to_char(C_DEFAULT, "Character denied.\n\r", ch);
					write_to_buffer(d, "Please choose a more medieval name.\n\r", 0 );
					sprintf(buf, "%s!%s@%s denied by %s.",
							d->character->name, d->user, d->host, ch->name);
					log_string( buf, CHANNEL_LOG, -1 );
					append_file( ch, AUTH_LIST, buf );
					close_socket(d);
					return;
				}
				else
				{
					send_to_char(C_DEFAULT, "Ok.\n\r", ch);
					sprintf(buf, "%s %s.\n\r", arg2, argument);
					write_to_buffer(d, buf, 0);
					close_socket(d);
					return;
				}
		}
	}
	if(mode)
		send_to_char(C_DEFAULT, "No such unauthorized person.\n\r", ch);
	return;
}

void send_to_area( AREA_DATA *pArea, char *txt )
{
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d; d = d->next )
	{
		if ( d->connected != CON_PLAYING )
			continue;
		if ( d->character )
		{
			if ( !( d->character->in_room ) || ( d->character->deleted )
					|| !IS_OUTSIDE( d->character ) || !IS_AWAKE( d->character ) )
				continue;
			if ( d->character->in_room->area == pArea )
				send_to_char( C_DEFAULT, txt, d->character );
		}
	}
	return;
}

bool check_ban( DESCRIPTOR_DATA *dnew, bool loggedin )
{
	BAN_DATA *pban;
	DESCRIPTOR_DATA *dremove;
	/*  char buf [ MAX_INPUT_LENGTH ]; */
	for ( pban = ban_list; pban; pban = pban->next )
	{
		if ( pban->type == 'N' )
			continue;
		/*
		   sprintf( buf, "pban->user: %s, pban->site: %s\n\rdnew->user: %s, dnew->site: %s\n\r",
		   !pban->user ? "NULL" : pban->user, pban->name, dnew->user, dnew->host );
		   write_to_descriptor( buf, 0, dnew );
		   */
		if ( ( !str_suffix( pban->name, dnew->host ) && !pban->user )

				|| ( pban->user && !str_cmp( pban->user, dnew->user )
					&& !str_suffix( pban->name, dnew->host ) )
		   )
		{
			if ( pban->user && !str_cmp( pban->user, dnew->user ) )
				if ( !str_suffix( pban->name, dnew->host ) )
				{
					write_to_descriptor(
							"You have been banned from Eye of the Storm.\n\r", 0, dnew );
				}
			if ( !str_suffix( pban->name, dnew->host ) &&
					!pban->user )
			{
				write_to_descriptor(
						"Your site has been banned from Eye of the Storm.\n\r", 0, dnew );
			}
			sprintf( log_buf, "%s!%s@%s being denied access.",
					dnew->character ? dnew->character->name : "(unknown)",
					dnew->user,
					dnew->host );
			bug( log_buf, 0 );
			close( dnew->descriptor );
			if ( loggedin )
			{
				for ( dremove = descriptor_list; dremove; dremove = dremove->next )
					if ( dremove->next == dnew ) break;

				dremove->next = dnew->next;
			}
			if ( dnew->character )
			{
				extract_char( dnew->character, TRUE );
			}
			free_string( dnew->host );
			free_string( dnew->user );
			free_string( dnew->showstr_head );
			free_mem( dnew->outbuf, dnew->outsize );
			free_mem( dnew, sizeof( *dnew ) );
			return TRUE;
		}
	}
	return FALSE;
}

bool check_newban( DESCRIPTOR_DATA *dnew, bool loggedin )
{
	BAN_DATA *pban;

	for ( pban = ban_list; pban; pban = pban->next )
	{
		if ( pban->type != 'N' )
			continue;
		if ( ( !str_suffix( pban->name, dnew->host ) && !pban->user )
				|| ( pban->user && !str_cmp( pban->user, dnew->user )
					&& !str_suffix( pban->name, dnew->host ) ) )
		{
			if ( pban->user && !str_cmp( pban->user, dnew->user ) )
				if ( !str_suffix( pban->name, dnew->host ) )
				{
					write_to_descriptor(
							"You have been banned from making new characters on Eye of the Storm.\n\r", 0, dnew );
				}
			if ( !str_suffix( pban->name, dnew->host ) &&
					!pban->user )
			{
				write_to_descriptor(
						"Your site has been banned from making new characters on Eye of the Storm.\n\r", 0, dnew );
			}
			sprintf( log_buf, "%s!%s@%s being denied access.",
					dnew->character ? dnew->character->name : "(unknown)",
					dnew->user,
					dnew->host );
			bug( log_buf, 0 );
			return TRUE;
		}
	}
	return FALSE;
}

void send_to_al( int clr, int level, char *text )
{
	CHAR_DATA *ch;

	for ( ch = char_list; ch; ch = ch->next )
	{
		if ( !ch->desc )
			continue;

		if ( get_trust( ch ) > level )
			send_to_char( clr, text, ch );
	}
	return;
}

void do_desc_check( CHAR_DATA *ch, char *argument )
{
	int desc;
	struct stat bstat;
	char buf[256];

	for ( desc = 0; desc < 100; desc++ )
	{
		if ( fstat(desc, &bstat) < 0 )
			sprintf(buf, "%3d:  no   ", desc);
		else
			sprintf(buf, "%3d: yes   ", desc);
		send_to_char(C_DEFAULT, buf, ch);
		if ( desc % 7 == 6 )
			send_to_char(C_DEFAULT, "\n\r", ch);
	}
	if ( desc % 7 != 6 )
		send_to_char(C_DEFAULT, "\n\r", ch);
	return;
}

void reset_builder_levels ( )
{
	int cmd;

	for ( cmd = 0; cmd_table[cmd].name != '\0'; cmd++ )
	{
		if ( !str_prefix( "redit", cmd_table[cmd].name ) )
			cmd_table[cmd].level = L_JUN;
		else if ( !str_prefix( "medit", cmd_table[cmd].name ) )
			cmd_table[cmd].level = L_DEM;
		else if ( !str_prefix( "oedit", cmd_table[cmd].name ) )
			cmd_table[cmd].level = L_SEN;
		else if ( !str_prefix( "resets", cmd_table[cmd].name ) )
			cmd_table[cmd].level = L_SEN;
		else if ( !str_prefix( "oload", cmd_table[cmd].name ) )
			cmd_table[cmd].level = L_SEN;
	}
	return;
}

/* IMC stuff.. -- Alty */
void imc_setup( void )
{
	struct sockaddr_in sock;

	if ( (imcport = init_socket(IMC_PORT)) != -2 )
		return;
	if ( (imcdesc = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		perror("imc_setup: socket");
		return;
	}
	sock.sin_addr.s_addr = inet_addr("205.134.192.31");
	sock.sin_port = htons(IMC_PORT);
	sock.sin_family = AF_INET;
	if ( connect(imcdesc, (struct sockaddr *)&sock, sizeof(sock)) != 0 )
	{
		perror("imc_setup: connect");
		close(imcdesc);
		imcdesc = -1;
		return;
	}
	return;
}

void imc_accept( void )
{
	struct sockaddr_in sock;
	socklen_t size;
	int desc;

	size = sizeof(sock);
	if ( (desc = accept(imcport, (struct sockaddr *)&sock, &size)) < 0 )
	{
		perror("imc_accept: accept");
		return;
	}
#if !defined( FNDELAY )
#define FNDELAY O_NDELAY
#endif
	if ( fcntl(desc, F_SETFL, FNDELAY) == -1 )
	{
		perror("imc_accept: fnctl: FNDELAY");
		close(desc);
		return;
	}
	if ( imcdesc > 0 )
	{
		close(desc);
		return;
	}
	imcdesc = desc;
	return;
}

void imc_update( fd_set *ins, fd_set *outs, fd_set *excs )
{
	if ( imcport < 0 && imcdesc < 0 )
	{
		static time_t ltime;

		if ( ltime < current_time-(5*60) )
		{
			imc_setup();
			ltime = current_time;
		}
		return;
	}
	if ( imcport > 0 && FD_ISSET(imcport, ins) )
	{
		imc_accept();
		return;
	}
	if ( imcdesc < 0 )
		return;
	if ( FD_ISSET(imcdesc, excs) )
	{
		FD_CLR(imcdesc, ins);
		FD_CLR(imcdesc, outs);
		close(imcdesc);
		imcdesc = -1;
		return;
	}
	if ( FD_ISSET(imcdesc, ins) && !imc_read() )
	{
		FD_CLR(imcdesc, outs);
		close(imcdesc);
		imcdesc = -1;
		return;
	}
	if ( FD_ISSET(imcdesc, outs) && !imc_write(NULL) )
	{
		close(imcdesc);
		imcdesc = -1;
		return;
	}
	return;
}

bool imc_read( void )
{
	static int iStart;
	static char buf[MAX_STRING_LENGTH];

	if ( iStart >= sizeof(buf) - 10 )
	{
		log_string("imc overflow", CHANNEL_GOD, -1);
		iStart = 0;
		return FALSE;
	}
	for ( ; ; )
	{
		int nRead;

		nRead = read(imcdesc, buf+iStart, sizeof(buf)-10-iStart);
		if ( nRead > 0 )
		{
			iStart += nRead;
			if ( buf[iStart-1] == '\n' || buf[iStart-1] == '\r' )
				break;
		}
		else if ( nRead == 0 )
		{
			iStart = 0;
			return FALSE;
		}
		else if ( errno == EWOULDBLOCK || errno == EAGAIN )
			break;
		else
		{
			perror("imc_read");
			iStart = 0;
			return FALSE;
		}
	}
	buf[iStart] = '\0';
	if ( buf[iStart-1] == '\n' || buf[iStart-1] == '\r' )
	{
		imc_channel(buf, iStart);
		iStart = 0;
	}
	return TRUE;
}

bool imc_write( const char *txt )
{
	static char buf[MAX_STRING_LENGTH];
	static int btop;
	int iStart, nWrite, nBlock;

	if ( txt )
	{
		nBlock = strlen(txt);
		imc_channel(txt, nBlock);
		if ( imcdesc < 0 )
			return TRUE;
		if ( btop + nBlock >= sizeof(buf) - 10 )
		{
			log_string("imc output overflow", CHANNEL_GOD, -1);
			btop = 0;
			return FALSE;
		}
		strcpy(buf+btop, txt);
		btop += nBlock;
		return TRUE;
	}
	for ( iStart = 0; iStart < btop; iStart += nWrite )
	{
		nBlock = UMIN(btop-iStart, 4096);
		if ( (nWrite = write(imcdesc, buf+iStart, nBlock)) < 0 )
		{
			perror("imc_write");
			btop = 0;
			return FALSE;
		}
	}
	btop = 0;
	return TRUE;
}

void imc_channel( const char *buf, int len )
{
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d; d = d->next )
	{
		CHAR_DATA *och = d->original ? d->original : d->character;

		if ( d->connected == CON_PLAYING &&
				IS_IMMORTAL(och) &&
				!IS_SET(och->deaf, CHANNEL_IMC) )
			write_to_buffer(d, buf, len);
	}
	return;
}

void do_imc( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];

	sprintf(buf, "%s %d: %s\n\r", ch->name, port, argument);
	if ( IS_SET(ch->deaf, CHANNEL_IMC) )
		REMOVE_BIT(ch->deaf, CHANNEL_IMC);
	if ( !imc_write(buf) )
	{
		close(imcdesc);
		imcdesc = -1;
	}
	return;
}

void do_hotreboo (CHAR_DATA *ch, char * argument)
{
	send_to_char( AT_RED, "If you want to HOTREBOOT spell it out.", ch );
}

void do_hotreboot (CHAR_DATA *ch, char * argument)
{
	FILE *fp;
	DESCRIPTOR_DATA *d, *d_next;
	char buf [100], buf2[100];

	if (!(fp = fopen (HOTREBOOT_FILE, "w")))
	{
		send_to_char ( AT_RED, "Can't write to hotreboot file, aborting.",ch);

		sprintf( buf, "Could not write to hotreboot file: %s", HOTREBOOT_FILE );
		log_string( buf, CHANNEL_GOD, 0 );
		perror ("do_hotreboot:fopen");
		return;
	}

	do_asave (NULL, ""); /* autosave changed areas */
	save_player_list();
	sprintf (buf, "\n\rHot Reboot by %s.\n\r", ch->name);

	/* For each playing descriptor, save its state */
	for (d = descriptor_list; d ; d = d_next)
	{
		CHAR_DATA * och = d->original ? d->original : d->character;
		d_next = d->next; /* We delete from the list , so need to save this */

		if (    !d->character								/* drop those logging on */
				||  (d->connected != CON_PLAYING			/* check if they're playing */
						&&  d->connected != CON_CHATTING)   /* check if they're in the chat room */
		   )
		{
			write_to_descriptor ( "\n\rSorry, we are rebooting.  Try again in a minute.\n\r", 0, d );
			close_socket (d); /* throw'em out */
		}
		else
		{
			fprintf (fp, "%d %s %s %s %d\n", d->descriptor, och->name, d->user, d->host, d->ansi);
			save_char_obj ( och );
			write_to_descriptor (buf, 0, d);
		}
	}

	fprintf (fp, "-1\n");
	fclose (fp);

	/* Close reserve and other always-open files and release other resources */

	fclose (fpReserve);

	/* exec - descriptors are inherited */

	sprintf (buf, "%d", port);
	sprintf (buf2, "%d", control);
	execl (EXE_FILE, "Envy", buf, "hotreboot", buf2, (char *) NULL);

	/* Failed - sucessful exec will not return */

	perror ("do_hotreboot: execl");
	send_to_char ( AT_RED, " Hot Reboot FAILED!\n\r",ch);

	/* reopen fpReserve */
	fpReserve = fopen( NULL_FILE, "r" );
}

/* Recover from a hotreboot - load players */
void hotreboot_recover ( )
{
	DESCRIPTOR_DATA *d;
	FILE *fp;
	char name [100];
	char user [100];
	char host[MAX_STRING_LENGTH];
	int ansi;
	int desc;
	bool fOld;

	log_string ("HotReboot recovery initiated", CHANNEL_GOD, 0);

	if (!(fp = fopen (HOTREBOOT_FILE, "r"))) /* there are some descriptors open which will hang forever then ? */
	{
		perror ("hotreboot_recover:fopen");
		log_string ("HotReboot file not found. Exitting.\n\r", CHANNEL_GOD, 0);
		exit (1);
	}

	unlink (HOTREBOOT_FILE); /* In case something crashes - doesn't prevent reading	*/

	for (;;)
	{
		if ( !fscanf (fp, "%d %s %s %s %d\n", &desc, name, user, host, &ansi) ) {
			/* do nothing */
		}

		if (desc == -1) {
			break;
		}

		d = alloc_perm (sizeof(DESCRIPTOR_DATA));
		init_descriptor (d,desc); /* set up various stuff */

		/* Write something, and check if it goes error-free */
		if (!write_to_descriptor ( "\n\rRecovering from hotreboot.\n\r",0, d)) {
			close(d->descriptor); /* nope */
			continue;
		}

		d->user = str_dup (user);
		d->host = str_dup (host);
		d->ansi = ansi;
		d->next = descriptor_list;
		descriptor_list = d;
		d->connected = CON_HOTREBOOT_RECOVER; /* -15, so close_socket frees the char */

		/* Now, find the pfile */

		fOld = load_char_obj (d, name);

		if (!fOld) /* Player file not found?! */
		{
			write_to_descriptor ("\n\rUnable to find your character.  Sorry.\n\r", 0, d);
			close_socket (d);
		}
		else /* ok! */
		{
			write_to_descriptor ( "\n\rHotReboot complete.\n\r",0, d);

			/* Just In Case */
			if (!d->character->in_room)
				d->character->in_room = get_room_index (ROOM_VNUM_TEMPLE);

			/* Insert in the char_list */
			d->character->next = char_list;
			char_list = d->character;

			char_to_room (d->character, d->character->in_room);
			do_look (d->character, "");
			act ( AT_WHITE, "$n materializes!", d->character, NULL, NULL, TO_ROOM);
			d->connected = CON_PLAYING;
		}

	}


}
