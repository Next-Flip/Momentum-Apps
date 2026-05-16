#!/usr/bin/env python3
"""Regenerate `release-app.yml` app-slot dropdowns and `valid-appids.txt`.

Scans every `<repo>/<app>/application.fam` for EXTERNAL appids and rewrites:

  - `.github/valid-appids.txt` (sorted reference list)
  - every `app_N` choice's `options:` block in
    `.github/workflows/release-app.yml`, between sentinel comments:

        # >>> appids start
        - "(none)"
        - <appid_1>
        ...
        # <<< appids end

Run from the repo root whenever apps are added/removed:

    python3 .github/scripts/update_release_workflow.py
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from _fam_parser import collect_external_appids  # noqa: E402


SENTINEL_RE = re.compile(
    r"(?P<indent>[ \t]*)# >>> appids start.*?# <<< appids end",
    re.DOTALL,
)


def render_block(indent: str, appids: list[str]) -> str:
    lines = [f"{indent}# >>> appids start (managed by update_release_workflow.py)"]
    lines.append(f'{indent}- "(none)"')
    for appid in appids:
        lines.append(f"{indent}- {appid}")
    lines.append(f"{indent}# <<< appids end")
    return "\n".join(lines)


def main() -> int:
    repo_root = Path(__file__).resolve().parents[2]
    appids = collect_external_appids(repo_root)
    if not appids:
        print("error: no EXTERNAL appids found", file=sys.stderr)
        return 1

    txt_path = repo_root / ".github" / "valid-appids.txt"
    txt_path.write_text("\n".join(appids) + "\n")
    print(f"wrote {len(appids)} appids to {txt_path.relative_to(repo_root)}")

    yml_path = repo_root / ".github" / "workflows" / "release-app.yml"
    if not yml_path.exists():
        print(f"warning: {yml_path} not found; skipping YAML update",
              file=sys.stderr)
        return 0

    original = yml_path.read_text()
    matches = list(SENTINEL_RE.finditer(original))
    if not matches:
        print(f"error: no `# >>> appids start` sentinels in {yml_path}",
              file=sys.stderr)
        return 1

    def replace(match: re.Match) -> str:
        return render_block(match.group("indent"), appids)

    updated = SENTINEL_RE.sub(replace, original)
    if updated != original:
        yml_path.write_text(updated)
        print(f"rewrote {len(matches)} app-slot dropdown(s) "
              f"in {yml_path.relative_to(repo_root)}")
    else:
        print(f"{yml_path.relative_to(repo_root)} already up to date")
    return 0


if __name__ == "__main__":
    sys.exit(main())
