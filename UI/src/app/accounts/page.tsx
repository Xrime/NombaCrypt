'use client';

import { useState } from 'react';

interface VirtualAccount {
  id: string;
  studentName: string;
  matric: string;
  nuban: string;
  bankName: string;
  status: 'ACTIVE' | 'INACTIVE';
  department: string;
  createdAt: string;
}

const BANKS = ['GTBank', 'Access Bank', 'First Bank', 'UBA', 'Zenith Bank', 'Nomba MFB'];
const DEPTS = ['Computer Science', 'Mechanical Engineering', 'Medicine', 'Law', 'Economics', 'Pharmacy', 'Architecture', 'Accounting'];

const MOCK_ACCOUNTS: VirtualAccount[] = [
  { id: '1', studentName: 'Adebayo Olamide', matric: 'UI/2024/CSC/0112', nuban: '9920145678', bankName: 'Nomba MFB', status: 'ACTIVE', department: 'Computer Science', createdAt: '2024-09-15T10:30:00Z' },
  { id: '2', studentName: 'Chidinma Okafor', matric: 'UI/2024/MED/0045', nuban: '9920234567', bankName: 'Nomba MFB', status: 'ACTIVE', department: 'Medicine', createdAt: '2024-09-15T10:32:00Z' },
  { id: '3', studentName: 'Ibrahim Musa', matric: 'UI/2023/LAW/0089', nuban: '9920345678', bankName: 'Nomba MFB', status: 'ACTIVE', department: 'Law', createdAt: '2024-09-14T09:15:00Z' },
  { id: '4', studentName: 'Fatima Abdullahi', matric: 'UI/2024/ECO/0201', nuban: '9920456789', bankName: 'Nomba MFB', status: 'ACTIVE', department: 'Economics', createdAt: '2024-09-15T11:00:00Z' },
  { id: '5', studentName: 'Oluwaseun Akinwale', matric: 'UI/2023/ENG/0067', nuban: '9920567890', bankName: 'Nomba MFB', status: 'ACTIVE', department: 'Mechanical Engineering', createdAt: '2024-09-14T14:20:00Z' },
  { id: '6', studentName: 'Ngozi Eze', matric: 'UI/2025/PHM/0033', nuban: '9920678901', bankName: 'Nomba MFB', status: 'ACTIVE', department: 'Pharmacy', createdAt: '2025-01-10T08:45:00Z' },
  { id: '7', studentName: 'Yusuf Bello', matric: 'UI/2024/ARC/0018', nuban: '9920789012', bankName: 'Nomba MFB', status: 'ACTIVE', department: 'Architecture', createdAt: '2024-09-16T10:00:00Z' },
  { id: '8', studentName: 'Blessing Onyekachi', matric: 'UI/2023/ACC/0155', nuban: '9920890123', bankName: 'Nomba MFB', status: 'INACTIVE', department: 'Accounting', createdAt: '2023-09-10T09:30:00Z' },
  { id: '9', studentName: 'Emeka Nwankwo', matric: 'UI/2024/CSC/0098', nuban: '9920901234', bankName: 'Nomba MFB', status: 'ACTIVE', department: 'Computer Science', createdAt: '2024-09-15T13:10:00Z' },
  { id: '10', studentName: 'Aisha Mohammed', matric: 'UI/2025/MED/0012', nuban: '9921012345', bankName: 'Nomba MFB', status: 'ACTIVE', department: 'Medicine', createdAt: '2025-01-12T07:55:00Z' },
  { id: '11', studentName: 'Tunde Bakare', matric: 'UI/2023/LAW/0134', nuban: '9921123456', bankName: 'Nomba MFB', status: 'ACTIVE', department: 'Law', createdAt: '2023-09-11T11:40:00Z' },
  { id: '12', studentName: 'Amara Obi', matric: 'UI/2024/ECO/0077', nuban: '9921234567', bankName: 'Nomba MFB', status: 'ACTIVE', department: 'Economics', createdAt: '2024-09-15T15:25:00Z' },
  { id: '13', studentName: 'Kehinde Adeyemi', matric: 'UI/2024/ENG/0143', nuban: '9921345678', bankName: 'Nomba MFB', status: 'INACTIVE', department: 'Mechanical Engineering', createdAt: '2024-09-14T16:00:00Z' },
  { id: '14', studentName: 'Halima Suleiman', matric: 'UI/2025/CSC/0005', nuban: '9921456789', bankName: 'Nomba MFB', status: 'ACTIVE', department: 'Computer Science', createdAt: '2025-01-08T10:10:00Z' },
  { id: '15', studentName: 'Chukwuemeka Agu', matric: 'UI/2023/PHM/0091', nuban: '9921567890', bankName: 'Nomba MFB', status: 'ACTIVE', department: 'Pharmacy', createdAt: '2023-09-12T12:30:00Z' },
];

export default function AccountsPage() {
  const [search, setSearch] = useState('');
  const [showModal, setShowModal] = useState(false);

  const filtered = MOCK_ACCOUNTS.filter(a =>
    a.studentName.toLowerCase().includes(search.toLowerCase()) ||
    a.matric.toLowerCase().includes(search.toLowerCase()) ||
    a.nuban.includes(search)
  );

  const activeCount = MOCK_ACCOUNTS.filter(a => a.status === 'ACTIVE').length;

  return (
    <div className="animate-in">
      <div className="page-header">
        <h2>Persistent NUBAN Provisioning</h2>
        <p>Dedicated virtual accounts mapped to student identities for the duration of their degree.</p>
      </div>

      <div className="metrics-grid" style={{ gridTemplateColumns: 'repeat(3, 1fr)', marginBottom: '24px' }}>
        <div className="metric-card">
          <div className="metric-icon">🏦</div>
          <div className="metric-value blue">{MOCK_ACCOUNTS.length}</div>
          <div className="metric-label">Total Accounts</div>
        </div>
        <div className="metric-card">
          <div className="metric-icon">✅</div>
          <div className="metric-value green">{activeCount}</div>
          <div className="metric-label">Active</div>
        </div>
        <div className="metric-card">
          <div className="metric-icon">⏸️</div>
          <div className="metric-value" style={{ color: 'var(--text-muted)' }}>{MOCK_ACCOUNTS.length - activeCount}</div>
          <div className="metric-label">Inactive</div>
        </div>
      </div>

      <div className="flex items-center justify-between mb-6">
        <input
          type="text"
          className="search-input"
          placeholder="Search by name, matric, or NUBAN..."
          value={search}
          onChange={e => setSearch(e.target.value)}
        />
        <button className="btn btn-primary" onClick={() => setShowModal(true)}>+ Provision New Account</button>
      </div>

      {showModal && (
        <div style={{ position: 'fixed', inset: 0, background: 'rgba(0,0,0,0.7)', display: 'flex', alignItems: 'center', justifyContent: 'center', zIndex: 9999 }}>
          <div className="card" style={{ width: '480px', maxWidth: '90vw' }}>
            <h3 style={{ marginBottom: '20px' }}>Provision Virtual Account</h3>
            <div style={{ display: 'flex', flexDirection: 'column', gap: '12px' }}>
              <input className="input-field" placeholder="Student Full Name" />
              <input className="input-field" placeholder="Matric Number (e.g. UI/2025/CSC/0001)" />
              <input className="input-field" placeholder="BVN (11 digits)" />
              <select className="input-field" style={{ cursor: 'pointer' }}>
                {DEPTS.map(d => <option key={d}>{d}</option>)}
              </select>
            </div>
            <div className="flex gap-3 mt-6">
              <button className="btn btn-primary" onClick={() => setShowModal(false)}>Provision via Nomba API</button>
              <button className="btn btn-ghost" onClick={() => setShowModal(false)}>Cancel</button>
            </div>
          </div>
        </div>
      )}

      <div className="card" style={{ padding: 0, overflow: 'hidden' }}>
        <table className="data-table">
          <thead>
            <tr>
              <th>Student Name</th>
              <th>Matric Number</th>
              <th>NUBAN</th>
              <th>Bank</th>
              <th>Department</th>
              <th>Status</th>
            </tr>
          </thead>
          <tbody>
            {filtered.map(acc => (
              <tr key={acc.id}>
                <td style={{ color: 'var(--text)', fontWeight: 500, fontFamily: 'var(--font-sans)' }}>{acc.studentName}</td>
                <td style={{ color: 'var(--text-muted)' }}>{acc.matric}</td>
                <td style={{ color: 'var(--info)' }}>{acc.nuban}</td>
                <td style={{ color: 'var(--text-muted)', fontFamily: 'var(--font-sans)' }}>{acc.bankName}</td>
                <td style={{ color: 'var(--text-muted)', fontFamily: 'var(--font-sans)', fontSize: '0.78rem' }}>{acc.department}</td>
                <td>
                  <span className={`badge badge-${acc.status.toLowerCase()}`}>{acc.status}</span>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </div>
  );
}
