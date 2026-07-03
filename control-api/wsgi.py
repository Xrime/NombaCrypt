#!/usr/bin/env python3
"""
NombaCrypt Shell — Control API  (production WSGI entry-point)
=============================================================
Usage with Gunicorn:
    gunicorn wsgi:app --bind 0.0.0.0:5000 --workers 4 --timeout 120

Usage with Flask-SocketIO (eventlet / gevent worker):
    gunicorn --worker-class eventlet wsgi:app --bind 0.0.0.0:5000
"""

import os

from app import create_app

# Default to "production" when launched via Gunicorn
os.environ.setdefault("FLASK_ENV", "production")

app = create_app()
