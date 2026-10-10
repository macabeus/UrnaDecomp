# The first ecourna unit: the voter-identifier classes of app/dados (IIdentificadorEleitor, CNumeroCPF,
# CNumeroInscricaoEleitoral, CNumeroIdentificacaoLivre, CFabricaIdentificadorEleitor) and what they call: CError
# (api/exception), base64 (api/security) and CStringUtils (api/util). The ecourna flags were measured on these units
# (docs/reference/app-flags.md); src/app/roots_ecourna.cpp calls them with the target's call-site counts.
ECOURNA_SRCS += api/exception/cerror.cpp api/security/base64.cpp api/util/cstringutils.cpp app/dados/iidentificadoreleitor.cpp \
                app/dados/cnumerocpf.cpp app/dados/cnumeroinscricaoeleitoral.cpp app/dados/cnumeroidentificacaolivre.cpp \
                app/dados/cfabricaidentificadoreleitor.cpp
