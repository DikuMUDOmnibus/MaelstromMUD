#include "test.h"

void setup_mocks(void) {
  mock_supermob = create_mobile(get_mob_index(MOB_VNUM_SUPERMOB));
  mock_dummy = create_object(get_obj_index(OBJ_VNUM_DUMMY), 0);
  mock_limbo = get_room_index(ROOM_VNUM_LIMBO);
}

/* alias the setup function, mostly for readability */
void reset_mocks(void) {
  setup_mocks();
}