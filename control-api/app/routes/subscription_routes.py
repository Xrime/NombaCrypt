"""
app.routes.subscription_routes — Subscription / installment endpoints
=====================================================================
Blueprint: ``subscriptions``
Prefix:    ``/api/subscriptions``
"""

from flask import Blueprint, jsonify, request

from app.middleware.auth_middleware import require_auth

subscriptions_bp = Blueprint("subscriptions", __name__, url_prefix="/api/subscriptions")


@subscriptions_bp.route("", methods=["GET"])
@require_auth
def list_subscriptions():
    """List all subscription / installment plans.

    Query params:
        ``user_id`` — (optional) filter by user
        ``state``   — (optional) filter by state (active, completed, failed)

    Returns:
        200 — array of subscription objects
    """
    from app.services.subscription_service import SubscriptionService

    user_id = request.args.get("user_id")
    state = request.args.get("state")

    subs = SubscriptionService.list_subscriptions(user_id=user_id, state=state)
    return jsonify(subs), 200


@subscriptions_bp.route("", methods=["POST"])
@require_auth
def create_subscription():
    """Create a new installment subscription.

    Request JSON:
        ``{ "user_id": "...", "total_amount": 50000, "installments": 4, "card_token": "..." }``

    Returns:
        201 — created subscription
        400 — validation error
    """
    data = request.get_json(silent=True) or {}

    # TODO: validate with marshmallow schema
    from app.services.subscription_service import SubscriptionService

    result, error = SubscriptionService.create(data)
    if error:
        return jsonify({"error": error}), 400

    return jsonify(result), 201
