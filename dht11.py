from fastapi import FastAPI,status,Depends,HTTPException
from database import engine,SessionLocal
from sqlalchemy.orm import Session
import schemas,models

app = FastAPI()
models.Base.metadata.create_all(bind = engine)
def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

@app.post('/dht11',status_code=status.HTTP_201_CREATED)
def post_dht11(data:schemas.dht11,db:Session = Depends(get_db)):
    new_data = models.DHT11Data(temperature= data.temperature,humidity = data.humidity)
    db.add(new_data)
    db.commit()
    db.refresh(new_data)
    return new_data

@app.get('/dht11/latest', status_code=200)
def show_latest_blog(db: Session = Depends(get_db)):
    data = db.query(models.DHT11Data).order_by(models.DHT11Data.id.desc()).first()
    if not data:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="No blogs available")
    return {'data': data}
    