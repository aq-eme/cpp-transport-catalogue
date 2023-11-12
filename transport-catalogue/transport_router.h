#pragma once

#include "router.h"
#include "transport_catalogue.h"

#include <memory>

namespace transport {

    struct RouterSettings {
        int bus_wait_time = 0;
        double bus_velocity = 0.0;
    };

    class Router {
    public:
        Router(const RouterSettings& settings, const Catalogue& catalogue)
                : settings_(settings) {
            BuildGraph(catalogue);
        }
        void BuildGraphWithStops(const std::map<std::string, const Stop>& all_stops);
        void BuildGraphWithBuses(const std::map<std::string, Bus>& all_buses, const Catalogue& catalogue, double velocity);
        void Initialize(const Catalogue& catalogue) {
            BuildGraph(catalogue);
        }

        const std::optional<graph::Router<double>::RouteInfo> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;
        const graph::DirectedWeightedGraph<double>& GetGraph() const;

    private:
        void BuildGraph(const Catalogue& catalogue) const;

        RouterSettings settings_;

        graph::DirectedWeightedGraph<double> graph_;
        std::map<std::string, graph::VertexId> stop_ids_;
        std::unique_ptr<graph::Router<double>> router_;


    };

}