#include "XMLInput.hpp"

#include <iostream>
#include <xercesc/util/PlatformUtils.hpp>

using namespace xercesc;

XMLInput::XMLInput() {
  try {
    XMLPlatformUtils::Initialize();
  } catch (const XMLException &e) {
    exit(1);
  }
  std::cout << "XML initialized\n";
}

XMLInput::~XMLInput() {
  XMLPlatformUtils::Terminate();
  std::cout << "XML terminated\n";
}
