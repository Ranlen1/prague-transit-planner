#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <chrono>

std::vector<std::string> line_processing(std::string &s)
{
    std::vector<std::string> return_output;

    std::string vector_element = "";
    bool double_quotes = false;
    for(char ch : s)
    {
        if(ch == '"')
        {
            double_quotes = !double_quotes;
            continue;
        }

        if(ch != ',' || double_quotes)
        {
            vector_element += ch;
        }
        else
        {
            return_output.push_back(vector_element);
            vector_element.clear();
        }
    }
    return_output.push_back(vector_element);
    return return_output;
};

std::pair<double, double> gps_to_km(double latitude, double longitude)
{
    std::pair<double, double> answer;

    return answer;
};

int main()
{

    auto start = std::chrono::steady_clock::now();

    std::ifstream stops_file("../data/stops.txt");

    if(!stops_file.is_open())
    {
        std::cerr << "could not load stops.txt" << std::endl;
        return 1;
    }

    struct station
    {
        std::string stop_id;
        std::string stop_name;
        double stop_latitude;
        double stop_longitude;
        std::string platform_code;
    };

    std::vector<station> station_list;
    std::unordered_map<std::string, int> station_id_to_int;
    int number_of_stations = 0;
    
    std::string line;
    bool first_line = true;

    while(std::getline(stops_file, line))
    {
        if(first_line)
        {
            first_line = false;
            continue;
        }
        if(!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }

        std::vector<std::string> stops_vector = line_processing(line);

        std::string stop_id, stop_name, zone_id, platform_code;
        double stop_latitude, stop_longitude;
        int location_type;
        
        stop_id = stops_vector[0];
        stop_name = stops_vector[1];
        stop_latitude = std::stod(stops_vector[2]);
        stop_longitude = std::stod(stops_vector[3]);
        zone_id = stops_vector[4];
        location_type = std::stoi(stops_vector[6]);
        platform_code = stops_vector[10];

        
        std::unordered_set<char> outside_prague_stations = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
        bool is_in_prague = true;
        for(char ch : zone_id)
        {
            if(outside_prague_stations.find(ch) != outside_prague_stations.end())
            {
                is_in_prague = false;
                break;
            }
        }

        if(stop_latitude < 49.90 || stop_latitude > 50.20 || stop_longitude < 14.20 || stop_longitude > 14.80)
            is_in_prague = false;

        if(is_in_prague && location_type == 0)
        {
            station_list.push_back({stop_id, stop_name, stop_latitude, stop_longitude, platform_code});
            station_id_to_int[stop_id] = number_of_stations;
            number_of_stations ++;
        }
    }
    std::ifstream trips_file("../data/stop_times.txt");

    if(!trips_file.is_open())
    {
        std::cerr << "could not load stop_times.txt" << std::endl;
        return 1;
    }

    bool time_saver = true;
    std::vector<std::string> line_first;
    std::vector<std::string> line_second;
    std::getline(trips_file, line);
    line_first = line_processing(line);
    line_second = line_processing(line);

    while(std::getline(trips_file, line))
    {
        if(time_saver)
        {
            line_first = line_processing(line);
            time_saver = !time_saver;

            if(line_first[0] == line_second[0] && station_id_to_int.find(line_first[3]) != station_id_to_int.end() && station_id_to_int.find(line_second[3]) != station_id_to_int.end())
            {
                std::cout << line_second[0] << '\n';
            }
        }
        else
        {
            line_second = line_processing(line);
            time_saver = !time_saver;
            if(line_first[0] == line_second[0] && station_id_to_int.find(line_first[3]) != station_id_to_int.end() && station_id_to_int.find(line_second[3]) != station_id_to_int.end())
            {
                std::cout << line_second[0] << '\n';
            }
        }
    }
    auto end = std::chrono::steady_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Cas behu: " << duration_ms.count() << " ms\n";
    return 0;
}
