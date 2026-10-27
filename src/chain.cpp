// The chains, from 0x41c720.

#include "chain.h"

void Chain::Release()
{
    this->ReleaseList(&this->calcChain);
    this->ReleaseList(&this->drawChain);
}
