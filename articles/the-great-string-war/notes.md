# Notes: The Great String War

An eleven-part series in the Programming languages train, about GString, the string class David wrote at OZ between 1998 and 2002 to end the Great String War, and about strings in general. The text was written in the Y repository (padresmurfa/y PR #57, branch `claude/optimistic-darwin-tocku5`, `articles/the-great-string-war/`) and moved to david-brandt-the-icelander/icelander.net (`articles/the_great_string_war/`) on 2026-11-06, as drafts: `published` is empty, and each carries a `status` line. Only the articles live there. These notes, and the working copies of the parts in this folder, live in god-string.

| Part | Title | Slug |
| --- | --- | --- |
| 1 | The Great String War | `the-great-string-war` |
| 2 | Falsehoods programmers believe about strings | `falsehoods-programmers-believe-about-strings` |
| 3 | Falsehoods programmers believe about localization | `falsehoods-programmers-believe-about-localization` |
| 4 | Every string in the building | `every-string-in-the-building` |
| 5 | The myth of the wide character | `the-myth-of-the-wide-character` |
| 6 | Equal, equivalent, and in order | `equal-equivalent-and-in-order` |
| 7 | Shlemiel and the string list | `shlemiel-and-the-string-list` |
| 8 | Borrowed, not copied | `borrowed-not-copied` |
| 9 | Tricks of the string trade | `tricks-of-the-string-trade` |
| 10 | Crazier than Cocaine Bear | `crazier-than-cocaine-bear` |
| 11 | Every method you could want | `every-method-you-could-want` |

## To publish

Fill in `published` in each part, run `npm run check`, then `npm run urls:record`, and commit the articles with `site/url-ledger.json`. The parts link to each other by relative Markdown links, so the build refuses to publish one part while another is still a draft: publish all eleven together.

## What changed in the move

- The front matter is new: train, series and part, `thought: 1998` with `thought_until: 2002`, `written: 2026-11-06`, a summary per part, and an empty `published`. The summaries were written for publishing; the articles' text was not.
- The date line "Built at OZ, 1998–2002 · Written November 2026" and the rule under it are dropped from each body, because the page prints both dates itself.
- The opening note in each part ("Part N of 11 in The Great String War. … is memory. … is general knowledge, checked in 2026.") is dropped, as the equivalent notes were from The lowest common denominator on 2026-11-03. The distinction is kept below.
- Subtitles lose their trailing full stop, as on the rest of the site. Part 7's subtitle loses its backticks, because the page prints the subtitle as plain text.
- Spelling made US, per the style guide: normalization, localization (also in part 3's title, slug and file name), favorite, optimization, catalog, labor, humor, honor, flavor, unlabeled, capitalized, tokenize, serialization, synchronizing, globalization, internationalization. Links to Unicode's own pages are unaffected.
- "Spoiler:" in part 5 is gone, per the style guide.
- References between the parts, which were italic titles, are now relative links to the Markdown files, so the build checks them and readers can follow them.
- The series is registered in `site/series.yml`.
- Nothing else in the text was touched.

## What is memory and what is not

- **Memory:** the story of GString at OZ, on iPulse's RPC layer; the string types it had to interoperate with; the factory for literals compiled into the binary; the GSpot and the manager's remark about it; GRegularExpression and its limited backtracking; the GSubString and the GStringList; how a GString was built inside (payload pointer, borrowed or owned, null as empty, reference count, just-in-time length and hash, same-pointer comparison, conversion replacing a shared payload); no support for embedded nulls; no Unix support.
- **Assumed, for David to check:** the catalog of methods in part 11, taken as the union of the string libraries the series names plus Voca, and the verdicts on each falsehood in parts 2 and 3 that follow from it. Two inferred statements about GString: that `a = b + c + d + e + "f" + g` built a six-piece list and copied no characters (part 7), and that a GSubString held a reference to the original's payload so the original couldn't be freed under it (part 8).
- **General knowledge, checked in 2026, with sources linked in the parts:** the falsehood lists and their authors; the string types of the day and their quirks (BSTR's length prefix and the null-equals-empty convention, TCHAR, CString's GetBuffer, strstream's freeze, Java's modified UTF-8); the history of UCS-2, UCS-4, UTF-16 and UTF-8; the byte counts of the flag of England, the family emoji and the kiss; normalization, comparison and collation, the glibc 2.28 and PostgreSQL episode, strcoll and strxfrm; Shlemiel the painter, ropes, V8 cons strings, SpiderMonkey ropes, javac and JEP 280, .NET's chunked StringBuilder, Python's join, Erlang iolists and writev; Java's 2012 substring change, Go, Swift's Substring, string_view and Rust's &str; hash flooding in 2011, SipHash and JEP 180; Herb Sutter's 1999 measurements and the C++11 and GCC 2015 ABI change; the small string optimization; JEP 254 and PEP 393; Thompson's 1968 construction, Russ Cox's 2007 article, the Stack Overflow 2016 and Cloudflare 2019 outages, RE2 and Rust's regex crate; UTS #18.
- **Y's design documents:** the strings section of the Y proposal (§ 3.3), the string ideas of earlier iterations in the idea harvest, the constraint language's strings, and the Rust implementation in `examples/y-string`, all in padresmurfa/y.

## Material that is not David's, worth a look

- Part 4: the per-type quirks named above, and the paragraph on single-block allocations being the pattern GString chose.
- Part 7: the Shlemiel reference, the ropes and later-language section, and the closing list of advice.
- Part 8: the two questions, the Java, Go, Swift, C++ and Rust comparisons, and the closing list of advice.
- Part 9: interning, the hash-flooding history, why copy-on-write fell, the small string optimization, compact strings, and the closing "Measure" section.
- Part 10: automata versus backtracking, the two outages, and the "What Y does" section.
- The titles "Shlemiel and the string list", "Borrowed, not copied", "Tricks of the string trade", "Crazier than Cocaine Bear" and "Every method you could want".

## Set-ups and payoffs

| Set-up | Payoff |
| --- | --- |
| Part 1: the Great String War, with no consensus, policy or authority | Part 1 and part 11: "The war was never won. It ended when GString made it not matter." |
| Part 1: Perl's regular expressions "as crazy as Cocaine Bear" | Part 10's title, and GRegularExpression leaving Perl in the dust |
| Part 1: GString had to give every faction the answers its old class gave | Part 6 and part 11: that is why GString never normalized, and why Y gets to decide what a string means |
| Part 3 and part 4: "what happened on the server in Vegas stayed in Vegas" | Part 11: "The server in Vegas does not get to decide how Reykjavík sorts." |
| Part 7: the lazy path and the fast path pointed in different directions | Part 11: "the laziest call is the right one", and the laziest programmer in the room, "who is usually me" |
| Part 10: "Twenty-five years ago I could only make that trade by not mentioning it." | Part 11: "regular expressions are the one place where Y does on purpose what GString only got away with" |

## David's call

- Profanity: "screw up" (part 7), "sucked" (part 7), "gave a damn" (part 4), "goes to hell" (part 5). Mild, and all his.
- The manager's remark about the GSpot (part 5) names no one but is identifiable to anyone who was at OZ.
- "Apple was dead in those days, or so we all thought" (part 2) and "Apple, which we had all written off, came back" (part 6) say the same thing twice across two parts.
- Part 1 and part 9 both carry the conversion-collapse paragraph, and part 1 and part 10 both carry the Perl sentence. Deliberate, so that each part stands alone, but worth a glance.
- Links this environment could not reach to verify: GitHub (Voca, awesome-falsehood), Roy Fielding's dissertation page, cppreference. Every other link loaded, including the IMDb page for Cocaine Bear.

## Plain-text fallbacks

The tables in parts 5 and 6 (the flag of England counts, the glossary, and the comparison-versus-collation table) will need rewriting as lists on platforms that strip tables. The Y code blocks in part 11 use a `y` language tag that highlighters won't know; plain fenced blocks are fine there.
