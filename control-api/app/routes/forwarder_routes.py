"""
app.routes.forwarder_routes — Webhook forwarder target management
=================================================================
Blueprint: ``forwarder``
Prefix:    ``/api/forwarder``
"""

from flask import Blueprint, jsonify, request

from app.middleware.auth_middleware import require_auth

forwarder_bp = Blueprint("forwarder", __name__, url_prefix="/api/forwarder")


@forwarder_bp.route("/targets", methods=["GET"])
@require_auth
def list_targets():
    """List all registered webhook forwarding targets.

    Returns:
        200 — array of webhook target objects
    """
    from app.services.forwarder_service import ForwarderService

    targets = ForwarderService.list_targets()
    return jsonify(targets), 200


@forwarder_bp.route("/targets", methods=["POST"])
@require_auth
def create_target():
    """Register a new webhook forwarding target.

    Request JSON:
        ``{ "url": "https://...", "auth_header": "Bearer ...", "active": true }``

    Returns:
        201 — created target
        400 — validation error
    """
    data = request.get_json(silent=True) or {}

    # TODO: validate with marshmallow schema
    from app.services.forwarder_service import ForwarderService

    result, error = ForwarderService.create_target(data)
    if error:
        return jsonify({"error": error}), 400

    return jsonify(result), 201


@forwarder_bp.route("/targets/<int:target_id>", methods=["DELETE"])
@require_auth
def delete_target(target_id: int):
    """Remove a webhook forwarding target.

    Returns:
        204 — Deleted
        404 — Not found
    """
    from app.services.forwarder_service import ForwarderService

    deleted = ForwarderService.delete_target(target_id)
    if not deleted:
        return jsonify({"error": "target not found"}), 404

    return "", 204
