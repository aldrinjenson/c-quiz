#include "../headers/requests.h"
#include "Poco/Net/HTTPRequest.h"
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <iostream>

using namespace Poco::Net;

json getData(URI uri) {
  std::string path(uri.getPathAndQuery());
  if (path.empty())
    path = "/";

  try {
    const Context::Ptr context =
        new Context(Context::CLIENT_USE, "", "", "", Context::VERIFY_NONE, 9,
                    false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
    HTTPSClientSession session(uri.getHost(), uri.getPort(), context);
    HTTPRequest request(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
    HTTPResponse response;
    session.sendRequest(request);
    std::istream &rs = session.receiveResponse(response);
    auto parsedJson = json::parse(rs);
    return parsedJson;
  } catch (Poco::Exception &e) {
    std::cout << "Error in getting quiz data: " << e.displayText() << std::endl;
    std::cout << "Check your internet connection and try again.\nExiting..\n";
    exit(0);
    return {};
  }
}