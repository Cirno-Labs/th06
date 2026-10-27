// cflags: /Os -/Oi
// The /Os file from 0x42ab30 to 0x42f6c9, with the result screen: the
// replay writer, the score file and its sorted lists.

#include "resultscreen.h"
#include "unsorted.h"

void FreeListNodes(ListNode *head)
{
    ListNode *next;

    head = head->next;
    while (head != NULL)
    {
        next = head->next;
        free(head);
        head = next;
    }
}
