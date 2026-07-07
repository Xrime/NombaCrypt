import { NextResponse } from 'next/server';

const ENGINE_URL = process.env.ENGINE_URL || 'http://127.0.0.1:8080';

export async function GET() {
  try {
    const res = await fetch(`${ENGINE_URL}/api/config`, { cache: 'no-store' });
    const data = await res.json();
    return NextResponse.json(data);
  } catch {
    return NextResponse.json({ error: 'Engine unreachable' }, { status: 502 });
  }
}

export async function POST(request: Request) {
  try {
    const body = await request.text();
    const res = await fetch(`${ENGINE_URL}/api/config`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: body,
    });
    const data = await res.json();
    return NextResponse.json(data);
  } catch {
    return NextResponse.json({ error: 'Engine unreachable' }, { status: 502 });
  }
}
