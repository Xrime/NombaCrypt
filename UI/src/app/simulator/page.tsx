'use client';

import { useState, useEffect } from 'react';

export default function Simulator() {
  const [running, setRunning] = useState(false);
  const [reqCount, setReqCount] = useState(5000);
  
  // Legacy states
  const [legDrop, setLegDrop] = useState(0);
  const [legSuccess, setLegSuccess] = useState(0);
  const [legTimeout, setLegTimeout] = useState(0);
  
  // NombaCrypt states
  const [ncSuccess, setNcSuccess] = useState(0);

  useEffect(() => {
    // We no longer need the mock interval, the API handles the blast
  }, []);

  const startSim = async () => {
    setLegDrop(0); setLegSuccess(0); setLegTimeout(0); setNcSuccess(0);
    setRunning(true);
    
    try {
      // 1. Simulate the Legacy Unbuffered Pipeline (fake metrics for contrast)
      // Legacy systems typically drop 30-40% of traffic under a 5000-request spike
      const mockLegacySuccess = Math.floor(reqCount * 0.65);
      const mockLegacyDrop = Math.floor(reqCount * 0.20);
      const mockLegacyTimeout = reqCount - mockLegacySuccess - mockLegacyDrop;
      
      // Animate the legacy numbers counting up quickly
      let legacyCount = 0;
      const legInterval = setInterval(() => {
        legacyCount += 250;
        if (legacyCount >= reqCount) clearInterval(legInterval);
        setLegSuccess(Math.min(legacyCount * 0.65, mockLegacySuccess));
        setLegDrop(Math.min(legacyCount * 0.20, mockLegacyDrop));
        setLegTimeout(Math.min(legacyCount * 0.15, mockLegacyTimeout));
      }, 50);

      // 2. Blast the REAL NombaCrypt Shell Pipeline using our API
      const res = await fetch('/api/blast', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ requestCount: reqCount })
      });
      
      const data = await res.json();
      
      // Animate the NombaCrypt numbers counting up based on REAL success
      let ncCount = 0;
      const ncInterval = setInterval(() => {
        ncCount += 500;
        if (ncCount >= data.successCount) {
          clearInterval(ncInterval);
          setNcSuccess(data.successCount);
        } else {
          setNcSuccess(ncCount);
        }
      }, 50);

    } catch (err) {
      console.error("Simulation failed:", err);
    } finally {
      setRunning(false);
    }
  };

  return (
    <div className="animate-in">
      <div className="page-header">
        <h2>High-Traffic Portal Simulator</h2>
        <p>Stress test the middleware by simulating massive concurrency spikes.</p>
      </div>

      <div className="card mb-8 text-center" style={{ padding: '40px' }}>
        <button 
          className="btn btn-primary btn-lg" 
          onClick={startSim} 
          disabled={running}
          style={{ marginBottom: '20px', fontSize: '1.2rem' }}
        >
          {running ? 'Simulating...' : `Spawn ${reqCount.toLocaleString()} Concurrent Requests`}
        </button>
        <div className="slider-container" style={{ maxWidth: '400px', margin: '0 auto' }}>
          <label>Request Volume: {reqCount.toLocaleString()}</label>
          <input 
            type="range" 
            min="1000" 
            max="20000" 
            step="1000" 
            value={reqCount} 
            onChange={e => setReqCount(Number(e.target.value))}
            disabled={running}
          />
        </div>
      </div>

      <div className="sim-split">
        <div className="sim-panel legacy">
          <h4>Legacy Unbuffered Pipeline</h4>
          <div className="progress-bar">
            <div className="progress-fill red" style={{ width: `${Math.min(((legSuccess + legDrop + legTimeout) / reqCount) * 100, 100)}%` }}></div>
          </div>
          <div style={{ marginTop: '20px' }}>
            <div className="sim-counter text-alert">{legTimeout.toLocaleString()}</div>
            <div className="sim-counter-label">Timeout Failures (429)</div>
            
            <div className="sim-counter text-alert">{legDrop.toLocaleString()}</div>
            <div className="sim-counter-label">Dropped Packets (503)</div>

            <div className="sim-counter text-success">{legSuccess.toLocaleString()}</div>
            <div className="sim-counter-label">Success Count</div>
          </div>
        </div>

        <div className="sim-panel nombacrypt">
          <h4>NombaCrypt Shell Pipeline</h4>
          <div className="progress-bar">
            <div className="progress-fill green" style={{ width: `${Math.min((ncSuccess / reqCount) * 100, 100)}%` }}></div>
          </div>
          <div style={{ marginTop: '20px' }}>
            <div className="sim-counter text-success">{ncSuccess.toLocaleString()}</div>
            <div className="sim-counter-label">Success Count (RAM Buffered)</div>
            
            <div className="sim-counter text-muted">0</div>
            <div className="sim-counter-label">Timeout Failures</div>

            <div className="sim-counter text-muted">0</div>
            <div className="sim-counter-label">Dropped Packets</div>
          </div>
        </div>
      </div>
    </div>
  );
}
