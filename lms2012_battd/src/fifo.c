#include <fifo.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <signal.h>

static int            fdOut       = -1;
static battd_memory_t *pMem       = NULL;
static volatile bool  quit        = false;
static uint32_t       sendCounter = 0;

static void softExit(int signo);

bool fifo_open(battd_msg_t *msg) {
    int result = unlink("/tmp/battd");
    if (result < 0 && errno != ENOENT) {
        perror("f/unlink1");
        return false;
    }
    result = unlink("/tmp/battd.tmp");
    if (result < 0 && errno != ENOENT) {
        perror("f/unlink2");
        return false;
    }
    fdOut = open("/tmp/battd.tmp", O_CREAT | O_EXCL | O_RDWR, 00777);
    if (fdOut < 0) {
        perror("f/open");
        return false;
    }
    result = ftruncate(fdOut, sizeof(battd_memory_t));
    if (result < 0) {
        perror("f/trunc");
        return false;
    }
    pMem = mmap(NULL, sizeof(battd_memory_t),
                PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED,
                fdOut, 0);
    if (pMem == MAP_FAILED) {
        perror("f/mmap");
        return false;
    }

    pthread_mutexattr_t attrs;
    if (pthread_mutexattr_init(&attrs) < 0) {
        perror("f/mtx1");
        return false;
    }
    if (pthread_mutexattr_setpshared(&attrs, PTHREAD_PROCESS_SHARED) < 0) {
        perror("f/mtx2");
        return false;
    }
    if (pthread_mutex_init(&pMem->Mutex, &attrs) < 0) {
        perror("f/mtx3");
        return false;
    }
    if (pthread_mutexattr_destroy(&attrs) < 0) {
        perror("f/mtx4");
        return false;
    }

    pthread_mutex_lock(&pMem->Mutex);
    pMem->CounterRx = 0;
    pMem->CounterTx = 0;
    pthread_mutex_unlock(&pMem->Mutex);

    struct sigaction sa;
    sa.sa_handler = softExit;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGTERM);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    sendCounter = 0;
    quit        = false;

    bool tmp = false;
    fifo_send(msg, &tmp);

    if (link("/tmp/battd.tmp", "/tmp/battd") < 0) {
        perror("f/link");
        return false;
    }
    result = unlink("/tmp/battd.tmp");
    if (result < 0 && errno != ENOENT) {
        perror("f/unlink4");
        return false;
    }
    return true;
}

void fifo_close(void) {
    if (pMem) {
        pthread_mutex_destroy(&pMem->Mutex);
        munmap(pMem, sizeof(battd_memory_t));
        pMem = NULL;
    }
    if (fdOut >= 0) {
        close(fdOut);
        fdOut = -1;
    }
    unlink("/tmp/battd.tmp");
    unlink("/tmp/battd");
}

void fifo_send(battd_msg_t *msg, bool *pResponsive) {
    pthread_mutex_lock(&pMem->Mutex);
    *pResponsive = pMem->CounterRx == sendCounter;
    pMem->CounterTx = sendCounter++;
    pMem->Message = *msg;
    pthread_mutex_unlock(&pMem->Mutex);
}

bool fifo_should_continue(void) {
    return !quit;
}

static void softExit(int signo) {
    quit = true;
}
