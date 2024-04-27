/*Postgres version speed test, only loads fakerides.json, works only locally*/
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include "json.hpp"

using json = nlohmann::json;


int main() {
    const char* data_path_cstr = getenv("DATA_PATH");
    std::string data_path = data_path_cstr ? data_path_cstr : "";

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
    return 0;
}