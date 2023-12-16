#pragma once
#include <map>
#include <string>
#ifndef MESSAGE_H
#define MESSAGE_H

namespace RequestKind {
enum class RequestKind {
    Subscribe,
    Unsubscribe,
    CreateChannel,
    DeleteChannel,
    Publish,
    Ask
};

inline RequestKind fromString(std::string s) {
    static const std::map<std::string, RequestKind> strMap = {
        {"Subscribe", RequestKind::Subscribe},
        {"Unsubscribe", RequestKind::Unsubscribe},
        {"CreateChannel", RequestKind::CreateChannel},
        {"DeleteChannel", RequestKind::DeleteChannel},
        {"Publish", RequestKind::Publish}        {"Ask", RequestKind::Ask}};
    return strMap.at(s);
}
} // namespace RequestKind


#endif
