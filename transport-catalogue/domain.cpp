#include "domain.h"
#include "request_handler.h"
/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области
 * (domain) вашего приложения и не зависят от транспортного справочника. Например Автобусные
 * маршруты и Остановки.
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */
std::optional<transport::BusStat> RequestHandler::GetBusStat(const std::string_view bus_number) const {
    transport::BusStat bus_stat{};
    const transport::Bus* bus = catalogue_.FindRoute(bus_number);

    if (!bus) throw std::invalid_argument("bus not found");
    if (bus->is_circle) bus_stat.stops_count = bus->stops.size();
    else bus_stat.stops_count = bus->stops.size() * 2 - 1;

    int route_length = 0;
    double geographic_length = 0.0;

    for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
        auto from = bus->stops[i];
        auto to = bus->stops[i + 1];
        if (bus->is_circle) {
            route_length += catalogue_.GetDistance(from, to);
            geographic_length += geo::ComputeDistance(from->coordinates,
                                                      to->coordinates);
        }
        else {
            route_length += catalogue_.GetDistance(from, to) + catalogue_.GetDistance(to, from);
            geographic_length += geo::ComputeDistance(from->coordinates,
                                                      to->coordinates) * 2;
        }
    }

    bus_stat.unique_stops_count = catalogue_.CalculateUniqueStopsCount(bus_number);
    bus_stat.route_length = route_length;
    bus_stat.curvature = route_length / geographic_length;

    return bus_stat;
}