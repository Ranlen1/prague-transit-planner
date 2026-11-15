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

    std::unordered_set<char> outside_prague_stations = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    std::vector<std::string> stops_vector;

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

        stops_vector = line_processing(line);

        double stop_latitude = std::stod(stops_vector[2]);
        double stop_longitude = std::stod(stops_vector[3]);
        std::string &zone_id = stops_vector[4];
        int location_type = std::stoi(stops_vector[6]);

        
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
            station_list.push_back({stops_vector[0], stops_vector[1], stop_latitude, stop_longitude, stops_vector[10]});
            station_id_to_int[stops_vector[0]] = number_of_stations;
            number_of_stations ++;
        }
    }
    std::ifstream routes_file("../data/routes.txt");
    if(!routes_file.is_open())
    {
        std::cerr << "could not load routes.txt";
        return 1;
    }

    std::getline(routes_file, line);

    std::unordered_map<std::string, std::pair<std::string, int>> route_id;
    std::vector<std::string> route_line;

    while(std::getline(routes_file, line))
    {
        route_line = line_processing(line); 
        route_id[route_line[0]] = {route_line[2], std::stoi(route_line[4])};
    }

    std::ifstream calendar_file("../data/calendar.txt");

    if(!calendar_file.is_open())
    {
        std::cerr << "could not load calendar.txt";
        return 1;
    }

    std::getline(calendar_file, line);

    std::vector<std::string> calendar_line;
    std::unordered_map<std::string, std::vector<int>> calendar_id;
    std::vector<int> days(7);

    while(std::getline(calendar_file, line))
    {
        calendar_line = line_processing(line);
        for(int i = 0; i < 7; i++)
        {
            days[i] = std::stoi(calendar_line[i+1]);
        }
        calendar_id[calendar_line[0]] = days;
    }

    std::ifstream trips_file("../data/trips.txt");

    if(!trips_file.is_open())
    {
        std::cerr << "could not load trips.txt";
        return 1;
    }

    std::getline(trips_file, line);

    std::vector<std::string> trips_line;
    std::unordered_map<std::string, std::tuple<std::string, std::string, std::string>> trip_connector;

    while(std::getline(trips_file, line))
    {
        trips_line = line_processing(line);
        trip_connector[trips_line[2]] = {trips_line[0], trips_line[1], trips_line[3]};
    }

    std::ifstream stop_times_file("../data/stop_times.txt");

    if(!stop_times_file.is_open())
    {
        std::cerr << "could not load stop_times.txt" << std::endl;
        return 1;
    }

    bool time_saver = true;
    std::vector<std::string> line_first;
    std::vector<std::string> line_second;
    std::getline(stop_times_file, line);
    line_first = line_processing(line);
    line_second = line_processing(line);

    while(std::getline(stop_times_file, line))
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
