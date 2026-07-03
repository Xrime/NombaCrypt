from flask import Blueprint, request, jsonify
from app.services.nomba_service import NombaService

# Group these routes under the 'nuban' blueprint
nuban_bp = Blueprint('nuban', __name__)

@nuban_bp.route('/api/accounts/provision', methods=['POST'])
def provision_account():
    """
    Receives a request from the React frontend to create a new student virtual account.
    Passes the data to the NombaService.
    """
    data = request.get_json()


    if not data or not data.get('accountName') or not data.get('bvn'):
        return jsonify({"error": "accountName and bvn are required"}), 400

    account_name = data['accountName']
    bvn = data['bvn']


    result = NombaService.provision_virtual_account(bvn, account_name)

    if result.get("success"):

        return jsonify(result), 201
    else:
        return jsonify({"error": "Failed to provision account"}), 500