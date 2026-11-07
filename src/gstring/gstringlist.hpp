// ================================================================================================
//  GString  ·  gstringlist.hpp
//  The GStringList: the pieces of a GString made with +, not yet joined.
//
//  A speculative reconstruction. See src/gstring/README.md, and Shlemiel and the string list,
//  part 7 of the articles.
//
//  Nobody makes one of these. People already had builders and (usually) didn't use them, so the
//  list lives inside the GString instead, as a payload: a = b + c + d + e + f builds a list of
//  five pieces and copies no characters. Appending, prepending, length, emptiness, comparison,
//  iteration and indexing all work on the list directly, and the length is the sum of the
//  lengths the pieces already know.
//
//  Sooner or later something needs the characters in one piece, such as a library call that
//  wants a std::wstring. Then the whole list is converted once, the result replaces the payload,
//  and the pieces are released. The length and hash the list had already computed are kept.
// ================================================================================================

#ifndef GSTRING_GSTRINGLIST_HPP
#define GSTRING_GSTRINGLIST_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "gstring.hpp"

#include <vector>

class GSTRING_API GStringList
{
public:
    GStringList();
    GStringList(const GString& left, const GString& right);

    void                Append(const GString& piece);       // a list appended is spliced in, flat
    void                Prepend(const GString& piece);      // costs what Append costs

    size_t              PieceCount() const                  { return m_pieces.size(); }
    const GString&      Piece(size_t index) const           { return m_pieces[index]; }

    bool                IsEmpty() const;
    GLength             LengthInCharacters() const;

    // Joins the pieces into one payload of the given encoding, once.
    GPayload*           Flatten(GEncoding::Type encoding, GCodePage codePage) const;

private:
    GStrings            m_pieces;
};

#endif // GSTRING_GSTRINGLIST_HPP
