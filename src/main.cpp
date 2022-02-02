#include "turboevents.hpp"

#include <gflags/gflags.h>
#include <iostream>

DEFINE_string(output, "print", "what kind of events to produce");

int main(int argc, char **argv) {
  gflags::SetUsageMessage("fast event generator");
  gflags::SetVersionString("0.1");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  auto turbo = TurboEvents::TurboEvents::create();
  if (FLAGS_output == "print")
    turbo->setPrintOutput();
  else if (FLAGS_output == "kafka")
    turbo->setKafkaOutput();
  else {
    std::cerr << "Unknown output: " << FLAGS_output << "\n";
    exit(1);
  }

  for (int i = 1; i < argc; ++i) turbo->createXMLFileInput(argv[i]);

  turbo->createStreamInput(5);
  turbo->createStreamInput(2, 1500);

  turbo->run();

  gflags::ShutDownCommandLineFlags();
  return 0;
}
