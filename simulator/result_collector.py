"""
NombaCrypt Shell — Result Collector

Aggregates simulation results and computes summary metrics
for the split-screen comparison display.
"""
import statistics
from dataclasses import dataclass, field
from typing import Optional


@dataclass
class SimulationSummary:
    """Aggregated summary of a simulation run."""
    path_name: str
    total_requests: int
    success_count: int
    timeout_count: int
    rate_limited_count: int  # HTTP 429
    server_error_count: int  # HTTP 5xx
    dropped_count: int       # Connection errors
    avg_latency_ms: float
    p50_latency_ms: float
    p95_latency_ms: float
    p99_latency_ms: float
    min_latency_ms: float
    max_latency_ms: float
    total_duration_ms: float
    requests_per_second: float
    success_rate_percent: float


class ResultCollector:
    """
    Collects individual request results and computes aggregate metrics.
    
    Used by both the Legacy and NombaCrypt path simulators to produce
    comparable summary statistics for the split-screen UI.
    """

    def __init__(self, path_name: str = "Unknown"):
        self.path_name = path_name
        self._results: list = []
        self._errors: list[str] = []
        self._start_time: Optional[float] = None
        self._end_time: Optional[float] = None

    def add_result(self, result) -> None:
        """Record a single request result."""
        if not self._results:
            self._start_time = result.timestamp
        self._end_time = result.timestamp
        self._results.append(result)

    def add_error(self, error: str) -> None:
        """Record an unhandled error."""
        self._errors.append(error)

    @property
    def total(self) -> int:
        return len(self._results) + len(self._errors)

    @property
    def success_count(self) -> int:
        return sum(1 for r in self._results if r.success)

    @property
    def timeout_count(self) -> int:
        return sum(1 for r in self._results if r.error == "TIMEOUT")

    @property
    def rate_limited_count(self) -> int:
        return sum(1 for r in self._results if r.status_code == 429)

    @property
    def server_error_count(self) -> int:
        return sum(1 for r in self._results if 500 <= r.status_code < 600)

    @property
    def dropped_count(self) -> int:
        return sum(
            1 for r in self._results
            if r.status_code == 0 and r.error != "TIMEOUT"
        ) + len(self._errors)

    def get_latencies(self) -> list[float]:
        """Get all latency values in ms."""
        return [r.latency_ms for r in self._results]

    def summarize(self) -> SimulationSummary:
        """Compute the full simulation summary."""
        latencies = self.get_latencies()

        if not latencies:
            return SimulationSummary(
                path_name=self.path_name,
                total_requests=0,
                success_count=0, timeout_count=0,
                rate_limited_count=0, server_error_count=0,
                dropped_count=0,
                avg_latency_ms=0, p50_latency_ms=0,
                p95_latency_ms=0, p99_latency_ms=0,
                min_latency_ms=0, max_latency_ms=0,
                total_duration_ms=0, requests_per_second=0,
                success_rate_percent=0,
            )

        sorted_latencies = sorted(latencies)
        total_duration_ms = (
            (self._end_time - self._start_time) * 1000
            if self._start_time and self._end_time
            else sum(latencies)
        )

        def percentile(data, pct):
            idx = int(len(data) * pct / 100)
            return data[min(idx, len(data) - 1)]

        total = self.total
        rps = (total / (total_duration_ms / 1000)) if total_duration_ms > 0 else 0

        return SimulationSummary(
            path_name=self.path_name,
            total_requests=total,
            success_count=self.success_count,
            timeout_count=self.timeout_count,
            rate_limited_count=self.rate_limited_count,
            server_error_count=self.server_error_count,
            dropped_count=self.dropped_count,
            avg_latency_ms=statistics.mean(latencies),
            p50_latency_ms=percentile(sorted_latencies, 50),
            p95_latency_ms=percentile(sorted_latencies, 95),
            p99_latency_ms=percentile(sorted_latencies, 99),
            min_latency_ms=min(latencies),
            max_latency_ms=max(latencies),
            total_duration_ms=total_duration_ms,
            requests_per_second=rps,
            success_rate_percent=(self.success_count / total * 100) if total > 0 else 0,
        )

    def to_dict(self) -> dict:
        """Serialize summary to dictionary for JSON transmission."""
        summary = self.summarize()
        return {
            "path_name": summary.path_name,
            "total_requests": summary.total_requests,
            "success_count": summary.success_count,
            "timeout_count": summary.timeout_count,
            "rate_limited_count": summary.rate_limited_count,
            "server_error_count": summary.server_error_count,
            "dropped_count": summary.dropped_count,
            "avg_latency_ms": round(summary.avg_latency_ms, 2),
            "p50_latency_ms": round(summary.p50_latency_ms, 2),
            "p95_latency_ms": round(summary.p95_latency_ms, 2),
            "p99_latency_ms": round(summary.p99_latency_ms, 2),
            "requests_per_second": round(summary.requests_per_second, 2),
            "success_rate_percent": round(summary.success_rate_percent, 2),
        }
