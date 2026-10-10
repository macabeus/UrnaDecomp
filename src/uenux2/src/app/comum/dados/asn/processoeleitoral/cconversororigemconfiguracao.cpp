#include "app/comum/dados/asn/processoeleitoral/cconversororigemconfiguracao.h"

namespace comum::asn {









































CConversorOrigemConfiguracao::TEntidade CConversorOrigemConfiguracao::DoConverte(const TDado& origem) const
{
    return origem == md::ORIGEM_COMUNITARIA ? TEntidade::comunitaria : TEntidade::oficial;
}

} // namespace comum::asn
