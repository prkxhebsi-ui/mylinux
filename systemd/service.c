#include "mylinux_service.h"

void mylinux_service_init(mylinux_service_t *service, const char *name)
{
    if (service == 0) {
        return;
    }

    service->name = name;
    service->state = MYLINUX_SERVICE_STOPPED;
    service->exit_code = 0;
}

void mylinux_service_set_state(
    mylinux_service_t *service,
    mylinux_service_state_t state,
    int exit_code
)
{
    if (service == 0) {
        return;
    }

    service->state = state;
    service->exit_code = exit_code;
}
