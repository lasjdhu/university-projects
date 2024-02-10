-- uart_rx_fsm.vhd: UART controller - finite state machine controlling RX side
-- Author(s): Dmitrii Ivanushkin (xivanu00)

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;



entity UART_RX_FSM is
    port(
       CLK       : in std_logic;
       RST       : in std_logic;
       DIN       : in std_logic;
       CNT_CYCLE : in std_logic_vector(3 downto 0);
       CNT_BIT   : in std_logic_vector(3 downto 0);
       CYCLE_EN  : out std_logic;
       BIT_EN    : out std_logic;
       DOUT_VLD  : out std_logic
    );
end entity;



architecture behavioral of UART_RX_FSM is

    type t_state is (IDLE, WAIT_FIRST, READ_DATA, STOP_READ);
    signal pstate : t_state;
    signal nstate : t_state;

begin

    -- Present state register
    pstate_register: process(CLK)
    begin
        if rising_edge(CLK) then
            if RST = '1' then
                pstate <= IDLE;
            else
                pstate <= nstate;
            end if;
        end if;
    end process;

    -- Next state combinatorical logic
    nstate_logic: process(pstate, DIN, CNT_CYCLE, CNT_BIT)
    begin
        nstate <= pstate;
        case pstate is
            when IDLE =>
                nstate <= WAIT_FIRST;
            when WAIT_FIRST =>
                if CNT_CYCLE = "1000" and DIN = '0' then
                    nstate <= READ_DATA;
                end if;
            when READ_DATA =>
                if CNT_BIT = "1001" then
                    nstate <= STOP_READ;
                end if;
            when STOP_READ =>
                if DIN = '1' then
                    nstate <= IDLE;
                end if;
            when others =>
                nstate <= IDLE;
        end case;
    end process;

    -- Output combinatorial logic
    output_logic: process(pstate)
    begin
        case pstate is
            when IDLE =>
                DOUT_VLD <= '0';
                CYCLE_EN <= '0';
                BIT_EN   <= '0';
            when WAIT_FIRST =>
                DOUT_VLD <= '0';
                CYCLE_EN <= '1';
                BIT_EN   <= '0';
            when READ_DATA =>
                DOUT_VLD <= '0';
                CYCLE_EN <= '1';
                BIT_EN   <= '1';
            when STOP_READ =>
                DOUT_VLD <= '1';
                CYCLE_EN <= '0';
                BIT_EN   <= '0';
        end case;
    end process;


end architecture;
