#ifndef __CmdProcessor_h__
#define __CmdProcessor_h__

#include "Arduino.h"

class Evebrain;
class CmdProcessor;

#include "Evebrain.h"
#include "./lib/ArduinoJson/ArduinoJson.h"

#define CMD_COUNT 45
#ifdef AVR
#define JSON_BUFFER_LENGTH 128
#endif //AVR
#ifdef ESP8266
#define JSON_BUFFER_LENGTH 512
#endif //AVR
#define OUTPUT_HANDLER_COUNT 2

typedef void (Evebrain::*EvebrainMemFn)(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
typedef void (* fp) (void *, char *);
typedef boolean (* fp_ready) (void *);
typedef void (* jsonMsgHandler) (ArduinoJson::JsonObject &);

struct Cmd {
  const char *cmd;
  EvebrainMemFn func;
  bool immediate;
};

class CmdProcessor {
  public:
    CmdProcessor();
    void addCmd(const char cmd[], EvebrainMemFn func, bool immediate);
    bool addOutputHandler(jsonMsgHandler);
    void process();
    void notify(const char[], ArduinoJson::JsonObject &);
    void setEvebrain(Evebrain &);
    void sendComplete();
    void sendCompleteMSG(char * msg);
    boolean processMsg(char * msg);
    boolean in_process;
  private:
    boolean processLine();
    void processCmd(const char &cmd, const char &arg, const char &id);
    void runCmd(char &cmd, char &arg, char &id);
    void sendResponse(const char state[], ArduinoJson::JsonObject &, const char &id);
    char webSocketKey[61];
    char current_id[11];
    boolean processJSON();
    Evebrain* _m;
    Cmd _cmds[CMD_COUNT];
    int cmd_counter;
    jsonMsgHandler outputHandlers[OUTPUT_HANDLER_COUNT];
};


#endif
