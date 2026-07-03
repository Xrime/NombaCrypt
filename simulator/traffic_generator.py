"""
NombaCrypt Shell — Async Traffic Generator

Core module that blasts N concurrent HTTP requests to test system throughput.
Uses asyncio + aiohttp for maximum concurrency.
"""
import asyncio
import aiohttp
import time
import json
from typing import Optional
from dataclasses import dataclass, field

from .payload_factory import PayloadFactory
from .result_collector import ResultCollector


@dataclass
class TrafficConfig:
    """Configuration for a traffic generation run."""
    total_requests: int = 5000
    concurrency_limit: int = 500
    target_url: str = "http://localhost:8080/api/ingest"
    payload_template: Optional[dict] = None
    timeout_seconds: float = 30.0


@dataclass
class RequestResult:
    """Result of a single HTTP request."""
    request_id: int
    status_code: int
    latency_ms: float
    success: bool
    error: Optional[str] = None
    timestamp: float = field(default_factory=time.time)


class TrafficGenerator:
    """
    Asynchronous traffic generator that blasts concurrent requests.
    
    Used by the simulation UI to demonstrate NombaCrypt Shell's
    high-throughput buffering vs. legacy direct API routing.
    """

    def __init__(self, config: TrafficConfig):
        self.config = config
        self.payload_factory = PayloadFactory()
        self.collector = ResultCollector()
        self._running = False
        self._completed = 0
        self._on_progress_callback = None

    @property
    def progress_percent(self) -> float:
        """Current completion percentage."""
        if self.config.total_requests == 0:
            return 100.0
        return (self._completed / self.config.total_requests) * 100.0

    def on_progress(self, callback):
        """Register a callback for progress updates. Called per completed request."""
        self._on_progress_callback = callback

    async def run(self) -> ResultCollector:
        """
        Execute the full traffic blast.
        
        Returns:
            ResultCollector with aggregated metrics.
        """
        self._running = True
        self._completed = 0

        semaphore = asyncio.Semaphore(self.config.concurrency_limit)
        connector = aiohttp.TCPConnector(limit=self.config.concurrency_limit)
        timeout = aiohttp.ClientTimeout(total=self.config.timeout_seconds)

        async with aiohttp.ClientSession(connector=connector, timeout=timeout) as session:
            tasks = [
                self._send_request(session, semaphore, i)
                for i in range(self.config.total_requests)
            ]
            results = await asyncio.gather(*tasks, return_exceptions=True)

            for result in results:
                if isinstance(result, RequestResult):
                    self.collector.add_result(result)
                elif isinstance(result, Exception):
                    self.collector.add_error(str(result))

        self._running = False
        return self.collector

    async def _send_request(
        self, 
        session: aiohttp.ClientSession,
        semaphore: asyncio.Semaphore,
        request_id: int
    ) -> RequestResult:
        """Send a single request with semaphore-controlled concurrency."""
        async with semaphore:
            payload = self.payload_factory.generate(request_id)
            start_time = time.perf_counter()

            try:
                async with session.post(
                    self.config.target_url,
                    json=payload,
                    headers={"Content-Type": "application/json"}
                ) as response:
                    elapsed_ms = (time.perf_counter() - start_time) * 1000
                    self._completed += 1

                    result = RequestResult(
                        request_id=request_id,
                        status_code=response.status,
                        latency_ms=elapsed_ms,
                        success=(response.status in (200, 202)),
                    )

                    if self._on_progress_callback:
                        await self._on_progress_callback(result)

                    return result

            except asyncio.TimeoutError:
                elapsed_ms = (time.perf_counter() - start_time) * 1000
                self._completed += 1
                return RequestResult(
                    request_id=request_id,
                    status_code=0,
                    latency_ms=elapsed_ms,
                    success=False,
                    error="TIMEOUT",
                )
            except aiohttp.ClientError as e:
                elapsed_ms = (time.perf_counter() - start_time) * 1000
                self._completed += 1
                return RequestResult(
                    request_id=request_id,
                    status_code=0,
                    latency_ms=elapsed_ms,
                    success=False,
                    error=str(e),
                )

    def stop(self):
        """Signal the generator to stop."""
        self._running = False
