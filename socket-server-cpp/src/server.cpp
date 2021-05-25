#include <server.hpp>

namespace net {
namespace pdev {

Server::Server() : logger("Server", DebugLogger::Color::RED, false) {
  logger.Info("Created Server");

  // Create D-Bus connection to the system bus and requests name on it.
  const char *serviceName = "org.sdbuscpp.concatenator";
  dbusConnection = sdbus::createSystemBusConnection(serviceName);

  // Create concatenator D-Bus object.
  const char *objectPath = "/";
  test = std::make_unique<net::pdev::TestMode>(*dbusConnection, objectPath);

  // Run the loop on the connection.
  dbusConnection->enterEventLoop();
}
} // namespace pdev
} // namespace net