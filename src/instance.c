#include "instance.h"

int instance_init(struct instance_desc** pdesc)
{
    struct instance_desc* desc;

    if(!pdesc) return -1;

    /* init mem for struct */
    desc = (struct instance_desc*)malloc(sizeof(struct instance_desc));

    /* clear */
    memset(desc, 0, sizeof(struct instance_desc));

    /* setup some defaults */
    strncpy(desc->log_file_name, "icp_dac_gpi_sinagller", MAX_NAME);
    desc->log_rotate_interval = 24 * 3600;

    *pdesc = desc;

    return 0;
};

int instance_release(struct instance_desc** pdesc)
{
    struct instance_desc* desc;

    if(!pdesc) return -1;
    desc = *pdesc;
    *pdesc = NULL;

    if(desc) return -2;

    free(desc);

    return 0;
};
