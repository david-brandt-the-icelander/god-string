# Tricks of the string trade

*Stored lengths, cached hashes, early exits, copy-on-write, and what became of them.*

Built at OZ, 1998–2002 · Written November 2026

---

> Part 9 of 11 in *The Great String War*. Which tricks GString used is memory. The rest is general knowledge, checked
> in 2026.

GString had to outperform every string class in the building to secure buy-in, which meant knowing exactly where each
of them was slow, and using every trick in the book. Most of the tricks are older than GString, and most are still in
use, under other names.

## Store the length

A C string doesn't know its length. `strlen` walks the whole string looking for the terminator, every time anyone
asks, and code that asks in a loop condition walks it once per iteration. Some of the string classes in the building
stored the length; some scanned for the terminator every time.

GString computed the length the first time anyone asked, and kept it. Storing it also meant nothing ever needed a
terminator to know where the string ended, though GString never went as far as allowing null characters inside a
string.

## Cache the hash

Some of the string classes had hash functions, and some didn't. Some stored the hash once it was computed (if I
remember correctly), and some recalculated it every time. When people had the great idea of using the strings that
didn't store their hash as keys in a hash dictionary, or in other collections like it, that really bit: every lookup
hashed the whole key again. Java's `String` caches its hash in a field for exactly that reason. GString computed its
hash the first time anyone asked, and kept it.

A cached hash has a catch that came later. In 2011, researchers showed that an attacker who knows a server's string
hash can send thousands of keys that all hash alike, and turn every hash table lookup into a linear search
([oCERT-2011-003](https://ocert.org/advisories/ocert-2011-003.html)). Most languages answered by seeding their hash
with a random number per process, and many moved to [SipHash](https://131002.net/siphash/). Java kept its predictable
`String` hash and, in Java 8, made `HashMap` turn crowded buckets into trees instead
([JEP 180](https://openjdk.org/jeps/180)).

## Exit early

With the length and the hash at hand, the oldest tricks in the book of strings become cheap. In order of cost:

1. **The same pointer.** Two strings that point at the same storage are equal, without looking any further.
2. **Different lengths.** Two strings of different lengths are not equal.
3. **Different hashes.** Two strings with different hashes are not equal.
4. **Only then, the characters.**

None of the first three needs to look at a single character. GString did all of them. The third has a condition that is
easy to miss: the hash has to agree with the comparison. A hash of the exact characters says nothing about a
comparison that ignores case, which is why GString's case-insensitive comparison had a hash of its own. *Equal,
equivalent, and in order* has more on that.

## Intern

Taken to its end, the same-pointer trick becomes *interning*: keep one copy of each distinct string, and hand out
pointers to it, so that equal strings are always the same pointer and comparing them costs one instruction. Lisp's
symbols work this way, as do Java's string literals and `String.intern()`, and Python's `sys.intern`. It pays for
strings that are compared far more often than they are made, such as identifiers and keys.

## Copy-on-write, and why it fell

Sharing a string instead of copying it, and copying the characters only when one of the copies is about to change,
made copies nearly free. `RWCString`, MFC's `CString`, several `std::string` implementations and GString all did it,
with a reference count.

Then threads arrived in earnest, and the reference count had to be changed atomically, every time a string was copied
or released. Herb Sutter measured what that cost in
[*Optimizations that aren't (in a multithreaded world)*](http://www.gotw.ca/publications/optimizations.htm) in 1999,
and the answer was: often more than the copies it saved. C++11 changed the rules for `std::string` in a way that
ruled copy-on-write out, and GCC introduced a new library ABI in 2015 to comply.

## Small strings live inside

What replaced copy-on-write in C++ was the *small string optimisation*: a string object is a few words big anyway, so a
short string can live inside the object itself, with no allocation at all. Most strings are short, so most strings
never allocate. Microsoft's, LLVM's and GNU's standard libraries all do it today.

## Pick the narrowest representation

A string that only holds ASCII or Latin-1 doesn't need two bytes per character. Java 9's *compact strings*
([JEP 254](https://openjdk.org/jeps/254)) store such strings in one byte per character and switch to two only when
they have to. Python 3.3 went further ([PEP 393](https://peps.python.org/pep-0393/)), and picks one, two or four bytes
per character for each string, by its widest character.

That is GString's payload, made automatic. A GString knew what it carried, and a fixed-width payload such as a
`const char*` could be indexed in constant time while a variable-width one had to be walked. The modern versions
choose the representation themselves. GString had to take whatever the faction that made the string had chosen.

## Convert once, compare once, then share

GString's own twist: when a string had to be converted, because some library function wanted a `std::wstring`, it was
converted once and the result replaced the payload. Every GString that shared the payload got the converted version.
The cached length and hash survived, because the text hadn't changed, only its representation. And every GString that
shared the payload now pointed at the same value, so comparing any two of them came down to comparing two pointers.

Comparing did the same. When two GStrings with different payloads turned out to be equal, character by character, they
were left sharing one of the payloads, and every GString that had shared the other got it too. If one of the two was a
literal in the binary, the literal won, and the allocated copy was released. It was interning without the table: equal
strings ended up as the same pointer, but only the ones that had actually been compared, and only after the first
comparison had paid for it.

## Borrow what lives forever

Literals in the binary are free to borrow, and copying them is pure waste. *Borrowed, not copied* covers that, and the
substrings that borrow too.

## Measure

None of these tricks is free everywhere. Copy-on-write was a win until threads made it a loss. A cached hash is a win
until it is a target. Interning saves comparisons and costs a table lookup at every creation. At OZ, every faction
measured, because every faction was looking for a reason to reject GString, and any operation that was slower than the
string class it replaced was that reason. That turned out to be the best performance discipline I've ever worked
under.
