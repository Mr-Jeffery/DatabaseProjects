from sqlalchemy import Column, ForeignKey, Integer, String, Float, DateTime, CheckConstraint, Text
from sqlalchemy.orm import relationship
from database import Base, engine
from sqlalchemy import MetaData

# initialize metadata
metadata = MetaData()

# bind metadata to engine
metadata.bind = engine

# drop all tables in the database
metadata.drop_all(bind=engine)

class Card(Base):
    __tablename__ = 'cards'
    code = Column(String(9), primary_key=True)
    money = Column(Float, nullable=False)
    create_time = Column(DateTime, nullable=False)
    rides = relationship('CardRide', backref='card')
    __table_args__ = (CheckConstraint("code ~ '^[0-9]{9}$'"),)

class Passenger(Base):
    __tablename__ = 'passengers'
    name = Column(String(255), nullable=False)
    id = Column(String(18), primary_key=True)
    phone_number = Column(String(11), nullable=False)
    gender = Column(String(255), nullable=False)
    district = Column(String(255), nullable=False)
    rides = relationship('PassengerRide', backref='passenger')
    __table_args__ = (CheckConstraint("id_number ~ '^[0-9]{17}[0-9X]?$'"),)

class Line(Base):
    __tablename__ = 'lines'
    id = Column(Integer, primary_key=True)
    name = Column(String(255))
    start_time = Column(DateTime)
    end_time = Column(DateTime)
    mileage = Column(Float)
    color = Column(String(255))
    first_opening = Column(DateTime)
    url = Column(String)
    intro = Column(String)
    stations = relationship('Station', secondary='line_details', backref='lines')

class Station(Base):
    __tablename__ = 'stations'
    id = Column(Integer, primary_key=True)
    name = Column(String(255))
    district = Column(String(255))
    intro = Column(String)
    chinese_name = Column(String(255))
    lines = relationship('Line', secondary='line_details', backref='stations')

class LineDetail(Base):
    __tablename__ = 'line_details'
    line_name = Column(String(255), ForeignKey('lines.name'), primary_key=True)
    station_name = Column(String(255), ForeignKey('stations.name'), primary_key=True)

class RideBase(Base):
    __abstract__ = True
    start_station = Column(String(255), ForeignKey('stations.name'))
    end_station = Column(String(255), ForeignKey('stations.name'))
    price = Column(Float)
    start_time = Column(DateTime, primary_key=True)
    end_time = Column(DateTime)

class PassengerRide(RideBase):
    __tablename__ = 'passenger_rides'
    passenger_id = Column(String(18), ForeignKey('passengers.id'), primary_key=True)

class CardRide(RideBase):
    __tablename__ = 'card_rides'
    card_code = Column(String(9), ForeignKey('cards.code'), primary_key=True)

class BusStation(Base):
    __tablename__ = 'bus_stations'
    id = Column(Integer, primary_key=True)
    name = Column(String(255))
    district = Column(String(255))

class BusLine(Base):
    __tablename__ = 'bus_lines'
    name = Column(String(255), primary_key=True)

class BusLineDetail(Base):
    __tablename__ = 'bus_line_details'
    bus_line_name = Column(String(255), ForeignKey('bus_lines.name'), primary_key=True)
    bus_station_name = Column(String(255), ForeignKey('bus_stations.name'), primary_key=True)

class Exit(Base):
    __tablename__ = 'exits'
    station_name = Column(String(255), ForeignKey('stations.name'), primary_key=True)
    name = Column(String(255), primary_key=True)
    textt = Column(Text)