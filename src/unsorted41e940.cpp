// cflags: /Os -/Oi
// The file starting at 0x41e940. It is built with /Os: functions end with
// `leave` instead of `mov esp, ebp; pop ebp`.

#include "unsorted.h"

Unknown41e981::~Unknown41e981()
{
    this->Func41e992();
}
