// ================================================================================================
//  GString  ·  gstring.hpp
//  The string class that ended the Great String War, by speaking every other string fluently.
//
//  A speculative reconstruction. See src/gstring/README.md.
//
//  GString was the union of every string library in the building, because every faction's
//  favourite method had to be there, or that faction walked. So this class is large on purpose:
//  every operation it is logical to want on a string is here, under its own name, and the
//  laziest call is the right one. The catalogue follows Every method you could want, part 11 of
//  the articles; the signatures are invented.
//
//  Three conventions hold throughout:
//
//      ·  Every length names its unit: bytes, code units or characters. A character is a code
//         point. Extended grapheme clusters arrived in 2008, after GString.
//      ·  Every conversion names its encoding. A char* gives no hint of what it holds.
//      ·  Methods that change the string are verbs, change it in place, copy-on-write, and
//         return *this. Methods that only look are const.
//
//  There is no operator[]. Characters have variable length in every UTF, and direct indexing
//  goes to hell. Use a GSpot.
// ================================================================================================

#ifndef GSTRING_GSTRING_HPP
#define GSTRING_GSTRING_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "gconfig.hpp"
#include "gencoding.hpp"
#include "gfwd.hpp"
#include "gformat.hpp"

#include <string>
#include <time.h>

class GSTRING_API GString
{
public:

    // ============================================================================================
    //  Making one
    //
    //  From any string type in the building, by copying it. From a null pointer, too: null is
    //  empty, as it was for BSTR.
    // ============================================================================================

    GString();
    GString(const GString& other);              // shares the body; nothing is copied
    ~GString();

    GString(const char* text);                  // in the process's code page, as most char* were
    GString(const char* text, GEncoding::Type encoding, GCodePage codePage = 0);
    GString(const char* text, GLength bytes, GEncoding::Type encoding, GCodePage codePage = 0);
    GString(const wchar_t* text);               // UTF-16, surrogates and all
    GString(const wchar_t* text, GLength units);
    GString(const GCodePoint* text, GLength codePoints);

    GString(const std::string& text, GEncoding::Type encoding);
    GString(const std::wstring& text);
    GString(const RWCString& text, GEncoding::Type encoding);   // a smuggled UTF-8 carrier, often
    GString(const RWWString& text);
    GString(const CString& text);               // narrow or wide, as the module's TCHAR decided
    GString(const _bstr_t& text);
    GString(const CComBSTR& text);
    GString(const GSubString& piece);           // shares the original's storage

    GString& operator=(const GString& other);


    // ============================================================================================
    //  Making one for free
    //
    //  A literal is compiled into the binary's read-only data and lives as long as the program.
    //  Literal() borrows it: nothing to allocate, nothing to copy, nothing to free. It is also the
    //  payload that wins when two equal strings are made to share one.
    //
    //      static const GString kContentType = GString::Literal("Content-Type");
    //
    //  Borrow() is for everything else that outlives the GString. The caller makes the promise;
    //  GString never frees what it borrowed, and never hands a borrowed payload on.
    // ============================================================================================

    static GString      Literal(const char* text, GEncoding::Type encoding = GEncoding::Ascii);
    static GString      Literal(const wchar_t* text);

    static GString      Borrow(const char* text, GEncoding::Type encoding, GCodePage codePage = 0);
    static GString      Borrow(const wchar_t* text);
    static GString      Borrow(const std::string& text, GEncoding::Type encoding);
    static GString      Borrow(const std::wstring& text);

    // A BSTR is a wchar_t* to the compiler, so it gets a name of its own. Adopt takes ownership,
    // and frees it with SysFreeString; Copy leaves it where it was.
    static GString      FromBstr(BSTR text);
    static GString      AdoptBstr(BSTR text);


    // ============================================================================================
    //  Making one from something else
    // ============================================================================================

    static GString      FromNumber(long value);
    static GString      FromNumber(unsigned long value);
    static GString      FromNumber(double value, int decimals = -1);
    static GString      FromBoolean(bool value);                    // "true" or "false"
    static GString      FromDate(const struct tm& date);            // ISO 8601
    static GString      FromDate(const struct tm& date, const GString& pattern);

    static GString      Repeat(const GString& text, GLength times);
    static GString      Join(const GStrings& pieces, const GString& separator);
    static GString      Join(const GSubStrings& pieces, const GString& separator);

    // %1$s, %2$d and so on, numbered so that a translator can move them around.
    static GString      Format(const GString& pattern);
    static GString      Format(const GString& pattern, const GFormatArg& a1);
    static GString      Format(const GString& pattern, const GFormatArg& a1, const GFormatArg& a2);
    static GString      Format(const GString& pattern, const GFormatArg& a1, const GFormatArg& a2,
                               const GFormatArg& a3);
    static GString      Format(const GString& pattern, const GFormatArg& a1, const GFormatArg& a2,
                               const GFormatArg& a3, const GFormatArg& a4);
    static GString      Format(const GString& pattern, const GFormatArg& a1, const GFormatArg& a2,
                               const GFormatArg& a3, const GFormatArg& a4, const GFormatArg& a5);

    // Strings in the program's resources, as MFC's CString::LoadString did.
    static GString      FromResource(unsigned int id);


    // ============================================================================================
    //  Asking about it
    //
    //  The length in characters and the hashes are worked out the first time anyone asks, and
    //  kept. Lengths in bytes and code units depend on the encoding, so the caller names it.
    // ============================================================================================

    bool                IsEmpty() const;
    bool                IsBlank() const;                            // nothing but whitespace

    GLength             LengthInCharacters() const;
    GLength             LengthInCodeUnits(GEncoding::Type encoding) const;
    GLength             LengthInBytes(GEncoding::Type encoding) const;

    bool                StartsWith(const GString& prefix) const;
    bool                StartsWithIgnoringCase(const GString& prefix) const;
    bool                EndsWith(const GString& suffix) const;
    bool                EndsWithIgnoringCase(const GString& suffix) const;
    bool                Contains(const GString& text) const;
    bool                ContainsIgnoringCase(const GString& text) const;
    bool                Contains(const GRegularExpression& pattern) const;
    GLength             CountOf(const GString& text) const;
    GLength             CountOfWords() const;

    bool                IsAlphabetic() const;
    bool                IsDigits() const;
    bool                IsAlphanumeric() const;
    bool                IsNumeric() const;                          // a number GString can parse
    bool                IsUpperCase() const;
    bool                IsLowerCase() const;
    bool                IsAscii() const;


    // ============================================================================================
    //  Finding things
    //
    //  A position is a GSpot. A search that finds nothing returns End().
    // ============================================================================================

    GSpot               Find(const GString& text) const;
    GSpot               Find(const GString& text, const GSpot& from) const;
    GSpot               FindIgnoringCase(const GString& text) const;
    GSpot               FindLast(const GString& text) const;
    GSpot               FindLast(const GString& text, const GSpot& before) const;
    GSpot               Find(GCodePoint character) const;
    GSpot               FindLast(GCodePoint character) const;
    GSpot               FindOneOf(const GString& set) const;
    GSpot               FindLastOneOf(const GString& set) const;
    GSpot               FindFirstNotOf(const GString& set) const;
    GSpot               FindLastNotOf(const GString& set) const;
    GSpot               Find(const GRegularExpression& pattern) const;
    GSpot               Find(const GRegularExpression& pattern, const GSpot& from) const;


    // ============================================================================================
    //  Taking pieces
    //
    //  Every piece is a GSubString, which borrows this string's storage instead of copying it.
    //  Left and First are the same thing, under both names, because both factions asked.
    // ============================================================================================

    GSubString          Left(GLength characters) const;
    GSubString          Right(GLength characters) const;
    GSubString          Mid(const GSpot& from) const;
    GSubString          Mid(const GSpot& from, GLength characters) const;
    GSubString          Between(const GSpot& from, const GSpot& to) const;
    GSubString          First(GLength characters) const;
    GSubString          Last(GLength characters) const;

    GSubString          Before(const GString& separator) const;
    GSubString          BeforeLast(const GString& separator) const;
    GSubString          After(const GString& separator) const;
    GSubString          AfterLast(const GString& separator) const;

    GSubString          CharacterAt(const GSpot& at) const;


    // ============================================================================================
    //  Changing it
    // ============================================================================================

    GString&            Append(const GString& text);
    GString&            Prepend(const GString& text);
    GString&            Insert(const GSpot& at, const GString& text);

    GString&            Replace(const GString& from, const GString& to);       // every occurrence
    GString&            ReplaceFirst(const GString& from, const GString& to);
    GString&            ReplaceLast(const GString& from, const GString& to);
    GString&            Replace(const GRegularExpression& pattern, const GString& to);
    GString&            Remove(const GString& text);
    GString&            Splice(const GSpot& at, GLength characters, const GString& replacement);
    GString&            Repeat(GLength times);

    GString&            MakeReverse();          // by character, keeping accents on their letters

    GString&            PadLeft(GLength width, GCodePoint fill = ' ');
    GString&            PadRight(GLength width, GCodePoint fill = ' ');
    GString&            Centre(GLength width, GCodePoint fill = ' ');

    GString&            Trim();
    GString&            TrimLeft();
    GString&            TrimRight();
    GString&            Trim(const GString& set);
    GString&            TrimLeft(const GString& set);
    GString&            TrimRight(const GString& set);

    GString&            Truncate(GLength width, const GString& ellipsis = GString::Literal("..."));
    GString&            Prune(GLength width, const GString& ellipsis = GString::Literal("..."));
    GString&            WrapWords(GLength width, const GString& newline = GString::Literal("\r\n"));
    GString&            Translate(const GString& from, const GString& to);     // Perl's tr


    // ============================================================================================
    //  Case
    //
    //  The upper case of i is I, everywhere, including Turkey. GString gave every faction the
    //  answer its old class gave, and none of them knew about the dotless ı.
    // ============================================================================================

    GString&            MakeUpper();
    GString&            MakeLower();
    GString&            MakeSwappedCase();
    GString&            MakeCapitalised();      // the first letter, and only the first
    GString&            MakeTitleCase();        // The First Letter Of Every Word

    // The programmers' cases, which code generators needed all the time.
    GString&            MakeCamelCase();        // camelCase
    GString&            MakePascalCase();       // PascalCase
    GString&            MakeSnakeCase();        // snake_case
    GString&            MakeKebabCase();        // kebab-case


    // ============================================================================================
    //  Splitting and joining
    //
    //  For lazy splitting, see GTokeniser. For joining, see Join above, and operator+ below.
    // ============================================================================================

    GSubStrings         Split(const GString& separator) const;
    GSubStrings         Split(const GRegularExpression& separator) const;
    GSubStrings         SplitWords() const;
    GSubStrings         SplitCharacters() const;
    GSubStrings         SplitLines() const;     // at \n, \r\n or \r, whichever the file used


    // ============================================================================================
    //  Comparing
    //
    //  Comparing and collating are different things, and GString kept them apart. Compare goes
    //  by code point, whatever the payloads: a UTF-8 payload and a UTF-16 one compare as two
    //  UTF-8 payloads would. Collate goes by the process's locale, which on a server was the
    //  server's, and nobody's in particular. Nothing is normalised: é as one code point and é as
    //  e and an accent are two different strings, as they were everywhere else in the building.
    //
    //  Equality tries the cheap answers first, in order:
    //
    //      1. the same payload: equal, without looking further
    //      2. different lengths: not equal
    //      3. different hashes: not equal
    //      4. only then, the characters
    //
    //  And when step 4 says equal, the two are made to share one payload, so that the next time
    //  step 1 answers. A literal always wins; see GSurvivor in gpayload.hpp. That is why these
    //  methods are const and still change something: what is shared, never what the text is.
    // ============================================================================================

    int                 Compare(const GString& other) const;
    int                 CompareIgnoringCase(const GString& other) const;
    int                 Collate(const GString& other) const;
    int                 CollateIgnoringCase(const GString& other) const;

    bool                Equals(const GString& other) const;
    bool                EqualsIgnoringCase(const GString& other) const;

    GHash               Hash() const;
    GHash               HashIgnoringCase() const;


    // ============================================================================================
    //  Escaping and stripping
    // ============================================================================================

    GString&            EscapeHtml();
    GString&            UnescapeHtml();
    GString&            EscapeXml();            // XML was going to save the world any day now
    GString&            UnescapeXml();
    GString&            EscapeForRegularExpression();
    GString&            EscapeForCSource();
    GString&            StripTags();
    GString&            StripByteOrderMark();
    GString&            StripAccents();         // for URLs and file names


    // ============================================================================================
    //  Converting
    //
    //  The As... methods return a pointer that stays valid for as long as this GString does. To
    //  get it, GString converts the whole string once and replaces the shared payload with the
    //  result: every GString sharing the body gets the converted payload too, the old one is
    //  released, the length and hashes are kept, and the next call is free.
    //
    //  The To... methods return a new object the caller owns.
    // ============================================================================================

    const char*         AsCharPointer(GEncoding::Type encoding, GCodePage codePage = 0) const;
    const wchar_t*      AsWideCharPointer() const;
    const std::string&  AsStdString(GEncoding::Type encoding) const;
    const std::wstring& AsStdWString() const;

    std::string         ToStdString(GEncoding::Type encoding) const;
    std::wstring        ToStdWString() const;
    RWCString           ToRogueWaveString(GEncoding::Type encoding) const;
    RWWString           ToRogueWaveWString() const;
    CString             ToCString() const;
    BSTR                ToBstr() const;         // free it with SysFreeString

    long                ToLong(bool* ok = 0) const;
    unsigned long       ToUnsignedLong(bool* ok = 0) const;
    double              ToDouble(bool* ok = 0) const;
    bool                ToBoolean(bool* ok = 0) const;


    // ============================================================================================
    //  Walking it
    //
    //  Forwards and backwards, by character, by code unit or by byte. A GSpot stepping by
    //  character steps over a surrogate pair as one, and never splits a UTF-8 sequence.
    // ============================================================================================

    struct Step { enum Type { ByCharacter, ByCodeUnit, ByByte }; };

    GSpot               Begin(Step::Type step = Step::ByCharacter) const;
    GSpot               End(Step::Type step = Step::ByCharacter) const;


    // ============================================================================================
    //  Operators
    //
    //  + builds a GStringList inside the result, so a = b + c + d + e + f copies no characters
    //  until somebody needs them in one piece. Prepending costs what appending costs.
    //  < orders by code point, for keys and indexes; for people, use Collate.
    // ============================================================================================

    GString&            operator+=(const GString& text);

    friend GSTRING_API GString  operator+(const GString& left, const GString& right);

    friend bool operator==(const GString& l, const GString& r) { return  l.Equals(r); }
    friend bool operator!=(const GString& l, const GString& r) { return !l.Equals(r); }
    friend bool operator< (const GString& l, const GString& r) { return  l.Compare(r) <  0; }
    friend bool operator<=(const GString& l, const GString& r) { return  l.Compare(r) <= 0; }
    friend bool operator> (const GString& l, const GString& r) { return  l.Compare(r) >  0; }
    friend bool operator>=(const GString& l, const GString& r) { return  l.Compare(r) >= 0; }


private:
    explicit GString(GBody* body);

    GBody*              Body() const        { return m_body; }
    void                MakeUnique();               // copy-on-write: copy the body if it is shared
    void                Flatten() const;            // join a string list's pieces, once
    void                Convert(GEncoding::Type encoding, GCodePage codePage) const;
    void                ShareWith(const GString& equal) const;  // after a comparison said equal

    mutable GBody*      m_body;     // mutable, because looking may change what is shared

    friend class GSubString;
    friend class GSpot;
    friend class GStringList;
};

#endif // GSTRING_GSTRING_HPP
