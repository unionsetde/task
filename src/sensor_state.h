#include <stdlib.h>
#include <stdint.h>

typedef enum
{
  IDLE,
  TALKING,
  LISTENING,
} STATE_SENSOR;

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
  PING_SENSOR,
  RESTART_SENSOR,
  ADD_KI_SENSOR,
  REMOVE_KI_SENSOR,
  OPEN_DOOR_SENSOR,
} REQUEST_GATEWAY;

#define MAX_RETRY 10

typedef struct SERVICE_SENSOR {
  REQUEST_GATEWAY request;
  STATE_SENSOR state;
  EVENT event;
  uint8_t *message;
  size_t num_of_message;
  size_t length_message;
  size_t retry;
  //uint8_t *reply;
  //size_t num_of_reply;
  //size_t length_reply;
} SERVICE_SENSOR;

void free_service_sensor(SERVICE_SENSOR service);
void reset_service_sensor(SERVICE_SENSOR *service);

REQUEST_GATEWAY parse_gateway_request(uint8_t *data);
void initialize_gateway_request(SERVICE_SENSOR *service, uint8_t *data);

void update_state_sensor(SERVICE_SENSOR *service);
void proceed_service_sensor(SERVICE_SENSOR* service);
