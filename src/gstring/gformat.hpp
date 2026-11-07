// ================================================================================================
//  GString  ·  gformat.hpp
//  Arguments to GString::Format.
//
//  A speculative reconstruction. See src/gstring/README.md.
//
//  Format strings took numbered arguments, %1$s and %2$d, so that a translator could move them
//  around without touching the code. Here, each argument carries its own type, so a %d given a
//  string is a mismatch that can be reported, where printf would have read past the stack.
// ================================================================================================

#ifndef GSTRING_GFORMAT_HPP
#define GSTRING_GFORMAT_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "gconfig.hpp"
#include "gfwd.hpp"

class GSTRING_API GFormatArg
{
public:
    GFormatArg(const GString& text);
    GFormatArg(const char* text);           // ASCII only; anything else should be a GString
    GFormatArg(const wchar_t* text);
    GFormatArg(int value);
    GFormatArg(long value);
    GFormatArg(unsigned long value);
    GFormatArg(double value);
    GFormatArg(bool value);

    struct Kind { enum Type { Text, Signed, Unsigned, Real, Boolean }; };

    Kind::Type      GetKind() const { return m_kind; }

private:
    Kind::Type      m_kind;
    const GString*  m_string;               // for Text, exactly one of these three is set
    const char*     m_narrow;
    const wchar_t*  m_wide;
    union
    {
        long            m_signed;
        unsigned long   m_unsigned;
        double          m_real;
        bool            m_boolean;
    };
};

#endif // GSTRING_GFORMAT_HPP
