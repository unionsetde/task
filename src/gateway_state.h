#include <stdlib.h>
#include <stdint.h>

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
  PING_SENSOR,
  PING_GATEWAY,
  RESTART_GATEWAY,
  RESTART_SENSOR,
  ADD_KI_SENSOR,
  REMOVE_KI_SENSOR,
  OPEN_DOOR_SENSOR,
} REQUEST_BACKEND;

#define MAX_RETRY 10

typedef struct SERVICE_GATEWAY {
  REQUEST_BACKEND request;
  STATE_GATEWAY state;
  EVENT event;
  uint8_t *reply;
  uint8_t *message;
  size_t num_of_message;
  size_t num_of_reply;
  size_t length_message;
  size_t length_reply;
  size_t retry;
} SERVICE_GATEWAY;

void free_service_gateway(SERVICE_GATEWAY service);
void reset_service_gateway(SERVICE_GATEWAY *service);

REQUEST_BACKEND parse_backend_request(uint8_t const **data, size_t length);
void initialize_backend_request(SERVICE_GATEWAY *service, uint8_t const **data, size_t length);

void update_state_gateway(SERVICE_GATEWAY *service);
void proceed_service_gateway(SERVICE_GATEWAY* service);
