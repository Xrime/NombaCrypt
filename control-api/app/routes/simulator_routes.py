"""
app.routes.simulator_routes — Traffic simulator endpoints
=========================================================
Blueprint: ``simulator``
Prefix:    ``/api/simulate``
"""

from flask import Blueprint, jsonify, request

from app.middleware.auth_middleware import require_auth

simulator_bp = Blueprint("simulator", __name__, url_prefix="/api/simulate")


@simulator_bp.route("/start", methods=["POST"])
@require_auth
def start_simulation():
    """Kick off a load-test / traffic simulation against the C++ engine.

    Request JSON:
        ``{ "tps": 500, "duration_seconds": 60, "pattern": "burst" }``

    Returns:
        202 — ``{ "run_id": "...", "status": "running" }``
        400 — Invalid config
    """
    data = request.get_json(silent=True) or {}

    # TODO: validate with marshmallow schema
    from app.services.simulator_service import SimulatorService

    result, error = SimulatorService.start(data)
    if error:
        return jsonify({"error": error}), 400

    return jsonify(result), 202


@simulator_bp.route("/status", methods=["GET"])
@require_auth
def simulation_status():
    """Get status of the latest (or specified) simulation run.

    Query params:
        ``run_id`` — (optional) specific run ID

    Returns:
        200 — ``{ "run_id": "...", "status": "...", "progress": ... }``
        404 — Run not found
    """
    run_id = request.args.get("run_id")

    from app.services.simulator_service import SimulatorService

    result = SimulatorService.get_status(run_id=run_id)
    if result is None:
        return jsonify({"error": "simulation run not found"}), 404

    return jsonify(result), 200
