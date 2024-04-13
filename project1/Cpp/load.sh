source ../setenv.sh

cd build
rm -f ./LoadData
make

# psql --command="" postgresql://<user>:<password>@<host>:<port>/<db>

# psql --command="DROP TABLE IF EXISTS cards;" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
# psql --command="DROP TABLE IF EXISTS passengers;" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
psql --command="DROP TABLE IF EXISTS rides;" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
psql --command="CREATE TABLE IF NOT EXISTS rides (rail_user VARCHAR(255), start_station VARCHAR(255), end_station VARCHAR(255), price DOUBLE PRECISION, start_time TIMESTAMP, end_time TIMESTAMP);" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
./LoadData
CSV_PATH=$(pwd)/rides.csv
time psql --command="\COPY rides FROM '$CSV_PATH' WITH (FORMAT CSV, HEADER, DELIMITER ',');" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
echo "Number of rides:"
psql --command="SELECT COUNT(*) FROM rides;" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME