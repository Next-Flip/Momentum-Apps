#!/usr/bin/env python3
"""Regenerate `.github/valid-appids.txt`.

Scans every `<repo>/<app>/application.fam`, extracts every EXTERNAL `appid`,
and writes a sorted newline-separated list to `.github/valid-appids.txt`.

Run from the repo root:
    python3 .github/scripts/dump_appids.py
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from _fam_parser import collect_external_appids  # noqa: E402


def main() -> int:
    repo_root = Path(__file__).resolve().parents[2]
    out_path = repo_root / ".github" / "valid-appids.txt"

    appids = collect_external_appids(repo_root)
    if not appids:
        print("error: no EXTERNAL appids found", file=sys.stderr)
        return 1

    out_path.write_text("\n".join(appids) + "\n")
    print(f"wrote {len(appids)} appids to {out_path.relative_to(repo_root)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
