#pragma once

namespace api {

/**
 * Conversões de imagens em memória, um byte por pixel, feitas no lugar.
 */
class CBmpConversor {
public:
    static void VerticalFlip(unsigned char* data, const int width, const int height);
    static void InvertColors(unsigned char* data, const int width, const int height);
};

} // namespace api
