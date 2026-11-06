# The Great String War

A series of eleven articles for [icelander.net](https://icelander.net/), in the *Programming languages* train of
thought. It is about GString, the string class I wrote at OZ to end the Great String War, and about strings in
general: what programmers believe about them that isn't true, the string types of the day, Unicode, normalisation and
collation, concatenation, borrowing, the classic optimisations, and regular expressions. The last part says what Y kept.

| Part | Title | Subtitle | Slug |
| --- | --- | --- | --- |
| 1 | [The Great String War](01-the-great-string-war.md) | How a string class nicknamed GString ended a war it couldn't win. | `the-great-string-war` |
| 2 | [Falsehoods programmers believe about strings](02-falsehoods-programmers-believe-about-strings.md) | Fifty-four of them, and what GString did about each. | `falsehoods-programmers-believe-about-strings` |
| 3 | [Falsehoods programmers believe about localisation](03-falsehoods-programmers-believe-about-localisation.md) | Fifteen of them, and why most were out of a string's reach. | `falsehoods-programmers-believe-about-localisation` |
| 4 | [Every string in the building](04-every-string-in-the-building.md) | The string types GString had to speak, and what each of them got wrong in its own way. | `every-string-in-the-building` |
| 5 | [The myth of the wide character](05-the-myth-of-the-wide-character.md) | UCS-2, UCS-4, code points, graphemes, and the GSpot. | `the-myth-of-the-wide-character` |
| 6 | [Equal, equivalent, and in order](06-equal-equivalent-and-in-order.md) | Normalisation, comparison and collation are three different things, and most programmers think they are one. | `equal-equivalent-and-in-order` |
| 7 | [Shlemiel and the string list](07-shlemiel-and-the-string-list.md) | Why `a + b + c` is slow, why builders don't fix it, and the GStringList. | `shlemiel-and-the-string-list` |
| 8 | [Borrowed, not copied](08-borrowed-not-copied.md) | Substrings and literals that share storage, and the two questions every one of them has to answer. | `borrowed-not-copied` |
| 9 | [Tricks of the string trade](09-tricks-of-the-string-trade.md) | Stored lengths, cached hashes, early exits, copy-on-write, and what became of them. | `tricks-of-the-string-trade` |
| 10 | [Crazier than Cocaine Bear](10-crazier-than-cocaine-bear.md) | Regular expressions, GRegularExpression, and the corner I cut. | `crazier-than-cocaine-bear` |
| 11 | [Every method you could want](11-every-method-you-could-want.md) | The CISC approach to strings, and what Y kept from GString. | `every-method-you-could-want` |

These are the working copies. The versions in the site's format, with front matter and US spelling, are in
[icelander.net](https://github.com/david-brandt-the-icelander/icelander.net) under `articles/the_great_string_war/`,
and [notes.md](notes.md) is the notes file for them: what changed in the move, what is memory and what is not, and
what is still for the author to check.

Every part carries the date line *Built at OZ, 1998–2002 · Written November 2026*, and opens with a short note saying
what in it is memory and what is not. The notes should stay when the parts are published. Parts refer to each other by
title, in italics, so that the references read the same here and on the site.

## Sources

- **Memory:** the story of GString at OZ, on iPulse's RPC layer: the string types it had to interoperate with, the
  factory for literals compiled into the binary, the GSpot, GRegularExpression and its limited backtracking,
  GSubString and GStringList, and how a GString was built inside.
- **Assumed, for the author to check:** the catalogue of methods in part 11, taken as the union of the string
  libraries the series names and [Voca](https://github.com/panzerdp/voca), and the verdicts on each falsehood in parts
  2 and 3 that follow from it.
- **General knowledge, checked in 2026, with sources linked in the parts:** the falsehoods and the lists they come
  from; the string types of the day and their quirks; the history of UCS-2, UCS-4, UTF-16 and UTF-8; normalisation,
  comparison and collation; ropes and builders in later languages; borrowing substrings in Java, Go, Swift, C++ and
  Rust; caching, interning, copy-on-write, the small string optimisation and compact representations; backtracking and
  automata, and the outages backtracking caused; and the byte counts, normalisations and orderings of the examples.
- **Y's design documents:** the strings section of [`../../proposal/Y-proposal.md`](../../proposal/Y-proposal.md)
  (§ 3.3, *Strings: the y-string library*), the string ideas of earlier iterations in
  [`../../proposal/Y-idea-harvest.md`](../../proposal/Y-idea-harvest.md) (§ 3.1, § 3.2, § 3.5 and § 3.7), the
  constraint language's strings in [`../../proposal/constraint-language.md`](../../proposal/constraint-language.md),
  and the Rust implementation in [`../../examples/y-string`](../../examples/y-string/).
