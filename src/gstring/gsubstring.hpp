// ================================================================================================
//  GString  ·  gsubstring.hpp
//  A piece of a GString that borrows the original's storage instead of copying it.
//
//  A speculative reconstruction. See src/gstring/README.md.
//
//  Parsing is mostly taking pieces: split a line into fields, trim each field, compare it with
//  something. With copying substrings, every one of those steps allocates. With these, none of
//  them does, until somebody keeps a piece for longer than the line it came from.
//
//  Every string that borrows its storage has to answer two questions.
//
//      What if the original goes away first?
//          A GSubString holds a reference to the original's body, so the original can't be freed
//          while a piece of it is in use.
//
//      What if the original is huge, and the piece is tiny?
//          Then the piece keeps the whole original alive. Call Detach() before storing it.
// ================================================================================================

#ifndef GSTRING_GSUBSTRING_HPP
#define GSTRING_GSUBSTRING_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "gstring.hpp"
#include "gspot.hpp"

class GSTRING_API GSubString
{
public:
    GSubString();                               // an empty piece of nothing
    GSubString(const GSpot& from, const GSpot& to);

    // GString(piece) shares the original's storage, as a Slice payload: cheap, and it keeps the
    // original alive. Detach() copies just these characters, and lets the original go.
    GString             Detach() const;

    bool                IsEmpty() const;
    GLength             LengthInCharacters() const;
    GLength             LengthInCodeUnits(GEncoding::Type encoding) const;

    GSpot               Begin() const       { return m_from; }
    GSpot               End() const         { return m_to; }
    const GString&      Original() const    { return m_original; }

    int                 Compare(const GString& other) const;
    int                 CompareIgnoringCase(const GString& other) const;
    bool                Equals(const GString& other) const;
    bool                EqualsIgnoringCase(const GString& other) const;

    // The same pieces a GString offers, taken from this piece, and still borrowing.
    GSubString          Left(GLength characters) const;
    GSubString          Right(GLength characters) const;
    GSubString          Mid(GLength from, GLength characters) const;
    GSubString          Trimmed() const;        // a piece can't change, so it gets a narrower piece

    friend bool operator==(const GSubString& l, const GString& r) { return  l.Equals(r); }
    friend bool operator!=(const GSubString& l, const GString& r) { return !l.Equals(r); }

private:
    GString             m_original;
    GSpot               m_from;
    GSpot               m_to;
};

#endif // GSTRING_GSUBSTRING_HPP
