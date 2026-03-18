#!/usr/bin/env python3
"""
generate_paper_report.py
────────────────────────
Extracts results from the GCC plugin differential testing folder structure
and produces a research-paper-ready Markdown report.

Expected folder structure
─────────────────────────
<root>/
    <plugin_name>/                          e.g.  arrm_ssp/
        <plugin_name>_test_N/               e.g.  arm_ssp_test_1/
            coverage_summary.txt            ← coverage data lives HERE
            <plugin_name>_instrumented/     e.g.  arm_ssp_instrumented/
                differential_report_full.json
                differential_report_bugs.json
                investigation_report.json
                investigation_report.txt
                bug_report_summary.txt
                Execution Logs/
                    BASE/   *.log
                    O1/     *.log
                    O2/     *.log
                    ...

Usage
─────
    python3 generate_paper_report.py --root /path/to/Diff_GCC_Plugin_Reports
    python3 generate_paper_report.py --root /path/to/results --out report.md
"""

from __future__ import annotations

import argparse
import json
import re
import sys
from collections import defaultdict
from pathlib import Path
from typing import Any, Dict, List, Optional


# ─────────────────────────────────────────────────────────────────────────────
# CONFIGURATION  ← edit these two lines
# ─────────────────────────────────────────────────────────────────────────────

ROOT_FOLDER = "/home/nimantha/Desktop/Diff_GCC_Plugin_Reports"
OUTPUT_FILE = "paper_report.md"

# ─────────────────────────────────────────────────────────────────────────────
# CONSTANTS
# ─────────────────────────────────────────────────────────────────────────────

DIFF_REPORT  = "differential_report_full.json"
INV_REPORT   = "investigation_report.json"
BUG_REPORT   = "differential_report_bugs.json"
COV_SUMMARY  = "coverage_summary.txt"

VERDICT_ORDER = [
    "CONFIRMED_BUG",
    "LIKELY_BUG",
    "NEEDS_REVIEW",
    "OPTIMISED_AWAY",
    "CONFIRMED_BENIGN",
    "LIKELY_BENIGN",
]

VERDICT_EMOJI = {
    "CONFIRMED_BUG":    "🔴",
    "LIKELY_BUG":       "🟠",
    "NEEDS_REVIEW":     "🟡",
    "OPTIMISED_AWAY":   "🟢",
    "CONFIRMED_BENIGN": "✅",
    "LIKELY_BENIGN":    "✅",
}

TAG_MEANINGS = {
    "COMPLEXITY_CHANGE":      "Binary grew AND has if/loop — possible new code path",
    "INLINED_COMPLEX_LOGIC":  "Inlined function had if/loop — inlining may alter flow",
    "DIFF_RETURN_PATH":       "Different return line hit — different branch taken",
    "DIFF_RETURN_VALUE":      "Same return line, different expression returned",
    "INVOCATION_COUNT_DIFF":  "Function called a different number of times",
    "FUNCTION_NOT_CALLED":    "Entire function body skipped in variant",
    "VOLATILE_RETURN":        "Return expression varies across invocations",
    "CALL_COUNT_ANOMALY":     "Static call_count > 0 but function never called",
}


# ─────────────────────────────────────────────────────────────────────────────
# FILE HELPERS
# ─────────────────────────────────────────────────────────────────────────────

def load_json(path: Path) -> Optional[Any]:
    try:
        with open(path, encoding="utf-8") as f:
            raw = f.read().strip()
        if not raw:
            # File exists but is empty — tester ran, found nothing
            return {}
        data = json.loads(raw)
        # Treat empty container same as no data
        if isinstance(data, dict) and not data:
            return {}
        if isinstance(data, list) and not data:
            return []
        return data
    except (OSError, json.JSONDecodeError) as e:
        print(f"  [WARN] Could not load {path.name}: {e}", file=sys.stderr)
        return None


def load_text(path: Path) -> Optional[str]:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return None


# ─────────────────────────────────────────────────────────────────────────────
# COVERAGE PARSING
# ─────────────────────────────────────────────────────────────────────────────

def parse_coverage_summary(text: str) -> Dict:
    """
    Parse coverage_summary.txt into a structured dict.
    Handles both gcov-style and custom summary formats.
    Extracts: overall line %, branch %, function %, and per-file breakdown.
    """
    result: Dict = {
        "line_pct":     None,
        "branch_pct":   None,
        "function_pct": None,
        "raw":          text[:500] if text else "",
    }

    # Try to find percentage values — handles formats like:
    #   "Lines executed: 72.34% of 100"
    #   "line coverage: 72.34%"
    #   "72.34% lines"
    line_re   = re.compile(r"(?:lines?|line[s\s]+(?:executed|coverage))[^0-9]*([0-9]+\.[0-9]+)\s*%",   re.I)
    branch_re = re.compile(r"(?:branch(?:es)?)[^0-9]*([0-9]+\.[0-9]+)\s*%", re.I)
    func_re   = re.compile(r"(?:function[s]?)[^0-9]*([0-9]+\.[0-9]+)\s*%",  re.I)

    m = line_re.search(text)
    if m:
        result["line_pct"] = float(m.group(1))
    m = branch_re.search(text)
    if m:
        result["branch_pct"] = float(m.group(1))
    m = func_re.search(text)
    if m:
        result["function_pct"] = float(m.group(1))

    # Also look for simple "XX%" patterns on lines containing "coverage"
    if result["line_pct"] is None:
        simple = re.search(r"([0-9]+\.?[0-9]*)\s*%", text)
        if simple:
            result["line_pct"] = float(simple.group(1))

    return result


# ─────────────────────────────────────────────────────────────────────────────
# STRUCTURE DISCOVERY
# ─────────────────────────────────────────────────────────────────────────────

def _find_report_dir(test_dir: Path) -> Optional[Path]:
    """
    Find the directory that contains the differential/investigation reports
    inside a test run folder.  Tries multiple strategies:

    1. Any subdirectory whose name contains "instrumented"
       that has at least one expected report file.
    2. test_dir itself (reports placed directly, no subfolder).
    3. Any immediate subdirectory that has at least one report file
       (catches unusual naming like funcp-encrypt-instrumented_bfr_testr_bug_fix).

    Returns the first matching path, or None if nothing found.
    """
    report_files = {DIFF_REPORT, INV_REPORT}

    def has_reports(p: Path) -> bool:
        return any((p / f).exists() for f in report_files)

    # Strategy 1: subdirectory with "instrumented" in name
    for sub in sorted(test_dir.iterdir()):
        if sub.is_dir() and "instrumented" in sub.name.lower():
            if has_reports(sub):
                return sub

    # Strategy 2: reports directly in test_dir
    if has_reports(test_dir):
        return test_dir

    # Strategy 3: any immediate subdirectory that has reports
    # (handles unusual naming — picks the one with the most report files)
    best: Optional[Path] = None
    best_count = 0
    for sub in sorted(test_dir.iterdir()):
        if not sub.is_dir():
            continue
        count = sum(1 for f in report_files if (sub / f).exists())
        if count > best_count:
            best_count = count
            best = sub

    return best if best_count > 0 else None


def _find_coverage(test_dir: Path, instrumented_path: Path) -> Optional[Path]:
    """
    Look for a coverage file in several locations, with broad name matching:
      1. test_dir/coverage_summary.txt  (standard location)
      2. instrumented_path/coverage_summary.txt
      3. Any *.txt file anywhere under test_dir whose name contains
         "coverage" or "cov" — catches coverage_report.txt, gcov_summary.txt,
         plugin_coverage.txt, etc.
    """
    # Standard name in standard location
    for search_dir in [test_dir, instrumented_path]:
        p = search_dir / COV_SUMMARY
        if p.exists():
            return p

    # Broad search: any txt file under test_dir with coverage in the name
    COV_KEYWORDS = ("coverage", "cov", "gcov", "lcov")
    for f in sorted(test_dir.rglob("*.txt")):
        name_lower = f.name.lower()
        if any(kw in name_lower for kw in COV_KEYWORDS):
            return f

    return None


def discover_structure(root: Path) -> Dict[str, List[Dict]]:
    """
    Walk the root and return:
        {plugin_name: [test_run_info, ...]}

    Each test_run_info contains:
        test_name, test_path, instrumented_path, coverage_path

    Handles all observed naming variations:
      - <plugin>_instrumented/
      - <plugin>-instrumented/
      - <plugin>-instrumented_bfr_testr_bug_fix/
      - <plugin>_plugin_instrumented/
      - reports directly in test_dir (no subfolder)
    """
    plugins: Dict[str, List[Dict]] = {}
    skipped: List[str] = []

    for plugin_dir in sorted(root.iterdir()):
        if not plugin_dir.is_dir():
            continue
        plugin_name = plugin_dir.name

        # Skip the SCRIPTS folder or any non-plugin directories
        if plugin_name.upper() in ("SCRIPTS", "SCRIPT", ".DS_STORE"):
            continue

        test_runs = []

        for test_dir in sorted(plugin_dir.iterdir()):
            if not test_dir.is_dir():
                continue

            instrumented_path = _find_report_dir(test_dir)

            if instrumented_path is None:
                skipped.append(f"{plugin_name}/{test_dir.name}")
                continue

            coverage_path = _find_coverage(test_dir, instrumented_path)

            test_runs.append({
                "test_name":         test_dir.name,
                "test_path":         test_dir,
                "instrumented_path": instrumented_path,
                "coverage_path":     coverage_path,
            })

        if test_runs:
            plugins[plugin_name] = test_runs

    if skipped:
        print(f"  [INFO] {len(skipped)} test dir(s) had no report files "
              f"and were skipped:", file=sys.stderr)
        for s in skipped:
            print(f"         {s}", file=sys.stderr)

    return plugins


# ─────────────────────────────────────────────────────────────────────────────
# PER-TEST EXTRACTION
# ─────────────────────────────────────────────────────────────────────────────

def extract_diff_summary(diff_data: Dict) -> Dict[str, Dict]:
    """Extract per-variant stats from differential_report_full.json."""
    summary = {}
    for variant, vdata in (diff_data or {}).items():
        if not isinstance(vdata, dict):
            continue

        base_frames = variant_frames = base_events = variant_events = 0
        real_issues = noise_issues = 0
        log_identical = True
        missing_logs  = False

        for log_name, log_info in vdata.get("log_diffs", {}).items():
            status = log_info.get("status", "")
            if status == "missing_variant_log":
                missing_logs = True
                continue
            s  = log_info.get("summary", {})
            bf = s.get("base_frames",    0)
            vf = s.get("variant_frames", 0)
            base_frames    += bf
            variant_frames += vf
            base_events    += s.get("base_events",    0)
            variant_events += s.get("variant_events", 0)
            real_issues    += s.get("issue_count",    0)
            noise_issues   += s.get("noise_count",    0)
            if bf != vf or s.get("issue_count", 0) > 0:
                log_identical = False

        sym_diff = vdata.get("symbol_diff", []) or []
        removed  = [l[1:].strip() for l in sym_diff
                    if l.startswith("-") and not l.startswith("---")]
        added    = [l[1:].strip() for l in sym_diff
                    if l.startswith("+") and not l.startswith("+++")]

        summary[variant] = {
            "tester_verdict":   vdata.get("severity", "?"),
            "log_identical":    log_identical,
            "missing_logs":     missing_logs,
            "base_frames":      base_frames,
            "variant_frames":   variant_frames,
            "base_events":      base_events,
            "variant_events":   variant_events,
            "real_issues":      real_issues,
            "noise_issues":     noise_issues,
            "symbols_removed":  removed,
            "symbols_added":    added,
            "changed_fns":      vdata.get("disasm_changed_functions") or [],
        }
    return summary


def extract_inv_summary(inv_data: List) -> Dict[str, Dict]:
    """Extract per-variant stats from investigation_report.json."""
    summary = {}
    for entry in (inv_data or []):
        if not isinstance(entry, dict):
            continue
        variant = entry.get("variant", "?")
        d5 = entry.get("stage5_deep", {}) or {}
        s6 = entry.get("stage6_runtime", {}) or {}
        v  = entry.get("verdict", {}) or {}

        bug_tag_types  = sorted({t["tag"] for t in d5.get("bug_tags",  [])})
        safe_tag_types = sorted({t["tag"] for t in d5.get("safe_tags", [])})

        summary[variant] = {
            "final_verdict":    v.get("severity", "?"),
            "bug_tag_count":    len(d5.get("bug_tags",  [])),
            "safe_tag_count":   len(d5.get("safe_tags", [])),
            "bug_tag_types":    bug_tag_types,
            "safe_tag_types":   safe_tag_types,
            "rationale":        d5.get("rationale", ""),
            "s6_available":     s6.get("available", False),
            "s6_verdict":       s6.get("confirmed_verdict", "UNAVAILABLE"),
            "s6_missing":       s6.get("missing_in_variant", []),
            "s6_extra":         s6.get("extra_in_variant",  []),
            "s6_base_count":    s6.get("base_instr_count",  0),
            "s6_var_count":     s6.get("var_instr_count",   0),
            "s6_detail":        s6.get("detail", ""),
        }
    return summary


def count_log_files(instrumented_path: Path) -> Dict[str, int]:
    """Count log files per variant in Execution Logs/."""
    counts: Dict[str, int] = {}
    logs_root = instrumented_path / "Execution Logs"
    if logs_root.exists():
        for vdir in sorted(logs_root.iterdir()):
            if vdir.is_dir():
                counts[vdir.name] = len(list(vdir.glob("*.log")))
    return counts


def load_test_run(info: Dict) -> Dict:
    """Load and merge all data for one test run."""
    ipath = info["instrumented_path"]

    # Log which files were found so missing data is transparent in output
    diff_path = ipath / DIFF_REPORT
    inv_path  = ipath / INV_REPORT
    if not diff_path.exists():
        print(f"    [MISSING] {DIFF_REPORT} in {ipath.name}/", file=sys.stderr)
    if not inv_path.exists():
        print(f"    [MISSING] {INV_REPORT} in {ipath.name}/", file=sys.stderr)

    diff_data = load_json(diff_path) if diff_path.exists() else None
    inv_data  = load_json(inv_path)  if inv_path.exists()  else None

    # Distinguish: None = file absent, {} = file present but empty (clean run)
    diff_ran_clean = diff_path.exists() and diff_data == {}
    inv_ran_clean  = inv_path.exists()  and inv_data  in ({}, [])

    diff_summary = extract_diff_summary(diff_data or {})
    inv_summary  = extract_inv_summary(inv_data  or [])

    # Merge per-variant
    all_variants = sorted(set(diff_summary) | set(inv_summary))
    variants = {}
    for v in all_variants:
        variants[v] = {**diff_summary.get(v, {}), **inv_summary.get(v, {})}

    # Coverage
    coverage = {}
    if info.get("coverage_path") and info["coverage_path"].exists():
        text = load_text(info["coverage_path"])
        if text:
            coverage = parse_coverage_summary(text)
            coverage["raw_path"] = str(info["coverage_path"])

    return {
        "test_name":      info["test_name"],
        "test_path":      str(info["test_path"]),
        "variants":       variants,
        "log_counts":     count_log_files(ipath),
        "coverage":       coverage,
        "has_diff":       diff_data is not None,
        "has_inv":        inv_data  is not None,
        "diff_ran_clean": diff_ran_clean,
        "inv_ran_clean":  inv_ran_clean,
    }


# ─────────────────────────────────────────────────────────────────────────────
# AGGREGATION
# ─────────────────────────────────────────────────────────────────────────────

def aggregate_plugin(test_runs: List[Dict]) -> Dict:
    """Roll up all test runs for one plugin."""
    verdict_counts:  Dict[str, int] = defaultdict(int)
    tester_counts:   Dict[str, int] = defaultdict(int)
    bug_tag_freq:    Dict[str, int] = defaultdict(int)
    confirmed_bugs:  List[Dict]     = []
    likely_bugs:     List[Dict]     = []
    total_variants = total_tests = 0
    coverage_values: List[float]    = []

    for run in test_runs:
        total_tests += 1

        cov = run.get("coverage", {})
        if cov.get("line_pct") is not None:
            coverage_values.append(cov["line_pct"])

        # If differential report ran but found nothing suspicious,
        # and investigator also ran clean, record this run as all-OK
        if run.get("diff_ran_clean") and not run["variants"]:
            verdict_counts["ALL_OK"] += 1
            tester_counts["OK"] += 1
            continue

        for variant, vdata in run["variants"].items():
            total_variants += 1
            fv = vdata.get("final_verdict", "?")
            tv = vdata.get("tester_verdict", "?")
            verdict_counts[fv] += 1
            tester_counts[tv]  += 1

            for tag in vdata.get("bug_tag_types", []):
                bug_tag_freq[tag] += 1

            if fv == "CONFIRMED_BUG":
                confirmed_bugs.append({
                    "test":      run["test_name"],
                    "variant":   variant,
                    "missing":   vdata.get("s6_missing",     []),
                    "extra":     vdata.get("s6_extra",       []),
                    "detail":    vdata.get("s6_detail",      ""),
                    "tags":      vdata.get("bug_tag_types",  []),
                    "removed":   vdata.get("symbols_removed",[]),
                })
            elif fv == "LIKELY_BUG":
                likely_bugs.append({
                    "test":      run["test_name"],
                    "variant":   variant,
                    "tags":      vdata.get("bug_tag_types", []),
                    "rationale": vdata.get("rationale",     ""),
                    "removed":   vdata.get("symbols_removed",[]),
                })

    avg_cov = (sum(coverage_values) / len(coverage_values)
               if coverage_values else None)

    return {
        "total_tests":     total_tests,
        "total_variants":  total_variants,
        "verdict_counts":  dict(verdict_counts),
        "tester_counts":   dict(tester_counts),
        "bug_tag_freq":    dict(bug_tag_freq),
        "confirmed_bugs":  confirmed_bugs,
        "likely_bugs":     likely_bugs,
        "avg_coverage":    avg_cov,
        "coverage_values": coverage_values,
    }


# ─────────────────────────────────────────────────────────────────────────────
# MARKDOWN RENDERING
# ─────────────────────────────────────────────────────────────────────────────

def ve(v: str) -> str:
    return VERDICT_EMOJI.get(v, "❓")


def pct(v: Optional[float]) -> str:
    return f"{v:.1f}%" if v is not None else "—"


def render_executive_summary(
    structure: Dict[str, List],
    all_agg:   Dict[str, Dict],
) -> str:
    lines = [
        "## Executive Summary",
        "",
        "| Plugin | Tests | Variants | 🔴 Confirmed Bugs | 🟠 Likely Bugs | "
        "🟢 Optimised Away | ✅ Benign | Avg Coverage |",
        "|--------|------:|---------:|------------------:|---------------:|"
        "-----------------:|----------:|-------------:|",
    ]

    tot_t = tot_v = tot_cb = tot_lb = tot_oa = tot_bn = 0
    cov_vals: List[float] = []

    for plugin_name in sorted(structure):
        agg = all_agg[plugin_name]
        vc  = agg["verdict_counts"]
        cb  = vc.get("CONFIRMED_BUG",    0)
        lb  = vc.get("LIKELY_BUG",       0)
        oa  = vc.get("OPTIMISED_AWAY",   0)
        bn  = (vc.get("CONFIRMED_BENIGN", 0)
               + vc.get("LIKELY_BENIGN",  0))
        t   = agg["total_tests"]
        v   = agg["total_variants"]
        cov = agg["avg_coverage"]
        if cov is not None:
            cov_vals.append(cov)

        tot_t  += t;  tot_v  += v
        tot_cb += cb; tot_lb += lb; tot_oa += oa; tot_bn += bn

        cb_str = f"**{cb}**" if cb > 0 else str(cb)
        lb_str = f"*{lb}*"   if lb > 0 else str(lb)

        lines.append(
            f"| `{plugin_name}` | {t} | {v} | {cb_str} | {lb_str} "
            f"| {oa} | {bn} | {pct(cov)} |"
        )

    avg_all = sum(cov_vals) / len(cov_vals) if cov_vals else None
    lines += [
        f"| **TOTAL** | **{tot_t}** | **{tot_v}** | **{tot_cb}** | "
        f"**{tot_lb}** | **{tot_oa}** | **{tot_bn}** | {pct(avg_all)} |",
        "",
    ]
    return "\n".join(lines)


def render_variant_sensitivity(all_agg: Dict[str, Dict]) -> str:
    variant_bug:  Dict[str, int] = defaultdict(int)
    variant_conf: Dict[str, int] = defaultdict(int)

    for agg in all_agg.values():
        for bug in agg["confirmed_bugs"]:
            variant_bug[bug["variant"]]  += 1
            variant_conf[bug["variant"]] += 1
        for bug in agg["likely_bugs"]:
            variant_bug[bug["variant"]] += 1

    if not variant_bug:
        return (
            "## Variant Sensitivity\n\n"
            "*No bugs found across any variant.*\n"
        )

    lines = [
        "## Variant Sensitivity",
        "",
        "Which compiler flag variants triggered bugs across all plugins:",
        "",
        "| Variant | Total Bugs | Confirmed | Likely | Severity |",
        "|---------|----------:|----------:|-------:|----------|",
    ]
    for variant, total in sorted(variant_bug.items(), key=lambda x: -x[1]):
        conf  = variant_conf.get(variant, 0)
        likely = total - conf
        sev   = "🔴 CONFIRMED" if conf > 0 else "🟠 LIKELY"
        lines.append(
            f"| `{variant}` | {total} | {conf} | {likely} | {sev} |"
        )

    lines.append("")
    return "\n".join(lines)


def render_bug_tag_analysis(all_agg: Dict[str, Dict]) -> str:
    combined: Dict[str, int] = defaultdict(int)
    for agg in all_agg.values():
        for tag, freq in agg["bug_tag_freq"].items():
            combined[tag] += freq

    if not combined:
        return "## Bug Tag Analysis\n\n*No bug tags found.*\n"

    lines = [
        "## Bug Tag Analysis",
        "",
        "Frequency of bug-indicating tags across all plugins and test runs:",
        "",
        "| Tag | Count | Meaning |",
        "|-----|------:|---------|",
    ]
    for tag, freq in sorted(combined.items(), key=lambda x: -x[1]):
        meaning = TAG_MEANINGS.get(tag, "")
        lines.append(f"| `{tag}` | {freq} | {meaning} |")

    lines.append("")
    return "\n".join(lines)


def render_coverage_section(
    structure: Dict[str, List],
    all_runs:  Dict[str, List[Dict]],
) -> str:
    lines = [
        "## Code Coverage Analysis",
        "",
        "Plugin code coverage achieved across test runs "
        "(from `coverage_summary.txt`):",
        "",
        "| Plugin | Test | Line % | Branch % | Function % |",
        "|--------|------|-------:|---------:|-----------:|",
    ]

    any_data = False
    for plugin_name in sorted(structure):
        for run in all_runs[plugin_name]:
            cov = run.get("coverage", {})
            lp  = pct(cov.get("line_pct"))
            bp  = pct(cov.get("branch_pct"))
            fp  = pct(cov.get("function_pct"))
            if cov:
                any_data = True
            lines.append(
                f"| `{plugin_name}` | {run['test_name']} | {lp} | {bp} | {fp} |"
            )

    if not any_data:
        return (
            "## Code Coverage Analysis\n\n"
            "*No `coverage_summary.txt` files found.*\n"
        )

    lines.append("")
    return "\n".join(lines)


def render_plugin_section(
    plugin_name: str,
    test_runs:   List[Dict],
    agg:         Dict,
) -> str:
    lines = [f"## Plugin: `{plugin_name}`", ""]

    # ── Verdict summary ───────────────────────────────────────────────────
    vc = agg["verdict_counts"]
    lines += ["### Verdict Summary", "", "| Verdict | Count |", "|---------|------:|"]
    for v in VERDICT_ORDER:
        c = vc.get(v, 0)
        if c > 0:
            lines.append(f"| {ve(v)} {v} | {c} |")
    lines.append("")

    # Coverage
    cov = agg["avg_coverage"]
    if cov is not None:
        lines += [
            "### Coverage",
            "",
            f"Average line coverage across {agg['total_tests']} test run(s): "
            f"**{pct(cov)}**",
            "",
        ]

    # ── Bug tag frequency ────────────────────────────────────────────────
    if agg["bug_tag_freq"]:
        lines += [
            "### Bug Tags Observed",
            "",
            "| Tag | Count |",
            "|-----|------:|",
        ]
        for tag, freq in sorted(
            agg["bug_tag_freq"].items(), key=lambda x: -x[1]
        ):
            lines.append(f"| `{tag}` | {freq} |")
        lines.append("")

    # ── Confirmed bugs ────────────────────────────────────────────────────
    if agg["confirmed_bugs"]:
        lines += ["### 🔴 Confirmed Bugs", ""]
        for bug in agg["confirmed_bugs"]:
            lines.append(
                f"**`{bug['test']}` / variant `{bug['variant']}`**"
            )
            lines.append("")
            lines.append(f"> {bug['detail']}")
            lines.append("")
            if bug["missing"]:
                fns = ", ".join(f"`{f}`" for f in bug["missing"])
                lines.append(f"- **Functions that lost protection:** {fns}")
            if bug["extra"]:
                fns = ", ".join(f"`{f}`" for f in bug["extra"])
                lines.append(f"- **Unexpectedly gained protection:** {fns}")
            if bug["removed"]:
                syms = ", ".join(f"`{s}`" for s in bug["removed"][:5])
                tail = f" (+{len(bug['removed'])-5} more)" \
                       if len(bug["removed"]) > 5 else ""
                lines.append(f"- **Symbols inlined/removed:** {syms}{tail}")
            if bug["tags"]:
                lines.append(
                    f"- **Bug tags:** "
                    + ", ".join(f"`{t}`" for t in bug["tags"])
                )
            lines.append("")

    # ── Likely bugs ────────────────────────────────────────────────────
    if agg["likely_bugs"]:
        lines += [
            "### 🟠 Likely Bugs",
            "",
            "> Stage 6 runtime confirmation was not run for these variants. "
            "Binary analysis suggests a bug but it has not been verified "
            "by re-running the plugin.",
            "",
            "| Test | Variant | Symbols Removed | Bug Tags |",
            "|------|---------|----------------|----------|",
        ]
        for bug in agg["likely_bugs"]:
            tags = ", ".join(f"`{t}`" for t in bug["tags"]) if bug["tags"] else "—"
            removed_n = len(bug.get("removed", []))
            lines.append(
                f"| {bug['test']} | `{bug['variant']}` | {removed_n} | {tags} |"
            )
        lines.append("")

    # ── Per-test run tables ───────────────────────────────────────────────
    lines += ["### Per-Test Run Detail", ""]

    for run in test_runs:
        lines += [
            f"#### {run['test_name']}",
            "",
        ]

        # Coverage for this run
        cov = run.get("coverage", {})
        if cov.get("line_pct") is not None:
            lines.append(
                f"*Coverage:* line {pct(cov.get('line_pct'))} | "
                f"branch {pct(cov.get('branch_pct'))} | "
                f"function {pct(cov.get('function_pct'))}"
            )
            lines.append("")

        if not run["variants"]:
            if run.get("diff_ran_clean"):
                lines.append(
                    "> ✅ **Differential tester ran and found no suspicious "
                    "or buggy variants** — all compiler flag variants produced "
                    "identical behaviour to BASE."
                )
            else:
                lines.append("*No variant data found.*")
            lines.append("")
            continue

        lines += [
            "| Variant | Tester | Investigation | Stage 6 | "
            "Sym Removed | Bug Tags | Frames B/V |",
            "|---------|--------|--------------|---------|"
            "------------:|----------|------------|",
        ]

        for variant, vdata in sorted(run["variants"].items()):
            tester   = vdata.get("tester_verdict", "?")
            inv      = vdata.get("final_verdict",  "?")
            s6v      = vdata.get("s6_verdict",     "UNAVAILABLE")
            s6_avail = vdata.get("s6_available",   False)
            s6_str   = f"{ve(s6v)} {s6v}" if s6_avail else "—"
            removed  = len(vdata.get("symbols_removed", []))
            tags     = (
                ", ".join(f"`{t}`" for t in vdata.get("bug_tag_types", []))
                or "—"
            )
            bf = vdata.get("base_frames",    "—")
            vf = vdata.get("variant_frames", "—")
            identical = " *(=)*" if vdata.get("log_identical") else ""
            missing_log = " ⚠️" if vdata.get("missing_logs") else ""
            frames = f"{bf}/{vf}{identical}{missing_log}"

            lines.append(
                f"| `{variant}` | {tester} | {ve(inv)} {inv} | "
                f"{s6_str} | {removed} | {tags} | {frames} |"
            )

        lines.append("")

        # Stage 6 confirmed bugs detail for this run
        s6_bugs = [
            (v, vd) for v, vd in run["variants"].items()
            if vd.get("s6_verdict") == "CONFIRMED_BUG"
        ]
        if s6_bugs:
            lines += ["**🔴 Stage 6 Runtime Confirmation:**", ""]
            for variant, vdata in s6_bugs:
                lines.append(f"- **`{variant}`**: {vdata.get('s6_detail', '')}")
                if vdata.get("s6_missing"):
                    fns = ", ".join(f"`{f}`" for f in vdata["s6_missing"])
                    lines.append(
                        f"  - Base instrumented: {vdata.get('s6_base_count', '?')} fn(s), "
                        f"variant: {vdata.get('s6_var_count', '?')} fn(s)"
                    )
                    lines.append(f"  - Missing protection: {fns}")
            lines.append("")

    return "\n".join(lines)


# ─────────────────────────────────────────────────────────────────────────────
# MAIN
# ─────────────────────────────────────────────────────────────────────────────

def generate_report(root: Path) -> str:
    print(f"[INFO] Scanning: {root}")
    structure = discover_structure(root)

    if not structure:
        print("[ERROR] No plugin directories with report files found.",
              file=sys.stderr)
        sys.exit(1)

    print(f"[INFO] Found {len(structure)} plugin(s):")
    for pname, runs in structure.items():
        print(f"       {pname}:  {len(runs)} test run(s)")

    # Load all data
    all_runs: Dict[str, List[Dict]] = {}
    all_agg:  Dict[str, Dict]       = {}

    for plugin_name, run_infos in structure.items():
        runs = []
        for info in run_infos:
            iname = info["instrumented_path"].name
            print(f"  Loading {plugin_name}/{info['test_name']}/{iname} ...", end=" ")
            run = load_test_run(info)
            runs.append(run)
            n_var    = len(run["variants"])
            has_cov  = "cov"      if run.get("coverage") else "no-cov"
            if run["has_diff"]:
                has_diff = "diff(clean)" if run.get("diff_ran_clean") else "diff"
            else:
                has_diff = "NO-DIFF"
            if run["has_inv"]:
                has_inv = "inv(clean)" if run.get("inv_ran_clean") else "inv"
            else:
                has_inv = "NO-INV"
            print(f"{n_var} variant(s)  [{has_diff}|{has_inv}|{has_cov}]")
        all_runs[plugin_name] = runs
        all_agg[plugin_name]  = aggregate_plugin(runs)

    # ── Assemble report ───────────────────────────────────────────────────
    sections = []

    sections.append(
        "# GCC Plugin Differential Testing — Research Report\n"
        "\n"
        "> Generated by `generate_paper_report.py`\n"
        "\n"
        "---\n"
    )

    sections.append(render_executive_summary(structure, all_agg))
    sections.append(render_variant_sensitivity(all_agg))
    sections.append(render_bug_tag_analysis(all_agg))
    sections.append(render_coverage_section(structure, all_runs))

    sections.append("---\n\n# Per-Plugin Results\n")
    for plugin_name in sorted(structure):
        sections.append(
            render_plugin_section(
                plugin_name,
                all_runs[plugin_name],
                all_agg[plugin_name],
            )
        )
        sections.append("---\n")

    return "\n".join(sections)


def main():
    parser = argparse.ArgumentParser(
        description=(
            "Generate a research paper report from GCC plugin "
            "differential testing results."
        )
    )
    parser.add_argument(
        "--root", metavar="DIR", default=ROOT_FOLDER,
        help=f"Root folder containing plugin result subdirectories "
             f"(default: {ROOT_FOLDER})",
    )
    parser.add_argument(
        "--out", metavar="FILE", default=OUTPUT_FILE,
        help=f"Output Markdown file (default: {OUTPUT_FILE})",
    )
    args = parser.parse_args()

    root = Path(args.root)
    if not root.exists():
        print(f"[ERROR] Root not found: {root}", file=sys.stderr)
        print(f"[INFO]  Update ROOT_FOLDER at the top of this script.", file=sys.stderr)
        sys.exit(1)

    report = generate_report(root)

    out = Path(args.out)
    out.write_text(report, encoding="utf-8")
    print(f"\n[INFO] Report written → {out}")
    print(f"[INFO] {report.count(chr(10))} lines, {len(report)} chars")


if __name__ == "__main__":
    main()