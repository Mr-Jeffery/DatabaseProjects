import os
import psycopg
import json
import random
from datetime import timedelta
from faker import Faker
from dotenv import load_dotenv
fake = Faker()
load_dotenv()  # take environment variables from .env.

db_name = os.getenv('DB_NAME')
db_user = os.getenv('DB_USER')
db_password = os.getenv('DB_PASSWORD')
db_host = os.getenv('DB_HOST')
db_port = os.getenv('DB_PORT')

# connect to database
conn = psycopg.connect(dbname=db_name, user=db_user, password=db_password, host=db_host, port=db_port)
cur = conn.cursor()

# Fetch users
cur.execute("SELECT id_number FROM passengers;")
passengers = [row[0] for row in cur.fetchall()]

# Fetch cards
cur.execute("SELECT code FROM cards;")
cards = [row[0] for row in cur.fetchall()]

# Fetch stations
cur.execute("SELECT name FROM stations;")
stations = [row[0] for row in cur.fetchall()]

data = []

for _ in range(1000000):  # Generate N cases
    start_station = random.choice(stations)
    end_station = random.choice(stations)
    while end_station == start_station:  # Ensure end_station is different from start_station
        end_station = random.choice(stations)

    # Randomly select a passenger ID or card code
    if random.choice([True, False]):
        user = random.choice(passengers)
    else:
        user = random.choice(cards)

    # Generate start_time
    start_time = fake.date_time_between(start_date='now', end_date='+1y')

    # Generate a random duration between 0 and 4 hours
    duration = timedelta(hours=random.uniform(0, 4))

    # Calculate end_time
    end_time = start_time + duration

    # Convert to provided format
    start_time = start_time.strftime('%Y-%m-%d %H:%M:%S')
    end_time = end_time.strftime('%Y-%m-%d %H:%M:%S')

    ride = {
        "user": user,
        "start_station": start_station,
        "end_station": end_station,
        "price": random.randint(2, 15),
        "start_time": start_time,
        "end_time": end_time
    }

    data.append(ride)

# Write JSON data
with open('fakerides.json', 'w') as f:
    json.dump(data, f, indent=2, ensure_ascii=False)

# Close the cursor and connection
cur.close()
conn.close()