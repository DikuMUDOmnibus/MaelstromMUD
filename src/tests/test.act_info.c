#include "test.h"

TEST(check_blind) {
  CU_ASSERT_TRUE(check_blind(mock_supermob));

  SET_BIT(mock_supermob->affected_by, AFF_BLIND);

  CU_ASSERT_FALSE(check_blind(mock_supermob));

  reset_mocks();
}

TEST(add_money) {
  MONEY_DATA a;
  MONEY_DATA b;

  a.gold   = 13;
  a.silver = 7;
  a.copper = 23;

  b.gold   = 18;
  b.silver = 73;
  b.copper = 39;

  add_money(&a, &b);

  CU_ASSERT_EQUAL(a.gold, 31);
  CU_ASSERT_EQUAL(a.silver, 80);
  CU_ASSERT_EQUAL(a.copper, 62);
}

TEST(sub_money) {
  MONEY_DATA a;
  MONEY_DATA b;

  a.gold   = 18;
  a.silver = 73;
  a.copper = 39;

  b.gold   = 13;
  b.silver = 7;
  b.copper = 23;

  sub_money(&a, &b);

  CU_ASSERT_EQUAL(a.gold, 5);
  CU_ASSERT_EQUAL(a.silver, 66);
  CU_ASSERT_EQUAL(a.copper, 16);
}

TEST(spend_money) {
  MONEY_DATA a;
  MONEY_DATA b;

  a.gold   = 100;
  a.silver = 50;
  a.copper = 25;

  b.gold   = 10;
  b.silver = 100;
  b.copper = 100;

  spend_money(&a, &b);

  CU_ASSERT_EQUAL(a.gold, 85);
  CU_ASSERT_EQUAL(a.silver, 0);
  CU_ASSERT_EQUAL(a.copper, 0);
}

TEST(money_string) {
  MONEY_DATA a;
  char *str;

  a.gold   = 100;
  a.silver = 0;
  a.copper = 0;

  str = money_string(&a);

  CU_ASSERT_STRING_EQUAL(str, "100 gold coins.");

  a.gold   = 100;
  a.silver = 50;
  a.copper = 25;

  str = money_string(&a);

  CU_ASSERT_STRING_EQUAL(str, "100 gold, 50 silver and 25 copper coins.");

  a.gold   = 100;
  a.silver = 0;
  a.copper = 25;

  str = money_string(&a);

  CU_ASSERT_STRING_EQUAL(str, "100 gold and 25 copper coins.");

  a.gold   = 100;
  a.silver = 50;
  a.copper = 0;

  str = money_string(&a);

  CU_ASSERT_STRING_EQUAL(str, "100 gold and 50 silver coins.");

  a.gold   = 0;
  a.silver = 50;
  a.copper = 0;

  str = money_string(&a);

  CU_ASSERT_STRING_EQUAL(str, "50 silver coins.");

  a.gold   = 0;
  a.silver = 50;
  a.copper = 25;

  str = money_string(&a);

  CU_ASSERT_STRING_EQUAL(str, "50 silver and 25 copper coins.");

  a.gold   = 0;
  a.silver = 0;
  a.copper = 25;

  str = money_string(&a);

  CU_ASSERT_STRING_EQUAL(str, "25 copper coins.");

  a.gold   = 0;
  a.silver = 0;
  a.copper = 0;

  str = money_string(&a);

  CU_ASSERT_STRING_EQUAL(str, "0 coins.");
}
