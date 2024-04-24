source ../setenv.sh
export DATA_PATH=$(cd .. && pwd)/Data
cd build
make 

# psql --command="" postgresql://<user>:<password>@<host>:<port>/<db>

# psql -U $DB_USER -d $DB_NAME -p $DB_PORT -h $DB_HOST

psql --command="DROP TABLE IF EXISTS rides CASCADE;" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME

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


time ./SpeedTest
