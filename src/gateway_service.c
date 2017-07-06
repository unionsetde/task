#include <stdlib.h>
#include <string.h>
#include "gateway/modem.h"
#include "gateway/wireless.h"
#include "common/device.h"
#include "gateway_state.h"
#include "gateway_service.h"

/**
 * Return true if the first "length" bytes from "ptr1" is identical to "ptr2",
 * otherwise return false
 */
bool datancmp(const void *ptr1, const void *ptr2, size_t length);

void initialize_INVALID(SERVICE_GATEWAY *service)
{
  service->request = INVALID;
  service->event = NEED_TO_TALK;
  service->num_of_message = 1; // 1 to BACKEND
  service->num_of_reply = 0;
  service->retry = 0;
  #define INVALID_RESPONSE "INVALID"
  service->length_message = strlen(INVALID_RESPONSE);
  service->message = realloc(service->message, service->length_message);
  service->message = memcpy(service->message, INVALID_RESPONSE, service->length_message);

  return;
}

void initialize_PING_GATEWAY(SERVICE_GATEWAY *service)
{
  service->request = PING_GATEWAY;
  service->event = NEED_TO_TALK;
  service->num_of_message = 1; // 1 to BACKEND
  service->num_of_reply = 0;
  service->retry = 0;
  #define PING_GATEWAY_RESPONSE "PONG_GATEWAY"
  service->length_message = strlen(PING_GATEWAY_RESPONSE);
  service->message = realloc(service->message, service->length_message);
  service->message = memcpy(service->message, PING_GATEWAY_RESPONSE, service->length_message);

  return;
}

void initialize_PING_SENSOR(SERVICE_GATEWAY *service)
{
  service->request = PING_SENSOR;
  service->event = NEED_TO_TALK;
  service->num_of_message = 2; // 1 to SENSOR, 1 to BACKEND
  service->num_of_reply = 1; // 1 from SENSOR
  service->retry = 0;
  #define PING_SENSOR_MESSAGE "PING_SENSOR"
  service->length_message = strlen(PING_SENSOR_MESSAGE);
  service->message = realloc(service->message, service->length_message);
  service->message = memcpy(service->message, PING_SENSOR_MESSAGE, service->length_message);
  #define PONG_SENSOR_RESPONSE "PONG_SENSOR"
  service->length_reply = strlen(PING_SENSOR_MESSAGE);
  service->reply = realloc(service->reply, service->length_reply);
  service->reply = memcpy(service->reply, PONG_SENSOR_RESPONSE, service->length_reply);

  return;
}

void initialize_RESTART_GATEWAY(SERVICE_GATEWAY *service)
{
  service->request = RESTART_GATEWAY;
  service->event = NEED_TO_TALK;
  service->num_of_message = 0;
  service->num_of_reply = 0;
  service->retry = 0;

  return;
}

void initialize_RESTART_SENSOR(SERVICE_GATEWAY *service)
{
  service->request = RESTART_SENSOR;
  service->event = NEED_TO_TALK;
  service->num_of_message = 1; // 1 to SENSOR
  service->num_of_reply = 0;
  service->retry = 0;
  #define RESTART_SENSOR_REQUEST "RESTART_SENSOR"
  service->length_message = strlen(RESTART_SENSOR_REQUEST);
  service->message = realloc(service->message, service->length_message);
  service->message = memcpy(service->message, RESTART_SENSOR_REQUEST, service->length_message);

  return;
}

/**
 * TODO Copy and reformat the data inside "**data" to a single array with the same length
 */
uint8_t* reformat_incoming_data(uint8_t const **data, size_t length);

void initialize_common(SERVICE_GATEWAY *service, uint8_t const **data, size_t length)
{
  service->event = NEED_TO_TALK;
  service->num_of_message = 2; // 1 to SENSOR, 1 to BACKEND
  service->num_of_reply = 1; // 1 from SENSOR
  service->retry = 0;
  uint8_t *buff = NULL;
  buff = reformat_incoming_data(data, length);
  service->length_message = length;
  service->message = realloc(service->message, service->length_message);
  service->message = memcpy(service->message, buff, service->length_message);
  service->length_reply = length;
  service->reply = realloc(service->reply, service->length_reply);
  service->reply = memcpy(service->reply, buff, service->length_reply);
  if (buff) free(buff);

  return;
}

void initialize_ADD_KI_SENSOR(SERVICE_GATEWAY *service, uint8_t const **data, size_t length)
{
  service->request = ADD_KI_SENSOR;
  initialize_common(service, data, length);

  return;
}

void initialize_REMOVE_KI_SENSOR(SERVICE_GATEWAY *service, uint8_t const **data, size_t length)
{
  service->request = REMOVE_KI_SENSOR;
  initialize_common(service, data, length);
  
  return;
}

void initialize_OPEN_DOOR_SENSOR(SERVICE_GATEWAY *service)
{
  service->request = OPEN_DOOR_SENSOR;
  service->event = NEED_TO_TALK;
  service->num_of_message = 2; // 1 to SENSOR, 1 to BACKEND
  service->num_of_reply = 1; // 1 from SENSOR
  service->retry = 0;
  #define OPEN_DOOR_SENSOR_MESSAGE "OPEN_DOOR"
  service->length_message = strlen(OPEN_DOOR_SENSOR_MESSAGE);
  service->message = realloc(service->message, service->length_message);
  service->message = memcpy(service->message, PING_SENSOR_MESSAGE, service->length_message);
  #define OPEN_DOOR_SENSOR_RESPONSE "DOOR_OPEN"
  service->length_reply = strlen(PING_SENSOR_MESSAGE);
  service->reply = realloc(service->reply, service->length_reply);
  service->reply = memcpy(service->reply, OPEN_DOOR_SENSOR_RESPONSE, service->length_reply);

  return;
}

void proceed_INVALID(SERVICE_GATEWAY *service)
{
  if (service->state == TALKING){
    if (service->length_message < MODEM_MAX_PAYLOAD_LENGTH)
      modem_enqueue_outgoing(service->message, service->length_message);
      service->num_of_message -= 1;
  }
  service->event = END_TALKING;

  return;
}

void proceed_PING_GATEWAY(SERVICE_GATEWAY *service)
{
  if (service->state == TALKING){
    if (service->length_message < MODEM_MAX_PAYLOAD_LENGTH){
      modem_enqueue_outgoing(service->message, service->length_message);
      service->num_of_message -= 1;
    }
  }
  service->event = END_TALKING;

  return;
}

void proceed_common(SERVICE_GATEWAY *service)
{
  device_id_t id_sensor = get_device_id();
  uint8_t data[WIRELESS_PAYLOAD_LENGTH];
  if (service->state == TALKING){
    if (service->event == RETURN_REPLY) service->num_of_message -= 1;
    switch (service->num_of_message){
      case 2:
        if (service->length_message < WIRELESS_PAYLOAD_LENGTH){
          wireless_enqueue_outgoing(id_sensor, service->message);
        }
        service->event = REQUIRE_REPLY;
        break;
      case 1:
        if (service->length_message < MODEM_MAX_PAYLOAD_LENGTH){
          modem_enqueue_outgoing(service->message, service->length_message);
          service->num_of_message -= 1;
        }
      case 0:
      default:
          service->event = END_TALKING;
        break;
    }
  }
  else if (service->state == LISTENING){
    if (wireless_dequeue_incoming(&id_sensor, data)){
      // The condition to decide whether the return message is valid
      if (datancmp(&data, service->reply, service->length_reply)){
        service->num_of_reply -= 1;
        service->event = RETURN_REPLY;
      }
    }
    /** Reply is not as expected, signal retry
     *  TODO or maybe other exception handling action following by event update
     */
    if (service->event != RETURN_REPLY){
      if (service->retry < MAX_RETRY){
        service->retry += 1;
        service->event = NEED_TO_TALK; // signal resend
      }
      else service->event = TIME_OUT; // signal failure after multiple retries
    }
  }

  return;
}

void proceed_PING_SENSOR(SERVICE_GATEWAY *service)
{
  proceed_common(service);

  return;
}

void proceed_RESTART_GATEWAY(SERVICE_GATEWAY *service)
{
  //TODO: signal restart
  service->event = END_TALKING;

  return;
}

void proceed_RESTART_SENSOR(SERVICE_GATEWAY *service)
{
  device_id_t id_sensor = get_device_id();
  if (service->state == TALKING){
    if (service->length_message < WIRELESS_PAYLOAD_LENGTH){
      wireless_enqueue_outgoing(id_sensor, service->message);
      service->num_of_message -= 1;
    }
  }
  service->event = END_TALKING;
  
  return;
}

void proceed_ADD_KI_SENSOR(SERVICE_GATEWAY *service)
{
  proceed_common(service);

  return;
}

void proceed_REMOVE_KI_SENSOR(SERVICE_GATEWAY *service)
{
  proceed_common(service);

  return;
}

void proceed_OPEN_DOOR_SENSOR(SERVICE_GATEWAY *service)
{
  proceed_common(service);

  return;
}
