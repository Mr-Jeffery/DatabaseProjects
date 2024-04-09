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

    // Read the JSON file
    std::ifstream file(data_path + "/cards.json");
    json cards;
    file >> cards;

    pqxx::connection c(conn_str);
    pqxx::work w(c);
    std::string create_table_query = "CREATE TABLE IF NOT EXISTS cards (code VARCHAR(255) PRIMARY KEY, money DOUBLE PRECISION, create_time TIMESTAMP)";
    w.exec(create_table_query);
    w.commit();

    // Start timing
    std::cout << "Inserting data into the database..." << std::endl;
    std::time_t start = std::time(nullptr);

    // Prepare the SQL statement
    try {
        c.prepare("insert_card", "INSERT INTO cards (code, money, create_time) VALUES ($1, $2, $3)");
    }
    catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
    }

    // Loop over each instance in the JSON array
    size_t i = 0;
    size_t total = cards.size();
    for (auto& card : cards) {
        // Extract data from JSON
        std::string code = card["code"];
        double money = card["money"];
        std::string create_time = card["create_time"];
        try {
            // Execute the prepared statement
            w.exec_prepared("insert_card", code, money, create_time);
            w.commit();
            printProgress((double) ++i / total);
        }
        catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
    }

    std::time_t end = std::time(nullptr);
    std::cout << std::endl;
    std::cout << "Data insertion completed in " << end - start << " seconds." << std::endl;

    return 0;
}
