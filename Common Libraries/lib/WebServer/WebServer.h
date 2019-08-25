#ifndef WebServer_h
#define WebServer_h

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <list>

namespace webserver {

/**
 * Extension class for the ESP8266 webserver which provides some common stuff
 * like access control headers.
 */
class WebServer {
public:
  /**
   * Callback event that is triggered when a http request was received
   */
  typedef std::function<void(ESP8266WebServer *server)> HttpRequestCallback;

  /**
   * Starts a new server on the given port.
   */
  WebServer(int port);

  /**
   *  Setup method used to initialize the component
   */
  void setup();

  /**
   * Logic executed during every loop() in the main programm.
   */
  void loop();

  /**
   * Registers an http callback for the given path that is triggered when the
   * web server receives a matching request.
   *
   * @param path
   *          the URI path to match
   * @param callback
   *          the callback to trigger
   */
  void onHttpRequest(String path, HttpRequestCallback callback);

private:
  /**
   * Data for an HTTP request callback.
   */
  struct CallbackData {
    /**
     * The path to match.
     */
    String *path;
    /**
     * The callback to call.
     */
    HttpRequestCallback callback;
  };

  /**
   * Send back the OPTION header in case it is requested by the client.
   */
  void sendOptionHeader();
  /**
   * Internal request handler for predefined endpoints like /reset and to call
   * registered callbacks.
   */
  void handleRequest();
  /**
   * Reference to the esp server.
   */
  ESP8266WebServer *server;

  /**
   * The port on which the server is started.
   */
  int port;
  /**
   * List of registered callbacks for multiple paths
   */
  std::list<CallbackData *> callbacks;
};

} // namespace webserver

#endif
