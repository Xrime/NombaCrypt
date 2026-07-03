"""
app.routes.nuban_routes — NUBAN virtual account endpoints
==========================================================
Blueprint: ``nuban``
Prefix:    ``/api/nuban``
"""

from flask import Blueprint, jsonify, request

from app.middleware.auth_middleware import require_auth

nuban_bp = Blueprint("nuban", __name__, url_prefix="/api/nuban")


@nuban_bp.route("/provision", methods=["POST"])
@require_auth
def provision_account():
    """Provision a new virtual NUBAN account for a user.

    Request JSON:
        ``{ "user_id": "...", "bank_name": "..." }``

    Returns:
        201 — ``{ "nuban": "...", "bank_name": "...", "active": true }``
        400 — Validation error
    """
    data = request.get_json(silent=True) or {}

    # TODO: validate with marshmallow schema
    from app.services.nuban_service import NubanService

    result, error = NubanService.provision(data)
    if error:
        return jsonify({"error": error}), 400

    return jsonify(result), 201


@nuban_bp.route("/list", methods=["GET"])
@require_auth
def list_accounts():
    """List all virtual accounts, optionally filtered by user.

    Query params:
        ``user_id`` — (optional) filter by user UUID

    Returns:
        200 — array of virtual account objects
    """
    user_id = request.args.get("user_id")

    from app.services.nuban_service import NubanService

    accounts = NubanService.list_accounts(user_id=user_id)
    return jsonify(accounts), 200
