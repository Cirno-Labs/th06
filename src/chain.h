// The chains that run the game: every frame, one runs the update
// callbacks of its elements and the other their draw callbacks, in
// order of priority ("add calc chain (pri = %d)").

#pragma once

#include "common.h"

// Chain callbacks are cdecl and take the element's `arg`. They return a
// status; 1 keeps the element and moves on to the next.
typedef int (*ChainCallback)(void *arg);

// An element of a chain. Allocated with new by Chain::CreateElem.
struct ChainElem
{
    // Sets the callback and clears the added and deleted callbacks.
    void SetCallback(ChainCallback callback)
    {
        this->callback = callback;
        this->addedCallback = NULL;
        this->deletedCallback = NULL;
    }

    // Lower runs first.
    short priority;
    // Bit 0 is set by Chain::CreateElem.
    unsigned short flags;
    ChainCallback callback;
    // Called when the element is added to a chain; adding it to the
    // update chain clears it after the call.
    ChainCallback addedCallback;
    ChainCallback deletedCallback;
    ChainElem *prev;
    ChainElem *next;
    unsigned char unk_18[0x4];
    void *arg;
};
ASSERT_SIZE(ChainElem, 0x20);

// The global at 0x69d918: the heads of the update and draw chains.
class Chain
{
public:
    // Allocates an element that runs `callback`.
    ChainElem *CreateElem(ChainCallback callback);
    // Inserts `elem` into the update chain by priority and calls its
    // added callback. Returns that callback's result, or 0 without one.
    int AddToCalcChain(ChainElem *elem, int priority);
    // Like AddToCalcChain, for the draw chain.
    int AddToDrawChain(ChainElem *elem, int priority);
    // Unlinks `elem` from its chain and deletes it.
    void Remove(ChainElem *elem);
    // Releases the elements of the list that starts at `root`, going
    // through a copy of the list it allocates.
    void ReleaseList(ChainElem *root);
    // Releases both chains.
    void Release();

    ChainElem calcChain;
    ChainElem drawChain;
};
extern Chain g_Chain;
