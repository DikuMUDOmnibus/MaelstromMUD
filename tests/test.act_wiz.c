#include "test.h"

DO_TEST(do_doubleexp) {
  do_doubleexp(mock_supermob, "on");
  CU_ASSERT_TRUE(doubleexp());

  do_doubleexp(mock_supermob, "off");
  CU_ASSERT_FALSE(doubleexp());

  do_doubleexp(mock_supermob, "potato");
  CU_ASSERT_FALSE(doubleexp());
}

DO_TEST_SUITE(act_wiz) = {
  TEST(do_doubleexp),
  CU_TEST_INFO_NULL,
};
