DROP TABLE IF EXISTS rides CASCADE;
DROP TABLE IF EXISTS cards CASCADE;
DROP TABLE IF EXISTS passengers CASCADE;
DROP TABLE IF EXISTS line_details CASCADE;
DROP TABLE IF EXISTS lines CASCADE;
DROP TABLE IF EXISTS exits CASCADE;
DROP TABLE IF EXISTS bus_stations CASCADE;
DROP TABLE IF EXISTS stations CASCADE;
DROP TABLE IF EXISTS bus_line_details CASCADE; 
DROP TABLE IF EXISTS bus_lines CASCADE;


CREATE TABLE IF NOT EXISTS cards(code VARCHAR(9) PRIMARY KEY CHECK (code ~ '^[0-9]{9}$'), money MONEY NOT NULL, create_time TIMESTAMP NOT NULL);
CREATE TABLE IF NOT EXISTS passengers (name VARCHAR(255) NOT NULL, id_number VARCHAR(18) PRIMARY KEY NOT NULL CHECK (id_number ~ '^[0-9]{17}[0-9X]?$'), phone_number VARCHAR(11) NOT NULL CHECK (phone_number ~ '^[0-9]{11}$'), gender VARCHAR(255) NOT NULL, district VARCHAR(255) NOT NULL);
CREATE TABLE IF NOT EXISTS rides (rail_user VARCHAR(255), start_station VARCHAR(255), end_station VARCHAR(255), price DOUBLE PRECISION, start_time TIMESTAMP, end_time TIMESTAMP);
CREATE TABLE IF NOT EXISTS lines (name VARCHAR(255)PRIMARY KEY, start_time TIME, end_time TIME, mileage DOUBLE PRECISION, color VARCHAR(255), first_opening DATE, url TEXT,intro TEXT);
CREATE TABLE IF NOT EXISTS line_details (line_name VARCHAR(255), station_name VARCHAR(255));
CREATE TABLE IF NOT EXISTS stations (name VARCHAR(255) PRIMARY KEY, district VARCHAR(255), intro TEXT, chinese_name VARCHAR(255));
CREATE TABLE IF NOT EXISTS bus_stations (name VARCHAR(255) PRIMARY KEY, district VARCHAR(255));
CREATE TABLE IF NOT EXISTS bus_line (name VARCHAR(255) PRIMARY KEY);
CREATE TABLE IF NOT EXISTS bus_line_details (bus_line_name VARCHAR(255), bus_station_name VARCHAR(255), FOREIGN KEY (bus_line_name) REFERENCES bus_line(name), FOREIGN KEY (bus_station_name) REFERENCES bus_stations(name));
CREATE TABLE IF NOT EXISTS exits (station_name VARCHAR(255), name VARCHAR(255),textt TEXT,FOREIGN KEY (station_name) REFERENCES stations(name),PRIMARY KEY (station_name, name));


COPY cards FROM '/tmp/cards.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');
COPY passengers FROM '/tmp/passengers.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');
COPY rides FROM '/tmp/rides.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');
COPY lines FROM '/tmp/lines.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');
COPY line_details FROM '/tmp/line_details.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');
COPY stations FROM '/tmp/stations.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');
COPY exits FROM '/tmp/exits.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');
CREATE TEMP TABLE tmp_bus_stations AS SELECT * FROM bus_stations LIMIT 0;COPY tmp_bus_stations FROM '/tmp/bus_stations.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');INSERT INTO bus_stations SELECT * FROM tmp_bus_stations ON CONFLICT DO NOTHING;
COPY bus_line_details FROM '/tmp/bus_line_details.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');
INSERT INTO bus_lines SELECT DISTINCT bus_line_name FROM bus_line_details;


-- for data cleaning
select distinct bus_line_name
from bus_line_details
where bus_line_name like '%出入口%';

UPDATE bus_line_details
SET bus_line_name = REGEXP_REPLACE(bus_line_name, '（新增）(.*)', '')
WHERE bus_line_name LIKE '%（新增）%';

UPDATE bus_line_details
SET bus_line_name = REGEXP_REPLACE(bus_line_name, '[A-Z]*[0-9]*出入口$', '')
WHERE bus_line_name LIKE '%出入口%';

UPDATE bus_line_details
SET bus_line_name = REGEXP_REPLACE(bus_line_name, '（原(.*)', '')
WHERE bus_line_name LIKE '%（原%';

UPDATE bus_line_details
SET bus_line_name = REGEXP_REPLACE(bus_line_name, '\(原(.*)', '')
WHERE bus_line_name LIKE '%\(原%';
