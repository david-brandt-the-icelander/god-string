// ================================================================================================
//  GString  ·  gconfig.hpp
//  The platform, the basic types, and the foreign string classes GString had to speak.
//
//  A speculative reconstruction. The original GString was written at OZ between 1998 and 2002,
//  and its source is gone. These headers are what it might have looked like, written in 2026
//  from memory and from the articles in articles/the-great-string-war/. Every name, signature
//  and layout in them is invented. See src/gstring/README.md.
// ================================================================================================

#ifndef GSTRING_GCONFIG_HPP
#define GSTRING_GCONFIG_HPP

#if _MSC_VER > 1000
#pragma once
#endif


// ------------------------------------------------------------------------------------------------
//  Platform
//
//  GString ran on Windows, where wchar_t is two bytes. On Unix, wchar_t is four bytes, and that
//  was, thankfully, somebody else's problem. GSTRING_SYNTAX_CHECK lets a 2026 compiler read these
//  headers without Windows; nothing built that way would ever have run.
// ------------------------------------------------------------------------------------------------

#if !defined(_WIN32) && !defined(GSTRING_SYNTAX_CHECK)
#error "GString runs on Windows only. Unix, with its four-byte wchar_t, is somebody else's problem."
#endif

#if defined(_WIN32)
#include <windows.h>
#include <oleauto.h>
#else
typedef wchar_t     OLECHAR;
typedef OLECHAR*    BSTR;
#endif

#include <stddef.h>


// ------------------------------------------------------------------------------------------------
//  Linkage
//
//  GString lived in a DLL, like everything else in the building. A payload remembers which
//  routine allocated it, so that it is freed by the matching one, whichever C runtime the
//  caller was linked against.
// ------------------------------------------------------------------------------------------------

#if defined(_WIN32) && defined(GSTRING_BUILDING_DLL)
#define GSTRING_API __declspec(dllexport)
#elif defined(_WIN32) && !defined(GSTRING_STATIC)
#define GSTRING_API __declspec(dllimport)
#else
#define GSTRING_API
#endif


// ------------------------------------------------------------------------------------------------
//  Basic types
//
//  long is 32 bits on Win32, which is all a code point needs.
// ------------------------------------------------------------------------------------------------

typedef unsigned char   GByte;          // one byte of a narrow payload
typedef wchar_t         GUnit16;        // one UTF-16 code unit
typedef unsigned long   GCodePoint;     // one Unicode code point, 0 to 0x10FFFF
typedef unsigned long   GLength;        // a count, always of a named unit
typedef unsigned long   GHash;          // a cached hash


// ------------------------------------------------------------------------------------------------
//  Foreign string classes
//
//  Declared, not included, so that a module which never touches MFC or Rogue Wave doesn't have
//  to drag their headers in to use a GString.
// ------------------------------------------------------------------------------------------------

class RWCString;        // Rogue Wave, narrow, and often a smuggled UTF-8 carrier
class RWWString;        // Rogue Wave, wide
class CString;          // MFC, TCHAR-wide
class _bstr_t;          // COM's BSTR, wrapped by the compiler's support library
class CComBSTR;         // COM's BSTR, wrapped by ATL, with its own idea of who owns what

#endif // GSTRING_GCONFIG_HPP
