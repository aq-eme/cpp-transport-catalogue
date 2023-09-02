#include "request_handler.h"



const std::set<std::string> RequestHandler::GetBusesByStop(std::string_view stop_name) const {
    return catalogue_.FindStop(stop_name)->buses_by_stop;
}

bool RequestHandler::IsBusNumber(const std::string_view bus_number) const {
    return catalogue_.FindRoute(bus_number);
}

bool RequestHandler::IsStopName(const std::string_view stop_name) const {
    return catalogue_.FindStop(stop_name);
}

svg::Document RequestHandler::RenderMap() const {
    return renderer_.GetSVG(catalogue_.GetSortedAllBuses());
}