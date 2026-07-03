import os
from dotenv import load_dotenv

# Automatically find and load the .env file from the project root
load_dotenv()

class Config:
    """Base configuration settings for the Control API."""

    # Nomba API Credentials (Team Aint)
    NOMBA_CLIENT_ID = os.getenv("NOMBA_TEST_CLIENT_ID")
    NOMBA_PRIVATE_KEY = os.getenv("NOMBA_TEST_PRIVATE_KEY")
    NOMBA_ACCOUNT_ID = os.getenv("NOMBA_TEST_ACCOUNT_ID")
    NOMBA_SUB_ACCOUNT_ID = os.getenv("NOMBA_TEST_SUB_ACCOUNT_ID")

    NOMBA_API_BASE_URL = os.getenv("NOMBA_API_BASE_URL", "https://api.nomba.com/v1")

    # Engine Endpoints (Where Python talks to the lock-free C++ Engine)
    ENGINE_HTTP_PORT = os.getenv("ENGINE_HTTP_PORT", "8080")
    ENGINE_WS_PORT = os.getenv("ENGINE_WS_PORT", "8081")

    ENGINE_BASE_URL = f"http://localhost:{ENGINE_HTTP_PORT}"
    ENGINE_WS_URL = f"ws://localhost:{ENGINE_WS_PORT}"

    # Flask Framework Settings
    SECRET_KEY = os.getenv("FLASK_SECRET_KEY", "dev-secret-key-for-hackathon-only")
    DEBUG = True