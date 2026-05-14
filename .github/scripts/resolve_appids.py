#!/usr/bin/env python3
"""Parse and validate the comma/space-separated `apps` workflow input.

Reads the user-supplied list from `--input`, validates each entry against the
EXTERNAL appids discovered by scanning `<apps-root>/*/application.fam`, and
writes two variables to the file given by `--out-env` (intended to be
`$GITHUB_ENV`):

    RESOLVED_APPIDS=<space-separated list>
    RESOLVED_APPIDS_MD<<EOF
    - appid_one
    - appid_two
    EOF

On any invalid appid, prints the offenders (with `difflib` suggestions) and
exits non-zero before any build runs.
"""

from __future__ import annotations

import argparse
import difflib
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from _fam_parser import collect_external_appids  # noqa: E402


SPLIT_RE = re.compile(r"[\s,]+")


def parse_input(raw: str) -> list[str]:
    seen: set[str] = set()
    ordered: list[str] = []
    for token in SPLIT_RE.split(raw.strip()):
        if not token:
            continue
        if token not in seen:
            seen.add(token)
            ordered.append(token)
    return ordered


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", required=True, help="raw user input")
    parser.add_argument("--apps-root", required=True, type=Path)
    parser.add_argument("--out-env", required=True, type=Path)
    args = parser.parse_args()

    requested = parse_input(args.input)
    if not requested:
        print("error: no appids supplied", file=sys.stderr)
        return 1

    valid = set(collect_external_appids(args.apps_root))
    if not valid:
        print(f"error: no EXTERNAL appids found under {args.apps_root}",
              file=sys.stderr)
        return 1

    invalid = [a for a in requested if a not in valid]
    if invalid:
        print(f"error: {len(invalid)} unknown appid(s):", file=sys.stderr)
        for bad in invalid:
            hints = difflib.get_close_matches(bad, valid, n=3, cutoff=0.6)
            suggestion = f"  (did you mean: {', '.join(hints)}?)" if hints else ""
            print(f"  - {bad}{suggestion}", file=sys.stderr)
        return 1

    md_lines = "\n".join(f"- {a}" for a in requested)
    with args.out_env.open("a") as f:
        f.write(f"RESOLVED_APPIDS={' '.join(requested)}\n")
        f.write("RESOLVED_APPIDS_MD<<EOF_APPIDS_MD\n")
        f.write(md_lines + "\n")
        f.write("EOF_APPIDS_MD\n")

    print(f"resolved {len(requested)} appid(s): {' '.join(requested)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
