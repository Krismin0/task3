#include "lab3.h"
#include <windows.h>
#include <iostream>

#include <stdio.h>

HANDLE tid[11];
HANDLE lock;
HANDLE sem_h, sem_i, sem_k, sem_a, sem_b, sem_end;

// 26
unsigned int lab3_thread_graph_id() { return 6; }
const char *lab3_unsynchronized_threads() { return "bcd"; }
const char *lab3_sequential_threads() { return "hik"; }

bool start_t(HANDLE *t, DWORD (*f)(LPVOID)) {
  *t = CreateThread(0, 0, f, NULL, 0, 0);
  return *t != NULL;
}
void wait(HANDLE h) { WaitForSingleObject(h, INFINITE); }

void sem_post(HANDLE s) { ReleaseSemaphore(s, 1, NULL); }

void log_t(const char *t) {
  wait(lock);
  std::cout << t << std::flush;
  ReleaseMutex(lock);
  computation();
}

DWORD WINAPI thread_a(LPVOID);
DWORD WINAPI thread_b(LPVOID);
DWORD WINAPI thread_c(LPVOID);
DWORD WINAPI thread_d(LPVOID);
DWORD WINAPI thread_e(LPVOID);
DWORD WINAPI thread_f(LPVOID);
DWORD WINAPI thread_g(LPVOID);
DWORD WINAPI thread_h(LPVOID);
DWORD WINAPI thread_i(LPVOID);
DWORD WINAPI thread_k(LPVOID);
DWORD WINAPI thread_m(LPVOID);

DWORD WINAPI thread_a(LPVOID) {
  for (int i = 0; i < 3; i++) {
    log_t("a");
  }
  if (!start_t(&tid[1], thread_b)) {
    std::cerr << "start_t error";
  }
  if (!start_t(&tid[2], thread_c)) {
    std::cerr << "start_t error";
  }
  if (!start_t(&tid[3], thread_d)) {
    std::cerr << "start_t error";
  }
  return 0;
}

DWORD WINAPI thread_b(LPVOID) {
  for (int i = 0; i < 3; i++) {
    log_t("b");
  }
  return 0;
}

DWORD WINAPI thread_c(LPVOID) {
  for (int i = 0; i < 3; i++) {
    log_t("c");
  }
  return 0;
}

DWORD WINAPI thread_d(LPVOID) {
  for (int i = 0; i < 3; i++) {
    log_t("d");
  }
  wait(tid[1]);
  wait(tid[2]);

  if (!start_t(&tid[4], thread_e)) {
    std::cerr << "start_t error";
  }
  if (!start_t(&tid[6], thread_g)) {
    std::cerr << "start_t error";
  }
  for (int i = 0; i < 3; i++) {
    log_t("d");
  }
  return 0;
}

DWORD WINAPI thread_e(LPVOID) {
  for (int i = 0; i < 3; i++) {
    log_t("e");
  }
  return 0;
}

DWORD WINAPI thread_f(LPVOID) {
  for (int i = 0; i < 3; i++) {
    log_t("f");
  }
  return 0;
}

DWORD WINAPI thread_g(LPVOID) {
  for (int i = 0; i < 3; i++) {
    log_t("g");
  }
  wait(tid[3]);
  wait(tid[4]);

  if (!start_t(&tid[5], thread_f)) {
    std::cerr << "start_t error";
  }
  if (!start_t(&tid[7], thread_h)) {
    std::cerr << "start_t error";
  }
  for (int i = 0; i < 3; i++) {
    log_t("g");
  }
  return 0;
}

DWORD WINAPI thread_h(LPVOID) {
  for (int i = 0; i < 3; i++) {
    log_t("h");
  }
  wait(tid[6]);
  wait(tid[5]);

  if (!start_t(&tid[8], thread_i)) {
    std::cerr << "start_t error";
  }
  if (!start_t(&tid[9], thread_k)) {
    std::cerr << "start_t error";
  }

  for (int i = 0; i < 3; i++) {
    wait(sem_h);
    log_t("h");
    sem_post(sem_i);
  }
  return 0;
}

DWORD WINAPI thread_i(LPVOID) {
  for (int i = 0; i < 3; i++) {
    wait(sem_i);
    log_t("i");
    sem_post(sem_k);
  }
  return 0;
}

DWORD WINAPI thread_k(LPVOID) {
  for (int i = 0; i < 3; i++) {
    wait(sem_k);
    log_t("k");
    sem_post(sem_h);
  }
  if (!start_t(&tid[10], thread_m)) {
    std::cerr << "start_t error";
  }
  for (int i = 0; i < 3; i++) {
    log_t("k");
  }
  wait(tid[10]);
  sem_post(sem_end);
  return 0;
}

DWORD WINAPI thread_m(LPVOID) {
  for (int i = 0; i < 3; i++) {
    log_t("m");
  }
  return 0;
}

int lab3_init() {
  lock = CreateMutex(NULL, false, NULL);
  if (lock == NULL) {
    std::cerr << "CreateMutex error: " << GetLastError() << std::endl;
    return 1;
  }

  sem_end = CreateSemaphore(NULL, 0, 1, NULL);
  if (sem_end == NULL) {
    std::cerr << "CreateSemaphore error: " << GetLastError() << std::endl;
    return 1;
  }
  sem_h = CreateSemaphore(NULL, 1, 1, NULL);
  if (sem_h == NULL) {
    std::cerr << "CreateSemaphore error: " << GetLastError() << std::endl;
    return 1;
  }
  sem_i = CreateSemaphore(NULL, 0, 1, NULL);
  if (sem_i == NULL) {
    std::cerr << "CreateSemaphore error: " << GetLastError() << std::endl;
    return 1;
  }
  sem_k = CreateSemaphore(NULL, 0, 1, NULL);
  if (sem_k == NULL) {
    std::cerr << "CreateSemaphore error: " << GetLastError() << std::endl;
    return 1;
  }

  if (!start_t(&tid[0], thread_a)) {
    std::cerr << "start_t error";
    return 1;
  }

  wait(sem_end);

  for (int i = 0; i < 11; i++) {
    CloseHandle(tid[i]);
  }
  CloseHandle(lock);
  CloseHandle(sem_h);
  CloseHandle(sem_i);
  CloseHandle(sem_k);
  CloseHandle(sem_end);

  std::cout << std::endl;

  return 0;
}
