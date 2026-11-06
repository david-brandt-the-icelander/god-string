# Crazier than Cocaine Bear

*Regular expressions, GRegularExpression, and the corner I cut.*

Built at OZ, 1998–2002 · Written November 2026

---

> Part 10 of 11 in *The Great String War*. GRegularExpression is memory. The history of regular expressions and the
> outages are general knowledge, checked in 2026.

Depending on the fashion of the day, various parts of the code base at OZ used regular expressions. And, of course,
the syntax varied from library to library: POSIX's basic and extended expressions, Rogue Wave's, and whatever Perl
had added that year. Each library had its own strengths and weaknesses, and nobody was willing to give up a strength.

Perl was the one everyone measured against. It is a vile
[write-only language](https://icelander.net/v1/programming-languages/write-only-languages), but its regular expressions
were as crazy as [*Cocaine Bear*](https://www.imdb.com/title/tt14209916/), and everyone secretly envied them.

## GRegularExpression

So, lo and behold, GString got GRegularExpression. I had run out of immature ideas by the time I coined that name. To
get buy-in, it had to be a superset of every other expression library's capabilities. It was perhaps the only
expression library I have ever seen that left Perl and obfuscated C in the dust.

There was one corner I cut. GRegularExpression supported backtracking only to a limited degree. Nobody in the code
base was using the kind of expression that needed more, so I got away with it.

## Why that corner mattered

There are two ways to run a regular expression, and they have been known since the 1960s.

One is to turn the expression into an automaton, as Ken Thompson did in 1968, and run the input through it once. It
takes time in proportion to the length of the input, whatever the expression. The other is to try one way of matching,
and when it fails, *backtrack* and try the next. Backtracking is what makes backreferences possible, such as `(a+)\1`,
which matches a run of `a`s followed by the same run again. It is also what makes some expressions take time that
grows exponentially with the input. Russ Cox explained both, and why the world mostly picked the wrong one, in
[*Regular expression matching can be simple and fast*](https://swtch.com/~rsc/regexp/regexp1.html) in 2007.

The wrong one has a body count:

- **Stack Overflow, 2016.** A post containing about 20,000 consecutive whitespace characters met an expression meant
  to trim whitespace, and a backtracking engine
  [took the site down](https://stackstatus.tumblr.com/post/147710624694/outage-postmortem-july-20-2016) for 34 minutes.
- **Cloudflare, 2019.** One new rule in its firewall, with `.*(?:.*=.*)` in it,
  [pinned every CPU](https://blog.cloudflare.com/details-of-the-cloudflare-outage-on-july-2-2019/) handling HTTP
  traffic across its network.

Google wrote RE2 to run untrusted expressions in linear time, at the price of backreferences, and Rust's widely used
`regex` crate made the same trade. GRegularExpression had made it by accident, years earlier, and nobody noticed,
because nobody needed more.

## Characters, again

Regular expressions have their own falsehoods about characters, and *Falsehoods programmers believe about strings*
lists them:

- `.` matches a code point, or in some engines a UTF-16 code unit, but not what a reader sees as one character.
- `[a-z]` doesn't match `á`, `ð`, `þ`, `æ` or `ö`, which are all lowercase letters.
- `\w` matches only ASCII in many engines.

GRegularExpression matched by code point, through the GSpot, and had Unicode's letter classes for anyone who used them.
Unicode's own guidance for regular expressions,
[UTS #18](https://www.unicode.org/reports/tr18/), came later, and says how an engine should handle all three.

## What Y does

Y does on purpose what GRegularExpression got away with. A `pattern"…"` literal is checked when the program is
compiled, so a malformed expression never reaches production. And Y's constraint language, which states rules about
code written in other languages, accepts linear-time regular expressions only. A pattern in it cannot take a server
down, whatever input it is given, and the price is the backreferences and lookarounds that make backtracking
necessary. Twenty-five years ago I could only make that trade by not mentioning it. Today I think it is the right one.
