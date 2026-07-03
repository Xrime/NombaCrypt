"""
app.routes.transaction_routes — Transaction query endpoints
============================================================
Blueprint: ``transactions``
Prefix:    ``/api/transactions``
"""

from flask import Blueprint, jsonify, request

from app.middleware.auth_middleware import require_auth

transactions_bp = Blueprint("transactions", __name__, url_prefix="/api/transactions")


@transactions_bp.route("", methods=["GET"])
@require_auth
def list_transactions():
    """Return a paginated list of transactions.

    Query params:
        ``cursor``    — (optional) cursor for pagination
        ``limit``     — (optional) page size (default 50)
        ``status``    — (optional) filter by status
        ``user_id``   — (optional) filter by user UUID

    Returns:
        200 — ``{ "data": [...], "next_cursor": "..." }``
    """
    from app.services.transaction_service import TransactionService

    cursor = request.args.get("cursor")
    limit = request.args.get("limit", 50, type=int)
    status = request.args.get("status")
    user_id = request.args.get("user_id")

    result = TransactionService.list_transactions(
        cursor=cursor,
        limit=limit,
        status=status,
        user_id=user_id,
    )
    return jsonify(result), 200


@transactions_bp.route("/<string:tx_id>", methods=["GET"])
@require_auth
def get_transaction(tx_id: str):
    """Retrieve a single transaction by ID.

    Returns:
        200 — Transaction object
        404 — Not found
    """
    from app.services.transaction_service import TransactionService

    tx = TransactionService.get_by_id(tx_id)
    if tx is None:
        return jsonify({"error": "transaction not found"}), 404

    return jsonify(tx), 200
