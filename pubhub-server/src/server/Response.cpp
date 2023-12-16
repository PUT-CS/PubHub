#include "Response.hpp"
#include <optional>
#include <string>
#include <utility>

using nlohmann::json;

Response::Response() = default;

Response::Response(ResponseKind kind, std::optional<std::string> str)
    : content(std::move(str)), kind(kind) {}

auto Response::toJson() const noexcept -> json {
    auto j = nlohmann::json{
        {"status", Response::kindToString(this->kind)},
    };
    if (this->content.has_value()) {
        j.push_back({"info", this->content.value()});
    }

    return j;
}

auto Response::kindToString(ResponseKind kind) -> std::string {
    return kind == ResponseKind::_Ok ? "Ok" : "Error";
}

auto Response::Ok() -> Response { return {ResponseKind::_Ok, std::nullopt}; }
auto Response::OkWithContent(std::string content) -> Response {
    return {ResponseKind::_Ok, content};
}
auto Response::InvalidRequest(std::string content) -> Response {
    return {ResponseKind::_Error, content};
}
auto Response::NetworkError(std::string why) -> Response {
    return {ResponseKind::_Error, why};
}
auto Response::InternalError() -> Response {
    return {ResponseKind::_Error, "Internal server error occured"};
}
