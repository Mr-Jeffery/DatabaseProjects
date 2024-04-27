#include <mysql.h>
#include "json2csv.hpp"

int main() {
    MYSQL *con = mysql_init(NULL);

    if (con == NULL) {
        fprintf(stderr, "%s\n", mysql_error(con));
        return 1;
    }
    const char* db_name_cstr = getenv("DB_NAME");
    const char* db_name = db_name_cstr ? db_name_cstr : "project1";

    const char* db_user_cstr = getenv("DB_USER");
    const char* db_user = db_user_cstr ? db_user_cstr : "root";

    const char* db_password_cstr = getenv("DB_PASSWORD");
    const char* db_password = db_password_cstr ? db_password_cstr : "";

    const char* db_host_cstr = getenv("DB_HOST");
    const char* db_host = db_host_cstr ? db_host_cstr : "localhost";

    const char* db_port_cstr = getenv("DB_PORT");
    const char* db_port = db_port_cstr ? db_port_cstr : "3306";

    // Enable LOAD DATA LOCAL INFILE
    if (mysql_options(con, MYSQL_OPT_LOCAL_INFILE, "1")) {
        fprintf(stderr, "%s\n", mysql_error(con));
        return 1;
    }

    if (mysql_real_connect(con, db_host, db_user, db_password, 
          db_name, 0, NULL, CLIENT_LOCAL_FILES) == NULL) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }  

    // if (mysql_query(con, "LOAD DATA LOCAL INFILE '/tmp/rides.csv' INTO TABLE rides FIELDS TERMINATED BY ','")) {
    //     fprintf(stderr, "%s\n", mysql_error(con));
    //     mysql_close(con);
    //     return 1;
    // }
    struct timeval start, end;
    double time_spent;

    // Get the csv file paths
    std::filesystem::path csv_path = json2csv();
    std::filesystem::path cards_csv_path = csv_path / "cards.csv";
    std::filesystem::path passengers_csv_path = csv_path / "passengers.csv";
    std::filesystem::path lines_csv_path = csv_path / "lines.csv";
    std::filesystem::path stations_csv_path = csv_path / "stations.csv";
    std::filesystem::path rides_csv_path = csv_path / "rides.csv";
    std::filesystem::path line_details_csv_path = csv_path / "line_details.csv";
    std::filesystem::path exits_csv_path = csv_path / "exits.csv";
    std::filesystem::path bus_stations_csv_path = csv_path / "bus_stations.csv";
    std::filesystem::path bus_line_details_csv_path = csv_path / "bus_line_details.csv";
    std::filesystem::path exit_details_csv_path = csv_path / "exit_details.csv";

    // Load the csv files into the database
std::string create_cards_query = std::string("CREATE TABLE IF NOT EXISTS cards( ")
        +"\n\tcode CHAR(9) PRIMARY KEY CHECK (code REGEXP '^[0-9]{9}$'),"
        +"\n\tmoney DECIMAL(19,4) NOT NULL, "
        +"\n\tcreate_time TIMESTAMP NOT NULL);";
    std::cout << create_cards_query << '\n';
    if (mysql_query(con, create_cards_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    // // Create the passengers table
    std::string create_passengers_query = std::string("CREATE TABLE IF NOT EXISTS passengers ( ")
        +"\n\tname CHAR(255) NOT NULL,"
        +"\n\tid_number CHAR(18) PRIMARY KEY NOT NULL CHECK (id_number REGEXP '^[0-9]{17}[0-9X]?$'),"
        +"\n\tphone_number CHAR(11) NOT NULL CHECK (phone_number REGEXP '^[0-9]{11}$'), "
        +"\n\tgender CHAR(255) NOT NULL, "
        +"\n\tdistrict CHAR(255) NOT NULL );";
    std::cout << create_passengers_query << '\n';
    if (mysql_query(con, create_passengers_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }
    

    // Create the lines table
    std::string create_lines_query = std::string("CREATE TABLE IF NOT EXISTS `lines` ( ")
        +"\n\tname CHAR(255)PRIMARY KEY,"
        +"\n\tstart_time TIME, "
        +"\n\tend_time TIME, "
        +"\n\tmileage DOUBLE PRECISION, "
        +"\n\tcolor CHAR(255), "
        +"\n\tfirst_opening DATE, "
        +"\n\turl TEXT,"
        +"\n\tintro TEXT);";
    std::cout << create_lines_query << '\n';
    if (mysql_query(con, create_lines_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    // Create the stations table
    std::string create_stations_query = std::string("CREATE TABLE IF NOT EXISTS stations (")
        // +"station_id VARCHAR(255) PRIMARY KEY, "
        +"\n\tname CHAR(255) PRIMARY KEY, "
        +"\n\tdistrict CHAR(255), "
        +"\n\tintro TEXT, "
        +"\n\tchinese_name CHAR(255));";
    std::cout << create_stations_query << '\n';
    if (mysql_query(con, create_stations_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    // Create the rides table
    std::string create_rides_query = std::string("CREATE TABLE IF NOT EXISTS rides (")
        +"\n\trail_user CHAR(255), "
        +"\n\tstart_station CHAR(255), "
        +"\n\tend_station CHAR(255), "
        +"\n\tprice DOUBLE PRECISION, "
        +"\n\tstart_time TIMESTAMP, "
        +"\n\tend_time TIMESTAMP"
        // +", \n\tFOREIGN KEY (start_station) REFERENCES stations(name), "
        // +"\n\tFOREIGN KEY (end_station) REFERENCES stations(name)"
        +");";
    std::cout << create_rides_query << '\n';
    if (mysql_query(con, create_rides_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    // Create the line_details table
    std::string create_line_details_query = std::string("CREATE TABLE IF NOT EXISTS line_details ( ")
        +"line_name CHAR(255), "
        +"station_name CHAR(255)"
        +", FOREIGN KEY (line_name) REFERENCES `lines`(name), "
        +"FOREIGN KEY (station_name) REFERENCES stations(name)"
        +");";
    std::cout << create_line_details_query << '\n';
    if (mysql_query(con, create_line_details_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    // Create the bus_stations table
    std::string create_bus_stations_query = std::string("CREATE TABLE IF NOT EXISTS bus_stations (")
        // +"station_id VARCHAR(255), "
        +"name CHAR(255) PRIMARY KEY, "
        +"district CHAR(255));";
    std::cout << create_bus_stations_query << '\n';
    if (mysql_query(con, create_bus_stations_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    // Create the bus_line table
    std::string create_bus_lines_query = std::string("CREATE TABLE IF NOT EXISTS bus_lines (")
        // +"bus_line_id VARCHAR(255), "
        +"name CHAR(255) PRIMARY KEY);";
    std::cout << create_bus_lines_query << '\n';
    if (mysql_query(con, create_bus_lines_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    // Create the bus_line_details table
    std::string create_bus_line_details_query = std::string("CREATE TABLE IF NOT EXISTS bus_line_details (")
        +"\n\tbus_line_name CHAR(255), "
        +"\n\tbus_station_name CHAR(255), "
        // +"\n\tFOREIGN KEY (bus_line_name) REFERENCES bus_lines(name), "// this line is commented because the bus_lines itself is created by copying the bus_line_details
        +"\n\tFOREIGN KEY (bus_station_name) REFERENCES bus_stations(name)"
        +");";
    std::cout << create_bus_line_details_query << '\n';
    if (mysql_query(con, create_bus_line_details_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    // Create the exits table
    std::string create_exits_query = std::string("CREATE TABLE IF NOT EXISTS exits (")
        +"\n\tstation_name CHAR(255), "
        +"\n\tname CHAR(255),"
        +"\n\ttextt TEXT,"
        +"\n\tFOREIGN KEY (station_name) REFERENCES stations(name),"
        +"\n\tPRIMARY KEY (station_name, name)"
        +");";
    std::cout << create_exits_query << '\n';
    if (mysql_query(con, create_exits_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    // Create the exit_details table
    std::string create_exit_details_query = std::string("CREATE TABLE IF NOT EXISTS exit_details (")
        +"\n\tstation_name CHAR(255), "
        +"\n\tname CHAR(255), "
        +"\n\tbus_station_name CHAR(255)"
        +", \n\tFOREIGN KEY (station_name, name) REFERENCES exits(station_name,name), "
        +"\n\tFOREIGN KEY (bus_station_name) REFERENCES bus_stations(name) "
        +", \n\tPRIMARY KEY (station_name, name, bus_station_name)"
        +");";
    std::cout << create_exit_details_query << '\n';
    if (mysql_query(con, create_exit_details_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    // Copy the csv files to the database
    gettimeofday(&start, NULL);
    std::string copy_cards_query = "LOAD DATA LOCAL INFILE '" + cards_csv_path.string() + "' INTO TABLE cards  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\\n' IGNORE 1 ROWS;";
    std::cout << copy_cards_query << '\n';
    if (mysql_query(con, copy_cards_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    std::string copy_passengers_query = "LOAD DATA LOCAL INFILE '" + passengers_csv_path.string() + "' INTO TABLE passengers  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\\n' IGNORE 1 ROWS;";
    std::cout << copy_passengers_query << '\n';
    if (mysql_query(con, copy_passengers_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    std::string copy_lines_query = "LOAD DATA LOCAL INFILE '" + lines_csv_path.string() + "' INTO TABLE `lines`  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\\n' IGNORE 1 ROWS;";
    std::cout << copy_lines_query << '\n';
    if (mysql_query(con, copy_lines_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    std::string copy_stations_query = "LOAD DATA LOCAL INFILE '" + stations_csv_path.string() + "' INTO TABLE stations  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\\n' IGNORE 1 ROWS;";
    std::cout << copy_stations_query << '\n';
    if (mysql_query(con, copy_stations_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    std::string copy_rides_query = "LOAD DATA LOCAL INFILE '" + rides_csv_path.string() + "' INTO TABLE rides  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\\n' IGNORE 1 ROWS;";
    std::cout << copy_rides_query << '\n';
    if (mysql_query(con, copy_rides_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    std::string copy_line_details_query = "LOAD DATA LOCAL INFILE '" + line_details_csv_path.string() + "' INTO TABLE line_details  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\\n' IGNORE 1 ROWS;";
    std::cout << copy_line_details_query << '\n';
    if (mysql_query(con, copy_line_details_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    std::string copy_exits_query = "LOAD DATA LOCAL INFILE '" + exits_csv_path.string() + "' INTO TABLE exits  FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\\n' IGNORE 1 ROWS;";
    std::cout << copy_exits_query << '\n';
    if (mysql_query(con, copy_exits_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }
    
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // different from postgresql. mysql has to execute queries one by one
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    std::string create_tmp_bus_stations_query = "CREATE TEMPORARY TABLE tmp_bus_stations AS SELECT * FROM bus_stations LIMIT 0;";
    std::cout << create_tmp_bus_stations_query << '\n';
    if (mysql_query(con, create_tmp_bus_stations_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    std::string load_tmp_bus_stations_query = "LOAD DATA LOCAL INFILE '"
        + bus_stations_csv_path.string()
        + "' INTO TABLE tmp_bus_stations FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\\n' IGNORE 1 ROWS;";
    std::cout << load_tmp_bus_stations_query << '\n';
    if (mysql_query(con, load_tmp_bus_stations_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    std::string insert_bus_stations_query = "INSERT IGNORE INTO bus_stations SELECT * FROM tmp_bus_stations;";
    std::cout << insert_bus_stations_query << '\n';
    if (mysql_query(con, insert_bus_stations_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    std::string create_tmp_bus_line_details_query = "CREATE TEMPORARY TABLE tmp_bus_line_details AS SELECT * FROM bus_line_details LIMIT 0;";
    std::cout << create_tmp_bus_line_details_query << '\n';
    if (mysql_query(con, create_tmp_bus_line_details_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    std::string load_tmp_bus_line_details_query = "LOAD DATA LOCAL INFILE '"
        + bus_line_details_csv_path.string()
        + "' INTO TABLE tmp_bus_line_details FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\\n' IGNORE 1 ROWS;";
    std::cout << load_tmp_bus_line_details_query << '\n';
    if (mysql_query(con, load_tmp_bus_line_details_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    std::string insert_bus_line_details_query = "INSERT IGNORE INTO bus_line_details SELECT DISTINCT * FROM tmp_bus_line_details;";
    std::cout << insert_bus_line_details_query << '\n';
    if (mysql_query(con, insert_bus_line_details_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    // the following query has been moved to be executed after data cleaning
    // std::string make_bus_line_query = "INSERT INTO bus_lines SELECT DISTINCT bus_line_name FROM bus_line_details;";
    // std::cout << make_bus_line_query << '\n';
    // if (mysql_query(con, make_bus_line_query.c_str())) {
    //     fprintf(stderr, "%s\n", mysql_error(con));
    //     mysql_close(con);
    //     return 1;
    // }

    // std::string copy_exit_details_query = "COPY exit_details FROM '" + exit_details_csv_path.string() + "' WITH (FORMAT CSV, HEADER, DELIMITER ',');";
    std::string create_tmp_exit_details_query = "CREATE TEMPORARY TABLE tmp_exit_details AS SELECT * FROM exit_details LIMIT 0;";
    std::cout << create_tmp_exit_details_query << '\n';
    if (mysql_query(con, create_tmp_exit_details_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    std::string load_tmp_exit_details_query = "LOAD DATA LOCAL INFILE '"
        + exit_details_csv_path.string()
        + "' INTO TABLE tmp_exit_details FIELDS TERMINATED BY ',' ENCLOSED BY '\"' LINES TERMINATED BY '\\n' IGNORE 1 ROWS;";
    std::cout << load_tmp_exit_details_query << '\n';
    if (mysql_query(con, load_tmp_exit_details_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    std::string insert_exits_query = "INSERT IGNORE INTO exits (SELECT station_name, name, bus_station_name AS textt FROM tmp_exit_details);";
    std::cout << insert_exits_query << '\n';
    if (mysql_query(con, insert_exits_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    std::string insert_exit_details_query = "INSERT IGNORE INTO exit_details (SELECT * FROM tmp_exit_details);";
    std::cout << insert_exit_details_query << '\n';
    if (mysql_query(con, insert_exit_details_query.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return 1;
    }

    gettimeofday(&end, NULL);
    time_spent = (double)(end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec)/1000000.0);
    std::cout << "\033[92mcsv to database completed in " << time_spent << " seconds.\033[0m" << '\n';
    return 0;

    printf("Data loaded successfully.\n");

    mysql_close(con);

    return 0;
}