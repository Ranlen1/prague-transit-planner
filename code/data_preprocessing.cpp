#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>

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

int main()
{
    std::ifstream stops_file("../data/stops.txt");

    if(!stops_file.is_open())
    {
        std::cerr << "could not load stops.txt" << std::endl;
        return 1;
    }

    
    std::string line;
    int k = 0;
    bool first_line = true;
    while(std::getline(stops_file, line) && k < 20)
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

        if(is_in_prague && location_type == 0)
        {
            std::cout << stop_id << ' ' << stop_name << ' ' << stop_latitude << ' ' << stop_longitude << ' ' << platform_code << '\n';
        }

        k++;
    }
    return 0;
}
