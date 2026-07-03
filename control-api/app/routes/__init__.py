from flask import Flask

def register_blueprints(app: Flask):
    """
    Registers all route blueprints with the main Flask app.
    This keeps our codebase modular and clean.
    """

    # Import the blueprints
    from app.routes.telemetry_routes import telemetry_bp
    from app.routes.nuban_routes import nuban_bp

    # Plug the blueprints into the app
    # Plug the blueprints into the app
    app.register_blueprint(telemetry_bp)
    app.register_blueprint(nuban_bp)