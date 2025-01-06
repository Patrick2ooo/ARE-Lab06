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
-- 1.0    20.12.2024  GoninG      Adapted to my needs

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
  SIGNAL led_reg_s : STD_LOGIC_VECTOR(9 DOWNTO 0);
  SIGNAL switches_s : STD_LOGIC_VECTOR(9 DOWNTO 0);
  SIGNAL button_s : STD_LOGIC_VECTOR(3 DOWNTO 0);
	
  SIGNAL delay_gen_s : STD_LOGIC_VECTOR(1 DOWNTO 0);
  SIGNAL mod_gen_s : STD_LOGIC;
  SIGNAL checksum_s : STD_LOGIC_VECTOR(7 DOWNTO 0);
  SIGNAL init_char_s : STD_LOGIC;
  SIGNAL new_char_s : STD_LOGIC;
	
  SIGNAL char_1_s : STD_LOGIC_VECTOR(7 DOWNTO 0);
  SIGNAL char_2_s : STD_LOGIC_VECTOR(7 DOWNTO 0);
  SIGNAL char_3_s : STD_LOGIC_VECTOR(7 DOWNTO 0);
  SIGNAL char_4_s : STD_LOGIC_VECTOR(7 DOWNTO 0);
  SIGNAL char_5_s : STD_LOGIC_VECTOR(7 DOWNTO 0);
  SIGNAL char_6_s : STD_LOGIC_VECTOR(7 DOWNTO 0);
  SIGNAL char_7_s : STD_LOGIC_VECTOR(7 DOWNTO 0);
  SIGNAL char_8_s : STD_LOGIC_VECTOR(7 DOWNTO 0);
  SIGNAL char_9_s : STD_LOGIC_VECTOR(7 DOWNTO 0);
  SIGNAL char_10_s : STD_LOGIC_VECTOR(7 DOWNTO 0);
  SIGNAL char_11_s : STD_LOGIC_VECTOR(7 DOWNTO 0);
  SIGNAL char_12_s : STD_LOGIC_VECTOR(7 DOWNTO 0);
  SIGNAL char_13_s : STD_LOGIC_VECTOR(7 DOWNTO 0);
  SIGNAL char_14_s : STD_LOGIC_VECTOR(7 DOWNTO 0);
  SIGNAL char_15_s : STD_LOGIC_VECTOR(7 DOWNTO 0);
  SIGNAL char_16_s : STD_LOGIC_VECTOR(7 DOWNTO 0);
  
  SIGNAL readdatavalid_next_s : STD_LOGIC;
  SIGNAL readdatavalid_reg_s : STD_LOGIC;
  SIGNAL readdata_next_s : STD_LOGIC_VECTOR(31 DOWNTO 0);
  SIGNAL readdata_reg_s : STD_LOGIC_VECTOR(31 DOWNTO 0);

  SIGNAL status_bits_0_s : STD_LOGIC;
  SIGNAL ack_s : STD_LOGIC;
  SIGNAL mem_d_s : STD_LOGIC;
  SIGNAL mode_is_pic_s: STD_LOGIC;

begin
  -- Input signals
  button_s <= button_i;
  switches_s <= switch_i;

  -- Output signals
  avl_readdatavalid_o <= readdatavalid_reg_s;
  avl_readdata_o <= readdata_reg_s;

  led_o <= led_reg_s;
  auto_o <= mod_gen_s;
  delay_o <= delay_gen_s;
  cmd_init_o <= init_char_s;
  cmd_new_char_o <= new_char_s;

  -- Read access part
  read_decoder_p : PROCESS (ALL)
  BEGIN
    readdatavalid_next_s <= '0'; --valeur par defaut
    readdata_next_s <= (OTHERS => '0'); --valeur par defaut

    IF avl_read_i = '1' THEN
      readdatavalid_next_s <= '1';

      CASE (to_integer(unsigned(avl_address_i))) IS

        WHEN 0 =>
          readdata_next_s <= INTERFACE_ID_C;

        WHEN 1 =>
          readdata_next_s(3 DOWNTO 0) <= button_s;
		 
		    WHEN 2 =>
          readdata_next_s(9 DOWNTO 0) <= switches_s;
			 
        WHEN 3 =>
          readdata_next_s(9 DOWNTO 0) <= led_reg_s;

        WHEN 4 =>
          readdata_next_s(1) <= '1';
          readdata_next_s(0) <= status_bits_0_s;

        WHEN 5 =>
          readdata_next_s(1 DOWNTO 0) <= delay_gen_s;
          readdata_next_s(4) <= mod_gen_s;
        
        WHEN 6 =>
          readdata_next_s(0) <= ack_s;
          CASE e_pres IS
            WHEN IDLE        => readdata_next_s(3 DOWNTO 1) <= "000";
            WHEN NEW_C       => readdata_next_s(3 DOWNTO 1) <= "001";
            WHEN PICT_M      => readdata_next_s(3 DOWNTO 1) <= "010";
            WHEN TAKE        => readdata_next_s(3 DOWNTO 1) <= "011";
            WHEN NEW_C_PIC_M => readdata_next_s(3 DOWNTO 1) <= "100";
            WHEN OTHERS      => readdata_next_s(3 DOWNTO 1) <= "111";
          END CASE;
        
        WHEN 7 =>
          readdata_next_s(0) <= mode_is_pic_s;
        
        WHEN 8 =>
          readdata_next_s(31 DOWNTO 24) <= char_1_s;
          readdata_next_s(23 DOWNTO 16) <= char_2_s;
          readdata_next_s(15 DOWNTO 8) <= char_3_s;
          readdata_next_s(7 DOWNTO 0) <= char_4_s;
        
        WHEN 9 =>
          readdata_next_s(31 DOWNTO 24) <= char_5_s;
          readdata_next_s(23 DOWNTO 16) <= char_6_s;
          readdata_next_s(15 DOWNTO 8) <= char_7_s;
          readdata_next_s(7 DOWNTO 0) <= char_8_s;
        
        WHEN 10 =>
          readdata_next_s(31 DOWNTO 24) <= char_9_s;
          readdata_next_s(23 DOWNTO 16) <= char_10_s;
          readdata_next_s(15 DOWNTO 8) <= char_11_s;
          readdata_next_s(7 DOWNTO 0) <= char_12_s;
        
        WHEN 11 =>
          readdata_next_s(31 DOWNTO 24) <= char_13_s;
          readdata_next_s(23 DOWNTO 16) <= char_14_s;
          readdata_next_s(15 DOWNTO 8) <= char_15_s;
          readdata_next_s(7 DOWNTO 0) <= char_16_s;
          
        WHEN 12 =>
          readdata_next_s(7 DOWNTO 0) <= checksum_s;

        WHEN OTHERS =>
          readdata_next_s <= RESERVED_VAL_C;

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
      readdata_reg_s <= readdata_next_s;

      IF mem_d_s = '1' THEN
        -- Update picture
        checksum_s <= checksum_i;
        char_1_s <= char_1_i;
        char_2_s <= char_2_i;
        char_3_s <= char_3_i;
        char_4_s <= char_4_i;
        char_5_s <= char_5_i;
        char_6_s <= char_6_i;
        char_7_s <= char_7_i;
        char_8_s <= char_8_i;
        char_9_s <= char_9_i;
        char_10_s <= char_10_i;
        char_11_s <= char_11_i;
        char_12_s <= char_12_i;
        char_13_s <= char_13_i;
        char_14_s <= char_14_i;
        char_15_s <= char_15_i;
        char_16_s <= char_16_i;
      END IF;
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
		  init_char_s <= '0';
		  new_char_s <= '0';
		  delay_gen_s <= (OTHERS => '0');
		  mod_gen_s <= '0';
      ack_s <= '0';

    ELSIF rising_edge(avl_clk_i) THEN
      ack_s <= '0'; -- by default ack is not active and when it's written it's active but for 1 clock time only (the write at WHEN 6 will override this one)
      new_char_s <= '0'; --valeur par defaut
      init_char_s <= '0'; --valeur par defaut

      IF avl_write_i = '1' THEN

        CASE (to_integer(unsigned(avl_address_i))) IS

          WHEN 3 =>
            led_reg_s <= avl_writedata_i(9 DOWNTO 0);

          WHEN 4 =>
            init_char_s <= avl_writedata_i(0);
            new_char_s <= avl_writedata_i(4);

          WHEN 5 =>
            delay_gen_s <= avl_writedata_i(1 DOWNTO 0);
            mod_gen_s <= avl_writedata_i(4);
          
          WHEN 6 =>
            ack_s <= avl_writedata_i(0);
          
          WHEN 7 =>
            mode_is_pic_s <= avl_writedata_i(0);

          WHEN OTHERS =>
            NULL;

        END CASE;
      END IF;
    END IF;
  END PROCESS;
  
  -- Interface management

end rtl; 