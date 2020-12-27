#include "route_planner.h"
#include "route_model.h"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <tuple>
#include <iomanip>


void RoutePlanner::InitNode(RouteModel::Node &node, RouteModel::Node *prev_node) {
    if (prev_node) {
        node.parent = prev_node;
        node.g_value = prev_node->g_value + prev_node->distance(node);
    } else {
        node.parent = nullptr;
        node.g_value = 0;
    }
    node.h_value = CalculateHValue(&node);
    node.visited = true; // I think??
}

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model), start_x(start_x), start_y(start_y), end_x(end_x), end_y(end_y) {
    // FindClosestNode works with percentages
    start_node = &model.FindClosestNode(start_x / 100, start_y / 100);
    end_node = &model.FindClosestNode(end_x / 100, end_y / 100);
    InitNode(*start_node, nullptr);
}


float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
    return node->distance(*end_node);
}


void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
    current_node->FindNeighbors();
    for (RouteModel::Node* neighbor : current_node->neighbors) {
        if (neighbor->visited) continue;
        InitNode(*neighbor, current_node);
        open_list.push_back(neighbor);
    }
}


RouteModel::Node *RoutePlanner::NextNode() {
    std::sort(open_list.begin(), open_list.end(), [](RouteModel::Node *a, RouteModel::Node *b){
        return a->g_value + a->h_value > b->g_value + b->h_value;
    });
    RouteModel::Node *optimal_node = open_list.back();
    open_list.pop_back();
    return optimal_node;
}


std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    std::vector<RouteModel::Node> path_found;

    while (current_node) {
        path_found.push_back(*current_node);
        current_node = current_node->parent;
    }
    std::reverse(std::begin(path_found), std::end(path_found));

    return path_found;
}

void RoutePlanner::AStarSearch() {
    open_list = {start_node};

    while (true) {
        if (open_list.empty()) {
            std::cout << "open list is empty :(\n";
            exit(1);
        }

        // Get the open node with the least estimated total distance
        auto optimal_node = NextNode();

        if (optimal_node == end_node) break;

        AddNeighbors(optimal_node);
    }

    distance = end_node->g_value * m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    m_Model.path = ConstructFinalPath(end_node);
}