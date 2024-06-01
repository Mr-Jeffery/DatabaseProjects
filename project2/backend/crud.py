from sqlalchemy.orm import Session
from sqlalchemy import and_, or_, func
from . import models, schemas

def get_station(db: Session, station_id: int):
    return db.query(models.Station).filter(models.Station.id == station_id).first()

def get_stations_by_name(db: Session, station_name: str):
    # if contains station_name in the station_name column
    return db.query(models.Station).filter(models.Station.name.ilike(f"%{station_name}%")).all()

def get_stations(db: Session, skip: int = 0, limit: int = 100):
    return db.query(models.Station).offset(skip).limit(limit).all()

def get_station_by_id(db: Session, station_id: int):
    return db.query(models.Station).filter(models.Station.id == station_id).first()

def create_station(db: Session, station: schemas.StationCreate):
    db_station = models.Station(name=station.name, district=station.district, intro=station.intro, chinese_name=station.chinese_name, status=station.status)
    db.add(db_station)
    db.commit()
    db.refresh(db_station)
    return db_station

def update_station(db: Session, station_id: int, station: schemas.StationUpdate):
    db_station = db.query(models.Station).filter(models.Station.id == station_id).first()
    db_station.name = station.name
    db_station.district = station.district
    db_station.intro = station.intro
    db_station.chinese_name = station.chinese_name
    db_station.status = station.status
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
    return db.query(models.Station).filter(or_(models.Station.name.ilike(f"%{station_name}%"), models.Station.chinese_name.ilike(f"%{station_name}%"))).first()

def get_card_by_code(db: Session, card_code: int):
    return db.query(models.Card).filter(models.Card.code == card_code).first()
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
    return db_line

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
    # Check if the station is already in the line
    if db.query(models.LineDetail).filter(and_(models.LineDetail.line_id == line_id, models.LineDetail.station_id == station_id)).first():
        raise ValueError(f"Station with id {station_id} is already in line with id {line_id}")

    # Get the current highest order in the line
    max_order = db.query(func.max(models.LineDetail.station_order)).filter(models.LineDetail.line_id == line_id).scalar()
    new_order = max_order + 1 if max_order is not None else 0

    # Add the new station
    new_station = models.LineDetail(line_id=line_id, station_id=station_id, station_order=new_order)
    db.add(new_station)

    db.commit()

def insert_station_to_line(db: Session, line_id: int, station_id: int, order: int):
    db_line = db.query(models.Line).filter(models.Line.id == line_id).first()
    if any(station.id == station_id for station in db_line.stations):
        raise ValueError(f"Station with id {station_id} is already in line with id {line_id}")

    # Increment the order of the subsequent stations
    db.query(models.LineDetail).filter(and_(models.LineDetail.line_id == line_id, models.LineDetail.station_order >= order)).update({"station_order": models.LineDetail.station_order + 1})

    new_station = models.LineDetail(line_id=line_id, station_id=station_id, station_order=order)
    db.add(new_station)

    db.commit()
    

def remove_station_from_line(db: Session, line_id: int, station_id: int):
    db_line = db.query(models.Line).filter(models.Line.id == line_id).first()
    db_station = db.query(models.Station).filter(models.Station.id == station_id).first()
    db_line.stations.remove(db_station)
    db.commit()

def get_nth_station_ahead(db: Session, line_id: int, station_id: int, n: int):
    line_details = db.query(models.LineDetail).filter(models.LineDetail.line_id == line_id).order_by(models.LineDetail.station_order).all()
    station_ids = [detail.station_id for detail in line_details]
    try:
        index = station_ids.index(station_id)
    except ValueError:
        raise ValueError(f"Station with id {station_id} not found in line with id {line_id}")
    if index + n < len(station_ids):
        return db.query(models.Station).filter(models.Station.id == station_ids[index + n]).first()
    else:
        return None

def get_nth_station_behind(db: Session, line_id: int, station_id: int, n: int):
    line_details = db.query(models.LineDetail).filter(models.LineDetail.line_id == line_id).order_by(models.LineDetail.station_order).all()
    station_ids = [detail.station_id for detail in line_details]
    try:
        index = station_ids.index(station_id)
    except ValueError:
        raise ValueError(f"Station with id {station_id} not found in line with id {line_id}")
    if index - n >= 0:
        return db.query(models.Station).filter(models.Station.id == station_ids[index - n]).first()
    else:
        return None


def create_passenger(db: Session, passenger: schemas.PassengerCreate):
    db_passenger = models.Passenger(name=passenger.name, id=passenger.id, phone_number=passenger.phone_number,gender=passenger.gender, district=passenger.district)
    db.add(db_passenger)
    db.commit()
    db.refresh(db_passenger)
    return db_passenger

def create_card(db: Session, card: schemas.CardCreate):
    db_card = models.Card(code=card.code, balance=card.balance, create_time=card.create_time)
    db.add(db_card)
    db.commit()
    db.refresh(db_card)
    return db_card

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

def update_card(db: Session, card_code: int, card: schemas.CardUpdate):
    db_card = db.query(models.Card).filter(models.Card.code == card_code).first()
    if db_card:
        for key, value in card.dict().items():
            setattr(db_card, key, value)
        db.commit()
        db.refresh(db_card)
    return db_card


def delete_passenger(db: Session, passenger_id: int):
    db_passenger = db.query(models.Passenger).filter(models.Passenger.id == passenger_id).first()
    if db_passenger:
        db.delete(db_passenger)
        db.commit()
    return db_passenger

def delete_card(db: Session, card_code: int):
    db_card = db.query(models.Card).filter(models.Card.code == card_code).first()
    if db_card:
        db.delete(db_card)
        db.commit()
    return db_card



# Existing CRUD operations for stations and lines (unchanged)

# View APIs
def get_all_stations(db: Session):
    return db.query(models.Station).all()

def get_all_lines(db: Session):
    return db.query(models.Line).all()

def get_all_passengers(db: Session):
    return db.query(models.Passenger).all()

def get_all_cards(db: Session):
    return  db.query(models.Card).all()


def get_all_line_stations(db: Session, line_id: int):
    return db.query(models.LineDetail.station_id).filter(models.LineDetail.line_id == line_id).order_by(models.LineDetail.station_order).all()


# Boarding Functionality
def board_passenger(db: Session, boarding: schemas.Boarding):
    passenger_ride = models.PassengerRide(id=boarding.passengerOrCardsId, start_station_id=boarding.start_station_id, start_time=boarding.start_time)
    db.add(passenger_ride)
    db.commit()
    return passenger_ride

def board_card(db: Session, boarding: schemas.Boarding):
    card_ride = models.CardRide(code=boarding.passengerOrCardsId, start_station_id=boarding.start_station_id, start_time=boarding.start_time)
    db.add(card_ride)
    db.commit()
    return card_ride

# Exit Functionality
def exit_passenger(db: Session, passenger_id: str, exit_info: schemas.ExitInfo):
    ride = db.query(models.PassengerRide).filter(models.PassengerRide.id == passenger_id, models.PassengerRide.end_time == None).first()
    if ride:
        ride.end_station_id = exit_info.end_station_id
        ride.end_time = exit_info.end_time
        ride.price = calculate_price(db,ride.start_station_id, ride.end_station_id)
        db.commit()
    return ride

def exit_card(db: Session, card_code: str, exit_info: schemas.ExitInfo):
    card = db.query(models.Card).filter(models.Card.code == card_code).first()
    ride = db.query(models.CardRide).filter(models.CardRide.code == card_code, models.CardRide.end_time == None).first()
    if ride and card:
        ride.end_station_id = exit_info.end_station_id
        ride.end_time = exit_info.end_time
        ride.price = calculate_price(db,ride.start_station_id, ride.end_station_id)
        card.balance -= ride.price
        db.commit()
    return ride

# View Current Boardings
def get_current_boardings(db: Session):
    passengers = db.query(models.PassengerRide).filter(models.PassengerRide.end_time == None).all()
    cards = db.query(models.CardRide).filter(models.CardRide.end_time == None).all()
    # print(passengers, cards)
    return {"passengers": passengers, "cards": cards}


def calculate_price(db: Session, start_station_id: int, end_station_id: int):
    db_price = db.query(models.Price).filter(and_(models.Price.station1_id == start_station_id, models.Price.station2_id == end_station_id)).first()
    if db_price:
        return db_price.price
    else:
        return None

