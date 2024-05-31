from sqlalchemy import Column, ForeignKey, Integer, String, Float, DateTime, CheckConstraint, Text, Boolean, DECIMAL as Decimal
from sqlalchemy.orm import relationship
from .database import Base, engine
from sqlalchemy import MetaData

# initialize metadata
metadata = MetaData()

# bind metadata to engine
metadata.bind = engine

# drop all tables in the database
metadata.drop_all(bind=engine)

class Card(Base):
    __tablename__ = 'cards'
    __table_args__ = {'extend_existing': True} 
    code = Column(String(9), primary_key=True)
    balance = Column(Decimal(19,2), nullable=False)
    create_time = Column(DateTime, nullable=False)
    rides = relationship('CardRide', backref='card')
    # __table_args__ = (CheckConstraint("code ~ '^[0-9]{9}$'"),)

class Passenger(Base):
    __tablename__ = 'passengers'
    __table_args__ = {'extend_existing': True} 
    id = Column(String(18), primary_key=True)
    name = Column(String(255), nullable=False)
    phone_number = Column(String(11), nullable=False)
    gender = Column(String(255), nullable=False)
    district = Column(String(255), nullable=False)
    rides = relationship('PassengerRide', backref='passenger')
    # on_board = Column(Boolean, default=False)
    # __table_args__ = (CheckConstraint("id_number ~ '^[0-9]{17}[0-9X]?$'"),)


class Station(Base):
    __tablename__ = 'stations'
    __table_args__ = {'extend_existing': True} 
    id = Column(Integer, primary_key=True)
    name = Column(String(255))
    district = Column(String(255))
    intro = Column(String(1023))
    chinese_name = Column(String(255))
    status = Column(String(255), default="Operational")
    lines = relationship('Line', secondary='line_details', backref='stations')

class Line(Base):
    __tablename__ = 'lines'
    __table_args__ = {'extend_existing': True} 
    id = Column(Integer, primary_key=True)
    name = Column(String(255))
    start_time = Column(DateTime)
    end_time = Column(DateTime)
    mileage = Column(Float)
    color = Column(String(255))
    first_opening = Column(DateTime)
    url = Column(String(255))
    intro = Column(String(2047))

class LineDetail(Base):
    __tablename__ = 'line_details'
    __table_args__ = {'extend_existing': True} 
    line_id = Column(Integer, ForeignKey('lines.id'), primary_key=True)
    station_id = Column(Integer, ForeignKey('stations.id'), primary_key=True)
    station_order = Column(Integer)
    

    

class RideBase(Base):
    __abstract__ = True
    start_station_id = Column(Integer, ForeignKey('stations.id'))
    end_station_id = Column(Integer, ForeignKey('stations.id'), nullable=True)
    price = Column(Decimal(4,2), nullable=True)
    start_time = Column(DateTime, primary_key=True)
    end_time = Column(DateTime, nullable=True)

class PassengerRide(RideBase):
    __tablename__ = 'passenger_rides'
    __table_args__ = {'extend_existing': True} 
    id = Column(String(18), ForeignKey('passengers.id'), primary_key=True)

class CardRide(RideBase):
    __tablename__ = 'card_rides'
    __table_args__ = {'extend_existing': True} 
    code = Column(String(9), ForeignKey('cards.code'), primary_key=True)

class BusStation(Base):
    __tablename__ = 'bus_stations'
    __table_args__ = {'extend_existing': True} 
    id = Column(Integer, primary_key=True)
    name = Column(String(255))
    district = Column(String(255))

class BusLine(Base):
    __tablename__ = 'bus_lines'
    __table_args__ = {'extend_existing': True} 
    name = Column(String(255), primary_key=True)

class BusLineDetail(Base):
    __tablename__ = 'bus_line_details'
    __table_args__ = {'extend_existing': True} 
    bus_line_name = Column(String(255), ForeignKey('bus_lines.name'), primary_key=True)
    bus_station_name = Column(Integer, ForeignKey('bus_stations.id'), primary_key=True)

# class Exit(Base):
#     __tablename__ = 'exits'
#     station_name = Column(Integer, ForeignKey('stations.id'), primary_key=True)
#     name = Column(String(255), primary_key=True)
#     textt = Column(Text)
    
class Price(Base):
    __tablename__ = 'prices'
    __table_args__ = {'extend_existing': True} 
    station1_id = Column(Integer, ForeignKey('stations.id'), primary_key=True)
    station2_id = Column(Integer, ForeignKey('stations.id'), primary_key=True)
    price = Column(Decimal(19,2), nullable=False)