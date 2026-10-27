// The chains, from 0x41c6d0.

#include "chain.h"
#include "unsorted.h"

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

int Chain::AddToCalcChain(ChainElem *elem, int priority)
{
    ChainElem *cur = &this->calcChain;

    DebugPrint("add calc chain (pri = %d)\n", priority);
    elem->priority = priority;
    while (cur->next != NULL)
    {
        if (cur->priority > priority)
        {
            break;
        }
        cur = cur->next;
    }
    if (cur->priority > priority)
    {
        elem->next = cur;
        elem->prev = cur->prev;
        if (elem->prev != NULL)
        {
            elem->prev->next = elem;
        }
        cur->prev = elem;
    }
    else
    {
        elem->next = NULL;
        elem->prev = cur;
        cur->next = elem;
    }
    if (elem->addedCallback != NULL)
    {
        int result = elem->addedCallback(elem->arg);
        elem->addedCallback = NULL;
        return result;
    }
    return 0;
}

int Chain::AddToDrawChain(ChainElem *elem, int priority)
{
    ChainElem *cur = &this->drawChain;

    DebugPrint("add draw chain (pri = %d)\n", priority);
    elem->priority = priority;
    while (cur->next != NULL)
    {
        if (cur->priority > priority)
        {
            break;
        }
        cur = cur->next;
    }
    if (cur->priority > priority)
    {
        elem->next = cur;
        elem->prev = cur->prev;
        if (elem->prev != NULL)
        {
            elem->prev->next = elem;
        }
        cur->prev = elem;
    }
    else
    {
        elem->next = NULL;
        elem->prev = cur;
        cur->next = elem;
    }
    if (elem->addedCallback != NULL)
    {
        return elem->addedCallback(elem->arg);
    }
    return 0;
}

int Chain::RunCalcChain()
{
    ChainElem *current;
    int updateCount;
    ChainElem *tmp;

restart:
    updateCount = 0;
    current = &this->calcChain;
    while (current != NULL)
    {
        if (current->callback != NULL)
        {
        again:
            switch (current->callback(current->arg))
            {
            case 0:
                tmp = current;
                current = current->next;
                this->Remove(tmp);
                updateCount++;
                continue;
            case 2:
                goto again;
            case 4:
                return 0;
            case 3:
                return 1;
            case 5:
                return -1;
            case 6:
                goto restart;
            default:
                updateCount++;
                break;
            }
        }
        current = current->next;
    }
    return updateCount;
}

int Chain::RunDrawChain()
{
    ChainElem *current;
    int updateCount;
    ChainElem *tmp;

    updateCount = 0;
    current = &this->drawChain;
    while (current != NULL)
    {
        if (current->callback != NULL)
        {
        again:
            switch (current->callback(current->arg))
            {
            case 0:
                tmp = current;
                current = current->next;
                this->Remove(tmp);
                updateCount++;
                continue;
            case 2:
                goto again;
            case 4:
                return 0;
            case 3:
                return 1;
            case 5:
                return -1;
            default:
                updateCount++;
                break;
            }
        }
        current = current->next;
    }
    return updateCount;
}

void Chain::Release()
{
    this->ReleaseList(&this->calcChain);
    this->ReleaseList(&this->drawChain);
}

ChainElem *Chain::CreateElem(ChainCallback callback)
{
    ChainElem *elem = new ChainElem();
    elem->SetCallback(callback);
    elem->allocated = 1;
    return elem;
}
