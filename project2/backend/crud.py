from sqlalchemy.orm import Session
from . import models, schemas

def get_station(db: Session, station_id: int):
    return db.query(models.Station).filter(models.Station.id == station_id).first()

def get_stations_by_name(db: Session, station_name: str):
    # if contains station_name in the station_name column
    return db.query(models.Station).filter(station_name in models.Station.name).all()

def get_stations(db: Session, skip: int = 0, limit: int = 100):
    return db.query(models.Station).offset(skip).limit(limit).all()

# name: Optional[str] = Field(None, max_length=255)
# district: Optional[str] = Field(None, max_length=255)
# intro: Optional[str] = Field(None, max_length=1023)
# chinese_name: Optional[str] = Field(None, max_length=255)
def create_station(db: Session, station: schemas.StationCreate):
    db_station = models.Station(name=station.name, district=station.district, intro=station.intro, chinese_name=station.chinese_name)
    db.add(db_station)
    db.commit()
    db.refresh(db_station)
    return db_station

def update_station(db: Session, station_id: int, station: schemas.StationUpdate):
    db_station = db.query(models.Station).filter(models.Station.id == station_id).first()
    db_station.name = station.name
    db_station.latitude = station.latitude
    db_station.longitude = station.longitude
    db.commit()
    db.refresh(db_station)
    return db_station

def delete_station(db: Session, station_id: int):
    db_station = db.query(models.Station).filter(models.Station.id == station_id).first()
    db.delete(db_station)
    for line in db.query(models.Line).all():
        if db_station in line.stations:
            line.stations.remove(db_station)
    db.commit()
    return db_station

def get_station_by_name(db: Session, station_name: str):
    return db.query(models.Station).filter(models.Station.name.ilike(f"%{station_name}%")).first()



def get_line(db: Session, line_id: int):
    return db.query(models.Line).filter(models.Line.id == line_id).first()

def get_line_by_name(db: Session, line_name: str):
    return db.query(models.Line).filter(models.Line.name.ilike(f"%{line_name}%")).first()

def get_lines_by_name(db: Session, line_name: str):
    return db.query(models.Line).filter(models.Line.name.ilike(f"%{line_name}%")).all()

def get_lines(db: Session, skip: int = 0, limit: int = 100):
    return db.query(models.Line).offset(skip).limit(limit).all()

def create_line(db: Session, line: schemas.LineCreate):
    # Make sure the arguments match the Line model's constructor
    db_line = models.Line(name=line.name, color=line.color, start_time=line.start_time, end_time=line.end_time, mileage=None, first_opening=None, url=None, intro=None)
    db.add(db_line)
    db.commit()

def update_line(db: Session, line_id: int, line: schemas.LineUpdate):
    db_line = db.query(models.Line).filter(models.Line.id == line_id).first()
    db_line.name = line.name
    db_line.color = line.color
    db.commit()
    db.refresh(db_line)
    return db_line

def delete_line(db: Session, line_id: int):
    db_line = db.query(models.Line).filter(models.Line.id == line_id).first()
    db.delete(db_line)
    db.commit()
    return db_line

def add_station_to_line(db: Session, line_id: int, station_id: int):
    db_line = db.query(models.Line).filter(models.Line.id == line_id).first()
    if any(station.id == station_id for station in db_line.stations):
        raise ValueError(f"Station with id {station_id} is already in line with id {line_id}")
    db_station = db.query(models.Station).filter(models.Station.id == station_id).first()
    db_line.stations.append(db_station)
    db.commit()

def remove_station_from_line(db: Session, line_id: int, station_id: int):
    db_line = db.query(models.Line).filter(models.Line.id == line_id).first()
    db_station = db.query(models.Station).filter(models.Station.id == station_id).first()
    db_line.stations.remove(db_station)
    db.commit()

def get_nth_station_ahead(db: Session, line_id: int, station_id: int, n: int):
    db_line = db.query(models.Line).filter(models.Line.id == line_id).first()
    station_ids = [station.id for station in db_line.stations]
    try:
        index = station_ids.index(station_id)
    except ValueError:
        raise ValueError(f"Station with id {station_id} not found in line with id {line_id}")
    if index - n >= 0:
        return db_line.stations[index + n]
    else:
        return None

def get_nth_station_behind(db: Session, line_id: int, station_id: int, n: int):
    db_line = db.query(models.Line).filter(models.Line.id == line_id).first()
    station_ids = [station.id for station in db_line.stations]
    try:
        index = station_ids.index(station_id)
    except ValueError:
        raise ValueError(f"Station with id {station_id} not found in line with id {line_id}")
    if index - n >= 0:
        return db_line.stations[index - n]
    else:
        return None


def create_passenger(db: Session, passenger: schemas.PassengerCreate):
    db_passenger = models.Passenger(**passenger.dict())
    db.add(db_passenger)
    db.commit()
    db.refresh(db_passenger)
    return db_passenger

def get_passenger(db: Session, passenger_id: int):
    return db.query(models.Passenger).filter(models.Passenger.id == passenger_id).first()

def get_passengers(db: Session, skip: int = 0, limit: int = 100):
    return db.query(models.Passenger).offset(skip).limit(limit).all()

def update_passenger(db: Session, passenger_id: int, passenger: schemas.PassengerUpdate):
    db_passenger = db.query(models.Passenger).filter(models.Passenger.id == passenger_id).first()
    if db_passenger:
        for key, value in passenger.dict().items():
            setattr(db_passenger, key, value)
        db.commit()
        db.refresh(db_passenger)
    return db_passenger

def delete_passenger(db: Session, passenger_id: int):
    db_passenger = db.query(models.Passenger).filter(models.Passenger.id == passenger_id).first()
    if db_passenger:
        db.delete(db_passenger)
        db.commit()
    return db_passenger


# Existing CRUD operations for stations and lines (unchanged)

# View APIs
def get_all_stations(db: Session):
    return db.query(models.Station).all()

def get_all_lines(db: Session):
    return db.query(models.Line).all()

def get_all_passengers(db: Session):
    return db.query(models.Passenger).all()
# import re

# def is_valid_id(id_number):
#     # This regular expression matches a string of 17 digits followed by a digit or 'X'
#     pattern = r'^[0-9]{17}[0-9X]?$'
#     return re.match(pattern, id_number) is not None

# def create_user(db: Session, user: schemas.UserCreate):
#     if not is_valid_id(user.id_number):
#         raise ValueError("Invalid ID number")
#     db_user = models.User(id_number=user.id_number, ...)
#     db.add(db_user)
#     db.commit()
#     db.refresh(db_user)
#     return db_user