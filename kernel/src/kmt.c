#include <os.h>

void kmt_spin_init(spinlock_t *lk, const char *name) { 
    lk->locked = 0;
    lk->name = name;
    lk->cpu = cpu_current();
}

void kmt_spin_lock(spinlock_t *lk) { 
    while (atomic_xchg(&lk->locked, 1));
    lk->cpu = cpu_current();
}

void kmt_spin_unlock(spinlock_t *lk) { 
    atomic_xchg(&lk->locked, 0);
}

void kmt_sem_init(sem_t *sem, const char *name, int value) { 
    sem->val = value;
    sem->name = name;
    kmt_spin_init(&sem->lk, name);
    initQueue(&sem->waits);
}

void kmt_sem_wait(sem_t *sem) { 
    while (1) {
        kmt_spin_lock(&sem->lk);
        if (sem->val > 0) {
            sem->val--;
            kmt_spin_unlock(&sem->lk);
            break;
        }
        else {
            // enqueue(&sem->waits, sem->name);
            kmt_spin_unlock(&sem->lk);
            yield();  //执行yield()函数，切换到其他线程
        }
    }
}

void kmt_sem_signal(sem_t *sem) { 
    kmt_spin_lock(&sem->lk);
    if (isQueueEmpty(&sem->waits)) {
        sem->val++;
    }
    else {
        dequeue(&sem->waits);
        sem->val++;
    }
    kmt_spin_unlock(&sem->lk);
}

MODULE_DEF(kmt) = {
 // TODO
    .spin_init = kmt_spin_init,
    .spin_lock = kmt_spin_lock,
    .spin_unlock = kmt_spin_unlock,
    .sem_init = kmt_sem_init,
    .sem_wait = kmt_sem_wait,
    .sem_signal = kmt_sem_signal,
};
