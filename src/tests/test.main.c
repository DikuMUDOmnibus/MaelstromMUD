#include "test.h"

/* Unit Tests */
CU_TestInfo test_act_info[] = {
  { "add_money", test_add_money },
  { "sub_money", test_sub_money },
  { "spend_money", test_spend_money },
  { "money_string", test_money_string },
  CU_TEST_INFO_NULL,
};

/* Test Suites */
CU_SuiteInfo suites[] = {
  { "act_info", NULL, NULL, test_act_info },
  CU_SUITE_INFO_NULL,
};

int main ( void ) {
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
