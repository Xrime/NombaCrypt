from flask import Blueprint, jsonify
from app.services.engine_bridge import EngineBridge

# A Blueprint is Flask's way of grouping related routes together
telemetry_bp = Blueprint('telemetry', __name__)

@telemetry_bp.route('/api/telemetry/snapshot', methods=['GET'])
def get_snapshot():
    """
    Proxies the request from the React frontend down to the
    high-speed C++ Engine to get the live Ring Buffer stats.
    """
    # Use the bridge from Step 18
    data = EngineBridge.get_telemetry()

    # If the C++ engine gave us an error or is offline,
    # we still return a 200 OK to the frontend so the UI doesn't crash,
    # but the data will show 0s.
    return jsonify(data), 200

@telemetry_bp.route('/api/simulate/start', methods=['POST'])
def start_simulation():
    """
    Tells the C++ Engine to blast dummy traffic for the hackathon demo.
    """
    # Hardcode 1,000,000 requests for the demo button
    success = EngineBridge.trigger_simulation(1000000)

    if success:
        return jsonify({"message": "Simulation started successfully"}), 200
    else:
        return jsonify({"error": "Failed to reach C++ Engine"}), 503