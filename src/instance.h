#ifndef INSTANCE_H
#define INSTANCE_H

#include <windows.h>

#define MAX_SIGNALS     256
#define MAX_ARGS        256
#define MAX_EXECUTIONS  256

#ifndef MAX_NAME
#define MAX_NAME    32768
#endif

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct signal_desc
{
    int channel;
    int level;
    char exe[MAX_NAME];
    char *args[MAX_ARGS];
} signal_t;

struct instance_desc;

typedef struct signal_exec_desc
{
    signal_t* signal;
    void* ctx;
    HANDLE th;
    struct instance_desc* instance;
}signal_exec_t;

typedef struct instance_desc
{
    int signals;
    signal_t* list[MAX_SIGNALS];

    /** log file name location */
    char log_file_name[MAX_NAME];

    /** log file rotation period */
    int log_rotate_interval;

    /** exit flag */
    int f_exit, f_debug;

    signal_exec_t* exe[MAX_EXECUTIONS];
} instance_t;

int instance_init(struct instance_desc** desc);
int instance_release(struct instance_desc** desc);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* INSTANCE_H */
