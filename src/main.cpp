#include "turboevents.hpp"

#include <gflags/gflags.h>
#include <iostream>
#include <string>

DEFINE_string(script, "", "file name for Python script");
DEFINE_bool(print, false, "print the Python commands and exit");
DEFINE_string(input, "", "comma-separated list of algorithmic input streams");
DEFINE_string(output, "print", "what kind of events to produce");
DEFINE_string(kafka_brokers, "localhost",
              "comma-separated list of kafka brokers");
DEFINE_string(kafka_ca_file, "", "path to ca file");
DEFINE_string(kafka_certificate_file, "", "path to certificate file");
DEFINE_string(kafka_key_file, "", "path to key file");
DEFINE_string(kafka_key_password, "", "password for the key file");
DEFINE_string(kafka_topic, "measurements", "topic to send kafka messages as");
DEFINE_bool(timeshift, false,
            "shift time stamps in file inputs to start immediately");
DEFINE_double(scale, 1.0,
              "scaling factor for intervals between events, less than 1 "
              "accelerates delivery");

int main(int argc, char **argv) {
  gflags::SetUsageMessage("fast event generator");
  gflags::SetVersionString("0.1");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  std::string cmds("import TurboEvents\n"
                   "t = TurboEvents.TurboEvents()\n");
  if (FLAGS_output == "print")
    cmds += "t.setPrintOutput()\n";
  else if (FLAGS_output == "kafka")
    cmds += "t.setKafkaOutput('" + FLAGS_kafka_brokers + "', '" +
            FLAGS_kafka_ca_file + "', '" + FLAGS_kafka_certificate_file +
            "', '" + FLAGS_kafka_key_file + "', '" + FLAGS_kafka_key_password +
            "', '" + FLAGS_kafka_topic + "')\n";
  else {
    std::cerr << "Unknown output: " << FLAGS_output << "\n";
    exit(1);
  }

  std::string tsArg = FLAGS_timeshift ? "True" : "False";
  for (int i = 1; i < argc; ++i)
    cmds +=
        "t.createXMLFileInput('" + std::string(argv[i]) + "', " + tsArg + ")\n";

  if (FLAGS_input.find("countdown") != std::string::npos) {
    cmds += "t.createCountDownInput(5, 200)\n"
            "t.createCountDownInput(2, 300)\n";
  }

  cmds += "t.run(" + std::to_string(FLAGS_scale) + ")\n";
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
