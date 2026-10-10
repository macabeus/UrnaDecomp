#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <memory>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <time.h>
#include <emscripten.h>

EM_JS(void, js_probe_log, (const char* msg), {
  if (Module.uenuxDebug) {
    console.debug("[probe]", UTF8ToString(msg));
  }
});

extern "C" {
EMSCRIPTEN_KEEPALIVE int probeKeepAlive(int x) { return x * 2; }
int probeExported(int x) { return x + 1; }
}

static void tick(void*) { printf("tick\n"); }

int main(int argc, char** argv) {
  std::string s = "hello ";
  s += argv[0] ? argv[0] : "";
  auto p = std::make_shared<std::string>(s);
  void* m = malloc(1234);
  memset(m, 1, 1234);
  free(m);
  char* d = (char*)malloc(100);
  strcpy(d, p->c_str());
  printf("%s %zu\n", d, strlen(d));
  free(d);
  try {
    if (argc < 100) throw std::runtime_error("boom");
  } catch (const std::exception& e) {
    printf("caught %s\n", e.what());
  }
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in a{};
  a.sin_family = AF_INET;
  a.sin_port = htons(8080);
  a.sin_addr.s_addr = inet_addr("127.0.0.1");
  connect(fd, (sockaddr*)&a, sizeof a);
  sendto(fd, "x", 1, 0, (sockaddr*)&a, sizeof a);
  struct stat st;
  stat("/etc/probe.txt", &st);
  time_t t = time(nullptr);
  struct tm tmv;
  localtime_r(&t, &tmv);
  gmtime_r(&t, &tmv);
  mktime(&tmv);
  strptime("2026", "%Y", &tmv);
  FILE* f = fopen("/etc/probe.txt", "r");
  if (f) {
    char buf[64];
    if (fgets(buf, 64, f)) printf("%s", buf);
    fclose(f);
  }
  js_probe_log("done");
  emscripten_async_call(tick, nullptr, 10);
  return 0;
}
