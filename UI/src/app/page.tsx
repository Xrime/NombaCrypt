'use client';

import { useState, useEffect } from 'react';
import { formatCurrency, formatLatency, formatTimestamp, truncateId } from '../lib/formatters';

const TOGGLES = [
  { id: 'congestion', name: 'Congestion Buffer', desc: 'Asynchronous transaction buffering to eliminate timeouts.', tag: 'SPEED' },
  { id: 'crypto', name: 'Crypto-Shield', desc: 'Automated, multi-threaded cryptographic pipelining.', tag: 'SECURITY' },
  { id: 'nuban', name: 'Dynamic NUBANs', desc: 'Persistent Virtual Account provisioning.', tag: 'SPEED' },
  { id: 'reconciler', name: 'Auto-Reconciler', desc: 'Handles misdirected, underpaid, or overpaid transfers.', tag: 'SPEED' },
  { id: 'subscription', name: 'Subscription Matrix', desc: 'Managed recurring-billing state machine.', tag: 'SECURITY' },
  { id: 'dunning', name: 'Dunning Engine', desc: 'Automated retry system for failed installment payments.', tag: 'SPEED' },
  { id: 'forwarder', name: 'Downstream Forwarder', desc: 'No-code webhook forwarding to Zoho/Zapier.', tag: 'SPEED' },
];

export default function Dashboard() {
  const [transactions, setTransactions] = useState<any[]>([]);
  const [totalProcessed, setTotalProcessed] = useState(1247832);
  const [toggles, setToggles] = useState<Record<string, boolean>>(() => {
    const initial: Record<string, boolean> = {};
    TOGGLES.forEach(t => initial[t.id] = true);
    return initial;
  });

  useEffect(() => {
    const interval = setInterval(() => {
      const isSuccess = Math.random() > 0.1;
      const newTx = {
        id: crypto.randomUUID(),
        amount: Math.floor(Math.random() * 100000) + 1000,
        latency: Math.floor(Math.random() * 100) + 10,
        status: isSuccess ? 'SUCCESS' : 'FAILED',
        timestamp: new Date().toISOString()
      };
      
      setTransactions(prev => [newTx, ...prev].slice(0, 50));
      if (isSuccess) setTotalProcessed(prev => prev + 1);
    }, 800);
    return () => clearInterval(interval);
  }, []);

  const handleToggle = (id: string) => {
    setToggles(prev => ({ ...prev, [id]: !prev[id] }));
  };

  return (
    <div className="animate-in">
      <div className="page-header">
        <h2>NombaCrypt Shell</h2>
        <p>Enterprise Payment Infrastructure Control Plane</p>
      </div>

      <div className="metrics-grid">
        <div className="metric-card">
          <div className="metric-icon">🚀</div>
          <div className="metric-value green">{totalProcessed.toLocaleString()}</div>
          <div className="metric-label">Total Processed</div>
        </div>
        <div className="metric-card">
          <div className="metric-icon">⚡</div>
          <div className="metric-value purple">42 μs</div>
          <div className="metric-label">Avg Latency</div>
        </div>
        <div className="metric-card">
          <div className="metric-icon">🧠</div>
          <div className="metric-value blue">34%</div>
          <div className="metric-label">Buffer Capacity</div>
        </div>
        <div className="metric-card">
          <div className="metric-icon">🧵</div>
          <div className="metric-value amber">12</div>
          <div className="metric-label">Active Threads</div>
        </div>
      </div>

      <div className="grid-2">
        <div className="card">
          <div className="section-header">
            <h3>Live Transaction Stream</h3>
            <span className="badge badge-success">LIVE</span>
          </div>
          <div className="terminal">
            {transactions.map((tx) => (
              <div key={tx.id} className="terminal-line">
                <span className="ts">[{new Date(tx.timestamp).toLocaleTimeString()}]</span>
                <span className="tx-id">{truncateId(tx.id)}</span>
                <span className="amount">{formatCurrency(tx.amount)}</span>
                <span className="latency">{tx.latency} μs</span>
                <span className={`status badge badge-${tx.status.toLowerCase()}`}>{tx.status}</span>
                <span className="msg">Payload verified and dispatched.</span>
              </div>
            ))}
          </div>
        </div>

        <div className="card">
           <div className="section-header">
            <h3>Module Configuration</h3>
          </div>
          <div className="toggles-grid" style={{ gridTemplateColumns: '1fr', gap: '12px' }}>
            {TOGGLES.map(mod => (
              <div key={mod.id} className="toggle-card" style={{ padding: '12px' }}>
                <div className="toggle-info">
                  <h3 style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                    {mod.name}
                    <span className={`badge badge-${mod.tag.toLowerCase()}`}>{mod.tag}</span>
                  </h3>
                  <p>{mod.desc}</p>
                </div>
                <label className="toggle-switch">
                  <input 
                    type="checkbox" 
                    checked={toggles[mod.id]} 
                    onChange={() => handleToggle(mod.id)}
                  />
                  <span className="toggle-slider"></span>
                </label>
              </div>
            ))}
          </div>
        </div>
      </div>
    </div>
  );
}
