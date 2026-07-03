import sys
import os

# Ensure Python knows where to look for our internal files
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from app import create_app
from config.settings import Config

# Initialize the Flask application
app = create_app()

if __name__ == '__main__':

    print(f" Starting NombaCrypt Control API")
    print(f" Pointed to C++ Engine at: {Config.ENGINE_BASE_URL}")


    # Start the local development server on port 5000
    app.run(host='0.0.0.0', port=5000, debug=Config.DEBUG)