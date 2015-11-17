#include "test.h"

/* Test Suites */
CU_SuiteInfo suites[] = {
  TEST_SUITE(act_info),
  TEST_SUITE(act_wiz),
  TEST_SUITE(bit),
  TEST_SUITE(mem),
  CU_SUITE_INFO_NULL,
};

int main ( void ) {
  boot_db();

  /* initialize the CUnit test registry */
  if ( CUE_SUCCESS != CU_initialize_registry() ) {
    return CU_get_error();
  }

  CU_ErrorCode error = CU_register_suites(suites);

  if ( error != CUE_SUCCESS ) {
    return error;
  }

  // Run all tests using the basic interface
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();

  printf("\n");

  CU_basic_show_failures(CU_get_failure_list());

  printf("\n");

  /* Clean up registry and return */
  CU_cleanup_registry();

  return CU_get_error();
}

int initialize_suite(void) {
  setup_mocks();
  return 0;
}
