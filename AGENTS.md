# Agent instructions

god-string is a recreation of the god-string used in iPulse: *The Great String War*, a series of articles about
GString, in `articles/the-great-string-war/`, and a speculative reconstruction of GString's headers, in `src/gstring/`.
`README.md` is the page a person reads first.

- **The articles are working copies.** The published parts are in the icelander.net repository, under
  `articles/the_great_string_war/`, and have been edited there since they were copied; see the open decision
  `working-copies` in `articles/the-great-string-war/.dev/decisions.json` before changing a part.
- **The headers are a reconstruction, and say so.** Every header opens by saying it is speculative, and marks what is
  memory and what is invented. Keep it that way: do not present an invented name or rule as the original. They
  declare an interface and implement none of it, in the C++98 and Win32 style of 1999.
- **Check the headers** after changing them, from the repository root, as C++98 and as C++17, with GCC and with Clang:

  ```bash
  echo '#include "gstring/all.hpp"' | g++ -x c++ -std=c++98 -DGSTRING_SYNTAX_CHECK -Wall -Wextra -pedantic -Isrc -fsyntax-only -
  ```

## Architecture diagrams and knowledge files

Each maintained folder documents itself in a `.dev/` directory next to its contents, as JSON following the skills in
`Meta/Agents/Skills/` and the knowledge catalogue in `Meta/Agents/Knowledge/` (human-assisted software development).
Read a folder's `.dev` files before changing it. `populate-dev` decides what a folder needs; the diagrams are:

- `describe-component-diagram`: the folder is a cohesive capability, adapter, boundary or subsystem (the repository
  root, and `src/gstring`).
- `describe-class-diagram`: the folder itself declares more than one class (`src/gstring`).
- `describe-state-diagram`: the folder implements a state machine, one `<Identifier>.state-diagram.json` per machine,
  named after its class. None does today: the headers implement nothing.
- `describe-deployment-diagram`: the folder holds something independently deployable. None does today: nothing here
  is built or deployed.

What a folder does, offers and exposes goes in its use cases, user journeys, capabilities, features and operations
(`describe-use-case`, `describe-user-journey`, `describe-capability`, `describe-feature`, `describe-operation`); today
that is the reader's journey through the series, in `articles/the-great-string-war`. The headers get none, because
they declare behavior and implement none of it. What a folder intends, promises and needs (requirements, backlog
items, decisions, blockers, contracts, verification, limitations and the rest) goes in knowledge files, written with
`describe-knowledge`. Wanted work is a `backlog` item in the folder that will own it, and a question only the owner
can answer is a `decisions` record with `status: open`; the articles' open questions for their author are in
`articles/the-great-string-war/.dev/decisions.json`.

Every actor a use case, journey or diagram names has a file in `Actors/` at the repository root, written with
`describe-actor` and linked from whatever names it; delete an actor nothing links to. Update the `.dev` files and
actors in the same commit as the code or text they describe, and delete one that stops applying. Before finishing,
run both validators from the repository root, with the `jsonschema` package installed:

```bash
python3 Meta/Agents/Skills/validate_diagrams.py
python3 Meta/Agents/Knowledge/validate_knowledge.py
```

The skills and the catalogue come from
<https://github.com/david-brandt-the-icelander/human-assisted-software-development> (`Agents/Skills` and
`Agents/Knowledge`). Change them there and copy them here unchanged, so there is one version.
