#include "turboevents.hpp"

#include <gflags/gflags.h>

int main(int argc, char **argv) {
  gflags::SetUsageMessage("fast event generator");
  gflags::SetVersionString("0.1");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  // This program does very little.
  TurboEvents::TurboEvents turbo;

  x = ;

  gflags::ShutDownCommandLineFlags();
  return 0;
}
