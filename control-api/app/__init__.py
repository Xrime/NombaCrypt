from flask import Flask
from flask_cors import CORS
import os
import sys

# Add the control-api directory to the Python path so it can find 'config'
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from config.settings import Config

def create_app(config_class=Config):
    """Factory function to construct the Flask application."""

    app = Flask(__name__)
    app.config.from_object(config_class)

    # Allow the React frontend (which usually runs on port 3000) to talk to us
    CORS(app)

    # --- Register Blueprints (Routes) ---
    # We will build out the individual routes later.
    # For now, we just add a simple health check.

    @app.route('/api/health', methods=['GET'])
    def health_check():
        return {
            "status": "healthy",
            "message": "NombaCrypt Shell Control API is running",
            "c_plus_plus_engine": Config.ENGINE_BASE_URL
        }, 200
        # --- Register Blueprints (Routes) ---
    from app.routes import register_blueprints
    register_blueprints(app)

    return app
    return app