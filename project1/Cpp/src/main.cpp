#include <iostream>
#include <fstream>
#include <pqxx/pqxx>
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
    w.commit();

    // Start timing
    std::cout << "Inserting data into the database..." << std::endl;
    std::time_t start = std::time(nullptr);

    // Prepare the SQL statement
    try {
        // c.prepare("insert_card", "INSERT INTO cards (code, money, create_time) VALUES ($1, $2, $3)");
        // c.prepare("insert_passenger", "INSERT INTO passengers (name, id_number, phone_number, gender, district) VALUES ($1, $2, $3, $4, $5)");
        c.prepare("insert_ride", "INSERT INTO rides (rail_user, start_station, end_station, price, start_time, end_time) VALUES ($1, $2, $3, $4, $5, $6)");
    }
    catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
    }
    size_t i;
    size_t total;
    // // Loop over each instance in the cards array
    // i = 0;
    // total = cards.size();
    // for (auto& card : cards) {
    //     // Extract data from JSON
    //     std::string code = card["code"];
    //     double money = card["money"];
    //     std::string create_time = card["create_time"];
    //     try {
    //         // Execute the prepared statement
    //         w.exec_prepared("insert_card", code, money, create_time);
    //         w.commit();
    //         printProgress((double) ++i / total);
    //     }
    //     catch (const std::exception &e) {
    //         std::cerr << e.what() << std::endl;
    //     }
    // }

    // // Loop over each instance in the passengers array
    // i = 0;
    // total = passengers.size();
    // for (auto& passenger : passengers) {
    //     // Extract data from JSON
    //     std::string name = passenger["name"];
    //     std::string id_number = passenger["id_number"];
    //     std::string phone = passenger["phone_number"];
    //     std::string gender = passenger["gender"];
    //     std::string district = passenger["district"];
    //     try {
    //         // Execute the prepared statement
    //         w.exec_prepared("insert_passenger", name, id_number, phone, gender, district);
    //         w.commit();
    //         printProgress((double) ++i / total);
    //     }
    //     catch (const std::exception &e) {
    //         std::cerr << e.what() << std::endl;
    //     }
    // }

    // Loop over each instance in the rides array
    
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
        try {
            // Execute the prepared statement
            w.exec_prepared("insert_ride", user, start_station, end_station, price, start_time, end_time);
            // w.commit();
            printProgress((double) ++i / total);
        }
        catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
    }
    w.commit();

    std::time_t end = std::time(nullptr);
    std::cout << std::endl;
    std::cout << "Data insertion completed in " << end - start << " seconds." << std::endl;

    return 0;
}
