#include "reg.h"

static float reg[10];


void reg_set(int pos, float val)
{
    reg[pos] = val;
}

float reg_get(int pos)
{
    return reg[pos];
}
