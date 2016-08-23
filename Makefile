# config
CC             = clang
PKG_CFG        = `pkg-config --cflags --libs jansson libcurl`

# directories
SRC_DIR        = src
BUILD_DIR      = $(SRC_DIR)/build
TEST_DIR       = tests
TEST_BUILD_DIR = $(TEST_DIR)/build
BIN_DIR        = bin

# envy config
C_FLAGS        = -c -O -Wall -g3 -DNOCRYPT
L_FLAGS        = -g
OBJ_NAMES      = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o comm.o const.o \
                 db.o fight.o handler.o magic.o save.o special.o update.o remort.o   \
                 bit.o mem.o olc.o olc_act.o olc_save.o string.o mob_commands.o      \
                 mob_prog.o gr_magic.o id.o chat_new.o interp.o war.o random_obj.o   \
                 chat_act.o ore_prog.o quest.o raceskill.o act_room.o track.o scan.o \
                 devops.o
OBJS           = $(addprefix $(BUILD_DIR)/, $(OBJ_NAMES))
MAIN_OBJ       = src/build/main.o

# test config
TEST_C_FLAGS   = $(C_FLAGS) -fprofile-arcs -ftest-coverage
TEST_L_FLAGS   = $(L_FLAGS) -lgcov -coverage
TEST_OBJ_NAMES = main.o mocks.o test.act_info.o test.act_wiz.o test.bit.o test.mem.o
TEST_OBJS      = $(addprefix $(TEST_BUILD_DIR)/, $(TEST_OBJ_NAMES))

.PHONY: test clean

default: $(MAIN_OBJ) $(OBJS)
	rm -f $(BIN_DIR)/envy
	$(CC) $(L_FLAGS) -o $(BIN_DIR)/envy $(MAIN_OBJ) $(OBJS) $(PKG_CFG)

test: $(TEST_OBJS)
	rm -rf $(BIN_DIR)/test.envy
	$(CC) $(TEST_L_FLAGS) -o $(BIN_DIR)/test.envy $(OBJS) $(TEST_OBJS) $(PKG_CFG) -lcunit

clean:
	rm -f $(BUILD_DIR)/*.o
	rm -f $(TEST_BUILD_DIR)/*.o
	rm -f $(BIN_DIR)/envy
	rm -f $(BIN_DIR)/test.envy

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(C_FLAGS) $< -o $@

$(TEST_BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(TEST_C_FLAGS) $< -o $@
