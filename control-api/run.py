#!/usr/bin/env python3
"""
NombaCrypt Shell — Control API  (development entry-point)
=========================================================
Usage:
    python run.py

Starts the Flask dev server on http://localhost:5000 with DEBUG enabled.
For production, use ``wsgi.py`` with Gunicorn instead.
"""

from app import create_app

app = create_app()

if __name__ == "__main__":
    app.run(
        host="0.0.0.0",
        port=5000,
        debug=True,
    )
