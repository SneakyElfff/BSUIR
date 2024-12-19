#pragma once

#include </Users/nina/Library/CloudStorage/OneDrive-Personal/Документы/Uni/АПЦУ/Lab3/TestBench3/src/Emulation/Counter.hpp>

static constexpr const std_logic IDC = H;

auto TestBenchCounter(void) -> void {
	Counter device;
	std::ofstream log;
	log.open("/Users/nina/Library/CloudStorage/OneDrive-Personal/Документы/Uni/АПЦУ/Lab3/TestBench3/tests/Counter.txt");
	log << device.header();
	// BEGIN TestBench
	// Async clear (3 CLK)
	device.OE = L;
	device.ACLR = L;
	device.SCLR = H;
	device.LOAD = H;
	device.ENP = H;
	device.ENT = H;
	device.UD = H;
	device.D.fill(H);
	for (usize n = 1; n <= 3; ++n) {
		if (n == 3) {
			device.ENP = L;
			device.ENT = L;
		}
		device.CLK = L;
		log << device.snapshot();
	}

	// Count up (4 CLK)
	device.OE = L;
	device.ACLR = H;
	device.SCLR = H;
	device.LOAD = H;
	device.UD = H;
	device.D.fill(H);
	device.CLK = H;
	log << device.snapshot();
	device.CLK = L;
	log << device.snapshot();
	device.CLK = H;
	log << device.snapshot();
	device.SCLR = L;
	device.LOAD = H;
	device.ENP = H;
	device.ENT = H;
	device.CLK = L;
	log << device.snapshot();

	// Sync clear (2 CLK)
	device.CLK = H;
	log << device.snapshot();
	device.SCLR = H;
	device.LOAD = L;
	device.UD = H;
	device.D[0] = L;
	device.D[1] = H;
	device.D[2] = H;
	device.D[3] = H;
	device.CLK = L;
	log << device.snapshot();

	// Sync load (2 CLK)
	device.CLK = H;
	log << device.snapshot();
	device.SCLR = H;
	device.LOAD = H;
	device.ENP = L;
	device.ENT = L;
	device.UD = H;
	device.D.fill(H);
	device.CLK = L;
	log << device.snapshot();

	// Count up (13 CLK)
	device.OE = L;
	device.ACLR = H;
	device.SCLR = H;
	device.LOAD = H;
	device.ENP = L;
	device.ENT = L;
	device.UD = H;
	device.D.fill(H);
	for (usize n = 1; n <= 7; ++n) {
		device.CLK = H;
		log << device.snapshot();
		if (n == 4) {
			device.OE = H;
			device.ACLR = H;
		} else if (n == 6) {
			device.OE = L;
			device.ACLR = H;
		}
		if (n != 7) {
			device.CLK = L;
			log << device.snapshot();
		}
	}

	// Count down (12 CLK)
	device.UD = L;
	device.D.fill(H);
	for (usize n = 1; n <= 7; ++n) {
		device.CLK = L;
		log << device.snapshot();
		device.CLK = H;
		log << device.snapshot();
	}

	// Inhibit counting (6 CLK)
	device.UD = H;
	device.D.fill(H);
	for (usize n = 1; n <= 3; ++n) {
		device.ENP = (n == 3) ? L : H;
		device.ENT = (n == 1) ? L : H;
		device.CLK = L;
		log << device.snapshot();
		device.CLK = H;
		log << device.snapshot();
	}

	// Force unknown state
	log << device.x();

	log.close();
}
