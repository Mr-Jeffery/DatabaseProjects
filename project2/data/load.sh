source ../setenv.sh
mysql -e "DROP DATABASE IF EXISTS $DB_NAME; CREATE DATABASE $DB_NAME;" -u $DB_USER -h $DB_HOST -P 3306 -p$DB_PASSWORD

time (
mysql --local-infile=1 -e "
        CREATE TABLE IF NOT EXISTS cards( 
                code CHAR(9) PRIMARY KEY,
                balance DECIMAL(19,2) NOT NULL, 
                create_time TIMESTAMP NOT NULL);

        DELIMITER //
                CREATE TRIGGER check_card_code
                BEFORE INSERT ON cards
                FOR EACH ROW
                BEGIN
                IF NEW.code NOT REGEXP '^[0-9]{9}$' THEN
                        SIGNAL SQLSTATE '45000'
                        SET MESSAGE_TEXT = 'Invalid card code. It should be in the form of ^[0-9]{9}$';
                END IF;
                IF NEW.balance < 0 THEN
                        SIGNAL SQLSTATE '45000'
                        SET MESSAGE_TEXT = 'Invalid card balance. It should be greater than or equal to 0';
                END IF;
                END;
                //
                CREATE TRIGGER prevent_code_update
                BEFORE UPDATE ON cards
                FOR EACH ROW
                BEGIN
                IF OLD.code != NEW.code THEN
                        SIGNAL SQLSTATE '45000'
                        SET MESSAGE_TEXT = 'Cannot change code field';
                END IF;
                END;
                //
        DELIMITER ;

        CREATE TABLE IF NOT EXISTS passengers (
                id CHAR(18) PRIMARY KEY NOT NULL,
                name CHAR(255) NOT NULL,
                phone_number CHAR(11) NOT NULL, 
                gender CHAR(255) NOT NULL, 
                district CHAR(255) NOT NULL );

        DELIMITER //
                CREATE TRIGGER check_passenger_id
                BEFORE INSERT ON passengers
                FOR EACH ROW
                BEGIN
                IF NEW.id NOT REGEXP '^[0-9]{17}[0-9X]?$' THEN
                        SIGNAL SQLSTATE '45000'
                        SET MESSAGE_TEXT = 'Invalid passenger id. It should be in the form of ^[0-9]{17}[0-9X]?$';
                END IF;
                END;
                //
                CREATE TRIGGER prevent_id_update
                BEFORE UPDATE ON passengers
                FOR EACH ROW
                BEGIN
                IF OLD.id != NEW.id THEN
                        SIGNAL SQLSTATE '45000'
                        SET MESSAGE_TEXT = 'Cannot change id field';
                END IF;
                END;
                //
        DELIMITER ;

        CREATE TABLE IF NOT EXISTS \`lines\` ( 
                id INT PRIMARY KEY AUTO_INCREMENT,
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
                id   integer      not null AUTO_INCREMENT,
                chinese_name CHAR(255),
                name         VARCHAR(255) not null,
                district     CHAR(255)  not null,
                intro        text         not null,
                status        CHAR(255)   not null,
                constraint stations_pk
                        primary key (id)
                );
        CREATE TABLE IF NOT EXISTS passenger_rides (
                ride_id        INT AUTO_INCREMENT,
                id        VARCHAR(18) not null,
                price            DECIMAL(19,2),
                start_time       TIMESTAMP    not null,
                end_time         TIMESTAMP,
                start_station_id INTEGER      not null,
                end_station_id   INTEGER,
                FOREIGN KEY (start_station_id) REFERENCES stations(id),
                FOREIGN KEY (end_station_id) REFERENCES stations(id),
                constraint rides_pk
                        primary key (ride_id)
                );

        CREATE INDEX end_time_index ON passenger_rides (end_time);
        
        CREATE TABLE IF NOT EXISTS passenger_ride_details (
                ride_id INT not null,
                price            DECIMAL(19,2),
                start_time       TIMESTAMP    not null,
                end_time         TIMESTAMP,
                start_station_id INTEGER      not null,
                end_station_id   INTEGER,
                FOREIGN KEY (ride_id) REFERENCES passenger_rides (ride_id),
                constraint rides_pk
                        primary key (ride_id)
                );
        
        CREATE TABLE IF NOT EXISTS card_rides (
                ride_id INT AUTO_INCREMENT,
                code        VARCHAR(9) not null,
                price            DECIMAL(19,2),
                start_time       TIMESTAMP    not null,
                end_time         TIMESTAMP,
                start_station_id INTEGER      not null,
                end_station_id   INTEGER,
                FOREIGN KEY (start_station_id) REFERENCES stations(id),
                FOREIGN KEY (end_station_id) REFERENCES stations(id),
                constraint rides_pk
                        primary key (ride_id)
                );

        CREATE INDEX end_time_index ON card_rides (end_time);

        CREATE TABLE IF NOT EXISTS card_ride_details (
                ride_id INT not null,
                price            DECIMAL(19,2),
                start_time       TIMESTAMP    not null,
                end_time         TIMESTAMP,
                start_station_id INTEGER      not null,
                end_station_id   INTEGER,
                FOREIGN KEY (ride_id) REFERENCES card_rides (ride_id),
                constraint rides_pk
                        primary key (ride_id)
                );
        CREATE TABLE IF NOT EXISTS line_details ( 
                line_id INT, 
                station_id INT,
                station_order INT
                ,FOREIGN KEY (line_id) REFERENCES \`lines\`(id)
                ,FOREIGN KEY (station_id) REFERENCES stations(id)
                );
        CREATE TABLE IF NOT EXISTS bus_stations (name CHAR(255) PRIMARY KEY, district CHAR(255));
        CREATE TABLE IF NOT EXISTS bus_lines (name CHAR(255) PRIMARY KEY);
        CREATE TABLE IF NOT EXISTS bus_line_details (
                bus_line_name CHAR(255), 
                station_order INT,
                bus_station_name CHAR(255),
                FOREIGN KEY (bus_station_name) REFERENCES bus_stations(name)
                );
        CREATE TABLE IF NOT EXISTS prices (
                station1_id INT,
                station2_id INT,
                price DECIMAL(19,2)
                );
        CREATE TABLE IF NOT EXISTS users (
                id INT PRIMARY KEY AUTO_INCREMENT,
                username CHAR(255) NOT NULL,
                password CHAR(255) NOT NULL
                );
        
        INSERT INTO users (username, password) VALUES ('luckychen', 'new_password');
        INSERT INTO users (username, password) VALUES ('jeffery', '1qaz2wsx');
        INSERT INTO users (username, password) VALUES ('guest', '123456');
                
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
        LOAD DATA LOCAL INFILE 'prices.csv' INTO TABLE prices  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
        UPDATE stations SET status = 'operational';
        " -u $DB_USER -h $DB_HOST -P 3306 -p$DB_PASSWORD $DB_NAME
)