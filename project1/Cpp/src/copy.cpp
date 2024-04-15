#include <iostream>
#include <fstream>
#include <pqxx/pqxx>
#include <sys/stat.h>
#include <cstdlib>
#include <sys/time.h>
#include "json.hpp"
#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

using json = nlohmann::json;

void printProgress(double percentage) {
    double val = percentage * 100;
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%4.1f%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush(stdout);
}


int main() {
    std::string data_path = getenv("DATA_PATH");

    // Database connection string
    std::string db_name = getenv("DB_NAME");
    std::string db_user = getenv("DB_USER");
    std::string db_password = getenv("DB_PASSWORD");
    std::string db_host = getenv("DB_HOST");
    std::string db_port = getenv("DB_PORT");
    std::string conn_str = "dbname=" + db_name + " user=" + db_user + " password=" + db_password + " host=" + db_host + " port=" + db_port;


    // timing
    // std::time_t start, end;
    struct timeval start, end;
    double time_spent;

    std::cout << "Parsing JSON files..." << std::endl;

    // start = std::time(nullptr);
    gettimeofday(&start, NULL);

    // Read from cards.json
    std::ifstream file(data_path + "/cards.json");
    json cards;
    file >> cards;

    // Read from passengers.json
    std::ifstream file2(data_path + "/passengers.json");
    json passengers;
    file2 >> passengers;

    // Read from rides.json
    std::ifstream file3(data_path + "/rides.json");
    json rides;
    file3 >> rides;

    // Read from lines.json
    std::ifstream file4(data_path + "/lines.json");
    json lines;
    file4 >> lines;

    // // Read from stations.json
    // std::ifstream file5(data_path + "/stations.json");
    // json stations;
    // file5 >> stations;

    // end = std::time(nullptr);   
    gettimeofday(&end, NULL);

    time_spent = (double)(end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec)/1000000.0);

    std::cout << "JSON parsing completed in " << time_spent << " seconds." << std::endl;

    // Connect to the database
    pqxx::connection c(conn_str);
    pqxx::work w(c);

    // // Create the cards table
    std::string create_cards_query = std::string("CREATE TABLE IF NOT EXISTS cards(")
        +"code VARCHAR(9) PRIMARY KEY CHECK (code ~ '^[0-9]{9}$'), "
        +"money MONEY NOT NULL, "
        +"create_time TIMESTAMP NOT NULL);";
    w.exec(create_cards_query);

    // // Create the passengers table
    std::string create_passengers_query = std::string("CREATE TABLE IF NOT EXISTS passengers (")
        +"name VARCHAR(255) NOT NULL, "
        +"id_number VARCHAR(18) PRIMARY KEY NOT NULL CHECK (id_number ~ '^[0-9]{17}[0-9X]?$'), "
        +"phone_number VARCHAR(11) NOT NULL CHECK (phone_number ~ '^[0-9]{11}$'), "
        +"gender VARCHAR(255) NOT NULL, "
        +"district VARCHAR(255) NOT NULL);";
    w.exec(create_passengers_query);

    // Create the rides table
    std::string create_rides_query = std::string("CREATE TABLE IF NOT EXISTS rides (")
        +"rail_user VARCHAR(255), "
        +"start_station VARCHAR(255), "
        +"end_station VARCHAR(255), "
        +"price DOUBLE PRECISION, "
        +"start_time TIMESTAMP, "
        +"end_time TIMESTAMP);";
    w.exec(create_rides_query);


    // Create the lines table
    std::string create_lines_query = std::string("CREATE TABLE IF NOT EXISTS lines (")
        +"line_id VARCHAR(255) PRIMARY KEY, "
        +"name VARCHAR(255), "
        +"start_time TIME, "
        +"end_time TIME, "
        +"mileage DOUBLE PRECISION, "
        +"color VARCHAR(255), "
        +"first_opening DATE, "
        +"url TEXT,"
        +"intro TEXT);";
    w.exec(create_lines_query);

    // Create the line_details table
    std::string create_line_details_query = std::string("CREATE TABLE IF NOT EXISTS line_details (")
        +"line_id VARCHAR(255), "
        +"station_name VARCHAR(255)"
        // +"station_id VARCHAR(255),"
        // +" FOREIGN KEY (line_id) REFERENCES lines(line_id), "
        // +"FOREIGN KEY (station_id) REFERENCES stations(station_id)"
        +");";
    w.exec(create_line_details_query);

    // Create the stations table
    std::string create_stations_query = std::string("CREATE TABLE IF NOT EXISTS stations (")
        +"station_id VARCHAR(255) PRIMARY KEY, "
        +"name VARCHAR(255), "
        +"district VARCHAR(255), "
        +"intro TEXT, "
        +"chinese_name VARCHAR(255));";
    w.exec(create_stations_query);

    // // Create the bus_station table
    // std::string create_bus_station_query = std::string("CREATE TABLE IF NOT EXISTS bus_station (")
    //     +"station_id VARCHAR(255), "
    //     +"name VARCHAR(255), "
    //     +"district VARCHAR(255));";
    // w.exec(create_bus_station_query);

    // // Create the bus_line table
    // std::string create_bus_line_query = std::string("CREATE TABLE IF NOT EXISTS bus_line (")
    //     +"bus_line_id VARCHAR(255), "
    //     +"name VARCHAR(255));";
    // w.exec(create_bus_line_query);

    // // Create the bus_line_details table
    // std::string create_bus_line_details_query = std::string("CREATE TABLE IF NOT EXISTS bus_line_details (")
    //     +"bus_line_id VARCHAR(255), "
    //     +"bus_station_name VARCHAR(255), "
    //     +"FOREIGN KEY (bus_line_id) REFERENCES bus_line(bus_line_id), "
    //     +"FOREIGN KEY (bus_station_name) REFERENCES bus_station(name));";
    // w.exec(create_bus_line_details_query);

    // // Create the exits table
    // std::string create_exits_query = std::string("CREATE TABLE IF NOT EXISTS exits (")
    //     +"exit_id VARCHAR(255), "
    //     +"name VARCHAR(255), "
    //     +"textt TEXT"
    //     +"PRIMARY KEY exit_id);";
    // w.exec(create_exits_query);

    // // Create the exits_details table
    // std::string create_exits_details_query = std::string("CREATE TABLE IF NOT EXISTS exits_details (")
    //     +"station_name VARCHAR(255), "
    //     +"name VARCHAR(255), "
    //     +"textt TEXT"
    //     +"PRIMARY KEY (station_name, name));";
    // w.exec(create_exits_details_query);

    // Commit the transaction
    w.commit();

    // Start timing
    // std::cout << "Inserting data into the database..." << std::endl;
    // start = std::time(nullptr);
    gettimeofday(&start, NULL);

    // Loop over each instance in the rides array and write to a csv file

    // size_t i, total;
    std::filesystem::path current_path = std::filesystem::current_path();

    std::cout << "Writing to cards.csv..." << std::endl;
    std::ofstream cards_csv("cards.csv");
    if (!cards_csv.is_open()) {
        std::cerr << "Error: could not open cards.csv" << std::endl;
        return 1;
    }
    std::filesystem::path cards_csv_path = current_path / "cards.csv";
    std::cout << "\tcards.csv path: " << cards_csv_path << std::endl;
    std::string cards_header = "code,money,create_time\n";
    cards_csv << cards_header;
    for (auto& card : cards) {
        // Extract data from JSON
        std::string code = card["code"];
        double money = card["money"];
        std::string create_time = card["create_time"];
        cards_csv << code << "," << money << "," << create_time << "\n";
    }
    cards_csv.close();


    std::cout << "Writing to passengers.csv..." << std::endl;
    std::ofstream passengers_csv("passengers.csv");
    if (!passengers_csv.is_open()) {
        std::cerr << "Error: could not open passengers.csv" << std::endl;
        return 1;
    }

    std::filesystem::path passengers_csv_path = current_path / "passengers.csv";
    std::cout << "\tpassengers.csv path: " << passengers_csv_path << std::endl;
    passengers_csv << "name,id_number,phone_number,gender,district\n";
    for (auto& passenger : passengers) {
        // Extract data from JSON
        std::string name = passenger["name"];
        std::string id_number = passenger["id_number"];
        std::string phone_number = passenger["phone_number"];
        std::string gender = passenger["gender"];
        std::string district = passenger["district"];
        passengers_csv << name << "," << id_number << "," << phone_number << "," << gender << "," << district << "\n";
    }
    passengers_csv.close(); 

    
    std::cout << "Writing to rides.csv..." << std::endl;
    std::ofstream rides_csv("rides.csv");
    if (!rides_csv.is_open()) {
        std::cerr << "Error: could not open rides.csv" << std::endl;
        return 1;
    }

    std::filesystem::path rides_csv_path = current_path / "rides.csv";
    std::cout << "\trides.csv path: " << rides_csv_path << std::endl;

    std::string rides_header = "rail_user,start_station,end_station,price,start_time,end_time\n";
    rides_csv << rides_header;
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
        str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
        rides_csv << str << "\n";
        // rides_csv << user << "," << start_station << "," << end_station << "," << price << "," << start_time << "," << end_time << "\n";
        // printProgress((double) ++i / total);
    }
    rides_csv.close();


    std::cout << "Writing to lines.csv..." << std::endl;
    std::ofstream lines_csv("lines.csv");

    std::cout << "Writing to line_details..." << std::endl;
    std::ofstream line_details_csv("line_details.csv");

    if (!lines_csv.is_open()||!line_details_csv.is_open()) {
        std::cerr << "Error: could not open csv" << std::endl;
        return 1;
    }
    std::filesystem::path lines_csv_path = current_path / "lines.csv";
    std::cout << "\tlines.csv path: " << lines_csv_path << std::endl;
    lines_csv << "line_id,name,start_time,end_time,mileage,color,first_opening,url,intro\n";

    std::filesystem::path line_details_csv_path = current_path / "line_details.csv";
    std::cout << "\tline_details.csv path: " << line_details_csv_path << std::endl;
    line_details_csv << "line_id,station_name\n";
    int id = 0;
    for (auto& line : lines.items()) {
        // std::cout << "Line: " << line.key() << std::endl;
        std::string line_id = std::to_string(id++);
        std::string name = line.key();
        
        std::string start_time = line.value()["start_time"];
        
        std::string end_time = line.value()["end_time"];
        std::string mileage = line.value()["mileage"];
        std::string color = line.value()["color"];
        std::string first_opening = line.value()["first_opening"];
        std::string url = line.value()["url"];
        std::string intro = line.value()["intro"];
        std::string line_str = line_id + "," + name + "," + start_time + "," + end_time + "," + mileage + "," + color + "," + first_opening + "," + url + "," + intro;
        line_str.erase(std::remove(line_str.begin(), line_str.end(), '\n'), line_str.end());
        lines_csv << line_str << "\n";
        
        for (const auto& station : line.value()["stations"]) {
            // std::cout << "  Station: " << station.get<std::string>() << std::endl;
            std::string line_details_str = line_id + "," + station.get<std::string>();
            
            line_details_str.erase(std::remove(line_details_str.begin(), line_details_str.end(), '\n'), line_details_str.end());
            line_details_csv << line_details_str << "\n";
        }
    }
    lines_csv.close();
    line_details_csv.close();

    // std::cout << "Writing to stations.csv..." << std::endl;
    // std::ofstream stations_csv("stations.csv");
    // if (!stations_csv.is_open()) {
    //     std::cerr << "Error: could not open stations.csv" << std::endl;
    //     return 1;
    // }
    // std::filesystem::path stations_csv_path = current_path / "stations.csv";
    // std::cout << "\tstations.csv path: " << stations_csv_path << std::endl;
    // stations_csv << "station_id,name,district,intro,chinese_name\n";


    // // Write to the database
    // const char* path = rides_csv_path.c_str();
    // // Change the permissions to read/write for user, read for group, and read for others.
    // if (chmod(path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) == -1) {
    //     std::perror("chmod");
    //     return 1;
    // }
    // w.exec("COPY rides FROM '" + rides_csv_path.string() + "' WITH (FORMAT CSV, HEADER, DELIMITER ',')");
    // w.commit();

    // end = std::time(nullptr);
    gettimeofday(&end, NULL);
    time_spent = (double)(end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec)/1000000.0);
    // std::cout << std::endl;
    std::cout << "json to csv completed in " << time_spent << " seconds." << std::endl;

    return 0;
}