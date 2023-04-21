#include "stdint.h"

static volatile uint32_t Interator_u32=0U;

int main()
{

    while(1)
    {
        Interator_u32++;
    }
}