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
    if (!running) return;
    
    let currentReqs = 0;
    const interval = setInterval(() => {
      currentReqs += 100;
      if (currentReqs >= reqCount) {
        clearInterval(interval);
        setRunning(false);
      }
      
      // Legacy behavior: high failure rate under load
      setLegSuccess(prev => prev + Math.floor(Math.random() * 20));
      setLegDrop(prev => prev + Math.floor(Math.random() * 40));
      setLegTimeout(prev => prev + Math.floor(Math.random() * 40));
      
      // NombaCrypt behavior: perfect queueing
      setNcSuccess(currentReqs);

    }, 50);
    
    return () => clearInterval(interval);
  }, [running, reqCount]);

  const startSim = () => {
    setLegDrop(0); setLegSuccess(0); setLegTimeout(0); setNcSuccess(0);
    setRunning(true);
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
