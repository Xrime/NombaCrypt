import { NextResponse } from 'next/server';

const ENGINE_URL = process.env.ENGINE_URL || 'http://127.0.0.1:8080';

export async function GET() {
  try {
    const res = await fetch(`${ENGINE_URL}/api/telemetry`, { cache: 'no-store' });
    const data = await res.json();
    return NextResponse.json(data);
  } catch {
    return NextResponse.json({ error: 'Engine unreachable' }, { status: 502 });
  }
}
