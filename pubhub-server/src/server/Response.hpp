#include <optional>
#include <string>
#include "../json.hpp"

class Response {
    private:
    enum ResponseKind { _Ok, _Error };
    static auto kindToString(ResponseKind) -> std::string;
    Response(ResponseKind, std::optional<std::string>);
    
    std::optional<std::string> content;
    ResponseKind kind;
    
  public:
    Response();
    static auto Ok() -> Response;
    static auto OkWithContent(std::string content) -> Response;
    static auto InvalidRequest(std::string why) -> Response;
    static auto NetworkError(std::string why) -> Response;
    static auto InternalError() -> Response;

    [[nodiscard]] auto toJson() const noexcept -> nlohmann::json;
};
