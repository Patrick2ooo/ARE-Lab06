------------------------------------------------------------------------------------------
-- HEIG-VD ///////////////////////////////////////////////////////////////////////////////
-- Haute Ecole d'Ingenerie et de Gestion du Canton de Vaud
-- School of Business and Engineering in Canton de Vaud
------------------------------------------------------------------------------------------
-- REDS Institute ////////////////////////////////////////////////////////////////////////
-- Reconfigurable Embedded Digital Systems
------------------------------------------------------------------------------------------
--
-- File                 : avl_user_interface.vhd
-- Author               : Guillaume Gonin
-- Date                 : 20.12.2024
--
-- Context              : Avalon user interface
--
------------------------------------------------------------------------------------------
-- Description : 
--   
------------------------------------------------------------------------------------------
-- Dependencies : 
--   
------------------------------------------------------------------------------------------
-- Modifications :
-- Ver    Date        Engineer    Comments
-- 0.0    18.12.2024  UB/GoninG   Initial version
-- 1.0    07.01.2025  GoninG      Adapted to my needs
-- 1.1    07.01.2025  GoninG      Added counter
-- 1.2    10.01.2025  GoninG      Added irq
-- 1.3    10.01.2025  GoninG      No compile errors
-- 1.4    10.01.2025  GoninG      Serial transmitter done
-- 1.5    10.01.2025  GoninG      Renamed process and some error resolved
------------------------------------------------------------------------------------------

library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;
    
entity avl_user_interface is
  port(
    -- Avalon bus
    avl_clk_i           : in  std_logic;
    avl_reset_i         : in  std_logic;
    avl_address_i       : in  std_logic_vector(13 downto 0);
    avl_byteenable_i    : in  std_logic_vector(3 downto 0);
    avl_write_i         : in  std_logic;
    avl_writedata_i     : in  std_logic_vector(31 downto 0);
    avl_read_i          : in  std_logic;
    avl_readdatavalid_o : out std_logic;
    avl_readdata_o      : out std_logic_vector(31 downto 0);
    avl_waitrequest_o   : out std_logic;
    avl_irq_o           : out std_logic;
    -- User interface
    button_i            : in  std_logic_vector(3 downto 0);
    switch_i            : in  std_logic_vector(9 downto 0);
    led_o               : out std_logic_vector(9 downto 0);
    hex0_o              : out std_logic_vector(6 downto 0);
    hex1_o              : out std_logic_vector(6 downto 0);
    hex2_o              : out std_logic_vector(6 downto 0);
    hex3_o              : out std_logic_vector(6 downto 0);
    -- Con 80p interface
    serial_data_o       : out std_logic;
    con_80p_status_i    : in  std_logic_vector(1 downto 0)
  );
end avl_user_interface;

architecture rtl of avl_user_interface is

  --| Components declaration |--------------------------------------------------------------
    
  --| Constants declarations |--------------------------------------------------------------
  CONSTANT INTERFACE_ID_C : STD_LOGIC_VECTOR(31 DOWNTO 0) := x"12345678";
  CONSTANT RESERVED_VAL_C : STD_LOGIC_VECTOR(31 DOWNTO 0) := x"DEADBEEF";

  --| Signals declarations   |--------------------------------------------------------------   
  -- I/O
  SIGNAL led_reg_s : STD_LOGIC_VECTOR(9 DOWNTO 0);
  SIGNAL hex0_reg_s : STD_LOGIC_VECTOR(6 DOWNTO 0);
  SIGNAL hex1_reg_s : STD_LOGIC_VECTOR(6 DOWNTO 0);
  SIGNAL hex2_reg_s : STD_LOGIC_VECTOR(6 DOWNTO 0);
  SIGNAL hex3_reg_s : STD_LOGIC_VECTOR(6 DOWNTO 0);
  SIGNAL switches_s : STD_LOGIC_VECTOR(9 DOWNTO 0);
  SIGNAL button_s : STD_LOGIC_VECTOR(3 DOWNTO 0);
	
  -- Con 80p interface
  SIGNAL serial_data_s : STD_LOGIC;
  SIGNAL f_ready_s : STD_LOGIC;
  SIGNAL con_80p_status_s : STD_LOGIC_VECTOR(1 DOWNTO 0);
  
  -- Avalon Interface
  SIGNAL readdatavalid_next_s : STD_LOGIC;
  SIGNAL readdatavalid_reg_s : STD_LOGIC;
  SIGNAL readdata_user_s : STD_LOGIC_VECTOR(31 DOWNTO 0);
  SIGNAL readdata_reg_s : STD_LOGIC_VECTOR(31 DOWNTO 0);
  SIGNAL irq_s : STD_LOGIC;

  -- Counter
  SIGNAL counter_value_next_s : UNSIGNED(31 DOWNTO 0);
  SIGNAL counter_value_reg_s : UNSIGNED(31 DOWNTO 0);
  SIGNAL enable_counter_s : STD_LOGIC;
  SIGNAL reset_counter_s : STD_LOGIC;

  -- Internal uses
  SIGNAL last_button0_state_s : STD_LOGIC;
  SIGNAL button0_s : STD_LOGIC;
  SIGNAL button0_irq_s : STD_LOGIC;
  SIGNAL clear_irq_s : STD_LOGIC;
  SIGNAL mask_irq_s : STD_LOGIC;
  SIGNAL clock_count_s : UNSIGNED(6 DOWNTO 0);
  SIGNAL pulse_serial_s : STD_LOGIC;
  SIGNAL data_reg_s : STD_LOGIC_VECTOR(19 DOWNTO 0);
  SIGNAL data_user_s : STD_LOGIC_VECTOR(19 DOWNTO 0);

begin
  -- Input signals
  con_80p_status_s <= con_80p_status_i;

  button_s <= button_i;
  switches_s <= switch_i;

  -- Output signals
  avl_readdatavalid_o <= readdatavalid_reg_s;
  avl_readdata_o <= readdata_reg_s;
  avl_irq_o <= irq_s;

  led_o <= led_reg_s;
  hex0_o <= hex0_reg_s;
  hex1_o <= hex1_reg_s;
  hex2_o <= hex2_reg_s;
  hex3_o <= hex3_reg_s;

  serial_data_o <= serial_data_s;

  -- Read access part
  read_decoder_p : PROCESS (ALL)
  BEGIN
    readdatavalid_next_s <= '0'; --valeur par defaut
    readdata_user_s <= (OTHERS => '0'); --valeur par defaut

    IF avl_read_i = '1' THEN
      readdatavalid_next_s <= '1';

      CASE (to_integer(unsigned(avl_address_i))) IS

        WHEN 0 =>
          readdata_user_s <= INTERFACE_ID_C;

        WHEN 1 =>
          readdata_user_s(3 DOWNTO 0) <= button_s;
		 
		    WHEN 2 =>
          readdata_user_s(9 DOWNTO 0) <= switches_s;
			 
        WHEN 3 =>
          readdata_user_s(9 DOWNTO 0) <= led_reg_s;

        WHEN 4 =>
          readdata_user_s(6 DOWNTO 0) <= hex0_reg_s;
          readdata_user_s(13 DOWNTO 7) <= hex1_reg_s;
          readdata_user_s(20 DOWNTO 14) <= hex2_reg_s;
          readdata_user_s(27 DOWNTO 21) <= hex3_reg_s;

        WHEN 5 =>
          readdata_user_s(1 DOWNTO 0) <= con_80p_status_s;
          readdata_user_s(2) <= f_ready_s;
        
        WHEN 6 =>
          readdata_user_s(31 DOWNTO 0) <= std_logic_vector(counter_value_reg_s);
        
        WHEN 7 =>
          readdata_user_s(0) <= irq_s;

        WHEN OTHERS =>
          readdata_user_s <= RESERVED_VAL_C;

      END CASE;
    END IF;
  END PROCESS;

  -- Read register process
  read_register_p : PROCESS (avl_reset_i, avl_clk_i)
  BEGIN
    IF avl_reset_i = '1' THEN
      readdatavalid_reg_s <= '0';
      readdata_reg_s <= (OTHERS => '0');

    ELSIF rising_edge(avl_clk_i) THEN
      readdatavalid_reg_s <= readdatavalid_next_s;
      readdata_reg_s <= readdata_user_s;
    END IF;
  END PROCESS;

  -- Write access part
  write_register_p : PROCESS (
    avl_reset_i,
    avl_clk_i
    )
  BEGIN
    IF avl_reset_i = '1' THEN
      led_reg_s <= (OTHERS => '0');
      hex0_reg_s <= (OTHERS => '0');
      hex1_reg_s <= (OTHERS => '0');
      hex2_reg_s <= (OTHERS => '0');
      hex3_reg_s <= (OTHERS => '0');
      reset_counter_s <= '0';
      enable_counter_s <= '0';
      clear_irq_s <= '0';
      mask_irq_s <= '0';
      pulse_serial_s <= '0';

    ELSIF rising_edge(avl_clk_i) THEN
      reset_counter_s <= '0'; --valeur par defaut
      clear_irq_s <= '0'; --valeur par defaut

      IF avl_write_i = '1' THEN

        CASE (to_integer(unsigned(avl_address_i))) IS

          WHEN 3 =>
            led_reg_s <= avl_writedata_i(9 DOWNTO 0);

          WHEN 4 =>
            hex0_reg_s <= avl_writedata_i(6 DOWNTO 0);
            hex1_reg_s <= avl_writedata_i(13 DOWNTO 7);
            hex2_reg_s <= avl_writedata_i(20 DOWNTO 14);
            hex3_reg_s <= avl_writedata_i(27 DOWNTO 21);

          WHEN 5 =>
            IF(f_ready_s = '1') THEN
              data_user_s <= avl_writedata_i(19 DOWNTO 0);
              pulse_serial_s <= '1';
            END IF;
          
          WHEN 6 =>
            reset_counter_s <= avl_writedata_i(0);
            enable_counter_s <= avl_writedata_i(1);
          
          WHEN 7 =>
            clear_irq_s <= avl_writedata_i(0);
            mask_irq_s <= avl_writedata_i(1);

          WHEN OTHERS =>
            NULL;

        END CASE;
      END IF;
    END IF;
  END PROCESS;
  
  -- Counter process
  counter_p : PROCESS (avl_reset_i, avl_clk_i)
  BEGIN
    IF avl_reset_i = '1' THEN
      counter_value_reg_s <= (OTHERS => '0');
      counter_value_next_s <= (OTHERS => '0');

    ELSIF rising_edge(avl_clk_i) THEN
      counter_value_reg_s <= counter_value_next_s;

      IF reset_counter_s = '1' THEN
        counter_value_next_s <= (OTHERS => '0');
      ELSIF enable_counter_s = '0' THEN
        counter_value_next_s <= counter_value_next_s;
      ELSIF counter_value_next_s = 4294967296 THEN -- 2^32 = 4 294 967 296
        counter_value_next_s <= counter_value_next_s; -- counter_value_next_s + 1 would equals 0
      ELSE
        counter_value_next_s <= counter_value_next_s + 1;
      END IF;
    END IF;
  END PROCESS;

  -- Button 0 reg
  button0_register_p : PROCESS (avl_reset_i, avl_clk_i)
  BEGIN
    IF avl_reset_i = '1' THEN
      last_button0_state_s <= '0';
      button0_s <= '0';

    ELSIF rising_edge(avl_clk_i) THEN
      button0_s <= button_s(0);
      last_button0_state_s <= button0_s;

    END IF;
  END PROCESS;

  -- Irq process
  irq_p : PROCESS (avl_reset_i, avl_clk_i)
  BEGIN
    IF avl_reset_i = '1' THEN
      irq_s <= '0';
      button0_irq_s <= '0';

    ELSIF rising_edge(avl_clk_i) THEN

      IF clear_irq_s = '1' THEN
        irq_s <= '0';
        button0_irq_s <= '0';

      ELSIF mask_irq_s = '0' AND button0_s = '1' AND last_button0_state_s = '0' THEN
        irq_s <= '1';
        button0_irq_s <= '1';

      END IF;
    END IF;
  END PROCESS;

  -- Serial transmitter process
  serial_transmitter_p : PROCESS (avl_reset_i, avl_clk_i)
  BEGIN
    IF avl_reset_i = '1' THEN
      clock_count_s <= (OTHERS => '1');
      f_ready_s <= '1';
      data_reg_s <= (OTHERS => '1');
      serial_data_s <= '1';

    ELSIF rising_edge(avl_clk_i) THEN

      -- 1/9600 s = 104,166.67 Nanoseconds so about 5 clk times. Need to transmit 22 bits (20 data + start bit + stop bit) so 110 clock times
      IF clock_count_s = 110 THEN
        f_ready_s <= '1';

      ELSIF pulse_serial_s = '1' THEN
        clock_count_s <= clock_count_s + 1; -- clock = '11..11' so clock + 1 = 0
        serial_data_s <= '0';
        data_reg_s <= data_user_s;
        f_ready_s <= '0';

      ELSIF f_ready_s = '1' THEN
        clock_count_s <= (OTHERS => '1');

      ELSIF clock_count_s(2 DOWNTO 0) = "101" OR clock_count_s(2 DOWNTO 0) = "000"  THEN -- when clock_count is multiple of five we transmit a new bit
        clock_count_s <= clock_count_s + 1;
        serial_data_s <= data_reg_s(19); -- MSB
        data_reg_s(19 DOWNTO 1) <= data_reg_s(18 DOWNTO 0);
        data_reg_s(0) <= '1'; -- data_ << 1 and append 1 in LSB

      ELSE
        clock_count_s <= clock_count_s + 1;

      END IF;
    END IF;
  END PROCESS;

end rtl; 