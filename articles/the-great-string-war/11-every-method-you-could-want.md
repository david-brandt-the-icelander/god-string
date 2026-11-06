# Every method you could want

*The CISC approach to strings, and what Y kept from GString.*

Built at OZ, 1998–2002 · Written November 2026

---

> Part 11 of 11 in *The Great String War*. GString's catalogue is reconstructed: I no longer remember the exact
> method names, so the names below say what the methods did. What this part says about Y is taken from Y's design
> documents as they stand today.

GString had a lot of methods. It was meant to. Buy-in meant that every faction's favourite feature had to be there,
so GString was the union of every string library in the building. This part is the catalogue, the argument for
having it, and what Y, the language I'm building now, kept.

## The catalogue

- **Making one.** From any string type in the building, either by copying it or by borrowing it. From numbers,
  booleans and dates. From a format string with numbered arguments (`%1$s`), so that a translator could move them
  around. By repeating a string, or by joining a list of them with a separator. From a null pointer, too: GString
  interpreted null as empty.
- **Making one for free.** A factory function took a `const char*` or a `const wchar_t*` pointing at a string compiled
  into the binary, and made a GString that borrowed it, with nothing to allocate, copy or free. *Borrowed, not
  copied* has the details.
- **Asking about it.** Whether it was empty, or blank (nothing but whitespace). Its length in bytes, in code units or
  in characters. Whether it started with, ended with or contained something, and how many times. How many words it
  had. Whether it was all letters, all digits, alphanumeric, numeric, upper case, lower case or ASCII.
- **Finding things.** The first or last occurrence of a substring, of a character, of any character from a set, or of
  the first character not in a set, from the start or from a given position. And searching by regular expression.
- **Taking pieces.** Left, right and mid. The first or last `n` characters. Everything before or after the first or
  last occurrence of something. The character at a position. Every one of them returned a GSubString, which shared the
  original's storage.
- **Changing it.** Append, prepend and insert. Replace the first, the last or every occurrence. Remove, splice and
  repeat. Reverse, character by character, keeping accents on their letters. Pad on the left, the right or both, to a
  width. Trim whitespace, or any set of characters, from either end or both. Truncate with an ellipsis, or prune at a
  word boundary. Wrap words at a width. Translate characters one for one, like Perl's `tr`.
- **Case.** Upper, lower, swapped, first letter capitalised, title case. And the programmers' cases, `camelCase`,
  `snake_case` and `kebab-case`, which code generators like mine needed all the time.
- **Splitting and joining.** Split by a separator or a regular expression, into words, into characters or into lines.
  Tokenise lazily, like `RWCTokenizer`, or like `strtok` without `strtok`'s hidden global state. Join it back.
- **Comparing.** Exactly, ignoring case, or by the locale's collation, as `CString::Collate` and C's `strcoll` did, each
  one a separate method. Hashing, exact or ignoring case.
- **Escaping and stripping.** Escape and unescape HTML and XML, since XML was going to save the world any day now.
  Escape a string for use inside a regular expression or in C source. Strip tags. Strip a byte-order mark. Strip the
  accents off letters, for URLs and file names.
- **Converting.** To and from every type in the building, always naming the encoding. To and from numbers. To the
  native type a function call needed, which GString then kept (*The Great String War* has the details).
- **Walking it.** Iterators, forwards and backwards, by byte, by code unit or by character. That is where the GSpot
  came from (*The myth of the wide character*).

Every one of them was somebody's favourite, and every one of them was a place where a caller didn't have to write
index arithmetic.

## CISC, not RISC

One of the important things you do to make strings fast is, ironically, also the lazy and aesthetically pleasing
thing: take a CISC approach to the interface of a string class rather than a RISC one.

The RISC approach says that `substr` is enough for left, right and substring. A programmer can just use `substr` and
figure out for themselves whether its second parameter is a length or a position, whether a zero-length substring has
its start equal to its end, and what funky thing should happen when an index is out of bounds. Or, God forbid, when
it's negative: Python supports that horror, where `s[-1]` is the last character, so an off-by-one at the start of a
string quietly becomes a read from its end. And, of course, remember that characters in any UTF have variable length.
Every caller makes those decisions again, and every caller has a chance to get them wrong.

The CISC approach says that every operation it is logical to want on a string should be there, under its own name.
Then three things happen:

- **The laziest call is the right one.** A programmer who reaches for the first method that does the job will
  usually find one that does exactly that job, and nothing else.
- **The library can optimise it.** A method that says what it means can be implemented in the fastest way for each
  representation. A pile of `substr` calls and index arithmetic can't.
- **The off-by-one bugs live in one place.** They get written once, in the library, and fixed once, instead of
  littering the code base and turning up in production.

GString was CISC because it had to be: every faction's favourite method had to be there, or that faction walked. Y
is CISC because I believe in it. Every method it is logical to want on an object such as a string or a blob should be
there.

## What Y kept

Y has been through a lot of versions since 2019, and strings have been an argument in most of them. One version could
not even make up its mind whether a rune was a code point or a grapheme, and its chapters disagreed with each other
about it. Here is how the string ideas in Y accumulated:

- **The 2024 design discussions** settled that a rune is a grapheme, that strings are immutable, that substrings share
  storage with the string they came from (the GSubString again, from *Borrowed, not copied*), and that bytes become
  text only through an explicit encoding. They also preferred separate methods to boolean flags:
  `date_to_long_string`, not `long_format: true`. That is CISC, stated as a style rule.
- **The C++ string library of late 2024** made text iterable by rune and hid code units from the library's users. It
  also gave lengths strong types, such as `length_in_bytes_t`, so that a length in bytes could not be passed where a
  length in characters was expected.
- **y-string, in March 2026,** made a rune exactly one extended grapheme cluster, normalised every string to NFC so
  that equality means canonical equivalence, typed every length and index by its unit, made locale explicit, and made
  lossy conversion explicit, rejecting invalid input by default.
- **The July 2026 conventions** said *always name the unit*: `length_in_bytes`, never `size`.

The current design keeps all of it, and moves the parts that used to be naming conventions into the type checker. Y's
standard library has a string library, y-string, with five terms:

| Term | Is |
| --- | --- |
| `string` | A sequence of runes |
| `rune` | One extended grapheme cluster: what a person sees as one character, such as `é` or `👍🏽` |
| `scalar` | One Unicode scalar value. A rune is one or more scalars |
| code unit | One unit of an encoding: a byte of UTF-8, a 16-bit unit of UTF-16 |
| `bytes` | A sequence of bytes, with no encoding |

**Counting names what it counts.** There is no single "length":

```y
name.count()                              -- a count<unit_type=rune>
name.scalars().count()                    -- how many scalars
name.code_units(encoding.utf16).count()   -- what NVARCHAR(50) limits
```

**A position is not a length.** A count of runes is a `count<unit_type=rune>`, and a position among them is an
`index<unit_type=rune>`. They are different types, and a plain integer doesn't silently become either. The compiler
knows which argument of a method is a position and which is a length, and a call that mixes them up doesn't compile.
An index among scalars can't be used among runes, and an index among runes can't be used on `bytes`. That is the
question `substr` makes every caller answer, answered once by the type system.

**Equal strings are equal.** Every string Y makes is normalised to NFC, including one made by joining two others,
where the joint may need it. So `é` typed as one code point and `é` typed as `e` and an accent are the same string,
hash the same and sort the same. Equality, hashing and ordering are derived by the compiler and can't be written by
hand, so they can't disagree with each other. GString's old trick, comparing lengths and hashes before characters, is
now the implementation's business, and the implementation is allowed to do it because equality means one thing.

**Order is for machines; collation is a function.** A string's own order is the order of its scalars: fast, and the
same on every machine, for keys and indexes. Ordering for people is collation, a named function that takes a
`locale`. Y's constraint language, which states rules about code in other languages, goes one step further and
writes a host's comparison down for what it is: `collate(x, 'Latin1_General_CI_AS')` gives a key that compares as
that collation does, and `sql_equals(a, b, collation)` also ignores trailing spaces, as T-SQL's `=` does.

**A string knows what it carries.** In Y, a string's character set is a constraint: `string<character_set=ascii>`
holds ASCII only. Each of its runes is one scalar and one byte, so a backend may store it as bytes and count and index
it in constant time, where a string of any runes has to walk its grapheme boundaries. An ASCII string is a `string`
without any conversion. That is GString's payload pointer that knew what it pointed to, turned into something the
compiler can check.

**Bytes are not text.** Converting between them names the encoding and what to do with invalid input, and rejecting
it is the default:

```y
text = string.decode_utf8(received, on_invalid: invalid_input.reject)
```

**Nothing is locale-sensitive by default.** Case folding, collation and formatting take a `locale` argument, and no
locale is ever inferred from the process. The server in Vegas does not get to decide how Reykjavík sorts.

**Rules about strings are types.** A refined type names its restrictions, and constructing it checks them:

```y
type email_address is string where has_one_at_sign = (value.count_of('@') == 1)
type word is string:
    where letters_only = value.matches(pattern"^[a-z]*$") and
          long_enough = (value.count() >= 4)
```

And regular expressions are the one place where Y does on purpose what GString only got away with:
*Crazier than Cocaine Bear* has that story.

## The war is over

GString ended the Great String War at OZ by making it not matter. Every faction kept its favourite string, GString
spoke all of them fluently, and nobody had to admit that anybody else had been right. What it couldn't do was decide
what a string *means*, because every faction already had an answer, and GString had to agree with all of them.

Y gets to do something I couldn't at OZ: decide what a string is before anyone has a favourite. A string is a
sequence of what people see as characters. Every count says what it counts. Bytes are not text. A locale is
something you pass, not something you inherit. And every method it is logical to want is there, so that the laziest
programmer in the room, who is usually me, picks the right one.
