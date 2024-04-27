source ../setenv.sh
export DATA_PATH=$(cd .. && pwd)/Data
cd build
# rm -f ./LoadData
make

# psql --command="" postgresql://<user>:<password>@<host>:<port>/<db>

# psql -U $DB_USER -d $DB_NAME -p $DB_PORT -h $DB_HOST

psql --command="DROP TABLE IF EXISTS rides CASCADE;\
                DROP TABLE IF EXISTS cards CASCADE;\
                DROP TABLE IF EXISTS passengers CASCADE;\
                DROP TABLE IF EXISTS line_details CASCADE;\
                DROP TABLE IF EXISTS lines CASCADE;\
                DROP TABLE IF EXISTS exits CASCADE;\
                DROP TABLE IF EXISTS bus_stations CASCADE;\
                DROP TABLE IF EXISTS stations CASCADE;\
                DROP TABLE IF EXISTS bus_line_details CASCADE;\
                DROP TABLE IF EXISTS bus_lines CASCADE;\
                DROP TABLE IF EXISTS exit_details;" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME

unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     machine=Linux;;
    Darwin*)    machine=Mac;;
    *)          machine="UNKNOWN:${unameOut}"
esac
echo ${machine}

if [ ${machine} == "Linux" ]; then
    export CSV_PATH=$CSV_PATH
elif [ ${machine} == "Mac" ]; then
    export CSV_PATH=$TMPDIR
else
    export CSV_PATH=$(pwd)
fi
time (
    ./Data
    psql --command="CREATE TABLE IF NOT EXISTS cards(
                            code VARCHAR(9) PRIMARY KEY CHECK (code ~ '^[0-9]{9}$'), 
                            money MONEY NOT NULL, 
                            create_time TIMESTAMP NOT NULL);
                    CREATE TABLE IF NOT EXISTS passengers (
                            name VARCHAR(255) NOT NULL, 
                            id_number VARCHAR(18) PRIMARY KEY NOT NULL CHECK (id_number ~ '^[0-9]{17}[0-9X]?$'), 
                            phone_number VARCHAR(11) NOT NULL CHECK (phone_number ~ '^[0-9]{11}$'), 
                            gender VARCHAR(255) NOT NULL, 
                            district VARCHAR(255) NOT NULL);
                    CREATE TABLE IF NOT EXISTS lines (
                            name VARCHAR(255)PRIMARY KEY, 
                            start_time TIME, 
                            end_time TIME, 
                            mileage DOUBLE PRECISION, 
                            color VARCHAR(255), 
                            first_opening DATE, 
                            url TEXT,
                            intro TEXT);
                    CREATE TABLE IF NOT EXISTS stations (
                            name VARCHAR(255) PRIMARY KEY, 
                            district VARCHAR(255), 
                            intro TEXT, 
                            chinese_name VARCHAR(255));
                    CREATE TABLE IF NOT EXISTS rides (
                            rail_user VARCHAR(255), 
                            start_station VARCHAR(255), 
                            end_station VARCHAR(255), 
                            price DOUBLE PRECISION, 
                            start_time TIMESTAMP, 
                            end_time TIMESTAMP, 
                            FOREIGN KEY (start_station) REFERENCES stations(name), 
                            FOREIGN KEY (end_station) REFERENCES stations(name));
                    CREATE TABLE IF NOT EXISTS line_details (line_name VARCHAR(255), station_name VARCHAR(255), FOREIGN KEY (line_name) REFERENCES lines(name), FOREIGN KEY (station_name) REFERENCES stations(name));
                    CREATE TABLE IF NOT EXISTS bus_stations (name VARCHAR(255) PRIMARY KEY, district VARCHAR(255));
                    CREATE TABLE IF NOT EXISTS bus_lines (name VARCHAR(255) PRIMARY KEY);
                    CREATE TABLE IF NOT EXISTS bus_line_details (
                            bus_line_name VARCHAR(255), 
                            bus_station_name VARCHAR(255), 
                            FOREIGN KEY (bus_station_name) REFERENCES bus_stations(name));
                    CREATE TABLE IF NOT EXISTS exits (
                            station_name VARCHAR(255), 
                            name VARCHAR(255),
                            textt TEXT,
                            FOREIGN KEY (station_name) REFERENCES stations(name),
                            PRIMARY KEY (station_name, name));
                    CREATE TABLE IF NOT EXISTS exit_details (
                            station_name VARCHAR(255), 
                            name VARCHAR(255), 
                            bus_station_name VARCHAR(255), 
                            FOREIGN KEY (station_name, name) REFERENCES exits(station_name,name), 
                            FOREIGN KEY (bus_station_name) REFERENCES bus_stations(name) , 
                            PRIMARY KEY (station_name, name, bus_station_name));" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
    psql --command="\copy cards FROM '$CSV_PATH/cards.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',')" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
    psql --command="\copy passengers FROM '$CSV_PATH/passengers.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',')" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
    psql --command="\copy lines FROM '$CSV_PATH/lines.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',')" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
    psql --command="\copy stations FROM '$CSV_PATH/stations.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',')" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
    psql --command="\copy rides FROM '$CSV_PATH/rides.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',')" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
    psql --command="\copy line_details FROM '$CSV_PATH/line_details.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',')" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
    psql --command="\copy exits FROM '$CSV_PATH/exits.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',')" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
    psql postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME <<EOF
    CREATE TEMP TABLE tmp_bus_stations AS SELECT * FROM bus_stations LIMIT 0;
    \copy tmp_bus_stations FROM '$CSV_PATH/bus_stations.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');
    INSERT INTO bus_stations SELECT * FROM tmp_bus_stations ON CONFLICT DO NOTHING;
    CREATE TEMP TABLE tmp_bus_line_details AS SELECT * FROM bus_line_details LIMIT 0;
    \copy tmp_bus_line_details FROM '$CSV_PATH/bus_line_details.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');
    INSERT INTO bus_line_details SELECT DISTINCT * FROM tmp_bus_line_details ON CONFLICT DO NOTHING;
    CREATE TEMP TABLE tmp_exit_details AS SELECT * FROM exit_details LIMIT 0;
    \copy tmp_exit_details FROM '$CSV_PATH/exit_details.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');
    INSERT INTO exits SELECT station_name, name, bus_station_name AS textt FROM tmp_exit_details ON CONFLICT DO NOTHING;
    INSERT INTO exit_details SELECT * FROM tmp_exit_details ON CONFLICT DO NOTHING;
EOF
)
