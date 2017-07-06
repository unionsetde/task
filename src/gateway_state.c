#include <stdlib.h>
#include "gateway_state.h"
#include "gateway_service.h"

void free_service_gateway(SERVICE_GATEWAY service)
{
  if (service.message) free(service.message);
  if (service.reply) free(service.reply);

  return;
}

void reset_service_gateway(SERVICE_GATEWAY *service)
{
  service->state = IDLE;
  service->request = INVALID;
  service->event = END_TALKING;
  service->num_of_message = 0;
  service->num_of_reply = 0;
  service->retry = 0;

  return;
}

/**
 * The following defines the architecture of the state machine (IDLE, TALKING, and LISTENING)
 */
void update_state_gateway(SERVICE_GATEWAY *service)
{
  if (service->state == LISTENING){
    if (service->event == TIME_OUT){
      reset_service_gateway(service);
      service->state = IDLE;
    }
    else if (service->event == RETURN_REPLY) service->state = TALKING;
    else if (service->event == NEED_TO_TALK) service->state = TALKING;
    else if (service->event == STILL_LISTENING) service->state = LISTENING;
    else service->state = IDLE;
  }
  else if (service->state == TALKING){
    if (service->event == END_TALKING){
      reset_service_gateway(service);
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

/**
 * TODO: Parse a valid "REQUEST_BACKEND" from "**data", otherwise return INVALID
 */
REQUEST_BACKEND parse_backend_request(uint8_t const **data, size_t length);

/**
 * Initialize "*service" and its members which define the stages of commnication in the protocol
 */
void initialize_backend_request(SERVICE_GATEWAY *service, uint8_t const **data, size_t length){
  service->request = parse_backend_request(data, length);
  switch (service->request){
    case PING_GATEWAY:
      initialize_PING_GATEWAY(service);
      break;
    case PING_SENSOR:
      initialize_PING_SENSOR(service);
      break;
    case RESTART_GATEWAY:
      initialize_RESTART_GATEWAY(service);
      break;
    case RESTART_SENSOR:
      initialize_RESTART_SENSOR(service);
      break;
    case ADD_KI_SENSOR:
      initialize_ADD_KI_SENSOR(service, data, length);
      break;
    case REMOVE_KI_SENSOR:
      initialize_REMOVE_KI_SENSOR(service, data, length);
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

/**
 * The only place where (enqueue/dequene + update evetns) happen
 */
void proceed_service_gateway(SERVICE_GATEWAY* service)
{
  switch (service->request){
    case PING_GATEWAY:
      proceed_PING_GATEWAY(service);
      break;
    case PING_SENSOR:
      proceed_PING_SENSOR(service);
      break;
    case RESTART_GATEWAY:
      proceed_RESTART_GATEWAY(service);
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
