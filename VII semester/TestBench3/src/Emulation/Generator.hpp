#pragma once

#include </Users/nina/Library/CloudStorage/OneDrive-Personal/Документы/Uni/АПЦУ/Lab3/TestBench3/src/std.hpp>

class Generator {
public:
    std_logic Cn;
    std_logic_vector Ps, Gs;

public:
    Generator() : Cn(U) {
        Ps.fill(U);
        Gs.fill(U);
    }

public:
    auto P() const -> std_logic {
        return static_cast<std_logic>(Ps[3] || Ps[2] || Ps[1] || Ps[0]);
    }

    auto G() const -> std_logic {
        return static_cast<std_logic>((Gs[3] && Gs[2] && Gs[1] && Gs[0]) ||
                                      (Ps[1] && Gs[3] && Gs[2] && Gs[1]) ||
                                      (Ps[2] && Gs[3] && Gs[2]) ||
                                      (Ps[3] && Gs[3]));
    }

    auto Cn_z() const -> std_logic {
        return static_cast<std_logic>(!((Gs[2] && Gs[1] && Gs[0] && (!Cn)) ||
                                        (Ps[0] && Gs[2] && Gs[1] && Gs[0]) ||
                                        (Ps[1] && Gs[2] && Gs[1]) ||
                                        (Ps[2] && Gs[2])));
    }

    auto Cn_y() const -> std_logic {
        return static_cast<std_logic>(!((Gs[1] && Gs[0] && (!Cn)) ||
                                        (Ps[0] && Gs[1] && Gs[0]) ||
                                        (Ps[1] && Gs[1])));
    }

    auto Cn_x() const -> std_logic {
        return static_cast<std_logic>(!((Gs[0] && (!Cn)) ||
                                        (Ps[0] && Gs[0])));
    }

    auto header() const -> std::string_view {
        static const std::string_view header_str =
            "P3          P2          P1          P0          G3          G2          G1          G0          Cn          P           G           Cn_z        Cn_y        Cn_x         \n";
        return header_str;
    }

    auto snapshot() const -> std::string {
        auto buffer = std::stringstream();
        for (usize i = 0; i < N; ++i)
            buffer << Ps[N - 1 - i] << ' ';
        for (usize i = 0; i < N; ++i)
            buffer << Gs[N - 1 - i] << ' ';
        buffer << Cn << ' '
               << P() << ' '
               << G() << ' '
               << Cn_z() << ' '
               << Cn_y() << ' '
               << Cn_x() << '\n';
        return buffer.str();
    }

    auto x() const -> std::string {
        auto buffer = std::stringstream();
        for (usize i = 0; i < N * 2 + 5; ++i)
            buffer << X << ' ';
        buffer << '\n';
        return buffer.str();
    }
};