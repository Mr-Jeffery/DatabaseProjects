CREATE TABLE IF NOT EXISTS cards (code VARCHAR(255) PRIMARY KEY, money DOUBLE PRECISION, create_time TIMESTAMP);

CREATE TABLE IF NOT EXISTS passengers (name VARCHAR(255), id_number VARCHAR(255) PRIMARY KEY, phone_number, gender VARCHAR(255), district VARCHAR(255));

CREATE TABLE IF NOT EXISTS rides (rail_user VARCHAR(255), start_station VARCHAR(255), end_station VARCHAR(255), price DOUBLE PRECISION, start_time TIMESTAMP, end_time TIMESTAMP);

CREATE TABLE IF NOT EXISTS lines (line_id VARCHAR(255), stations VARCHAR[], start_time TIME, end_time TIME, intro TEXT, mileage DOUBLE PRECISION, color VARCHAR(255), first_opening DATE, url VARCHAR(255));