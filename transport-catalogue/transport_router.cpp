#include "transport_router.h"

namespace transport {

    void Router::BuildGraph(const Catalogue& catalogue) const {
        catalogue.GetSortedAllStops();
        catalogue.GetSortedAllBuses();
    }

    void Router::BuildGraphWithStops(const std::map<std::string, const Stop>& all_stops) {
        graph::DirectedWeightedGraph<double> stops_graph(all_stops.size() * 2);
        std::map<std::string, graph::VertexId> stop_ids;
        graph::VertexId vertex_id = 0;

        for (const auto& [stop_name, stop_info] : all_stops) {
            stop_ids[std::string(stop_name)] = vertex_id;
            stops_graph.AddEdge({
                                        std::string(stop_name),
                                        0,
                                        vertex_id,
                                        ++vertex_id,
                                        static_cast<double>(settings_.bus_wait_time)
                                });
            ++vertex_id;
        }
        stop_ids_ = std::move(stop_ids);
        graph_ = std::move(stops_graph);
        router_ = std::make_unique<graph::Router<double>>(graph_);
    }

    void Router::BuildGraphWithBuses(const std::map<std::string, Bus>& all_buses, const Catalogue& catalogue, double velocity) {
        for (const auto& [_, bus_info] : all_buses) {
            const auto& stops = bus_info.stops;
            size_t stops_count = stops.size();
            for (size_t i = 0; i < stops_count; ++i) {
                for (size_t j = i + 1; j < stops_count; ++j) {
                    const Stop* stop_from = stops[i];
                    const Stop* stop_to = stops[j];
                    int dist_sum = 0;
                    int dist_sum_inverse = 0;
                    for (size_t k = i + 1; k <= j; ++k) {
                        dist_sum += catalogue.GetDistance(stops[k - 1], stops[k]);
                        dist_sum_inverse += catalogue.GetDistance(stops[k], stops[k - 1]);
                    }
                    graph_.AddEdge({
                                           bus_info.number,
                                           j - i,
                                           stop_ids_.at(stop_from->name) + 1,
                                           stop_ids_.at(stop_to->name),
                                           static_cast<double>(dist_sum) / velocity
                                   });

                    if (!bus_info.is_circle) {
                        graph_.AddEdge({
                                               bus_info.number,
                                               j - i,
                                               stop_ids_.at(stop_to->name) + 1,
                                               stop_ids_.at(stop_from->name),
                                               static_cast<double>(dist_sum_inverse) / velocity
                                       });
                    }
                }
            }
        }
    }

    const std::optional<graph::Router<double>::RouteInfo> Router::FindRoute(const std::string_view stop_from, const std::string_view stop_to) const {
        return router_->BuildRoute(stop_ids_.at(std::string(stop_from)), stop_ids_.at(std::string(stop_to)));
    }

    const graph::DirectedWeightedGraph<double>& Router::GetGraph() const {
        return graph_;
    }

}