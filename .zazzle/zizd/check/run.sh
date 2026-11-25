# Checks god-string as AGENTS.md says: src/gstring/all.hpp parses as C++98 and as C++17, with GCC and with Clang, and
# the .dev files pass both validators. Reads the repository in place (it writes nothing there); leaves nothing in
# /zazzle/output.
set -euo pipefail
export HOME=/tmp PYTHONDONTWRITEBYTECODE=1
src=$(ls -d /zazzle/input/repos/*/god-string | head -1)
cd "$src"
g++ --version | head -1; clang++ --version | head -1
for cxx in g++ clang++; do
  for std in c++98 c++17; do
    echo '#include "gstring/all.hpp"' | "$cxx" -x c++ -std=$std -DGSTRING_SYNTAX_CHECK -Wall -Wextra -pedantic -Isrc -fsyntax-only -
    echo "ok    the headers with $cxx -std=$std"
  done
done
python3 Meta/Agents/Skills/validate_diagrams.py
python3 Meta/Agents/Knowledge/validate_knowledge.py
echo "god-string checked"
