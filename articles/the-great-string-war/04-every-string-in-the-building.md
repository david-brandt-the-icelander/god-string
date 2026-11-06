# Every string in the building

*The string types GString had to speak, and what each of them got wrong in its own way.*

Built at OZ, 1998–2002 · Written November 2026

---

> Part 4 of 11 in *The Great String War*. Which types were in the code base is from memory. How each of them behaved
> is general knowledge, checked in 2026.

Everything in iPulse talked to everything else through our proprietary RPC mechanism. There was no good standard
option open to us at the time. DCOM and CORBA ruled the world, and both were plain bad ideas. SOAP was arriving, and
had yet to prove to the world just how bad an idea it was too. Nobody had yet given up on SOAP, on XML-RPC and on
everybody's own proprietary protocol over HTTP, and moved on to REST. Roy Fielding had described REST in
[his dissertation](https://ics.uci.edu/~fielding/pubs/dissertation/top.htm) in 2000, and for years hardly anyone
building systems had read it. So every string type that anyone used, anywhere, sooner or later passed through my
code.

Unicode was the first major wrench thrown into the cogs. The cogs were already chock-full of gremlins from classical
code pages, which in turn were a slithering cesspool of snakes and bugs, put there by people who didn't care about
languages other than English. Even European programmers hardly gave a damn. Coding is done in English, so they were
all fluent, and they generally felt that normal people should adapt to the new English-only world anyway.

More gremlins: nobody seems to have thought about globalisation in the days of code pages. Software was supposed to
run in a single country at a time. Different clients could run in different countries, each happily using its own
broken and ambiguous code page, but what happened on the server in Vegas stayed in Vegas. A server that talked to
clients in Reykjavík, Stockholm and Tokyo at once was not a case anyone had designed for.

Here, roughly, is what GString had to speak.

## `char*`, in every flavour

The null-terminated C string was the common ancestor, and it came in more flavours than its type admitted. ASCII.
Code-paged text, where the same byte meant `Þ` on one machine and something else entirely on the next, and the code
page lived in the machine's settings rather than in the string. UTF-8, which needed no new type, and so got smuggled
into `char*` without a word. A `char*` gave no hint of which one it held.

Some people thought it a great idea to skip the null terminator and pass a pointer and a length around separately,
usually without telling anybody. A function that expected the terminator then read on until it found a zero byte
somewhere further along in memory.

## Literals in the binary

`"Hello"` in C++ source is a string the compiler puts in the binary's read-only data, and the linker may merge
identical ones into one. Programmers don't like sacrificing memory to copy a literal that is already sitting in the
data segment, so plenty of code kept the pointer and never copied it. Writing through such a pointer crashed, and
freeing it crashed worse. GString's factory for literals made a GString that borrowed one, at no cost; *Borrowed, not
copied* is about that.

## `wchar_t*`, `LPWSTR` and `OLECHAR`

Wide strings were two bytes per unit, since we ran on Windows. Whether those two bytes were UCS-2 or UTF-16 is a story
of its own, told in *The myth of the wide character*. There was UTF-32 too, in a handful of places, as arrays of
four-byte units. On Unix, `wchar_t` was four bytes, but we didn't support Unix, so that was, thankfully, somebody
else's problem.

## `TCHAR` and `LPTSTR`

One type that was two types, depending on a `#define`. With `UNICODE` and `_UNICODE` defined, `TCHAR` was a
`wchar_t`; without them, it was a `char`. A module compiled one way and a module compiled the other way could pass the
"same" `LPTSTR` to each other, and one of them read every second byte as a zero.

## `BSTR`

COM's string: wide characters, a four-byte length stored just before the first one, and a terminator after the last.
Because the length was stored, a `BSTR` could contain null characters, which every C function would then stop at. By
convention, a null `BSTR` was the same as an empty one, which is how GString treated null too. A `BSTR` had to be
allocated with `SysAllocString` and freed with `SysFreeString`, and it came with wrappers, `_bstr_t` and `CComBSTR`,
each with its own idea of who owned what.

## MFC's `CString`

Reference-counted and copy-on-write, with `Left`, `Right` and `Mid`, `Find`, `Format`, `LoadString` for strings in the
program's resources, and `Collate` for comparing by the locale's rules. And `GetBuffer`, which handed out the raw
buffer for writing and switched copy-on-write off until somebody remembered to call `ReleaseBuffer`.

## `std::string`

The standard's string, and in 1998 still new. Whether it was copy-on-write depended on whose standard library you had,
because the standard allowed both until 2011. Iterators, `find_first_of` and `find_last_not_of`, and `substr`, whose
second argument was a length, which half the code base thought was a position. It was also the most common carrier of
smuggled UTF-8, after `char*` itself.

## Rogue Wave's `RWCString`

The professional's choice in those days, before the standard library's strings were considered even remotely decent.
Reference-counted and copy-on-write, with `RWCSubString` for pieces that pointed into the original, `RWCTokenizer` for
splitting, its own regular expressions, and `RWWString` for wide characters. Many developers used the plain
`RWCString` to carry UTF-8, typically without telling anybody, since, of course: use the force, read the source, or you
just didn't deserve to know.

## `strstream`

The builder of the day. `ostrstream::str()` handed you the buffer and *froze* the stream, which meant the stream would
no longer free it. Either you called `freeze(false)` or you freed it yourself, and most people did neither.

## Java's `String` and Perl's scalars

Strings also arrived from the other languages at OZ. Java's `String` was immutable UTF-16, with its hash cached inside
it. When it crossed into C++ through JNI or Java's own serialisation, it came as *modified* UTF-8, which writes the null
character as two bytes, so that C code can't mistake it for a terminator, and writes characters outside the 16-bit
range as two encoded surrogates of three bytes each. It looked like UTF-8 and wasn't quite. Perl's scalars were bytes
or characters depending on a flag that most Perl programmers had never heard of.

## The single-block allocations

Some people had the better idea of allocating a string together with its reference count, hash and length, as a
single variable-length structure: the bookkeeping first, then the characters. One allocation instead of two, and
everything about the string in the same cache line. `BSTR`'s length prefix is the simplest case. That is the pattern
I chose for GString.

## Constant strings

Some strings were constant, and never changed after they were made. They needed neither copy-on-write nor
copy-everything-all-the-time, and every string class that offered one or the other made them pay for it anyway.
GString's borrowed payloads were partly for them.

## Who frees what

And God forbid that something should be allocated in one part of the code base and freed in another. Depending on
headers, `#ifdef`s, correct build dependencies, corrupted incremental build files, and which allocation and
deallocation routines each string class used, a week-long hunt for a memory corruption was not rare. A DLL built
against one C runtime freeing what another had allocated was a classic. So was a `BSTR` freed with `free`. CI/CD,
decent source control (in the Windows ecosystem, at least) and automated testing were hardly the fashion in those days
either, so finding the change that had caused the bug was nigh on impossible.

GString's answer was to make every payload remember where it came from. A payload was freed by the routine that
matched how it was allocated, and a borrowed payload wasn't freed at all.
