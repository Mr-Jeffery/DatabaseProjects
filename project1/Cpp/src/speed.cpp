/*Postgres version speed test, only loads fakerides.json, works only locally*/
#include <iostream>
#include <fstream>
#include <pqxx/pqxx>
#include <sys/time.h>
#include "json.hpp"

using json = nlohmann::json;


int main() {
    const char* data_path_cstr = getenv("DATA_PATH");
    std::string data_path = data_path_cstr ? data_path_cstr : "";

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

    // CSV path, /tmp by default
    const char* csv_path_ctsr = getenv("CSV_PATH");
    std::string csv_path = csv_path_ctsr ? csv_path_ctsr : "./";
    const std::filesystem::path current_path = std::filesystem::path(csv_path);



    // timing
    // std::time_t start, end;
    struct timeval start, end;
    double time_spent;

    std::cout << "Parsing JSON files..." << '\n';

    // start = std::time(nullptr);
    gettimeofday(&start, NULL);

    // Read from rides.json
    std::ifstream file(data_path + "/fakerides.json");
    json rides;
    file >> rides;


    // end = std::time(nullptr);   
    gettimeofday(&end, NULL);

    time_spent = (double)(end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec)/1000000.0);

    std::cout << "\033[92mJSON parsing completed in " << time_spent << " seconds.\033[0m" << '\n';

    // Connect to the database
    pqxx::connection c(conn_str);
    pqxx::work w(c);

    // Create the rides table
    std::string create_rides_query = std::string("CREATE TABLE IF NOT EXISTS rides (")
        +"rail_user VARCHAR(255), "
        +"start_station VARCHAR(255), "
        +"end_station VARCHAR(255), "
        +"price DOUBLE PRECISION, "
        +"start_time TIMESTAMP, "
        +"end_time TIMESTAMP"
        // +", FOREIGN KEY (start_station) REFERENCES stations(name), "
        // +"FOREIGN KEY (end_station) REFERENCES stations(name)"
        +");";
    std::cout << create_rides_query << '\n';
    w.exec(create_rides_query);

    gettimeofday(&start, NULL);
    // Create the rides table  
    std::cout << "Writing to rides.csv..." << '\n';
    std::filesystem::path rides_csv_path = current_path / "fakerides.csv";
    std::ofstream rides_csv(rides_csv_path);
    if (!rides_csv.is_open()) {
        std::cerr << "Error: could not open fakerides.csv" << '\n';
        return 1;
    }

    
    std::cout << "\t fakerides.csv path: " << rides_csv_path << '\n';

    rides_csv << "rail_user,start_station,end_station,price,start_time,end_time" << '\n';
    // i = 0;
    // total = rides.size();
    // printf("Total rides: %ld\n", total);
    for (auto& ride : rides) {
        // Extract data from JSON
        std::string user = ride["user"];
        std::string start_station = ride["start_station"];
        std::string end_station = ride["end_station"];
        double price = ride["price"];
        std::string start_time = ride["start_time"];
        std::string end_time = ride["end_time"];
        std::string str = user + "," + start_station + "," + end_station + "," + std::to_string(price) + "," + start_time + "," + end_time;
        rides_csv << str << '\n';
    }
    rides_csv.close();

    // end = std::time(nullptr);
    gettimeofday(&end, NULL);
    time_spent = (double)(end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec)/1000000.0);
    std::cout << "\033[92mjson to csv completed in " << time_spent << " seconds.\033[0m" << '\n';


    // Copy the csv files to the database
    gettimeofday(&start, NULL);
    std::string copy_rides_query = "COPY rides FROM '" + rides_csv_path.string() + "' WITH (FORMAT CSV, HEADER, DELIMITER ',');";
    std::cout << copy_rides_query << '\n';
    w.exec(copy_rides_query);
    
    // Commit the transaction
    w.commit();

    gettimeofday(&end, NULL);
    time_spent = (double)(end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec)/1000000.0);
    std::cout << "\033[92mcsv to database completed in " << time_spent << " seconds.\033[0m" << '\n';
    return 0;
}