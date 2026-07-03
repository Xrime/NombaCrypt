"""
NombaCrypt Shell — Legacy Path Simulator

Simulates Path A: Direct API routing WITHOUT NombaCrypt Shell buffering.
This intentionally shows failures (429s, timeouts, dropped connections)
to contrast against the NombaCrypt path in the split-screen comparison.
"""
import asyncio
import aiohttp
import time
from typing import Optional

from .traffic_generator import TrafficConfig, RequestResult
from .payload_factory import PayloadFactory
from .result_collector import ResultCollector


class LegacyPathSimulator:
    """
    Simulates what happens when a traffic surge hits a raw API endpoint
    without any buffering, rate-limiting, or queuing.
    
    Key differences from the NombaCrypt path:
    - No ring buffer (direct HTTP forwarding)
    - No rate-limit drip controller (sends all at once)
    - No retry logic (failures are permanent)
    - No parallel crypto verification
    """

    def __init__(self, config: TrafficConfig):
        self.config = config
        self.config.target_url = config.target_url.replace(
            "/api/ingest", "/api/legacy/direct"
        )
        self.payload_factory = PayloadFactory()
        self.collector = ResultCollector(path_name="Legacy")
        self._completed = 0

    async def run(self, on_progress=None) -> ResultCollector:
        """
        Execute the legacy path simulation.
        
        This sends ALL requests simultaneously without any throttling,
        which will trigger rate limits and timeouts.
        """
        # No semaphore limiting — blast everything at once (intentionally bad)
        connector = aiohttp.TCPConnector(limit=0)  # Unlimited connections
        timeout = aiohttp.ClientTimeout(total=10.0)  # Short timeout to show failures

        async with aiohttp.ClientSession(connector=connector, timeout=timeout) as session:
            tasks = [
                self._send_unprotected(session, i, on_progress)
                for i in range(self.config.total_requests)
            ]
            results = await asyncio.gather(*tasks, return_exceptions=True)

            for result in results:
                if isinstance(result, RequestResult):
                    self.collector.add_result(result)
                elif isinstance(result, Exception):
                    self.collector.add_error(str(result))

        return self.collector

    async def _send_unprotected(
        self,
        session: aiohttp.ClientSession,
        request_id: int,
        on_progress=None,
    ) -> RequestResult:
        """Send a single request with NO protection — raw API call."""
        payload = self.payload_factory.generate(request_id)
        start_time = time.perf_counter()

        try:
            async with session.post(
                self.config.target_url,
                json=payload,
            ) as response:
                elapsed_ms = (time.perf_counter() - start_time) * 1000
                self._completed += 1

                result = RequestResult(
                    request_id=request_id,
                    status_code=response.status,
                    latency_ms=elapsed_ms,
                    success=(response.status == 200),
                )

                if on_progress:
                    await on_progress(result)
                return result

        except (asyncio.TimeoutError, aiohttp.ClientError) as e:
            elapsed_ms = (time.perf_counter() - start_time) * 1000
            self._completed += 1
            error_type = "TIMEOUT" if isinstance(e, asyncio.TimeoutError) else str(e)
            return RequestResult(
                request_id=request_id,
                status_code=0,
                latency_ms=elapsed_ms,
                success=False,
                error=error_type,
            )
