#pragma once

namespace ecourna::api::io {

enum class EFileOperation {
    None,
    Open,
    Close,
    Read,
    Write,
    Seek,
    Tell,
    Sync,
    Mode,
};

const char* Description(EFileOperation operacao);

} // namespace ecourna::api::io
