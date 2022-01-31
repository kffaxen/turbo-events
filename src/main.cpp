#include "turboevents.hpp"

#include <gflags/gflags.h>

int main(int argc, char **argv) {
  gflags::SetUsageMessage("fast event generator");
  gflags::SetVersionString("0.1");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  auto turbo = TurboEvents::TurboEvents::create();

  auto output = TurboEvents::TurboEvents::createPrintOutput();
  std::vector<std::unique_ptr<TurboEvents::Input>> inputs;

  for (int i = 1; i < argc; ++i)
    inputs.push_back(TurboEvents::TurboEvents::createXMLFileInput(argv[i]));

  inputs.push_back(TurboEvents::TurboEvents::createStreamInput(5));
  inputs.push_back(TurboEvents::TurboEvents::createStreamInput(2, 1500));

  turbo->run(*output, inputs);

  gflags::ShutDownCommandLineFlags();
  return 0;
}
