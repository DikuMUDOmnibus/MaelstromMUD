#include "test.h"

DO_TEST(new_reset_data) {
  RESET_DATA *testReset = new_reset_data();

  CU_ASSERT_EQUAL(sizeof(*testReset), 20);
}

DO_TEST(new_area) {
  AREA_DATA *testArea = new_area();

  CU_ASSERT_EQUAL(sizeof(*testArea), 68);
}

DO_TEST(new_exit) {
  EXIT_DATA *testExit = new_exit();

  CU_ASSERT_EQUAL(sizeof(*testExit), 40);
}

DO_TEST(new_extra_descr) {
  EXTRA_DESCR_DATA *testExtra;

  CU_ASSERT_EQUAL(sizeof(*testExtra), 16);
}

DO_TEST(new_social_index) {
  SOCIAL_DATA *testSocial;

  CU_ASSERT_EQUAL(sizeof(*testSocial), 36);
}

DO_TEST(new_room_index) {
  ROOM_INDEX_DATA *testRoom;

  CU_ASSERT_EQUAL(sizeof(*testRoom), 92);
}

DO_TEST(new_newbie_index) {
  NEWBIE_DATA *testNewbie;

  CU_ASSERT_EQUAL(sizeof(*testNewbie), 16);
}

DO_TEST(new_clan_index) {
  CLAN_DATA *testClan;

  CU_ASSERT_EQUAL(sizeof(*testClan), 92);
}

DO_TEST(new_affect) {
  AFFECT_DATA *testAffect;

  CU_ASSERT_EQUAL(sizeof(*testAffect), 32);
}

DO_TEST(new_shop) {
  SHOP_DATA *testShop;

  CU_ASSERT_EQUAL(sizeof(*testShop), 44);
}

DO_TEST(new_mob_index) {
  MOB_INDEX_DATA *testMob;

  CU_ASSERT_EQUAL(sizeof(*testMob), 132);
}

DO_TEST(new_mprog_data) {
  MPROG_DATA *testMobProg;

  CU_ASSERT_EQUAL(sizeof(*testMobProg), 16);
}

DO_TEST_SUITE(mem) = {
  TEST(new_reset_data),
  TEST(new_area),
  TEST(new_exit),
  TEST(new_extra_descr),
  TEST(new_social_index),
  TEST(new_room_index),
  TEST(new_newbie_index),
  TEST(new_clan_index),
  TEST(new_affect),
  TEST(new_shop),
  TEST(new_mob_index),
  TEST(new_mprog_data),
  CU_TEST_INFO_NULL,
};
