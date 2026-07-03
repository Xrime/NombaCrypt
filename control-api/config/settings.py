"""
config.settings — Base configuration
=====================================
All values are overridable via environment variables.
Sub-classes (``DevelopmentConfig``, ``ProductionConfig``) inherit from
``BaseConfig`` and override as needed.
"""

import os
from datetime import timedelta

from dotenv import load_dotenv

load_dotenv()  # Load .env file if present


class BaseConfig:
    """Shared configuration defaults."""

    # ---- Flask core --------------------------------------------------------
    SECRET_KEY: str = os.getenv("SECRET_KEY", "change-me-in-production")
    DEBUG: bool = False
    TESTING: bool = False

    # ---- Database ----------------------------------------------------------
    SQLALCHEMY_DATABASE_URI: str = os.getenv(
        "DATABASE_URL",
        "sqlite:///nombacrypt.db",
    )
    SQLALCHEMY_TRACK_MODIFICATIONS: bool = False

    # ---- JWT ---------------------------------------------------------------
    JWT_SECRET_KEY: str = os.getenv("JWT_SECRET_KEY", "jwt-dev-secret")
    JWT_ACCESS_TOKEN_EXPIRES: timedelta = timedelta(
        minutes=int(os.getenv("JWT_ACCESS_MINUTES", "30")),
    )
    JWT_REFRESH_TOKEN_EXPIRES: timedelta = timedelta(
        days=int(os.getenv("JWT_REFRESH_DAYS", "7")),
    )

    # ---- C++ Engine (HTTP API) ---------------------------------------------
    ENGINE_HTTP_URL: str = os.getenv("ENGINE_HTTP_URL", "http://localhost:8080")

    # ---- C++ Engine (WebSocket telemetry) ----------------------------------
    ENGINE_WS_URL: str = os.getenv("ENGINE_WS_URL", "ws://localhost:8081")

    # ---- CORS --------------------------------------------------------------
    CORS_ORIGINS: str = os.getenv("CORS_ORIGINS", "*")

    # ---- Rate Limiting -----------------------------------------------------
    RATELIMIT_STORAGE_URI: str = os.getenv("RATELIMIT_STORAGE_URI", "memory://")

    # ---- Fernet encryption key for stored API keys -------------------------
    FERNET_KEY: str = os.getenv("FERNET_KEY", "")  # Must be set in prod

    # ---- Pagination --------------------------------------------------------
    DEFAULT_PAGE_SIZE: int = int(os.getenv("DEFAULT_PAGE_SIZE", "50"))
    MAX_PAGE_SIZE: int = int(os.getenv("MAX_PAGE_SIZE", "200"))
