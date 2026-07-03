/**
 * @module constants
 * @description Application-wide constants for API URLs, WebSocket endpoints,
 * feature toggle names, and the colour palette.
 */

/* ──────────────────────────── API Endpoints ──────────────────────── */

/** Base URL for the Python REST API. */
export const API_BASE_URL = process.env.NEXT_PUBLIC_API_URL ?? 'http://localhost:8000';

/** REST resource paths (append to API_BASE_URL). */
export const API = {
  TELEMETRY: '/api/telemetry',
  TOGGLES: '/api/toggles',
  CHANNELS: '/api/channels',
  TRANSACTIONS: '/api/transactions',
  ACCOUNTS: '/api/accounts',
  BILLING: '/api/billing',
  SUBSCRIPTIONS: '/api/billing/subscriptions',
  RECONCILER: '/api/reconciler',
  DISCREPANCIES: '/api/reconciler/discrepancies',
  FORWARDER: '/api/forwarder/targets',
  SIMULATOR: '/api/simulator',
  SIMULATION_HISTORY: '/api/simulator/history',
  SECURITY_EVENTS: '/api/security/events',
  KILL_SWITCH: '/api/system/kill',
  EXPORT_CSV: '/api/transactions/export',
} as const;

/* ──────────────────────────── WebSocket ──────────────────────────── */

export const WS_BASE_URL = process.env.NEXT_PUBLIC_WS_URL ?? 'ws://localhost:8000';

export const WS = {
  TELEMETRY: `${WS_BASE_URL}/ws/telemetry`,
  TRANSACTIONS: `${WS_BASE_URL}/ws/transactions`,
  SIMULATION: `${WS_BASE_URL}/ws/simulation`,
  SECURITY: `${WS_BASE_URL}/ws/security`,
} as const;

/* ──────────────────────────── Feature Toggles ───────────────────── */

import type { ToggleName } from './types';

/** Ordered list of all feature toggle keys. */
export const TOGGLE_NAMES: ToggleName[] = [
  'hmac_verification',
  'payload_encryption',
  'rate_limiter',
  'circuit_breaker',
  'parallel_dispatch',
  'zero_copy_buffers',
  'batch_mode',
  'debug_logging',
];

/** Human-readable labels mapped by toggle key. */
export const TOGGLE_LABELS: Record<ToggleName, string> = {
  hmac_verification: 'HMAC Verification',
  payload_encryption: 'Payload Encryption',
  rate_limiter: 'Rate Limiter',
  circuit_breaker: 'Circuit Breaker',
  parallel_dispatch: 'Parallel Dispatch',
  zero_copy_buffers: 'Zero-Copy Buffers',
  batch_mode: 'Batch Mode',
  debug_logging: 'Debug Logging',
};

/** Tags indicating whether a toggle relates to SPEED or SECURITY. */
export const TOGGLE_TAGS: Record<ToggleName, 'SPEED' | 'SECURITY'> = {
  hmac_verification: 'SECURITY',
  payload_encryption: 'SECURITY',
  rate_limiter: 'SECURITY',
  circuit_breaker: 'SECURITY',
  parallel_dispatch: 'SPEED',
  zero_copy_buffers: 'SPEED',
  batch_mode: 'SPEED',
  debug_logging: 'SPEED',
};

/* ──────────────────────────── Colour Palette ────────────────────── */

export const COLORS = {
  bg: '#0a0a0f',
  bgCard: '#12121a',
  bgHover: '#1a1a2e',
  border: '#2a2a3e',
  text: '#f0f0f0',
  textMuted: '#8888aa',
  success: '#00ff88',
  alert: '#ff3366',
  info: '#3399ff',
  warning: '#ffaa00',
  accent: '#7b61ff',
} as const;

/* ──────────────────────────── Navigation ─────────────────────────── */

export interface NavItem {
  label: string;
  href: string;
  icon: string;
}

export const NAV_ITEMS: NavItem[] = [
  { label: 'Dashboard', href: '/', icon: '⚡' },
  { label: 'Transactions', href: '/transactions', icon: '📊' },
  { label: 'Channels', href: '/channels', icon: '🔗' },
  { label: 'Accounts', href: '/accounts', icon: '🏦' },
  { label: 'Billing', href: '/billing', icon: '💳' },
  { label: 'Reconciler', href: '/reconciler', icon: '🔍' },
  { label: 'Security', href: '/security', icon: '🛡️' },
  { label: 'Forwarder', href: '/forwarder', icon: '📡' },
  { label: 'Simulator', href: '/simulator', icon: '🧪' },
];
