// ================================================================================================
//  GString  ·  gspot.hpp
//  The GSpot: a position in a GString, for walking strings whose characters have no fixed size.
//
//  A speculative reconstruction. See src/gstring/README.md. For the name, and what my manager
//  said about it after a particularly gnarly off-by-one bug, see The myth of the wide character,
//  part 5 of the articles.
//
//  Stepping by character, a GSpot steps over a surrogate pair as one, and never splits a UTF-8
//  sequence. For a fixed-width payload, such as ASCII in a const char*, it can jump straight to a
//  position, and indexing takes constant time. For a variable-width one, it has to walk. The GSpot
//  makes walking the honest operation, and the rest of GString's methods mean most code never
//  needs a position at all.
//
//  A GSpot holds a reference to the body it walks. When a conversion or a comparison replaces
//  that body's payload, the generation goes up, and the GSpot finds its place again in the new
//  payload, by the character index it kept. A unit offset belongs to a payload; a character
//  index belongs to the text.
// ================================================================================================

#ifndef GSTRING_GSPOT_HPP
#define GSTRING_GSPOT_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "gstring.hpp"

class GSTRING_API GSpot
{
public:
    GSpot();
    GSpot(const GSpot& other);
    ~GSpot();
    GSpot& operator=(const GSpot& other);

    GCodePoint          operator*() const;          // the character here, or the unit, or the byte

    GSpot&              operator++();
    GSpot               operator++(int);
    GSpot&              operator--();
    GSpot               operator--(int);
    GSpot&              Advance(long steps);        // negative steps walk backwards

    bool                IsStart() const;
    bool                IsEnd() const;

    GLength             CharacterIndex() const;     // walks to find out, unless it already knows
    GLength             UnitOffset() const;         // in the payload as it is now

    GString::Step::Type Stepping() const            { return m_step; }

    friend bool operator==(const GSpot& l, const GSpot& r) { return  l.Same(r); }
    friend bool operator!=(const GSpot& l, const GSpot& r) { return !l.Same(r); }
    friend bool operator< (const GSpot& l, const GSpot& r) { return  l.Before(r); }

private:
    GSpot(GBody* body, GLength unit, GLength character, GString::Step::Type step);

    bool                Same(const GSpot& other) const;
    bool                Before(const GSpot& other) const;
    void                Resync() const;             // after the body's generation has moved on

    GBody*              m_body;
    mutable unsigned long m_generation;
    mutable GLength     m_unit;
    GLength             m_character;
    GString::Step::Type m_step;

    friend class GString;
    friend class GSubString;
};

#endif // GSTRING_GSPOT_HPP
