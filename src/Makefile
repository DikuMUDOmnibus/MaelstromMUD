CC      			= clang
C_FLAGS 			= -c -O -Wall -g3 -DNOCRYPT
L_FLAGS 			= -g
PKG_CFG 			= `pkg-config --cflags --libs jansson libcurl`
SYSTEM  			=

OBJS 					= act_comm.o act_info.o act_move.o act_obj.o act_wiz.o comm.o const.o \
        			  db.o fight.o handler.o magic.o save.o special.o update.o remort.o \
        			  bit.o mem.o olc.o olc_act.o olc_save.o string.o mob_commands.o \
        			  mob_prog.o gr_magic.o id.o chat_new.o interp.o war.o random_obj.o \
	  						chat_act.o ore_prog.o quest.o raceskill.o act_room.o track.o scan.o \
	  						devops.o

MAIN_OBJ  		= main.o

TEST_OBJS 		= tests/test.act_info.o
TEST_MAIN_OBJ = tests/test.main.o

envy: $(MAIN_OBJ) $(OBJS)
	@echo "[1;35mBuilding the [1;32mOASIS[0;37m"
	rm -f envy
	$(CC) $(L_FLAGS) -o envy $(MAIN_OBJ) $(OBJS) $(PKG_CFG)
	@echo "Done."

test: $(TEST_MAIN_OBJ) $(OBJS) $(TEST_OBJS)
	@echo "[1;35mBuilding the [1;32mTests[0;37m"
	rm -rf test
	$(CC) $(L_FLAGS) -o test $(OBJS) $(TEST_MAIN_OBJ) $(TEST_OBJS) $(PKG_CFG) -lcunit
	@echo "Done."

clean:
	rm -f *.o
	rm -f tests/*.o
	rm -f envy
	rm -f test
	make
	make test

%.o: %.c
	$(CC) $(C_FLAGS) $(SYSTEM) $< -o $@

tests/%.o: tests/%.c
	$(CC) $(C_FLAGS) $(SYSTEM) $< -o $@
