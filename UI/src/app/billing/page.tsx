'use client';

import { useState } from 'react';
import { formatCurrency } from '../../lib/formatters';

type SubState = 'PENDING' | 'PARTIALLY_PAID' | 'SETTLED';

interface Installment {
  id: string;
  label: string;
  amount: number;
  dueDate: string;
  paidAt: string | null;
  status: 'UPCOMING' | 'PAID' | 'OVERDUE';
}

interface Subscription {
  id: string;
  studentName: string;
  matric: string;
  planName: string;
  totalAmount: number;
  paidAmount: number;
  state: SubState;
  installments: Installment[];
}

const MOCK_SUBS: Subscription[] = [
  {
    id: '1', studentName: 'Adebayo Olamide', matric: 'UI/2024/CSC/0112', planName: '50/50 Tuition Split', totalAmount: 320000, paidAmount: 160000, state: 'PARTIALLY_PAID',
    installments: [
      { id: '1a', label: 'First Installment', amount: 160000, dueDate: '2026-02-15', paidAt: '2026-02-14T09:30:00Z', status: 'PAID' },
      { id: '1b', label: 'Second Installment', amount: 160000, dueDate: '2026-06-15', paidAt: null, status: 'OVERDUE' },
    ]
  },
  {
    id: '2', studentName: 'Chidinma Okafor', matric: 'UI/2024/MED/0045', planName: '60/40 Tuition Split', totalAmount: 580000, paidAmount: 580000, state: 'SETTLED',
    installments: [
      { id: '2a', label: 'First Installment (60%)', amount: 348000, dueDate: '2026-02-01', paidAt: '2026-01-28T14:00:00Z', status: 'PAID' },
      { id: '2b', label: 'Second Installment (40%)', amount: 232000, dueDate: '2026-05-01', paidAt: '2026-04-30T11:20:00Z', status: 'PAID' },
    ]
  },
  {
    id: '3', studentName: 'Ibrahim Musa', matric: 'UI/2023/LAW/0089', planName: '50/50 Tuition Split', totalAmount: 420000, paidAmount: 0, state: 'PENDING',
    installments: [
      { id: '3a', label: 'First Installment', amount: 210000, dueDate: '2026-07-15', paidAt: null, status: 'UPCOMING' },
      { id: '3b', label: 'Second Installment', amount: 210000, dueDate: '2026-11-15', paidAt: null, status: 'UPCOMING' },
    ]
  },
  {
    id: '4', studentName: 'Fatima Abdullahi', matric: 'UI/2024/ECO/0201', planName: '3-Way Split', totalAmount: 180000, paidAmount: 120000, state: 'PARTIALLY_PAID',
    installments: [
      { id: '4a', label: 'Installment 1/3', amount: 60000, dueDate: '2026-01-15', paidAt: '2026-01-14T08:00:00Z', status: 'PAID' },
      { id: '4b', label: 'Installment 2/3', amount: 60000, dueDate: '2026-04-15', paidAt: '2026-04-15T16:45:00Z', status: 'PAID' },
      { id: '4c', label: 'Installment 3/3', amount: 60000, dueDate: '2026-07-15', paidAt: null, status: 'UPCOMING' },
    ]
  },
  {
    id: '5', studentName: 'Oluwaseun Akinwale', matric: 'UI/2023/ENG/0067', planName: '50/50 Departmental Dues', totalAmount: 75000, paidAmount: 75000, state: 'SETTLED',
    installments: [
      { id: '5a', label: 'First Half', amount: 37500, dueDate: '2026-02-01', paidAt: '2026-01-30T10:00:00Z', status: 'PAID' },
      { id: '5b', label: 'Second Half', amount: 37500, dueDate: '2026-05-01', paidAt: '2026-04-29T12:15:00Z', status: 'PAID' },
    ]
  },
  {
    id: '6', studentName: 'Ngozi Eze', matric: 'UI/2025/PHM/0033', planName: '60/40 Tuition Split', totalAmount: 450000, paidAmount: 270000, state: 'PARTIALLY_PAID',
    installments: [
      { id: '6a', label: 'First Installment (60%)', amount: 270000, dueDate: '2026-03-01', paidAt: '2026-02-28T09:00:00Z', status: 'PAID' },
      { id: '6b', label: 'Second Installment (40%)', amount: 180000, dueDate: '2026-07-01', paidAt: null, status: 'OVERDUE' },
    ]
  },
];

const stateColors: Record<SubState, string> = {
  PENDING: 'var(--text-muted)',
  PARTIALLY_PAID: 'var(--warning)',
  SETTLED: 'var(--success)',
};

const stateBadge: Record<SubState, string> = {
  PENDING: 'badge-pending',
  PARTIALLY_PAID: 'badge-throttled',
  SETTLED: 'badge-success',
};

const installmentBadge: Record<string, string> = {
  PAID: 'badge-success',
  OVERDUE: 'badge-failed',
  UPCOMING: 'badge-pending',
};

export default function BillingPage() {
  const [expanded, setExpanded] = useState<string | null>(null);

  const settledCount = MOCK_SUBS.filter(s => s.state === 'SETTLED').length;
  const partialCount = MOCK_SUBS.filter(s => s.state === 'PARTIALLY_PAID').length;
  const totalCollected = MOCK_SUBS.reduce((sum, s) => sum + s.paidAmount, 0);

  return (
    <div className="animate-in">
      <div className="page-header">
        <h2>Managed Recurring-Billing State Machine</h2>
        <p>Custom installment plans with tokenized card billing and automated state transitions.</p>
      </div>

      <div className="metrics-grid" style={{ gridTemplateColumns: 'repeat(4, 1fr)' }}>
        <div className="metric-card">
          <div className="metric-icon">💳</div>
          <div className="metric-value blue">{MOCK_SUBS.length}</div>
          <div className="metric-label">Active Plans</div>
        </div>
        <div className="metric-card">
          <div className="metric-icon">✅</div>
          <div className="metric-value green">{settledCount}</div>
          <div className="metric-label">Fully Settled</div>
        </div>
        <div className="metric-card">
          <div className="metric-icon">⏳</div>
          <div className="metric-value amber">{partialCount}</div>
          <div className="metric-label">Partially Paid</div>
        </div>
        <div className="metric-card">
          <div className="metric-icon">₦</div>
          <div className="metric-value green">{formatCurrency(totalCollected)}</div>
          <div className="metric-label">Total Collected</div>
        </div>
      </div>

      {/* State Machine Visualization */}
      <div className="card mb-8">
        <h3 style={{ marginBottom: '16px' }}>Billing State Machine</h3>
        <div className="flex items-center justify-between" style={{ maxWidth: '600px', margin: '0 auto', padding: '20px 0' }}>
          <div style={{ textAlign: 'center' }}>
            <div style={{ width: '80px', height: '80px', borderRadius: '50%', border: '2px solid var(--text-muted)', display: 'flex', alignItems: 'center', justifyContent: 'center', fontSize: '0.75rem', fontWeight: 600, color: 'var(--text-muted)', margin: '0 auto 8px' }}>
              PENDING
            </div>
          </div>
          <div style={{ flex: 1, height: '2px', background: 'var(--border)', position: 'relative' }}>
            <span style={{ position: 'absolute', top: '-20px', left: '50%', transform: 'translateX(-50%)', fontSize: '0.7rem', color: 'var(--text-muted)' }}>charge_success</span>
            <span style={{ position: 'absolute', right: '-4px', top: '-4px', borderTop: '6px solid transparent', borderBottom: '6px solid transparent', borderLeft: '8px solid var(--border)' }}></span>
          </div>
          <div style={{ textAlign: 'center' }}>
            <div style={{ width: '80px', height: '80px', borderRadius: '50%', border: '2px solid var(--warning)', display: 'flex', alignItems: 'center', justifyContent: 'center', fontSize: '0.65rem', fontWeight: 600, color: 'var(--warning)', margin: '0 auto 8px' }}>
              PARTIALLY<br/>PAID
            </div>
          </div>
          <div style={{ flex: 1, height: '2px', background: 'var(--border)', position: 'relative' }}>
            <span style={{ position: 'absolute', top: '-20px', left: '50%', transform: 'translateX(-50%)', fontSize: '0.7rem', color: 'var(--text-muted)' }}>final_charge</span>
            <span style={{ position: 'absolute', right: '-4px', top: '-4px', borderTop: '6px solid transparent', borderBottom: '6px solid transparent', borderLeft: '8px solid var(--border)' }}></span>
          </div>
          <div style={{ textAlign: 'center' }}>
            <div style={{ width: '80px', height: '80px', borderRadius: '50%', border: '2px solid var(--success)', display: 'flex', alignItems: 'center', justifyContent: 'center', fontSize: '0.75rem', fontWeight: 600, color: 'var(--success)', margin: '0 auto 8px', boxShadow: '0 0 12px rgba(0,255,136,0.2)' }}>
              SETTLED
            </div>
          </div>
        </div>
      </div>

      {/* Subscription Cards */}
      <div className="section-header">
        <h3>Active Subscription Plans</h3>
      </div>
      <div style={{ display: 'grid', gap: '16px' }}>
        {MOCK_SUBS.map(sub => (
          <div key={sub.id} className="card" style={{ cursor: 'pointer' }} onClick={() => setExpanded(expanded === sub.id ? null : sub.id)}>
            <div className="flex items-center justify-between" style={{ marginBottom: expanded === sub.id ? '16px' : 0 }}>
              <div>
                <div style={{ fontWeight: 600, marginBottom: '4px' }}>{sub.studentName}</div>
                <div style={{ fontSize: '0.8rem', color: 'var(--text-muted)' }}>{sub.matric} · {sub.planName}</div>
              </div>
              <div className="flex items-center gap-4">
                <div style={{ textAlign: 'right' }}>
                  <div style={{ fontFamily: 'var(--font-mono)', fontSize: '1.1rem', fontWeight: 700, color: stateColors[sub.state] }}>
                    {formatCurrency(sub.paidAmount)} <span style={{ color: 'var(--text-muted)', fontSize: '0.8rem' }}>/ {formatCurrency(sub.totalAmount)}</span>
                  </div>
                  <div className="progress-bar" style={{ width: '200px', marginTop: '6px', marginBottom: 0 }}>
                    <div className={`progress-fill ${sub.state === 'SETTLED' ? 'green' : sub.state === 'PARTIALLY_PAID' ? 'blue' : ''}`} style={{ width: `${(sub.paidAmount / sub.totalAmount) * 100}%` }}></div>
                  </div>
                </div>
                <span className={`badge ${stateBadge[sub.state]}`}>{sub.state.replace('_', ' ')}</span>
              </div>
            </div>

            {expanded === sub.id && (
              <div style={{ borderTop: '1px solid var(--border)', paddingTop: '16px' }}>
                <table className="data-table">
                  <thead>
                    <tr>
                      <th>Installment</th>
                      <th>Amount</th>
                      <th>Due Date</th>
                      <th>Paid</th>
                      <th>Status</th>
                    </tr>
                  </thead>
                  <tbody>
                    {sub.installments.map(inst => (
                      <tr key={inst.id}>
                        <td style={{ fontFamily: 'var(--font-sans)' }}>{inst.label}</td>
                        <td style={{ color: 'var(--success)' }}>{formatCurrency(inst.amount)}</td>
                        <td style={{ color: 'var(--text-muted)' }}>{inst.dueDate}</td>
                        <td style={{ color: 'var(--text-muted)' }}>{inst.paidAt ? new Date(inst.paidAt).toLocaleDateString() : '—'}</td>
                        <td><span className={`badge ${installmentBadge[inst.status]}`}>{inst.status}</span></td>
                      </tr>
                    ))}
                  </tbody>
                </table>
              </div>
            )}
          </div>
        ))}
      </div>
    </div>
  );
}
