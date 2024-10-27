/// file gccjit-refpersys/jzon.h
/// SPDX-License-Identifier: MIT 
// Copyright (c) 2020 Karl Zylinski
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
  #pragma once

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif

#include <stdint.h>
#include <stdlib.h>

struct JzonKeyValuePair;
typedef struct JzonKeyValuePair JzonKeyValuePair;

typedef struct JzonValue
{
    bool is_string : 1;
    bool is_int : 1;
    bool is_float : 1;
    bool is_table : 1;
    bool is_array : 1;
    bool is_bool : 1;
    bool is_null : 1;
    uint32_t size;

    union
    {
        char* string_val;
        int int_val;
        bool bool_val;
        float float_val;
        struct JzonKeyValuePair* table_val;
        struct JzonValue* array_val;
    };
} JzonValue;

struct JzonKeyValuePair {
    char* key;
    uint64_t key_hash;
    JzonValue val;
};

typedef struct JzonParseResult {
    bool ok;
    JzonValue output;
} JzonParseResult;

typedef void* (*jzon_allocate)(size_t);
typedef void (*jzon_deallocate)(void*);
typedef void* (*jzon_reallocate)(void*, size_t);

typedef struct JzonAllocator {
    jzon_allocate allocate;
    jzon_deallocate deallocate;
    jzon_reallocate reallocate;
} JzonAllocator;

// Parse using default malloc allocator.
JzonParseResult jzon_parse(const char* input);

// Parse using custom allocator. Make sure to call jzon_free_custom_allocator using the same allocator.
JzonParseResult jzon_parse_custom_allocator(const char* input, JzonAllocator* allocator);

// Free parse result data structure using default free deallocator.
void jzon_free(JzonValue* value);

// Free parse result data structure which was parsed using custom allocator. Make sure to pass the same allocator you did to jzon_parse_custom_allocator.
void jzon_free_custom_allocator(JzonValue* value, JzonAllocator* allocator);

// Find object value by key. Returns NULL if object is not an actual jzon object or there exists no value with the specified key. Uses a binary search algorithm.
JzonValue* jzon_get(JzonValue* object, const char* key);

#ifdef __cplusplus
}
#endif
