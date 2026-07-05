import asyncio
import aiohttp
import time

URL = "http://localhost:8080/api/accounts"
TOTAL_REQUESTS = 1000000
CONCURRENCY_LIMIT = 2000

async def send_request(session, semaphore):
    payload = {
        "accountName": "Simulated User",
        "bvn": "12345678901"
    }
    async with semaphore:
        try:
            async with session.post(URL, json=payload) as response:
                await response.read()
                return response.status
        except Exception:
            return 0

async def blast_traffic():
    print(f"🚀 Preparing to blast {TOTAL_REQUESTS} requests into the C++ Engine...")

    semaphore = asyncio.Semaphore(CONCURRENCY_LIMIT)
    connector = aiohttp.TCPConnector(limit=CONCURRENCY_LIMIT)

    async with aiohttp.ClientSession(connector=connector) as session:
        print("🔥 FIRE! Flooding the port...")
        start_time = time.time()

        # Lowered to 5,000 so it prints progress updates instantly!
        chunk_size = 5000
        total_accepted = 0
        total_rejected = 0

        for i in range(0, TOTAL_REQUESTS, chunk_size):
            tasks = []
            for _ in range(chunk_size):
                tasks.append(asyncio.create_task(send_request(session, semaphore)))

            results = await asyncio.gather(*tasks)

            total_accepted += results.count(202)
            total_rejected += results.count(429)

            print(f"Sent {i + chunk_size:,} requests... (Accepted: {total_accepted:,} | Full: {total_rejected:,})")

        end_time = time.time()
        duration = end_time - start_time

        print("\n===========================================")
        print(" 🛑 BOMBARDMENT COMPLETE")
        print(f" Time Taken: {duration:.2f} seconds")
        print(f" Python Network Throughput: {TOTAL_REQUESTS / duration:,.0f} requests/sec")
        print(f" Successfully Ingested by C++: {total_accepted:,}")
        print(f" Dropped by C++ (Buffer Full): {total_rejected:,}")
        print("===========================================")

if __name__ == "__main__":
    asyncio.run(blast_traffic())