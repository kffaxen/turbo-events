#include "turboevents.hpp"

#include <gflags/gflags.h>

DEFINE_string(output, "print", "what kind of events to produce");

int main(int argc, char **argv) {
  gflags::SetUsageMessage("fast event generator");
  gflags::SetVersionString("0.1");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  auto turbo = TurboEvents::TurboEvents::create();

  auto output = TurboEvents::TurboEvents::createOutput(FLAGS_output);
  for (int i = 1; i < argc; ++i) turbo->createXMLFileInput(argv[i]);

  turbo->createStreamInput(5);
  turbo->createStreamInput(2, 1500);

  turbo->run(*output);

  gflags::ShutDownCommandLineFlags();
  return 0;
}
