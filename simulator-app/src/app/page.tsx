'use client';

import { useState } from 'react';

export default function SimulatorPage() {
  const [concurrency, setConcurrency] = useState<number>(50);
  const [transactions, setTransactions] = useState<number>(1000);
  const [status, setStatus] = useState<'IDLE' | 'BLASTING' | 'COMPLETE'>('IDLE');
  const [progress, setProgress] = useState(0);
  const [attackStatus, setAttackStatus] = useState<string>('');
  const [lastResult, setLastResult] = useState<string>('');

  const handleBlast = async () => {
    setStatus('BLASTING');
    setProgress(0);
    setLastResult('');

    const batchSize = concurrency;
    const totalBatches = Math.ceil(transactions / batchSize);

    for (let i = 0; i < totalBatches; i++) {
      try {
        const res = await fetch('/api/blast', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ batchSize, isAttack: false })
        });
        const data = await res.json();
        if (!data.success) {
          setLastResult(`Batch ${i + 1} failed`);
        }
      } catch (err) {
        setLastResult(`Error on batch ${i + 1}`);
      }
      setProgress(Math.round(((i + 1) / totalBatches) * 100));
    }
    
    setStatus('COMPLETE');
    setLastResult(`Successfully sent ${transactions} transactions to the engine.`);
  };

  const handleSimulateAttack = async () => {
    setAttackStatus('SENDING...');
    try {
      const res = await fetch('/api/blast', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ batchSize: 5, isAttack: true })
      });
      const data = await res.json();
      if (data.success) {
        setAttackStatus('ATTACK SENT — Check Control App for HMAC_FAIL events!');
      } else {
        setAttackStatus('Attack failed to send.');
      }
    } catch {
      setAttackStatus('Connection error. Is the C++ engine running?');
    }
    setTimeout(() => setAttackStatus(''), 5000);
  };

  return (
    <div className="animate-in" style={{ padding: '40px', maxWidth: '800px', margin: '0 auto' }}>
      <div className="page-header" style={{ marginBottom: '40px', textAlign: 'center' }}>
        <h2 style={{ fontSize: '2.5rem', color: '#ffcc00', marginBottom: '8px' }}>NombaCrypt Simulator</h2>
        <p style={{ color: 'var(--text-muted)' }}>High-Frequency Payment Traffic Generator</p>
      </div>

      <div className="card" style={{ padding: '32px' }}>
        <div className="section-header" style={{ borderBottom: '1px solid var(--border)', paddingBottom: '16px', marginBottom: '24px' }}>
          <h3>Load Configuration</h3>
        </div>

        <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: '24px', marginBottom: '32px' }}>
          <div style={{ display: 'flex', flexDirection: 'column', gap: '8px' }}>
            <label style={{ fontSize: '0.85rem', color: 'var(--text-muted)', fontWeight: 600 }}>Total Transactions</label>
            <input 
              type="number" 
              className="input-field" 
              value={transactions} 
              onChange={(e) => setTransactions(Number(e.target.value))} 
              style={{ fontSize: '1.5rem', padding: '16px' }}
            />
          </div>
          
          <div style={{ display: 'flex', flexDirection: 'column', gap: '8px' }}>
            <label style={{ fontSize: '0.85rem', color: 'var(--text-muted)', fontWeight: 600 }}>Concurrency Level</label>
            <input 
              type="number" 
              className="input-field" 
              value={concurrency} 
              onChange={(e) => setConcurrency(Number(e.target.value))} 
              style={{ fontSize: '1.5rem', padding: '16px' }}
            />
          </div>
        </div>

        <div style={{ display: 'flex', gap: '16px' }}>
          <button 
            onClick={handleBlast} 
            disabled={status === 'BLASTING'}
            className="btn btn-primary" 
            style={{ flex: 2, padding: '20px', fontSize: '1.1rem', justifyContent: 'center', background: status === 'BLASTING' ? 'var(--bg-hover)' : 'linear-gradient(135deg, #ffcc00, #ffaa00)', color: status === 'BLASTING' ? '#888' : '#000' }}
          >
            {status === 'BLASTING' ? `SIMULATION IN PROGRESS (${progress}%)` : status === 'COMPLETE' ? 'RUN AGAIN' : 'INITIATE TRAFFIC SPIKE'}
          </button>

          <button 
            onClick={handleSimulateAttack} 
            className="btn btn-danger" 
            style={{ flex: 1, padding: '20px', fontSize: '1.1rem', justifyContent: 'center' }}
          >
            Simulate Attack
          </button>
        </div>

        {attackStatus && (
          <div style={{ 
            marginTop: '16px', padding: '12px 20px', borderRadius: '8px', fontSize: '0.9rem', fontWeight: 600,
            background: 'rgba(255, 51, 102, 0.12)', color: 'var(--alert)', border: '1px solid rgba(255, 51, 102, 0.3)',
            animation: 'fade-in 0.3s ease-out'
          }}>
            {attackStatus}
          </div>
        )}

        {lastResult && (
          <div style={{ 
            marginTop: '12px', padding: '12px 20px', borderRadius: '8px', fontSize: '0.85rem',
            background: 'rgba(0, 255, 136, 0.08)', color: 'var(--success)', border: '1px solid rgba(0, 255, 136, 0.2)'
          }}>
            {lastResult}
          </div>
        )}

        {progress > 0 && (
          <div style={{ marginTop: '24px' }}>
            <div style={{ display: 'flex', justifyContent: 'space-between', marginBottom: '8px', fontSize: '0.85rem', color: 'var(--text-muted)' }}>
              <span>Progress</span>
              <span>{progress}%</span>
            </div>
            <div className="progress-bar" style={{ height: '12px', background: 'var(--bg-hover)', borderRadius: '6px' }}>
              <div className="progress-fill" style={{ width: `${progress}%`, height: '100%', background: '#ffcc00', borderRadius: '6px', transition: 'width 0.2s ease' }}></div>
            </div>
          </div>
        )}
      </div>
    </div>
  );
}
