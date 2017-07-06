#include <stdlib.h>
#include "sensor_state.h"
#include "sensor_service.h"

void free_service_sensor(SERVICE_SENSOR service)
{
  if (service.message) free(service.message);

  return;
}

void reset_service_sensor(SERVICE_SENSOR *service)
{
  service->state = IDLE;
  service->request = INVALID;
  service->event = END_TALKING;
  service->num_of_message = 0;
  service->retry = 0;

  return;
}

/**
 * TODO: Parse a valid "REQUEST_GATEWAY" from "**data", otherwise return INVALID
 */
REQUEST_GATEWAY parse_gateway_request(uint8_t *data);

void initialize_gateway_request(SERVICE_SENSOR *service, uint8_t *data)
{
  service->request = parse_gateway_request(data);
  switch (service->request){
    case PING_SENSOR:
      initialize_PING_SENSOR(service);
      break;
    case RESTART_SENSOR:
      initialize_RESTART_SENSOR(service);
      break;
    case ADD_KI_SENSOR:
      initialize_ADD_KI_SENSOR(service, data);
      break;
    case REMOVE_KI_SENSOR:
      initialize_REMOVE_KI_SENSOR(service, data);
      break;
    case OPEN_DOOR_SENSOR:
      initialize_OPEN_DOOR_SENSOR(service);
      break;
    case INVALID:
    default:
      initialize_INVALID(service);
      break;
  }

  return;
}

void proceed_service_sensor(SERVICE_SENSOR* service)
{
  switch (service->request){
    case PING_SENSOR:
      proceed_PING_SENSOR(service);
      break;
    case RESTART_SENSOR:
      proceed_RESTART_SENSOR(service);
      break;
    case ADD_KI_SENSOR:
      proceed_ADD_KI_SENSOR(service);
      break;
    case REMOVE_KI_SENSOR:
      proceed_REMOVE_KI_SENSOR(service);
      break;
    case OPEN_DOOR_SENSOR:
      proceed_OPEN_DOOR_SENSOR(service);
      break;
    case INVALID:
    default:
      proceed_INVALID(service);
      break;
  }

  return;
}

/**
 * The following defines the architecture of the state machine (IDLE, TALKING, and LISTENING)
 */
void update_state_sensor(SERVICE_SENSOR *service)
{
  if (service->state == LISTENING){
    if (service->event == TIME_OUT){
      reset_service_sensor(service);
      service->state = IDLE;
    }
    else if (service->event == RETURN_REPLY) service->state = TALKING;
    else if (service->event == NEED_TO_TALK) service->state = TALKING;
    else if (service->event == STILL_LISTENING) service->state = LISTENING;
    else service->state = IDLE;
  }
  else if (service->state == TALKING){
    if (service->event == END_TALKING){
      reset_service_sensor(service);
      service->state = IDLE;
    }
    else if (service->event == STILL_TALKING) service->state = TALKING;
    else if (service->event == REQUIRE_REPLY) service->state = LISTENING;
    else service->state = IDLE;
  }
  else if (service->state == IDLE){
    if (service->event == DATA_INCOMING) service->state = LISTENING;
    else service->state = IDLE;
  }

  return;
}
