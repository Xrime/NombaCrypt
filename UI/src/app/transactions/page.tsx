'use client';

import { useState } from 'react';
import { formatCurrency, formatLatency, formatTimestamp, truncateId } from '../../lib/formatters';

type TxStatus = 'SUCCESS' | 'FAILED' | 'PENDING' | 'TIMEOUT';

interface Transaction {
  txId: string;
  amount: number;
  currency: string;
  latencyUs: number;
  status: TxStatus;
  timestamp: string;
  channelAlias: string;
}

const CHANNELS = ['nomba-primary', 'nomba-secondary', 'nomba-burst'];
const STATUSES: TxStatus[] = ['SUCCESS', 'FAILED', 'PENDING', 'TIMEOUT'];

function generateMockTxs(count: number): Transaction[] {
  const txs: Transaction[] = [];
  const now = Date.now();
  for (let i = 0; i < count; i++) {
    const rand = Math.random();
    let status: TxStatus;
    if (rand < 0.72) status = 'SUCCESS';
    else if (rand < 0.84) status = 'FAILED';
    else if (rand < 0.93) status = 'PENDING';
    else status = 'TIMEOUT';

    txs.push({
      txId: crypto.randomUUID(),
      amount: Math.floor(Math.random() * 250000) + 2500,
      currency: 'NGN',
      latencyUs: Math.floor(Math.random() * 200) + 8,
      status,
      timestamp: new Date(now - i * 45000 - Math.random() * 30000).toISOString(),
      channelAlias: CHANNELS[Math.floor(Math.random() * CHANNELS.length)],
    });
  }
  return txs;
}

const MOCK_TXS = generateMockTxs(40);

export default function TransactionsPage() {
  const [filter, setFilter] = useState<TxStatus | 'ALL'>('ALL');
  const [search, setSearch] = useState('');

  const filtered = MOCK_TXS.filter(tx => {
    if (filter !== 'ALL' && tx.status !== filter) return false;
    if (search && !tx.txId.toLowerCase().includes(search.toLowerCase()) && !tx.channelAlias.includes(search.toLowerCase())) return false;
    return true;
  });

  const counts = {
    ALL: MOCK_TXS.length,
    SUCCESS: MOCK_TXS.filter(t => t.status === 'SUCCESS').length,
    FAILED: MOCK_TXS.filter(t => t.status === 'FAILED').length,
    PENDING: MOCK_TXS.filter(t => t.status === 'PENDING').length,
    TIMEOUT: MOCK_TXS.filter(t => t.status === 'TIMEOUT').length,
  };

  return (
    <div className="animate-in">
      <div className="page-header">
        <h2>Transaction Ledger</h2>
        <p>Immutable, append-only record of all processed payloads.</p>
      </div>

      <div className="metrics-grid" style={{ gridTemplateColumns: 'repeat(5, 1fr)', marginBottom: '24px' }}>
        <div className="metric-card">
          <div className="metric-value green">{counts.ALL}</div>
          <div className="metric-label">Total</div>
        </div>
        <div className="metric-card">
          <div className="metric-value green">{counts.SUCCESS}</div>
          <div className="metric-label">Success</div>
        </div>
        <div className="metric-card">
          <div className="metric-value red">{counts.FAILED}</div>
          <div className="metric-label">Failed</div>
        </div>
        <div className="metric-card">
          <div className="metric-value amber">{counts.PENDING}</div>
          <div className="metric-label">Pending</div>
        </div>
        <div className="metric-card">
          <div className="metric-value" style={{ color: 'var(--text-muted)' }}>{counts.TIMEOUT}</div>
          <div className="metric-label">Timeout</div>
        </div>
      </div>

      <div className="flex items-center gap-4 mb-6">
        <input
          type="text"
          className="search-input"
          placeholder="Search by txId or channel..."
          value={search}
          onChange={e => setSearch(e.target.value)}
        />
        <div className="filter-bar" style={{ marginBottom: 0 }}>
          {(['ALL', ...STATUSES] as const).map(s => (
            <button key={s} className={`filter-btn ${filter === s ? 'active' : ''}`} onClick={() => setFilter(s)}>
              {s} ({counts[s]})
            </button>
          ))}
        </div>
      </div>

      <div className="card" style={{ padding: 0, overflow: 'hidden' }}>
        <table className="data-table">
          <thead>
            <tr>
              <th>Transaction ID</th>
              <th>Amount</th>
              <th>Latency</th>
              <th>Channel</th>
              <th>Timestamp</th>
              <th>Status</th>
            </tr>
          </thead>
          <tbody>
            {filtered.map(tx => (
              <tr key={tx.txId}>
                <td style={{ color: 'var(--info)' }}>{truncateId(tx.txId, 12)}</td>
                <td style={{ color: 'var(--success)' }}>{formatCurrency(tx.amount)}</td>
                <td style={{ color: 'var(--accent)' }}>{formatLatency(tx.latencyUs)}</td>
                <td style={{ color: 'var(--text-muted)' }}>{tx.channelAlias}</td>
                <td style={{ color: 'var(--text-muted)', fontSize: '0.75rem' }}>{formatTimestamp(tx.timestamp)}</td>
                <td>
                  <span className={`badge badge-${tx.status.toLowerCase()}`}>{tx.status}</span>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </div>
  );
}
