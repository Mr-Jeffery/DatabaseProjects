source ../setenv.sh

cd build
rm -f ./LoadData
make

# psql --command="" postgresql://<user>:<password>@<host>:<port>/<db>

# psql -U $DB_USER -d $DB_NAME -p $DB_PORT -h $DB_HOST


# psql --command="CREATE TABLE IF NOT EXISTS rides (rail_user VARCHAR(255), start_station VARCHAR(255), end_station VARCHAR(255), price DOUBLE PRECISION, start_time TIMESTAMP, end_time TIMESTAMP);" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME

CSV_PATH=$(pwd)
time (
    psql --command="DROP TABLE IF EXISTS rides;" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME

    psql --command="DROP TABLE IF EXISTS cards;" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
    psql --command="DROP TABLE IF EXISTS passengers;" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
    
    psql --command="DROP TABLE IF EXISTS line_details;" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME

    psql --command="DROP TABLE IF EXISTS lines;" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
    psql --command="DROP TABLE IF EXISTS exits;" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
    psql --command="DROP TABLE IF EXISTS bus_stations;" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
    psql --command="DROP TABLE IF EXISTS stations;" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
    ./LoadData

    psql --command="\COPY cards FROM '$CSV_PATH/cards.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
    psql --command="\COPY passengers FROM '$CSV_PATH/passengers.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
    psql --command="\COPY rides FROM '$CSV_PATH/rides.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
    psql --command="\COPY lines FROM '$CSV_PATH/lines.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME

    psql --command="\COPY stations FROM '$CSV_PATH/stations.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
    psql --command="\COPY line_details FROM '$CSV_PATH/line_details.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
    # psql --command="\COPY bus_stations FROM '$CSV_PATH/bus_stations.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',') ;" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
    psql postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME <<EOF
    CREATE TEMP TABLE tmp_bus_stations AS SELECT * FROM bus_stations LIMIT 0;
    \COPY tmp_bus_stations FROM '$CSV_PATH/bus_stations.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');
    INSERT INTO bus_stations SELECT * FROM tmp_bus_stations ON CONFLICT DO NOTHING;
EOF

    
)
