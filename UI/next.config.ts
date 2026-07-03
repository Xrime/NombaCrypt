import type { NextConfig } from 'next';

/**
 * Next.js configuration for NombaCrypt Shell UI.
 *
 * - reactStrictMode enabled for development safety
 * - API rewrites proxy /api/* to the Python backend
 */
const nextConfig: NextConfig = {
  reactStrictMode: true,
  /** Allow images from the backend origin */
  images: {
    remotePatterns: [
      { protocol: 'http', hostname: 'localhost', port: '8000' },
    ],
  },
  /** Proxy API requests to the Python backend during dev */
  async rewrites() {
    return [
      {
        source: '/api/:path*',
        destination: 'http://localhost:8000/api/:path*',
      },
    ];
  },
};

export default nextConfig;
