#include <pqxx/pqxx>
#include "json2csv.hpp"

int main(){
    // Database connection string
    const char* db_name_cstr = getenv("DB_NAME");
    std::string db_name = db_name_cstr ? db_name_cstr : "postgres";

    const char* db_user_cstr = getenv("DB_USER");
    std::string db_user = db_user_cstr ? db_user_cstr : "postgres";

    const char* db_password_cstr = getenv("DB_PASSWORD");
    std::string db_password = db_password_cstr ? db_password_cstr : "";

    const char* db_host_cstr = getenv("DB_HOST");
    std::string db_host = db_host_cstr ? db_host_cstr : "localhost";

    const char* db_port_cstr = getenv("DB_PORT");
    std::string db_port = db_port_cstr ? db_port_cstr : "5432";

    std::string conn_str = "dbname=" + db_name + " user=" + db_user + " password=" + db_password + " host=" + db_host + " port=" + db_port;
    // Connect to the database
    pqxx::connection c(conn_str);
    pqxx::work w(c);
    
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

    // // Create the cards table
    std::string create_cards_query = std::string("CREATE TABLE IF NOT EXISTS cards(")
        +"\n\tcode VARCHAR(9) PRIMARY KEY CHECK (code ~ '^[0-9]{9}$'), "
        +"\n\tmoney MONEY NOT NULL, "
        +"\n\tcreate_time TIMESTAMP NOT NULL);";
    std::cout << create_cards_query << '\n';
    w.exec(create_cards_query);

    // // Create the passengers table
    std::string create_passengers_query = std::string("CREATE TABLE IF NOT EXISTS passengers (")
        +"\n\tname VARCHAR(255) NOT NULL, "
        +"\n\tid_number VARCHAR(18) PRIMARY KEY NOT NULL CHECK (id_number ~ '^[0-9]{17}[0-9X]?$'), "
        +"\n\tphone_number VARCHAR(11) NOT NULL CHECK (phone_number ~ '^[0-9]{11}$'), "
        +"\n\tgender VARCHAR(255) NOT NULL, "
        +"\n\tdistrict VARCHAR(255) NOT NULL);";
    std::cout << create_passengers_query << '\n';
    w.exec(create_passengers_query);
    

    // Create the lines table
    std::string create_lines_query = std::string("CREATE TABLE IF NOT EXISTS lines (")
        // +"line_id VARCHAR(255) , "
        +"\n\tname VARCHAR(255)PRIMARY KEY, "
        +"\n\tstart_time TIME, "
        +"\n\tend_time TIME, "
        +"\n\tmileage DOUBLE PRECISION, "
        +"\n\tcolor VARCHAR(255), "
        +"\n\tfirst_opening DATE, "
        +"\n\turl TEXT,"
        +"\n\tintro TEXT);";
    std::cout << create_lines_query << '\n';
    w.exec(create_lines_query);

    // Create the stations table
    std::string create_stations_query = std::string("CREATE TABLE IF NOT EXISTS stations (")
        // +"station_id VARCHAR(255) PRIMARY KEY, "
        +"\n\tname VARCHAR(255) PRIMARY KEY, "
        +"\n\tdistrict VARCHAR(255), "
        +"\n\tintro TEXT, "
        +"\n\tchinese_name VARCHAR(255));";
    std::cout << create_stations_query << '\n';
    w.exec(create_stations_query);

    // Create the rides table
    std::string create_rides_query = std::string("CREATE TABLE IF NOT EXISTS rides (")
        +"\n\trail_user VARCHAR(255), "
        +"\n\tstart_station VARCHAR(255), "
        +"\n\tend_station VARCHAR(255), "
        +"\n\tprice DOUBLE PRECISION, "
        +"\n\tstart_time TIMESTAMP, "
        +"\n\tend_time TIMESTAMP"
        +", \n\tFOREIGN KEY (start_station) REFERENCES stations(name), "
        +"\n\tFOREIGN KEY (end_station) REFERENCES stations(name)"
        +");";
    std::cout << create_rides_query << '\n';
    w.exec(create_rides_query);

    // Create the line_details table
    std::string create_line_details_query = std::string("CREATE TABLE IF NOT EXISTS line_details (")
        +"line_name VARCHAR(255), "
        +"station_name VARCHAR(255)"
        +", FOREIGN KEY (line_name) REFERENCES lines(name), "
        +"FOREIGN KEY (station_name) REFERENCES stations(name)"
        +");";
    std::cout << create_line_details_query << '\n';
    w.exec(create_line_details_query);

    // Create the bus_stations table
    std::string create_bus_stations_query = std::string("CREATE TABLE IF NOT EXISTS bus_stations (")
        // +"station_id VARCHAR(255), "
        +"name VARCHAR(255) PRIMARY KEY, "
        +"district VARCHAR(255));";
    std::cout << create_bus_stations_query << '\n';
    w.exec(create_bus_stations_query);

    // Create the bus_line table
    std::string create_bus_lines_query = std::string("CREATE TABLE IF NOT EXISTS bus_lines (")
        // +"bus_line_id VARCHAR(255), "
        +"name VARCHAR(255) PRIMARY KEY);";
    std::cout << create_bus_lines_query << '\n';
    w.exec(create_bus_lines_query);

    // Create the bus_line_details table
    std::string create_bus_line_details_query = std::string("CREATE TABLE IF NOT EXISTS bus_line_details (")
        +"\n\tbus_line_name VARCHAR(255), "
        +"\n\tbus_station_name VARCHAR(255), "
        // +"\n\tFOREIGN KEY (bus_line_name) REFERENCES bus_lines(name), "// this line is commented because the bus_lines itself is created by copying the bus_line_details
        +"\n\tFOREIGN KEY (bus_station_name) REFERENCES bus_stations(name)"
        +");";
    std::cout << create_bus_line_details_query << '\n';
    w.exec(create_bus_line_details_query);

    // Create the exits table
    std::string create_exits_query = std::string("CREATE TABLE IF NOT EXISTS exits (")
        +"\n\tstation_name VARCHAR(255), "
        +"\n\tname VARCHAR(255),"
        +"\n\ttextt TEXT,"
        +"\n\tFOREIGN KEY (station_name) REFERENCES stations(name),"
        +"\n\tPRIMARY KEY (station_name, name)"
        +");";
    std::cout << create_exits_query << '\n';
    w.exec(create_exits_query);

    // Create the exit_details table
    std::string create_exit_details_query = std::string("CREATE TABLE IF NOT EXISTS exit_details (")
        +"\n\tstation_name VARCHAR(255), "
        +"\n\tname VARCHAR(255), "
        +"\n\tbus_station_name VARCHAR(255)"
        +", \n\tFOREIGN KEY (station_name, name) REFERENCES exits(station_name,name), "
        +"\n\tFOREIGN KEY (bus_station_name) REFERENCES bus_stations(name) "
        +", \n\tPRIMARY KEY (station_name, name, bus_station_name)"
        +");";
    w.exec(create_exit_details_query);

    // Copy the csv files to the database
    gettimeofday(&start, NULL);
    std::string copy_cards_query = "COPY cards FROM '" + cards_csv_path.string() + "' WITH (FORMAT CSV, HEADER, DELIMITER ',');";
    std::cout << copy_cards_query << '\n';
    w.exec(copy_cards_query);

    std::string copy_passengers_query = "COPY passengers FROM '" + passengers_csv_path.string() + "' WITH (FORMAT CSV, HEADER, DELIMITER ',');";
    std::cout << copy_passengers_query << '\n';
    w.exec(copy_passengers_query);

    std::string copy_lines_query = "COPY lines FROM '" + lines_csv_path.string() + "' WITH (FORMAT CSV, HEADER, DELIMITER ',');";
    std::cout << copy_lines_query << '\n';
    w.exec(copy_lines_query);

    std::string copy_stations_query = "COPY stations FROM '" + stations_csv_path.string() + "' WITH (FORMAT CSV, HEADER, DELIMITER ',');";
    std::cout << copy_stations_query << '\n';
    w.exec(copy_stations_query);

    std::string copy_rides_query = "COPY rides FROM '" + rides_csv_path.string() + "' WITH (FORMAT CSV, HEADER, DELIMITER ',');";
    std::cout << copy_rides_query << '\n';
    w.exec(copy_rides_query);

    std::string copy_line_details_query = "COPY line_details FROM '" + line_details_csv_path.string() + "' WITH (FORMAT CSV, HEADER, DELIMITER ',');";
    std::cout << copy_line_details_query << '\n';
    w.exec(copy_line_details_query);

    std::string copy_exits_query = "COPY exits FROM '" + exits_csv_path.string() + "' WITH (FORMAT CSV, HEADER, DELIMITER ',');";
    std::cout << copy_exits_query << '\n';
    w.exec(copy_exits_query);
    
    std::string copy_bus_stations_query = std::string("CREATE TEMP TABLE tmp_bus_stations AS SELECT * FROM bus_stations LIMIT 0;")
    +"\n\tCOPY tmp_bus_stations FROM '"
    + bus_stations_csv_path.string()
    +"' WITH (FORMAT CSV, HEADER, DELIMITER ',');"
    +"\n\tINSERT INTO bus_stations SELECT * FROM tmp_bus_stations ON CONFLICT DO NOTHING;";
    std::cout << copy_bus_stations_query << '\n';
    w.exec(copy_bus_stations_query);

    // std::string copy_bus_line_details_query = "COPY bus_line_details FROM '" + bus_line_details_csv_path.string() + "' WITH (FORMAT CSV, HEADER, DELIMITER ',');";
    std::string copy_bus_line_details_query = std::string("CREATE TEMP TABLE tmp_bus_line_details AS SELECT * FROM bus_line_details LIMIT 0;")
    /*create a temp table based on bus_line_details*/
    +"\n\tCOPY tmp_bus_line_details FROM '"
    + bus_line_details_csv_path.string()
    +"' WITH (FORMAT CSV, HEADER, DELIMITER ',');"
    +"\n\tINSERT INTO bus_line_details (SELECT DISTINCT * FROM tmp_bus_line_details) ON CONFLICT DO NOTHING;";
    std::cout << copy_bus_line_details_query << '\n';
    w.exec(copy_bus_line_details_query);

    // the following query has been moved to be executed after data cleaning
    // std::string make_bus_line_query = "INSERT INTO bus_lines SELECT DISTINCT bus_line_name FROM bus_line_details;";
    // std::cout << make_bus_line_query << '\n';
    // w.exec(make_bus_line_query);

    // std::string copy_exit_details_query = "COPY exit_details FROM '" + exit_details_csv_path.string() + "' WITH (FORMAT CSV, HEADER, DELIMITER ',');";
    std::string copy_exit_details_query = std::string("CREATE TEMP TABLE tmp_exit_details AS SELECT * FROM exit_details LIMIT 0;")
    +"\n\tCOPY tmp_exit_details FROM '"
    + exit_details_csv_path.string()
    +"' WITH (FORMAT CSV, HEADER, DELIMITER ',');"
    +"\n\tINSERT INTO exits (SELECT station_name, name, bus_station_name AS textt FROM tmp_exit_details) ON CONFLICT DO NOTHING;"
    +"\n\tINSERT INTO exit_details (SELECT * FROM tmp_exit_details) ON CONFLICT DO NOTHING;";
    std::cout << copy_exit_details_query << '\n';
    w.exec(copy_exit_details_query);
    
    // Commit the transaction
    w.commit();

    gettimeofday(&end, NULL);
    time_spent = (double)(end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec)/1000000.0);
    std::cout << "\033[92mcsv to database completed in " << time_spent << " seconds.\033[0m" << '\n';
    return 0;
}