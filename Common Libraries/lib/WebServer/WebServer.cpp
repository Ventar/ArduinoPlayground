
#include <WebServer.h>

namespace webserver {

WebServer::WebServer(int port) { this->port = port; }

void WebServer::sendOptionHeader() {
  server->sendHeader("Access-Control-Allow-Origin", "*");
  server->sendHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
  server->sendHeader("Access-Control-Allow-Headers", "*");
  server->send(204);
}

void WebServer::onHttpRequest(String path, HttpRequestCallback callback) {
  CallbackData *cbd = new CallbackData();
  cbd->path = new String(path);
  cbd->callback = callback;
  callbacks.push_back(cbd);
}

void WebServer::handleRequest() {
  if (server->method() == HTTP_OPTIONS) {
    sendOptionHeader();
    return;
  }

  if (server->hasHeader("Origin")) {
    server->sendHeader("Access-Control-Allow-Origin", server->header("Origin"));
  } else {
    server->sendHeader("Access-Control-Allow-Origin", "*");
  }

  server->sendHeader("Access-Control-Allow-Credentials", "true");

  const String path = server->uri();

  for (CallbackData *cbd : callbacks) {
    if (*cbd->path == path) {
      cbd->callback(server);
      return;
    }
  }

  if (path == "/") {
    String s = "<body><h2>ESP8266 webserver is running...</h2>";

    for (CallbackData *cbd : callbacks) {
      s += "<p><a href='" + *cbd->path + "'>" + *cbd->path + "</a> </p>";
    }

    s += "</body>";
    server->send(200, "text/html", s);
    return;
  }

  if (path == "/reset") {
    ESP.restart();
    return;
  }

  // in case the request was not handled.
  server->send(200, "text/plain",
               String("ESP8266 webserver is running...no endpoint for ::= [") +
                   path + "] available");
}

void WebServer::setup() {
  this->server = new ESP8266WebServer(this->port);
  server->on("/", [=]() { handleRequest(); });
  server->onNotFound([=]() { handleRequest(); });
  server->begin();
}

void WebServer::loop() { server->handleClient(); }

} // namespace webserver