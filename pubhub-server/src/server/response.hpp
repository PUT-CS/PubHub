#include <optional>
#include <string>
#include "nlohmann/json.hpp"

class Response {
    private:
    enum ResponseKind { _Ok, _Error };
    static std::string kindToString(ResponseKind);
    Response(ResponseKind, std::optional<std::string>);
    
    std::optional<std::string> content;
    ResponseKind kind;
    
  public:
    Response();
    static Response Ok();
    static Response OkWithContent(std::string content);
    static Response InvalidRequest(std::string why);
    static Response NetworkError(std::string why);
    static Response InternalError();

    nlohmann::json toJson() const noexcept;
};
