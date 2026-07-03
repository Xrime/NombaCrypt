"""
config.development — Development overrides
===========================================
"""

from config.settings import BaseConfig


class DevelopmentConfig(BaseConfig):
    """Configuration used during local development."""

    DEBUG: bool = True
    TESTING: bool = False

    # SQLite for zero-setup local dev
    SQLALCHEMY_DATABASE_URI: str = "sqlite:///nombacrypt_dev.db"

    # More permissive rate limits during development
    RATELIMIT_ENABLED: bool = False

    # CORS — allow any origin during dev
    CORS_ORIGINS: str = "*"
