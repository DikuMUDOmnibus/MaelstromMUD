#include "test.h"

void setup_mocks(void) {
  mock_supermob = create_mobile(get_mob_index(MOB_VNUM_SUPERMOB));
}

/* alias the setup function, mostly for readability */
void reset_mocks(void) {
  setup_mocks();
}