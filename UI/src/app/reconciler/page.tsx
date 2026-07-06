'use client';

import { useState } from 'react';
import { formatCurrency } from '../../lib/formatters';

type DiscType = 'UNDERPAID' | 'OVERPAID' | 'MISSING';

interface Discrepancy {
  id: string;
  txId: string;
  studentName: string;
  type: DiscType;
  expectedAmount: number;
  actualAmount: number;
  variance: number;
  detectedAt: string;
  resolved: boolean;
}

const MOCK_DISCS: Discrepancy[] = [
  { id: '1', txId: 'a3f8c2d1-b4e5', studentName: 'Adebayo Olamide', type: 'UNDERPAID', expectedAmount: 160000, actualAmount: 150000, variance: -10000, detectedAt: '2026-07-05T14:30:00Z', resolved: false },
  { id: '2', txId: 'b7c3e1f9-d2a4', studentName: 'Fatima Abdullahi', type: 'OVERPAID', expectedAmount: 60000, actualAmount: 65000, variance: 5000, detectedAt: '2026-07-05T13:15:00Z', resolved: true },
  { id: '3', txId: 'c1d9f4a2-e8b7', studentName: 'Ngozi Eze', type: 'UNDERPAID', expectedAmount: 180000, actualAmount: 170000, variance: -10000, detectedAt: '2026-07-05T11:00:00Z', resolved: false },
  { id: '4', txId: 'd4e7a1b3-f9c2', studentName: 'Emeka Nwankwo', type: 'MISSING', expectedAmount: 75000, actualAmount: 0, variance: -75000, detectedAt: '2026-07-04T16:45:00Z', resolved: false },
  { id: '5', txId: 'e2f8b3c4-a1d5', studentName: 'Tunde Bakare', type: 'OVERPAID', expectedAmount: 210000, actualAmount: 230000, variance: 20000, detectedAt: '2026-07-04T10:20:00Z', resolved: true },
  { id: '6', txId: 'f5a9c1d2-b3e8', studentName: 'Amara Obi', type: 'UNDERPAID', expectedAmount: 45000, actualAmount: 40000, variance: -5000, detectedAt: '2026-07-04T09:00:00Z', resolved: true },
  { id: '7', txId: 'a8b2d4e1-c7f3', studentName: 'Yusuf Bello', type: 'OVERPAID', expectedAmount: 320000, actualAmount: 325000, variance: 5000, detectedAt: '2026-07-03T15:30:00Z', resolved: true },
  { id: '8', txId: 'b1c3e5f7-d9a2', studentName: 'Halima Suleiman', type: 'MISSING', expectedAmount: 50000, actualAmount: 0, variance: -50000, detectedAt: '2026-07-03T11:10:00Z', resolved: false },
];

const discBadge: Record<DiscType, string> = { UNDERPAID: 'badge-underpaid', OVERPAID: 'badge-overpaid', MISSING: 'badge-missing' };

export default function ReconcilerPage() {
  const [filter, setFilter] = useState<DiscType | 'ALL'>('ALL');

  const filtered = MOCK_DISCS.filter(d => filter === 'ALL' || d.type === filter);
  const unresolvedCount = MOCK_DISCS.filter(d => !d.resolved).length;
  const resolvedCount = MOCK_DISCS.filter(d => d.resolved).length;
  const totalVariance = MOCK_DISCS.filter(d => !d.resolved).reduce((sum, d) => sum + Math.abs(d.variance), 0);

  // Dunning stats
  const dunning = { totalFailed: 23, recovered: 18, recoveryRate: 78.3, totalRecovered: 1420000 };

  return (
    <div className="animate-in">
      <div className="page-header">
        <h2>Automated Transfer Reconciliation</h2>
        <p>Real-time ledger matching and discrepancy resolution for inbound bank transfers.</p>
      </div>

      <div className="metrics-grid" style={{ gridTemplateColumns: 'repeat(4, 1fr)' }}>
        <div className="metric-card">
          <div className="metric-icon">🔍</div>
          <div className="metric-value amber">{unresolvedCount}</div>
          <div className="metric-label">Open Discrepancies</div>
        </div>
        <div className="metric-card">
          <div className="metric-icon">✅</div>
          <div className="metric-value green">{resolvedCount}</div>
          <div className="metric-label">Auto-Resolved</div>
        </div>
        <div className="metric-card">
          <div className="metric-icon">₦</div>
          <div className="metric-value red">{formatCurrency(totalVariance)}</div>
          <div className="metric-label">Outstanding Variance</div>
        </div>
        <div className="metric-card">
          <div className="metric-icon">🔄</div>
          <div className="metric-value green">{dunning.recoveryRate}%</div>
          <div className="metric-label">Dunning Recovery Rate</div>
        </div>
      </div>

      {/* Dunning Engine Card */}
      <div className="card mb-8">
        <div className="section-header">
          <h3>Dunning Engine — Failed Payment Recovery</h3>
          <span className="badge badge-speed">SPEED</span>
        </div>
        <div className="grid-2">
          <div>
            <div style={{ fontSize: '0.8rem', color: 'var(--text-muted)', marginBottom: '4px' }}>Recovery Progress</div>
            <div className="progress-bar" style={{ height: '10px' }}>
              <div className="progress-fill green" style={{ width: `${dunning.recoveryRate}%` }}></div>
            </div>
            <div style={{ fontSize: '0.8rem', color: 'var(--text-muted)', marginTop: '6px' }}>
              {dunning.recovered} of {dunning.totalFailed} failed charges recovered
            </div>
          </div>
          <div style={{ textAlign: 'right' }}>
            <div style={{ fontSize: '0.8rem', color: 'var(--text-muted)', marginBottom: '4px' }}>Total Recovered Revenue</div>
            <div style={{ fontFamily: 'var(--font-mono)', fontSize: '1.8rem', fontWeight: 800, color: 'var(--success)' }}>
              {formatCurrency(dunning.totalRecovered)}
            </div>
          </div>
        </div>
      </div>

      <div className="flex items-center gap-4 mb-6">
        <div className="filter-bar" style={{ marginBottom: 0 }}>
          {(['ALL', 'UNDERPAID', 'OVERPAID', 'MISSING'] as const).map(f => (
            <button key={f} className={`filter-btn ${filter === f ? 'active' : ''}`} onClick={() => setFilter(f)}>{f}</button>
          ))}
        </div>
      </div>

      <div className="card" style={{ padding: 0, overflow: 'hidden' }}>
        <table className="data-table">
          <thead>
            <tr>
              <th>Transaction</th>
              <th>Student</th>
              <th>Type</th>
              <th>Expected</th>
              <th>Actual</th>
              <th>Variance</th>
              <th>Detected</th>
              <th>Status</th>
            </tr>
          </thead>
          <tbody>
            {filtered.map(d => (
              <tr key={d.id}>
                <td style={{ color: 'var(--info)' }}>{d.txId}</td>
                <td style={{ fontFamily: 'var(--font-sans)', fontWeight: 500 }}>{d.studentName}</td>
                <td><span className={`badge ${discBadge[d.type]}`}>{d.type}</span></td>
                <td style={{ color: 'var(--text-muted)' }}>{formatCurrency(d.expectedAmount)}</td>
                <td style={{ color: d.actualAmount === 0 ? 'var(--alert)' : 'var(--text)' }}>{d.actualAmount === 0 ? '—' : formatCurrency(d.actualAmount)}</td>
                <td style={{ color: d.variance < 0 ? 'var(--alert)' : 'var(--success)', fontWeight: 600 }}>
                  {d.variance < 0 ? '-' : '+'}{formatCurrency(Math.abs(d.variance))}
                </td>
                <td style={{ color: 'var(--text-muted)', fontSize: '0.75rem' }}>{new Date(d.detectedAt).toLocaleDateString()}</td>
                <td>
                  <span className={`badge ${d.resolved ? 'badge-success' : 'badge-pending'}`}>
                    {d.resolved ? 'RESOLVED' : 'OPEN'}
                  </span>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </div>
  );
}
