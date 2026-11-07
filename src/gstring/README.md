# GString, reconstructed

These headers are a speculative reconstruction of GString, the string class I wrote at OZ between 1998 and 2002 to
end the Great String War. The original source is gone. What is here is what it might have looked like: a historical
curiosity, written in 2026 to go with [the articles](../../articles/the-great-string-war/README.md), not a recovered
artefact.

**Nothing here is the original.** Every class name except GString, GSubString, GSpot, GStringList and
GRegularExpression is invented, and so is every method name, signature, enum, field and layout. The headers declare
an interface and implement none of it. They have never been part of a program that ran.

## What they are built from

- **Memory,** as the articles record it: the payload pointer that knew what it pointed to and whether it was borrowed
  or owned; null as empty; the reference count and copy-on-write; the length and hash worked out just in time and
  kept; the cheap checks before comparing characters; a separate hash for comparing without case; conversion that
  replaced a shared payload; comparison that left two equal strings sharing one payload, with a literal always
  winning; the factory for literals; the GSpot; the GSubString; the GStringList hidden inside the GString;
  GRegularExpression and its limited backtracking; the CISC catalogue of methods; no embedded nulls; and no Unix.
- **The articles' catalogue,** in *Every method you could want*, part 11, which is itself partly an assumption: the
  union of the string libraries the series names.
- **Guesswork,** for everything that makes those ideas fit together as C++. The headers mark the places where a
  specific rule is invented, such as which payload survives when neither is a literal.

The style is meant to pass for 1999: C++98, Win32, `#pragma once` behind `_MSC_VER`, a DLL export macro, enums
wrapped in structs, MFC's `MakeUpper` and `MakeReverse` naming for case and reversal, and no exceptions.

## The headers

| Header | Holds |
| --- | --- |
| [`all.hpp`](all.hpp) | Everything, in order. Include this one. |
| [`gconfig.hpp`](gconfig.hpp) | Windows only, the basic types, and the foreign string classes, declared but not included. |
| [`gencoding.hpp`](gencoding.hpp) | What the bytes mean, named every time. |
| [`gpayload.hpp`](gpayload.hpp) | Inside a GString: the payload, the shared body, who frees what, and who wins a comparison. |
| [`gstring.hpp`](gstring.hpp) | GString itself, with the whole catalogue. |
| [`gspot.hpp`](gspot.hpp) | The GSpot, a position by code point, which survives a change of payload. |
| [`gsubstring.hpp`](gsubstring.hpp) | The GSubString, and its answers to the two questions every borrowed string must answer. |
| [`gstringlist.hpp`](gstringlist.hpp) | The GStringList, the pieces of `a + b + c`, joined only when needed. |
| [`gtokeniser.hpp`](gtokeniser.hpp) | Lazy splitting, like `RWCTokenizer`. |
| [`gregularexpression.hpp`](gregularexpression.hpp) | GRegularExpression, every dialect at once, and the corner that was cut. |
| [`gformat.hpp`](gformat.hpp) | Arguments to `GString::Format`, numbered for translators. |
| [`gfwd.hpp`](gfwd.hpp) | Forward declarations. |

## A taste

```cpp
#include "gstring/all.hpp"

static const GString kGreeting = GString::Literal("Hello");    // borrowed from the binary, for free

GString Greet(const GString& name, const RWCString& title)
{
    GString line = kGreeting + GString::Literal(", ")          // a string list; nothing is copied
                 + GString(title, GEncoding::Utf8) + name;
    line.MakeUpper().PadRight(40);

    if (line.StartsWith(kGreeting))                             // lengths, hashes, then characters
        ::MessageBoxW(0, line.AsWideCharPointer(), 0, MB_OK);   // converted once, and kept

    return line;
}
```

## Checking them

GString never ran anywhere but Windows, and the headers say so with an `#error`. To have a modern compiler read them
anyway, define `GSTRING_SYNTAX_CHECK`, which replaces the Windows headers with the two types they need:

```bash
echo '#include "gstring/all.hpp"' | g++ -x c++ -std=c++98 -DGSTRING_SYNTAX_CHECK -Wall -Wextra -pedantic -Isrc -fsyntax-only -
```

They compile cleanly that way, as C++98 and as C++17, with GCC and with Clang. That says the declarations are
well-formed, and nothing more.
