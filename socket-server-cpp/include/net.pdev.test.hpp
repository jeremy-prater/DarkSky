#include "net.pdev.test.h"
#include <debuglogger/debuglogger.hpp>
#include <sdbus-c++/sdbus-c++.h>

namespace net {
namespace pdev {

class TestMode : public sdbus::AdaptorInterfaces<net::pdev::test_adaptor> {
public:
  TestMode(sdbus::IConnection &connection, std::string objectPath)
      : AdaptorInterfaces(connection, std::move(objectPath)),
        logger("TestMode-" + objectPath, DebugLogger::Color::WHITE, false) {
    logger.Info("Created TestMode");

    registerAdaptor();
  }

  ~TestMode() {
    logger.Info("Destructed");
    
    unregisterAdaptor();
  }

private:
  DebugLogger logger;

protected:
  std::string Test(const std::string &key) override {}

  //     std::string concatenate(const std::vector<int32_t>& numbers, const
  //     std::string& separator) override
  //     {
  //         // Return error if there are no numbers in the collection
  //         if (numbers.empty())
  //             throw sdbus::Error("org.sdbuscpp.TestMode.Error", "No
  //             numbers provided");

  //         // Concatenate the numbers
  //         std::string result;
  //         for (auto number : numbers)
  //         {
  //             result += (result.empty() ? std::string() : separator) +
  //             std::to_string(number);
  //         }

  //         // Emit the 'concatenated' signal with the resulting string
  //         emitConcatenated(result);

  //         // Return the resulting string
  //         return result;
  //     }
};
} // namespace pdev
} // namespace net
