from pydantic import BaseModel, Field
from typing import List, Optional
from datetime import datetime
from sqlalchemy import Column
class BaseModel(BaseModel):
    created_at: Optional[datetime] = Field(default_factory=datetime.utcnow)
    updated_at: Optional[datetime] = Field(default_factory=datetime.utcnow)

class UserBase(BaseModel):
    username: str

class UserCreate(UserBase):
    password: str

class User(UserBase):
    id: int
    is_active: bool

    class Config:
        orm_mode = True

# Passengers
class PassengerBase(BaseModel):
    name: str
    id: str = Field(..., max_length=18)
    phone_number: str = Field(..., max_length=11)
    gender: str
    district: str

class PassengerCreate(PassengerBase):
    pass

class PassengerUpdate(PassengerBase):
    pass

# schemas.py
class PassengerCreate(BaseModel):
    class Config:
        orm_mode = True

# Stations
class StationBase(BaseModel):
    name: Optional[str] = Field(None, max_length=255)
    district: Optional[str] = Field(None, max_length=255)
    intro: Optional[str] = Field(None, max_length=1023)
    chinese_name: Optional[str] = Field(None, max_length=255)
    status: Optional[str] = Field(None, max_length=255)
class StationCreate(StationBase):
    pass

class StationUpdate(StationBase):
    pass

class Station(StationBase):
    id: int

    class Config:
        orm_mode = True

class LineBase(BaseModel):
    name: Optional[str] = Field(None, max_length=255)
    start_time: Optional[datetime] = Field(None)
    end_time: Optional[datetime] = Field(None)
    mileage: Optional[float] = Field(None)
    color: Optional[str] = Field(None, max_length=255)
    first_opening: Optional[datetime]= Field(None, max_length=255)
    url: Optional[str] = Field(None, max_length=255)
    intro: Optional[str] = Field(None, max_length=2047)
    # status: Optional[str] = Field(None, max_length=255)

class LineCreate(LineBase):
    pass

class LineUpdate(LineBase):
    pass

class Line(LineBase):
    id: int

    class Config:
        orm_mode = True

class RideBase(BaseModel):
    start_station: int
    end_station: int
    price: Optional[float]
    start_time: datetime
    end_time: Optional[datetime]

class CardRideBase(RideBase):
    card_code: str = Field(..., max_length=9)

class CardRideCreate(CardRideBase):
    pass

class CardRideUpdate(CardRideBase):
    pass

class CardRide(CardRideBase):
    class Config:
        orm_mode = True

class PassengerRideBase(RideBase):
    passenger_id: str = Field(..., max_length=18)

class PassengerRideCreate(PassengerRideBase):
    pass

class PassengerRideUpdate(PassengerRideBase):
    pass
        
class Boarding(BaseModel):
    start_station_id: int
    start_time: datetime

class ExitInfo(BaseModel):
    end_station_id: int
    end_time: datetime