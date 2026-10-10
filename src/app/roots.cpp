// Every root the skeleton keeps alive, called once. The per-library root sets:
//   EM_JS      the 34 JS functions of src/web/wasm_shim.cpp (emcc drops an EM_JS function nobody imports)
//   sqlite     build/out/app/gen/sqlite_roots.c  (the sqlite3_* API functions present in the target, attribution.tsv)
//   openssl    build/out/app/gen/openssl_roots.c (tools/libs/openssl/roots.txt, 258 symbols the app calls)
//   7zip       tools/libs/7zip/driver.cpp via roots_7zip.cpp   (UpdateArchive / Open3 / Extract)
//   rhvoice    tools/libs/rhvoice/driver.cpp via roots_rhvoice.cpp (engine::create + document synthesis)
//   boost      tools/libs/boost/probe.cpp via roots_boost.cpp  (regex, signals2, date_time, lexical_cast, function)
#include "roots.h"

int urna_roots_ecourna(int a);   // src/app/roots_ecourna.cpp: the reconstructed ecourna units
int urna_roots_uenux2(int a);    // src/app/roots_uenux2.cpp: the reconstructed uenux2 units

extern "C" {
// EM_JS imports (src/web/wasm_shim.cpp)
void js_use_external_keyboard(void);
int js_ler_dimensao_tela(int, int);
void js_wasm_beep_init(void);
void js_wasm_beep_set_volume(int);
void js_wasm_beep_queue(int, int);
int wasm_input_has_key(void);
int wasm_input_get_key(void);
void wasm_input_clear(void);
void js_thread_log(int);
void js_emit_event(int, int);
void js_push_key(int);
void js_console_error(int);
void js_wasm_web_sound_play_wav(int, int, int, int, int, int);
void js_wasm_web_sound_pause(void);
void js_wasm_web_sound_stop(void);
void js_wasm_web_sound_mute(int);
int js_wasm_web_sound_get_status(void);
void js_wasm_web_sound_wait_async(int);
void js_wasm_web_sound_cancel_wait(int);
void js_resource_log(int, int, int, int);
void js_init(int, int);
void js_clear(int);
void js_fill(int, int, int, int, int);
void js_line(int, int, int, int, int, int);
void js_text(int, int, int, int, int, int, int, int, int);
int js_measure_text_width(int, int, int, int);
void js_circle(int, int, int, int, int);
void js_round_rect(int, int, int, int, int, int, int);
void js_gray_gradient(int, int, int, int);
void js_path(int, int, int, int, int, int);
void js_image(int, int, int, int, int, int);
void js_log(int);
void js_mt_set(int, int);
double js_obter_data_hora_local_navegador(void);
// generated root callers (gen-probe-main.py output compiled with -Dmain=<name>)
int urna_sqlite_roots(void);
int urna_openssl_roots(void);
// library drivers (EMSCRIPTEN_KEEPALIVE neutralised, see roots_*.cpp)
int probe_update(const char *);
int probe_extract(const char *);
int probe_speak(const char *, const char *);
int probe_regex(const char *, const char *);
int probe_app_signal(unsigned long, unsigned long, const char *);
int probe_signals(int);
int probe_datetime(const char *);
int probe_lexical(const char *);
int probe_function(int);
}

volatile int urna_never = 0;

int urna_roots(int a) {
  if (!urna_never) return a;
  int r = a + urna_roots_ecourna(a);
  r += urna_roots_uenux2(a);
  js_use_external_keyboard();
  r += js_ler_dimensao_tela(a, a);
  js_wasm_beep_init();
  js_wasm_beep_set_volume(a);
  js_wasm_beep_queue(a, a);
  r += wasm_input_has_key() + wasm_input_get_key();
  wasm_input_clear();
  js_thread_log(a);
  js_emit_event(a, a);
  js_push_key(a);
  js_console_error(a);
  js_wasm_web_sound_play_wav(a, a, a, a, a, a);
  js_wasm_web_sound_pause();
  js_wasm_web_sound_stop();
  js_wasm_web_sound_mute(a);
  r += js_wasm_web_sound_get_status();
  js_wasm_web_sound_wait_async(a);
  js_wasm_web_sound_cancel_wait(a);
  js_resource_log(a, a, a, a);
  js_init(a, a);
  js_clear(a);
  js_fill(a, a, a, a, a);
  js_line(a, a, a, a, a, a);
  js_text(a, a, a, a, a, a, a, a, a);
  r += js_measure_text_width(a, a, a, a);
  js_circle(a, a, a, a, a);
  js_round_rect(a, a, a, a, a, a, a);
  js_gray_gradient(a, a, a, a);
  js_path(a, a, a, a, a, a);
  js_image(a, a, a, a, a, a);
  js_log(a);
  js_mt_set(a, a);
  r += (int)js_obter_data_hora_local_navegador();
  r += urna_sqlite_roots() + urna_openssl_roots();
  const char *s = reinterpret_cast<const char *>(a);
  r += probe_update(s) + probe_extract(s) + probe_speak(s, s);
  r += probe_regex(s, s) + probe_app_signal(a, a, s) + probe_signals(a) + probe_datetime(s) + probe_lexical(s) + probe_function(a);
  return r;
}
