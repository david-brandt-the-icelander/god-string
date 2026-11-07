// ================================================================================================
//  GString  ·  gtokeniser.hpp
//  Splitting a string lazily, one piece at a time.
//
//  A speculative reconstruction. See src/gstring/README.md.
//
//  Like Rogue Wave's RWCTokenizer, and like strtok without strtok's hidden global state, which
//  lets two interleaved loops, or two threads, trample each other's place. Every token is a
//  GSubString, so tokenising allocates nothing.
//
//      GTokeniser next(line);
//      while (!next.IsDone())
//          fields.push_back(next(GString::Literal(",")));
// ================================================================================================

#ifndef GSTRING_GTOKENISER_HPP
#define GSTRING_GTOKENISER_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "gstring.hpp"
#include "gsubstring.hpp"

class GSTRING_API GTokeniser
{
public:
    explicit GTokeniser(const GString& text);

    GSubString          operator()();                               // the next word
    GSubString          operator()(const GString& separators);      // up to any of these
    GSubString          operator()(const GRegularExpression& separator);

    bool                IsDone() const;
    GSubString          Rest() const;

private:
    GString             m_text;
    GSpot               m_next;
};

#endif // GSTRING_GTOKENISER_HPP
