import { NextResponse } from 'next/server';

export async function POST(req: Request) {
  try {
    const { requestCount } = await req.json();
    const count = requestCount || 5000;
    
    // Fallback to localhost if env var is missing
    const engineUrl = process.env.NEXT_PUBLIC_ENGINE_URL || 'http://localhost:8080';
    const targetUrl = `${engineUrl}/api/accounts`;

    console.log(`Blasting ${count} requests to ${targetUrl}...`);

    let successCount = 0;
    let failCount = 0;

    // To prevent Node.js from running out of memory/sockets, we blast in batches
    const batchSize = 500;
    const batches = Math.ceil(count / batchSize);

    for (let i = 0; i < batches; i++) {
      const currentBatchSize = Math.min(batchSize, count - i * batchSize);
      
      const promises = Array.from({ length: currentBatchSize }).map(async () => {
        try {
          const res = await fetch(targetUrl, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ amount: Math.floor(Math.random() * 5000) + 100 }),
            // Very short timeout to simulate load failures on legacy
            signal: AbortSignal.timeout(2000) 
          });

          if (res.ok || res.status === 202) {
            successCount++;
          } else {
            failCount++;
          }
        } catch (err) {
          failCount++;
        }
      });

      await Promise.all(promises);
    }

    console.log(`Blast complete: ${successCount} success, ${failCount} failed.`);

    return NextResponse.json({
      success: true,
      successCount,
      failCount
    });

  } catch (error) {
    console.error("Blast error:", error);
    return NextResponse.json({ success: false, error: "Blast failed" }, { status: 500 });
  }
}
