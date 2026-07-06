'use client';

import { useState } from 'react';

interface WebhookTarget {
  id: string;
  name: string;
  url: string;
  active: boolean;
  lastStatus: number | null;
  lastDeliveredAt: string | null;
  deliveryCount: number;
}

const MOCK_TARGETS: WebhookTarget[] = [
  { id: '1', name: 'QuickBooks Online', url: 'https://webhooks.quickbooks.com/v1/nombacrypt/inbound', active: true, lastStatus: 200, lastDeliveredAt: '2026-07-06T08:30:00Z', deliveryCount: 4218 },
  { id: '2', name: 'Zapier — Email Receipts', url: 'https://hooks.zapier.com/hooks/catch/123456/nombacrypt/', active: true, lastStatus: 200, lastDeliveredAt: '2026-07-06T08:29:45Z', deliveryCount: 3891 },
  { id: '3', name: 'Slack Alerts', url: 'https://hooks.slack.com/services/T0XXXX/B0XXXX/nombacrypt', active: true, lastStatus: 200, lastDeliveredAt: '2026-07-06T08:28:10Z', deliveryCount: 1204 },
  { id: '4', name: 'University ERP', url: 'https://erp.uniibadan.edu.ng/api/webhooks/payments', active: false, lastStatus: 502, lastDeliveredAt: '2026-07-05T22:15:00Z', deliveryCount: 892 },
  { id: '5', name: 'Zoho Books', url: 'https://books.zoho.com/api/v3/webhooks/nombacrypt', active: true, lastStatus: 200, lastDeliveredAt: '2026-07-06T08:30:02Z', deliveryCount: 2105 },
];

export default function ForwarderPage() {
  const [targets, setTargets] = useState(MOCK_TARGETS);
  const [newUrl, setNewUrl] = useState('');
  const [newName, setNewName] = useState('');

  const toggleTarget = (id: string) => {
    setTargets(prev => prev.map(t => t.id === id ? { ...t, active: !t.active } : t));
  };

  const addTarget = () => {
    if (!newUrl.trim()) return;
    setTargets(prev => [...prev, {
      id: crypto.randomUUID(),
      name: newName || 'Custom Endpoint',
      url: newUrl,
      active: true,
      lastStatus: null,
      lastDeliveredAt: null,
      deliveryCount: 0,
    }]);
    setNewUrl('');
    setNewName('');
  };

  const totalDelivered = targets.reduce((sum, t) => sum + t.deliveryCount, 0);
  const activeCount = targets.filter(t => t.active).length;

  return (
    <div className="animate-in">
      <div className="page-header">
        <h2>Downstream Webhook Forwarder</h2>
        <p>Securely forward verified payment events to external systems, no code required.</p>
      </div>

      <div className="metrics-grid" style={{ gridTemplateColumns: 'repeat(3, 1fr)' }}>
        <div className="metric-card">
          <div className="metric-icon">📡</div>
          <div className="metric-value blue">{targets.length}</div>
          <div className="metric-label">Registered Targets</div>
        </div>
        <div className="metric-card">
          <div className="metric-icon">✅</div>
          <div className="metric-value green">{activeCount}</div>
          <div className="metric-label">Active Endpoints</div>
        </div>
        <div className="metric-card">
          <div className="metric-icon">📦</div>
          <div className="metric-value purple">{totalDelivered.toLocaleString()}</div>
          <div className="metric-label">Total Deliveries</div>
        </div>
      </div>

      {/* Add new target */}
      <div className="card mb-8">
        <h3 style={{ marginBottom: '16px' }}>Register New Webhook Target</h3>
        <div className="flex gap-3">
          <input
            className="input-field"
            placeholder="Endpoint Name (e.g. Slack Alerts)"
            value={newName}
            onChange={e => setNewName(e.target.value)}
            style={{ maxWidth: '250px' }}
          />
          <input
            className="input-field"
            placeholder="https://your-endpoint.com/webhook"
            value={newUrl}
            onChange={e => setNewUrl(e.target.value)}
            style={{ flex: 1 }}
          />
          <button className="btn btn-primary" onClick={addTarget}>+ Add Target</button>
        </div>
      </div>

      {/* Target cards */}
      <div className="section-header">
        <h3>Registered Endpoints</h3>
      </div>
      <div style={{ display: 'grid', gap: '12px' }}>
        {targets.map(target => (
          <div key={target.id} className="card" style={{ padding: '16px 20px' }}>
            <div className="flex items-center justify-between">
              <div style={{ flex: 1 }}>
                <div className="flex items-center gap-3" style={{ marginBottom: '6px' }}>
                  <span style={{ fontWeight: 600 }}>{target.name}</span>
                  {target.lastStatus && (
                    <span className={`badge ${target.lastStatus === 200 ? 'badge-success' : 'badge-failed'}`}>
                      HTTP {target.lastStatus}
                    </span>
                  )}
                </div>
                <div style={{ fontFamily: 'var(--font-mono)', fontSize: '0.78rem', color: 'var(--info)', wordBreak: 'break-all' }}>
                  {target.url}
                </div>
                <div style={{ fontSize: '0.75rem', color: 'var(--text-muted)', marginTop: '4px' }}>
                  {target.deliveryCount.toLocaleString()} deliveries
                  {target.lastDeliveredAt && ` · Last: ${new Date(target.lastDeliveredAt).toLocaleString()}`}
                </div>
              </div>
              <label className="toggle-switch">
                <input type="checkbox" checked={target.active} onChange={() => toggleTarget(target.id)} />
                <span className="toggle-slider"></span>
              </label>
            </div>
          </div>
        ))}
      </div>
    </div>
  );
}
