// TARGET 9406 V(ARG)
#include <algorithm>
#include <iterator>
#include <string>

static bool NaoEhBranco(unsigned char c) { return c > ' '; }
static bool EhBranco(unsigned char c) { return c <= ' '; }

void V1(std::string& s)
{
    if (!s.empty()) {
        const std::string::reverse_iterator r = std::find_if(s.rbegin(), s.rend(), NaoEhBranco);
        s.erase(s.size() - (r - s.rbegin()));
    }
    const size_t inicio = std::find_if(s.begin(), s.end(), NaoEhBranco) - s.begin();
    s.erase(0, inicio);
}

void V2(std::string& s)
{
    const std::string::reverse_iterator r = std::find_if(s.rbegin(), s.rend(), NaoEhBranco);
    s.erase(s.size() - (r - s.rbegin()));
    const size_t inicio = std::find_if(s.begin(), s.end(), NaoEhBranco) - s.begin();
    s.erase(0, inicio);
}

void V3(std::string& s)
{
    if (!s.empty()) {
        const std::string::reverse_iterator r = std::find_if(s.rbegin(), s.rend(), NaoEhBranco);
        s.erase(s.size() - std::distance(s.rbegin(), r));
    }
    s.erase(0, std::distance(s.begin(), std::find_if(s.begin(), s.end(), NaoEhBranco)));
}

void V4(std::string& s)
{
    if (!s.empty()) {
        std::string::reverse_iterator r = s.rbegin();
        while (r != s.rend() && EhBranco(*r)) {
            ++r;
        }
        s.erase(s.size() - (r - s.rbegin()));
    }
    std::string::iterator i = s.begin();
    while (i != s.end() && EhBranco(*i)) {
        ++i;
    }
    s.erase(0, i - s.begin());
}

void V5(std::string& s)
{
    if (!s.empty()) {
        size_t brancos = 0;
        for (std::string::reverse_iterator r = s.rbegin(); r != s.rend() && EhBranco(*r); ++r) {
            ++brancos;
        }
        s.erase(s.size() - brancos);
    }
    size_t inicio = 0;
    for (std::string::iterator i = s.begin(); i != s.end() && EhBranco(*i); ++i) {
        ++inicio;
    }
    s.erase(0, inicio);
}

void V6(std::string& s)
{
    if (s.empty()) {
        return;
    }
    const std::string::reverse_iterator r = std::find_if(s.rbegin(), s.rend(), NaoEhBranco);
    s.erase(s.size() - (r - s.rbegin()));
    const size_t inicio = std::find_if(s.begin(), s.end(), NaoEhBranco) - s.begin();
    s.erase(0, inicio);
}
