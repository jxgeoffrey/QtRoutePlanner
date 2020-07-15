#pragma once

#include <iostream>
#include <vector>
#include <string>
#include "route_model.h"


class RoutePlanner {
  public:
    RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y);
    float GetDistance() const { return distance; }

    // the A* search method
    void AStarSearch();

  private:
    RouteModel &m_Model;
    RouteModel::Node *start_node;
    RouteModel::Node *end_node;

    // the distance variable holds the total distance for the route that A*
    // search finds from start_node to end_node
    float distance;
    std::vector<RouteModel::Node *> open_list;

    std::vector<RouteModel::Node> ConstructFinalPath(RouteModel::Node *current_node);
    float CalculateHValue(const RouteModel::Node *node);
    RouteModel::Node *NextNode();
    void AddNeighbors(RouteModel::Node *current_node);
};
