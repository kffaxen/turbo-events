#include "TurboXML.hpp"

#include <iostream>
#include <xercesc/util/PlatformUtils.hpp>

using namespace xercesc;

TurboXML::TurboXML() {
  try {
    XMLPlatformUtils::Initialize();
  } catch (const XMLException &e) {
    exit(1);
  }
  std::cout << "XML initialized\n";
}

TurboXML::~TurboXML() {
  XMLPlatformUtils::Terminate();
  std::cout << "XML terminated\n";
}

// TurboXML turboXML();
