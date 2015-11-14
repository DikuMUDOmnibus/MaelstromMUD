CC      			= clang
C_FLAGS 			= -c -O -Wall -g3 -DNOCRYPT
L_FLAGS 			= -g
PKG_CFG 			= `pkg-config --cflags --libs jansson libcurl`

OBJS 					= act_comm.o act_info.o act_move.o act_obj.o act_wiz.o comm.o const.o \
        			  db.o fight.o handler.o magic.o save.o special.o update.o remort.o \
        			  bit.o mem.o olc.o olc_act.o olc_save.o string.o mob_commands.o \
        			  mob_prog.o gr_magic.o id.o chat_new.o interp.o war.o random_obj.o \
	  						chat_act.o ore_prog.o quest.o raceskill.o act_room.o track.o scan.o \
	  						devops.o

MAIN_OBJ  		= main.o

.PHONY: envy test

envy:
	make clean -f Makefile.envy

test:
	make clean -f Makefile.test
