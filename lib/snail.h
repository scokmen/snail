#ifndef SNAIL_SNAIL_H
#define SNAIL_SNAIL_H

#ifndef S_BUFFER
#if __GNUC__ >= 4
#define S_BUF_AREA __attribute__ ((nonstring))
#else
#define S_BUF_AREA /* nothing */
#endif
#endif

#define LOG_ERR(MESSAGE) \
    fprintf(stderr, MESSAGE " %s:%d\n", __FILE__, __LINE__);

int s_listen(int port);

#endif //SNAIL_SNAIL_H
