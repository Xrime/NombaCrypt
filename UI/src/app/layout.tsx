import './globals.css';
import { Inter, JetBrains_Mono } from 'next/font/google';
import Link from 'next/link';

const inter = Inter({ subsets: ['latin'], variable: '--font-inter' });
const jetbrainsMono = JetBrains_Mono({ subsets: ['latin'], variable: '--font-jetbrains-mono' });

export const metadata = {
  title: 'NombaCrypt Shell | Enterprise Payment Infrastructure',
  description: 'High-throughput backend middleware for Nomba API',
};

const NAV_ITEMS = [
  { label: 'Dashboard', href: '/' },
  { label: 'Transactions', href: '/transactions' },
  { label: 'Channels', href: '/channels' },
  { label: 'Accounts', href: '/accounts' },
  { label: 'Billing', href: '/billing' },
  { label: 'Reconciler', href: '/reconciler' },
  { label: 'Security', href: '/security' },
  { label: 'Forwarder', href: '/forwarder' },
];

export default function RootLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  return (
    <html lang="en" className={`${inter.variable} ${jetbrainsMono.variable}`}>
      <body>
        <div className="app-layout">
          <aside className="sidebar">
            <div className="sidebar-brand">
              <h1>
                <div className="brand-icon">N</div>
                NombaCrypt
              </h1>
              <div className="brand-sub">Shell Control Plane</div>
            </div>
            <ul className="sidebar-nav">
              {NAV_ITEMS.map((item) => (
                <li key={item.label}>
                  <Link href={item.href} className="nav-link">
                    {item.label}
                  </Link>
                </li>
              ))}
            </ul>
            <div className="sidebar-footer">
              <button className="kill-switch">System Kill Switch</button>
            </div>
          </aside>
          
          <main className="main-content">
            <header className="topbar">
              <div className="topbar-status">
                <span className="status-dot"></span>
                System Online | C++ Data Plane Connected
              </div>
              <div className="text-muted text-mono" style={{ fontSize: '0.8rem' }}>
                v1.0.0-rc.1
              </div>
            </header>
            {children}
          </main>
        </div>
      </body>
    </html>
  );
}
