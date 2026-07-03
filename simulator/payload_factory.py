"""
NombaCrypt Shell — Payload Factory

Generates realistic dummy JSON payloads for traffic simulation.
Mimics real Nomba checkout request structures.
"""
import uuid
import random
import time
from typing import Optional


class PayloadFactory:
    """
    Generates realistic transaction payloads for stress testing.
    
    Payloads mimic the structure of real Nomba Checkout API requests
    with randomized amounts, student IDs, and course codes.
    """

    # Sample data for realistic payload generation
    STUDENT_NAMES = [
        "Adebayo Ogunlesi", "Chidinma Nwosu", "Emeka Okafor",
        "Fatima Abdullahi", "Grace Ibe", "Hassan Musa",
        "Ifeoma Chukwu", "Jide Akintola", "Kemi Balogun",
        "Lukman Abubakar", "Maryam Suleiman", "Nnamdi Eze",
        "Oluwaseun Adeyemi", "Patience Okoro", "Rasheed Yusuf",
    ]

    COURSE_CODES = [
        "CSC301", "EEE201", "MEE401", "CHE102", "PHY301",
        "MTH201", "BIO101", "ACC301", "ECN201", "LAW401",
    ]

    FEE_TYPES = [
        "tuition", "exam_fee", "lab_fee", "registration",
        "accommodation", "library_access", "transcript",
    ]

    def generate(self, request_id: int, template: Optional[dict] = None) -> dict:
        """
        Generate a single transaction payload.
        
        Args:
            request_id: Sequential request identifier.
            template: Optional custom template to override defaults.
            
        Returns:
            Dictionary payload ready for JSON serialization.
        """
        if template:
            payload = template.copy()
            payload["transaction_id"] = str(uuid.uuid4())
            payload["request_sequence"] = request_id
            return payload

        student_name = random.choice(self.STUDENT_NAMES)
        course_code = random.choice(self.COURSE_CODES)
        fee_type = random.choice(self.FEE_TYPES)

        # Realistic fee amounts in Nigerian Naira
        amount = round(random.uniform(5000.00, 150000.00), 2)

        return {
            "transaction_id": str(uuid.uuid4()),
            "request_sequence": request_id,
            "amount": amount,
            "currency": "NGN",
            "customer": {
                "name": student_name,
                "email": f"{student_name.split()[0].lower()}@students.ui.edu.ng",
                "student_id": f"UI/{random.randint(2020, 2026)}/{random.randint(10000, 99999)}",
            },
            "metadata": {
                "fee_type": fee_type,
                "course_code": course_code,
                "semester": random.choice(["first", "second"]),
                "academic_year": "2025/2026",
            },
            "callback_url": "https://portal.ui.edu.ng/api/payment/callback",
            "timestamp": time.time(),
        }

    def generate_batch(self, count: int) -> list[dict]:
        """Generate a batch of payloads."""
        return [self.generate(i) for i in range(count)]
