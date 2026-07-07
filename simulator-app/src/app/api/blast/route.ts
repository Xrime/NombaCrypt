import { NextResponse } from 'next/server';
import crypto from 'crypto';

const ENGINE_URL = process.env.ENGINE_URL || 'http://127.0.0.1:8080';
const PRIVATE_KEY = 'nomba_hackathon_super_secret_key_2024';

export async function POST(request: Request) {
  try {
    const { batchSize, isAttack } = await request.json();
    const count = Math.min(batchSize || 1, 500); // Cap at 500 per batch

    const reqs = [];
    for (let i = 0; i < count; i++) {
      const payloadObj = {
        amount: Math.floor(Math.random() * 50000) + 1000,
        account_id: 'ACCT_SIM',
        tx_ref: `SIM_${Date.now()}_${i}`,
        timestamp: Date.now()
      };
      const payloadStr = JSON.stringify(payloadObj);
      
      let signature = 'simulated_signature_123';
      if (!isAttack) {
        signature = crypto.createHmac('sha256', PRIVATE_KEY).update(payloadStr).digest('hex');
      }

      reqs.push(
        fetch(`${ENGINE_URL}/api/accounts`, {
          method: 'POST',
          headers: { 
            'Content-Type': 'application/json',
            'X-Signature': signature
          },
          body: payloadStr
        }).catch(err => ({ error: String(err) }))
      );
    }

    const results = await Promise.all(reqs);
    const errors = results.filter((r: any) => r.error);

    if (errors.length === count) {
      return NextResponse.json({ 
        success: false, 
        error: 'All requests failed. Is the C++ engine running on port 8080?' 
      }, { status: 502 });
    }

    return NextResponse.json({ success: true, count, errors: errors.length });
  } catch (error) {
    return NextResponse.json({ 
      success: false, 
      error: 'Simulation failed' 
    }, { status: 500 });
  }
}
