source ../setenv.sh
export DATA_PATH=$(cd .. && pwd)/Data
cd build
make # can be commented out if the executable is already built

mysql -e "DROP DATABASE IF EXISTS $DB_NAME; CREATE DATABASE $DB_NAME;" -u $DB_USER -h $DB_HOST -P 3306 -p$DB_PASSWORD

unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     machine=Linux;;
    Darwin*)    machine=Mac;;
    *)          machine="UNKNOWN:${unameOut}"
esac
echo ${machine}

if [ ${machine} == "Linux" ]; then
    export CSV_PATH=/tmp
elif [ ${machine} == "Mac" ]; then
    export CSV_PATH=$TMPDIR
else
    export CSV_PATH=$(pwd)
fi
time (
mysql --local-infile=1 -e "CREATE TABLE IF NOT EXISTS cards( 
                    code CHAR(9) PRIMARY KEY,
                    money DECIMAL(19,4) NOT NULL, 
                    create_time TIMESTAMP NOT NULL);
            CREATE TABLE IF NOT EXISTS passengers ( 
                    name CHAR(255) NOT NULL,
                    id_number CHAR(18) PRIMARY KEY NOT NULL,
                    phone_number CHAR(11) NOT NULL, 
                    gender CHAR(255) NOT NULL, 
                    district CHAR(255) NOT NULL );
            CREATE TABLE IF NOT EXISTS \`lines\` ( 
                    name CHAR(255) PRIMARY KEY,
                    start_time TIME, 
                    end_time TIME, 
                    mileage DOUBLE PRECISION, 
                    color CHAR(255), 
                    first_opening DATE, 
                    url TEXT,
                    intro TEXT);
            CREATE TABLE IF NOT EXISTS stations (
                    name CHAR(255), 
                    district CHAR(255), 
                    intro TEXT, 
                    chinese_name CHAR(255) PRIMARY KEY);
            CREATE TABLE IF NOT EXISTS rides (
                    rail_user CHAR(255), 
                    start_station CHAR(255), 
                    end_station CHAR(255), 
                    price DOUBLE PRECISION, 
                    start_time TIMESTAMP, 
                    end_time TIMESTAMP);
            CREATE TABLE IF NOT EXISTS line_details ( 
                    line_name CHAR(255), 
                    station_order INT,
                    station_name CHAR(255),
                    FOREIGN KEY (line_name) REFERENCES \`lines\`(name), 
                    FOREIGN KEY (station_name) REFERENCES stations(chinese_name)
                    );
            CREATE TABLE IF NOT EXISTS bus_stations (name CHAR(255) PRIMARY KEY, district CHAR(255));
            CREATE TABLE IF NOT EXISTS bus_lines (name CHAR(255) PRIMARY KEY);
            CREATE TABLE IF NOT EXISTS bus_line_details (
                    bus_line_name CHAR(255), 
                    station_order INT,
                    bus_station_name CHAR(255)
                    -- ,FOREIGN KEY (bus_station_name) REFERENCES bus_stations(name), 
                    );
            LOAD DATA LOCAL INFILE 'cards.csv' INTO TABLE cards  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
            LOAD DATA LOCAL INFILE 'passengers.csv' INTO TABLE passengers  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
            LOAD DATA LOCAL INFILE 'lines.csv' INTO TABLE \`lines\`  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
            LOAD DATA LOCAL INFILE 'stations.csv' INTO TABLE stations  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
            LOAD DATA LOCAL INFILE 'rides.csv' INTO TABLE rides  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
            LOAD DATA LOCAL INFILE 'shenzhen_metro.csv' INTO TABLE line_details  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
            LOAD DATA LOCAL INFILE 'bus_lines.csv' INTO TABLE bus_lines  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
            LOAD DATA LOCAL INFILE 'bus_stations.csv' INTO TABLE bus_stations  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
            LOAD DATA LOCAL INFILE 'BusOrder.csv' INTO TABLE bus_line_details  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
            " -u $DB_USER -h $DB_HOST -P 3306 -p$DB_PASSWORD $DB_NAME
)