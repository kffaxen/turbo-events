#include "turboevents.hpp"

#include <gflags/gflags.h>
#include <iostream>
#include <sstream>
#include <string>

DEFINE_string(script, "", "file name for Python script");
DEFINE_bool(print, false, "print the Python commands and exit");
DEFINE_string(input, "", "comma-separated list of algorithmic input streams");
DEFINE_string(output, "print", "comma-separated list of outputs");
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
DEFINE_string(xml_ctrl, "patient:id/glucose_level/event:ts:value",
              "what to extract from xml file");

int main(int argc, char **argv) {
  gflags::SetUsageMessage("fast event generator");
  gflags::SetVersionString("0.1");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  std::string cmds("import TurboEvents\n");
  std::string tsArg = FLAGS_timeshift ? "True" : "False";
  cmds += "t = TurboEvents.TurboEvents(" + tsArg + ")\n";

  { // Deal with the output flag.
    std::istringstream iss(FLAGS_output);
    std::string output;
    while (std::getline(iss, output, ',')) {
      if (output == "kafka")
        cmds += "t.addKafkaOutput('" + FLAGS_kafka_brokers + "', '" +
                FLAGS_kafka_ca_file + "', '" + FLAGS_kafka_certificate_file +
                "', '" + FLAGS_kafka_key_file + "', '" +
                FLAGS_kafka_key_password + "', '" + FLAGS_kafka_topic + "')\n";
      else if (output == "print")
        cmds += "t.addPrintOutput()\n";
      else {
        std::cerr << "Unknown output: " << output << "\n";
        exit(1);
      }
    }
  }

  { // Deal with the xml_ctrl flag and corresponding XML inputs.
    std::vector<std::vector<std::string>> xmlCtrl;
    std::istringstream iss(FLAGS_xml_ctrl);
    std::string item;
    while (std::getline(iss, item, ',')) {
      std::vector<std::string> xmlCtrl2;
      std::istringstream iss2(item);
      std::string item2;
      while (std::getline(iss2, item2, '/')) xmlCtrl2.push_back(item2);
      xmlCtrl.push_back(xmlCtrl2);
    }
    for (int i = 1; i < argc; ++i) {
      cmds += "t.createXMLFileInput('" + std::string(argv[i]) + "', [";
      for (auto &ctrl : xmlCtrl) {
        cmds += "[";
        for (auto &ctrl2 : ctrl) cmds += "'" + ctrl2 + "', ";
        cmds += "], ";
      }
      cmds += "])\n";
    }
  }

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
