#include <Preferences.h>
#include "StaticConfig.h"
#include <SoftwareSerial.h>
#include "utils/utils.hpp"
#include "utils/Blinker.hpp"
#include "utils/config.hpp"
#include "bluetooth/Ps4Client.hpp"
#include "sbus/sbus.hpp"

SoftwareSerial serial;
#if SERIAL_MODE == SRXL

#include "srxl2/RemoteReceiver.hpp"
RemoteReceiver itx(serial, DEVICES_REMOTE_RECEIVER);

#elif SERIAL_MODE == SBUS

#include "sbus/sbus.hpp"
Sbus itx(serial);

#else
    #error "SERIAL_MODE should be 1 of : SRXL, SBUS"
#endif
Blinker blk(2, 500);
Ps4Client ps4;
Preferences prefs;
Configurator config(prefs, CONFIG_PIN_OUT, CONFIG_PIN_IN);

void setup() {
  Serial.begin(115200);
  prefs.begin("app", RO_MODE);
  config.begin();
  blk.begin();
  delay(1000);
  if(config.enabled()){
    Serial.println("Starting in configuration mode");
    blk.set_delay(200);
  }else{
    String bt_address = prefs.getString("bt_address");
    if(bt_address.isEmpty()){
        bt_address = PS4_BT_ADDR;
    }
    Serial.println("Starting in normal mode");
    Serial.printf("bt_address = '%s'\n", bt_address.c_str());
    blk.set_delay(500);
    itx.begin(SERIAL_PIN);
    ps4.begin(bt_address.c_str());
  }
}

void loop() {
  blk.update();
  if(config.enabled()){
    config.update();
  }else{
      itx.set_channel(itx.CH_THROTTLE, ps4.throttle);
      itx.set_channel(itx.CH_AILERON, ps4.roll);
      itx.set_channel(itx.CH_ELEVATOR, ps4.pitch);
      itx.set_channel(itx.CH_RUDDER, ps4.yaw);
      itx.update();
  }
  delay(0);
}
