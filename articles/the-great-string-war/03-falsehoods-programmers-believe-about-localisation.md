# Falsehoods programmers believe about localisation

*Fifteen of them, and why most were out of a string's reach.*

Built at OZ, 1998–2002 · Written November 2026

---

> Part 3 of 11 in *The Great String War*. The falsehoods come from the genre, and the sources are linked. The
> verdicts are about GString, the string class I wrote at OZ, and are from memory.

Strings are the part of internationalisation that programmers notice. The rest of it is at least as bad, and it has
its own lists: Ben Hamill's
[*Falsehoods programmers believe about language*](http://garbled.benhamill.com/2017/04/18/falsehoods-programmers-believe-about-language)
and Lexiconista's [follow-up](https://www.lexiconista.com/falsehoods-about-languages/), Patrick McKenzie's
[*Falsehoods programmers believe about names*](https://www.kalzumeus.com/2010/06/17/falsehoods-programmers-believe-about-names/),
which started the genre, the W3C's [*Personal names around the world*](https://www.w3.org/International/questions/qa-personal-names),
and Noah Sussman's [*Falsehoods programmers believe about time*](https://infiniteundo.com/post/25326999628/falsehoods-programmers-believe-about-time).

GString was a string, not a localisation framework, so most of these were out of its reach. They are here because
every one of them became a string at some point: a message, a number, a date or a name on its way through the RPC
layer. The verdicts use the same words as part 2. **Solved** means GString got it right, **partly** as far as it
could, and **unsolved** not at all.

**Translation**

1. Translation means replacing the English strings. (Word order changes, so sentences assembled by concatenation
   can't be translated at all.) **Partly:** formatting with numbered arguments (`%1$s`) let a translator move them,
   but building a sentence with `+` was still easier.
2. Plurals come in two forms, one and many. (Arabic has six. Polish and Russian pick a form by the last digits of the
   number. In Icelandic, 21 takes the singular, because it ends in 1, but 11 doesn't. Unicode's CLDR has
   [the rules for every language](https://www.unicode.org/cldr/charts/latest/supplemental/language_plural_rules.html).)
   **Unsolved.**
3. A message doesn't need to know anyone's gender. **Unsolved.**

**Script and text**

4. Text runs left to right. (Arabic and Hebrew run right to left, with numbers and Latin names running left to right
   inside them, by the [bidirectional algorithm](https://www.unicode.org/reports/tr9/).) **Unsolved:** GString stored
   right-to-left text perfectly well, and knew nothing about it.
5. Words are separated by spaces. (Not in Chinese, Japanese or Thai.) **Unsolved:** counting words and wrapping them
   assumed spaces.

**Locale**

6. A locale is a language. A country has one language. A language belongs to one country. **Unsolved.**
7. The machine's locale is the user's locale. (On a server, the machine's locale is a configuration accident.)
   **Unsolved,** and built into GString's collation and case conversion.
8. Software runs in one country at a time. (What happens in Vegas stays in Vegas.) **Partly:** strings from every
   country could share a server, because each payload carried its encoding. The rest of this list still assumed one
   country.

**Numbers and dates**

9. The decimal separator is a dot. (In Iceland, one thousand and a half is `1.000,5`.) **Unsolved:** converting
   numbers used the process's locale.
10. Digits are grouped in threes. (In India, ten million is `1,00,00,000`.) **Unsolved,** for the same reason.
11. Everyone writes dates month first, or day first, and starts the week on the same day. **Unsolved.**

**Not a string's problem**

12. A translation is about as long as the original. (German grows; buttons don't.)
13. Everyone uses the same calendar.
14. Everyone has a first name and a family name. (Most Icelanders have a patronymic, and the phone book is sorted by
    first name.)
15. The server's time zone is the user's time zone.

## The score

GString got 2 of these 15 partly right and left 9 unsolved, and 4 were never a string's problem.

That's not a failure of GString so much as a description of what a string is. A string can carry a translated
message, a formatted number or a person's name. It can't know what language the message is in, which locale the
number was meant for, or which part of the name is the family name, unless somebody tells it. At OZ, nobody told it:
the locale came from the process, and on a server the process belonged to nobody in particular. Y's answer, in
*Every method you could want*, is that a locale is always passed and never inherited.
