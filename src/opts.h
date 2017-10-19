#ifndef OPTS_H
#define OPTS_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

int opts_init(struct instance_desc* desc, int argc, char** argv);
int opts_usage();

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* OPTS_H */
