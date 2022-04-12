#define _XOPEN_SOURCE 700
#include <signal.h>
#include <unistd.h>

namespace TurboEvents {

void segvHandler(int) {
  // write() has a __attribute__((warn_unused_result)), and the usual
  // "trick" of casting to void is deliberately ignored by GCC.
  // This choice has been discussed since 2005.
  //
  // There is no recovery from SIGSEGV at this stage, add a dummy
  // variable that is unused to silence GCC.
  [[maybe_unused]] auto dummy = write(2, "stack overflow\n", 15);
  _exit(127);
}

void installSegvHandler() {
  static char stack[SIGSTKSZ];
  stack_t ss = {
      .ss_sp = stack,
      .ss_flags = 0,
      .ss_size = SIGSTKSZ,
  };
  sigaltstack(&ss, NULL);
  struct sigaction sa;
  sa.sa_handler = segvHandler;
  sigfillset(&sa.sa_mask);
  sa.sa_flags = SA_ONSTACK;
  sigaction(SIGSEGV, &sa, NULL);
}

} // namespace TurboEvents
