"""
app.routes.telemetry_routes — Telemetry proxy endpoints
=======================================================
Blueprint: ``telemetry``
Prefix:    ``/api/telemetry``

Proxies real-time metrics from the C++ NombaCrypt engine.
"""

from flask import Blueprint, jsonify

from app.middleware.auth_middleware import require_auth

telemetry_bp = Blueprint("telemetry", __name__, url_prefix="/api/telemetry")


@telemetry_bp.route("/snapshot", methods=["GET"])
@require_auth
def telemetry_snapshot():
    """Proxy a telemetry snapshot from the C++ engine (HTTP).

    Calls ``EngineBridge.get_telemetry()`` and returns the JSON verbatim.

    Returns:
        200 — Engine telemetry JSON
        502 — Engine unreachable
    """
    from app.services.engine_bridge import EngineBridge

    try:
        snapshot = EngineBridge.get_telemetry()
        return jsonify(snapshot), 200
    except ConnectionError as exc:
        return jsonify({"error": "engine unreachable", "detail": str(exc)}), 502
