#include "test.h"

void test_do_doubleexp(void) {
  do_doubleexp(mock_supermob, "on");
  CU_ASSERT_TRUE(doubleexp());

  do_doubleexp(mock_supermob, "off");
  CU_ASSERT_FALSE(doubleexp());

  do_doubleexp(mock_supermob, "potato");
  CU_ASSERT_FALSE(doubleexp());
}