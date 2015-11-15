#include "test.h"

void test_is_stat(void) {
  const struct flag_type mock_flags[] = {
    { "none", 0, FALSE },
  };

  /* sex flags are stat flags */
  CU_ASSERT_TRUE(is_stat(sex_flags));

  /* area flags are not stat flags */
  CU_ASSERT_FALSE(is_stat(area_flags));

  /* these flags don't even exist in the flag stat table */
  CU_ASSERT_FALSE(is_stat(mock_flags));
}

void test_flag_lookup(void) {
  /* existent settable flag */
  CU_ASSERT_EQUAL(flag_lookup("locked", exit_flags), EX_LOCKED);

  /* existent unsettable flag */
  CU_ASSERT_EQUAL(flag_lookup("bashed", exit_flags), NO_FLAG);

  /* non-existent flag */
  CU_ASSERT_EQUAL(flag_lookup("potato", exit_flags), NO_FLAG);
}

void test_flag_value(void) {
  /* existent settable flag */
  CU_ASSERT_EQUAL(flag_value(exit_flags, "locked"), EX_LOCKED);

  /* multiple existent settable flags */
  CU_ASSERT_EQUAL(flag_value(exit_flags, "closed locked"), EX_CLOSED | EX_LOCKED);

  /* existent unsettable flag */
  CU_ASSERT_EQUAL(flag_value(exit_flags, "bashed"), NO_FLAG);

  /* existent settable and unsettable flags */
  CU_ASSERT_EQUAL(flag_value(exit_flags, "closed bashed"), EX_CLOSED);

  /* nonexistent flag */
  CU_ASSERT_EQUAL(flag_value(exit_flags, "potato"), NO_FLAG);
  
  /* existent settable and nonexistent flags */
  CU_ASSERT_EQUAL(flag_value(exit_flags, "closed potato"), EX_CLOSED);
  
  /* existent settable, unsettable, and nonexistent flags */
  CU_ASSERT_EQUAL(flag_value(exit_flags, "closed bashed potato"), EX_CLOSED);
}

void test_flag_string(void) {
  /* existent settable bit */
  CU_ASSERT_STRING_EQUAL(flag_string( exit_flags, EX_LOCKED ), "locked");

  /* multiple existent settable bits */
  CU_ASSERT_STRING_EQUAL(flag_string( exit_flags, EX_CLOSED | EX_LOCKED ), "closed locked");

  /* existent unsettable bit */
  CU_ASSERT_STRING_EQUAL(flag_string( exit_flags, EX_BASHED ), "bashed");

  /* existent settable and unsettable bits */
  CU_ASSERT_STRING_EQUAL(flag_string( exit_flags, EX_CLOSED | EX_BASHED ), "closed bashed");

  /* nonexistent bit */
  CU_ASSERT_STRING_EQUAL(flag_string( exit_flags, 0 ), "none");

  /* existent settable and nonexistant bits */
  CU_ASSERT_STRING_EQUAL(flag_string( exit_flags, EX_CLOSED | 0 ), "closed");

  /* existent settable, unsettable, and nonexistent bits */
  CU_ASSERT_STRING_EQUAL(flag_string( exit_flags, EX_CLOSED | EX_BASHED | 0 ), "closed bashed");
}