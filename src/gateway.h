#include <stdlib.h>
#include <string.h>
#include "gateway/modem.h"
#include "gateway/wireless.h"
#include "common/device.h"

typedef enum
{
  IDLE,
  TALKING,
  LISTENING,
} STATE_GATEWAY;

typedef enum
{
  // TALKING events
  END_TALKING,
  REQUIRE_REPLY,
  STILL_TALKING,
  // LISTENING events
  TIME_OUT,
  RETURN_REPLY,
  NEED_TO_TALK,
  STILL_LISTENING,
  // IDLE events
  DATA_INCOMING,
} EVENT;

typedef enum
{
  INVALID,
  PING,
  RESTART_GATEWAY,
  RESTART_SENSOR,
  ADD_KI_SENSOR,
  REMOVE_KI_SENSOR,
  OPEN_DOOR_SENSOR,
} REQUEST_BACKEND;

typedef struct SERVICE_GATEWAY {
  REQUEST_BACKEND request;
  STATE_GATEWAY state;
  EVENT event;
  uint8_t *reply;
  uint8_t *message;
  size_t length_reply;
  size_t num_of_reply;
  size_t length_message;
  size_t num_of_message;
  size_t retry;
} SERVICE_GATEWAY;

/**
 * prototypes
 */
void free_service(SERVICE_GATEWAY service);
void reset_service(SERVICE_GATEWAY *service);

void initialize_INVALID(SERVICE_GATEWAY *service);
void initialize_PING(SERVICE_GATEWAY *service);
void initialize_RESTART_GATEWAY(SERVICE_GATEWAY *service);
void initialize_ADD_KI_SENSOR(SERVICE_GATEWAY *service);
void initialize_RESTART_SENSOR(SERVICE_GATEWAY *service);
void initialize_REMOVE_KI_SENSOR(SERVICE_GATEWAY *service);
void initialize_OPEN_DOOR_SENSOR(SERVICE_GATEWAY *service);

REQUEST_BACKEND parse_backend_request(uint8_t const **data, size_t length);
void decode_backend_request(SERVICE_GATEWAY *service, uint8_t const **data, size_t length);

void proceed_INVALID(SERVICE_GATEWAY *service);
void proceed_PING(SERVICE_GATEWAY *service);
void proceed_RESTART_GATEWAY(SERVICE_GATEWAY *service);
void proceed_ADD_KI_SENSOR(SERVICE_GATEWAY *service);
void proceed_RESTART_SENSOR(SERVICE_GATEWAY *service);
void proceed_REMOVE_KI_SENSOR(SERVICE_GATEWAY *service);
void proceed_OPEN_DOOR_SENSOR(SERVICE_GATEWAY *service);

void proceed_service(SERVICE_GATEWAY* service);
void update_state(SERVICE_GATEWAY *service);

/**
 * definitions
 */
void free_service(SERVICE_GATEWAY service)
{
  if (service.message) free(service.message);
  if (service.reply) free(service.reply);
  return;
}

void reset_service(SERVICE_GATEWAY *service)
{
  service->state = IDLE;
  service->request = INVALID;
  service->event = END_TALKING;
  service->length_reply = 0;
  service->num_of_reply = 0;
  service->num_of_message = 0;
  service->length_message = 0;
  return;
}

/**
 * TODO: Parse a valid "REQUEST_BACKEND" from "**data", otherwise return INVALID
 */
REQUEST_BACKEND parse_backend_request(uint8_t const **data, size_t length);

void initialize_INVALID(SERVICE_GATEWAY *service)
{
  #define INVALID_RESPONSE "INVALID"
  service->request = INVALID;
  service->event = NEED_TO_TALK;
  service->length_reply = 0;
  service->num_of_reply = 0;
  service->num_of_message = 1;
  service->length_message = strlen(INVALID_RESPONSE);
  service->message = realloc(service->message, service->length_message);
  service->message = memcpy(service->message, INVALID_RESPONSE, service->length_message);
  return;
}

void initialize_PING(SERVICE_GATEWAY *service)
{
  #define PING_RESPONSE "PONG"
  service->request = PING;
  service->event = NEED_TO_TALK;
  service->length_reply = 0;
  service->num_of_reply = 0;
  service->num_of_message = 1;
  service->length_message = strlen(PING_RESPONSE);
  service->message = realloc(service->message, service->length_message);
  service->message = memcpy(service->message, PING_RESPONSE, service->length_message);
  return;
}

void initialize_RESTART_GATEWAY(SERVICE_GATEWAY *service)
{
  service->request = RESTART_GATEWAY;
  service->event = NEED_TO_TALK;
  service->num_of_reply = 0;
  service->num_of_message = 0;
  return;
}

void initialize_ADD_KI_SENSOR(SERVICE_GATEWAY *service);
void initialize_RESTART_SENSOR(SERVICE_GATEWAY *service);
void initialize_REMOVE_KI_SENSOR(SERVICE_GATEWAY *service);
void initialize_OPEN_DOOR_SENSOR(SERVICE_GATEWAY *service);


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

void proceed_PING(SERVICE_GATEWAY *service)
{
  if (service->state == TALKING){
    if (service->length_message < MODEM_MAX_PAYLOAD_LENGTH)
      modem_enqueue_outgoing(service->message, service->length_message);
      service->num_of_message -= 1;
  }
  service->event = END_TALKING;
  return;
}

void proceed_RESTART_GATEWAY(SERVICE_GATEWAY *service){
  //TODO: signal restart
  service->event = END_TALKING;
  return;
}

void proceed_ADD_KI_SENSOR(SERVICE_GATEWAY *service);
void proceed_RESTART_SENSOR(SERVICE_GATEWAY *service);
void proceed_REMOVE_KI_SENSOR(SERVICE_GATEWAY *service);
void proceed_OPEN_DOOR_SENSOR(SERVICE_GATEWAY *service);

/**
 * Initialize "*service" and its members which define the stages of commnication in the protocol
 */
void decode_backend_request(SERVICE_GATEWAY *service, uint8_t const **data, size_t length){
  service->request = parse_backend_request(data, length);
  switch (service->request){
    case PING:
      initialize_PING(service);
      break;
    case RESTART_GATEWAY:
      initialize_RESTART_GATEWAY(service);
      break;
    case RESTART_SENSOR:
      initialize_RESTART_SENSOR(service);
      break;
    case ADD_KI_SENSOR:
      initialize_ADD_KI_SENSOR(service);
      break;
    case REMOVE_KI_SENSOR:
      initialize_REMOVE_KI_SENSOR(service);
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
void proceed_service(SERVICE_GATEWAY* service)
{
  switch (service->request){
    case PING:
      proceed_PING(service);
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

/**
 * This defines the architecture of the state machine (IDLE, TALKING, and LISTENING)
 */
void update_state(SERVICE_GATEWAY *service)
{
  if (service->state == LISTENING){
    if (service->event == TIME_OUT){
      reset_service(service);
      service->state = IDLE;
    }
    else if (service->event == RETURN_REPLY) service->state = TALKING;
    else if (service->event == NEED_TO_TALK) service->state = TALKING;
    else if (service->event == STILL_LISTENING) service->state = LISTENING;
    else service->state = IDLE;
  }
  else if (service->state == TALKING){
    if (service->event == END_TALKING){
      reset_service(service);
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
