#pragma once
#include <map>
#include <string>
#ifndef MESSAGE_H
#define MESSAGE_H

// enum HubError {
//     NoSuchChannel,
//     ChannelAlreadyExists,
//     NotSubscribed,
//     AlreadySubscribed,
//     InternalError,
// };

namespace Request {
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
        {"Publish", RequestKind::Publish}};
    return strMap.at(s);
}
} // namespace Request

#endif
