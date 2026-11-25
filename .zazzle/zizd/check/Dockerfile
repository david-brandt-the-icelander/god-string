# The `check` step of god-string's zizd pipeline: the headers as C++98 and C++17 with GCC and Clang, and the .dev
# diagrams and knowledge files against their schemas (AGENTS.md). Python and the validators' packages are pinned; GCC
# and Clang are Debian 13's (the base image's snapshot).
FROM python:3.13.9-slim
RUN apt-get update && apt-get install -y --no-install-recommends g++ clang && rm -rf /var/lib/apt/lists/* \
 && pip install --no-cache-dir jsonschema==4.26.0 referencing==0.37.0
COPY run.sh /run.sh
CMD ["bash", "/run.sh"]
