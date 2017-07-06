void initialize_INVALID(SERVICE_SENSOR *service);
void initialize_PING_SENSOR(SERVICE_SENSOR *service);
void initialize_RESTART_SENSOR(SERVICE_SENSOR *service);
void initialize_OPEN_DOOR_SENSOR(SERVICE_SENSOR *service);
void initialize_ADD_KI_SENSOR(SERVICE_SENSOR *service, uint8_t *data);
void initialize_REMOVE_KI_SENSOR(SERVICE_SENSOR *service, uint8_t *data);

void proceed_common(SERVICE_SENSOR *service);
void proceed_INVALID(SERVICE_SENSOR *service);
void proceed_PING_SENSOR(SERVICE_SENSOR *service);
void proceed_ADD_KI_SENSOR(SERVICE_SENSOR *service);
void proceed_RESTART_SENSOR(SERVICE_SENSOR *service);
void proceed_REMOVE_KI_SENSOR(SERVICE_SENSOR *service);
void proceed_OPEN_DOOR_SENSOR(SERVICE_SENSOR *service);
