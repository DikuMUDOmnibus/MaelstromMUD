#include "test.h"

/* Unit Tests */
CU_TestInfo test_act_info[] = {
  { "check_blind", test_check_blind },
  { "add_money", test_add_money },
  { "sub_money", test_sub_money },
  { "spend_money", test_spend_money },
  { "money_string", test_money_string },
  CU_TEST_INFO_NULL,
};

CU_TestInfo test_act_wiz[] = {
  { "do_doubleexp", test_do_doubleexp },
  CU_TEST_INFO_NULL,
};

CU_TestInfo test_bit[] = {
  { "is_stat", test_is_stat },
  { "flag_lookup", test_flag_lookup },
  { "flag_value", test_flag_value },
  { "flag_string", test_flag_string },
  CU_TEST_INFO_NULL,
};

/* Test Suites */
CU_SuiteInfo suites[] = {
  { "act_info", initialize_suite, NULL, test_act_info },
  { "act_wiz", initialize_suite, NULL, test_act_wiz },
  { "bit", initialize_suite, NULL, test_bit },
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