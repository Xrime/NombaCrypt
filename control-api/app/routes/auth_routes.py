"""
app.routes.auth_routes — Authentication endpoints
==================================================
Blueprint: ``auth``
Prefix:    ``/api/auth``
"""

from flask import Blueprint, jsonify, request
from flask_jwt_extended import create_access_token, create_refresh_token

auth_bp = Blueprint("auth", __name__, url_prefix="/api/auth")


@auth_bp.route("/login", methods=["POST"])
def login():
    """Authenticate a user and return a JWT access + refresh token pair.

    Request JSON:
        ``{ "email": "...", "password": "..." }``

    Returns:
        200 — ``{ "access_token": "...", "refresh_token": "..." }``
        401 — Invalid credentials
    """
    # TODO: validate request body with marshmallow schema
    data = request.get_json(silent=True) or {}
    email = data.get("email")
    password = data.get("password")

    if not email or not password:
        return jsonify({"error": "email and password are required"}), 400

    # TODO: look up user in DB and verify password hash
    from app.services.auth_service import AuthService

    user = AuthService.login(email, password)
    if user is None:
        return jsonify({"error": "invalid credentials"}), 401

    access_token = create_access_token(identity=str(user.id))
    refresh_token = create_refresh_token(identity=str(user.id))

    return jsonify(
        {
            "access_token": access_token,
            "refresh_token": refresh_token,
            "user": {"id": str(user.id), "name": user.name, "role": user.role},
        }
    ), 200


@auth_bp.route("/token", methods=["POST"])
def refresh_token():
    """Exchange a valid refresh token for a new access token.

    Headers:
        ``Authorization: Bearer <refresh_token>``

    Returns:
        200 — ``{ "access_token": "..." }``
        401 — Token invalid / expired
    """
    # TODO: implement refresh-token rotation
    from flask_jwt_extended import get_jwt_identity, jwt_required

    @jwt_required(refresh=True)
    def _inner():
        identity = get_jwt_identity()
        new_token = create_access_token(identity=identity)
        return jsonify({"access_token": new_token}), 200

    return _inner()
