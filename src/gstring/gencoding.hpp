// ================================================================================================
//  GString  ·  gencoding.hpp
//  What the bytes mean.
//
//  A speculative reconstruction. See src/gstring/README.md.
//
//  A char* gave no hint of what it held, so GString never guessed: every narrow string came in
//  with its encoding named, and every conversion out named the encoding it wanted. A wide
//  string was read as UTF-16, surrogates and all, which is also safe for UCS-2.
// ================================================================================================

#ifndef GSTRING_GENCODING_HPP
#define GSTRING_GENCODING_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "gconfig.hpp"

struct GEncoding
{
    enum Type
    {
        Ascii,              // seven bits, and nothing else
        Latin1,             // ISO 8859-1
        CodePage,           // whatever the code page says; the code page travels with it
        ProcessCodePage,    // whatever this machine's settings say, which is what most char* meant
        Utf8,
        ModifiedUtf8,       // Java's, through JNI: null as two bytes, surrogates as two sequences
        Utf16,              // wchar_t on Windows; UCS-2 reads the same, until it doesn't
        Utf32               // a handful of places, as arrays of four-byte units
    };
};

typedef unsigned short GCodePage;   // a Windows code page, such as 1252, for GEncoding::CodePage

#endif // GSTRING_GENCODING_HPP
