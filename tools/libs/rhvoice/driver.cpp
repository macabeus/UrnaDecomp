// Keep-alive driver for the RHVoice probe: the app (api::CRHVoiceTextToSpeech) drives the C++ core API — the C API
// of src/lib/lib.cpp is not linked (none of its error literals is in the target) — so the probe does the same:
// create an engine, synthesise a plain-text and an SSML document into a client. Exported so wasm-opt keeps this
// reachable set (a bare library link keeps only ~100 functions).
#include <emscripten.h>

#include <string>

#include "core/client.hpp"
#include "core/document.hpp"
#include "core/engine.hpp"

namespace {
class probe_client : public RHVoice::client {
 public:
  bool play_speech(const short *samples, std::size_t count) override { return samples != nullptr && count > 0; }
  RHVoice::event_mask get_supported_events() const override { return RHVoice::event_mask(-1); }
  bool process_mark(const std::string &name) override { return !name.empty(); }
  bool play_audio(const std::string &src) override { return !src.empty(); }
  bool set_sample_rate(int sample_rate) override { return sample_rate > 0; }
  bool sentence_starts(std::size_t, std::size_t) override { return true; }
  bool sentence_ends(std::size_t, std::size_t) override { return true; }
  bool word_starts(std::size_t, std::size_t) override { return true; }
  bool word_ends(std::size_t, std::size_t) override { return true; }
  void done() override {}
};
}  // namespace

extern "C" {

EMSCRIPTEN_KEEPALIVE int probe_speak(const char *text, const char *profile) {
  RHVoice::engine::init_params p;
  p.data_path = "/share/RHVoice";
  p.config_path = "/etc/RHVoice";
  std::shared_ptr<RHVoice::engine> e = RHVoice::engine::create(p);
  std::string s(text);
  probe_client client;
  RHVoice::voice_profile vp = e->create_voice_profile(std::string(profile));
  std::unique_ptr<RHVoice::document> d = RHVoice::document::create_from_plain_text(e, s.begin(), s.end(), RHVoice::content_text, vp);
  d->set_owner(client);
  d->synthesize();
  std::unique_ptr<RHVoice::document> d2 = RHVoice::document::create_from_ssml(e, s.begin(), s.end(), vp);
  d2->set_owner(client);
  d2->synthesize();
  int n = 0;
  for (const RHVoice::voice_profile &q : e->get_voice_profiles()) {
    n += (int)q.get_name().size();
  }
  return n;
}
}
