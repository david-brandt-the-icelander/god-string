# The myth of the wide character

*UCS-2, UCS-4, code points, graphemes, and the GSpot.*

Built at OZ, 1998–2002 · Written November 2026

---

> Part 5 of 11 in *The Great String War*. The history of the encodings and the byte counts are general knowledge,
> checked in 2026. The GSpot, and what my manager said about it, are memory.

Two of the string types GString had to speak, the two-byte wide string and the four-byte one that we were lucky enough
not to need, are why so many programmers still believe that a character has a fixed size. It doesn't, and it never
really did.

## UCS-2

Unicode began in 1991 as a 16-bit character set: 65,536 characters, which was supposed to be enough for every script
in use. Its encoding was UCS-2, two bytes per character, fixed width, no exceptions. Windows NT (1993) and Java (1995)
were built on it, and so were Windows' two-byte `wchar_t`, `OLECHAR` and `BSTR`. For a few years a wide character
really was a code point, and if you didn't look too closely at combining accents, it was even a character.

Meanwhile ISO had its own universal character set, ISO 10646, with a 31-bit code space and a four-byte encoding:
UCS-4. Unicode and ISO 10646 merged their repertoires early on, but kept their own encodings.

## UTF-16

In 1996, [Unicode 2.0](https://www.unicode.org/versions/Unicode2.0.0/) admitted that 65,536 was not enough. The code
space grew to 0x10FFFF, and UCS-2 became UTF-16 ([RFC 2781](https://www.rfc-editor.org/rfc/rfc2781)): the same
two-byte units, plus *surrogate pairs*, two units taken from a reserved range that together stand for one code point
above 0xFFFF.

Everything built on UCS-2 kept working, which was the point. It also kept believing that one unit was one character,
which was the problem. Code written for UCS-2 handles UTF-16 perfectly well until the first character outside the old
16-bit range arrives, and then it counts that character twice, cuts it in half, or reverses the halves. Java only
learned about surrogates in 2004, and its `char` and JavaScript's `length` count UTF-16 code units to this day. On
Windows, UCS-2 and UTF-16 were both a `wchar_t*`, and nothing in the type told them apart.

So when GString was handed a wide string, it read it as UTF-16, surrogates and all, which is also safe for UCS-2. It
couldn't fix the UCS-2 code on the other end of the wire, but at least it didn't add to it.

## UCS-4 and UTF-32

UCS-4 went the other way. Unix made `wchar_t` four bytes, on the theory that four bytes can hold any Unicode character.
In 2003 both standards capped the code space at 0x10FFFF, UTF-8 was cut from up to six bytes per code point
([RFC 2279](https://www.rfc-editor.org/rfc/rfc2279)) to at most four
([RFC 3629](https://www.rfc-editor.org/rfc/rfc3629)), and UCS-4 restricted to that range is what we now call UTF-32.
So four bytes hold any Unicode *code point*. Spoiler: they still don't hold any Unicode *character*, which is not the
same thing, as we'll see in a moment.

We didn't support Unix, thankfully, so its four-byte `wchar_t` never reached GString.

## X marks the spot

One of the things every faction demanded of GString was a way to walk through a string.

Some implementations (looking at you, `std::*string`) offered iterators to walk the characters of a string. Some
allowed direct indexing. Direct indexing goes to hell in any UTF scheme, because characters there have variable
length.

How variable? Here is the flag of England, which people actually send each other:

| 🏴󠁧󠁢󠁥󠁮󠁧󠁿 counted as | Count |
| --- | --- |
| What a person sees | 1 |
| Unicode code points | 7 |
| UTF-16 code units | 14 |
| UTF-8 bytes | 28 |

A family emoji, man, woman, girl and boy joined by zero-width joiners, is one character on screen, seven code points
and 25 bytes of UTF-8. Add skin tones to a couple kissing and you are at 35 bytes. And there is no upper bound in
principle: combining marks can be stacked on a letter for as long as you like. Unicode's own *stream-safe* text format
([UAX #15](https://www.unicode.org/reports/tr15/)) caps a run of them at 30, which, depending on the marks, is already
around 60 to 90 bytes for a single character.

But with Unicode, was that "character" a code point, a code unit, a character, a grapheme, a grapheme cluster, a
glyph, or what? For some strange (sarcasm intended) reason, most programmers never bothered to learn the difference.
Those who did couldn't keep it straight in their heads for more than an hour anyway. So here it is, one more time:

| Term | What it is |
| --- | --- |
| Byte | Today, eight bits, though C and C++ still don't promise it. Historically, a machine's unit for a character, which has been 6, 7, 8 or 9 bits. Not a character, ever, unless you are sure the text is ASCII. |
| Code unit | One unit of an encoding: a byte of UTF-8, a 16-bit unit of UTF-16, a 32-bit unit of UTF-32. |
| Code point | A number in Unicode's code space, from 0 to 0x10FFFF. Some of them are not characters at all. |
| Scalar value | A code point that is not a surrogate: what UTF-32 can actually hold, and what Rust's `char` is. |
| Grapheme cluster | What a person reading the text sees as one character: `é`, `👍🏽`, 🏴󠁧󠁢󠁥󠁮󠁧󠁿. One or more scalars. |
| Glyph | A shape drawn by a font. A font may draw `f` and `i` as one ligature glyph, and some scripts draw one character with several glyphs. |

Naturally, all of this led to the introduction of the even more immature GSpot: the position of an iterator in a
GString. After a particularly gnarly off-by-one bug involving the GSpot, my manager told me that this was why she
didn't date nerds.

The GSpot walked by code point. It stepped over a surrogate pair as one character, and never split a UTF-8 sequence.
It did not walk by grapheme cluster, because Unicode only defined the extended grapheme cluster, what a reader sees as
one character, in 2008 ([UAX #29](https://www.unicode.org/reports/tr29/)). In 2000 the question barely came up, since
emoji weren't in Unicode yet, and the flag of England arrived much later still.

For the fixed-width payloads, such as `std::string` and `const char*`, a GSpot could jump straight to a position, and
indexing took constant time. For the variable-width ones, it had to walk. The GSpot made walking the honest
operation, and the rest of GString's methods meant most code never needed a position at all. *Every method you could
want* is about that.

## Further reading

- Joel Spolsky,
  [*The absolute minimum every software developer absolutely, positively must know about Unicode*](https://www.joelonsoftware.com/2003/10/08/the-absolute-minimum-every-software-developer-absolutely-positively-must-know-about-unicode-and-character-sets-no-excuses/),
  2003.
- Manish Goregaokar,
  [*Let's stop ascribing meaning to code points*](https://manishearth.github.io/blog/2017/01/14/stop-ascribing-meaning-to-unicode-code-points/),
  2017.
- Unicode, [UAX #29: Unicode text segmentation](https://www.unicode.org/reports/tr29/), which defines grapheme
  clusters.
