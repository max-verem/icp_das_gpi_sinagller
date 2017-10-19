#include <stdio.h>
#include <stdlib.h>

#include "instance.h"
#include "getopt.h"
#include "opts.h"

static const char short_options [] = "hl";
static const struct option long_options [] =
{
    { "log-file-name",      required_argument,      NULL,   '5'},
    { "log-rotate-interval",required_argument,      NULL,   '6'},

    { "channel",            required_argument,      NULL,   '1'},
    { "level",              required_argument,      NULL,   '2'},
    { "exe",                required_argument,      NULL,   '3'},
    { "arg",                required_argument,      NULL,   '4'},

    { "help",               required_argument,      NULL,   'h'},
    { "debug",              required_argument,      NULL,   'g'},
    { 0,                    0,                      0,      0}
};

int opts_init(struct instance_desc* desc, int argc, char** argv)
{
    int r, c, index = 0;
    signal_t* s;

    /* reset datas */
    optind = 0; opterr = 0; optopt = 0;

    /* program arguments processing */
    while(1)
    {
        c = getopt_long(argc, argv, short_options, long_options, &index);

        if(c == (-1)) break;

        switch(c)
        {
            case 0: break;

            /** --log-file-name         */
            case '5':
                strncpy(desc->log_file_name, optarg, MAX_PATH);
                break;

            /** --log-rotate-interval   */
            case '6':
                desc->log_rotate_interval = atol(optarg);
                break;

            /** --channel */
            case '1':
                s = (signal_t*)malloc(sizeof(signal_t));
                memset(s, 0, sizeof(signal_t));

                s->channel = atol(optarg);

                desc->list[desc->signals] = s;
                desc->signals++;
                break;

            /** --level */
            case '2':
                if(desc->signals)
                {
                    s = desc->list[desc->signals - 1];
                    s->level = atol(optarg);
                };
                break;

            /** --exe */
            case '3':
                if(desc->signals)
                {
                    s = desc->list[desc->signals - 1];
                    strncpy(s->exe, optarg, MAX_PATH);
                };
                break;

            /** --arg */
            case '4':
                if(desc->signals)
                {
                    s = desc->list[desc->signals - 1];
                    for(r = 0; r < MAX_ARGS && s->args[r]; r++);
                    if(r < MAX_ARGS && !s->args[r])
                    {
                        s->args[r] = (char*)malloc(MAX_PATH + 1);
                        strncpy(s->args[r], optarg, MAX_PATH);
                    };
                };
                break;

            /** --debug         */
            case 'g':
                desc->f_debug = 1;
                break;

            case 'h':
                opts_usage();
                return 1;
                break;

            default:
                fprintf(stderr, "ERROR: Incorrect argument: '%s'\n", optarg);
                return -1;
        };
    };

    return 0;
};

int opts_usage()
{
    fprintf
    (
        stderr,
        "Usage:\n"
        "    mchanply.exe <options>\n"
        "\n"
        "Where:\n"
        "  -l --list                 Show available ASIO drivers\n"

        "  --spool=<path>            Spool directory name\n"
        "  -d --driver               Driver index\n"
        "  --new-player=<STR>        Create new virtual player\n"
        "  --add-channel=<N>         Add channel to use for preveously added player\n"

        "  --log-file-name=<FILE>    Log file name.\n"
        "  --log-rotate-interval=<D> Period in seconds of log file rotation\n"
        "  --debug                   Enable debug output\n"
        "  -h --help                 Show this help\n"
	);
	return 0;
};
