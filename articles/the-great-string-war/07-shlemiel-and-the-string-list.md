# Shlemiel and the string list

*Why `a + b + c` is slow, why builders don't fix it, and the GStringList.*

Built at OZ, 1998–2002 · Written November 2026

---

> Part 7 of 11 in *The Great String War*. The GStringList is memory. The patterns that came after it are general
> knowledge, checked in 2026.

For all their claims that their string class was the best, programmers are generally lazy. It is even said that most
technological progress happens because somebody smart was too lazy to do manual labour. A programmer will spend a
month writing a tool that is used once a year and saves five minutes each time. That is the worst case, but you get
my drift.

## The plus sign

C++'s `<<` streaming paradigm did not become mainstream in later languages, so it is fairly safe to say the obvious:
it sucked. It also took two characters, `<<`, where `+` takes one. A string-builder pattern was even more verbose and
annoying. And most programmers think that their code is, obviously, great and fast. They don't look under the hood
unless they are dissing somebody else's code, and have no idea of the horrors that

```cpp
string a = b + c + d + e + "f" + g;
```

performs. Five new strings, one after another, each a fresh allocation filled with a `memcpy` of the one before it,
and all but the last freed again straight away. The characters of `b` are copied five times over before `a` exists at
all.

Put that in a loop, appending one piece at a time, and the cost grows with the square of the length. Joel Spolsky
named the pattern in [*Back to basics*](https://www.joelonsoftware.com/2001/12/11/back-to-basics/) after Shlemiel
the painter, who paints the lines on a road and gets slower every day, because he keeps walking back to the paint can
at the start. C's `strcat` is Shlemiel: it walks the whole string to find its end before every append.

## Builders, which nobody used

The textbook answer is a builder: a separate, growable buffer you append to, and turn into a string once at the end.
`strstream` was the builder of the day, and most of the string classes in the building came with something like it.
Some supported efficient appending on the string itself, by growing the buffer more than they needed, at the cost of
wasted memory. None had prepend or insert operations that were as fast as append, except through helper classes like
`strstream`.

And people didn't use them. A builder is a second class to learn, a second variable to declare, and a conversion to
remember at the end, all to do what `+` appears to do already. The lazy path and the fast path pointed in different
directions, and the lazy path won, every time.

## The GStringList

Enter the GStringList. It kept a list of the GStrings it was made of, and every operation worked on the list
directly: appending, prepending, length, *is empty*, comparison, iteration, indexing. All of them were a breeze for a
GString backed by a list. Prepending cost what appending cost, since both just added a piece.

I chose an internal string list rather than a builder, because people already had builders and (usually) didn't use
them. The list lived inside the GString, so `a = b + c + d + e + "f" + g` built a list of six pieces and copied no
characters at all. Lazy programmers could no longer screw up performance in that particular way, at least where
strings were concerned.

A list is not free forever. Sooner or later something needs the characters in one piece, such as a call into a
library that wants a `std::wstring`. Then GString converted the whole list once, replaced its payload with the
result, and kept the length and hash it had already computed. *The Great String War* has the details.

## The same idea, since

The GStringList was a flat cousin of the *rope*, which Hans Boehm, Russ Atkinson and Michael Plass described in
*Ropes: an alternative to strings* in 1995: a string kept as a tree of pieces, so that joining and slicing don't copy.
The idea has kept turning up since, usually where the people who designed a language noticed what lazy programmers
actually write.

- **JavaScript engines** make `+` cheap the same way. V8 builds a *cons string*, two pointers to the halves, and
  SpiderMonkey builds a *rope*, and both flatten it into one buffer only when something needs the characters.
- **Java's compiler** quietly turns `a + b + c` into a builder, a `StringBuffer` at first and a `StringBuilder` from
  Java 5, and since Java 9 into a call that works out the length first
  ([JEP 280](https://openjdk.org/jeps/280)). In a loop, though, it still makes a new builder every time round.
- **.NET's `StringBuilder`** has been a linked list of chunks since .NET 4, so it never copies what it already holds
  when it grows.
- **Python** tells you to collect the pieces in a list and `''.join()` them, and
  [says plainly](https://docs.python.org/3/library/stdtypes.html) that repeated concatenation is quadratic. CPython
  makes `s += t` fast when nothing else refers to `s`, which is an implementation detail you're told not to rely on.
- **Erlang** has *iolists*, nested lists of strings and bytes that a socket or a file
  [accepts as they are](https://www.erlang.org/doc/system/eff_guide_functions.html), and Unix has `writev`, which
  writes several buffers in one call. The fastest concatenation is the one you never do, because the destination
  could take the pieces.

## What I'd tell someone today

- **Make the lazy path the fast one.** A builder that people have to remember to use is a builder they won't use. Put
  the cleverness behind `+`.
- **Prepend and insert matter too.** Code that wraps a string in a prefix and a suffix is as common as code that
  appends. A list of pieces makes all three cheap.
- **Flatten once, and keep the result.** When the pieces have to become one buffer, do it once, and let every later
  use share it.
- **Don't flatten at all if you can help it.** If the next stop is a socket, a file or a hash, feed it the pieces.
- **Mind the joints.** Two normalised strings, joined, aren't always a normalised string, and two valid halves of a
  UTF-8 string aren't always valid when you cut them apart. *Equal, equivalent, and in order* has the details.
- **For anything a person will read, format, don't concatenate.** A translator can reorder `%1$s` and `%2$s`, and can't
  reorder `a + b`. *Falsehoods programmers believe about localisation* has more.
