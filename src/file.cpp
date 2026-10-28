// cflags: -/Od -/Op /O2 /Ob1
// The file class, 0x43ce40 to 0x43d158.

#include "archive.h"

File::File()
{
    this->handle = INVALID_HANDLE_VALUE;
    this->access = 0;
}

int File::Open(char *path, char *mode)
{
    BOOL append = FALSE;
    DWORD disposition;
    char *p;

    this->Close();
    for (p = mode; *p != '\0'; p++)
    {
        if (*p == 'r')
        {
            this->access = GENERIC_READ;
            disposition = OPEN_EXISTING;
            break;
        }
        if (*p == 'w')
        {
            DeleteFileA(path);
            this->access = GENERIC_WRITE;
            disposition = OPEN_ALWAYS;
            break;
        }
        if (*p == 'a')
        {
            append = TRUE;
            this->access = GENERIC_WRITE;
            disposition = OPEN_ALWAYS;
            break;
        }
    }
    if (*p == '\0')
    {
        return 0;
    }
    this->handle = CreateFileA(path, this->access, FILE_SHARE_READ, NULL, disposition,
                               FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (this->handle == INVALID_HANDLE_VALUE)
    {
        return 0;
    }
    if (append)
    {
        SetFilePointer(this->handle, 0, NULL, FILE_END);
    }
    return 1;
}
