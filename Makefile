# ═══════════════════════════════════════════════════════════════
# NombaCrypt Shell — Top-Level Build Orchestrator
# ═══════════════════════════════════════════════════════════════

.PHONY: all engine api dashboard setup clean test

# ── Full Stack ──
all: engine api dashboard

# ── C++ Engine ──
engine:
	@echo "🔧 Building C++ Data Plane Engine..."
	cd backend-engine && mkdir -p build && cd build && cmake .. && make -j$(nproc)

engine-run: engine
	@echo "🚀 Starting C++ Engine on port 8080..."
	./backend-engine/build/NombaCrypt_Shell

# ── Python Control API ──
api:
	@echo "🐍 Setting up Python Control API..."
	cd control-api && pip install -r requirement.txt

api-run:
	@echo "🚀 Starting Python API on port 5000..."
	cd control-api && python run.py

# ── React Dashboard ──
dashboard:
	@echo "⚛️  Installing Dashboard dependencies..."
	cd UI && npm install

dashboard-run:
	@echo "🚀 Starting Dashboard on port 3000..."
	cd UI && npm run dev

# ── Setup (First Time) ──
setup:
	@echo "📦 Setting up development environment..."
	cp -n .env.example .env || true
	$(MAKE) engine
	$(MAKE) api
	$(MAKE) dashboard
	cd control-api && python scripts/seed_toggles.py
	cd control-api && python scripts/seed_test_users.py
	@echo "✅ Setup complete! Run 'make start' to launch all services."

# ── Start All Services ──
start:
	@echo "🚀 Starting NombaCrypt Shell (all layers)..."
	docker-compose up --build

# ── Tests ──
test: test-engine test-api test-dashboard

test-engine:
	@echo "🧪 Running C++ unit tests..."
	cd backend-engine/build && ctest --output-on-failure

test-api:
	@echo "🧪 Running Python API tests..."
	cd control-api && pytest tests/ -v

test-dashboard:
	@echo "🧪 Running Frontend tests..."
	cd UI && npm test

# ── Clean ──
clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -rf backend-engine/build
	rm -rf backend-engine/cmake-build-*
	rm -rf control-api/__pycache__
	rm -rf control-api/**/__pycache__
	rm -rf UI/node_modules
	rm -rf UI/.next
