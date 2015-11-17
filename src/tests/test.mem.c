#include "test.h"

DO_TEST(new_reset_data) {
  RESET_DATA *testReset = new_reset_data();

  CU_ASSERT_EQUAL(sizeof(*testReset), 20);
}

DO_TEST(free_reset_data) {
  // RESET_DATA *testReset = new_reset_data();
}

DO_TEST(new_area) {
  // AREA_DATA *testArea = new_area();
  //
  // CU_ASSERT_EQUAL(sizeof(*testReset)), 20);
}

DO_TEST(free_area) {
  // AREA_DATA *testArea = new_area();
}

DO_TEST(new_exit) {
  // EXIT_DATA *testExit = new_exit();
}

DO_TEST(free_exit) {
  // EXIT_DATA *testExit = new_exit();
}

DO_TEST(free_trap_data) {

}

DO_TEST(new_extra_descr) {

}

DO_TEST(free_extra_descr) {

}

DO_TEST(new_social_index) {

}

DO_TEST(free_social_index) {

}

DO_TEST(new_room_index) {

}

DO_TEST(free_room_index) {

}

DO_TEST(new_newbie_index) {

}

DO_TEST(new_clan_index) {

}

DO_TEST(new_affect) {

}

DO_TEST(free_affect) {

}

DO_TEST(new_alias) {

}

DO_TEST(free_alias) {

}

DO_TEST(new_shop) {

}

DO_TEST(free_shop) {

}

DO_TEST(free_obj_index) {

}

DO_TEST(new_mob_index) {

}

DO_TEST(free_mob_index) {

}

DO_TEST(new_mprog_data) {

}

DO_TEST(free_mprog_data) {

}

DO_TEST(new_race_data) {

}

DO_TEST(free_race_data) {

}

DO_TEST_SUITE(mem) = {
  TEST(new_reset_data),
  CU_TEST_INFO_NULL,
};
