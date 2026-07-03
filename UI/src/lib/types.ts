/**
 * @module types
 * @description Core TypeScript interfaces for the NombaCrypt Shell UI.
 * All domain models used across hooks, contexts, and components.
 */

/* ──────────────────────────── Telemetry ──────────────────────────── */

/** Snapshot of the internal ring-buffer telemetry. */
export interface TelemetrySnapshot {
  /** Current buffer fill percentage (0–100). */
  bufferPercent: number;
  /** Active thread counts per pool. */
  threads: ThreadAllocation;
  /** Average processing latency in microseconds. */
  avgLatencyUs: number;
  /** Current throughput in requests per second. */
  throughputRps: number;
  /** ISO-8601 timestamp of the snapshot. */
  timestamp: string;
}

export interface ThreadAllocation {
  ingest: number;
  crypto: number;
  dispatch: number;
}

/** Aggregate pipeline statistics from the backend. */
export interface PipelineStats {
  totalProcessed: number;
  totalDropped: number;
  totalTimeouts: number;
  uptimeSeconds: number;
}

/** Health status levels used across the UI. */
export type HealthStatus = 'OK' | 'WARNING' | 'CRITICAL';

/* ──────────────────────────── Channels ──────────────────────────── */

/** A single API channel (Multi-API provider). */
export interface ChannelSnapshot {
  id: string;
  alias: string;
  health: ChannelHealth;
  /** Rate-limit headroom as a percentage (0–100). */
  headroomPercent: number;
  dispatchedCount: number;
  lastDispatchedAt: string | null;
}

export type ChannelHealth = 'HEALTHY' | 'THROTTLED' | 'DOWN';

export interface ChannelCreatePayload {
  alias: string;
  clientId: string;
  privateKey: string;
}

/* ──────────────────────────── Transactions ──────────────────────── */

/** Single processed transaction. */
export interface Transaction {
  txId: string;
  amount: number;
  currency: string;
  latencyUs: number;
  status: TransactionStatus;
  timestamp: string;
  channelAlias: string;
}

export type TransactionStatus = 'SUCCESS' | 'FAILED' | 'PENDING' | 'TIMEOUT';

export interface TransactionFilter {
  status?: TransactionStatus;
  channelAlias?: string;
  startDate?: string;
  endDate?: string;
  page: number;
  pageSize: number;
}

export interface PaginatedResponse<T> {
  data: T[];
  total: number;
  page: number;
  pageSize: number;
}

/* ──────────────────────────── Feature Toggles ────────────────────── */

/** A single runtime feature toggle. */
export interface FeatureToggle {
  name: string;
  enabled: boolean;
  description: string;
  /** Visual tag for UI categorisation. */
  tag: 'SPEED' | 'SECURITY';
}

/** All eight feature toggles used in the system. */
export type ToggleName =
  | 'hmac_verification'
  | 'payload_encryption'
  | 'rate_limiter'
  | 'circuit_breaker'
  | 'parallel_dispatch'
  | 'zero_copy_buffers'
  | 'batch_mode'
  | 'debug_logging';

/* ──────────────────────────── NUBAN / Accounts ───────────────────── */

export interface VirtualAccount {
  id: string;
  studentName: string;
  nuban: string;
  bankName: string;
  status: 'ACTIVE' | 'INACTIVE';
  createdAt: string;
}

export interface ProvisionAccountPayload {
  studentName: string;
  bankCode: string;
  email: string;
}

/* ──────────────────────────── Billing ─────────────────────────────── */

export interface Subscription {
  id: string;
  studentName: string;
  planName: string;
  totalAmount: number;
  paidAmount: number;
  state: SubscriptionState;
  nextDueDate: string | null;
  installments: Installment[];
}

export type SubscriptionState = 'PENDING' | 'PARTIALLY_PAID' | 'SETTLED';

export interface Installment {
  id: string;
  amount: number;
  dueDate: string;
  paidAt: string | null;
  status: 'UPCOMING' | 'PAID' | 'OVERDUE';
}

export interface InstallmentPlanConfig {
  splitType: '50/50' | '60/40' | 'CUSTOM';
  customSplits?: number[];
  dueDates: string[];
}

/* ──────────────────────────── Reconciler ──────────────────────────── */

export interface Discrepancy {
  id: string;
  txId: string;
  type: DiscrepancyType;
  expectedAmount: number;
  actualAmount: number;
  variance: number;
  detectedAt: string;
  resolved: boolean;
}

export type DiscrepancyType = 'UNDERPAID' | 'OVERPAID' | 'MISSING';

export interface DunningStats {
  totalOutstanding: number;
  recoveredCount: number;
  totalCount: number;
  recoveryRatePercent: number;
}

/* ──────────────────────────── Forwarder ───────────────────────────── */

export interface WebhookTarget {
  id: string;
  url: string;
  authHeader: string | null;
  lastStatus: number | null;
  lastDeliveredAt: string | null;
  active: boolean;
}

export interface WebhookTargetCreatePayload {
  url: string;
  authHeader?: string;
}

/* ──────────────────────────── Simulator ───────────────────────────── */

export interface SimulationConfig {
  requestCount: number;
  concurrency: number;
  payloadSizeBytes: number;
  payloadTemplate?: string;
}

export interface SimulationResult {
  id: string;
  config: SimulationConfig;
  legacy: PathResult;
  nombacrypt: PathResult;
  startedAt: string;
  completedAt: string;
}

export interface PathResult {
  successCount: number;
  timeoutCount: number;
  droppedCount: number;
  avgLatencyUs: number;
}

export interface SimulationProgress {
  completedRequests: number;
  totalRequests: number;
  legacyProgress: number;
  nombacryptProgress: number;
}

/* ──────────────────────────── Security ────────────────────────────── */

export interface SecurityEvent {
  id: string;
  type: SecurityEventType;
  sourceIp: string;
  message: string;
  timestamp: string;
  blocked: boolean;
}

export type SecurityEventType = 'HMAC_FAIL' | 'REPLAY_ATTACK' | 'RATE_LIMIT' | 'INTRUSION' | 'VERIFICATION_OK';

/* ──────────────────────────── WebSocket ───────────────────────────── */

export type WSConnectionState = 'CONNECTING' | 'OPEN' | 'CLOSING' | 'CLOSED' | 'RECONNECTING';

export interface WSMessage<T = unknown> {
  event: string;
  data: T;
  timestamp: string;
}
