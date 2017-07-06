#include <stdlib.h>
#include <string.h>
#include "sensor/wireless.h"
#include "sensor/ki_store.h"
#include "sensor/door.h"
#include "common/device.h"
#include "sensor_state.h"
#include "sensor_service.h"

void initialize_INVALID(SERVICE_SENSOR *service)
{
  service->request = INVALID;
  service->event = NEED_TO_TALK;
  service->num_of_message = 1; // 1 to GATEWAY
  //service->num_of_reply = 0;
  service->retry = 0;
  #define INVALID_RESPONSE "INVALID"
  service->length_message = strlen(INVALID_RESPONSE);
  service->message = realloc(service->message, service->length_message);
  service->message = memcpy(service->message, INVALID_RESPONSE, service->length_message);

  return;
}

void initialize_PING_SENSOR(SERVICE_SENSOR *service)
{
  service->request = PING_SENSOR;
  service->event = NEED_TO_TALK;
  service->num_of_message = 1; // 1 to GATEWAY
  //service->num_of_reply = 0;
  service->retry = 0;
  #define PING_SENSOR_RESPONSE "PONG_SENSOR"
  service->length_message = strlen(PING_SENSOR_RESPONSE);
  service->message = realloc(service->message, service->length_message);
  service->message = memcpy(service->message, PING_SENSOR_RESPONSE, service->length_message);

  return;
}

void initialize_RESTART_SENSOR(SERVICE_SENSOR *service)
{
  service->request = RESTART_SENSOR;
  service->event = NEED_TO_TALK;
  service->num_of_message = 0;
  //service->num_of_reply = 0;
  service->retry = 0;

  return;
}

void initialize_OPEN_DOOR_SENSOR(SERVICE_SENSOR *service)
{
  service->request = OPEN_DOOR_SENSOR;
  service->event = NEED_TO_TALK;
  service->num_of_message = 1; // 1 to GATEWAY
  //service->num_of_reply = 0;
  service->retry = 0;
  #define OPEN_DOOR_SENSOR_RESPONSE "DOOR_OPEN"
  service->length_message = strlen(OPEN_DOOR_SENSOR_RESPONSE);
  service->message = realloc(service->message, service->length_message);
  service->message = memcpy(service->message, OPEN_DOOR_SENSOR_RESPONSE, service->length_message);

  return;
}

void initialize_ADD_KI_SENSOR(SERVICE_SENSOR *service, uint8_t *data)
{
  service->request = ADD_KI_SENSOR;
  service->event = NEED_TO_TALK;
  service->num_of_message = 1; // 1 to GATEWAY
  //service->num_of_reply = 0;
  service->retry = 0;
  service->length_message = 29; // As stated in PROTOCOL ("add ki" request)
  service->message = realloc(service->message, service->length_message);
  service->message = memcpy(service->message, data, service->length_message);

  return;
}

void initialize_REMOVE_KI_SENSOR(SERVICE_SENSOR *service, uint8_t *data)
{
  service->request = ADD_KI_SENSOR;
  service->event = NEED_TO_TALK;
  service->num_of_message = 1; // 1 to GATEWAY
  //service->num_of_reply = 0;
  service->retry = 0;
  service->length_message = 32; // As stated in PROTOCOL ("remove ki" request)
  service->message = realloc(service->message, service->length_message);
  service->message = memcpy(service->message, data, service->length_message);

  return;
}

void proceed_INVALID(SERVICE_SENSOR *service)
{
  if (service->state == TALKING){
    if (service->length_message < WIRELESS_PAYLOAD_LENGTH){
      wireless_enqueue_outgoing(service->message);
      service->num_of_message -= 1;
    }
  }
  service->event = END_TALKING;

  return;
}

void proceed_PING_SENSOR(SERVICE_SENSOR *service)
{
  if (service->state == TALKING){
    if (service->length_message < WIRELESS_PAYLOAD_LENGTH){
      wireless_enqueue_outgoing(service->message);
      service->num_of_message -= 1;
    }
  }
  service->event = END_TALKING;

  return;
}

/**
 * TODO extract the token from the wirless packet
 */
uint8_t* extract_token(uint8_t *data);

void proceed_ADD_KI_SENSOR(SERVICE_SENSOR *service)
{
  uint8_t *token = NULL;
  ki_store_result_t result = KI_STORE_ERROR_FULL; 

  token = extract_token(service->message);
  if (service->state == TALKING){
    result = ki_store_add(token); // ki_store_add
    switch (result){
      case KI_STORE_SUCCESS:
        break;
      case KI_STORE_ERROR_FULL:
        #define KI_STORE_ERROR_FULL_RESPONSE "KI_STORE_ERROR_FULL_RESPONSE"
        service->length_message = strlen(KI_STORE_ERROR_FULL_RESPONSE);
        service->message = realloc(service->message, service->length_message);
        service->message = memcpy(service->message, KI_STORE_ERROR_FULL_RESPONSE,
                                  service->length_message);
        break;
      case KI_STORE_ERROR_UNKNOWN:
      default:
        #define KI_STORE_ERROR_UNKNOWN_RESPONSE "KI_STORE_ERROR_UNKNOWN_RESPONSE"
        service->length_message = strlen(KI_STORE_ERROR_UNKNOWN_RESPONSE);
        service->message = realloc(service->message, service->length_message);
        service->message = memcpy(service->message, KI_STORE_ERROR_UNKNOWN_RESPONSE,
                                  service->length_message);
        break;
    }
    if (service->length_message < WIRELESS_PAYLOAD_LENGTH){
      wireless_enqueue_outgoing(service->message);
      service->num_of_message -= 1;
    }
  }
  service->event = END_TALKING;
  if (token) free(token);

  return;
}

void proceed_RESTART_SENSOR(SERVICE_SENSOR *service)
{
  //TODO: signal restart
  service->event = END_TALKING;

  return;
}

void proceed_REMOVE_KI_SENSOR(SERVICE_SENSOR *service)
{
  uint8_t *token = NULL;
  ki_store_result_t result = KI_STORE_ERROR_FULL; 

  token = extract_token(service->message);
  if (service->state == TALKING){
    result = ki_store_remove(token); // ki_store_remove
    switch (result){
      case KI_STORE_SUCCESS:
        break;
      case KI_STORE_ERROR_FULL:
        #define KI_STORE_ERROR_FULL_RESPONSE "KI_STORE_ERROR_FULL_RESPONSE"
        service->length_message = strlen(KI_STORE_ERROR_FULL_RESPONSE);
        service->message = realloc(service->message, service->length_message);
        service->message = memcpy(service->message, KI_STORE_ERROR_FULL_RESPONSE,
                                  service->length_message);
        break;
      case KI_STORE_ERROR_UNKNOWN:
      default:
        #define KI_STORE_ERROR_UNKNOWN_RESPONSE "KI_STORE_ERROR_UNKNOWN_RESPONSE"
        service->length_message = strlen(KI_STORE_ERROR_UNKNOWN_RESPONSE);
        service->message = realloc(service->message, service->length_message);
        service->message = memcpy(service->message, KI_STORE_ERROR_UNKNOWN_RESPONSE,
                                  service->length_message);
        break;
    }
    if (service->length_message < WIRELESS_PAYLOAD_LENGTH){
      wireless_enqueue_outgoing(service->message);
      service->num_of_message -= 1;
    }
  }
  service->event = END_TALKING;
  if (token) free(token);

  return;
}

void proceed_OPEN_DOOR_SENSOR(SERVICE_SENSOR *service)
{
  if (service->state == TALKING){
    door_trigger();
    if (service->length_message < WIRELESS_PAYLOAD_LENGTH){
      wireless_enqueue_outgoing(service->message);
      service->num_of_message -= 1;
    }
  }
  service->event = END_TALKING;

  return;
}
