from sqlalchemy import create_engine
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy import create_engine, Table, MetaData
from sqlalchemy.orm import sessionmaker
from dotenv import load_dotenv
import os
load_dotenv()  # take environment variables from .env.

db_name = os.getenv('DB_NAME')
db_user = os.getenv('DB_USER')
db_password = os.getenv('DB_PASSWORD')
db_host = os.getenv('DB_HOST')
db_port = os.getenv('DB_PORT')
root_user = os.getenv('ROOT_USER')
root_password = os.getenv('ROOT_PASSWORD')

SQLALCHEMY_DATABASE_URL = f"mysql+mysqlconnector://{db_user}:{db_password}@{db_host}:{db_port}/{db_name}"
engine = create_engine(
    SQLALCHEMY_DATABASE_URL
)
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)

Base = declarative_base()

# Define the engine (replace with your actual MySQL connection string)
root_engine = create_engine(f"mysql+mysqlconnector://{root_user}:{root_password}@{db_host}:{db_port}/mysql")

# Define a session
RootSession = sessionmaker(bind=engine)
session = RootSession()

# Define the metadata
metadata = MetaData()

# Define the user table
user_table = Table('user', metadata, autoload_with=root_engine)