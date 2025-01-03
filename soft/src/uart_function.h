
// Base address
#define UART0_BASE_ADDRESS 0xFFC02000

// UART0 register offset (not all are used)
#define  RBR_THR_DLL    0x0
#define  IER_DLH        0x4
#define  IIR            0x8
#define  FCR            0x8
#define  LCR            0xC
#define  MCR            0x10
#define  LSR            0x14
#define  MSR            0x18
#define  SCR            0x1C
#define  SRBR           0x30
#define  STHR           0x34
#define  FAR            0x70
#define  TFR            0x74
#define  RFW            0x78
#define  USR            0x7C
#define  TFL            0x80
#define  RFL            0x84
#define  SRR            0x88
#define  SRTS           0x8C
#define  SBCR           0x90
#define  SDMAM          0x94
#define  SFE            0x98
#define  SRT            0x9C
#define  STET           0xA0
#define  HTX            0xA4
#define  DMASA          0xA8
#define  CPR            0xF4
#define  UCV            0xF8
#define  CTR            0xFC

// ACCESS MACROS
#define UART0_REG(_x_)   (volatile uint32_t *)(AXI_LW_HPS_FPGA_BASE_ADD + _x_)

// MASK 
#define  LSR_THRE_MASK            0x20


void uart_config();
void uart_send_char(char c);
void send_to_uart(const char* mystring);
