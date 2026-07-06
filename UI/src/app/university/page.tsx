'use client';

import { useState, useEffect, useRef } from 'react';

/*
 * University Portal Simulator
 * 
 * This is the second UI — a mock student-facing university payment portal
 * that demonstrates the exact problem NombaCrypt Shell solves:
 * 40,000 students trying to pay fees at the same time during registration week.
 *
 * Two modes:
 *   1. WITHOUT NombaCrypt (legacy) — portal freezes, timeouts, payment failures
 *   2. WITH NombaCrypt — smooth, instant, zero dropped payments
 */

interface PaymentAttempt {
  id: string;
  studentName: string;
  matric: string;
  amount: number;
  fee: string;
  status: 'QUEUED' | 'PROCESSING' | 'SUCCESS' | 'TIMEOUT' | 'FAILED' | 'DROPPED';
  timestamp: string;
  latencyMs: number | null;
}

const NIGERIAN_NAMES = [
  'Adebayo Olamide', 'Chidinma Okafor', 'Ibrahim Musa', 'Fatima Abdullahi', 'Oluwaseun Akinwale',
  'Ngozi Eze', 'Yusuf Bello', 'Blessing Onyekachi', 'Emeka Nwankwo', 'Aisha Mohammed',
  'Tunde Bakare', 'Amara Obi', 'Kehinde Adeyemi', 'Halima Suleiman', 'Chukwuemeka Agu',
  'Ifeoma Nwosu', 'Abdulrahman Idris', 'Grace Adichie', 'Musa Danjuma', 'Bukola Saraki',
  'Obinna Eze', 'Zainab Abubakar', 'Segun Obasanjo', 'Nneka Okwu', 'Dauda Lawal',
  'Funke Akindele', 'Yakubu Gowon', 'Chioma Ude', 'Aliyu Baba', 'Omolara Johnson',
];

const DEPTS = ['CSC', 'MED', 'LAW', 'ECO', 'ENG', 'PHM', 'ARC', 'ACC', 'POL', 'CHE'];
const FEES = ['Tuition Fee', 'Departmental Dues', 'Hostel Fee', 'Library Fee', 'Lab Fee', 'Exam Fee', 'Course Registration'];
const FEE_AMOUNTS: Record<string, number> = {
  'Tuition Fee': 320000, 'Departmental Dues': 45000, 'Hostel Fee': 85000,
  'Library Fee': 15000, 'Lab Fee': 25000, 'Exam Fee': 35000, 'Course Registration': 10000,
};

function randomStudent(): Omit<PaymentAttempt, 'status' | 'latencyMs'> {
  const name = NIGERIAN_NAMES[Math.floor(Math.random() * NIGERIAN_NAMES.length)];
  const dept = DEPTS[Math.floor(Math.random() * DEPTS.length)];
  const year = 2023 + Math.floor(Math.random() * 3);
  const fee = FEES[Math.floor(Math.random() * FEES.length)];
  return {
    id: crypto.randomUUID(),
    studentName: name,
    matric: `UI/${year}/${dept}/${String(Math.floor(Math.random() * 300) + 1).padStart(4, '0')}`,
    amount: FEE_AMOUNTS[fee],
    fee,
    timestamp: new Date().toISOString(),
  };
}

export default function UniversitySimulator() {
  const [mode, setMode] = useState<'idle' | 'legacy' | 'nombacrypt'>('idle');
  const [studentCount, setStudentCount] = useState(5000);
  const [payments, setPayments] = useState<PaymentAttempt[]>([]);
  const [stats, setStats] = useState({ success: 0, failed: 0, timeout: 0, dropped: 0, processing: 0 });
  const [elapsed, setElapsed] = useState(0);
  const [serverLoad, setServerLoad] = useState(0);
  const [showCrash, setShowCrash] = useState(false);
  const intervalRef = useRef<NodeJS.Timeout | null>(null);
  const timerRef = useRef<NodeJS.Timeout | null>(null);

  const resetState = () => {
    setPayments([]);
    setStats({ success: 0, failed: 0, timeout: 0, dropped: 0, processing: 0 });
    setElapsed(0);
    setServerLoad(0);
    setShowCrash(false);
    if (intervalRef.current) clearInterval(intervalRef.current);
    if (timerRef.current) clearInterval(timerRef.current);
  };

  const startSimulation = (simMode: 'legacy' | 'nombacrypt') => {
    resetState();
    setMode(simMode);

    let count = 0;
    const batchSize = simMode === 'legacy' ? 30 : 80;
    const intervalMs = simMode === 'legacy' ? 200 : 60;

    timerRef.current = setInterval(() => setElapsed(prev => prev + 100), 100);

    intervalRef.current = setInterval(() => {
      if (count >= studentCount) {
        if (intervalRef.current) clearInterval(intervalRef.current);
        if (timerRef.current) clearInterval(timerRef.current);
        setMode('idle');
        return;
      }

      const batch: PaymentAttempt[] = [];
      for (let i = 0; i < batchSize && count < studentCount; i++) {
        const student = randomStudent();
        count++;

        if (simMode === 'legacy') {
          // Legacy: after ~40% load, start failing hard
          const loadFactor = count / studentCount;
          setServerLoad(Math.min(loadFactor * 200, 100));
          const rand = Math.random();

          if (loadFactor > 0.3 && rand < 0.35) {
            batch.push({ ...student, status: 'TIMEOUT', latencyMs: 30000 + Math.floor(Math.random() * 10000) });
            setStats(prev => ({ ...prev, timeout: prev.timeout + 1 }));
          } else if (loadFactor > 0.5 && rand < 0.55) {
            batch.push({ ...student, status: 'DROPPED', latencyMs: null });
            setStats(prev => ({ ...prev, dropped: prev.dropped + 1 }));
          } else if (loadFactor > 0.4 && rand < 0.25) {
            batch.push({ ...student, status: 'FAILED', latencyMs: 5000 + Math.floor(Math.random() * 15000) });
            setStats(prev => ({ ...prev, failed: prev.failed + 1 }));
          } else {
            batch.push({ ...student, status: 'SUCCESS', latencyMs: 2000 + Math.floor(Math.random() * 8000) });
            setStats(prev => ({ ...prev, success: prev.success + 1 }));
          }

          // Trigger crash screen at high load
          if (loadFactor > 0.65 && !showCrash) setShowCrash(true);

        } else {
          // NombaCrypt: smooth, near-instant
          setServerLoad(Math.min((count / studentCount) * 40, 35));
          batch.push({ ...student, status: 'SUCCESS', latencyMs: Math.floor(Math.random() * 50) + 5 });
          setStats(prev => ({ ...prev, success: prev.success + 1 }));
        }
      }

      setPayments(prev => [...batch, ...prev].slice(0, 200));
    }, intervalMs);
  };

  useEffect(() => {
    return () => {
      if (intervalRef.current) clearInterval(intervalRef.current);
      if (timerRef.current) clearInterval(timerRef.current);
    };
  }, []);

  const totalProcessed = stats.success + stats.failed + stats.timeout + stats.dropped;
  const successRate = totalProcessed > 0 ? ((stats.success / totalProcessed) * 100).toFixed(1) : '0.0';

  return (
    <div style={{ minHeight: '100vh', background: '#0a0a0f', color: '#f0f0f0', fontFamily: "'Inter', system-ui, sans-serif" }}>
      {/* University Header */}
      <header style={{
        background: 'linear-gradient(135deg, #1a3a5c 0%, #0d2137 100%)',
        padding: '20px 40px',
        borderBottom: '2px solid #2a4a6c',
        display: 'flex', alignItems: 'center', justifyContent: 'space-between'
      }}>
        <div style={{ display: 'flex', alignItems: 'center', gap: '16px' }}>
          <div style={{ width: '48px', height: '48px', borderRadius: '50%', background: '#c8a200', display: 'flex', alignItems: 'center', justifyContent: 'center', fontSize: '24px', fontWeight: 800, color: '#1a3a5c' }}>U</div>
          <div>
            <h1 style={{ fontSize: '1.3rem', fontWeight: 700, margin: 0, letterSpacing: '-0.01em' }}>University of Ibadan</h1>
            <div style={{ fontSize: '0.75rem', color: '#8899aa', letterSpacing: '0.05em' }}>STUDENT PAYMENT PORTAL — REGISTRATION WEEK</div>
          </div>
        </div>
        <div style={{ fontSize: '0.8rem', color: '#ff6666', fontWeight: 600, display: 'flex', alignItems: 'center', gap: '8px' }}>
          <span style={{ width: '8px', height: '8px', borderRadius: '50%', background: mode !== 'idle' ? '#ff3366' : '#00ff88', animation: mode !== 'idle' ? 'pulse-glow 1s ease-in-out infinite' : 'none' }}></span>
          {mode === 'idle' ? 'Portal Ready' : `LIVE — ${totalProcessed.toLocaleString()} / ${studentCount.toLocaleString()} students`}
        </div>
      </header>

      <div style={{ maxWidth: '1400px', margin: '0 auto', padding: '32px 40px' }}>
        {/* Crash Overlay for Legacy mode */}
        {showCrash && mode === 'legacy' && (
          <div style={{
            background: 'rgba(255, 20, 20, 0.08)', border: '2px solid rgba(255, 51, 102, 0.5)',
            borderRadius: '12px', padding: '24px 32px', marginBottom: '24px',
            animation: 'slide-in 0.5s ease-out',
          }}>
            <div style={{ fontSize: '1.2rem', fontWeight: 700, color: '#ff3366', marginBottom: '8px' }}>
              ⚠️ SERVER OVERLOAD — Portal Unresponsive
            </div>
            <div style={{ color: '#ff8899', fontSize: '0.85rem' }}>
              Connection pool exhausted. Students experiencing &quot;Error 504: Gateway Timeout&quot; and &quot;Error 503: Service Unavailable&quot;.
              The payment gateway is dropping requests faster than they can be processed.
              Admin intervention required.
            </div>
          </div>
        )}

        {/* Mode Selection */}
        {mode === 'idle' && (
          <div style={{ textAlign: 'center', padding: '40px 0 32px' }}>
            <h2 style={{ fontSize: '1.6rem', fontWeight: 700, marginBottom: '8px' }}>Registration Week Traffic Simulator</h2>
            <p style={{ color: '#8888aa', maxWidth: '600px', margin: '0 auto 32px', fontSize: '0.9rem' }}>
              Simulate {studentCount.toLocaleString()} students trying to pay their fees simultaneously.
              Compare what happens with and without NombaCrypt Shell.
            </p>

            <div style={{ maxWidth: '400px', margin: '0 auto 32px' }}>
              <label style={{ display: 'block', fontSize: '0.8rem', color: '#8888aa', marginBottom: '8px' }}>
                Concurrent Students: <strong style={{ color: '#f0f0f0' }}>{studentCount.toLocaleString()}</strong>
              </label>
              <input type="range" min="1000" max="20000" step="1000" value={studentCount}
                onChange={e => setStudentCount(Number(e.target.value))}
                style={{ width: '100%' }}
              />
            </div>

            <div style={{ display: 'flex', gap: '20px', justifyContent: 'center' }}>
              <button onClick={() => startSimulation('legacy')} style={{
                padding: '16px 32px', borderRadius: '12px', border: '2px solid rgba(255,51,102,0.4)',
                background: 'rgba(255,51,102,0.1)', color: '#ff3366', fontSize: '1rem', fontWeight: 700,
                cursor: 'pointer', transition: '200ms ease',
              }}>
                ❌ Run WITHOUT NombaCrypt
              </button>
              <button onClick={() => startSimulation('nombacrypt')} style={{
                padding: '16px 32px', borderRadius: '12px', border: '2px solid rgba(0,255,136,0.4)',
                background: 'rgba(0,255,136,0.1)', color: '#00ff88', fontSize: '1rem', fontWeight: 700,
                cursor: 'pointer', transition: '200ms ease', boxShadow: '0 0 20px rgba(0,255,136,0.15)',
              }}>
                ✅ Run WITH NombaCrypt Shell
              </button>
            </div>
          </div>
        )}

        {/* Live Stats Bar */}
        {mode !== 'idle' && (
          <>
            <div style={{
              display: 'grid', gridTemplateColumns: 'repeat(6, 1fr)', gap: '12px', marginBottom: '24px'
            }}>
              <div className="metric-card">
                <div className="metric-value green">{stats.success.toLocaleString()}</div>
                <div className="metric-label">Success</div>
              </div>
              <div className="metric-card">
                <div className="metric-value red">{stats.failed.toLocaleString()}</div>
                <div className="metric-label">Failed</div>
              </div>
              <div className="metric-card">
                <div className="metric-value amber">{stats.timeout.toLocaleString()}</div>
                <div className="metric-label">Timeouts</div>
              </div>
              <div className="metric-card">
                <div className="metric-value" style={{ color: '#8888aa' }}>{stats.dropped.toLocaleString()}</div>
                <div className="metric-label">Dropped</div>
              </div>
              <div className="metric-card">
                <div className="metric-value blue">{successRate}%</div>
                <div className="metric-label">Success Rate</div>
              </div>
              <div className="metric-card">
                <div className="metric-value purple">{(elapsed / 1000).toFixed(1)}s</div>
                <div className="metric-label">Elapsed</div>
              </div>
            </div>

            {/* Server Load Bar */}
            <div className="card" style={{ marginBottom: '24px' }}>
              <div style={{ display: 'flex', justifyContent: 'space-between', marginBottom: '8px' }}>
                <span style={{ fontSize: '0.8rem', color: '#8888aa' }}>Server CPU Load</span>
                <span style={{ fontSize: '0.85rem', fontFamily: "'JetBrains Mono', monospace", fontWeight: 700, color: serverLoad > 80 ? '#ff3366' : serverLoad > 50 ? '#ffaa00' : '#00ff88' }}>
                  {serverLoad.toFixed(0)}%
                </span>
              </div>
              <div className="progress-bar" style={{ height: '10px' }}>
                <div
                  className={`progress-fill ${serverLoad > 80 ? 'red' : serverLoad > 50 ? 'blue' : 'green'}`}
                  style={{ width: `${serverLoad}%`, transition: 'width 0.3s ease' }}
                ></div>
              </div>
            </div>

            {/* Live Payment Feed */}
            <div className="section-header">
              <h3>Live Payment Queue</h3>
              <span className="badge badge-success">STREAMING</span>
            </div>
            <div className="terminal" style={{ maxHeight: '450px' }}>
              {payments.map(p => (
                <div key={p.id} className="terminal-line">
                  <span className="ts">[{new Date(p.timestamp).toLocaleTimeString()}]</span>
                  <span style={{ color: '#3399ff', minWidth: '140px', fontSize: '0.75rem' }}>{p.matric}</span>
                  <span style={{ color: '#f0f0f0', minWidth: '150px', fontFamily: "'Inter', sans-serif", fontSize: '0.78rem' }}>{p.studentName}</span>
                  <span style={{ color: '#8888aa', minWidth: '130px', fontSize: '0.75rem' }}>{p.fee}</span>
                  <span style={{ color: '#00ff88', minWidth: '90px', textAlign: 'right' }}>₦{p.amount.toLocaleString()}</span>
                  <span style={{ minWidth: '80px', textAlign: 'right', color: p.latencyMs && p.latencyMs > 5000 ? '#ff3366' : '#7b61ff', fontSize: '0.75rem' }}>
                    {p.latencyMs !== null ? (p.latencyMs > 1000 ? `${(p.latencyMs / 1000).toFixed(1)}s` : `${p.latencyMs}ms`) : '—'}
                  </span>
                  <span className={`badge badge-${p.status.toLowerCase()}`} style={{ minWidth: '70px', textAlign: 'center' }}>
                    {p.status}
                  </span>
                </div>
              ))}
            </div>
          </>
        )}
      </div>
    </div>
  );
}
