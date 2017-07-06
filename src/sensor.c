#include "sensor/wireless.h"
#include "sensor/ki_store.h"
#include "sensor/door.h"
#include "common/device.h"
#include "sensor_state.h"
#include "sensor_service.h"

/**
 * This function is polled by the main loop and should handle any packets coming
 * in over the 868 MHz communication channel.
 */
void handle_communication(void)
{
  // YOUR CODE GOES HERE
  #define CNT_MAX 255
  uint8_t cnt = 0;
  SERVICE_SENSOR service = {0};
  uint8_t data[WIRELESS_PAYLOAD_LENGTH];

  service.state = IDLE;
  while (cnt < CNT_MAX){
    if (service.state == IDLE){
      if (wireless_dequeue_incoming(data)){
        service.event = DATA_INCOMING;
      }
      service.num_of_message = 0;
    }
    else if (service.state == LISTENING){
      if (service.num_of_message == 0){
        initialize_gateway_request(&service, data);
      }
      else{
        proceed_service_sensor(&service);
      }
    }
    else if (service.state == TALKING){
      proceed_service_sensor(&service);
    }
    update_state_sensor(&service);
    cnt += 1;
  }
  free_service_sensor(service);

  return;
}
