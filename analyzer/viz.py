"""Visualization helpers for the log analyzer app."""

from __future__ import annotations

import altair as alt


def configure_altair() -> None:
    """Configure Altair defaults for the Streamlit application."""
    # Altair limits datasets to 5,000 rows by default which breaks our large trace logs.
    alt.data_transformers.disable_max_rows()
