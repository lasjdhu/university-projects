-- uart_rx.vhd: UART controller - receiving (RX) side
-- Author(s): Dmitrii Ivanushkin (xivanu00)

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;



-- Entity declaration (DO NOT ALTER THIS PART!)
entity UART_RX is
    port(
        CLK      : in std_logic;
        RST      : in std_logic;
        DIN      : in std_logic;
        DOUT     : out std_logic_vector(7 downto 0);
        DOUT_VLD : out std_logic
    );
end entity;



-- Architecture implementation (INSERT YOUR IMPLEMENTATION HERE)
architecture behavioral of UART_RX is

    signal CNT_CYCLE : std_logic_vector(3 downto 0);
    signal CNT_BIT   : std_logic_vector(3 downto 0);
    signal CYCLE_EN  : std_logic;
    signal BIT_EN    : std_logic;

begin

    -- Instance of RX FSM
    fsm: entity work.UART_RX_FSM
    port map (
        CLK       => CLK,
        RST       => RST,
        DIN       => DIN,
        CNT_CYCLE => CNT_CYCLE,
        CNT_BIT   => CNT_BIT,
        CYCLE_EN  => CYCLE_EN,
        BIT_EN    => BIT_EN,
        DOUT_VLD  => DOUT_VLD
    );

    counter_cycle: process(CLK)
    begin
        if rising_edge(CLK) then
            if CYCLE_EN = '1' then
                if BIT_EN = '0' and CNT_CYCLE = "1000" then
                    CNT_CYCLE <= (others => '0');
                else
                    CNT_CYCLE <= CNT_CYCLE + 1;
                end if;
            else
                CNT_CYCLE <= (others => '0');
            end if;
        end if;
    end process;

    counter_bit: process(CLK)
    begin
        if rising_edge(CLK) then
            if BIT_EN  = '1' then
                if CNT_CYCLE = "1111" then
                    CNT_BIT <= CNT_BIT + 1;
                end if;
            else
                CNT_BIT <= (others => '0');
            end if;
        end if;
    end process;

    p_demux_reg: process(CLK)
    begin
        if rising_edge(CLK) then
            case CNT_BIT is
                when "0000" => DOUT(0) <= DIN;
                when "0001" => DOUT(1) <= DIN;
                when "0010" => DOUT(2) <= DIN;
                when "0011" => DOUT(3) <= DIN;
                when "0100" => DOUT(4) <= DIN;
                when "0101" => DOUT(5) <= DIN;
                when "0110" => DOUT(6) <= DIN;
                when "0111" => DOUT(7) <= DIN;
                when others => null;
            end case;
        end if;
    end process;

end architecture;
