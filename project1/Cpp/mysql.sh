source ../setenv.sh
export DATA_PATH=$(cd .. && pwd)/Data
cd build
# rm -f ./LoadData
make

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
    time (
        ./Mysql
    )
elif [ ${machine} == "Mac" ]; then
    echo "not supported yet"
else
    export CSV_PATH=$(pwd)
fi
