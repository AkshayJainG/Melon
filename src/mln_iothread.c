
/*
 * Copyright (C) Niklaus F.Schen.
 */
#include "mln_iothread.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#if defined(WIN32)
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

static inline void mln_iothread_fd_nonblock_set(int fd);
static inline mln_iothread_msg_t *mln_iothread_msg_new(mln_u32_t type, void *data, int feedback);
static inline void mln_iothread_msg_free(mln_iothread_msg_t *msg);
MLN_CHAIN_FUNC_DECLARE(mln_iothread_msg, mln_iothread_msg_t, static inline void,);
MLN_CHAIN_FUNC_DEFINE(mln_iothread_msg, mln_iothread_msg_t, static inline void, prev, next);

int mln_iothread_init(mln_iothread_t *t, struct mln_iothread_attr *attr)
{
    mln_u32_t i;
    int fds[2];

    if (!attr->nthread || attr->entry == NULL) {
        return -1;
    }

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) < 0) {
        return -1;
    }
    t->io_fd = fds[0];
    t->user_fd = fds[1];
    mln_iothread_fd_nonblock_set(t->io_fd);
    mln_iothread_fd_nonblock_set(t->user_fd);
    t->entry = attr->entry;
    t->args = attr->args;
    t->handler = attr->handler;
    pthread_mutex_init(&(t->io_lock), NULL);
    pthread_mutex_init(&(t->user_lock), NULL);
    t->io_head = t->io_tail = NULL;
    t->user_head = t->user_tail = NULL;
    t->nthread = attr->nthread;

    if ((t->tids = (pthread_t *)calloc(t->nthread, sizeof(pthread_t))) == NULL) {
        mln_socket_close(fds[0]);
        mln_socket_close(fds[1]);
        return -1;
    }
    for (i = 0; i < t->nthread; ++i) {
        if (pthread_create(t->tids + i, NULL, t->entry, t->args) != 0) {
            mln_iothread_destroy(t);
            return -1;
        }
        if (pthread_detach(t->tids[i]) != 0) {
            mln_iothread_destroy(t);
            return -1;
        }
    }

    return 0;
}

void mln_iothread_destroy(mln_iothread_t *t)
{
    if (t == NULL) return;
    if (t->tids != NULL) {
        mln_u32_t i;
        for (i = 0; i < t->nthread; ++i) {
            pthread_cancel(t->tids[i]);
            pthread_join(t->tids[i], NULL);
        }
        free(t->tids);
    }
    mln_socket_close(t->io_fd);
    mln_socket_close(t->user_fd);
}

int mln_iothread_send(mln_iothread_t *t, mln_u32_t type, void *data, mln_iothread_ep_type_t to, int feedback)
{
    int fd;
    pthread_mutex_t *plock;
    mln_iothread_msg_t *msg;
    mln_iothread_msg_t **head, **tail;

    if (to == io_thread) {
        fd = t->user_fd;
        plock = &(t->io_lock);
        head = &(t->io_head);
        tail = &(t->io_tail);
    } else {
        fd = t->io_fd;
        plock = &(t->user_lock);
        head = &(t->user_head);
        tail = &(t->user_tail);
    }

    if ((msg = mln_iothread_msg_new(type, data, feedback)) == NULL)
        return -1;

    if (feedback)
        pthread_mutex_lock(&(msg->mutex));

    pthread_mutex_lock(plock);

    mln_iothread_msg_chain_add(head, tail, msg);

    if (*head == *tail && *head == msg && send(fd, " ", 1, 0) != 1) {
        mln_iothread_msg_chain_del(head, tail, msg);
        pthread_mutex_unlock(plock);
        pthread_mutex_unlock(&(msg->mutex));
        mln_iothread_msg_free(msg);
        return 1;
    }

    pthread_mutex_unlock(plock);

    if (feedback) {
        pthread_mutex_lock(&(msg->mutex));
        pthread_mutex_unlock(&(msg->mutex));
        mln_iothread_msg_free(msg);
    }

    return 0;
}

int mln_iothread_recv(mln_iothread_t *t, mln_iothread_ep_type_t from)
{
    int fd;
    mln_s8_t buf[512];
    pthread_mutex_t *plock;
    mln_iothread_msg_t *msg;
    mln_iothread_msg_t **head, **tail;

    if (from == io_thread) {
        fd = t->user_fd;
        plock = &(t->user_lock);
        head = &(t->user_head);
        tail = &(t->user_tail);
    } else {
        fd = t->io_fd;
        plock = &(t->io_lock);
        head = &(t->io_head);
        tail = &(t->io_tail);
    }

    pthread_mutex_lock(plock);

    while (1) {
        if (recv(fd, buf, sizeof(buf), 0) <= 0)
            break;
    }

    while ((msg = *head) != NULL) {
        mln_iothread_msg_chain_del(head, tail, msg);
        if (t->handler != NULL)
            t->handler(t, from, msg->type, msg->data);
        if (msg->feedback) {
            pthread_mutex_unlock(&(msg->mutex));
        } else {
            mln_iothread_msg_free(msg);
        }
    }

    pthread_mutex_unlock(plock);

    return 0;
}


static inline mln_iothread_msg_t *mln_iothread_msg_new(mln_u32_t type, void *data, int feedback)
{
    mln_iothread_msg_t *msg = (mln_iothread_msg_t *)malloc(sizeof(mln_iothread_msg_t));
    if (msg == NULL)
        return NULL;

    msg->feedback = feedback;
    msg->type = type;
    msg->data = data;
    msg->prev = msg->next = NULL;

    if (feedback && pthread_mutex_init(&(msg->mutex), NULL) != 0) {
        free(msg);
        return NULL;
    }
    return msg;
}

static inline void mln_iothread_msg_free(mln_iothread_msg_t *msg)
{
    if (msg == NULL)
        return;

    if (msg->feedback)
        pthread_mutex_destroy(&(msg->mutex));

    free(msg);
}

static inline void mln_iothread_fd_nonblock_set(int fd)
{
#if defined(WIN32)
    u_long opt = 1;
    ioctlsocket(fd, FIONBIO, &opt);
#else
    int flg = fcntl(fd, F_GETFL, NULL);
    fcntl(fd, F_SETFL, flg | O_NONBLOCK);
#endif
}
