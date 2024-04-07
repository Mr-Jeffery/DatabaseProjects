export DATA_PATH=$(cd ../../Data && pwd)
source ./setenv.sh

# psql --command="DROP TABLE IF EXISTS cards;" postgresql://<user>:<password>@<host>:<port>/<db>
psql --command="DROP TABLE IF EXISTS cards;" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
./LoadData