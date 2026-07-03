export default function ForwarderPage() {
  return (
    <div className="animate-in">
      <div className="page-header">
        <h2>Downstream Webhook Forwarder</h2>
        <p>Instant sync to Zoho, QuickBooks, and Zapier.</p>
      </div>
      <div className="card">
        <p className="text-muted">Loading webhook targets...</p>
      </div>
    </div>
  );
}
