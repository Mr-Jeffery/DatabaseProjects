#include <iostream>
#include <fstream>
#include <pqxx/pqxx>
#include <sys/stat.h>
#include <ctime>
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
    std::time_t start, end;

    std::cout << "Parsing JSON files..." << std::endl;

    start = std::time(nullptr);

    // // Read from cards.json
    // std::ifstream file(data_path + "/cards.json");
    // json cards;
    // file >> cards;

    // // Read from passengers.json
    // std::ifstream file2(data_path + "/passengers.json");
    // json passengers;
    // file2 >> passengers;

    // Read from rides.json
    std::ifstream file3(data_path + "/rides.json");
    json rides;
    file3 >> rides;

    end = std::time(nullptr);   

    std::cout << "JSON parsing completed in " << end - start << " seconds." << std::endl;

    // Connect to the database
    pqxx::connection c(conn_str);
    pqxx::work w(c);

    // // Create the cards table
    // std::string create_cards_query = "CREATE TABLE IF NOT EXISTS cards (code VARCHAR(255) PRIMARY KEY, money DOUBLE PRECISION, create_time TIMESTAMP);";
    // w.exec(create_cards_query);
    // w.commit();

    // // Create the passengers table
    // std::string create_passengers_query = "CREATE TABLE IF NOT EXISTS passengers (name VARCHAR(255), id_number VARCHAR(255) PRIMARY KEY, phone_number, gender VARCHAR(255), district VARCHAR(255))";
    // w.exec(create_passengers_query);
    // w.commit();

    // Create the rides table
    std::string create_rides_query = "CREATE TABLE IF NOT EXISTS rides (rail_user VARCHAR(255), start_station VARCHAR(255), end_station VARCHAR(255), price DOUBLE PRECISION, start_time TIMESTAMP, end_time TIMESTAMP);";
    w.exec(create_rides_query);
    // w.commit();

    // Start timing
    std::cout << "Inserting data into the database..." << std::endl;
    start = std::time(nullptr);

    // Loop over each instance in the rides array and write to a csv file
    size_t i, total;

    
    std::filesystem::path current_path = std::filesystem::current_path();
    std::ofstream rides_csv("rides.csv");
    if (!rides_csv.is_open()) {
        std::cerr << "Error: could not open rides.csv" << std::endl;
        return 1;
    }
    std::filesystem::path rides_csv_path = current_path / "rides.csv";
    std::cout << "rides.csv path: " << rides_csv_path << std::endl;
    std::cout << "Writing to rides.csv..." << std::endl;

    std::string header = "rail_user,start_station,end_station,price,start_time,end_time\n";
    rides_csv << header;
    i = 0;
    total = rides.size();
    printf("Total rides: %ld\n", total);
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
        printProgress((double) ++i / total);
    }
    rides_csv.close();


    // // Write to the database
    // const char* path = rides_csv_path.c_str();
    // // Change the permissions to read/write for user, read for group, and read for others.
    // if (chmod(path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) == -1) {
    //     std::perror("chmod");
    //     return 1;
    // }
    // w.exec("COPY rides FROM '" + rides_csv_path.string() + "' WITH (FORMAT CSV, HEADER, DELIMITER ',')");
    // w.commit();


    end = std::time(nullptr);
    std::cout << std::endl;
    std::cout << "Data insertion completed in " << end - start << " seconds." << std::endl;

    return 0;
}