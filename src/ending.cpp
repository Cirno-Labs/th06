// The ending's file, 0x40f4d0 to 0x410e2c.

#include <stdlib.h>
#include <string.h>

#include "ending.h"
#include "gamemanager.h"
#include "unsorted.h"

int Ending::ReadInt()
{
    int value;

    value = atoi(this->cursor);
    while (*this->cursor != '\0')
    {
        this->cursor++;
    }
    while (*this->cursor == '\0')
    {
        this->cursor++;
    }
    return value;
}
