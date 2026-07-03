"""
app.routes — Blueprint registration
====================================
Centralises all blueprint imports so that ``create_app`` only needs to call
``register_blueprints(app)``.
"""

from __future__ import annotations

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from flask import Flask


def register_blueprints(app: "Flask") -> None:
    """Import and register every API blueprint on *app*."""

    from app.routes.auth_routes import auth_bp
    from app.routes.toggle_routes import toggles_bp
    from app.routes.telemetry_routes import telemetry_bp
    from app.routes.channel_routes import channels_bp
    from app.routes.nuban_routes import nuban_bp
    from app.routes.transaction_routes import transactions_bp
    from app.routes.reconciler_routes import reconciler_bp
    from app.routes.subscription_routes import subscriptions_bp
    from app.routes.dunning_routes import dunning_bp
    from app.routes.forwarder_routes import forwarder_bp
    from app.routes.simulator_routes import simulator_bp
    from app.routes.export_routes import export_bp

    blueprints = [
        auth_bp,
        toggles_bp,
        telemetry_bp,
        channels_bp,
        nuban_bp,
        transactions_bp,
        reconciler_bp,
        subscriptions_bp,
        dunning_bp,
        forwarder_bp,
        simulator_bp,
        export_bp,
    ]

    for bp in blueprints:
        app.register_blueprint(bp)
