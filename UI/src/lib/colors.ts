/**
 * @module colors
 * @description Semantic colour constants used by recharts, badges, and other
 * visual elements across the NombaCrypt Shell UI.
 */

/* ─── Core Theme Colours ─────────────────────────────────────────── */

export const THEME = {
  bg: '#0a0a0f',
  bgCard: '#12121a',
  bgHover: '#1a1a2e',
  border: '#2a2a3e',
  text: '#f0f0f0',
  textMuted: '#8888aa',
} as const;

/* ─── Status Colours ─────────────────────────────────────────────── */

export const STATUS = {
  success: '#00ff88',
  warning: '#ffaa00',
  error: '#ff3366',
  info: '#3399ff',
} as const;

/* ─── Chart Colour Palette ───────────────────────────────────────── */

/** Ordered palette for multi-series recharts. */
export const CHART_PALETTE = [
  '#00ff88', // green
  '#3399ff', // blue
  '#ff3366', // red
  '#ffaa00', // amber
  '#7b61ff', // purple
  '#00ccdd', // cyan
  '#ff88cc', // pink
  '#88ff44', // lime
] as const;

/** Thread-pool specific colours for ThreadAllocationChart. */
export const THREAD_COLORS: Record<string, string> = {
  ingest: '#3399ff',
  crypto: '#7b61ff',
  dispatch: '#00ff88',
};

/* ─── Badge Variant Map ──────────────────────────────────────────── */

export const BADGE_COLORS: Record<string, { bg: string; text: string }> = {
  success: { bg: 'rgba(0, 255, 136, 0.15)', text: '#00ff88' },
  warning: { bg: 'rgba(255, 170, 0, 0.15)', text: '#ffaa00' },
  error: { bg: 'rgba(255, 51, 102, 0.15)', text: '#ff3366' },
  info: { bg: 'rgba(51, 153, 255, 0.15)', text: '#3399ff' },
};

/* ─── Gauge Thresholds ───────────────────────────────────────────── */

/** Return the appropriate colour for a buffer-fill percentage. */
export function gaugeColor(percent: number): string {
  if (percent < 60) return STATUS.success;
  if (percent <= 85) return STATUS.warning;
  return STATUS.error;
}
