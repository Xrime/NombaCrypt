"""
app.routes.dunning_routes — Dunning / retry-metric endpoints
=============================================================
Blueprint: ``dunning``
Prefix:    ``/api/dunning``
"""

from flask import Blueprint, jsonify

from app.middleware.auth_middleware import require_auth

dunning_bp = Blueprint("dunning", __name__, url_prefix="/api/dunning")


@dunning_bp.route("/metrics", methods=["GET"])
@require_auth
def dunning_metrics():
    """Return aggregated dunning metrics (retry rates, recovery, churn).

    Returns:
        200 — ``{ "total_retries": ..., "recovered": ..., "churned": ..., ... }``
    """
    from app.services.dunning_service import DunningService

    metrics = DunningService.aggregate_metrics()
    return jsonify(metrics), 200
