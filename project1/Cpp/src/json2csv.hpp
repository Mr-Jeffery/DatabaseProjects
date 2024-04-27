/* file for loading json into csv, 
will be used by both postgres and mysql*/
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <cstdlib>
#include <sys/time.h>
#include <string>
#include <locale>
#include <codecvt>
#include "json.hpp"

using json = nlohmann::json;

std::filesystem::path json2csv() {
    const char* data_path_cstr = getenv("DATA_PATH");
    std::string data_path = data_path_cstr ? data_path_cstr : "";

    // CSV path, /tmp by default
    const char* csv_path_ctsr = getenv("CSV_PATH");
    std::string csv_path = csv_path_ctsr ? csv_path_ctsr : "?";
    const std::filesystem::path current_path = std::filesystem::path(csv_path);

    // std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    // Chinese string
    // const std::string chu1_ru4_kou3 = "出入口";

    // timing
    // std::time_t start, end;
    struct timeval start, end;
    double time_spent;

    std::cout << "Parsing JSON files..." << '\n';

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

    std::cout << "\033[92mJSON parsing completed in " << time_spent << " seconds.\033[0m" << '\n';


    // Commit the transaction
    // w.commit();

    // Start timing
    // std::cout << "Inserting data into the database..." << '\n';
    // start = std::time(nullptr);
    gettimeofday(&start, NULL);

    // Loop over each instance in the rides array and write to a csv file

    // size_t i, total;
    // std::filesystem::path current_path = std::filesystem::current_path();
    // std::filesystem::path current_path = std::filesystem::path("/tmp");
    

    std::cout << "Writing to cards.csv..." << '\n';
    std::filesystem::path cards_csv_path = current_path / "cards.csv";
    std::ofstream cards_csv(cards_csv_path);
    if (!cards_csv.is_open()) {
        std::cerr << "Error: could not open cards.csv" << '\n';
        return std::filesystem::path("");;
    }
    
    std::cout << "\tcards.csv path: " << cards_csv_path << '\n';
    cards_csv << "code,money,create_time" << '\n';
    for (auto& card : cards) {
        // Extract data from JSON
        std::string code = card["code"];
        double money = card["money"];
        std::string create_time = card["create_time"];
        cards_csv << code << "," << money << "," << create_time << '\n';
    }
    cards_csv.close();



    std::cout << "Writing to passengers.csv..." << '\n';
    std::filesystem::path passengers_csv_path = current_path / "passengers.csv";
    std::ofstream passengers_csv(passengers_csv_path);
    if (!passengers_csv.is_open()) {
        std::cerr << "Error: could not open passengers.csv" << '\n';
        return std::filesystem::path("");
    }

    
    std::cout << "\tpassengers.csv path: " << passengers_csv_path << '\n';
    passengers_csv << "name,id_number,phone_number,gender,district" << '\n';
    for (auto& passenger : passengers) {
        // Extract data from JSON
        std::string name = passenger["name"];
        std::string id_number = passenger["id_number"];
        std::string phone_number = passenger["phone_number"];
        std::string gender = passenger["gender"];
        std::string district = passenger["district"];
        passengers_csv << name << "," << id_number << "," << phone_number << "," << gender << "," << district << '\n';
    }
    passengers_csv.close(); 


    
    std::cout << "Writing to rides.csv..." << '\n';
    std::filesystem::path rides_csv_path = current_path / "rides.csv";
    std::ofstream rides_csv(rides_csv_path);
    if (!rides_csv.is_open()) {
        std::cerr << "Error: could not open rides.csv" << '\n';
        return std::filesystem::path("");;
    }

    
    std::cout << "\trides.csv path: " << rides_csv_path << '\n';

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
        // str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
        // rides_csv << str << "\n";
        rides_csv << str << '\n';
        // printProgress((double) ++i / total);
    }
    rides_csv.close();



    std::cout << "Writing to lines.csv..." << '\n';
    std::filesystem::path lines_csv_path = current_path / "lines.csv";
    std::ofstream lines_csv(lines_csv_path);
    std::cout << "\tlines.csv path: " << lines_csv_path << '\n';
    lines_csv << "name,start_time,end_time,mileage,color,first_opening,url,intro" << '\n';



    std::cout << "Writing to line_details..." << '\n';
    std::filesystem::path line_details_csv_path = current_path / "line_details.csv";
    std::ofstream line_details_csv(line_details_csv_path);
    std::cout << "\tline_details.csv path: " << line_details_csv_path << '\n';
    line_details_csv << "line_name,station_name" << '\n';
    // int id;
    // id = 0;
    if (!lines_csv.is_open()||!line_details_csv.is_open()) {
        std::cerr << "Error: could not open csv" << '\n';
        return std::filesystem::path("");;
    }

    for (auto& line : lines.items()) {
        // std::cout << "Line: " << line.key() << '\n';
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
        // line_str.erase(std::remove(line_str.begin(), line_str.end(), '\n'), line_str.end());
        lines_csv << line_str << '\n';
        
        for (const auto& station : line.value()["stations"]) {
            // std::cout << "  Station: " << station.get<std::string>() << '\n';
            std::string line_details_str = name + "," + station.get<std::string>();
            
            // line_details_str.erase(std::remove(line_details_str.begin(), line_details_str.end(), '\n'), line_details_str.end());
            line_details_csv << line_details_str << '\n';
        }
    }
    lines_csv.close();
    line_details_csv.close();



    std::cout << "Writing to stations.csv..." << '\n';
    std::filesystem::path stations_csv_path = current_path / "stations.csv";
    std::ofstream stations_csv(stations_csv_path);
    std::cout << "\tstations.csv path: " << stations_csv_path << '\n';
    stations_csv << "name,district,intro,chinese_name"<< '\n';

    std::cout << "Writing to bus_stations.csv..." << '\n';
    std::filesystem::path bus_stations_csv_path = current_path / "bus_stations.csv";
    std::ofstream bus_stations_csv(bus_stations_csv_path);
    std::cout << "\tbus_stations.csv path: " << bus_stations_csv_path << '\n';
    bus_stations_csv << "name,district" << '\n';

    std::cout << "Writing to exits.csv..." << '\n';
    std::filesystem::path exits_csv_path = current_path / "exits.csv";
    std::ofstream exits_csv(exits_csv_path);
    std::cout << "\texits.csv path: " << exits_csv_path << '\n';
    exits_csv << "station_name,name,textt" << '\n';

    std::cout << "Writing to bus_line_details.csv..." << '\n';
    std::filesystem::path bus_line_details_csv_path = current_path / "bus_line_details.csv";
    std::ofstream bus_line_details_csv(bus_line_details_csv_path);
    std::cout << "\tbus_line_details.csv path: " << bus_line_details_csv_path << '\n';
    bus_line_details_csv << "bus_line_name,bus_station_name" << '\n';  
    
    std::cout << "Writing to exit_details.csv..." << '\n';
    std::filesystem::path exit_details_csv_path = current_path / "exit_details.csv";
    std::ofstream exit_details_csv(exit_details_csv_path);
    std::cout << "\texit_details.csv path: " << exit_details_csv_path << '\n';
    exit_details_csv << "station_name,name,bus_station_name" << '\n';

    if (!bus_line_details_csv.is_open()||!exits_csv.is_open()||!stations_csv.is_open()||!bus_stations_csv.is_open()||!exit_details_csv.is_open()){
        std::cerr << "Error: could not open csv" << '\n';
        return std::filesystem::path("");;
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
        // station_str.erase(std::remove(station_str.begin(), station_str.end(), '\n'), station_str.end());
        stations_csv << station_str << '\n';

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
            exits_csv << exit_str << '\n';
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
            // std::cout << bus_station_name << '\n'; 
            std::string bus_station_str = bus_station_name + "," + district;
            // bus_station_str.erase(std::remove(bus_station_str.begin(), bus_station_str.end(), '\n'), bus_station_str.end());
            bus_station_str.erase(std::remove(bus_station_str.begin(), bus_station_str.end(), ' '), bus_station_str.end());
            bus_stations_csv << bus_station_str << '\n';

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
            while ((pos = bus_line_details.find_first_of(L",，路号")) != std::wstring::npos) {
                if (bus_line_details[pos] == L'路'||bus_line_details[pos] == L'号') {
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
                if (!token.empty()) {
                    bus_line_details_arr.push_back(converter.to_bytes(token));
                }
            }
            std::string bus_line_str;
            for (const auto& bus_line : bus_line_details_arr) {
                bus_line_str = bus_line + "," + bus_station_name;
                // bus_line_str.erase(std::remove(bus_line_str.begin(), bus_line_str.end(), '\n'), bus_line_str.end());
                bus_line_str.erase(std::remove(bus_line_str.begin(), bus_line_str.end(), ' '), bus_line_str.end());
                bus_line_details_csv << bus_line_str << '\n';
            }
            // bus_line_str.erase(std::remove(bus_line_str.begin(), bus_line_str.end(), ' '), bus_line_str.end());
            // bus_line_details_csv << bus_line_str << '\n';

            // std::string bus_line_details = bus_station["busOutInfo"][0]["busInfo"].get<std::string>();
            // size_t pos = 0;
            // std::string token = "";
            // while (pos < bus_line_details.size()) {
            //     // regular character
            //     if (std::isalnum(bus_line_details[pos])) {
            //         token += bus_line_details[pos];
            //         pos++;
            //     }
            //     // chinese character
            //     else if (bus_line_details[pos] >= 0x4e00 && bus_line_details[pos] <= 0x9fff) {
            //         token += bus_line_details.substr(pos,3);
            //         pos += 3;
            //     }
            //     // chinese separator
            //     else if (bus_line_details.substr(pos,3) == "、"||bus_line_details.substr(pos,3) == "，") {
            //         if (!token.empty()) {
            //             bus_line_details_csv << token << ',' << bus_station_name << '\n';
            //             token = "";
            //         }
            //         pos += 3;
            //     } 
            //     // remove chinese "路" at the end of bus line name
            //     else if (bus_line_details.substr(pos,3) == "路"){
            //         pos += 3;
            //     } 
            //     // english separator
            //     else if (bus_line_details[pos] == ','||bus_line_details[pos] == '|'){
            //         if (!token.empty()) {
            //             bus_line_details_csv << token << ',' << bus_station_name << '\n';
            //             token = "";
            //         }
            //         pos++;
            //     } 
            //     else {
            //         pos++;
            //     }
            // }

            // add exits details for each bus station
            std::string exit_name = bus_station["chukou"];
            if (exit_name.empty()) {
                continue;
            }
            std::string exit_details_str = name + "," + exit_name + "," + bus_station_name;
            exit_details_str.erase(std::remove(exit_details_str.begin() + name.size() - 1, exit_details_str.end(), ' '), exit_details_str.end());
            exit_details_csv << exit_details_str << '\n';
            
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
    std::cout << "\033[92mjson to csv completed in " << time_spent << " seconds.\033[0m" << '\n';

    return current_path;
}