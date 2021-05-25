#pragma once

#include <debuglogger/debuglogger.hpp>
#include <net.pdev.test.hpp>

namespace net {
namespace pdev {

class Server {
public:
  Server();

private:
  DebugLogger logger;
  std::unique_ptr<net::pdev::TestMode> test;

  std::unique_ptr<sdbus::IConnection> dbusConnection;
};

} // namespace pdev
} // namespace net