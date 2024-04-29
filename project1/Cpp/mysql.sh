source ../setenv.sh
export DATA_PATH=$(cd .. && pwd)/Data
cd build
make # can be commented out if the executable is already built

mysql -e "DROP DATABASE IF EXISTS $DB_NAME; CREATE DATABASE $DB_NAME;" -u $DB_USER -h $DB_HOST -P $DB_PORT -p$DB_PASSWORD

unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     machine=Linux;;
    Darwin*)    machine=Mac;;
    *)          machine="UNKNOWN:${unameOut}"
esac
echo ${machine}

if [ ${machine} == "Linux" ]; then
    export CSV_PATH=/tmp
    time (
        ./Mysql
    )
else 
    echo "not supported yet"
fi
