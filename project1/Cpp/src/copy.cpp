#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <pqxx/pqxx>
#include <cstdlib>
#include <sys/time.h>
#include <algorithm>
#include <string>
#include <locale>
#include <codecvt>
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

// std::wstring utf8_to_wstring(const std::string& str) {
//     std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
//     return converter.from_bytes(str);
// }

// std::string wstring_to_utf8(const std::wstring& wstr) {
//     std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
//     return converter.to_bytes(wstr);
// }

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
    std::string csv_path = csv_path_ctsr ? csv_path_ctsr : "?";
    const std::filesystem::path current_path = std::filesystem::path(csv_path);

    // std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    // Chinese string
    const std::string chu1_ru4_kou3 = "出入口";

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

    // Read from stations.json
    std::ifstream file5(data_path + "/stations.json");
    json stations;
    file5 >> stations;

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
    std::cout << create_cards_query << std::endl;
    w.exec(create_cards_query);

    // // Create the passengers table
    std::string create_passengers_query = std::string("CREATE TABLE IF NOT EXISTS passengers (")
        +"name VARCHAR(255) NOT NULL, "
        +"id_number VARCHAR(18) PRIMARY KEY NOT NULL CHECK (id_number ~ '^[0-9]{17}[0-9X]?$'), "
        +"phone_number VARCHAR(11) NOT NULL CHECK (phone_number ~ '^[0-9]{11}$'), "
        +"gender VARCHAR(255) NOT NULL, "
        +"district VARCHAR(255) NOT NULL);";
    std::cout << create_passengers_query << std::endl;
    w.exec(create_passengers_query);

    // Create the rides table
    std::string create_rides_query = std::string("CREATE TABLE IF NOT EXISTS rides (")
        +"rail_user VARCHAR(255), "
        +"start_station VARCHAR(255), "
        +"end_station VARCHAR(255), "
        +"price DOUBLE PRECISION, "
        +"start_time TIMESTAMP, "
        +"end_time TIMESTAMP);";
    std::cout << create_rides_query << std::endl;
    w.exec(create_rides_query);
    


    // Create the lines table
    std::string create_lines_query = std::string("CREATE TABLE IF NOT EXISTS lines (")
        // +"line_id VARCHAR(255) , "
        +"name VARCHAR(255)PRIMARY KEY, "
        +"start_time TIME, "
        +"end_time TIME, "
        +"mileage DOUBLE PRECISION, "
        +"color VARCHAR(255), "
        +"first_opening DATE, "
        +"url TEXT,"
        +"intro TEXT);";
    std::cout << create_lines_query << std::endl;
    w.exec(create_lines_query);

    // Create the stations table
    std::string create_stations_query = std::string("CREATE TABLE IF NOT EXISTS stations (")
        // +"station_id VARCHAR(255) PRIMARY KEY, "
        +"name VARCHAR(255) PRIMARY KEY, "
        +"district VARCHAR(255), "
        +"intro TEXT, "
        +"chinese_name VARCHAR(255));";
    std::cout << create_stations_query << std::endl;
    w.exec(create_stations_query);

    // Create the line_details table
    std::string create_line_details_query = std::string("CREATE TABLE IF NOT EXISTS line_details (")
        +"line_name VARCHAR(255), "
        +"station_name VARCHAR(255),"
        // +"station_id VARCHAR(255),"
        +"FOREIGN KEY (line_name) REFERENCES lines(name), "
        +"FOREIGN KEY (station_name) REFERENCES stations(name)"
        +");";
    std::cout << create_line_details_query << std::endl;
    w.exec(create_line_details_query);

    // Create the bus_stations table
    std::string create_bus_stations_query = std::string("CREATE TABLE IF NOT EXISTS bus_stations (")
        // +"station_id VARCHAR(255), "
        +"name VARCHAR(255) PRIMARY KEY, "
        +"district VARCHAR(255));";
    std::cout << create_bus_stations_query << std::endl;
    w.exec(create_bus_stations_query);

    // Create the bus_line table
    std::string create_bus_lines_query = std::string("CREATE TABLE IF NOT EXISTS bus_lines (")
        // +"bus_line_id VARCHAR(255), "
        +"name VARCHAR(255) PRIMARY KEY);";
    std::cout << create_bus_lines_query << std::endl;
    w.exec(create_bus_lines_query);

    // Create the bus_line_details table
    std::string create_bus_line_details_query = std::string("CREATE TABLE IF NOT EXISTS bus_line_details (")
        +"bus_line_name VARCHAR(255), "
        +"bus_station_name VARCHAR(255)"
        // +", FOREIGN KEY (bus_line_name) REFERENCES bus_lines(name), "
        // +"FOREIGN KEY (bus_station_name) REFERENCES bus_stations(name)"
        +");";
    std::cout << create_bus_line_details_query << std::endl;
    w.exec(create_bus_line_details_query);

    // Create the exits table
    std::string create_exits_query = std::string("CREATE TABLE IF NOT EXISTS exits (")
        +"station_name VARCHAR(255), "
        +"name VARCHAR(255),"
        +"textt TEXT,"
        +"FOREIGN KEY (station_name) REFERENCES stations(name),"
        +"PRIMARY KEY (station_name,name)"
        +");";
        
    std::cout << create_exits_query << std::endl;
    w.exec(create_exits_query);

    // Create the exit_details table
    std::string create_exit_details_query = std::string("CREATE TABLE IF NOT EXISTS exit_details (")
        +"station_name VARCHAR(255), "
        +"name VARCHAR(255), "
        +"bus_station_name VARCHAR(255)"
        // +", FOREIGN KEY (station_name, name) REFERENCES exits(station_name,name), "
        // +"FOREIGN KEY (bus_station_name) REFERENCES bus_stations(name) "
        // +",PRIMARY KEY ((station_name, name)，bus_station_name)"
        +");";
    w.exec(create_exit_details_query);


    // Commit the transaction
    // w.commit();

    // Start timing
    // std::cout << "Inserting data into the database..." << std::endl;
    // start = std::time(nullptr);
    gettimeofday(&start, NULL);

    // Loop over each instance in the rides array and write to a csv file

    // size_t i, total;
    // std::filesystem::path current_path = std::filesystem::current_path();
    // std::filesystem::path current_path = std::filesystem::path("/tmp");
    

    std::cout << "Writing to cards.csv..." << std::endl;
    std::filesystem::path cards_csv_path = current_path / "cards.csv";
    std::ofstream cards_csv(cards_csv_path);
    if (!cards_csv.is_open()) {
        std::cerr << "Error: could not open cards.csv" << std::endl;
        return 1;
    }
    
    std::cout << "\tcards.csv path: " << cards_csv_path << std::endl;
    cards_csv << "code,money,create_time" << std::endl;
    for (auto& card : cards) {
        // Extract data from JSON
        std::string code = card["code"];
        double money = card["money"];
        std::string create_time = card["create_time"];
        cards_csv << code << "," << money << "," << create_time << std::endl;
    }
    cards_csv.close();



    std::cout << "Writing to passengers.csv..." << std::endl;
    std::filesystem::path passengers_csv_path = current_path / "passengers.csv";
    std::ofstream passengers_csv(passengers_csv_path);
    if (!passengers_csv.is_open()) {
        std::cerr << "Error: could not open passengers.csv" << std::endl;
        return 1;
    }

    
    std::cout << "\tpassengers.csv path: " << passengers_csv_path << std::endl;
    passengers_csv << "name,id_number,phone_number,gender,district" << std::endl;
    for (auto& passenger : passengers) {
        // Extract data from JSON
        std::string name = passenger["name"];
        std::string id_number = passenger["id_number"];
        std::string phone_number = passenger["phone_number"];
        std::string gender = passenger["gender"];
        std::string district = passenger["district"];
        passengers_csv << name << "," << id_number << "," << phone_number << "," << gender << "," << district << std::endl;
    }
    passengers_csv.close(); 


    
    std::cout << "Writing to rides.csv..." << std::endl;
    std::filesystem::path rides_csv_path = current_path / "rides.csv";
    std::ofstream rides_csv(rides_csv_path);
    if (!rides_csv.is_open()) {
        std::cerr << "Error: could not open rides.csv" << std::endl;
        return 1;
    }

    
    std::cout << "\trides.csv path: " << rides_csv_path << std::endl;

    rides_csv << "rail_user,start_station,end_station,price,start_time,end_time" << std::endl;
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
        // str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
        // rides_csv << str << "\n";
        rides_csv << str << std::endl;
        // rides_csv << user << "," << start_station << "," << end_station << "," << price << "," << start_time << "," << end_time << "\n";
        // printProgress((double) ++i / total);
    }
    rides_csv.close();



    std::cout << "Writing to lines.csv..." << std::endl;
    std::filesystem::path lines_csv_path = current_path / "lines.csv";
    std::ofstream lines_csv(lines_csv_path);
    std::cout << "\tlines.csv path: " << lines_csv_path << std::endl;
    lines_csv << "name,start_time,end_time,mileage,color,first_opening,url,intro" << std::endl;



    std::cout << "Writing to line_details..." << std::endl;
    std::filesystem::path line_details_csv_path = current_path / "line_details.csv";
    std::ofstream line_details_csv(line_details_csv_path);
    std::cout << "\tline_details.csv path: " << line_details_csv_path << std::endl;
    line_details_csv << "line_name,station_name" << std::endl;
    // int id;
    // id = 0;
    if (!lines_csv.is_open()||!line_details_csv.is_open()) {
        std::cerr << "Error: could not open csv" << std::endl;
        return 1;
    }

    for (auto& line : lines.items()) {
        // std::cout << "Line: " << line.key() << std::endl;
        // std::string line_id = std::to_string(id++);
        std::string name = line.key();
        
        std::string start_time = line.value()["start_time"];
        
        std::string end_time = line.value()["end_time"];
        std::string mileage = line.value()["mileage"];
        std::string color = line.value()["color"];
        std::string first_opening = line.value()["first_opening"];
        std::string url = line.value()["url"];
        std::string intro = line.value()["intro"];
        std::string line_str = name + "," + start_time + "," + end_time + "," + mileage + "," + color + "," + first_opening + "," + url + "," + intro;
        line_str.erase(std::remove(line_str.begin(), line_str.end(), '\n'), line_str.end());
        lines_csv << line_str << std::endl;
        
        for (const auto& station : line.value()["stations"]) {
            // std::cout << "  Station: " << station.get<std::string>() << std::endl;
            std::string line_details_str = name + "," + station.get<std::string>();
            
            line_details_str.erase(std::remove(line_details_str.begin(), line_details_str.end(), '\n'), line_details_str.end());
            line_details_csv << line_details_str << std::endl;
        }
    }
    lines_csv.close();
    line_details_csv.close();



    std::cout << "Writing to stations.csv..." << std::endl;
    std::filesystem::path stations_csv_path = current_path / "stations.csv";
    std::ofstream stations_csv(stations_csv_path);
    std::cout << "\tstations.csv path: " << stations_csv_path << std::endl;
    stations_csv << "name,district,intro,chinese_name"<< std::endl;

    std::cout << "Writing to bus_stations.csv..." << std::endl;
    std::filesystem::path bus_stations_csv_path = current_path / "bus_stations.csv";
    std::ofstream bus_stations_csv(bus_stations_csv_path);
    std::cout << "\tbus_stations.csv path: " << bus_stations_csv_path << std::endl;
    bus_stations_csv << "name,district" << std::endl;

    std::cout << "Writing to exits.csv..." << std::endl;
    std::filesystem::path exits_csv_path = current_path / "exits.csv";
    std::ofstream exits_csv(exits_csv_path);
    std::cout << "\texits.csv path: " << exits_csv_path << std::endl;
    exits_csv << "station_name,name,textt" << std::endl;

    std::cout << "Writing to bus_line_details.csv..." << std::endl;
    std::filesystem::path bus_line_details_csv_path = current_path / "bus_line_details.csv";
    std::ofstream bus_line_details_csv(bus_line_details_csv_path);
    std::cout << "\tbus_line_details.csv path: " << bus_line_details_csv_path << std::endl;
    bus_line_details_csv << "bus_line_name,station_name" << std::endl;  
    
    std::cout << "Writing to exit_details.csv..." << std::endl;
    std::filesystem::path exit_details_csv_path = current_path / "exit_details.csv";
    std::ofstream exit_details_csv(exit_details_csv_path);
    std::cout << "\texit_details.csv path: " << exit_details_csv_path << std::endl;

    if (!bus_line_details_csv.is_open()||!exits_csv.is_open()||!stations_csv.is_open()||!bus_stations_csv.is_open()||!exit_details_csv.is_open()){
        std::cerr << "Error: could not open csv" << std::endl;
        return 1;
    }
    
    for (auto& station : stations.items()) {
        // Extract data from JSON
        // std::string station_id = std::to_string(id++);
        std::string name = station.key();
        std::string district = station.value()["district"];
        std::string intro = station.value()["intro"];
        // Replace commas with Chinese commas, using wstring because the Chinese characters are longer than 1 byte
        std::wstring intro_wstr = converter.from_bytes(intro);
        std::replace(intro_wstr.begin(), intro_wstr.end(), L',', L'，');
        intro = converter.to_bytes(intro_wstr);

        std::string chinese_name = station.value()["chinese_name"];
        std::string station_str = name + "," + district + "," + intro + "," + chinese_name;
        station_str.erase(std::remove(station_str.begin(), station_str.end(), '\n'), station_str.end());
        stations_csv << station_str << std::endl;

        // add exits for each station
        for (const auto& exit : station.value()["out_info"]) {
            if (!exit.contains("textt") && exit["textt"].empty()) {
                continue;
            }
            std::string exit_name = exit["outt"];
            // exit_name.erase(std::remove_if(exit_name.begin(), exit_name.end(), [](unsigned char c) { return !std::isalnum(c); }), exit_name.end());
            // std::wstring exit_name_wstr = converter.from_bytes(exit_name);

            // Remove all spaces, both chinese and english, they are not the same!!!!!
            // exit_name_wstr.erase(std::remove(exit_name_wstr.begin(), exit_name_wstr.end(), L' '), exit_name_wstr.end());
            // exit_name_wstr.erase(std::remove(exit_name_wstr.begin(), exit_name_wstr.end(), L' '), exit_name_wstr.end());
            // size_t pos = exit_name.find(chu1_ru4_kou3);
            // size_t pos = exit_name.find("出");
            // if (pos != std::wstring::npos) {
            //     exit_name.replace(pos, chu1_ru4_kou3.length(), "");
            // }

            std::string exit_text = exit["textt"];
            std::wstring exit_text_wstr = converter.from_bytes(exit_text);
            std::replace(exit_text_wstr.begin(), exit_text_wstr.end(), L',', L'、');
            std::replace(exit_text_wstr.begin(), exit_text_wstr.end(), L'，', L'、');
            exit_text = converter.to_bytes(exit_text_wstr);

            std::string exit_str = name + "," + exit_name + "," + exit_text;
            exit_str.erase(std::remove(exit_str.begin() + name.size() - 1, exit_str.end(), ' '), exit_str.end());
            exits_csv << exit_str << std::endl;
        }
        
        // add bus stations for each station
        for (const auto& bus_station : station.value()["bus_info"]) {
            if (!bus_station.contains("busOutInfo") || bus_station["busOutInfo"].empty()) {
                continue;// skip stations without bus info
            }
            std::string bus_station_name = bus_station["busOutInfo"][0]["busName"];
            if (bus_station_name.empty()) {// skip empty bus station names
                continue;
            }
            // std::cout << bus_station_name << std::endl; 
            std::string bus_station_str = bus_station_name + "," + district;
            bus_station_str.erase(std::remove(bus_station_str.begin(), bus_station_str.end(), '\n'), bus_station_str.end());
            bus_stations_csv << bus_station_str << std::endl;

            // add bus line details for each bus station
            std::string bus_line_name = bus_station["busOutInfo"][0]["busInfo"].get<std::string>();
            std::wstring bus_line_details = converter.from_bytes(bus_line_name);
            size_t pos;

            // while ((pos = bus_line_details.find(L',')) != std::wstring::npos) {
            //     bus_line_details.replace(pos, 1, L"、");
            // }

            // while ((pos = bus_line_details.find(L'，')) != std::wstring::npos) {
            //     bus_line_details.replace(pos, 1, L"、");
            // }

            // while ((pos = bus_line_details.find(L'路')) != std::wstring::npos) {
            //     bus_line_details.replace(pos, 1, L" ");
            // }
            while ((pos = bus_line_details.find_first_of(L",，路")) != std::wstring::npos) {
                if (bus_line_details[pos] == L'路') {
                    bus_line_details.replace(pos, 1, L" ");
                } else {
                    bus_line_details.replace(pos, 1, L"、");
                }
            }
            std::wstringstream wss(bus_line_details);
            std::wstring token;
            std::vector<std::string> bus_line_details_arr;
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

            while (std::getline(wss, token, L'、')) {
                if (!token.empty()) {\
                    bus_line_details_arr.push_back(converter.to_bytes(token));
                }
            }
            std::string bus_line_str;
            for (const auto& bus_line : bus_line_details_arr) {
                // bus_line_str = bus_line_str + bus_line + "," + bus_station_name + "\n";
                bus_line_str = bus_line + "," + bus_station_name;
                // bus_line_str.erase(std::remove(bus_line_str.begin(), bus_line_str.end(), '\n'), bus_line_str.end());
                bus_line_str.erase(std::remove(bus_line_str.begin(), bus_line_str.end(), ' '), bus_line_str.end());
                bus_line_details_csv << bus_line_str << std::endl;
            }
            // bus_line_str.erase(std::remove(bus_line_str.begin(), bus_line_str.end(), ' '), bus_line_str.end());
            // bus_line_details_csv << bus_line_str;

            // add exits details for each bus station
            std::string exit_name = bus_station["chukou"];
            if (exit_name.empty()) {
                continue;
            }
            std::string exit_details_str = name + "," + exit_name + "," + bus_station_name;
            exit_details_str.erase(std::remove(exit_details_str.begin() + name.size() - 1, exit_details_str.end(), ' '), exit_details_str.end());
            exit_details_csv << exit_details_str << std::endl;
            
        }



    }
    stations_csv.close();
    exits_csv.close();
    bus_stations_csv.close();
    bus_line_details_csv.close();
    exit_details_csv.close();

    // end = std::time(nullptr);
    gettimeofday(&end, NULL);
    time_spent = (double)(end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec)/1000000.0);
    std::cout << "json to csv completed in " << time_spent << " seconds." << std::endl;


    // Copy the csv files to the database
    gettimeofday(&start, NULL);
    std::string copy_cards_query = "COPY cards FROM '" + cards_csv_path.string() + "' WITH (FORMAT CSV, HEADER, DELIMITER ',');";
    std::cout << copy_cards_query << std::endl;
    w.exec(copy_cards_query);

    std::string copy_passengers_query = "COPY passengers FROM '" + passengers_csv_path.string() + "' WITH (FORMAT CSV, HEADER, DELIMITER ',');";
    std::cout << copy_passengers_query << std::endl;
    w.exec(copy_passengers_query);

    std::string copy_rides_query = "COPY rides FROM '" + rides_csv_path.string() + "' WITH (FORMAT CSV, HEADER, DELIMITER ',');";
    std::cout << copy_rides_query << std::endl;
    w.exec(copy_rides_query);

    std::string copy_lines_query = "COPY lines FROM '" + lines_csv_path.string() + "' WITH (FORMAT CSV, HEADER, DELIMITER ',');";
    std::cout << copy_lines_query << std::endl;
    w.exec(copy_lines_query);

    std::string copy_stations_query = "COPY stations FROM '" + stations_csv_path.string() + "' WITH (FORMAT CSV, HEADER, DELIMITER ',');";
    std::cout << copy_stations_query << std::endl;
    w.exec(copy_stations_query);

    std::string copy_line_details_query = "COPY line_details FROM '" + line_details_csv_path.string() + "' WITH (FORMAT CSV, HEADER, DELIMITER ',');";
    std::cout << copy_line_details_query << std::endl;
    w.exec(copy_line_details_query);

    std::string copy_exits_query = "COPY exits FROM '" + exits_csv_path.string() + "' WITH (FORMAT CSV, HEADER, DELIMITER ',');";
    std::cout << copy_exits_query << std::endl;
    w.exec(copy_exits_query);
    
    std::string copy_bus_stations_query = std::string("CREATE TEMP TABLE tmp_bus_stations AS SELECT * FROM bus_stations LIMIT 0;")
    +"COPY tmp_bus_stations FROM '"
    + bus_stations_csv_path.string()
    +"' WITH (FORMAT CSV, HEADER, DELIMITER ',');"
    +"INSERT INTO bus_stations SELECT * FROM tmp_bus_stations ON CONFLICT DO NOTHING;";
    std::cout << copy_bus_stations_query << std::endl;
    w.exec(copy_bus_stations_query);

    std::string make_bus_line_query = "INSERT INTO bus_lines SELECT DISTINCT bus_line_name FROM bus_line_details;";
    std::cout << make_bus_line_query << std::endl;
    w.exec(make_bus_line_query);

    std::string copy_bus_line_details_query = "COPY bus_line_details FROM '" + bus_line_details_csv_path.string() + "' WITH (FORMAT CSV, HEADER, DELIMITER ',');";
    std::cout << copy_bus_line_details_query << std::endl;
    w.exec(copy_bus_line_details_query);

    // std::string copy_exit_details_query = "COPY exit_details FROM '" + exit_details_csv_path.string() + "' WITH (FORMAT CSV, HEADER, DELIMITER ',');";
    std::string copy_exit_details_query = std::string("CREATE TEMP TABLE tmp_exit_details AS SELECT * FROM exit_details LIMIT 0;")
    +"COPY tmp_exit_details FROM '"
    + exit_details_csv_path.string()
    +"' WITH (FORMAT CSV, HEADER, DELIMITER ',');"
    +"INSERT INTO exit_details SELECT * FROM tmp_exit_details ON CONFLICT DO NOTHING;";
    std::cout << copy_exit_details_query << std::endl;
    w.exec(copy_exit_details_query);

    // Commit the transaction

    w.commit();

    gettimeofday(&end, NULL);
    time_spent = (double)(end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec)/1000000.0);
    std::cout << "csv to database completed in " << time_spent << " seconds." << std::endl;

    return 0;
}