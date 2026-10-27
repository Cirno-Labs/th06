// The chains, from 0x41c6d0.

#include "chain.h"

Chain::~Chain()
{
}

ChainElem::ChainElem()
{
    this->prev = NULL;
    this->next = NULL;
    this->callback = NULL;
    this->self = this;
    this->addedCallback = NULL;
    this->deletedCallback = NULL;
    this->priority = 0;
    this->allocated = 0;
}

ChainElem::~ChainElem()
{
    if (this->deletedCallback != NULL)
    {
        this->deletedCallback(this->arg);
    }
    this->prev = NULL;
    this->next = NULL;
    this->callback = NULL;
    this->addedCallback = NULL;
    this->deletedCallback = NULL;
}

Chain::Chain()
{
    this->midiOutDevs = midiOutGetNumDevs();
    this->unk_44 = 0;
}

void Chain::Release()
{
    this->ReleaseList(&this->calcChain);
    this->ReleaseList(&this->drawChain);
}
