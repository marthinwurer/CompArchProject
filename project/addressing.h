//
// Created by benjamin on 2/24/18.
//

#ifndef P1_ADDRESSING_H
#define P1_ADDRESSING_H

#include "globals.h"

void check_addressing();
bool calc_addressing(ulong am, ulong reg, struct am_data & data);

void load(struct am_data & am, StorageObject & dest);
void writeback(struct am_data & am);


#endif //P1_ADDRESSING_H
