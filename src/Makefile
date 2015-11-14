CC      = clang
C_FLAGS = -c -O -Wall -g3 -DNOCRYPT
L_FLAGS = -g
PKG_CFG = `pkg-config --cflags --libs jansson libcurl`
SYSTEM  =

O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o comm.o const.o \
          db.o fight.o handler.o magic.o save.o special.o update.o remort.o \
          bit.o mem.o olc.o olc_act.o olc_save.o string.o mob_commands.o \
          mob_prog.o gr_magic.o id.o chat_new.o interp.o war.o random_obj.o \
	  			chat_act.o ore_prog.o quest.o raceskill.o act_room.o track.o scan.o \
	  			devops.o

envy: $(O_FILES)
	@echo "[1;35mBuilding the [1;32mOASIS[0;37m"
	rm -f envy
	$(CC) $(L_FLAGS) -o envy $(O_FILES) $(PKG_CFG)
	@echo "Done."

clean:
	rm -f *.o
	rm -f envy
	make

.c.o: merc.h
	$(CC) $(C_FLAGS) $(SYSTEM) $<

check: