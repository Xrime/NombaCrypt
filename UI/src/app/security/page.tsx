'use client';

import { useState, useEffect, useRef } from 'react';
import { formatCurrency, formatLatency, formatTimestamp, truncateId } from '../../lib/formatters';

type SecurityEventType = 'HMAC_FAIL' | 'REPLAY_ATTACK' | 'RATE_LIMIT' | 'INTRUSION' | 'VERIFICATION_OK';

interface SecurityEvent {
  id: string;
  type: SecurityEventType;
  sourceIp: string;
  message: string;
  timestamp: string;
  blocked: boolean;
}

const IPS = ['41.190.2.45', '105.112.76.88', '197.210.55.12', '102.89.33.91', '154.118.42.66', '169.159.88.21', '41.58.100.3', '105.67.0.99'];
const MSGS: Record<SecurityEventType, string[]> = {
  VERIFICATION_OK: ['HMAC-SHA256 signature verified in 2.1μs', 'Payload hash matches. Forwarding to dispatcher.', 'Webhook signature valid. Crypto thread released.', 'Signature match confirmed. Slot promoted to PROCESSING.'],
  HMAC_FAIL: ['Signature mismatch on checkout webhook.', 'Invalid HMAC header detected. Payload rejected.', 'Crypto verification failed: header tampered.'],
  REPLAY_ATTACK: ['Duplicate transaction hash detected in ledger.', 'Replay attempt: timestamp sequence violation.', 'Anti-replay: hash collision on tx fingerprint.'],
  RATE_LIMIT: ['Rate limit exceeded from source. 429 returned.', 'Token bucket depleted for channel. Throttled.'],
  INTRUSION: ['Spoofed success webhook intercepted.', 'Forged payment confirmation dropped.', 'Unauthorized callback signature. Connection reset.'],
};

function generateEvent(): SecurityEvent {
  const rand = Math.random();
  let type: SecurityEventType;
  if (rand < 0.6) type = 'VERIFICATION_OK';
  else if (rand < 0.75) type = 'HMAC_FAIL';
  else if (rand < 0.85) type = 'REPLAY_ATTACK';
  else if (rand < 0.93) type = 'RATE_LIMIT';
  else type = 'INTRUSION';

  const msgs = MSGS[type];
  return {
    id: crypto.randomUUID(),
    type,
    sourceIp: IPS[Math.floor(Math.random() * IPS.length)],
    message: msgs[Math.floor(Math.random() * msgs.length)],
    timestamp: new Date().toISOString(),
    blocked: type !== 'VERIFICATION_OK',
  };
}

export default function SecurityPage() {
  const [events, setEvents] = useState<SecurityEvent[]>([]);
  const [flashVisible, setFlashVisible] = useState(false);
  const [stats, setStats] = useState({ verified: 14832, blocked: 47, avgUs: 2.3 });
  const termRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    const interval = setInterval(() => {
      const ev = generateEvent();
      setEvents(prev => [ev, ...prev].slice(0, 100));
      if (ev.blocked) {
        setStats(prev => ({ ...prev, blocked: prev.blocked + 1 }));
      } else {
        setStats(prev => ({ ...prev, verified: prev.verified + 1, avgUs: +(1.5 + Math.random() * 3).toFixed(1) }));
      }
    }, 1200);
    return () => clearInterval(interval);
  }, []);

  const simulateAttack = () => {
    const attack: SecurityEvent = {
      id: crypto.randomUUID(),
      type: 'INTRUSION',
      sourceIp: '41.190.2.45',
      message: '⚠️ SPOOFED SUCCESS WEBHOOK — Forged payment confirmation intercepted and DROPPED.',
      timestamp: new Date().toISOString(),
      blocked: true,
    };
    setEvents(prev => [attack, ...prev].slice(0, 100));
    setStats(prev => ({ ...prev, blocked: prev.blocked + 1 }));
    setFlashVisible(true);
    setTimeout(() => setFlashVisible(false), 3000);
  };

  const typeColor = (t: SecurityEventType) => {
    switch (t) {
      case 'VERIFICATION_OK': return 'var(--success)';
      case 'HMAC_FAIL': return 'var(--alert)';
      case 'REPLAY_ATTACK': return 'var(--warning)';
      case 'RATE_LIMIT': return 'var(--warning)';
      case 'INTRUSION': return 'var(--alert)';
    }
  };

  return (
    <div className="animate-in">
      {flashVisible && (
        <div className="flash-alert">
          🛡️ INTRUSION BLOCKED — Spoofed webhook dropped by Crypto Shield
        </div>
      )}

      <div className="page-header">
        <h2>Microsecond Security Shield</h2>
        <p>Multi-threaded HMAC-SHA256 verification and anti-replay protection.</p>
      </div>

      <div className="metrics-grid" style={{ gridTemplateColumns: 'repeat(4, 1fr)' }}>
        <div className="metric-card">
          <div className="metric-icon">✅</div>
          <div className="metric-value green">{stats.verified.toLocaleString()}</div>
          <div className="metric-label">Verified Payloads</div>
        </div>
        <div className="metric-card">
          <div className="metric-icon">🛡️</div>
          <div className="metric-value red">{stats.blocked}</div>
          <div className="metric-label">Attacks Blocked</div>
        </div>
        <div className="metric-card">
          <div className="metric-icon">⚡</div>
          <div className="metric-value purple">{stats.avgUs} μs</div>
          <div className="metric-label">Avg Verification Time</div>
        </div>
        <div className="metric-card">
          <div className="metric-icon">🧵</div>
          <div className="metric-value blue">4</div>
          <div className="metric-label">Crypto Threads Active</div>
        </div>
      </div>

      <div style={{ marginBottom: '24px' }}>
        <button className="btn btn-danger btn-lg" onClick={simulateAttack}>
          ⚡ Simulate Webhook Attack
        </button>
      </div>

      <div className="grid-2">
        <div>
          <div className="section-header">
            <h3>Live Verification Terminal</h3>
            <span className="badge badge-success">STREAMING</span>
          </div>
          <div className="terminal" ref={termRef} style={{ maxHeight: '500px' }}>
            {events.map(ev => (
              <div key={ev.id} className="terminal-line">
                <span className="ts">[{new Date(ev.timestamp).toLocaleTimeString()}]</span>
                <span style={{ color: typeColor(ev.type), minWidth: '120px', fontWeight: 600, fontSize: '0.72rem' }}>
                  {ev.type}
                </span>
                <span style={{ color: 'var(--text-muted)', minWidth: '110px' }}>{ev.sourceIp}</span>
                <span className="msg">{ev.message}</span>
              </div>
            ))}
          </div>
        </div>

        <div>
          <div className="section-header">
            <h3>Immutable Attack Ledger</h3>
          </div>
          <div className="card" style={{ padding: 0, overflow: 'hidden' }}>
            <table className="data-table">
              <thead>
                <tr>
                  <th>Time</th>
                  <th>Type</th>
                  <th>Source IP</th>
                  <th>Status</th>
                </tr>
              </thead>
              <tbody>
                {events.filter(e => e.blocked).slice(0, 20).map(ev => (
                  <tr key={ev.id}>
                    <td style={{ color: 'var(--text-muted)' }}>{new Date(ev.timestamp).toLocaleTimeString()}</td>
                    <td><span style={{ color: typeColor(ev.type) }}>{ev.type}</span></td>
                    <td>{ev.sourceIp}</td>
                    <td><span className="badge badge-failed">BLOCKED</span></td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        </div>
      </div>
    </div>
  );
}
