from pydantic import BaseModel
class dht11(BaseModel):
    temperature:float
    humidity:float

