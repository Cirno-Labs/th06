// The file with Stage, from 0x403810.

#include "stage.h"

void Stage::RemoveChain()
{
    g_Chain.Remove(&g_StageCalcChain);
    g_Chain.Remove(&g_StageDrawChain);
    g_Chain.Remove(&g_StageDrawChain2);
}
