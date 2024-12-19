library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use std.textio.all;
use ieee.std_logic_textio.all;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity autoTestGenerator2 is
--  Port ( );
end autoTestGenerator2;
    
architecture Behavioral of autoTestGenerator2 is
signal Ps: std_logic_vector(0 to 3) := (others => '0');
signal Gs: std_logic_vector(0 to 3) := (others => '0');
signal Cn: std_logic := '0';

signal P: std_logic := '0';
signal G: std_logic := '0';
signal Cn_z: std_logic := '0';
signal Cn_y: std_logic := '0';
signal Cn_x: std_logic := '0';

signal P2: std_logic := '0';
signal G2: std_logic := '0';
signal Cn_z2: std_logic := '0';
signal Cn_y2: std_logic := '0';
signal Cn_x2: std_logic := '0';

function to_std_logic(H, L: boolean) return std_logic is begin
    if H then
        if L then return 'Z';
        else return 'X';
        end if;
    else
        if L then return '1';
        else return '0';
        end if;
    end if;
end function;

begin
    mapping: entity work.generator(Behavioral) port map (
        Ps => Ps,
        Gs => Gs,
        Cn => Cn,
        P => P,
        G => G,
        Cn_z => Cn_z,
        Cn_y => Cn_y,
        Cn_x => Cn_x
    );

    mapping2: entity work.generator_seq(Behavioral_seq) port map (
        Ps => Ps2,
        Gs => Gs2,
        Cn => Cn2,
        P => P2,
        G => G2,
        Cn_z => Cn_z2,
        Cn_y => Cn_y2,
        Cn_x => Cn_x2
    );

    process
        file input_file: text;
        variable text_row: line;
        variable P3_h, P2_h, P1_h, P0_h, G3_h, G2_h, G1_h, G0_h, Cn_h, P_h, G_h, Cn_z_h, Cn_y_h, Cn_x_h: boolean;
        variable P3_l, P2_l, P1_l, P0_l, G3_l, G2_l, G1_l, G0_l, Cn_l, P_l, G_l, Cn_z_l, Cn_y_l, Cn_x_l: boolean;

    begin
        file_open(input_file, "/Users/nina/Library/CloudStorage/OneDrive-Personal/Документы/Uni/АПЦУ/Lab3/TestBench3/tests/Generator.txt", read_mode);
        readline(input_file, text_row);
        while not endfile(input_file) loop
            readline(input_file, text_row);

            read(text_row, P3_h);
            read(text_row, P3_l);
			read(text_row, P2_h);
            read(text_row, P2_l);
			read(text_row, P1_h);
            read(text_row, P1_l);
			read(text_row, P0_h);
            read(text_row, P0_l);
			read(text_row, G3_h);
            read(text_row, G3_l);
			read(text_row, G2_h);
            read(text_row, G2_l);
			read(text_row, G1_h);
            read(text_row, G1_l);
			read(text_row, G0_h);
            read(text_row, G0_l);
			read(text_row, Cn_h);
            read(text_row, Cn_l);
			read(text_row, P_h);
            read(text_row, P_l);
			read(text_row, G_h);
            read(text_row, G_l);
			read(text_row, Cn_z_h);
            read(text_row, Cn_z_l);
			read(text_row, Cn_y_h);
            read(text_row, Cn_y_l);
			read(text_row, Cn_x_h);
            read(text_row, Cn_x_l);

            Ps <= to_std_logic(P3_h, P3_l) & to_std_logic(P2_h, P2_l) & to_std_logic(P1_h, P1_l) & to_std_logic(P0_h, P0_l);
            Gs <= to_std_logic(G3_h, G3_l) & to_std_logic(G2_h, G2_l) & to_std_logic(G1_h, G1_l) & to_std_logic(G0_h, G0_l);
            Cn <= to_std_logic(Cn_h, Cn_l);

            wait for 0.001 ns;

            assert P = P2
				report "Mismatch detected for P" severity error;

            assert G = G2
				report "Mismatch detected for G" severity error;

            assert Cn_z = Cn_z2
				report "Mismatch detected for Cn_z" severity error;

            assert Cn_y = Cn_y2
				report "Mismatch detected for Cn_y" severity error;

        
            assert Cn_x <= Cn_x2
				report "Mismatch detected for Cn_x" severity error;

            wait for 20 ns;

        end loop;

            file_close(input_file);

            wait;

    end process;

end Behavioral;
