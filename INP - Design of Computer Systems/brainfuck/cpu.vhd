-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2024 Brno University of Technology,
-- Faculty of Information Technology
-- Author: Dmitrii Ivanushkin 
--
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.std_logic_arith.ALL;
USE ieee.std_logic_unsigned.ALL;

-- ----------------------------------------------------------------------------
-- Entity declaration
-- ----------------------------------------------------------------------------
ENTITY cpu IS
  PORT (
    CLK : IN std_logic; -- hodinovy signal
    RESET : IN std_logic; -- asynchronni reset procesoru
    EN : IN std_logic; -- povoleni cinnosti procesoru

    -- synchronni pamet RAM
    DATA_ADDR : OUT std_logic_vector(12 DOWNTO 0); -- adresa do pameti
    DATA_WDATA : OUT std_logic_vector(7 DOWNTO 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
    DATA_RDATA : IN std_logic_vector(7 DOWNTO 0); -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
    DATA_RDWR : OUT std_logic; -- cteni (1) / zapis (0)
    DATA_EN : OUT std_logic; -- povoleni cinnosti
 
    -- vstupni port
    IN_DATA : IN std_logic_vector(7 DOWNTO 0); -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
    IN_VLD : IN std_logic; -- data platna
    IN_REQ : OUT std_logic; -- pozadavek na vstup data
 
    -- vystupni port
    OUT_DATA : OUT std_logic_vector(7 DOWNTO 0); -- zapisovana data
    OUT_BUSY : IN std_logic; -- LCD je zaneprazdnen (1), nelze zapisovat
    OUT_INV : OUT std_logic; -- pozadavek na aktivaci inverzniho zobrazeni (1)
    OUT_WE : OUT std_logic; -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'

    -- stavove signaly
    READY : OUT std_logic; -- hodnota 1 znamena, ze byl procesor inicializovan a zacina vykonavat program
    DONE : OUT std_logic -- hodnota 1 znamena, ze procesor ukoncil vykonavani programu (narazil na instrukci halt)
  );
END cpu;
-- ----------------------------------------------------------------------------
-- Architecture declaration
-- ----------------------------------------------------------------------------
ARCHITECTURE behavioral OF cpu IS

  TYPE t_state IS (
  S_IDLE, 
  ------
  S_FIND_AT_EN, 
  S_FIND_AT, 
  ------
  S_INIT, 
  ------
  S_FETCH, 
  S_DECODE, 
  ------
  S_PTR_INC, 
  S_PTR_DEC, 
  ------
  S_MEM_INC_EN, 
  S_MEM_INC, 
  S_MEM_INC_W, 
  ------
  S_MEM_DEC_EN, 
  S_MEM_DEC, 
  S_MEM_DEC_W, 
  ------
  S_PUTCHAR_EN, 
  S_PUTCHAR_WAIT, 
  S_PUTCHAR, 
  ------
  S_GETCHAR, 
  S_GETCHAR_WAIT, 
  ------
  S_LOOP_RETURN_TO_FETCH, 
  ------
  S_LOOP_START, 
  S_LOOP_START_READ, 
  S_LOOP_START_CHECK_PTR, 
  S_LOOP_START_CHECK_CNT, 
  S_LOOP_START_SEARCH, 
  ------
  S_LOOP_END, 
  S_LOOP_END_READ, 
  S_LOOP_END_CHECK_PTR, 
  S_LOOP_END_CHECK_CNT, 
  S_LOOP_CONTINUE_READ, 
  S_LOOP_END_SEARCH, 
  ------
  S_TMP_LOAD_EN, 
  S_TMP_LOAD, 
  S_TMP_LOAD_W, 
  ------
  S_TMP_OUT_EN, 
  S_TMP_OUT, 
  S_TMP_OUT_W, 
  ------
  S_HALT, 
  ------
  S_OTHERS
  );

  SIGNAL PSTATE : t_state := S_IDLE;
  SIGNAL NSTATE : t_state := S_IDLE;

  SIGNAL PC_INC : std_logic;
  SIGNAL PC_DEC : std_logic;
  SIGNAL PC_REG : std_logic_vector(12 DOWNTO 0);

  SIGNAL PTR_INC : std_logic;
  SIGNAL PTR_DEC : std_logic;
  SIGNAL PTR_REG : std_logic_vector(12 DOWNTO 0);

  SIGNAL CNT_INC : std_logic;
  SIGNAL CNT_DEC : std_logic;
  SIGNAL CNT_REG : std_logic_vector(7 DOWNTO 0);

  SIGNAL TMP_LOAD : std_logic;
  SIGNAL TMP_REG : std_logic_vector(7 DOWNTO 0);

  SIGNAL MX1_SEL : std_logic;
  SIGNAL MX2_SEL : std_logic_vector(1 DOWNTO 0);

BEGIN
  pc_reg_p : PROCESS (CLK, RESET, EN, PC_INC, PC_DEC)
  BEGIN
    IF RESET = '1' THEN
      PC_REG <= (OTHERS => '0');
    ELSIF rising_edge(CLK) THEN
      IF EN = '1' THEN
        IF PC_INC = '1' THEN
          PC_REG <= PC_REG + 1;
        ELSIF PC_DEC = '1' THEN
          PC_REG <= PC_REG - 1;
        END IF;
      END IF;
    END IF;
  END PROCESS;

  ptr_reg_p : PROCESS (CLK, RESET, EN, PTR_INC, PTR_DEC)
  BEGIN
    IF RESET = '1' THEN
      PTR_REG <= (OTHERS => '0');
    ELSIF rising_edge(CLK) THEN
      IF EN = '1' THEN
        IF PTR_INC = '1' THEN
          PTR_REG <= PTR_REG + 1;
        ELSIF PTR_DEC = '1' THEN
          PTR_REG <= PTR_REG - 1;
        END IF;
      END IF;
    END IF;
  END PROCESS;

  cnt_reg_p : PROCESS (CLK, RESET, EN, CNT_INC, CNT_DEC)
  BEGIN
    IF RESET = '1' THEN
      CNT_REG <= (OTHERS => '0');
    ELSIF rising_edge(CLK) THEN
      IF EN = '1' THEN
        IF CNT_INC = '1' THEN
          CNT_REG <= CNT_REG + 1;
        ELSIF CNT_DEC = '1' THEN
          CNT_REG <= CNT_REG - 1;
        END IF;
      END IF;
    END IF;
  END PROCESS;

  tmp_reg_p : PROCESS (CLK, RESET, EN, TMP_LOAD)
  BEGIN
    IF RESET = '1' THEN
      TMP_REG <= (OTHERS => '0');
    ELSIF rising_edge(CLK) THEN
      IF EN = '1' AND TMP_LOAD = '1' THEN
        TMP_REG <= DATA_RDATA;
      END IF;
    END IF;
  END PROCESS;

  mx1_p : PROCESS (CLK, RESET, MX1_SEL)
  BEGIN
    IF RESET = '1' THEN
      DATA_ADDR <= (OTHERS => '0');
    ELSIF rising_edge(CLK) THEN
      CASE MX1_SEL IS
        WHEN '0' => 
          DATA_ADDR <= PC_REG;
        WHEN '1' => 
          DATA_ADDR <= PTR_REG;
        WHEN OTHERS => 
      END CASE;
    END IF;
  END PROCESS;

  mx2_p : PROCESS (CLK, RESET, MX2_SEL)
  BEGIN
    IF RESET = '1' THEN
      DATA_WDATA <= (OTHERS => '0');
    ELSIF rising_edge(CLK) THEN
      CASE MX2_SEL IS
        WHEN "00" => 
          DATA_WDATA <= DATA_RDATA + 1;
        WHEN "01" => 
          DATA_WDATA <= DATA_RDATA - 1;
        WHEN "10" => 
          DATA_WDATA <= IN_DATA;
        WHEN "11" => 
          DATA_WDATA <= TMP_REG;
        WHEN OTHERS => 
      END CASE;
    END IF;
  END PROCESS;

  pstate_reg_p : PROCESS (CLK, RESET, EN)
  BEGIN
    IF RESET = '1' THEN
      PSTATE <= S_IDLE;
    ELSIF rising_edge(CLK) THEN
      IF EN = '1' THEN
        PSTATE <= NSTATE;
      END IF;
    END IF;
  END PROCESS;

  nstate_logic : PROCESS (PSTATE, OUT_BUSY, IN_VLD)
  BEGIN
    NSTATE <= PSTATE;
    PC_INC <= '0';
    PC_DEC <= '0';
    PTR_INC <= '0';
    PTR_DEC <= '0';
    CNT_INC <= '0';
    CNT_DEC <= '0';
    TMP_LOAD <= '0';
    MX1_SEL <= '0';
    DATA_EN <= '0';
    DATA_RDWR <= '1';
    OUT_WE <= '0';
    OUT_INV <= '0';
    IN_REQ <= '0';
 
    CASE PSTATE IS
      WHEN S_IDLE => 
        READY <= '0';
        DONE <= '0';
        NSTATE <= S_FIND_AT_EN;

        -------------------------------------------------

      WHEN S_FIND_AT_EN => 
        DATA_EN <= '1';
        MX1_SEL <= '1';
        NSTATE <= S_FIND_AT;

      WHEN S_FIND_AT => 
        DATA_EN <= '1';
        MX1_SEL <= '1';
        IF DATA_RDATA = X"40" THEN
          NSTATE <= S_INIT;
        ELSE
          PTR_INC <= '1';
          NSTATE <= S_FIND_AT_EN;
        END IF;

        -------------------------------------------------

      WHEN S_INIT => 
        READY <= '1';
        NSTATE <= S_FETCH;

        -------------------------------------------------

      WHEN S_FETCH => 
        DATA_EN <= '0';
        MX1_SEL <= '0';
        NSTATE <= S_DECODE;

      WHEN S_DECODE => 
        DATA_EN <= '1';
        MX1_SEL <= '1';
        CASE DATA_RDATA IS
          WHEN X"3E" => -- >
            NSTATE <= S_PTR_INC;
          WHEN X"3C" => -- <
            NSTATE <= S_PTR_DEC;
          WHEN X"2B" => -- +
            NSTATE <= S_MEM_INC_EN;
          WHEN X"2D" => -- -
            NSTATE <= S_MEM_DEC_EN;
          WHEN X"2E" => -- .
            NSTATE <= S_PUTCHAR_EN;
          WHEN X"2C" => -- ,
            NSTATE <= S_GETCHAR_WAIT;
          WHEN X"5B" => -- [
            NSTATE <= S_LOOP_START;
          WHEN X"5D" => -- ]
            NSTATE <= S_LOOP_END;
          WHEN X"24" => -- $
            NSTATE <= S_TMP_LOAD_EN;
          WHEN X"21" => -- !
            NSTATE <= S_TMP_OUT_EN;
          WHEN X"40" => -- @
            NSTATE <= S_HALT;
          WHEN OTHERS => 
            NSTATE <= S_OTHERS;
      END CASE;

      -------------------------------------------------

      WHEN S_PTR_INC => 
        MX1_SEL <= '1';
        PTR_INC <= '1';
        PC_INC <= '1';
        NSTATE <= S_FETCH;

      WHEN S_PTR_DEC => 
        MX1_SEL <= '1';
        PTR_DEC <= '1';
        PC_INC <= '1';
        NSTATE <= S_FETCH;

        -------------------------------------------------

      WHEN S_MEM_INC_EN => 
        DATA_EN <= '1';
        MX1_SEL <= '0';
        NSTATE <= S_MEM_INC;

      WHEN S_MEM_INC => 
        DATA_EN <= '1';
        MX1_SEL <= '1';
        MX2_SEL <= "00";
        NSTATE <= S_MEM_INC_W;

      WHEN S_MEM_INC_W => 
        DATA_EN <= '1';
        MX1_SEL <= '0';
        DATA_RDWR <= '0';
        PC_INC <= '1';
        NSTATE <= S_FETCH;

        -------------------------------------------------

      WHEN S_MEM_DEC_EN => 
        DATA_EN <= '1';
        MX1_SEL <= '0';
        NSTATE <= S_MEM_DEC;

      WHEN S_MEM_DEC => 
        DATA_EN <= '1';
        MX1_SEL <= '1';
        MX2_SEL <= "01";
        NSTATE <= S_MEM_DEC_W;

      WHEN S_MEM_DEC_W => 
        DATA_EN <= '1';
        MX1_SEL <= '0';
        DATA_RDWR <= '0';
        PC_INC <= '1';
        NSTATE <= S_FETCH;

        -------------------------------------------------

      WHEN S_PUTCHAR_EN => 
        OUT_DATA <= (OTHERS => '0');
        DATA_EN <= '1';
        MX1_SEL <= '0';
        NSTATE <= S_PUTCHAR_WAIT;

      WHEN S_PUTCHAR_WAIT => 
        IF OUT_BUSY = '0' THEN
          NSTATE <= S_PUTCHAR;
        ELSE
          NSTATE <= PSTATE;
        END IF;

      WHEN S_PUTCHAR => 
        DATA_EN <= '1';
        MX1_SEL <= '0';
        OUT_DATA <= DATA_RDATA;
        OUT_WE <= '1';
        OUT_INV <= '0';
        PC_INC <= '1';
        NSTATE <= S_FETCH;

        -------------------------------------------------

      WHEN S_GETCHAR_WAIT => 
        IN_REQ <= '1';
        MX1_SEL <= '1';
        IF IN_VLD = '1' THEN
          NSTATE <= S_GETCHAR;
        ELSE
          NSTATE <= PSTATE;
        END IF;

      WHEN S_GETCHAR => 
        DATA_EN <= '1';
        IN_REQ <= '0';
        DATA_RDWR <= '0';
        MX2_SEL <= "10";
        PC_INC <= '1';
        NSTATE <= S_FETCH;
        MX1_SEL <= '0';

        --------------------------------------------------

      WHEN S_LOOP_RETURN_TO_FETCH => 
        DATA_EN <= '1';
        MX1_SEL <= '0';
        NSTATE <= S_FETCH;

        ---------------------------------------------------

      WHEN S_LOOP_START => 
        DATA_EN <= '1';
        MX1_SEL <= '0';
        NSTATE <= S_LOOP_START_READ;

      WHEN S_LOOP_START_READ => 
        DATA_EN <= '0';
        MX1_SEL <= '1';
        NSTATE <= S_LOOP_START_CHECK_PTR;

      WHEN S_LOOP_START_CHECK_PTR => 
        PC_INC <= '1'; -- PC ← PC + 1
        DATA_EN <= '1';
        MX1_SEL <= '0';
        IF DATA_RDATA = X"00" THEN -- if (mem[PTR] == 0)
          CNT_INC <= '1'; -- CNT ← 1
          NSTATE <= S_LOOP_START_CHECK_CNT;
        ELSE
          NSTATE <= S_LOOP_RETURN_TO_FETCH;
        END IF;

      WHEN S_LOOP_START_CHECK_CNT => 
        DATA_EN <= '1';
        MX1_SEL <= '0';
        IF CNT_REG = X"00" THEN -- while (CNT != 0)
          NSTATE <= S_FETCH;
        ELSE
          NSTATE <= S_LOOP_START_SEARCH;
        END IF;

      WHEN S_LOOP_START_SEARCH => 
        DATA_EN <= '0';
        MX1_SEL <= '1';
        IF DATA_RDATA = X"5B" THEN -- if (c == ’[’)
          CNT_INC <= '1'; -- CNT ← CNT + 1
        ELSIF DATA_RDATA = X"5D" THEN -- elsif (c == ’]’
          CNT_DEC <= '1'; -- CNT ← CNT - 1
        END IF;
        PC_INC <= '1'; -- PC ← PC + 1
        NSTATE <= S_LOOP_START_CHECK_PTR;

        -------------------------------------------------

      WHEN S_LOOP_END => 
        DATA_EN <= '1';
        MX1_SEL <= '0';
        NSTATE <= S_LOOP_END_READ;

      WHEN S_LOOP_END_READ => 
        DATA_EN <= '0';
        MX1_SEL <= '1';
        NSTATE <= S_LOOP_END_CHECK_PTR;

      WHEN S_LOOP_END_CHECK_PTR => 
        DATA_EN <= '1';
        MX1_SEL <= '0';
        IF DATA_RDATA = X"00" THEN -- if (mem[PTR] == 0)
          PC_INC <= '1'; -- PC ← PC + 1
          NSTATE <= S_LOOP_RETURN_TO_FETCH;
        ELSE
          CNT_INC <= '1'; -- CNT ← 1
          PC_DEC <= '1'; -- PC ← PC - 1
          NSTATE <= S_LOOP_CONTINUE_READ;
        END IF;

      WHEN S_LOOP_CONTINUE_READ => 
        DATA_EN <= '1';
        MX1_SEL <= '0';
        NSTATE <= S_LOOP_END_CHECK_CNT;

      WHEN S_LOOP_END_CHECK_CNT => 
        DATA_EN <= '1';
        MX1_SEL <= '0';
        IF CNT_REG = X"00" THEN -- while (CNT != 0)
          NSTATE <= S_FETCH;
        ELSE
          NSTATE <= S_LOOP_END_SEARCH;
        END IF;

      WHEN S_LOOP_END_SEARCH => 
        DATA_EN <= '0';
        MX1_SEL <= '1';
        IF DATA_RDATA = X"5D" THEN -- if (c == ’]’)
          CNT_INC <= '1'; -- CNT ← CNT + 1
        ELSIF DATA_RDATA = X"5B" THEN -- elsif (c == ’[’
          CNT_DEC <= '1'; -- CNT ← CNT - 1
        END IF;
        NSTATE <= S_LOOP_END_CHECK_PTR;

        -------------------------------------------------

      WHEN S_TMP_LOAD_EN => 
        DATA_EN <= '1';
        MX1_SEL <= '0';
        NSTATE <= S_TMP_LOAD;

      WHEN S_TMP_LOAD => 
        DATA_EN <= '1';
        MX1_SEL <= '1';
        TMP_LOAD <= '1';
        NSTATE <= S_TMP_LOAD_W;

      WHEN S_TMP_LOAD_W => 
        DATA_EN <= '1';
        MX1_SEL <= '0';
        DATA_RDWR <= '0';
        PC_INC <= '1';
        NSTATE <= S_FETCH;

        -------------------------------------------------

      WHEN S_TMP_OUT_EN => 
        DATA_EN <= '1';
        MX1_SEL <= '0';
        NSTATE <= S_TMP_OUT;

      WHEN S_TMP_OUT => 
        DATA_EN <= '1';
        MX1_SEL <= '1';
        MX2_SEL <= "11";
        NSTATE <= S_TMP_OUT_W;

      WHEN S_TMP_OUT_W => 
        DATA_EN <= '1';
        MX1_SEL <= '0';
        DATA_RDWR <= '0';
        PC_INC <= '1';
        NSTATE <= S_FETCH;

        -------------------------------------------------

      WHEN S_HALT => 
        DONE <= '1';
        NSTATE <= S_HALT;

        -------------------------------------------------

      WHEN S_OTHERS => 
        PC_INC <= '1';
        NSTATE <= S_FETCH;

    END CASE;
  END PROCESS;

END behavioral;
