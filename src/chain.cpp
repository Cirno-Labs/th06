// The chains, from 0x41c6d0.

#include "chain.h"

Chain::~Chain()
{
}

void Chain::Release()
{
    this->ReleaseList(&this->calcChain);
    this->ReleaseList(&this->drawChain);
}
