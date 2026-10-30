// cflags: -/Od -/Op /O2 /Ob1
// The file class, 0x43ce40 to 0x43d158.

#include "archive.h"

File::File()
{
    this->handle = INVALID_HANDLE_VALUE;
    this->access = 0;
}

#ifdef TH06_EXE
// Unused: the original's object also mentioned five more KERNEL32
// functions before File::Open, from code /OPT:REF dropped. The Rich
// header counts their import objects and the IAT order needs five more
// KERNEL32 entries in the linker's import sort (the dropped ones are
// still sorted). Which five is unknown; these file helpers stand in.
BOOL UnusedReplaceFile(char *path, char *backup, char *tempDir)
{
    if (GetTempPathA(MAX_PATH, tempDir) == 0)
    {
        return FALSE;
    }
    if (GetFileAttributesA(path) == INVALID_FILE_ATTRIBUTES)
    {
        return FALSE;
    }
    SetFileAttributesA(path, FILE_ATTRIBUTE_NORMAL);
    if (!CopyFileA(path, backup, FALSE))
    {
        return FALSE;
    }
    return MoveFileA(backup, path);
}
#endif // TH06_EXE

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

void File::Close()
{
    if (this->handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(this->handle);
        this->handle = INVALID_HANDLE_VALUE;
        this->access = 0;
    }
}

int File::Read(void *buffer, DWORD size, DWORD *read)
{
    if (this->access != GENERIC_READ)
    {
        return 0;
    }
    return ReadFile(this->handle, buffer, size, read, NULL);
}

int File::Write(void *buffer, DWORD size, DWORD *written)
{
    if (this->access != GENERIC_WRITE)
    {
        return 0;
    }
    return WriteFile(this->handle, buffer, size, written, NULL);
}

int File::ReadByte()
{
    BYTE c;
    DWORD read;

    if (!this->Read(&c, 1, &read))
    {
        return -1;
    }
    else if (read == 0)
    {
        return -1;
    }
    else
    {
        return c;
    }
}

int File::WriteByte(int c)
{
    BYTE b = c;
    DWORD written;

    if (!this->Write(&b, 1, &written))
    {
        return -1;
    }
    else if (written == 0)
    {
        return -1;
    }
    else
    {
        return c;
    }
}

int File::Seek(DWORD offset, DWORD origin)
{
    if (this->handle == INVALID_HANDLE_VALUE)
    {
        return 0;
    }
    SetFilePointer(this->handle, offset, NULL, origin);
    return 1;
}

DWORD File::Tell()
{
    if (this->handle == INVALID_HANDLE_VALUE)
    {
        return 0;
    }
    return SetFilePointer(this->handle, 0, NULL, FILE_CURRENT);
}

DWORD File::GetSize()
{
    if (this->handle == INVALID_HANDLE_VALUE)
    {
        return 0;
    }
    return GetFileSize(this->handle, NULL);
}

void *File::ReadAll(DWORD maxSize)
{
    DWORD size;
    void *buffer;
    DWORD position;
    DWORD read;

    if (this->access != GENERIC_READ)
    {
        return NULL;
    }
    size = this->GetSize();
    if (size > maxSize)
    {
        return NULL;
    }
    buffer = LocalAlloc(LPTR, size);
    if (buffer == NULL)
    {
        return NULL;
    }
    position = this->Tell();
    if (!this->Seek(position, FILE_BEGIN))
    {
        return NULL;
    }
    if (!this->Read(buffer, size, &read))
    {
        LocalFree(buffer);
        return NULL;
    }
    this->Seek(position, FILE_BEGIN);
    return buffer;
}

File::~File()
{
    if (this->handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(this->handle);
        this->handle = INVALID_HANDLE_VALUE;
        this->access = 0;
    }
}
