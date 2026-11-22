
#include "httplib.h"
#include "nlohmann/json.hpp"

#include "Graph.h"
#include "MST.h"
#include "ShortestPath.h"
#include "MaxFlow.h"
#include "faulttolerance.h"

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

using json = nlohmann::json;

// --------------------------------------------------
// Convert MST result to JSON
// --------------------------------------------------

json mstToJson(const MSTResult& result) {
    json edges = json::array();

    for (const auto& edge : result.selectedEdges) {
        edges.push_back({
            {"line_id", edge.lineId},
            {"from", edge.fromId},
            {"to", edge.toId},
            {"cost", edge.cost}
        });
    }

    return {
        {"selected_edges", edges},
        {"total_cost", result.totalCost},
        {"component_count", result.componentCount},
        {"is_spanning_tree", result.isSpanningTree}
    };
}

// --------------------------------------------------
// Create Graph from incoming JSON
// --------------------------------------------------

Graph buildGraph(const json& input) {
    if (!input.contains("substations") ||
        !input.contains("lines") ||
        !input["substations"].is_array() ||
        !input["lines"].is_array()) {
        throw std::runtime_error(
            "Request must contain substations and lines arrays."
        );
    }

    Graph grid;

    for (const auto& s : input["substations"]) {
        int id = s.at("id").get<int>();
        std::string name = s.at("name").get<std::string>();
        double x = s.value("x", 0.0);
        double y = s.value("y", 0.0);
        std::string type = s.at("type").get<std::string>();
        double supply = s.value("supply_mw", 0.0);
        double demand = s.value("demand_mw", 0.0);

        if (id <= 0 || name.empty() ||
            supply < 0 || demand < 0) {
            throw std::runtime_error(
                "Invalid substation data."
            );
        }

        if (type != "generator" && type != "load") {
            throw std::runtime_error(
                "Substation type must be generator or load."
            );
        }

        if (!grid.addSubstation(
            Substation(id, name, x, y, type, supply, demand)
        )) {
            throw std::runtime_error(
                "Could not add substation. Check duplicate IDs."
            );
        }
    }

    if (grid.vertexCount() == 0) {
        throw std::runtime_error(
            "At least one substation is required."
        );
    }

    for (const auto& l : input["lines"]) {
        int id = l.at("id").get<int>();
        int from = l.at("from").get<int>();
        int to = l.at("to").get<int>();
        double distance = l.at("distance_km").get<double>();
        double terrain = l.value("terrain_factor", 1.0);
        double capacity = l.at("capacity_mw").get<double>();

        if (id <= 0 || from == to ||
            distance < 0 || terrain < 0 || capacity < 0) {
            throw std::runtime_error(
                "Invalid transmission line data."
            );
        }

        if (!grid.findSubstation(from) ||
            !grid.findSubstation(to)) {
            throw std::runtime_error(
                "Transmission line refers to an unknown substation."
            );
        }

        if (!grid.addLine(
            TransmissionLine(
                id, from, to, distance, terrain, capacity
            )
        )) {
            throw std::runtime_error(
                "Could not add transmission line. Check duplicate connections or IDs."
            );
        }
    }

    return grid;
}

// --------------------------------------------------
// Run all existing GridGuard algorithms
// --------------------------------------------------

json analyzeGrid(const Graph& grid, const json& input) {
    json response;

    // Basic network information
    double totalSupply = 0;
    double totalDemand = 0;

    json substations = json::array();

    for (const auto& s : grid.getSubstations()) {
        totalSupply += s.getSupplyMW();
        totalDemand += s.getDemandMW();

        substations.push_back({
            {"id", s.getId()},
            {"name", s.getName()},
            {"type", s.getType()},
            {"supply_mw", s.getSupplyMW()},
            {"demand_mw", s.getDemandMW()},
            {"x", s.getX()},
            {"y", s.getY()}
        });
    }

    json lines = json::array();

    for (const auto& line : grid.getLines()) {
        lines.push_back({
            {"id", line.getId()},
            {"from", line.getFrom()},
            {"to", line.getTo()},
            {"distance_km", line.getDistance()},
            {"terrain_factor", line.getTerrainFactor()},
            {"capacity_mw", line.getCapacity()},
            {"cost", line.getConstructionCost()}
        });
    }

    response["network"] = {
        {"substation_count", grid.vertexCount()},
        {"line_count", grid.edgeCount()},
        {"total_supply_mw", totalSupply},
        {"total_demand_mw", totalDemand},
        {"generation_deficit_mw",
            std::max(0.0, totalDemand - totalSupply)},
        {"substations", substations},
        {"lines", lines}
    };

    // MODULE 2: MST
    MSTResult kruskal = MST::kruskal(grid);
    MSTResult prim = MST::prim(grid);

    response["mst"] = {
        {"kruskal", mstToJson(kruskal)},
        {"prim", mstToJson(prim)}
    };

    // MODULE 3: FAULT TOLERANCE
    FaultTolerance fault(grid);

    json bridges = json::array();

    for (int id : fault.findBridges()) {
        bridges.push_back(id);
    }

    json failures = json::array();

    for (const auto& failure : fault.testSingleEdgeFailures()) {
        failures.push_back({
            {"line_id", failure.lineId},
            {"components_after_removal",
                failure.componentsAfterRemoval},
            {"network_remains_connected",
                failure.networkRemainsConnected}
        });
    }

    std::vector<FT_Edge> candidates;

    if (input.contains("candidates") &&
        input["candidates"].is_array()) {

        for (const auto& c : input["candidates"]) {
            candidates.push_back({
                c.at("id").get<int>(),
                c.at("from").get<int>(),
                c.at("to").get<int>(),
                c.at("cost").get<double>()
            });
        }
    }

    double budget = input.value("budget", 100.0);

    AugmentationResult augmentation =
        fault.optimizeBudget(candidates, budget);

    json selectedLinks = json::array();

    for (const auto& link : augmentation.selectedLinks) {
        selectedLinks.push_back({
            {"id", link.id},
            {"from", link.fromId},
            {"to", link.toId},
            {"cost", link.cost}
        });
    }

    response["fault_tolerance"] = {
        {"bridges", bridges},
        {"bridge_count", bridges.size()},
        {"failures", failures},
        {"augmentation", {
            {"selected_links", selectedLinks},
            {"initial_bridge_count",
                augmentation.initialBridgeCount},
            {"final_bridge_count",
                augmentation.finalBridgeCount},
            {"budget_used", augmentation.budgetUsed},
            {"budget_remaining", augmentation.budgetRemaining}
        }}
    };

    // MODULE 4: DIJKSTRA
    int source = input.value("source_id", 1);
    int destination = input.value("destination_id", 4);

    if (!grid.findSubstation(source) ||
        !grid.findSubstation(destination)) {
        throw std::runtime_error(
            "Shortest path source or destination does not exist."
        );
    }

    ShortestPathResult shortest =
        ShortestPath::dijkstra(grid, source, destination);

    json route = json::array();

    for (int id : shortest.path) {
        route.push_back(id);
    }

    response["shortest_path"] = {
        {"source", source},
        {"destination", destination},
        {"path_found", shortest.pathFound},
        {"path", route},
        {"total_distance_km", shortest.totalDistance},
        {"weight_type", "distance"}
    };

    // MAXIMUM FLOW
    // Existing implementation automatically considers
    // all generator and load substations.

    MaxFlowResult flow = MaxFlow::edmondsKarp(grid);

    json cutLines = json::array();

    for (int id : flow.bottleneckLineIds) {
        cutLines.push_back(id);
    }

    json sourceSide = json::array();

    for (int id : flow.sourceSideSubstationIds) {
        sourceSide.push_back(id);
    }

    response["maximum_flow"] = {
        {"maximum_flow_mw", flow.maximumFlowMW},
        {"minimum_cut_line_ids", cutLines},
        {"source_side_substation_ids", sourceSide},
        {"unmet_demand_mw",
            std::max(0.0, totalDemand - flow.maximumFlowMW)},
        {"model",
            "Simplified network flow model, not electrical load-flow simulation"}
    };

    return response;
}

// --------------------------------------------------
// MAIN: HTTP SERVER
// --------------------------------------------------

int main() {
    httplib::Server server;

    server.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });

    server.Options("/api/.*",
        [](const httplib::Request&, httplib::Response& res) {
            res.status = 204;
        }
    );

    server.Get("/api/health",
        [](const httplib::Request&, httplib::Response& res) {
            res.set_content(
                R"({"status":"online","message":"GridGuard C++ backend running"})",
                "application/json"
            );
        }
    );

    server.Post("/api/analyze",
        [](const httplib::Request& req, httplib::Response& res) {
            try {
                json input = json::parse(req.body);

                Graph grid = buildGraph(input);
                json result = analyzeGrid(grid, input);

                res.status = 200;
                res.set_content(result.dump(4), "application/json");
            }
            catch (const std::exception& e) {
                res.status = 400;

                json error = {
                    {"error", e.what()}
                };

                res.set_content(error.dump(4), "application/json");
            }
        }
    );

    std::cout << "\n====================================\n";
    std::cout << "       GRIDGUARD C++ API SERVER\n";
    std::cout << "====================================\n";
    std::cout << "Health:  http://localhost:8080/api/health\n";
    std::cout << "Analysis: POST http://localhost:8080/api/analyze\n";
    std::cout << "====================================\n";

    if (!server.listen("127.0.0.1", 8080)) {
        std::cerr << "Could not start server on port 8080.\n";
        return 1;
    }

    return 0;
}
