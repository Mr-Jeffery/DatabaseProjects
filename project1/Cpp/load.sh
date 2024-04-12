export DATA_PATH=$(cd ../Data && pwd)
source ../setenv.sh

cd build
rm -f ./LoadData
make

# psql --command="DROP TABLE IF EXISTS cards;" postgresql://<user>:<password>@<host>:<port>/<db>
psql --command="DROP TABLE IF EXISTS cards;" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
psql --command="DROP TABLE IF EXISTS passengers;" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
psql --command="DROP TABLE IF EXISTS rides;" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
./LoadData
echo "Number of rides:"
psql --command="SELECT COUNT(*) FROM rides;" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME