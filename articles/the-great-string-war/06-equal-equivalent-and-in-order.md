# Equal, equivalent, and in order

*Normalisation, comparison and collation are three different things, and most programmers think they are one.*

Built at OZ, 1998–2002 · Written November 2026

---

> Part 6 of 11 in *The Great String War*. The Unicode is general knowledge, and the examples were checked in 2026.
> What GString did is memory.

There is a distinction most programmers never make, and many don't know exists: between *normalisation*, *comparison*
and *collation*. They write `a == b` and `a < b`, and believe that the first means "the same text" and the second
means "in alphabetical order". Neither is true.

## Normalisation

**Normalisation is about representation.** Unicode often has more than one way of writing the same text. `é` can be
one code point, U+00E9, or two: `e` followed by a combining acute accent, U+0301. The Ångström sign, U+212B, is the
same text as the Scandinavian letter `Å`, U+00C5. These are *canonically equivalent*: the same text, and no reader
can tell them apart. Normalisation ([UAX #15](https://www.unicode.org/reports/tr15/)) picks one representation. NFC
composes, using the single code point wherever there is one, and NFD decomposes. The compatibility forms, NFKC and
NFKD, go further and fold together things that mean only roughly the same: the ligature `ﬁ` becomes `fi`, the
fullwidth `ＡＢ` becomes `AB`, `①` becomes `1`, and `x²` becomes `x2`. That is useful for search and for identifiers,
and destructive for nearly everything else.

Normalisation bites in places nobody expects. Apple, which we had all written off, came back, and its HFS+ file
system stored file names decomposed. A file called `Þórður.txt` saved on a Mac and the same name typed on Windows
were different strings, byte for byte, and two systems synchronising them each saw a file that the other didn't have.
Normalisation is also not closed under concatenation: `e` is in NFC, and so is a lone combining accent, but `e`
followed by the accent is not. A list of normalised pieces is not a normalised string, which is a trap for anything
built like a GStringList (*Shlemiel and the string list*).

## Comparison

**Comparison answers the question "are these the same?"** and there are several answers:

- *Binary:* the same code units. Fast, and wrong for the two spellings of `é`.
- *Canonical:* normalise, then compare. The same text, whatever the representation.
- *Case-insensitive:* case-fold, then compare. Which already depends on the language, because Turkish has two
  `i`s.
- *Whatever the database says.* In SQL Server, `'a' = 'A'` is true under a case-insensitive collation, and
  `'a' = 'a  '` is true under any of them.

Whichever you choose, hashing has to agree with it: strings that are equal must hash alike. That is what makes
GString's oldest trick safe, the one where different hashes mean different strings. Hash the code units and compare
case-insensitively, and the hash says `Reykjavík` and `REYKJAVÍK` are different at the same moment the comparison
says they are equal. A hash table built that way quietly loses entries.

## Ordering and collation

**Ordering is for machines, and collation is for people.** Sorting by code point is fast, total, the same on every
machine and in every locale: exactly right for keys, indexes, binary search, removing duplicates and deterministic
output. It is meaningless to a person, because `Z` sorts before `a` and `é` after `z`. Even binary order is not one
thing. UTF-8 bytes sort in code point order, but UTF-16 code units do not: by code point, the fullwidth `Ａ` (U+FF21)
comes before 😀 (U+1F600), and by UTF-16 code unit it comes after, because the surrogates sit below U+E000. Java's
`compareTo` sorts by UTF-16 code units, so a Java program and a UTF-8 database can disagree about the order of the
same strings, and a merge of their two sorted lists silently skips rows.

Collation is order as a reader of a particular language expects it. The
[Unicode Collation Algorithm](https://www.unicode.org/reports/tr10/), tailored per language by CLDR, compares in
levels: base letters first, then accents, then case, then punctuation. So `role`, `Role` and `rôle` sort in that
order, because the accent outranks the capital. Collation is not equality: two different strings can collate as
equal. It is not stable either: when glibc 2.28 updated its locale data in 2018,
[PostgreSQL indexes](https://wiki.postgresql.org/wiki/Locale_data_changes) built under the old order were silently
wrong and had to be rebuilt. And it is not new. C has had `strcoll`, and `strxfrm` for making a sort key that compares
with plain `strcmp`, since 1989. Most C programmers have never called either.

| The question | The operation | Depends on the language? |
| --- | --- | --- |
| Are these the same bytes? | Binary comparison | No |
| Are these the same text? | Normalise, then compare | No |
| Are these the same, ignoring case? | Case-fold, then compare | Sometimes |
| In what order do I store them? | Code point order | No |
| In what order do I show them? | Collation | Yes |

## What GString did

GString kept comparing and collating apart, as separate methods: comparing exactly, comparing without case, and
collating by the locale, the way MFC's `CString::Collate` and C's `strcoll` did. The cached hash was the exact one, and
a comparison that ignored case had a hash of its own, so the hash and the comparison never disagreed. Comparisons
went by code point, whatever the payloads were, because GString had to compare a UTF-8 payload with a UTF-16 one and
get the same answer as two UTF-8 payloads would.

What it didn't do was normalise. Hardly any library in the building offered normalisation, and a GString that
normalised would have disagreed with every string class it replaced about which strings were equal. In a war of string
classes, that's a bug report against the newcomer. So `é` and `e` followed by an accent stayed two different strings in
GString, as they were everywhere else at OZ. Its collation used the process's locale, which on a server was the
server's, and nobody's in particular.

Y does both differently, and *Every method you could want* says how.
