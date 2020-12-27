#include "route_planner.h"
#include "route_model.h"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <tuple>


void init_node(RouteModel::Node &node, RouteModel::Node *prev_node, RouteModel::Node &end_node) {
    node.FindNeighbors(); // fuck this hidden state-modifying shit
    if (prev_node) {
        node.parent = prev_node;
        node.g_value = prev_node->g_value + prev_node->distance(node);
        // node.g_value = prev_node->g_value + 1;
    } else {
        node.parent = nullptr;
        node.g_value = 0;
    }
    node.h_value = node.distance(end_node);
    node.visited = true; // I think??
}

std::vector<RouteModel::Node> construct_final_path(RouteModel::Node &end_node) {
    std::vector<RouteModel::Node> path_found;

    RouteModel::Node &current_node = end_node;
    while (current_node.parent) {
        path_found.push_back(*current_node.parent);
        current_node = *current_node.parent;
    }
    std::reverse(std::begin(path_found), std::end(path_found));

    return path_found;
}

// Returns the final path ordered from start to end, and the total distance.
std::tuple<std::vector<RouteModel::Node>, float> plan_the_route(RouteModel &model, float start_x, float start_y, float end_x, float end_y) {
    // FindClosestNode works with percentages
    RouteModel::Node& start_node = model.FindClosestNode(start_x / 100, start_y / 100);
    RouteModel::Node& end_node = model.FindClosestNode(end_x / 100, end_y / 100);
    std::cout << "start node has " << start_node.neighbors.size() << " neighbors, end node " << end_node.neighbors.size() << "\n";
    init_node(start_node, nullptr, end_node);

    std::vector<RouteModel::Node*> open_list = {&start_node};

    while (true) {
        if (open_list.empty()) {
            std::cout << "open list is empty :(\n";
            exit(1);
        }
        std::cout << "getting next node\n";

        for (int i = 0; i < open_list.size() + 1; i++) {
            auto n = open_list[i];
            std::cout << "uhhh? " << n << "\n";
            if ((size_t) n < 100) {
                std::cout << "There's a fucking " << (size_t) n << " in here\n";
            }
        }
        std::cout << "ok now: {";
        for (auto elem : open_list) {
            std::cout << elem << ", ";
        }
        std::cout << "}\n";

        // Get the open node with the least estimated total distance
        std::sort(open_list.begin(), open_list.end(), [](RouteModel::Node *a, RouteModel::Node *b){
            std::cout << "wtfffffff " << a << " " << b << "\n";
            if ((size_t) a < 0x550000000000 || (size_t) b < 0x550000000000) return true; // i dont even know
            if ((size_t) a > 0x560000000000 || (size_t) b > 0x560000000000) return true; // i dont even FUCKING know
            return a->g_value + a->h_value > b->g_value + b->h_value;
            // return a.g_value + a.h_value > b.g_value + a.g_value;
        });
        RouteModel::Node* optimal_node = open_list.back();
        open_list.pop_back();

        if (optimal_node == &end_node) break;

        std::cout << "expand next node\n";
        std::cout << "optimal node: " << optimal_node->x << " " << optimal_node->y << " has " << optimal_node->neighbors.size() << " neighbors\n";
        // Expand the node we chose to its neighbors
        for (RouteModel::Node* neighbor : optimal_node->neighbors) {
            std::cout << "wtf is this\n";
            if (neighbor->visited) {
                std::cout << "already visited\n";
                continue;
            }
            std::cout << "not visited yet! adding " << neighbor << "\n";
            init_node(*neighbor, optimal_node, end_node);
            open_list.push_back(neighbor);
        }
        std::cout << "done with the shit\n";
    }

    auto distance = end_node.g_value * model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    auto path_found = construct_final_path(end_node);
    return {path_found, distance};
}

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model), start_x(start_x), start_y(start_y), end_x(end_x), end_y(end_y) {
    // FindClosestNode works with percentages
    start_node = &model.FindClosestNode(start_x / 100, start_y / 100);
    end_node = &model.FindClosestNode(end_x / 100, end_y / 100);
}


float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
    std::cout << "Unimplemented!!!\n";
}


void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
    std::cout << "Unimplemented!!!\n";
}


RouteModel::Node *RoutePlanner::NextNode() {
    std::cout << "Unimplemented!!!\n";
}


std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    std::cout << "Unimplemented!!!\n";
}

void RoutePlanner::AStarSearch() {
    auto [path_found, distance] = plan_the_route(m_Model, start_x, start_y, end_x, end_y);
    m_Model.path = path_found;
    this->distance = distance;
    std::cout << "The path has " << path_found.size() << " nodes and distance " << distance << "\n";
}