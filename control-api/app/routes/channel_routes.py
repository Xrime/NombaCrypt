"""
app.routes.channel_routes — Multi-API channel management
=========================================================
Blueprint: ``channels``
Prefix:    ``/api/channels``
"""

from flask import Blueprint, jsonify, request

from app.middleware.auth_middleware import require_auth

channels_bp = Blueprint("channels", __name__, url_prefix="/api/channels")


@channels_bp.route("", methods=["GET"])
@require_auth
def list_channels():
    """Return all configured API channels.

    Returns:
        200 — ``[ { "alias": "...", "client_id": "...", "health": "...", ... }, ... ]``
    """
    from app.services.channel_service import ChannelService

    channels = ChannelService.list_all()
    return jsonify(channels), 200


@channels_bp.route("", methods=["POST"])
@require_auth
def create_channel():
    """Create a new API channel.

    Request JSON:
        ``{ "alias": "...", "client_id": "...", "private_key": "..." }``

    Returns:
        201 — Created channel
        409 — Alias already exists
    """
    data = request.get_json(silent=True) or {}

    # TODO: validate with marshmallow schema
    from app.services.channel_service import ChannelService

    result, error = ChannelService.create(data)
    if error:
        return jsonify({"error": error}), 409

    return jsonify(result), 201


@channels_bp.route("/<string:alias>", methods=["DELETE"])
@require_auth
def delete_channel(alias: str):
    """Delete an API channel by alias.

    Returns:
        204 — Deleted
        404 — Not found
    """
    from app.services.channel_service import ChannelService

    deleted = ChannelService.delete(alias)
    if not deleted:
        return jsonify({"error": f"channel '{alias}' not found"}), 404

    return "", 204
