#include <optional>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <QGuiApplication>
#include "../inc/route_model.h"
#include "../inc/route_planner.h"
#include "../inc/render.h"
#include "../inc/rasterwindow.h"

static std::optional<std::vector<std::byte>> ReadFile(const std::string &path)
{
    std::ifstream is{path, std::ios::binary | std::ios::ate};
    if( !is )
        return std::nullopt;

    auto size = is.tellg();
    std::vector<std::byte> contents(size);

    is.seekg(0);
    is.read(reinterpret_cast<char*>(contents.data()), size);

    if( contents.empty() )
        return std::nullopt;
    return std::move(contents);
}

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    std::string osm_data_file = "";
    if( argc > 1 ) {
       for( int i = 0; i < argc; ++i )
           if( std::string_view{argv[i]} == "-f" && ++i < argc )
               osm_data_file = argv[i];
    }
    else {
       std::cout << "Usage: [executable] [-f filename.osm]" << std::endl;
    }

    std::vector<std::byte> osm_data;

    if( osm_data.empty() && !osm_data_file.empty() ) {
       std::cout << "Reading OpenStreetMap data from the following file: " <<  osm_data_file << std::endl;
       auto data = ReadFile(osm_data_file);
       if( !data )
           std::cout << "Failed to read." << std::endl;
       else
           osm_data = std::move(*data);
    }

    // start and end node
    float start_x, start_y, end_x, end_y;
    start_x = 10.F;
    start_y = 10.F;
    end_x = 90.F;
    end_y = 90.F;
#if 0
    // get the user input for start node: (start_x, start_y) and end node: (end_x, end_y)
    std::cout << "The map coordinates start at (0,0) in the lower left corner and end at (100,100) ine the upper right corner."<<std::endl;
    std::cout << "Enter a value for start x between 0-100: ";
    std::cin >> start_x;
    std::cout << "Enter a value for start y between 0-100: ";
    std::cin >> start_y;
    std::cout << "Enter a value for end x between 0-100: ";
    std::cin >> end_x;
    std::cout << "Enter a value for end y between 0-100: ";
    std::cin >> end_y;
#endif
    // Build Model.
    RouteModel model{osm_data};

    // Perform search and render results.
    RoutePlanner route_planner{model, start_x, start_y, end_x, end_y};
    route_planner.AStarSearch();

    // printout the length of the path
    std::cout<<"The length of the path = "<<route_planner.GetDistance()<<" meters"<<std::endl;
    RenderMap render(model);
    render.show();

    return a.exec();
}
