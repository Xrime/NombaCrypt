"""
config.production — Production overrides
=========================================
"""

import os

from config.settings import BaseConfig


class ProductionConfig(BaseConfig):
    """Configuration used in production deployment."""

    DEBUG: bool = False
    TESTING: bool = False

    # PostgreSQL — URL must be set via DATABASE_URL env var
    SQLALCHEMY_DATABASE_URI: str = os.getenv(
        "DATABASE_URL",
        "postgresql://nombacrypt:password@localhost:5432/nombacrypt_prod",
    )

    # Strict CORS — restrict to the dashboard origin
    CORS_ORIGINS: str = os.getenv("CORS_ORIGINS", "https://dashboard.nombacrypt.io")

    # Redis-backed rate limiter in production
    RATELIMIT_STORAGE_URI: str = os.getenv(
        "RATELIMIT_STORAGE_URI",
        "redis://localhost:6379/0",
    )
