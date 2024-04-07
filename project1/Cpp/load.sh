mkdir -p lib
cd lib
wget https://github.com/jtv/libpqxx/archive/refs/tags/7.9.0.tar.gz
tar -xvf 7.9.0.tar.gz
mv libpqxx-7.9.0 libpqxx
cd ..

mkdir -p build
cd build
cmake ..
make

export DATA_PATH=$(cd ../../Data && pwd)
sh ../setenv.sh

# psql --command="DROP TABLE IF EXISTS cards;" postgresql://<user>:<password>@<host>:<port>/<db>
psql --command="DROP TABLE IF EXISTS cards;" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME
./LoadData