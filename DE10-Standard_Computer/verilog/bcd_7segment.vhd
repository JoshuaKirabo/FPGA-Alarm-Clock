LIBRARY ieee;
USE ieee.std_logic_1164.ALL;

ENTITY bcd_7segment IS
    PORT (
        bcd_in : IN STD_LOGIC_VECTOR (3 downto 0); 
        Seven_segment_out: OUT STD_LOGIC_VECTOR (6 downto 0)
    );
END bcd_7segment;

ARCHITECTURE Behavioral OF bcd_7segment IS
BEGIN
    PROCESS(bcd_in)
    BEGIN
        CASE bcd_in IS
            WHEN "0000" =>  -- 0
                Seven_segment_out <= "1000000";  -- Active low for '0'
            WHEN "0001" =>  -- 1
                Seven_segment_out <= "1111001";  -- Active low for '1'
            WHEN "0010" =>  -- 2
                Seven_segment_out <= "0100100";  -- Active low for '2'
            WHEN "0011" =>  -- 3
                Seven_segment_out <= "0110000";  -- Active low for '3'
            WHEN "0100" =>  -- 4
                Seven_segment_out <= "0011001";  -- Active low for '4'
            WHEN "0101" =>  -- 5
                Seven_segment_out <= "0010010";  -- Active low for '5'
            WHEN "0110" =>  -- 6
                Seven_segment_out <= "0000010";  -- Active low for '6'
            WHEN "0111" =>  -- 7
                Seven_segment_out <= "1111000";  -- Active low for '7'
            WHEN "1000" =>  -- 8
                Seven_segment_out <= "0000000";  -- Active low for '8'
            WHEN "1001" =>  -- 9
                Seven_segment_out <= "0010000";  -- Active low for '9'
            WHEN OTHERS =>  -- Default case
                Seven_segment_out <= "1111111";  -- Turns off all segments
        END CASE;
    END PROCESS;
END Behavioral;
