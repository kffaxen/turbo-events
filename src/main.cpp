#include "turboevents.hpp"

#include <gflags/gflags.h>
#include <iostream>
#include <string>

DEFINE_string(script, "", "file name for Python script");
DEFINE_bool(print, false, "print the Python commands and exit");
DEFINE_string(input, "", "comma-separated list of algorithmic input streams");
DEFINE_string(output, "print", "what kind of events to produce");

int main(int argc, char **argv) {
  gflags::SetUsageMessage("fast event generator");
  gflags::SetVersionString("0.1");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  std::string cmds("import TurboEvents\n"
                   "t = TurboEvents.TurboEvents()\n");
  if (FLAGS_output == "print")
    cmds += "t.setPrintOutput()\n";
  else if (FLAGS_output == "kafka")
    cmds += "t.setKafkaOutput()\n";
  else {
    std::cerr << "Unknown output: " << FLAGS_output << "\n";
    exit(1);
  }

  for (int i = 1; i < argc; ++i)
    cmds += "t.createXMLFileInput('" + std::string(argv[i]) + "')\n";

  if (FLAGS_input.find("countdown") != std::string::npos) {
    cmds += "t.createCountDownInput(5, 200)\n"
            "t.createCountDownInput(2, 300)\n";
  }

  cmds += "t.run()\n";
  if (FLAGS_print) {
    std::cout << cmds;
    goto out;
  }

  if (gflags::GetCommandLineFlagInfoOrDie("script").is_default)
    TurboEvents::TurboEvents::runString(cmds);
  else
    TurboEvents::TurboEvents::runScript(FLAGS_script);

out:
  gflags::ShutDownCommandLineFlags();
  return 0;
}
