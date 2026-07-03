"""
NombaCrypt Shell — Result Streamer

Streams live simulation results to the frontend dashboard
via WebSocket for the real-time split-screen comparison.
"""
import asyncio
import json
import websockets
from typing import Optional


class ResultStreamer:
    """
    WebSocket streamer that sends live simulation progress
    to the frontend dashboard in real-time.
    
    Message format:
    {
        "type": "progress" | "complete",
        "path": "Legacy" | "NombaCrypt",
        "data": {
            "completed": 1234,
            "total": 5000,
            "latest_result": { ... },
            "running_summary": { ... }
        }
    }
    """

    def __init__(self, ws_url: str = "ws://localhost:5000/ws/simulation"):
        self.ws_url = ws_url
        self._ws: Optional[websockets.WebSocketClientProtocol] = None

    async def connect(self) -> None:
        """Establish WebSocket connection to the control API."""
        try:
            self._ws = await websockets.connect(self.ws_url)
        except Exception as e:
            print(f"[ResultStreamer] Failed to connect to {self.ws_url}: {e}")
            self._ws = None

    async def stream_progress(self, path_name: str, result, collector) -> None:
        """
        Stream a single progress update.
        
        Args:
            path_name: "Legacy" or "NombaCrypt"
            result: The latest RequestResult
            collector: The running ResultCollector
        """
        if not self._ws:
            return

        message = {
            "type": "progress",
            "path": path_name,
            "data": {
                "completed": collector.total,
                "latest_result": {
                    "request_id": result.request_id,
                    "status_code": result.status_code,
                    "latency_ms": round(result.latency_ms, 2),
                    "success": result.success,
                    "error": result.error,
                },
                "running_summary": collector.to_dict(),
            },
        }

        try:
            await self._ws.send(json.dumps(message))
        except websockets.ConnectionClosed:
            self._ws = None

    async def stream_complete(self, path_name: str, collector) -> None:
        """Stream the final summary when simulation completes."""
        if not self._ws:
            return

        message = {
            "type": "complete",
            "path": path_name,
            "data": collector.to_dict(),
        }

        try:
            await self._ws.send(json.dumps(message))
        except websockets.ConnectionClosed:
            self._ws = None

    async def close(self) -> None:
        """Close the WebSocket connection."""
        if self._ws:
            await self._ws.close()
            self._ws = None
