"""
app.routes.reconciler_routes — Reconciliation endpoints
========================================================
Blueprint: ``reconciler``
Prefix:    ``/api/reconciler``
"""

from flask import Blueprint, jsonify, request

from app.middleware.auth_middleware import require_auth

reconciler_bp = Blueprint("reconciler", __name__, url_prefix="/api/reconciler")


@reconciler_bp.route("/discrepancies", methods=["GET"])
@require_auth
def get_discrepancies():
    """Fetch current ledger-vs-settlement discrepancies.

    Query params:
        ``since`` — (optional) ISO-8601 timestamp lower-bound
        ``limit`` — (optional) max results

    Returns:
        200 — ``[ { "tx_id": "...", "expected": ..., "actual": ..., "diff": ... }, ... ]``
    """
    from app.services.reconciler_service import ReconcilerService

    since = request.args.get("since")
    limit = request.args.get("limit", 100, type=int)

    discrepancies = ReconcilerService.fetch_discrepancies(since=since, limit=limit)
    return jsonify(discrepancies), 200
