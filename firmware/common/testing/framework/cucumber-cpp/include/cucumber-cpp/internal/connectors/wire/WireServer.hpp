#ifndef CUKE_WIRESERVER_HPP_
#define CUKE_WIRESERVER_HPP_

#include "ProtocolHandler.hpp"

#include <string>

#include <boost/asio.hpp>

namespace cucumber {
namespace internal {

using namespace boost::asio;
using namespace boost::asio::ip;
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
using namespace boost::asio::local;
#endif

/**
 * Socket server that calls a protocol handler line by line
 */
class SocketServer {
public:
    /**
      * Constructor for DI
      */
    SocketServer(const ProtocolHandler *protocolHandler);

    /**
     * Accept one connection
     */
    virtual void acceptOnce() = 0;

protected:
    const ProtocolHandler *protocolHandler;
    io_service ios;

    template <typename Protocol, typename Service>
    void doListen(basic_socket_acceptor<Protocol, Service>& acceptor,
            const typename Protocol::endpoint& endpoint);
    template <typename Protocol, typename Service>
    void doAcceptOnce(basic_socket_acceptor<Protocol, Service>& acceptor);
    void processStream(std::iostream &stream);
};

/**
 * Socket server that calls a protocol handler line by line
 */
class TCPSocketServer : public SocketServer {
public:
    /**
     * Type definition for TCP port
     */
    typedef unsigned short port_type;

    /**
      * Constructor for DI
      */
    TCPSocketServer(const ProtocolHandler *protocolHandler);

    /**
     * Bind and listen to a TCP port
     */
    void listen(const port_type port);

    /**
     * Endpoint (IP address and port number) that this server is currently
     * listening on.
     *
     * @throw boost::system::system_error when not listening on any socket or
     *        the endpoint cannot be determined.
     */
    tcp::endpoint listenEndpoint() const;

    virtual void acceptOnce();

private:
    tcp::acceptor acceptor;
};

#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
/**
 * Socket server that calls a protocol handler line by line
 */
class UnixSocketServer : public SocketServer {
public:
    /**
      * Constructor for DI
      */
    UnixSocketServer(const ProtocolHandler *protocolHandler);

    /**
     * Bind and listen on a local stream socket
     */
    void listen(const std::string& unixPath);

    /**
     * Port number that this server is currently listening on.
     *
     * @throw boost::system::system_error when not listening on any socket or
     *        the endpoint cannot be determined.
     */
    stream_protocol::endpoint listenEndpoint() const;

    virtual void acceptOnce();

    ~UnixSocketServer();

private:
    stream_protocol::acceptor acceptor;
};
#endif

}
}

#endif /* CUKE_WIRESERVER_HPP_ */
