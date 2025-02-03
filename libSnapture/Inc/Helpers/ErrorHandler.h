#ifndef _ErrorHandler_H_
#define _ErrorHandler_H_
#include <Windows.h>
#include <iostream>

inline std::wstring GetErrorMessage(DWORD errorCode) {
    LPWSTR messageBuffer = nullptr;
    size_t size = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&messageBuffer,
        0,
        nullptr
    );

    std::wstring message(messageBuffer, size);
    LocalFree(messageBuffer);
    return message;
}

#endif