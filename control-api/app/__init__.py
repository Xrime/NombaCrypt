"""
app — NombaCrypt Shell Control API package
==========================================
Exposes the ``create_app`` factory used by ``run.py`` and ``wsgi.py``.
"""

from flask import Flask
from flask_cors import CORS
from flask_jwt_extended import JWTManager
from flask_limiter import Limiter
from flask_limiter.util import get_remote_address
from flask_socketio import SocketIO

from app.models import db

# ---------------------------------------------------------------------------
# Extension instances (initialised in create_app)
# ---------------------------------------------------------------------------
jwt = JWTManager()
cors = CORS()
limiter = Limiter(key_func=get_remote_address, default_limits=["200 per minute"])
socketio = SocketIO()


def create_app(config_name: str | None = None) -> Flask:
    """Application factory.

    Parameters
    ----------
    config_name:
        One of ``"development"`` (default), ``"production"``, or a dotted
        path to a custom config object.

    Returns
    -------
    Flask
        Fully-configured Flask application instance.
    """
    import os

    app = Flask(__name__)

    # ---- Load configuration ------------------------------------------------
    if config_name is None:
        config_name = os.getenv("FLASK_ENV", "development")

    config_map = {
        "development": "config.development.DevelopmentConfig",
        "production": "config.production.ProductionConfig",
    }
    app.config.from_object(config_map.get(config_name, config_name))

    # ---- Initialise extensions ---------------------------------------------
    db.init_app(app)
    jwt.init_app(app)
    cors.init_app(app, resources={r"/api/*": {"origins": app.config.get("CORS_ORIGINS", "*")}})
    limiter.init_app(app)
    socketio.init_app(app, cors_allowed_origins="*", async_mode="threading")

    # ---- Register blueprints -----------------------------------------------
    from app.routes import register_blueprints

    register_blueprints(app)

    # ---- Register error handlers -------------------------------------------
    from app.middleware.error_handler import register_error_handlers

    register_error_handlers(app)

    # ---- Create DB tables (dev convenience) --------------------------------
    with app.app_context():
        db.create_all()

    return app
