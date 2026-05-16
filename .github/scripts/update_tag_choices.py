#!/usr/bin/env python3
"""Refresh the `tag` dropdown in `release-app.yml`.

Reads recent release tags from stdin (one per line — typically piped from
`gh api repos/<owner>/<repo>/releases --paginate -q '.[].tag_name'`) and
rewrites the `tag` choice's `options:` block in
`.github/workflows/release-app.yml`, between sentinel comments:

    # >>> tags start
    - new
    - <tag_1>
    ...
    # <<< tags end

`new` is always the first option. Tags are written in the order received
(callers should sort by recency); duplicates are removed.

Usage in CI:

    gh api repos/${{ github.repository }}/releases --paginate \\
        -q '.[].tag_name' \\
      | python3 .github/scripts/update_tag_choices.py --keep 15
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path


SENTINEL_RE = re.compile(
    r"(?P<indent>[ \t]*)# >>> tags start.*?# <<< tags end",
    re.DOTALL,
)


def render_block(indent: str, tags: list[str]) -> str:
    lines = [f"{indent}# >>> tags start (managed by update_tag_choices.py)"]
    lines.append(f"{indent}- new")
    for tag in tags:
        lines.append(f'{indent}- "{tag}"')
    lines.append(f"{indent}# <<< tags end")
    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--workflow", type=Path,
                        default=Path(".github/workflows/release-app.yml"))
    parser.add_argument("--keep", type=int, default=15,
                        help="maximum number of recent tags to show")
    args = parser.parse_args()

    seen: set[str] = set()
    ordered: list[str] = []
    for line in sys.stdin:
        tag = line.strip()
        if tag and tag not in seen:
            seen.add(tag)
            ordered.append(tag)
        if len(ordered) >= args.keep:
            break

    if not args.workflow.exists():
        print(f"error: {args.workflow} not found", file=sys.stderr)
        return 1

    original = args.workflow.read_text()
    match = SENTINEL_RE.search(original)
    if not match:
        print(f"error: no `# >>> tags start` sentinel in {args.workflow}",
              file=sys.stderr)
        return 1

    new_block = render_block(match.group("indent"), ordered)
    updated = original[:match.start()] + new_block + original[match.end():]
    if updated != original:
        args.workflow.write_text(updated)
        print(f"rewrote tag dropdown with {len(ordered)} tag(s)")
    else:
        print("tag dropdown already up to date")
    return 0


if __name__ == "__main__":
    sys.exit(main())
