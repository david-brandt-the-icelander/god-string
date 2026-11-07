// ================================================================================================
//  GString  ·  gpayload.hpp
//  Inside a GString: the payload, the body that shares it, and the rule for who wins.
//
//  A speculative reconstruction. See src/gstring/README.md.
//
//  What is memory: a GString carried its payload as a pointer to whatever native string it had
//  been given. It knew what type the pointer pointed to, and whether it was borrowed or owned,
//  and it never freed what it had borrowed. A null pointer was an empty string. It had a
//  reference count, so it was copy-on-write whatever the native string was. It worked out its
//  length and hash the first time anyone asked, and kept them. A conversion replaced the shared
//  payload, and so did a comparison that found two strings equal, and a literal always won.
//
//  What is invented: everything below that says how.
//
//      GString ──► GBody ──────────────► GPayload ──► the native string
//                  reference count        reference count
//                  generation             carrier and encoding
//                  length, hashes         ownership: who frees it, and how
//
//  Many GStrings share one body; that is copy-on-write. Bodies can share one payload; that is
//  what conversion and comparison leave behind, and why comparing two GStrings so often came
//  down to comparing two pointers.
// ================================================================================================

#ifndef GSTRING_GPAYLOAD_HPP
#define GSTRING_GPAYLOAD_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "gconfig.hpp"
#include "gencoding.hpp"
#include "gfwd.hpp"


// ------------------------------------------------------------------------------------------------
//  What the pointer points to
// ------------------------------------------------------------------------------------------------

struct GCarrier
{
    enum Type
    {
        Null,               // no pointer at all, which is an empty string
        CharPointer,        // const char*, terminated, or with a length for those who skip that
        WideCharPointer,    // const wchar_t*, LPWSTR, OLECHAR*
        CodePointArray,     // four-byte units, with a length
        Bstr,               // length-prefixed, may hold nulls, which GString would not accept
        StdString,          // a std::string, held by value
        StdWString,         // a std::wstring, held by value
        RogueWaveString,    // an RWCString, held by value, so its own count stays honest
        RogueWaveWString,   // an RWWString, likewise
        MfcString,          // a CString, likewise
        StringList,         // a GStringList: the pieces of a + b + c, not yet joined
        Slice               // a GSubString made into a GString: an offset into another payload
    };
};


// ------------------------------------------------------------------------------------------------
//  Who frees it, and how
//
//  "God forbid that something should be allocated in one part of the code base and freed in
//  another." Every payload remembers where it came from, and is freed by the matching routine.
// ------------------------------------------------------------------------------------------------

struct GOwnership
{
    enum Type
    {
        Literal,        // compiled into the binary; lives for ever; never freed; always wins
        Borrowed,       // the caller promised it outlives every GString that shares it
        Inline,         // GString's own: the characters follow the header, in the same block
        ArrayNew,       // freed with delete[]
        Malloc,         // freed with free(), by the runtime that allocated it
        SysAlloc,       // freed with SysFreeString(), never with free()
        HeldObject      // a foreign string object held by value; its destructor does the rest
    };
};


// ------------------------------------------------------------------------------------------------
//  GPayload: one native string
//
//  When GString makes a payload of its own, it is a single variable-length block: this header
//  first, then the characters. One allocation instead of two, and the bookkeeping in the same
//  cache line as the text. BSTR's length prefix is the simplest case of the same idea.
// ------------------------------------------------------------------------------------------------

struct GSTRING_API GPayload
{
    volatile long       references;     // changed with InterlockedIncrement and -Decrement
    GCarrier::Type      carrier;
    GEncoding::Type     encoding;
    GCodePage           codePage;       // for GEncoding::CodePage only
    GOwnership::Type    ownership;
    const void*         data;           // the native string, or the held object
    GLength             units;          // its length in code units, when the carrier knows it

    // The characters of an Inline payload start here.

    static GPayload*    Literal(const char* text, GEncoding::Type encoding);
    static GPayload*    Literal(const wchar_t* text);
    static GPayload*    Inline(GEncoding::Type encoding, GLength units);

    void                AddReference();
    void                Release();      // frees the native string by the matching routine

    bool                IsLiteral() const   { return ownership == GOwnership::Literal; }
    bool                IsFixedWidth() const;   // ASCII, Latin-1, a single-byte code page, UTF-32
    bool                CanBeShared() const;    // a merely borrowed payload is never handed on
};


// ------------------------------------------------------------------------------------------------
//  GBody: what GStrings share
//
//  Copy-on-write lives here. So do the caches, because they describe the text, not the payload:
//  a conversion changes the payload and leaves the length and hashes as they were.
//
//  The generation goes up whenever the payload is replaced, so that a GSpot that was walking the
//  old payload knows to find its place again, by character, in the new one.
// ------------------------------------------------------------------------------------------------

struct GSTRING_API GBody
{
    volatile long       references;
    unsigned long       generation;
    unsigned long       known;              // which of the caches below are filled in
    GLength             characters;         // code points, counted the first time anyone asked
    GHash               hash;               // of the exact code points
    GHash               hashIgnoringCase;   // its own, so it never disagrees with its comparison
    GPayload*           payload;

    enum Known
    {
        KnowsCharacters         = 0x01,
        KnowsHash               = 0x02,
        KnowsHashIgnoringCase   = 0x04,
        KnowsAscii              = 0x08,     // and is all ASCII, so any narrow encoding will do
        KnowsNotAscii           = 0x10
    };

    void                AddReference();
    void                Release();
    bool                IsShared() const    { return references > 1; }

    // Replaces the payload, keeps the caches, and bumps the generation. Every GString that shares
    // this body sees the new payload at once. The old one is released, and if nobody else holds
    // it, freed: the twenty pieces of a string list go here.
    void                Replace(GPayload* replacement);
};


// ------------------------------------------------------------------------------------------------
//  Who wins
//
//  When two GStrings with different payloads turn out to be equal, character by character, one
//  payload is kept and the other is released, and both bodies point at the survivor. From then
//  on, comparing them comes down to comparing two pointers.
//
//      1. A literal wins. It cost nothing to keep and is never freed.
//      2. A merely borrowed payload never wins, because it can't promise to outlive the other
//         string's sharers.
//      3. Otherwise, the left-hand operand's payload is kept.
//
//  Rule 1 is memory. Rules 2 and 3 are invented.
// ------------------------------------------------------------------------------------------------

GSTRING_API const GPayload* GSurvivor(const GPayload* left, const GPayload* right);

#endif // GSTRING_GPAYLOAD_HPP
