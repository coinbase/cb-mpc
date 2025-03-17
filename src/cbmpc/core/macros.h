
#pragma once

#include "precompiled.h"

#ifdef __APPLE__
#include "TargetConditionals.h"

#if TARGET_OS_IOS || TARGET_OS_TV || TARGET_OS_WATCH
#define TARGET_OS_IOSX 1
#endif

#endif

#define __vectorcall

#define STDCALL
#define SELECTANY __attribute__((weak))
#define DLLEXPORT __attribute__((visibility("default")))
#define DLLEXPORT_DEF DLLEXPORT

#ifndef NULL
#define NULL ((void*)0)
#endif

#define FOR_EACH(i, c) for (auto i = (c).begin(); i != (c).end(); ++i)

typedef void* void_ptr;
typedef const void* const_void_ptr;

typedef uint8_t byte_t;
typedef byte_t* byte_ptr;
typedef const byte_t* const_byte_ptr;

typedef char* char_ptr;
typedef const char* const_char_ptr;

#define unordered_map_t std::unordered_map
#define unordered_set_t std::unordered_set
#define unordered_multiset_t std::unordered_multiset

#ifndef _countof
#define _countof(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifdef __x86_64__
#pragma GCC attribute(__attribute__((target("sse4.1"))), apply_to = function)
#pragma GCC attribute(__attribute__((target("aes"))), apply_to = function)
#pragma GCC attribute(__attribute__((target("pclmul"))), apply_to = function)
#endif

#define AUTO(var, val) decltype(val) var = val

#define MASKED_SELECT(mask, y, z) (((y) & (mask)) | ((z) & ~(mask)))