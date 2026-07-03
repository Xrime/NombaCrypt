"""
app.routes.toggle_routes — Feature-toggle endpoints
====================================================
Blueprint: ``toggles``
Prefix:    ``/api/toggles``
"""

from flask import Blueprint, jsonify, request

from app.middleware.auth_middleware import require_auth

toggles_bp = Blueprint("toggles", __name__, url_prefix="/api/toggles")


@toggles_bp.route("", methods=["GET"])
@require_auth
def list_toggles():
    """Return the current state of all feature toggles.

    Returns:
        200 — ``[ { "feature": "...", "enabled": true, "updated_at": "..." }, ... ]``
    """
    from app.services.toggle_service import ToggleService

    toggles = ToggleService.list_all()
    return jsonify(toggles), 200


@toggles_bp.route("/<string:feature>", methods=["PUT"])
@require_auth
def update_toggle(feature: str):
    """Enable or disable a specific feature toggle.

    URL params:
        feature — The feature key to update.

    Request JSON:
        ``{ "enabled": true }``

    Returns:
        200 — Updated toggle object
        404 — Feature not found
    """
    data = request.get_json(silent=True) or {}
    enabled = data.get("enabled")

    if enabled is None:
        return jsonify({"error": "'enabled' field is required"}), 400

    from app.services.toggle_service import ToggleService

    result = ToggleService.set_toggle(feature, bool(enabled))
    if result is None:
        return jsonify({"error": f"feature '{feature}' not found"}), 404

    return jsonify(result), 200
