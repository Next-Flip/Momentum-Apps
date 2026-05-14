"""Shared application.fam parsing helpers.

`application.fam` files are Python snippets that call `App(...)` (and
occasionally `Lib(...)`) with keyword arguments. They reference
`FlipperAppType.EXTERNAL` / `PLUGIN` / `METAPACKAGE` as enum-like sentinels.
We mimic fbt's loading strategy: exec the file in a sandbox where `App` is a
recorder and the referenced symbols are stub sentinels. This sidesteps
`fap_version`'s inconsistent shapes (string / tuple / list / multi-line /
missing in some apps) because we never read fap_version here.
"""

from __future__ import annotations

import sys
from pathlib import Path
from types import SimpleNamespace


APPTYPES = ("EXTERNAL", "PLUGIN", "METAPACKAGE", "SYSTEM", "SERVICE",
            "APP", "DEBUG", "ARCHIVE", "SETTINGS", "STARTUP", "MENUEXTERNAL")


def _make_sandbox():
    apps: list[dict] = []

    def App(**kwargs):
        apps.append(kwargs)

    def Lib(**kwargs):
        return SimpleNamespace(**kwargs)

    def ExtFile(**kwargs):
        return SimpleNamespace(**kwargs)

    flipper_app_type = SimpleNamespace(**{name: name for name in APPTYPES})

    sandbox = {
        "App": App,
        "Lib": Lib,
        "ExtFile": ExtFile,
        "FlipperAppType": flipper_app_type,
    }
    return sandbox, apps


def parse_fam(path: Path) -> list[dict]:
    """Return every `App(...)` call recorded from `path`, as kwarg dicts."""
    sandbox, apps = _make_sandbox()
    source = path.read_text()
    try:
        exec(compile(source, str(path), "exec"), sandbox)
    except Exception as exc:
        raise RuntimeError(f"Failed to parse {path}: {exc}") from exc
    return apps


def collect_external_appids(apps_root: Path) -> list[str]:
    """Return a sorted list of every EXTERNAL appid under `apps_root`.

    Walks one level deep (`<apps_root>/*/application.fam`). The root-level
    `application.fam` METAPACKAGE is intentionally skipped.
    """
    appids: set[str] = set()
    for fam in sorted(apps_root.glob("*/application.fam")):
        try:
            apps = parse_fam(fam)
        except RuntimeError as exc:
            print(f"warning: {exc}", file=sys.stderr)
            continue
        for app in apps:
            if app.get("apptype") == "EXTERNAL" and "appid" in app:
                appids.add(app["appid"])
    return sorted(appids)
