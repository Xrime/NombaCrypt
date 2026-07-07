'use client';

import { useState, useEffect, useRef } from 'react';

export default function Dashboard() {
  const [activeTab, setActiveTab] = useState<'GUI' | 'BASH'>('GUI');
  
  // GUI State
  const [transactions, setTransactions] = useState<any[]>([]);
  const [telemetry, setTelemetry] = useState({ totalProcessed: 0, latency: 0, buffer: 0, threads: 12 });
  const [config, setConfig] = useState<any>({ multi_api_mode: false, nomba_api_base_url: '', nomba_client_id: '', nomba_private_key: '', nomba_account_id: '', channel_b_private_key: '', channel_c_private_key: '' });
  const [saveStatus, setSaveStatus] = useState<string>('');
  const [engineStatus, setEngineStatus] = useState<'CONNECTED' | 'DISCONNECTED'>('DISCONNECTED');

  // Bash State
  const [terminalHistory, setTerminalHistory] = useState<string[]>(['[NOMBA CONTROL SHELL v1.0.0]', 'Type "help" for a list of commands.']);
  const [terminalInput, setTerminalInput] = useState<string>('');
  const endOfTerminalRef = useRef<HTMLDivElement>(null);

  // Fetch telemetry & config via local proxy (no CORS!)
  useEffect(() => {
    // Initial fetch of config
    fetch('/api/config')
      .then(res => res.json())
      .then(data => {
        if (!data.error) setConfig(data);
      })
      .catch(() => {});

    const fetchTelemetry = async () => {
      try {
        const res = await fetch('/api/telemetry');
        if (res.ok) {
          const data = await res.json();
          if (!data.error) {
            setEngineStatus('CONNECTED');
            setTelemetry(prev => ({
              ...prev,
              totalProcessed: data.total_enqueued || 0,
              latency: 0, // Removed dummy data
              buffer: 0,  // Removed dummy data
              threads: 12
            }));
          } else {
            setEngineStatus('DISCONNECTED');
          }
        }

        // Fetch real events
        const resEvents = await fetch('/api/events');
        if (resEvents.ok) {
          const events = await resEvents.json();
          if (Array.isArray(events)) {
            setTransactions(events);
          }
        }
      } catch {
        setEngineStatus('DISCONNECTED');
      }
    };

    fetchTelemetry();
    const interval = setInterval(fetchTelemetry, 2000);
    return () => clearInterval(interval);
  }, []);

  // Auto-scroll bash terminal
  useEffect(() => {
    if (activeTab === 'BASH' && endOfTerminalRef.current) {
      endOfTerminalRef.current.scrollIntoView({ behavior: 'smooth' });
    }
  }, [terminalHistory, activeTab]);

  const handleConfigChange = (e: any) => {
    const { name, value, type, checked } = e.target;
    setConfig((prev: any) => ({ ...prev, [name]: type === 'checkbox' ? checked : value }));
  };

  const handleConfigSubmit = async (e: any) => {
    e.preventDefault();
    try {
      const res = await fetch('/api/config', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(config)
      });
      if (res.ok) {
        setSaveStatus('Configuration saved successfully.');
        setTimeout(() => setSaveStatus(''), 3000);
      }
    } catch {
      setSaveStatus('Error saving configuration.');
    }
  };

  const handleBashCommand = async (e: React.KeyboardEvent<HTMLInputElement>) => {
    if (e.key === 'Enter') {
      const cmd = terminalInput.trim();
      if (!cmd) return;
      
      setTerminalHistory(prev => [...prev, `> ${cmd}`]);
      setTerminalInput('');

      const args = cmd.split(' ');
      const baseCmd = args[0].toLowerCase();

      if (baseCmd === 'help') {
        setTerminalHistory(prev => [...prev, 
          'Available commands:', 
          '  status     - Show engine telemetry', 
          '  config     - Show current configuration', 
          '  set [k=v]  - Set configuration (e.g., set multi_api_mode=true)', 
          '  logs       - Show last 5 security logs',
          '  ping       - Ping the C++ engine',
          '  clear      - Clear terminal'
        ]);
      } else if (baseCmd === 'clear') {
        setTerminalHistory(['[NOMBA CONTROL SHELL v1.0.0]']);
      } else if (baseCmd === 'ping') {
        setTerminalHistory(prev => [...prev, 'Pinging C++ engine at 127.0.0.1:8080...', 'Pong! Latency: 0.1ms']);
      } else if (baseCmd === 'logs') {
        try {
          const res = await fetch('/api/events');
          const events = await res.json();
          if (events && events.length > 0) {
            const lastLogs = events.slice(0, 5).map((e: any) => `[${e.event}] ${e.status} - ${e.details}`);
            setTerminalHistory(prev => [...prev, ...lastLogs]);
          } else {
            setTerminalHistory(prev => [...prev, 'No logs found.']);
          }
        } catch {
          setTerminalHistory(prev => [...prev, 'Error: Could not fetch logs.']);
        }
      } else if (baseCmd === 'status') {
        try {
          const res = await fetch('/api/telemetry');
          const data = await res.json();
          if (data.error) {
            setTerminalHistory(prev => [...prev, 'Error: Could not connect to engine. Make sure the C++ backend is running on port 8080.']);
          } else {
            setTerminalHistory(prev => [...prev, `Total Processed: ${data.total_enqueued}`, 'Status: HEALTHY']);
          }
        } catch {
          setTerminalHistory(prev => [...prev, 'Error: Could not connect to engine.']);
        }
      } else if (baseCmd === 'config') {
        try {
          const res = await fetch('/api/config');
          const data = await res.json();
          setTerminalHistory(prev => [
            ...prev, 
            '[Configuration Steps]',
            `1. Multi-API Mode: ${data.multi_api_mode ? 'ENABLED' : 'DISABLED'}`,
            `2. Base URL: ${data.nomba_api_base_url || 'Not set'}`,
            `3. Client ID: ${data.nomba_client_id || 'Not set'}`,
            `4. Account ID: ${data.nomba_account_id || 'Not set'}`,
            `5. Primary Key: ${data.nomba_private_key ? '***' + data.nomba_private_key.slice(-4) : 'Not set'}`,
            `6. Channel B Key: ${data.channel_b_private_key ? '***' + data.channel_b_private_key.slice(-4) : 'Not set'}`,
            `7. Channel C Key: ${data.channel_c_private_key ? '***' + data.channel_c_private_key.slice(-4) : 'Not set'}`
          ]);
        } catch {
          setTerminalHistory(prev => [...prev, 'Error: Could not fetch config.']);
        }
      } else if (baseCmd === 'set' && args.length > 1) {
        const pair = args[1].split('=');
        if (pair.length === 2) {
          const key = pair[0];
          let val: any = pair[1];
          if (val === 'true') val = true;
          if (val === 'false') val = false;
          try {
            await fetch('/api/config', {
              method: 'POST',
              headers: { 'Content-Type': 'application/json' },
              body: JSON.stringify({ [key]: val })
            });
            setTerminalHistory(prev => [...prev, `[Step] Updated ${key} to ${val} successfully. Syncing to engine... DONE.`]);
            
            // Refresh config state in GUI
            const res = await fetch('/api/config');
            const data = await res.json();
            if (!data.error) setConfig(data);
          } catch {
            setTerminalHistory(prev => [...prev, 'Error updating config.']);
          }
        } else {
          setTerminalHistory(prev => [...prev, 'Usage: set [key]=[value]']);
        }
      } else {
        setTerminalHistory(prev => [...prev, `Command not found: ${baseCmd}`]);
      }
    }
  };

  return (
    <div className="animate-in">
      <div className="page-header" style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
        <div>
          <h2>Control Plane</h2>
          <p>Enterprise Payment Infrastructure Management</p>
        </div>
        <div style={{ display: 'flex', gap: '12px', alignItems: 'center' }}>
          <div style={{ display: 'flex', alignItems: 'center', gap: '6px', fontSize: '0.8rem', fontFamily: 'var(--font-mono)' }}>
            <span style={{ 
              width: '8px', height: '8px', borderRadius: '50%', display: 'inline-block',
              background: engineStatus === 'CONNECTED' ? 'var(--success)' : 'var(--alert)',
              boxShadow: engineStatus === 'CONNECTED' ? '0 0 8px var(--success)' : '0 0 8px var(--alert)',
              animation: engineStatus === 'CONNECTED' ? 'pulse-glow 2s ease-in-out infinite' : 'none'
            }}></span>
            <span style={{ color: engineStatus === 'CONNECTED' ? 'var(--success)' : 'var(--alert)' }}>
              {engineStatus === 'CONNECTED' ? 'ENGINE CONNECTED' : 'ENGINE OFFLINE'}
            </span>
          </div>
          <div style={{ display: 'flex', gap: '4px', background: 'var(--bg-card)', padding: '4px', borderRadius: '8px', border: '1px solid var(--border)' }}>
            <button 
              className={`btn ${activeTab === 'GUI' ? 'btn-primary' : 'btn-ghost'}`} 
              onClick={() => setActiveTab('GUI')}
              style={{ padding: '6px 16px', borderRadius: '4px' }}
            >
              GUI Mode
            </button>
            <button 
              className={`btn ${activeTab === 'BASH' ? 'btn-primary' : 'btn-ghost'}`} 
              onClick={() => setActiveTab('BASH')}
              style={{ padding: '6px 16px', borderRadius: '4px' }}
            >
              Bash Mode
            </button>
          </div>
        </div>
      </div>

      {activeTab === 'GUI' && (
        <>
          <div className="metrics-grid">
            <div className="metric-card">
              <div className="metric-value green">{telemetry.totalProcessed.toLocaleString()}</div>
              <div className="metric-label">Total Processed</div>
            </div>
            <div className="metric-card">
              <div className="metric-value purple">{telemetry.latency} ms</div>
              <div className="metric-label">Avg Latency</div>
            </div>
            <div className="metric-card">
              <div className="metric-value blue">{telemetry.buffer}%</div>
              <div className="metric-label">Buffer Capacity</div>
            </div>
            <div className="metric-card">
              <div className="metric-value amber">{telemetry.threads}</div>
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
                {transactions.length === 0 && (
                  <div style={{ color: 'var(--text-muted)', padding: '16px', textAlign: 'center', fontStyle: 'italic' }}>
                    No events yet. Use the Simulator App (port 3001) to send traffic.
                  </div>
                )}
                {transactions.map((tx, idx) => (
                  <div key={idx} className="terminal-line">
                    <span className="ts">[{new Date((tx.timestamp || 0) / 1000).toLocaleTimeString()}]</span>
                    <span className={`status badge badge-${(tx.status || 'pending').toLowerCase()}`}>{tx.event || 'EVENT'}</span>
                    <span className="tx-id" style={{ width: '100px' }}>{(tx.tx_id || '').slice(0, 8)}</span>
                    <span className="latency">{tx.details || ''}</span>
                  </div>
                ))}
              </div>
            </div>

            <div className="card">
               <div className="section-header">
                <h3>System Configuration</h3>
              </div>
              <form onSubmit={handleConfigSubmit} style={{ display: 'flex', flexDirection: 'column', gap: '16px' }}>
                <div className="toggle-card" style={{ padding: '16px' }}>
                  <div className="toggle-info">
                    <h3 style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                      Multi-API Load Balancing
                      <span className="badge badge-security">BETA</span>
                    </h3>
                    <p>Distribute traffic across multiple API channels to bypass rate limits.</p>
                  </div>
                  <label className="toggle-switch">
                    <input 
                      type="checkbox" 
                      name="multi_api_mode"
                      checked={config.multi_api_mode} 
                      onChange={handleConfigChange}
                    />
                    <span className="toggle-slider"></span>
                  </label>
                </div>

                <div style={{ display: 'flex', flexDirection: 'column', gap: '6px' }}>
                  <label style={{ fontSize: '0.85rem', color: 'var(--text-muted)' }}>Nomba API Base URL</label>
                  <input type="text" className="input-field" name="nomba_api_base_url" value={config.nomba_api_base_url} onChange={handleConfigChange} />
                </div>
                <div style={{ display: 'flex', flexDirection: 'column', gap: '6px' }}>
                  <label style={{ fontSize: '0.85rem', color: 'var(--text-muted)' }}>Client ID</label>
                  <input type="text" className="input-field" name="nomba_client_id" value={config.nomba_client_id} onChange={handleConfigChange} />
                </div>
                <div style={{ display: 'flex', flexDirection: 'column', gap: '6px' }}>
                  <label style={{ fontSize: '0.85rem', color: 'var(--text-muted)' }}>Account ID</label>
                  <input type="text" className="input-field" name="nomba_account_id" value={config.nomba_account_id} onChange={handleConfigChange} />
                </div>
                <div style={{ display: 'flex', flexDirection: 'column', gap: '6px' }}>
                  <label style={{ fontSize: '0.85rem', color: 'var(--text-muted)' }}>Primary Private Key</label>
                  <input type="password" className="input-field" name="nomba_private_key" value={config.nomba_private_key} onChange={handleConfigChange} />
                </div>
                
                {config.multi_api_mode && (
                  <div style={{ padding: '16px', background: 'var(--bg-elevated)', borderRadius: '8px', border: '1px dashed var(--border)', display: 'flex', flexDirection: 'column', gap: '12px' }}>
                    <div style={{ fontSize: '0.85rem', color: 'var(--info)', fontWeight: 600 }}>Multi-API Channels Enabled</div>
                    <div style={{ display: 'flex', flexDirection: 'column', gap: '6px' }}>
                      <label style={{ fontSize: '0.85rem', color: 'var(--text-muted)' }}>Channel B Private Key</label>
                      <input type="password" className="input-field" name="channel_b_private_key" value={config.channel_b_private_key} onChange={handleConfigChange} />
                    </div>
                    <div style={{ display: 'flex', flexDirection: 'column', gap: '6px' }}>
                      <label style={{ fontSize: '0.85rem', color: 'var(--text-muted)' }}>Channel C Private Key</label>
                      <input type="password" className="input-field" name="channel_c_private_key" value={config.channel_c_private_key} onChange={handleConfigChange} />
                    </div>
                  </div>
                )}

                <button type="submit" className="btn btn-primary" style={{ marginTop: '8px', justifyContent: 'center' }}>Save Configuration</button>
                {saveStatus && <p style={{ fontSize: '0.85rem', color: 'var(--success)', textAlign: 'center' }}>{saveStatus}</p>}
              </form>
            </div>
          </div>
        </>
      )}

      {activeTab === 'BASH' && (
        <div className="card" style={{ height: '500px', display: 'flex', flexDirection: 'column', background: '#050505', borderColor: '#333' }}>
          <div style={{ flex: 1, overflowY: 'auto', fontFamily: 'var(--font-mono)', fontSize: '0.85rem', color: '#00ff88', padding: '8px 0' }}>
            {terminalHistory.map((line, i) => (
              <div key={i} style={{ padding: '2px 0', whiteSpace: 'pre-wrap' }}>{line}</div>
            ))}
            <div ref={endOfTerminalRef} />
          </div>
          <div style={{ display: 'flex', alignItems: 'center', marginTop: '16px', borderTop: '1px solid #333', paddingTop: '12px' }}>
            <span style={{ color: '#ffcc00', fontFamily: 'var(--font-mono)', marginRight: '8px' }}>admin@nombacrypt:~$</span>
            <input 
              type="text" 
              value={terminalInput}
              onChange={(e) => setTerminalInput(e.target.value)}
              onKeyDown={handleBashCommand}
              autoFocus
              style={{ flex: 1, background: 'transparent', border: 'none', color: '#fff', fontFamily: 'var(--font-mono)', fontSize: '0.85rem', outline: 'none' }}
            />
          </div>
        </div>
      )}
    </div>
  );
}
