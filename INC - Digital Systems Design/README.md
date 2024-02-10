# UART RX

## April 2023

### Description

Circuit for receiving data words after asynchronous serial lin

### Architecture of the designed circuit at the RTL level

> **_NOTE:_**  There are no CLK and RST inputs at scheme

![Scheme picture](/assets/uart.png)

### Finite State Machine

Legend:
+ States: IDLE, WAIT_FISRT, READ_DATA, STOP_READ
+ Inputs: DIN, CNT_CYCLE [4:0], CNT_BIT [3:0]
+ Outputs (Moore): DOUT_VLD, CYCLE_EN, BIT_EN

![Scheme picture](/assets/fsm.png)
