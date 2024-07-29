#ifndef COMPRESSION_H
#define COMPRESSION_H

namespace shork::cs::intervention {
    struct Compression {
        bool is_rate_sufficient;
        bool is_force_sufficient;
    };
}

#endif // COMPRESSION_H
