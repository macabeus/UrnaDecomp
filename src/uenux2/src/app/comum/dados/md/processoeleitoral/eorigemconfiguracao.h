#pragma once

namespace comum::md {

// Where a configuration came from. CConversorOrigemConfiguracao::DoConverte fixes only ORIGEM_COMUNITARIA = 2;
// the enumerator names and ORIGEM_OFICIAL's value are not in the target.
enum EOrigemConfiguracao {
    ORIGEM_OFICIAL = 1,
    ORIGEM_COMUNITARIA = 2,
};

} // namespace comum::md
