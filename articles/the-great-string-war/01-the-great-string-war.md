# The Great String War

*How a string class nicknamed GString ended a war it couldn't win.*

Built at OZ, 1998–2002 · Written November 2026

---

> Part 1 of 11 in *The Great String War*. The story of GString is told from memory, a quarter of a century later.
> Where I'm not sure of a detail, I say so. The other parts of the series take GString apart piece by piece, and the
> last one says what Y, the language I'm building now, kept from it.

Strings are one of my obsessions. It started at OZ, on iPulse, and it has never really stopped.

I was implementing the RPC layer, which also did most of the code generation in the system. That meant my code had to
interoperate with everybody else's: different clients, different servers, and code that integrated third-party
components. Whatever anyone else had chosen to use, my code had to accept it, marshal it, send it across the wire
and hand it back in whatever form the other end wanted.

One of the most heated topics in our development team, which was large, was which string class was best.

There was no consensus. There was no policy. There was no authority that could cut the knot. Nobody gave an inch.

## The war

Since my code had to marshal strings to and from every format that everyone else used, I eventually gave up and made
yet another string class: one that would interoperate cleanly with all the others. That way the RPC layer, and the
framework in general, could stay out of the Great String War.

But I needed buy-in from the warring factions, and that set the requirements. The new string class had to do
everything that each faction's favourite could do, because people are sticklers about that. And it had to be, for
all practical purposes, as fast as if there had never been a second string class to deal with at all.

## The name

It was the God-String. That quickly got shortened to GString.

Yes, that was immature and dumb. It also had nothing to do with GLib's `GString`, which existed at the same time and
was a far more respectable piece of software. But it was good marketing among the geeks of OZ in the dot-bomb era,
and it helped win buy-in. It is hard to stay serious, and stubborn as a rock, in the face of humour, tasteless or
not.

Software developers called themselves programmers in those days. They took pride in being introverted, criminally
socially inept, utterly lacking in fashion sense, and driven by ego and an unshakable faith in their own superiority.
They had typically been bullied in school, and wore it as a scarred badge of honour. A joke in bad taste was the
closest thing to diplomacy that worked on them.

## What a GString was

Buy-in meant that every faction's favourite feature had to be there, so the simplest way to describe GString is as the
union of every string library in the building: Rogue Wave's `RWCString`, MFC's `CString`, `std::string`, the C
library, Java's `String`, and Perl. Perl is a vile
[write-only language](https://icelander.net/v1/programming-languages/write-only-languages), but its regular expressions
were as crazy as [*Cocaine Bear*](https://www.imdb.com/title/tt14209916/), and everyone secretly envied them.

That made for a lot of methods, and the full catalogue is in the last part of this series, *Every method you could
want*. A handful of pieces, each with a name as immature as the first, did the heavy lifting:

- **The payload.** A GString carried whatever native string it was given, and knew what it was carrying.
- **A factory for literals.** It took a `const char*` or a `const wchar_t*` pointing at a string compiled into the
  binary, and made a GString that borrowed it: no allocation, no copy, nothing to free.
- **The GSpot,** the position of an iterator in a GString, for walking strings whose characters had no fixed size.
- **The GSubString,** a piece of a GString that borrowed the original's storage instead of copying it.
- **The GStringList,** which made `a + b + c` cheap, and hid inside the GString itself.
- **GRegularExpression,** a superset of every regular-expression dialect in the building. Almost.

The rest of this part is about how they fitted together, and why that was enough to end the war.

## Inside a GString

- **A GString carried its payload as a pointer to whatever native string it had been given.** It knew what type the
  pointer pointed to, and whether the pointer was *borrowed*, as with a constant that would outlive the GString, or
  *owned*. It never freed what it had borrowed. A null pointer was simply an empty string.
- **It had a reference count,** so it was copy-on-write no matter what the underlying string was.
- **It calculated its length and hash just in time,** and kept them. That let it use some of the oldest tricks in the
  book of strings. Two GStrings that point at the same payload are equal. Two whose lengths or hashes differ are not.
  Neither check looks at a single character.
- **Every operation on two or more strings used the most efficient mechanism available.** This mattered most for
  adoption. A single operation that was not as fast as it could be was an OCD trigger for whoever spotted it, and
  their faction's buy-in was lost.

And here is the twist that made it all pay off. Say you need to turn a string into a `std::wstring`, because you have
to pass it to a standard library function. And say the string is made of twenty pieces in a string list: some of them
ASCII pointers, some `RWCString`s being abused as UTF-8 carriers, some two-byte wide-character pointers. There is no
way around converting the whole thing.

So GString did that, and then replaced its own payload with the result. The payload might be shared, copy-on-write,
with other GStrings, and they all got the result too. The twenty pieces were released, and the next conversion was
free. The cached length and hash survived, because the text hadn't changed, only its representation. And every GString
that shared the converted payload now pointed at the same value, so comparing any two of them came down to comparing
two pointers.

Comparing did the same. When two GStrings with different payloads turned out to be equal, character by character,
GString kept one payload and released the other, and every GString that had shared the released one got the one that
was kept. From then on, comparing any of them came down to comparing two pointers. If one of the two was a literal
compiled into the binary, the literal won. It cost nothing to keep and was never freed, so the allocated copy went,
and every GString that had held it now borrowed the literal.

The tricks themselves are older than GString, and most of them are still in use. *Tricks of the string trade*
collects them, along with the ones that came later.

GString's source is long gone. The [god-string](https://github.com/david-brandt-the-icelander/god-string/tree/main/src/gstring)
repository has a reconstruction of its headers, written in 2026 in the C++ of 1999: what GString might have looked
like, not what it was.

## The scorecard

Strings are full of things programmers believe that aren't true. The next two parts list them: 54 *Falsehoods
programmers believe about strings* and 15 *Falsehoods programmers believe about localisation*.

**What GString solved was representation, memory and speed:** what a string is made of, where it lives, who frees it, and
what it costs. That was GString's job, and it did it.

**What it left unsolved was meaning:** what a character is to the person reading it, when two strings are the same
text, what upper case is in Turkish, and whose alphabet a list is sorted in. There were two reasons for that.

The first was the state of the art. Unicode didn't define the extended grapheme cluster, what a reader sees as one
character, until 2008, and hardly any library in the building offered normalisation. GString couldn't be the union of
features that didn't exist yet.

The second was politics. GString had to give every faction the answers its old string class had given. A GString that
normalised, or counted graphemes, or folded case properly, would have disagreed with the class it replaced, and in a
war of string classes, every disagreement is a bug report against the newcomer.

## The war is over

The Great String War at OZ was never won. It ended when GString made it not matter. Every faction kept its favourite
string, GString spoke all of them fluently, and nobody had to admit that anybody else had been right.

That second reason, the politics, is the one Y doesn't have. Y gets to decide what a string means before anyone has a
favourite, and the last part of this series is about what it decided.

## The series

1. **The Great String War.** This part.
2. **Falsehoods programmers believe about strings,** and which of them GString solved.
3. **Falsehoods programmers believe about localisation,** most of which were out of a string's reach.
4. **Every string in the building.** The string types GString had to speak, and their quirks.
5. **The myth of the wide character.** UCS-2, UCS-4, code points, graphemes, and the GSpot.
6. **Equal, equivalent, and in order.** Normalisation, comparison and collation, which are three different things.
7. **Shlemiel and the string list.** Why `a + b + c` is slow, builders, and the GStringList.
8. **Borrowed, not copied.** Substrings and literals that share storage, and what that costs.
9. **Tricks of the string trade.** Cached lengths and hashes, early exits, copy-on-write, and their descendants.
10. **Crazier than Cocaine Bear.** Regular expressions, GRegularExpression, and backtracking.
11. **Every method you could want.** The CISC approach to strings, and what Y kept.
