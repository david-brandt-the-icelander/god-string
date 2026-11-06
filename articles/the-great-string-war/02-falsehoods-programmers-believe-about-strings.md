# Falsehoods programmers believe about strings

*Fifty-four of them, and what GString did about each.*

Built at OZ, 1998–2002 · Written November 2026

---

> Part 2 of 11 in *The Great String War*. The falsehoods come from the genre, and the sources are linked. The
> verdicts are about GString, the string class I wrote at OZ, and are from memory.

The genre started with Patrick McKenzie's
[*Falsehoods programmers believe about names*](https://www.kalzumeus.com/2010/06/17/falsehoods-programmers-believe-about-names/)
in 2010. Text got its own lists later: Jeremy Hussell's
[*Falsehoods programmers believe about plain text*](https://jeremyhussell.blogspot.com/2017/11/falsehoods-programmers-believe-about.html),
Thias's [*Falsehoods programmers believe about text*](https://wiesmann.codiferes.net/wordpress/archives/30296), Jean
Abou Samra's [*Unicode misconceptions*](https://jean.abou-samra.fr/blog/unicode-misconceptions/), and the reverse of the
genre, James Bennett's [*Truths programmers should know about case*](https://www.b-list.org/weblog/2018/nov/26/case/).
Manish Goregaokar's
[*Let's stop ascribing meaning to code points*](https://manishearth.github.io/blog/2017/01/14/stop-ascribing-meaning-to-unicode-code-points/)
and Joel Spolsky's
[*The absolute minimum every software developer absolutely, positively must know about Unicode*](https://www.joelonsoftware.com/2003/10/08/the-absolute-minimum-every-software-developer-absolutely-positively-must-know-about-unicode-and-character-sets-no-excuses/)
explain most of them. Kevin Deldycke's [*Awesome falsehood*](https://github.com/kdeldycke/awesome-falsehood) collects
the lot.

Of all that, this list is the part that mattered to GString. Every one of these has bitten me, a colleague, or a code
base I was responsible for, and most of them bit while GString was earning its trust.

Each one ends with what GString did about it, and with which part of it: the payload, which knew what kind of string
it carried; the factory functions; the GSpot; the GSubString; the GStringList; or GRegularExpression. *The Great
String War*, part 1, introduces them. **Solved** means GString got it right for anyone who used it. **Partly** means
it got it right as far as it could. **Unsolved** means it didn't.

**About characters**

1. A character is a byte. **Solved:** the payload knew its encoding, and a length in bytes and a length in characters
   were different methods.
2. A character is a `char`, a `wchar_t`, or 16 bits. **Solved:** the payload type, which the factory functions made
   you name.
3. Well then, a character is 32 bits. (It's a code point, and the flag of England is seven of them.) **Unsolved:** a
   GString character was a code point. Unicode didn't define the extended grapheme cluster, what a reader sees as one
   character, until 2008 ([UAX #29](https://www.unicode.org/reports/tr29/)).
4. Unicode is a 16-bit character set. (It was, from 1991 until
   [Unicode 2.0](https://www.unicode.org/versions/Unicode2.0.0/) in 1996. Windows NT and Java were designed in
   between, which is why both are UTF-16 to this day.) **Solved:** a two-byte payload was read as UTF-16, surrogates
   and all.
5. UTF-16 is fixed-width. (Surrogate pairs: [RFC 2781](https://www.rfc-editor.org/rfc/rfc2781).) **Solved:** the
   GSpot stepped over a surrogate pair as one code point.
6. UCS-2 and UTF-16 are the same thing. (UTF-16 is UCS-2 plus surrogate pairs. Code written for UCS-2 works on UTF-16
   until the first emoji.) **Partly:** GString read every two-byte string as UTF-16, which is safe for UCS-2 data. It
   couldn't fix the UCS-2 code on the other end of the wire.
7. A wide character is Unicode. (It is UCS-2, UTF-16 or UCS-4, depending on the platform, and the type doesn't say
   which.) **Solved:** the payload type said which.
8. Every code point is a character. (Combining marks, joiners, variation selectors, unassigned and private-use code
   points.) **Unsolved.**
9. A character always looks the same. (Arabic letters change shape by position; fonts substitute ligatures.) **Not a
   string's problem:** it's the font's.

**About length and position**

10. A string has a length. (It has at least four: bytes, code units, code points and grapheme clusters. Its width on
    screen is a fifth.) **Partly:** bytes, code units and code points each had a method. Grapheme clusters and width
    on screen didn't.
11. Getting the length is cheap. (Not if you have to find the null terminator, or the grapheme boundaries.) **Solved:**
    the length was computed when first asked for, and kept.
12. Indexing into a string is constant time. **Partly:** GString supported the classical fixed-width formats, such as
    `std::string` and `const char*`, natively, and indexing those took constant time. For the variable-width ones it
    was a walk. The GSpot made walking the honest operation, and the CISC methods meant most code never needed an
    index at all.
13. A string of `n` characters takes `n` columns on screen. (East Asian wide characters take two, per
    [UAX #11](https://www.unicode.org/reports/tr11/); combining marks and
    zero-width joiners take none; emoji take whatever the terminal feels like.) **Unsolved.**
14. Cutting a string at `n` bytes gives you a valid string. (It can cut a UTF-8 sequence, a surrogate pair or a
    grapheme in half.) **Partly:** left, right, truncate and prune never split a UTF-8 sequence or a surrogate pair,
    but they could separate an accent from its letter.
15. Reversing a string reverses its characters. (Reverse the code points of 🇮🇸 and you get a different country's
    flag; reverse `é` written as `e` plus an accent and the accent lands on whatever came before.) **Partly:**
    reversing kept combining accents on their letters. Flags arrived in Unicode a decade later.
16. Positions start at 0. (Not in PL/SQL, Pascal or COBOL.) **Partly:** GString counted from 0, but left, right,
    first, last and the GSpot meant most code never wrote a position down.
17. `NVARCHAR(50)` holds 50 characters. (It holds 50 UTF-16 code units.) **Partly:** you could ask a GString for its
    length in UTF-16 code units. Nothing made you.
18. Oracle's `VARCHAR2(50)` holds 50 characters. (It holds 50 bytes, unless someone declared it in characters.)
    **Partly:** the same, in bytes.

**About equality, normalisation and order**

19. Two strings that look the same are equal. (`é` can be one code point or two, and they only compare equal after
    normalisation: [UAX #15](https://www.unicode.org/reports/tr15/).) **Unsolved:** GString didn't normalise.
20. Two strings that are different look different. (Latin `a`, Cyrillic `а`. Phishing domains depend on it;
    [UTS #39](https://www.unicode.org/reports/tr39/) lists the confusables.)
    **Unsolved,** here and everywhere else.
21. The empty string and no string are different things. (Not in Oracle, where the empty string is `NULL`.)
    **Solved,** Oracle's way: GString interpreted null as empty, so there was no difference left to lose on the way
    into or out of the database.
22. Alphabetical order is the same everywhere. (In Icelandic, `á` is its own letter right after `a`, `ð` sits after
    `d`, and `þ`, `æ` and `ö` come at the very end. Swedish puts `å`, `ä` and `ö` after `z`, and German has two
    orders, one of them for phone books.) **Partly:** collation was there, but it used the process's locale, and on a
    server that was the server's.
23. Sorting by code point is sorting alphabetically. (It is sorting by number, and `Z` comes before `a`.) **Solved:**
    comparing and collating were separate methods.
24. Code point order is the same in every encoding. (UTF-16 code units put 😀 before `Ａ`; code points put it after.)
    **Solved:** GString compared by code point whatever its payloads were, because it had to compare a UTF-8 payload
    with a UTF-16 one.
25. Comparing strings and collating them are the same thing. **Solved:** separate methods.
26. Strings that collate equal are equal. (Collations can ignore case, accents or trailing spaces; T-SQL's `=`
    ignores trailing spaces whatever the collation.) **Solved:** separate methods again.
27. A collation never changes. (It changes with library versions, and the indexes built on it go wrong, as
    [PostgreSQL found out](https://wiki.postgresql.org/wiki/Locale_data_changes).)
    **Unsolved.**
28. Normalising text is always safe. (NFKC turns `x²` into `x2`.) **Unsolved,** by not normalising at all.
29. Two normalised strings, joined, make a normalised string. **Unsolved,** for the same reason, and the GStringList
    is exactly where it would have bitten.
30. The file name you saved is the file name you get back. (HFS+ decomposed it.) **Unsolved.** Apple was dead in those
    days, or so we all thought.
31. A case-insensitive comparison can use the ordinary hash. **Solved:** the cached hash was the exact one, and a
    case-insensitive comparison had a hash of its own and never took the exact hash's shortcut.

**About case**

32. Upper and lower case map one to one. (`ß` uppercases to `SS`, and `ﬃ` to `FFI`; see Unicode's
    [SpecialCasing.txt](https://www.unicode.org/Public/UCD/latest/ucd/SpecialCasing.txt).) **Unsolved:** case
    mapping went one character at a time, so `ß` stayed `ß`.
33. Changing case doesn't change the length. **Unsolved,** for the same reason.
34. Changing case doesn't depend on the language. (Turkish `i` uppercases to `İ`, and `I` lowercases to `ı`.)
    **Unsolved:** the process's locale again.
35. Changing case doesn't depend on the position. (Greek `Σ` lowercases to `ς` at the end of a word and `σ`
    elsewhere.) **Unsolved.**
36. Lowercasing both strings is a case-insensitive comparison. (That's what case folding is for, and it still depends
    on the language.) **Partly:** comparing without case was a method of its own, but it folded one character at a
    time.
37. Every script has case. (Most don't.) **Solved,** in that nothing in GString assumed it.

**About encodings and bytes**

38. A byte is eight bits. (Machines have had bytes of 6, 7, 8 and 9 bits, and more. A byte was originally a machine's
    unit for storing or handling a character, not a number of bits.) **Not a string's problem,** luckily: we were born
    after that, and GString didn't care. Still, even today, the C and C++ standards don't require a byte to be eight
    bits.
39. Text is a sequence of bytes. (Text is a sequence of characters, and becomes bytes only through an encoding.)
    **Solved:** the payload type.
40. You can tell what encoding text is in by looking at it. (You can guess.) **Partly:** the factory functions made
    you say what you were handing over, so GString never guessed. Text from files and the network still arrived
    unlabelled.
41. The text is in the machine's code page. (Whose machine?) **Solved:** a code-paged payload carried its code page
    with it, because the RPC layer carried strings from one machine to another.
42. It's UTF-8, because everything is UTF-8 now. (Excel would like a word about CSV files.) **Solved:** the payload
    type, again.
43. Invalid input can be quietly fixed up. (Replacing what can't be decoded with `?` or `�` throws data away, and
    passing it through unchecked lets the next program choke on it.) **Unsolved:** conversions replaced what they
    couldn't convert with `?`, like everybody else's.
44. The null byte ends the string. (`BSTR` can contain them; UTF-16 is full of zero bytes; and some people skipped the
    terminator anyway.) **Partly:** the length was stored, so nothing scanned for a terminator more than once. But
    GString had no support for embedded nulls.
45. A string in one module can be freed in another. **Solved:** a payload was freed by the routine that matched how
    it was allocated, and a borrowed payload wasn't freed at all.

**About regular expressions**

46. `.` matches one character. (It matches a code point, or in some engines a UTF-16 code unit.) **Partly:**
    GRegularExpression matched by code point.
47. `[a-z]` matches the lowercase letters. (`á`, `ð`, `þ`, `æ` and `ö` are lowercase letters;
    [UTS #18](https://www.unicode.org/reports/tr18/) says how a regular expression should know.) **Partly:** Unicode's
    letter classes were there, for anyone who used them.
48. `\w` matches the characters of a word. (In many engines, only ASCII ones.) **Partly:** the same.
49. A regular expression finishes in reasonable time. (Backtracking engines can take exponential time on the wrong
    input. That is how one badly written expression
    [took Stack Overflow down in 2016](https://stackstatus.tumblr.com/post/147710624694/outage-postmortem-july-20-2016).)
    **Solved,** mostly by omission: GRegularExpression backtracked only to a limited degree.
50. Every regular-expression library speaks the same language. **Solved:** GRegularExpression was the one language.

**About performance**

51. Concatenation is cheap. (See `b + c + d + e + "f" + g` in *Shlemiel and the string list*.) **Solved:** the
    GStringList.
52. Comparing two strings is cheap. (Not if they are long, equal up to the last character, and nobody cached a hash.)
    **Solved:** cached lengths and hashes.
53. Copying a string is cheap. (Copy-on-write makes it cheap, until two threads
    [share the reference count](http://www.gotw.ca/publications/optimizations.htm).)
    **Solved:** reference counting, and borrowed literals that were never copied at all.
54. My string class is the fast one. **Solved.** Mine was.

## The score

GString solved 23 of these 54, got 15 partly right and left 14 unsolved, and 2 were never a string's problem.

Read down the verdicts and the pattern is plain. GString solved the falsehoods about representation, memory and
speed: what kind of string something is, where its length is, who frees it, and what it costs to copy, join and
compare. It left the ones about meaning: what a reader sees as one character, when two strings are the same text, and
what upper case means in a given language. *The Great String War* says why, and *Every method you could want* says
what Y does about the second half.
