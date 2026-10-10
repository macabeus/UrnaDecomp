#pragma once

#include "asn1/asn1.h"

namespace comum::asn {

// Converts between a generated ASN.1 entity and a comum data class: Converte/Desconverte validate the entity and
// call the virtual pair; the default DoConverte/DoDesconverte throw (records at lines 89 and 98).
template <typename ENTIDADE, typename DADO>
class IConversorASN {
public:
    typedef ENTIDADE TEntidade;
    typedef DADO TDado;

    virtual ~IConversorASN() {}

// (original lines 17-84: content not recovered)
#line 85
protected:
    virtual TEntidade DoConverte(const TDado& dado) const;

    // (original lines 88-93: content not recovered)
#line 94
    virtual TDado DoDesconverte(const TEntidade& entidade) const;
};

} // namespace comum::asn

// <stubs>
// f11483 · 1181 B · unmatched · placement: link order · name unknown
// void f11483(i32, i32, i32) { ... }

// f11482 · 594 B · unmatched · placement: link order · name unknown
// void f11482(i32, i32, i32) { ... }

// f11457 · 2011 B · unmatched · placement: link order · name unknown
// void f11457(i32, i32, i32) { ... }

// f11383 · 753 B · unmatched · placement: link order · name unknown
// void f11383(i32, i32, i32) { ... }

// f11382 · 691 B · unmatched · placement: link order · name unknown
// void f11382(i32, i32, i32) { ... }

// f5711 · 20 B · unmatched · placement: link order · name unknown
// void f5711(i32, i32, i32) { ... }

// f5692 · 20 B · unmatched · placement: link order · name unknown
// void f5692(i32, i32, i32) { ... }

// f5688 · 20 B · unmatched · placement: link order · name unknown
// void f5688(i32, i32, i32) { ... }

// f3730 · 20 B · unmatched · placement: link order · name unknown
// void f3730(i32, i32, i32) { ... }

// f5828 · 575 B · unmatched · placement: link order · name unknown
// ? f5828(i32, i32) { ... }

// f3799 · 20 B · unmatched · placement: link order · name unknown
// void f3799(i32, i32, i32) { ... }

// f11458 · 1955 B · unmatched · placement: link order · name unknown
// void f11458(i32, i32, i32) { ... }

// f11454 · 1522 B · unmatched · placement: link order · name unknown
// void f11454(i32, i32, i32) { ... }

// f11449 · 29 B · unmatched · placement: link order · name unknown
// void f11449(i32, i32, i32) { ... }

// f11421 · 29 B · unmatched · placement: link order · name unknown
// void f11421(i32, i32, i32) { ... }

// f11415 · 29 B · unmatched · placement: link order · name unknown
// void f11415(i32, i32, i32) { ... }

// f11380 · 29 B · unmatched · placement: link order · name unknown
// void f11380(i32, i32, i32) { ... }

// f11374 · 29 B · unmatched · placement: link order · name unknown
// void f11374(i32, i32, i32) { ... }

// f11372 · 29 B · unmatched · placement: link order · name unknown
// void f11372(i32, i32, i32) { ... }

// f11370 · 29 B · unmatched · placement: link order · name unknown
// void f11370(i32, i32, i32) { ... }

// f11362 · 29 B · unmatched · placement: link order · name unknown
// void f11362(i32, i32, i32) { ... }

// f10266 · 1338 B · unmatched · placement: link order · name unknown
// void f10266(i32, i32, i32) { ... }

// f5721 · 20 B · unmatched · placement: link order · name unknown
// void f5721(i32, i32, i32) { ... }

// f5716 · 20 B · unmatched · placement: link order · name unknown
// void f5716(i32, i32, i32) { ... }

// f5715 · 20 B · unmatched · placement: link order · name unknown
// void f5715(i32, i32, i32) { ... }

// f5710 · 1431 B · unmatched · placement: link order · name unknown
// void f5710(i32, i32, i32) { ... }

// f5697 · 20 B · unmatched · placement: link order · name unknown
// void f5697(i32, i32, i32) { ... }

// f5696 · 20 B · unmatched · placement: link order · name unknown
// void f5696(i32, i32, i32) { ... }

// f5691 · 20 B · unmatched · placement: link order · name unknown
// void f5691(i32, i32, i32) { ... }

// f5686 · 20 B · unmatched · placement: link order · name unknown
// void f5686(i32, i32, i32) { ... }

// f5684 · 574 B · unmatched · placement: link order · name unknown
// ? f5684(i32, i32) { ... }

// f5680 · 739 B · unmatched · placement: link order · name unknown
// void f5680(i32, i32, i32) { ... }

// f3738 · 20 B · unmatched · placement: link order · name unknown
// void f3738(i32, i32, i32) { ... }

// f3737 · 20 B · unmatched · placement: link order · name unknown
// void f3737(i32, i32, i32) { ... }

// f3727 · 20 B · unmatched · placement: link order · name unknown
// void f3727(i32, i32, i32) { ... }

// f3726 · 20 B · unmatched · placement: link order · name unknown
// void f3726(i32, i32, i32) { ... }

// f11446 · 29 B · unmatched · placement: link order · name unknown
// void f11446(i32, i32, i32) { ... }

// f11444 · 29 B · unmatched · placement: link order · name unknown
// void f11444(i32, i32, i32) { ... }

// f11439 · 29 B · unmatched · placement: link order · name unknown
// void f11439(i32, i32, i32) { ... }

// f11424 · 3377 B · unmatched · placement: link order · name unknown
// void f11424(i32, i32, i32) { ... }

// f11409 · 29 B · unmatched · placement: link order · name unknown
// void f11409(i32, i32, i32) { ... }

// f11376 · 29 B · unmatched · placement: link order · name unknown
// void f11376(i32, i32, i32) { ... }

// f11368 · 29 B · unmatched · placement: link order · name unknown
// void f11368(i32, i32, i32) { ... }

// f11364 · 29 B · unmatched · placement: link order · name unknown
// void f11364(i32, i32, i32) { ... }

// f11360 · 29 B · unmatched · placement: link order · name unknown
// void f11360(i32, i32, i32) { ... }

// f11358 · 29 B · unmatched · placement: link order · name unknown
// void f11358(i32, i32, i32) { ... }

// f10275 · 29 B · unmatched · name unknown
// void f10275(i32, i32, i32) { ... }

// f10272 · 29 B · unmatched · name unknown
// void f10272(i32, i32, i32) { ... }

// f10265 · 29 B · unmatched · name unknown
// void f10265(i32, i32, i32) { ... }
// </stubs>
