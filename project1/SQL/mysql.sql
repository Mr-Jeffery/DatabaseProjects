CREATE TABLE IF NOT EXISTS cards( 
        code CHAR(9) PRIMARY KEY CHECK (code REGEXP '^[0-9]{9}$'),
        money DECIMAL(19,4) NOT NULL, 
        create_time TIMESTAMP NOT NULL);
CREATE TABLE IF NOT EXISTS passengers ( 
        name CHAR(255) NOT NULL,
        id_number CHAR(18) PRIMARY KEY NOT NULL CHECK (id_number REGEXP '^[0-9]{17}[0-9X]?$'),
        phone_number CHAR(11) NOT NULL CHECK (phone_number REGEXP '^[0-9]{11}$'), 
        gender CHAR(255) NOT NULL, 
        district CHAR(255) NOT NULL );
CREATE TABLE IF NOT EXISTS `lines` ( 
        name CHAR(255)PRIMARY KEY,
        start_time TIME, 
        end_time TIME, 
        mileage DOUBLE PRECISION, 
        color CHAR(255), 
        first_opening DATE, 
        url TEXT,
        intro TEXT);
CREATE TABLE IF NOT EXISTS stations (
        name CHAR(255) PRIMARY KEY, 
        district CHAR(255), 
        intro TEXT, 
        chinese_name CHAR(255));
CREATE TABLE IF NOT EXISTS rides (
        rail_user CHAR(255), 
        start_station CHAR(255), 
        end_station CHAR(255), 
        price DOUBLE PRECISION, 
        start_time TIMESTAMP, 
        end_time TIMESTAMP);
CREATE TABLE IF NOT EXISTS line_details ( 
        line_name CHAR(255), 
        station_name CHAR(255), 
        FOREIGN KEY (line_name) REFERENCES `lines`(name), 
        FOREIGN KEY (station_name) REFERENCES stations(name));
CREATE TABLE IF NOT EXISTS bus_stations (name CHAR(255) PRIMARY KEY, district CHAR(255));
CREATE TABLE IF NOT EXISTS bus_lines (name CHAR(255) PRIMARY KEY);
CREATE TABLE IF NOT EXISTS bus_line_details (
        bus_line_name CHAR(255), 
        bus_station_name CHAR(255), 
        FOREIGN KEY (bus_station_name) REFERENCES bus_stations(name));
CREATE TABLE IF NOT EXISTS exits (
        station_name CHAR(255), 
        name CHAR(255),
        textt TEXT,
        FOREIGN KEY (station_name) REFERENCES stations(name),
        PRIMARY KEY (station_name, name));
CREATE TABLE IF NOT EXISTS exit_details (
        station_name CHAR(255), 
        name CHAR(255), 
        bus_station_name CHAR(255), 
        FOREIGN KEY (station_name, name) REFERENCES exits(station_name,name), 
        FOREIGN KEY (bus_station_name) REFERENCES bus_stations(name) , 
        PRIMARY KEY (station_name, name, bus_station_name));
LOAD DATA LOCAL INFILE '/tmp/cards.csv' INTO TABLE cards  FIELDS TERMINATED BY ',' ENCLOSED BY '"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
LOAD DATA LOCAL INFILE '/tmp/passengers.csv' INTO TABLE passengers  FIELDS TERMINATED BY ',' ENCLOSED BY '"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
LOAD DATA LOCAL INFILE '/tmp/lines.csv' INTO TABLE `lines`  FIELDS TERMINATED BY ',' ENCLOSED BY '"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
LOAD DATA LOCAL INFILE '/tmp/stations.csv' INTO TABLE stations  FIELDS TERMINATED BY ',' ENCLOSED BY '"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
LOAD DATA LOCAL INFILE '/tmp/rides.csv' INTO TABLE rides  FIELDS TERMINATED BY ',' ENCLOSED BY '"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
LOAD DATA LOCAL INFILE '/tmp/line_details.csv' INTO TABLE line_details  FIELDS TERMINATED BY ',' ENCLOSED BY '"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
LOAD DATA LOCAL INFILE '/tmp/exits.csv' INTO TABLE exits  FIELDS TERMINATED BY ',' ENCLOSED BY '"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
CREATE TEMPORARY TABLE tmp_bus_stations AS SELECT * FROM bus_stations LIMIT 0;
LOAD DATA LOCAL INFILE '/tmp/bus_stations.csv' INTO TABLE tmp_bus_stations FIELDS TERMINATED BY ',' ENCLOSED BY '"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
INSERT IGNORE INTO bus_stations SELECT * FROM tmp_bus_stations;
CREATE TEMPORARY TABLE tmp_bus_line_details AS SELECT * FROM bus_line_details LIMIT 0;
LOAD DATA LOCAL INFILE '/tmp/bus_line_details.csv' INTO TABLE tmp_bus_line_details FIELDS TERMINATED BY ',' ENCLOSED BY '"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
INSERT IGNORE INTO bus_line_details SELECT DISTINCT * FROM tmp_bus_line_details;
CREATE TEMPORARY TABLE tmp_exit_details AS SELECT * FROM exit_details LIMIT 0;
LOAD DATA LOCAL INFILE '/tmp/exit_details.csv' INTO TABLE tmp_exit_details FIELDS TERMINATED BY ',' ENCLOSED BY '"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
INSERT IGNORE INTO exits (SELECT station_name, name, bus_station_name AS textt FROM tmp_exit_details);
INSERT IGNORE INTO exit_details (SELECT * FROM tmp_exit_details);
