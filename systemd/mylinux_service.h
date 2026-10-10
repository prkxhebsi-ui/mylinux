#ifndef MYLINUX_SERVICE_H
#define MYLINUX_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MYLINUX_SERVICE_STOPPED = 0,
    MYLINUX_SERVICE_RUNNING = 1,
    MYLINUX_SERVICE_FAILED = 2
} mylinux_service_state_t;

typedef struct {
    const char *name;
    mylinux_service_state_t state;
    int exit_code;
} mylinux_service_t;

void mylinux_service_init(mylinux_service_t *service, const char *name);
void mylinux_service_set_state(
    mylinux_service_t *service,
    mylinux_service_state_t state,
    int exit_code
);

#ifdef __cplusplus
}
#endif

#endif
