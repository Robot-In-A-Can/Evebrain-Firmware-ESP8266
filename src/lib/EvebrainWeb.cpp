#ifdef ESP8266
#include "Arduino.h"
#include "EvebrainWeb.h"
#include "web.h"

AsyncWebServer server(80);

class EvebrainRequestHandler: public AsyncWebHandler {
  public:
    EvebrainRequestHandler(){}
    bool canHandle(AsyncWebServerRequest *request){
      return request->method() == HTTP_GET;
    }

    void handleRequest(AsyncWebServerRequest *request){
      uint8_t i;
      bool found = false;

      if(request->method() == HTTP_GET){
        String path = request->url();
        if(path.endsWith("/")) path += "index.html";

        // Loop through our files to find one that matches
        for(i=0; i<fileCount; i++){
          if(path == files[i].filename){
            // If it's a match, send the file
            AsyncWebServerResponse *response = request->beginResponse(
              files[i].mime,
              files[i].len,
              [i](uint8_t *buffer, size_t maxLen, size_t alreadySent) -> size_t {
                if (strlen_P(files[i].content+alreadySent)>maxLen) {
                  // We have more to read than fits in maxLen Buffer
                  memcpy_P((char*)buffer, files[i].content+alreadySent, maxLen);
                  return maxLen;
                }
                // Ok, last chunk
                memcpy_P((char*)buffer, files[i].content+alreadySent, strlen_P(files[i].content+alreadySent));
                return strlen_P(files[i].content+alreadySent); // Return from here to end of document
              }
            );
            //response->addHeader("content-encoding", "gzip, deflate");
            request->send(response);
            found = true;
            break;
          }
        }
        // If we get here, send a 404
        if(!found) request->send(404, "text/html", "<h1>404</h1> This is not the page you are looking for...");
      }
    }
};

EvebrainWeb::EvebrainWeb() {
  server.addHandler(new EvebrainRequestHandler());
	server.begin();
}

#endif
