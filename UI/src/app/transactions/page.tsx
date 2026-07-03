export default function TransactionsPage() {
  return (
    <div className="animate-in">
      <div className="page-header">
        <h2>Transaction Ledger</h2>
        <p>Immutable record of all processed payloads.</p>
      </div>
      <div className="card">
        <p className="text-muted">Loading transactions...</p>
      </div>
    </div>
  );
}
