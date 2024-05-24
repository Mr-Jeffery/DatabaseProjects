source ../setenv.sh
mysql -e "DROP DATABASE IF EXISTS $DB_NAME; CREATE DATABASE $DB_NAME;" -u $DB_USER -h $DB_HOST -P 3306 -p$DB_PASSWORD

time (
mysql --local-infile=1 -e "
        CREATE TABLE IF NOT EXISTS cards( 
                code CHAR(9) PRIMARY KEY,
                money DECIMAL(19,2) NOT NULL, 
                create_time TIMESTAMP NOT NULL);
        CREATE TABLE IF NOT EXISTS passengers (
                id_number CHAR(18) PRIMARY KEY NOT NULL,
                name CHAR(255) NOT NULL,
                phone_number CHAR(11) NOT NULL, 
                gender CHAR(255) NOT NULL, 
                district CHAR(255) NOT NULL );
        CREATE TABLE IF NOT EXISTS \`lines\` ( 
                line_id INT PRIMARY KEY,
                name CHAR(255),
                start_time TIME, 
                end_time TIME, 
                mileage DOUBLE PRECISION, 
                color CHAR(255), 
                first_opening DATE, 
                url TEXT,
                intro TEXT
                );
        CREATE TABLE IF NOT EXISTS stations (
                station_id   integer      not null,
                chinese_name CHAR(255)  not null,
                name         VARCHAR(255) not null,
                district     CHAR(255)  not null,
                intro        text         not null,
                constraint stations_pk
                        primary key (station_id)
                );
        CREATE TABLE IF NOT EXISTS passenger_rides (
                rail_user        VARCHAR(256) not null,
                price            DOUBLE       not null,
                start_time       TIMESTAMP    not null,
                end_time         TIMESTAMP    not null,
                start_station_id INTEGER      not null,
                end_station_id   INTEGER      not null,
                constraint rides_pk
                        primary key (rail_user, start_station_id, start_time)
                );
        CREATE TABLE IF NOT EXISTS card_rides (
                rail_user        VARCHAR(256) not null,
                price            DOUBLE       not null,
                start_time       TIMESTAMP    not null,
                end_time         TIMESTAMP    not null,
                start_station_id INTEGER      not null,
                end_station_id   INTEGER      not null,
                constraint rides_pk
                        primary key (rail_user, start_station_id, start_time)
                );
        CREATE TABLE IF NOT EXISTS line_details ( 
                line_id INT, 
                station_id INT,
                station_order INT
                -- ,FOREIGN KEY (line_name) REFERENCES \`lines\`(name)
                -- ,FOREIGN KEY (station_name) REFERENCES stations(chinese_name)
                );
        CREATE TABLE IF NOT EXISTS bus_stations (name CHAR(255) PRIMARY KEY, district CHAR(255));
        CREATE TABLE IF NOT EXISTS bus_lines (name CHAR(255) PRIMARY KEY);
        CREATE TABLE IF NOT EXISTS bus_line_details (
                bus_line_name CHAR(255), 
                station_order INT,
                bus_station_name CHAR(255)
                -- ,FOREIGN KEY (bus_station_name) REFERENCES bus_stations(name), 
                );
        CREATE TABLE IF NOT EXISTS price (
                station1_id INT,
                station2_id INT,
                price DOUBLE PRECISION
                );        
            LOAD DATA LOCAL INFILE 'cards.csv' INTO TABLE cards  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
            LOAD DATA LOCAL INFILE 'passengers.csv' INTO TABLE passengers  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
            LOAD DATA LOCAL INFILE 'lines.csv' INTO TABLE \`lines\`  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
            LOAD DATA LOCAL INFILE 'stations.csv' INTO TABLE stations  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
            LOAD DATA LOCAL INFILE 'passenger_rides.csv' INTO TABLE passenger_rides  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
            LOAD DATA LOCAL INFILE 'card_rides.csv' INTO TABLE card_rides  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
            LOAD DATA LOCAL INFILE 'line_details.csv' INTO TABLE line_details  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
            LOAD DATA LOCAL INFILE 'bus_lines.csv' INTO TABLE bus_lines  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
            LOAD DATA LOCAL INFILE 'bus_stations.csv' INTO TABLE bus_stations  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
            LOAD DATA LOCAL INFILE 'BusOrder.csv' INTO TABLE bus_line_details  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
            LOAD DATA LOCAL INFILE 'price.csv' INTO TABLE price  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
            " -u $DB_USER -h $DB_HOST -P 3306 -p$DB_PASSWORD $DB_NAME
)