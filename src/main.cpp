#include "turboevents.hpp"

#include <gflags/gflags.h>
#include <xercesc/util/PlatformUtils.hpp>

using namespace xercesc;

int main(int argc, char **argv) {
  gflags::SetUsageMessage("fast event generator");
  gflags::SetVersionString("0.1");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  try {
    XMLPlatformUtils::Initialize();
  } catch (const XMLException &e) {
    return 1;
  }

  auto turbo = TurboEvents::TurboEvents::create();

  XMLPlatformUtils::Terminate();
  gflags::ShutDownCommandLineFlags();
  return 0;
}
