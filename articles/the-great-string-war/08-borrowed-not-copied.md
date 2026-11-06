# Borrowed, not copied

*Substrings and literals that share storage, and the two questions every one of them has to answer.*

Built at OZ, 1998–2002 · Written November 2026

---

> Part 8 of 11 in *The Great String War*. The GSubString and the factory for literals are memory. What other
> languages did is general knowledge, checked in 2026.

The cheapest copy is the one you never make. Most strings in a program are pieces of other strings, or literals that
the compiler already put somewhere, and copying them is often pure waste. GString had two ways of not copying, and
both had to answer the same two questions.

## The GSubString

Some of the string classes in the building had efficient substrings: a substring held a reference to the original,
with an offset and a length, and so needed no allocation and no `memcpy`. `RWCSubString` was one. GString had one
too, the GSubString, and every method that took a piece of a GString returned one: left, right, mid, everything before
or after a separator, each token of a split.

That mattered more than it sounds. Parsing is mostly taking pieces: split a line into fields, trim each field, compare
it with something. With copying substrings, every one of those steps allocates. With borrowing ones, none of them
does, until somebody keeps a piece for longer than the line it came from.

## The factory for literals

The other kind of borrowing was for literals. A string literal in C++ is put in the binary's read-only data by the
compiler, and lives as long as the program does. Copying it into a string object just to read it is a waste, so
plenty of code in the building kept the raw pointer instead, and plenty of string classes copied it anyway.

GString had a factory function for exactly that. It took a `const char*` or a `const wchar_t*` pointing at a string
compiled into the binary, and made a GString that borrowed it: no allocation, no copy, and nothing to free, because
the data segment outlives everything. Its payload was marked *borrowed*, so GString knew never to free it. Literals
were the most common strings in the code base, and the factory made them cost nothing.

## The two questions

Every string that borrows its storage has to answer two questions.

**What if the original goes away first?** A borrowed piece of a string that has been freed points at memory that now
belongs to something else. The GSubString answered this with the reference count: it held a reference to its
original's payload, so the original couldn't be freed while a piece of it was still in use. The literal factory
answered it by only borrowing what lives forever. C++17's `std::string_view` doesn't answer it at all. A
`string_view` of a temporary string compiles without a word and points at freed memory on the next line. Rust answers
it at compile time: a `&str` carries a lifetime, and the compiler won't let it outlive what it borrows.

**What if the original is huge, and the piece is tiny?** A ten-byte substring of a ten-megabyte string keeps the ten
megabytes alive for as long as the ten bytes are in use. Java learned this the hard way. Its `substring` shared the
original's character array until 2012, when Java 7 update 6 changed it to copy, because programs were holding whole
files in memory through a few small substrings. Go's substrings still share, and have the same problem. Swift made the
trade-off visible in the type system: a slice of a `String` is a
[`Substring`](https://developer.apple.com/documentation/swift/substring), a different type, and the documentation
warns against keeping one for long. You have to convert it to a `String` on purpose, and that's when it copies.

## What I'd tell someone today

- **Borrow by default inside a function, own by default across one.** Pieces that live as long as the line being
  parsed should borrow. Pieces that get stored should copy, once, when they are stored.
- **Make the borrow visible.** A substring type that is distinct from a string, like Swift's `Substring` or Rust's
  `&str`, turns "this might keep something big alive" into something a reader can see.
- **Make the lifetime safe.** Either count references, as GString did, or check lifetimes when compiling, as Rust does.
  A borrowed view with neither, like `string_view`, is a use-after-free waiting for a temporary.
- **Literals are free.** Anything the compiler put in the binary can be borrowed for ever, and a string type that
  copies it is wasting work.

Y's design took one of these straight from GString: substrings share storage with the string they came from. Y has
value semantics, so whether a piece shares or copies is the compiler's choice, not the programmer's, and a piece can
never outlive what it shares. *Every method you could want* has the rest of what Y kept.
