from __future__ import annotations
import argparse, json
from oht_analyzer.pipeline import analyze_in_order


def main():
    ap = argparse.ArgumentParser(description="Run standard OHT log analysis in the required order")
    ap.add_argument("--bundle", required=True, help="Path to folder/zip/tar or single file")
    ap.add_argument("--out", default="artifacts/analysis", help="Output directory for summaries/plots")
    ap.add_argument("--axis", type=int, default=3, help="Axis focus (default 3=SLIDE)")
    args = ap.parse_args()
    res = analyze_in_order(args.bundle, args.out, axis_focus=args.axis)
    print(json.dumps({"artifacts_dir": res.artifacts_dir, "steps": res.plan}, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
