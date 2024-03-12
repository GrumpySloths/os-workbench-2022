#include<os.h>
#include<thread.h>
#define P kmt->sem_wait
#define V kmt->sem_signal
sem_t fill, empty;

void producer() {
  while (1) {
    P(&empty);
    printf("(");
    V(&fill);
  }
}

void consumer() {
  while (1) {
    P(&fill);
    printf(")");
    V(&empty);
  }
}

int main(int argc, char *argv[]) {
  assert(argc == 2);
  kmt->sem_init(&fill,"fill", 0);
  kmt->sem_init(&empty,"empty", atoi(argv[1]));
  for (int i = 0; i < 8; i++) {
    create(producer);
    create(consumer);
  }
}
