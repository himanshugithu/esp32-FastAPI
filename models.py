from database import Base
from sqlalchemy import Column,  Integer,  Float, DateTime
from datetime import datetime

class DHT11Data(Base):
    __tablename__ = "dht11_data"

    id = Column(Integer, primary_key=True, index=True)
    temperature = Column(Float, nullable=False)
    humidity = Column(Float, nullable=False)
    timestamp = Column(DateTime, default=datetime.utcnow)
