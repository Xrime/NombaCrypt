'use client';

import { useState } from 'react';

type ChannelHealth = 'HEALTHY' | 'THROTTLED' | 'DOWN';

interface Channel {
  id: string;
  alias: string;
  health: ChannelHealth;
  headroomPercent: number;
  dispatchedCount: number;
  rateLimit: number;
  currentRps: number;
  lastDispatchedAt: string | null;
}

const MOCK_CHANNELS: Channel[] = [
  { id: '1', alias: 'nomba-primary', health: 'HEALTHY', headroomPercent: 72, dispatchedCount: 847321, rateLimit: 100, currentRps: 28, lastDispatchedAt: '2026-07-06T08:39:55Z' },
  { id: '2', alias: 'nomba-secondary', health: 'HEALTHY', headroomPercent: 88, dispatchedCount: 312456, rateLimit: 100, currentRps: 12, lastDispatchedAt: '2026-07-06T08:39:52Z' },
  { id: '3', alias: 'nomba-burst', health: 'THROTTLED', headroomPercent: 15, dispatchedCount: 98234, rateLimit: 50, currentRps: 47, lastDispatchedAt: '2026-07-06T08:39:58Z' },
  { id: '4', alias: 'nomba-reserve', health: 'DOWN', headroomPercent: 0, dispatchedCount: 5102, rateLimit: 100, currentRps: 0, lastDispatchedAt: '2026-07-05T22:10:00Z' },
];

const healthColor: Record<ChannelHealth, string> = {
  HEALTHY: 'var(--success)',
  THROTTLED: 'var(--warning)',
  DOWN: 'var(--alert)',
};

const healthBadge: Record<ChannelHealth, string> = {
  HEALTHY: 'badge-healthy',
  THROTTLED: 'badge-throttled',
  DOWN: 'badge-down',
};

export default function ChannelsPage() {
  const [showAdd, setShowAdd] = useState(false);

  const totalDispatched = MOCK_CHANNELS.reduce((s, c) => s + c.dispatchedCount, 0);
  const healthyCount = MOCK_CHANNELS.filter(c => c.health === 'HEALTHY').length;

  return (
    <div className="animate-in">
      <div className="page-header">
        <h2>API Channels</h2>
        <p>Multi-channel dispatcher routing and health monitoring.</p>
      </div>

      <div className="metrics-grid" style={{ gridTemplateColumns: 'repeat(4, 1fr)' }}>
        <div className="metric-card">
          <div className="metric-icon">🔗</div>
          <div className="metric-value blue">{MOCK_CHANNELS.length}</div>
          <div className="metric-label">Total Channels</div>
        </div>
        <div className="metric-card">
          <div className="metric-icon">✅</div>
          <div className="metric-value green">{healthyCount}</div>
          <div className="metric-label">Healthy</div>
        </div>
        <div className="metric-card">
          <div className="metric-icon">📦</div>
          <div className="metric-value purple">{totalDispatched.toLocaleString()}</div>
          <div className="metric-label">Total Dispatched</div>
        </div>
        <div className="metric-card">
          <div className="metric-icon">⚡</div>
          <div className="metric-value amber">{MOCK_CHANNELS.reduce((s, c) => s + c.currentRps, 0)} rps</div>
          <div className="metric-label">Combined Throughput</div>
        </div>
      </div>

      <div className="flex items-center justify-between mb-6">
        <h3>Channel Health</h3>
        <button className="btn btn-primary" onClick={() => setShowAdd(!showAdd)}>+ Add Channel</button>
      </div>

      {showAdd && (
        <div className="card mb-6">
          <h3 style={{ marginBottom: '16px' }}>Register API Channel</h3>
          <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: '12px' }}>
            <input className="input-field" placeholder="Channel Alias (e.g. nomba-overflow)" />
            <input className="input-field" placeholder="Client ID" />
            <input className="input-field" placeholder="Private Key" type="password" />
            <input className="input-field" placeholder="Rate Limit (rps)" type="number" />
          </div>
          <div className="flex gap-3 mt-4">
            <button className="btn btn-primary">Register Channel</button>
            <button className="btn btn-ghost" onClick={() => setShowAdd(false)}>Cancel</button>
          </div>
        </div>
      )}

      <div style={{ display: 'grid', gridTemplateColumns: 'repeat(2, 1fr)', gap: '16px' }}>
        {MOCK_CHANNELS.map(ch => (
          <div key={ch.id} className="card">
            <div className="flex items-center justify-between" style={{ marginBottom: '16px' }}>
              <div>
                <div style={{ fontWeight: 600, fontSize: '1.05rem', marginBottom: '4px' }}>{ch.alias}</div>
                <span className={`badge ${healthBadge[ch.health]}`}>{ch.health}</span>
              </div>
              <div style={{ width: '12px', height: '12px', borderRadius: '50%', background: healthColor[ch.health], boxShadow: `0 0 8px ${healthColor[ch.health]}` }}></div>
            </div>

            {/* Rate Limit Headroom Gauge */}
            <div style={{ marginBottom: '16px' }}>
              <div className="flex items-center justify-between" style={{ marginBottom: '6px' }}>
                <span style={{ fontSize: '0.75rem', color: 'var(--text-muted)' }}>Rate Limit Headroom</span>
                <span style={{ fontSize: '0.8rem', fontFamily: 'var(--font-mono)', color: ch.headroomPercent < 30 ? 'var(--alert)' : ch.headroomPercent < 60 ? 'var(--warning)' : 'var(--success)' }}>
                  {ch.headroomPercent}%
                </span>
              </div>
              <div className="progress-bar" style={{ height: '8px', marginBottom: 0 }}>
                <div className={`progress-fill ${ch.headroomPercent < 30 ? 'red' : ch.headroomPercent < 60 ? 'blue' : 'green'}`} style={{ width: `${ch.headroomPercent}%` }}></div>
              </div>
            </div>

            <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: '12px', fontSize: '0.8rem' }}>
              <div>
                <div style={{ color: 'var(--text-muted)' }}>Dispatched</div>
                <div style={{ fontFamily: 'var(--font-mono)', fontWeight: 600 }}>{ch.dispatchedCount.toLocaleString()}</div>
              </div>
              <div>
                <div style={{ color: 'var(--text-muted)' }}>Current Rate</div>
                <div style={{ fontFamily: 'var(--font-mono)', fontWeight: 600 }}>{ch.currentRps} / {ch.rateLimit} rps</div>
              </div>
            </div>
          </div>
        ))}
      </div>
    </div>
  );
}
