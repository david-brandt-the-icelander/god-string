# `.zazzle/zizd/`: god-string's zizd pipeline

The steps zazzle's builders run for this repository, written against zazzle's step-pipeline spec
(`src/macos/zazzle-pipeline/pipeline-spec.md`, sections 1–4). One pipeline, `default`, with one step, so the step is
the folder `.zazzle/zizd/check/` with a `Dockerfile` and a `run.sh`. The pipeline file itself lives in the orchestrator
(`continuous_integration_and_delivery`, `.zazzle/zizd/pipelines/david-brandt-the-icelander/god-string/default.json`);
the copy below is what goes there.

| Step | Phase | Image | What it does | Leaves in `/zazzle/output` |
|---|---|---|---|---|
| `check` | build | `python:3.13.9-slim` (Debian 13: GCC 14, Clang 19), jsonschema 4.26.0 | the check `AGENTS.md` asks for after changing the headers: `src/gstring/all.hpp` with `-fsyntax-only` as C++98 and C++17, with GCC and Clang; then both `.dev` validators | nothing |

There is no build and no deployment: the headers declare an interface and implement nothing, and the articles are
published from the icelander.net repository, not from here.

## The pipeline file

```json
{
  "id": "david-brandt-the-icelander/god-string:default",
  "repos": ["https://github.com/david-brandt-the-icelander/god-string.git"],
  "paths": ["src/**", "**/.dev/**", "Actors/**", "Meta/**", ".zazzle/zizd/**"],
  "steps": [
    { "name": "check", "path": ".zazzle/zizd/check" }
  ],
  "supersede": true
}
```

`paths`: a change to the articles' text alone checks nothing, so only the headers, the `.dev` files (the articles'
included), the actors, the validators and this folder start the pipeline.

## Config keys

None. The repository is public, and the step needs no key. The builder's own `GITHUB_TOKEN` and z3 keys
(`shared.json`) are zazzle's and reach no step.

## Trying it by hand

Run with the spec's "Trying a step by hand" recipe: green (all four compiler runs, 9 schemas, 16 knowledge files).

## What goes at cutover

Nothing: the repository has no workflow and no legacy runner.
