// Boost probe: the header-only parts the vota app instantiates (docs/reference/facts/libraries.md §B7: regex with
// re_detail_600::parser_buf<char>, signals2, date_time gregorian/posix_time, function, lexical_cast, optional),
// instantiated the way an app would and kept alive through exported entry points.
#include <emscripten.h>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/regex.hpp>
#include <boost/make_shared.hpp>
#include <boost/signals2.hpp>
#include <boost/weak_ptr.hpp>
#include <string>

extern "C" {

EMSCRIPTEN_KEEPALIVE int probe_regex(const char *pattern, const char *text) {
  boost::regex re(pattern);
  boost::cmatch cm;
  int r = boost::regex_search(text, cm, re) ? (int)cm.size() : -1;
  std::string s(text);
  boost::smatch sm;
  if (boost::regex_match(s, sm, re)) {
    r += (int)sm[0].length();
  }
  std::string replaced = boost::regex_replace(s, re, std::string("$1"));
  boost::sregex_iterator it(s.begin(), s.end(), re), end;
  for (; it != end; ++it) {
    r += (int)it->position();
  }
  boost::regex ic(pattern, boost::regex::icase | boost::regex::perl);
  r += boost::regex_search(s, ic) ? 1 : 0;
  return r + (int)replaced.size();
}

// the app's own signal type (target RTTI: signals2::signal<void (unsigned long, unsigned long, std::string const&)>,
// sp_counted_impl_p<signals2::mutex>, sp_counted_impl_p<optional_last_value<void>>, sp_ms_deleter<signal_impl<...>>)
using app_signal = boost::signals2::signal<void(unsigned long, unsigned long, const std::string &)>;

EMSCRIPTEN_KEEPALIVE int probe_app_signal(unsigned long a, unsigned long b, const char *text) {
  app_signal sig;
  std::size_t acc = 0;
  boost::signals2::connection c = sig.connect([&acc](unsigned long x, unsigned long y, const std::string &s) { acc += x + y + s.size(); });
  sig(a, b, std::string(text));
  app_signal *heap = new app_signal;
  heap->connect([&acc](unsigned long, unsigned long, const std::string &s) { acc += s.size(); });
  (*heap)(a, b, std::string(text));
  delete heap;
  c.disconnect();
  boost::shared_ptr<std::string> sp = boost::make_shared<std::string>(text);
  boost::weak_ptr<std::string> wp(sp);
  if (boost::shared_ptr<std::string> l = wp.lock()) {
    acc += l->size();
  }
  return (int)acc + (int)sig.num_slots();
}

EMSCRIPTEN_KEEPALIVE int probe_signals(int x) {
  boost::signals2::signal<void()> sig;
  int acc = 0;
  sig.connect([&acc] { acc++; });
  boost::signals2::connection c = sig.connect([&acc] { acc += 2; });
  sig();
  c.disconnect();
  sig();
  boost::signals2::signal<void(int)> sig2;
  sig2.connect([&acc](int v) { acc += v; });
  sig2(x);
  boost::signals2::signal<int(int)> sig3;
  sig3.connect([](int v) { return v * 2; });
  boost::optional<int> last = sig3(x);
  return acc + (last ? *last : 0);
}

EMSCRIPTEN_KEEPALIVE int probe_datetime(const char *text) {
  using namespace boost::posix_time;
  using namespace boost::gregorian;
  ptime t = time_from_string(text);
  date d = from_simple_string(text);
  date d2 = from_undelimited_string(text);
  ptime now = second_clock::local_time();
  ptime u = microsec_clock::universal_time();
  std::string out = to_simple_string(t) + to_iso_string(d) + to_iso_extended_string(d2) + to_simple_string(u.time_of_day());
  time_duration diff = now - t;
  date_duration dd = d - d2;
  return (int)out.size() + (int)diff.total_seconds() + (int)dd.days() + (int)d.day_of_week() + (int)t.date().year();
}

EMSCRIPTEN_KEEPALIVE int probe_lexical(const char *text) {
  int i = boost::lexical_cast<int>(text);
  long long l = boost::lexical_cast<long long>(text);
  unsigned u = boost::lexical_cast<unsigned>(text);
  double dbl = boost::lexical_cast<double>(text);
  std::string s = boost::lexical_cast<std::string>(i) + boost::lexical_cast<std::string>(dbl) + boost::lexical_cast<std::string>(l);
  return i + (int)u + (int)s.size() + (int)dbl;
}

EMSCRIPTEN_KEEPALIVE int probe_function(int x) {
  boost::function<int(int)> f;
  boost::optional<int> o;
  if (!o) {
    o = x;
  }
  if (f.empty()) {
    f = [](int v) { return v + 1; };
  }
  boost::function<void()> g;
  int r = f(*o);
  try {
    g();
  } catch (const boost::bad_function_call &) {
    r++;
  }
  return r;
}
}
