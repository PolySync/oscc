#ifndef CUKE_PROTOCOLHANDLER_HPP_
#define CUKE_PROTOCOLHANDLER_HPP_

#include <string>

namespace cucumber {
namespace internal {

/**
 * Protocol that reads one command for each input line.
 */
class ProtocolHandler {
public:
    virtual std::string handle(const std::string &request) const = 0;
    virtual ~ProtocolHandler() {};
};

}
}

#endif /* CUKE_PROTOCOLHANDLER_HPP_ */
