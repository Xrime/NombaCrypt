/**
 * @module formatters
 * @description Utility functions for formatting currency, latency,
 * timestamps, and identifiers across the dashboard UI.
 */

/**
 * Format a number as Nigerian Naira (₦).
 * @param amount - Raw numeric amount
 * @param decimals - Decimal places (default 2)
 * @returns Formatted string, e.g. "₦1,250.00"
 */
export function formatCurrency(amount: number, decimals = 2): string {
  return `₦${amount.toLocaleString('en-NG', {
    minimumFractionDigits: decimals,
    maximumFractionDigits: decimals,
  })}`;
}

/**
 * Format a latency value given in microseconds into a human-readable string.
 * Values ≥ 1 000 μs are shown as milliseconds; ≥ 1 000 000 μs as seconds.
 *
 * @param us - Latency in microseconds
 * @returns e.g. "342 μs", "12.5 ms", "1.02 s"
 */
export function formatLatency(us: number): string {
  if (us < 1_000) return `${us.toFixed(0)} μs`;
  if (us < 1_000_000) return `${(us / 1_000).toFixed(1)} ms`;
  return `${(us / 1_000_000).toFixed(2)} s`;
}

/**
 * Format an ISO-8601 timestamp to a short locale string.
 * @param iso - ISO timestamp string
 * @returns e.g. "03 Jul 2026, 14:32:05"
 */
export function formatTimestamp(iso: string): string {
  const date = new Date(iso);
  return date.toLocaleDateString('en-GB', {
    day: '2-digit',
    month: 'short',
    year: 'numeric',
  }) + ', ' + date.toLocaleTimeString('en-GB', {
    hour: '2-digit',
    minute: '2-digit',
    second: '2-digit',
  });
}

/**
 * Truncate a UUID/transaction-id for display.
 * @param id - Full identifier string
 * @param chars - Number of leading characters to keep (default 8)
 * @returns e.g. "a3f8c2d1…"
 */
export function truncateId(id: string, chars = 8): string {
  if (id.length <= chars) return id;
  return `${id.slice(0, chars)}…`;
}

/**
 * Format a percentage value.
 * @param value - 0–100
 * @param decimals - Decimal places (default 1)
 */
export function formatPercent(value: number, decimals = 1): string {
  return `${value.toFixed(decimals)}%`;
}

/**
 * Format an uptime duration given in seconds.
 * @param seconds - Total seconds
 * @returns e.g. "2d 14h 32m"
 */
export function formatUptime(seconds: number): string {
  const d = Math.floor(seconds / 86_400);
  const h = Math.floor((seconds % 86_400) / 3_600);
  const m = Math.floor((seconds % 3_600) / 60);
  const parts: string[] = [];
  if (d > 0) parts.push(`${d}d`);
  if (h > 0) parts.push(`${h}h`);
  parts.push(`${m}m`);
  return parts.join(' ');
}
