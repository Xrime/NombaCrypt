"""
NombaCrypt Shell — NombaCrypt Path Simulator

Simulates Path B: Requests routed through the NombaCrypt Shell C++ engine.
This path should show smooth ingestion, zero drops, and consistent latency.
"""
import asyncio
import aiohttp
import time

from .traffic_generator import TrafficConfig, RequestResult
from .payload_factory import PayloadFactory
from .result_collector import ResultCollector


class NombaCryptPathSimulator:
    """
    Simulates traffic flowing through the NombaCrypt Shell engine.
    
    Key advantages demonstrated:
    - Ring buffer absorbs all requests instantly (HTTP 202)
    - Rate-limit drip controller prevents 429s
    - Multi-API dispatcher spreads load across channels
    - Zero packet drop under load
    """

    def __init__(self, config: TrafficConfig):
        self.config = config
        # Route through the C++ engine's ingest endpoint
        self.config.target_url = config.target_url or "http://localhost:8080/api/ingest"
        self.payload_factory = PayloadFactory()
        self.collector = ResultCollector(path_name="NombaCrypt")
        self._completed = 0

    async def run(self, on_progress=None) -> ResultCollector:
        """
        Execute the NombaCrypt path simulation.
        
        Sends requests through the C++ engine's ring buffer,
        which should accept all of them with HTTP 202 instantly.
        """
        semaphore = asyncio.Semaphore(self.config.concurrency_limit)
        connector = aiohttp.TCPConnector(limit=self.config.concurrency_limit)
        timeout = aiohttp.ClientTimeout(total=self.config.timeout_seconds)

        async with aiohttp.ClientSession(connector=connector, timeout=timeout) as session:
            tasks = [
                self._send_buffered(session, semaphore, i, on_progress)
                for i in range(self.config.total_requests)
            ]
            results = await asyncio.gather(*tasks, return_exceptions=True)

            for result in results:
                if isinstance(result, RequestResult):
                    self.collector.add_result(result)
                elif isinstance(result, Exception):
                    self.collector.add_error(str(result))

        return self.collector

    async def _send_buffered(
        self,
        session: aiohttp.ClientSession,
        semaphore: asyncio.Semaphore,
        request_id: int,
        on_progress=None,
    ) -> RequestResult:
        """Send a single request through the NombaCrypt buffer."""
        async with semaphore:
            payload = self.payload_factory.generate(request_id)
            start_time = time.perf_counter()

            try:
                async with session.post(
                    self.config.target_url,
                    json=payload,
                    headers={"Content-Type": "application/json"},
                ) as response:
                    elapsed_ms = (time.perf_counter() - start_time) * 1000
                    self._completed += 1

                    # The engine returns 202 Accepted when buffering is active
                    result = RequestResult(
                        request_id=request_id,
                        status_code=response.status,
                        latency_ms=elapsed_ms,
                        success=(response.status in (200, 202)),
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
