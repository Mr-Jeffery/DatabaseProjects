source ../setenv.sh
export DATA_PATH=$(cd .. && pwd)/Data
cd build
make # can be commented out if the executable is already built

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
    export CSV_PATH=/tmp
elif [ ${machine} == "Mac" ]; then
    export CSV_PATH=$TMPDIR
else
    export CSV_PATH=$(pwd)
fi
time (
    ./Postgres
    # ./LoadData
)

psql --command="
select distinct bus_line_name
from bus_line_details
where bus_line_name like '%出入口%';

UPDATE bus_line_details
SET bus_line_name = REGEXP_REPLACE(bus_line_name, '（新增）(.*)', '')
WHERE bus_line_name LIKE '%（新增）%';

UPDATE bus_line_details
SET bus_line_name = REGEXP_REPLACE(bus_line_name, '[A-Z]*[0-9]*出入口$', '')
WHERE bus_line_name LIKE '%出入口%';

UPDATE bus_line_details
SET bus_line_name = REGEXP_REPLACE(bus_line_name, '（原(.*)', '')
WHERE bus_line_name LIKE '%（原%';

UPDATE bus_line_details
SET bus_line_name = REGEXP_REPLACE(bus_line_name, '\(原(.*)', '')
WHERE bus_line_name LIKE '%\(原%';" postgresql://$DB_USER:$DB_PASSWORD@$DB_HOST:$DB_PORT/$DB_NAME