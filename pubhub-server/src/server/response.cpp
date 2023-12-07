#include "response.hpp"
#include <optional>
#include <string>
#include <utility>

Response::Response() {}

Response::Response(ResponseKind kind, std::optional<std::string> str) {
    this->kind = kind;
    this->content = str;
}

nlohmann::json Response::toJson() const noexcept {
    auto j = nlohmann::json {
        {"status", Response::kindToString(this->kind)},
    };
    if (this->content.has_value()) {
        j.push_back({"info", this->content.value()});
    }
    
    return j;
}

std::string Response::kindToString(ResponseKind kind) {
    return kind == ResponseKind::_Ok ? "Ok" : "Error";
}

Response Response::Ok() {
    return Response(ResponseKind::_Ok, std::nullopt);
}
Response Response::OkWithContent(std::string content) {
    return Response(ResponseKind::_Ok, content);
}
Response Response::InvalidRequest(std::string content) {
    return Response(ResponseKind::_Error, content);
}
Response Response::NetworkError(std::string why) {
    return Response(ResponseKind::_Error, why);
}
Response Response::InternalError() {
    return Response(ResponseKind::_Error, "Internal server error occured");
}
