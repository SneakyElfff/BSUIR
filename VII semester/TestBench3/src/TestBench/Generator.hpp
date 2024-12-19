#pragma once

#include </Users/nina/Library/CloudStorage/OneDrive-Personal/Документы/Uni/АПЦУ/Lab3/TestBench3/src/Emulation/Generator.hpp>

auto TestBenchGenerator(void) -> void {
	Generator device;
	std::ofstream log;
	log.open("/Users/nina/Library/CloudStorage/OneDrive-Personal/Документы/Uni/АПЦУ/Lab3/TestBench3/tests/Generator.txt");
	log << device.header();
	    for (usize int_p = 0; int_p < (1 << N); ++int_p) {
            for (usize int_g = 0; int_g < (1 << N); ++int_g) {
                for (std_logic int_cn : {L, H}) {
                    device.Ps = vectorize(int_p);
                    device.Gs = vectorize(int_g);
                    device.Cn = int_cn;
                    log << device.snapshot();
                }
            }
        }
	log << device.x();
	log.close();
}
