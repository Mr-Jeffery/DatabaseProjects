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
    mysql -e "
    UPDATE bus_line_details
    SET bus_line_name = REGEXP_REPLACE(bus_line_name, '（新增）(.*)', '')
    WHERE bus_line_name LIKE '%（新增）%';

    UPDATE bus_line_details
    SET bus_line_name = REGEXP_REPLACE(bus_line_name, '[A-Z]*[0-9]*出入口$', '')
    WHERE bus_line_name LIKE '%出入口%';

    UPDATE bus_line_details
    SET bus_line_name = REGEXP_REPLACE(bus_line_name, '（原(.*)', '')
    WHERE bus_line_name LIKE '%（原%';" -u $DB_USER -h $DB_HOST -P 3306 -p$DB_PASSWORD $DB_NAME
else 
    echo "not supported yet"
fi
