#include "moti/moti.h"


int main() {
    MOTI_TRACE("sdffsdsdf %d", 44);
    MOTI_ASSERT(1 == 2, "1 != %d", 2);
    return 0;
}