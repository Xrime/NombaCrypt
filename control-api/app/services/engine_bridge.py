import requests
from config.settings import Config

class EngineBridge:
    @staticmethod
    def get_telemetry():
        """Fetch real-time metrics from the C++ ring buffer."""
        try:
            # We will build this endpoint in the C++ engine later
            response = requests.get(f"{Config.ENGINE_BASE_URL}/api/telemetry", timeout=2.0)
            if response.status_code == 200:
                return response.json()
            return {"error": f"Engine returned {response.status_code}"}

        except requests.exceptions.RequestException:
            # If the C++ engine isn't running yet, return safe fallback data
            return {
                "active_count": 0,
                "capacity_percent": 0.0,
                "total_enqueued": 0,
                "total_dispatched": 0,
                "status": "ENGINE_OFFLINE"
            }

    @staticmethod
    def trigger_simulation(requests_to_send: int):

        try:
            response = requests.post(
                f"{Config.ENGINE_BASE_URL}/api/simulate",
                json={"count": requests_to_send},
                timeout=5.0
            )
            return response.status_code == 200
        except requests.exceptions.RequestException:
            return False