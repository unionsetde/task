#include "gateway/modem.h"
#include "gateway/wireless.h"
#include "common/device.h"
#include "gateway.h"

/**
 * This function is polled by the main loop and should handle any packets coming
 * in over the modem or 868 MHz communication channel.
 */
void handle_communication(void)
{
  // YOUR CODE GOES HERE
  #define CNT_MAX 255
  uint8_t cnt = 0;
  SERVICE_GATEWAY service = {0};
  const uint8_t **data = {NULL};
  size_t length = 0;

  service.state = IDLE;
  while (cnt < CNT_MAX){
    if (service.state == IDLE){
      if (modem_dequeue_incoming(data, &length)){
        service.event = DATA_INCOMING;
      }
      service.num_of_message = 0;
    }
    else if (service.state == LISTENING){
      if (service.num_of_message == 0){
        decode_backend_request(&service, data, length);
      }
      else{
        proceed_service(&service);
      }
    }
    else if (service.state == TALKING){
      proceed_service(&service);
    }
    update_state(&service);
    cnt += 1;
  }
  free_service(service);
  return;
}
