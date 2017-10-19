#include <stdio.h>
#include "instance.h"
#include "opts.h"
#include "logger.h"
#include "extapp.h"
#include "UniDAQ.h"
#pragma comment(lib, "UniDAQ.lib")

/*

--channel=16 --level=-1 --exe=ping --arg="-n" --arg="2" --arg="10.1.1.1"


*/

static unsigned long WINAPI executor(void* ptr)
{
    signal_exec_t* se = (signal_exec_t*)ptr;

    logger_printf(0, "executor: starting [%s]", se->signal->exe);

    se->ctx = extapp_create(se->signal->exe, se->signal->args,
        NULL, NULL, NULL, NULL, NULL, NULL);

    extapp_exec(se->ctx, 15);

    logger_printf(0, "executor: finishing [%s]", se->signal->exe);

    extapp_destroy(se->ctx);

    se->signal = NULL;

    return 0;
};

int main(int argc, char** argv)
{
    int r;
    WORD c;
    instance_t* desc;

    /* init instance */
    instance_init(&desc);

    r = opts_init(desc, argc, argv);
    if(r)
    {
        if(1 != r)
            fprintf(stderr, "ERROR: parameters parsing error\n");
        instance_release(&desc);
        return 1;
    };

    /* start logger */
    logger_init(desc->log_file_name, desc->log_rotate_interval);

    /* check boards */
    r = Ixud_DriverInit(&c);
    if(r || !c)
        logger_printf(1, "no boards found");
    else
    {
        int i, j, k, f = 1;
        IXUD_DEVICE_INFO sDevInfo;
        IXUD_CARD_INFO sCardInfo;
        char szModelName[20] = "Unknow Device";
        DWORD curr[256], prev[256];

        Ixud_GetCardInfo(0, &sDevInfo, &sCardInfo, szModelName);
        logger_printf(0, "found board [%s]", szModelName);

        /* set all ports for input */
        for(i = 0; i < sCardInfo.wDIOPorts + sCardInfo.wDIPorts; i++)
            Ixud_SetDIOMode(0, i, 0);

        while(!desc->f_exit)
        {
            /* read current state */
            for(i = 0; i < sCardInfo.wDIOPorts + sCardInfo.wDIPorts; i++)
                Ixud_ReadDI(0, i, &curr[i]);

            if(f)
                f = 0;
            else
            {
                /* find the difference */
                for(i = 0, k = 0; i < sCardInfo.wDIOPorts + sCardInfo.wDIPorts; i++)
                    for(j = 0; j < sCardInfo.wDIOPortWidth; j++)
                    {
                        int
                            curr_level = (curr[i] & (1 << j))?1:0,
                            prev_level = (prev[i] & (1 << j))?1:0;

                        if(curr_level != prev_level)
                        {
                            int s, l;

                            logger_printf(0, "channel %d changed to %d", k, curr_level);

                            /* search for a executor to start */
                            for(s = 0; s < desc->signals; s++)
                            {
                                /* check channel */
                                if(k != desc->list[s]->channel)
                                    continue;

                                /* check level */
                                if
                                (
                                    -1 != desc->list[s]->level
                                    &&
                                    curr_level != desc->list[s]->level
                                )
                                    continue;

                                /* find free slot */
                                for(l = 0; l < MAX_EXECUTIONS && desc->exe[l] && desc->exe[l]->signal; l++);
                                if(!desc->exe[l] || !desc->exe[l]->signal)
                                {
                                    logger_printf(0, "using execution slot %d", l);

                                    if(!desc->exe[l])
                                    {
                                        desc->exe[l] = (signal_exec_t*)malloc(sizeof(signal_exec_t));
                                        memset(desc->exe[l], 0, sizeof(signal_exec_t));
                                    }
                                    else
                                        CloseHandle(desc->exe[l]->th);

                                    desc->exe[l]->instance = desc;
                                    desc->exe[l]->signal = desc->list[s];
                                    desc->exe[l]->th = CreateThread(NULL, 8096, executor, desc->exe[l], 0, NULL);
                                }
                                else
                                    logger_printf(1, "no free execution slot found");
                            };
                        };

                        k++;
                    };
            };

            memcpy(prev, curr, sizeof(prev));

            /* sleep a bit */
            Sleep(100);
        };

        Ixud_DriverClose();
    };

    /* release instance */
    instance_release(&desc);

    logger_release();

    return 0;
};
