// ================================================================================================
//  GString  ·  gregularexpression.hpp
//  GRegularExpression: a superset of every regular-expression dialect in the building. Almost.
//
//  A speculative reconstruction. See src/gstring/README.md, and Crazier than Cocaine Bear,
//  part 10 of the articles.
//
//  To get buy-in, it had to accept every other expression library's syntax, so the dialect is a
//  parameter, and the default is all of them at once. It matches by code point, through the GSpot,
//  and knows Unicode's letter classes for anyone who uses them.
//
//  The corner that was cut: backtracking is supported only to a limited degree. Nobody in the
//  code base was writing the kind of expression that needed more. Here, the limit is a budget of
//  steps per match, and running out of it is an answer of its own, never a hung server.
// ================================================================================================

#ifndef GSTRING_GREGULAREXPRESSION_HPP
#define GSTRING_GREGULAREXPRESSION_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "gstring.hpp"
#include "gsubstring.hpp"

#include <vector>


struct GDialect
{
    enum Type
    {
        Everything,         // the union of all of the below, where they don't contradict each other
        Perl,               // Perl 5, the one every other faction measured itself against
        PosixBasic,         // grep and sed
        PosixExtended,      // egrep and awk
        RogueWave,          // RWCRegexp, and the faction that swore by it
        Wildcard            // *.txt and ?, for file names
    };
};

struct GRegexOption
{
    enum Flag
    {
        None                = 0x00,
        IgnoreCase          = 0x01,
        Multiline           = 0x02,     // ^ and $ match at every line
        DotMatchesNewline   = 0x04,
        Extended            = 0x08,     // whitespace and # comments in the pattern are ignored
        Anchored            = 0x10      // the whole string, or nothing
    };
};

struct GMatchOutcome
{
    enum Type
    {
        Matched,
        NoMatch,
        TooAmbitious        // ran out of backtracking budget; the pattern needs rewriting
    };
};


// ------------------------------------------------------------------------------------------------
//  GMatch: one match, and its groups, all of them borrowing the subject's storage
// ------------------------------------------------------------------------------------------------

class GSTRING_API GMatch
{
public:
    GMatch();

    GMatchOutcome::Type Outcome() const             { return m_outcome; }
    bool                IsMatch() const             { return m_outcome == GMatchOutcome::Matched; }

    GSubString          Whole() const;
    GSubString          Group(int number) const;    // \1, $1, or %1, whatever the dialect called it
    GSubString          Group(const GString& name) const;
    int                 GroupCount() const;

    GSpot               Begin() const;
    GSpot               End() const;

private:
    GMatchOutcome::Type m_outcome;
    GSubStrings         m_groups;                   // the whole match first
    GStrings            m_names;

    friend class GRegularExpression;
};

typedef std::vector<GMatch> GMatches;


// ------------------------------------------------------------------------------------------------
//  GRegularExpression
// ------------------------------------------------------------------------------------------------

class GSTRING_API GRegularExpression
{
public:
    explicit GRegularExpression(const GString& pattern,
                                unsigned long options = GRegexOption::None,
                                GDialect::Type dialect = GDialect::Everything);
    GRegularExpression(const GRegularExpression& other);   // shares the compiled program
    ~GRegularExpression();
    GRegularExpression& operator=(const GRegularExpression& other);

    // A malformed pattern is found when it is compiled, which is to say at run time, in 1999.
    bool                IsValid() const;
    GString             Error() const;
    GSpot               ErrorAt() const;

    const GString&      Pattern() const             { return m_pattern; }

    bool                Matches(const GString& subject) const;          // the whole of it
    GMatch              Search(const GString& subject) const;
    GMatch              Search(const GString& subject, const GSpot& from) const;
    GMatches            SearchAll(const GString& subject) const;

    // $1 and \1 both work in the replacement, since both factions asked.
    GString             Replace(const GString& subject, const GString& replacement) const;
    GString             ReplaceFirst(const GString& subject, const GString& replacement) const;
    GSubStrings         Split(const GString& subject) const;

    static GString      Escape(const GString& text);

private:
    struct Program;                                 // compiled, reference-counted, and private

    GString             m_pattern;
    unsigned long       m_options;
    GDialect::Type      m_dialect;
    Program*            m_program;
};

#endif // GSTRING_GREGULAREXPRESSION_HPP
