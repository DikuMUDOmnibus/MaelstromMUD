#include <stdio.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <CUnit/Automated.h>
#include <CUnit/Console.h>
#include <CUnit/TestDB.h>
#include "../merc.h"

/* macros */
#define TEST( test_name ) void test_##test_name(void)

/* globals */
CHAR_DATA       *mock_supermob;
OBJ_DATA        *mock_dummy;
ROOM_INDEX_DATA *mock_limbo;

/* da main */
int main ( void );
int initialize_suite(void);

/* mock data */
void setup_mocks(void);
void reset_mocks(void);

/* test.act_info.c */
TEST(check_blind);
TEST(add_money);
TEST(sub_money);
TEST(spend_money);
TEST(money_string);

/* test.act_wiz.c */
TEST(do_doubleexp);

/* test.bit.c */
TEST(is_stat);
TEST(flag_lookup);
TEST(flag_value);
TEST(flag_string);

/* test.mem.c */
TEST(new_reset_data);
TEST(free_reset_data);
TEST(new_area);
TEST(free_area);
TEST(new_exit);
TEST(free_trap_data);
TEST(free_exit);
TEST(new_extra_descr);
TEST(free_extra_descr);
TEST(free_social_index);
TEST(new_room_index);
TEST(new_social_index);
TEST(new_newbie_index);
TEST(new_clan_index);
TEST(free_room_index);
TEST(new_affect);
TEST(new_alias);
TEST(free_alias);
TEST(free_affect);
TEST(new_shop);
TEST(free_shop);
TEST(free_obj_index);
TEST(new_mob_index);
TEST(free_mprog_data);
TEST(free_mob_index);
TEST(new_mprog_data);
TEST(new_race_data);
TEST(free_race_data);
