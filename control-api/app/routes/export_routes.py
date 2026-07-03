"""
app.routes.export_routes — Ledger export endpoints
====================================================
Blueprint: ``export``
Prefix:    ``/api/ledger``
"""

from flask import Blueprint, Response, request

from app.middleware.auth_middleware import require_auth

export_bp = Blueprint("export", __name__, url_prefix="/api/ledger")


@export_bp.route("/export", methods=["GET"])
@require_auth
def export_csv():
    """Export the event log as a downloadable CSV file.

    Query params:
        ``since`` — (optional) ISO-8601 timestamp lower-bound
        ``until`` — (optional) ISO-8601 timestamp upper-bound
        ``event_type`` — (optional) filter by event type

    Returns:
        200 — ``text/csv`` attachment
    """
    from app.services.export_service import ExportService

    since = request.args.get("since")
    until = request.args.get("until")
    event_type = request.args.get("event_type")

    csv_content = ExportService.generate_csv(
        since=since,
        until=until,
        event_type=event_type,
    )

    return Response(
        csv_content,
        mimetype="text/csv",
        headers={"Content-Disposition": "attachment; filename=ledger_export.csv"},
    )
