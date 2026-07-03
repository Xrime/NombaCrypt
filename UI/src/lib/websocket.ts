/**
 * @module websocket
 * @description Resilient WebSocket client with automatic reconnection,
 * exponential back-off, and typed message handling.
 */

import type { WSConnectionState, WSMessage } from './types';

/** Options for configuring the WebSocket client. */
export interface WSClientOptions {
  /** WebSocket server URL (ws:// or wss://). */
  url: string;
  /** Maximum number of reconnection attempts before giving up (default: 10). */
  maxRetries?: number;
  /** Base delay in ms between reconnection attempts (doubled each time, default: 1000). */
  baseDelayMs?: number;
  /** Callback fired when connection state changes. */
  onStateChange?: (state: WSConnectionState) => void;
  /** Callback fired on each incoming message. */
  onMessage?: (msg: WSMessage) => void;
  /** Callback fired on unrecoverable error. */
  onError?: (error: Event) => void;
}

/**
 * Managed WebSocket client with auto-reconnect.
 *
 * @example
 * ```ts
 * const ws = new WSClient({ url: WS.TELEMETRY, onMessage: handleMsg });
 * ws.connect();
 * // later …
 * ws.close();
 * ```
 */
export class WSClient {
  private ws: WebSocket | null = null;
  private retries = 0;
  private reconnectTimer: ReturnType<typeof setTimeout> | null = null;
  private readonly opts: Required<Omit<WSClientOptions, 'onStateChange' | 'onMessage' | 'onError'>> & WSClientOptions;

  constructor(options: WSClientOptions) {
    this.opts = {
      maxRetries: 10,
      baseDelayMs: 1_000,
      ...options,
    };
  }

  /** Open the WebSocket connection. */
  connect(): void {
    this.setConnectionState('CONNECTING');
    this.ws = new WebSocket(this.opts.url);

    this.ws.onopen = () => {
      this.retries = 0;
      this.setConnectionState('OPEN');
    };

    this.ws.onmessage = (event: MessageEvent) => {
      try {
        const parsed: WSMessage = JSON.parse(event.data as string);
        this.opts.onMessage?.(parsed);
      } catch {
        // Non-JSON payload — ignore or log
      }
    };

    this.ws.onerror = (event: Event) => {
      this.opts.onError?.(event);
    };

    this.ws.onclose = () => {
      this.setConnectionState('CLOSED');
      this.attemptReconnect();
    };
  }

  /** Send a JSON-serialisable payload to the server. */
  send<T>(data: T): void {
    if (this.ws?.readyState === WebSocket.OPEN) {
      this.ws.send(JSON.stringify(data));
    }
  }

  /** Gracefully close the connection and stop reconnection attempts. */
  close(): void {
    this.clearReconnectTimer();
    this.setConnectionState('CLOSING');
    this.ws?.close();
    this.ws = null;
  }

  /* ─── Private helpers ────────────────────────────────────────────── */

  private attemptReconnect(): void {
    if (this.retries >= this.opts.maxRetries) return;
    this.retries += 1;
    const delay = this.opts.baseDelayMs * Math.pow(2, this.retries - 1);
    this.setConnectionState('RECONNECTING');
    this.reconnectTimer = setTimeout(() => this.connect(), delay);
  }

  private clearReconnectTimer(): void {
    if (this.reconnectTimer) {
      clearTimeout(this.reconnectTimer);
      this.reconnectTimer = null;
    }
  }

  private setConnectionState(state: WSConnectionState): void {
    this.opts.onStateChange?.(state);
  }
}
