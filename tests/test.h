#include <stdio.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <CUnit/Automated.h>
#include <CUnit/Console.h>
#include <CUnit/TestDB.h>
#include "../src/merc.h"

/* macros */
#define STRINGIFY(s) #s

#define DO_TEST( test_name ) void test_##test_name(void)
#define DO_TEST_SUITE( suite_name ) CU_TestInfo test_##suite_name[]

#define TEST( test_name ) {STRINGIFY(test_name), test_##test_name}
#define TEST_SUITE( suite_name ) { STRINGIFY(suite_name), initialize_suite, NULL, test_##suite_name }

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

/* test suites */
extern DO_TEST_SUITE(act_info);
extern DO_TEST_SUITE(act_wiz);
extern DO_TEST_SUITE(bit);
extern DO_TEST_SUITE(mem);
