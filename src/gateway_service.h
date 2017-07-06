void initialize_INVALID(SERVICE_GATEWAY *service);
void initialize_PING_SENSOR(SERVICE_GATEWAY *service);
void initialize_PING_GATEWAY(SERVICE_GATEWAY *service);
void initialize_RESTART_SENSOR(SERVICE_GATEWAY *service);
void initialize_RESTART_GATEWAY(SERVICE_GATEWAY *service);
void initialize_OPEN_DOOR_SENSOR(SERVICE_GATEWAY *service);
void initialize_common(SERVICE_GATEWAY *service, uint8_t const **data, size_t length);
void initialize_ADD_KI_SENSOR(SERVICE_GATEWAY *service, uint8_t const **data, size_t length);
void initialize_REMOVE_KI_SENSOR(SERVICE_GATEWAY *service, uint8_t const **data, size_t length);

void proceed_common(SERVICE_GATEWAY *service);
void proceed_INVALID(SERVICE_GATEWAY *service);
void proceed_PING_SENSOR(SERVICE_GATEWAY *service);
void proceed_PING_GATEWAY(SERVICE_GATEWAY *service);
void proceed_ADD_KI_SENSOR(SERVICE_GATEWAY *service);
void proceed_RESTART_SENSOR(SERVICE_GATEWAY *service);
void proceed_RESTART_GATEWAY(SERVICE_GATEWAY *service);
void proceed_REMOVE_KI_SENSOR(SERVICE_GATEWAY *service);
void proceed_OPEN_DOOR_SENSOR(SERVICE_GATEWAY *service);
