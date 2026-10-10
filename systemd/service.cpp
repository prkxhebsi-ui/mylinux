#include <iostream>
#include "mylinux_service.h"

const char *mylinux_service_state_name(mylinux_service_state_t state)
{
    switch (state) {
        case MYLINUX_SERVICE_STOPPED:
            return "stopped";

        case MYLINUX_SERVICE_RUNNING:
            return "running";

        case MYLINUX_SERVICE_FAILED:
            return "failed";

        default:
            return "unknown";
    }
}

void mylinux_service_print(const mylinux_service_t *service)
{
    if (service == nullptr) {
        std::cerr << "MyLinux service: invalid service\n";
        return;
    }

    std::cout
        << "Service: " << (service->name ? service->name : "(unnamed)")
        << "\nState: " << mylinux_service_state_name(service->state)
        << "\nExit code: " << service->exit_code
        << '\n';
}
