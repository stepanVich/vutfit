-- fsm.vhd: Terminalova aplikace
-- Author: Stepan Vich, xvichs00
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (PSWD1, PSWD2, PSWD3, PSWD4A, PSWD5A, PSWD6A, PSWD7A , PSWD8A, PSWD9A, PSWD10A, PSWD4B, PSWD5B, PSWD6B, PSWD7B, PSWD8B, PSWD9B, PSWD10B, LAST_CHAR, BAD_PSWD_ERROR, GOOD_PSWD, BAD_PSWD, REINIT);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
  if (RESET = '1') then
    present_state <= PSWD1;
  elsif (CLK'event AND CLK = '1') then
    present_state <= next_state;
  end if;
end process sync_logic;

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
  case (present_state) is
  -----------------------
  when PSWD1 =>
    next_state <= PSWD1;
    if( KEY(1)='1' ) then
      next_state <= PSWD2;
    elsif( KEY(15)='1' ) then
      next_state <= BAD_PSWD;
    elsif( KEY(15 downto 0) /= "0000000000000000" ) then
      next_state <= BAD_PSWD_ERROR;
    end if;
  -----------------------
  when PSWD2 =>
    next_state <= PSWD2;
    if( KEY(1)='1' ) then
      next_state <= PSWD3;
    elsif(  KEY(15)='1' ) then
      next_state <= BAD_PSWD;
    elsif( KEY(15 downto 0) /= "0000000000000000" ) then
      next_state <= BAD_PSWD_ERROR;
    end if;
  -----------------------
  when PSWD3 =>
    next_state <= PSWD3;
    if( KEY(5)='1' ) then
      next_state <= PSWD4A;
    elsif( KEY(2)='1' ) then
      next_state <= PSWD4B;
    elsif(  KEY(15)='1' ) then
      next_state <= BAD_PSWD;
    elsif( KEY(15 downto 0) /= "0000000000000000" ) then
      next_state <= BAD_PSWD_ERROR;
    end if;
  -----------------------
  when PSWD4A =>
    next_state <= PSWD4A;
    if( KEY(1)='1' ) then
      next_state <= PSWD5A;
    elsif(  KEY(15)='1' ) then
      next_state <= BAD_PSWD;
    elsif( KEY(15 downto 0) /= "0000000000000000" ) then
      next_state <= BAD_PSWD_ERROR;
    end if;
  -----------------------
  when PSWD4B =>
    next_state <= PSWD4B;
    if( KEY(7)='1' ) then
      next_state <= PSWD5B;
    elsif(  KEY(15)='1' ) then
      next_state <= BAD_PSWD;
    elsif( KEY(15 downto 0) /= "0000000000000000" ) then
      next_state <= BAD_PSWD_ERROR;
    end if;
  -----------------------
  when PSWD5A =>
    next_state <= PSWD5A;
    if( KEY(5)='1' ) then
      next_state <= PSWD6A;
    elsif(  KEY(15)='1' ) then
      next_state <= BAD_PSWD;
    elsif( KEY(15 downto 0) /= "0000000000000000" ) then
      next_state <= BAD_PSWD_ERROR;
    end if;
  -----------------------
  when PSWD5B =>
    next_state <= PSWD5B;
    if( KEY(2)='1' ) then
      next_state <= PSWD6B;
    elsif(  KEY(15)='1' ) then
      next_state <= BAD_PSWD;
    elsif( KEY(15 downto 0) /= "0000000000000000" ) then
      next_state <= BAD_PSWD_ERROR;
    end if;
  -----------------------
  when PSWD6A =>
    next_state <= PSWD6A;
    if( KEY(0)='1' ) then
      next_state <= PSWD7A;
    elsif(  KEY(15)='1' ) then
      next_state <= BAD_PSWD;
    elsif( KEY(15 downto 0) /= "0000000000000000" ) then
      next_state <= BAD_PSWD_ERROR;
    end if;
  -----------------------
  when PSWD6B =>
    next_state <= PSWD6B;
    if( KEY(5)='1' ) then
      next_state <= PSWD7B;
    elsif(  KEY(15)='1' ) then
      next_state <= BAD_PSWD;
    elsif( KEY(15 downto 0) /= "0000000000000000" ) then
      next_state <= BAD_PSWD_ERROR;
    end if;
  -----------------------
  when PSWD7A =>
    next_state <= PSWD7A;
    if( KEY(3)='1' ) then
      next_state <= PSWD8A;
    elsif(  KEY(15)='1' ) then
      next_state <= BAD_PSWD;
    elsif( KEY(15 downto 0) /= "0000000000000000" ) then
      next_state <= BAD_PSWD_ERROR;
    end if;
  -----------------------
  when PSWD7B =>
    next_state <= PSWD7B;
    if( KEY(5)='1' ) then
      next_state <= PSWD8B;
    elsif(  KEY(15)='1' ) then
      next_state <= BAD_PSWD;
    elsif( KEY(15 downto 0) /= "0000000000000000" ) then
      next_state <= BAD_PSWD_ERROR;
    end if;
  -----------------------
  when PSWD8A =>
    next_state <= PSWD8A;
    if( KEY(7)='1' ) then
      next_state <= PSWD9A;
    elsif(  KEY(15)='1' ) then
      next_state <= BAD_PSWD;
    elsif( KEY(15 downto 0) /= "0000000000000000" ) then
      next_state <= BAD_PSWD_ERROR;
    end if;
  -----------------------   
  when PSWD8B =>
    next_state <= PSWD8B;
    if( KEY(5)='1' ) then
      next_state <= PSWD9B;
    elsif(  KEY(15)='1' ) then
      next_state <= BAD_PSWD;
    elsif( KEY(15 downto 0) /= "0000000000000000" ) then
      next_state <= BAD_PSWD_ERROR;
    end if;
  ----------------------- 
  when PSWD9A =>
    next_state <= PSWD9A;
    if( KEY(0)='1' ) then
      next_state <= PSWD10A;
    elsif(  KEY(15)='1' ) then
      next_state <= BAD_PSWD;
    elsif( KEY(15 downto 0) /= "0000000000000000" ) then
      next_state <= BAD_PSWD_ERROR;
    end if;
  -----------------------
  when PSWD9B =>
    next_state <= PSWD9B;
    if( KEY(6)='1' ) then
      next_state <= PSWD10B;
    elsif(  KEY(15)='1' ) then
      next_state <= BAD_PSWD;
    elsif( KEY(15 downto 0) /= "0000000000000000" ) then
      next_state <= BAD_PSWD_ERROR;
    end if;
  -----------------------
  when PSWD10A =>
    next_state <= PSWD10A;
    if( KEY(9)='1' ) then
      next_state <= LAST_CHAR;
    elsif(  KEY(15)='1' ) then
      next_state <= BAD_PSWD;
    elsif( KEY(15 downto 0) /= "0000000000000000" ) then
      next_state <= BAD_PSWD_ERROR;
    end if;
  -----------------------
  when PSWD10B =>
    next_state <= PSWD10B;
    if( KEY(4)='1' ) then
      next_state <= LAST_CHAR;
    elsif(  KEY(15)='1' ) then
      next_state <= BAD_PSWD;
    elsif( KEY(15 downto 0) /= "0000000000000000" ) then
      next_state <= BAD_PSWD_ERROR;
    end if;
  -----------------------
  when LAST_CHAR =>
    next_state <= LAST_CHAR;
    if( KEY(15)='1' ) then
      next_state <= GOOD_PSWD;
    elsif( KEY(15 downto 0) /= "0000000000000000" ) then
      next_state <= BAD_PSWD_ERROR;
    end if;
  -----------------------  
  when BAD_PSWD_ERROR =>
    next_state <= BAD_PSWD_ERROR;
    if (KEY(15) = '1') then
      next_state <= BAD_PSWD;
    elsif (KEY(15 downto 0) /= "0000000000000000") then
      next_state <= BAD_PSWD_ERROR;
    end if;
  -----------------------
  when GOOD_PSWD =>
    next_state <= GOOD_PSWD;
    if (CNT_OF = '1') then
      next_state <= REINIT;
    end if;
  -----------------------
  when BAD_PSWD =>
    next_state <= BAD_PSWD;
    if (CNT_OF = '1') then
      next_state <= REINIT;
    end if;
  -----------------------
  when REINIT =>
    next_state <= REINIT;
    if (KEY(15) = '1') then
      next_state <= PSWD1;
    end if;
  -----------------------
  when others =>
    next_state <= PSWD1;
  end case;
end process next_state_logic;

---------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when BAD_PSWD =>
      FSM_MX_MEM     <= '0';
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when GOOD_PSWD =>
      FSM_MX_MEM     <= '1';
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when REINIT =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   end case;
end process output_logic;

end architecture behavioral;