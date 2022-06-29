/*
 * Driver per il modulo UART
 */

#include "hardwareprofile.h"
#include "system.h"
#include <stdint.h>
#include <string.h>
#include "gel/timer/timecheck.h"
#include "timer.h"
#include "uart_driver.h"


#define UART_BUFFER_SIZE 512
#define UART_BUFFER_RX 300
//#define UART_TIMEOUT     100
#define UART_TIMEOUT     50

static uint8_t       uart_tx_buffer[UART_BUFFER_SIZE];
volatile static int  f_tx_on  = 0;
volatile static int  len_tx   = 0;
volatile static int  index_tx = 0;

static uint8_t       uart_rx_buffer[UART_BUFFER_RX];
volatile static int  index_rx = 0;
static unsigned long ts=0;

#define BAUDRATE    115200
#define BAUDRATEREG FOSC / 8 / BAUDRATE - 1


#if BAUDRATEREG > 0xFFFF
#error Cannot set up UART1 for the SYSCLK and BAUDRATE.
Correct values in main.h and uart2.h files
    .
#endif


/*----------------------------------------------------------------------------*/
/*  Inizializzazione UART                                                     */
/*----------------------------------------------------------------------------*/
void uart_init(void) {
    U1MODEbits.UARTEN = 0;     // Bit15 TX, RX DISABLED, ENABLE at end of func

    UART_RX_TRIS = 1;
    UART_TX_TRIS = 0;

    U1MODEbits.USIDL  = 0;     // Bit13 Continue in Idle
    U1MODEbits.IREN   = 0;     // Bit12 No IR translation
    U1MODEbits.RTSMD  = 0;     // Bit11 Simplex Mode //0?
    U1MODEbits.UEN    = 0;     // Bits8,9 TX,RX enabled, CTS,RTS not
    U1MODEbits.WAKE   = 0;     // Bit7 No Wake up (since we don't sleep here)
    U1MODEbits.LPBACK = 0;     // Bit6 No Loop Back
    U1MODEbits.ABAUD  = 0;     // Bit5 No Autobaud (would require sending '55')
    U1MODEbits.RXINV  = 0;     // Bit4 IdleState = 1
    U1MODEbits.BRGH   = 1;     // Bit3 4 clocks per bit period
    U1MODEbits.PDSEL  = 0;     // Bits1,2 8bit, No Parity
    U1MODEbits.STSEL  = 0;     // Bit0 One Stop Bit

    U1BRG = BAUDRATEREG;     // baud rate

    // Load all values in for U1STA SFR
    U1STAbits.UTXISEL1 = 1;     // Bit15 Interrupt quando il buffer di 4 byte e' vuoto
    U1STAbits.UTXINV   = 0;     // Bit14 N/A, IRDA config => 0 = Idle state is 1, 1 = idle state is 0
    U1STAbits.UTXISEL0 = 0;     // Bit13 Other half of Bit15
    U1STAbits.UTXBRK   = 0;     // Bit11 Sync Break transmission is disabled
    U1STAbits.UTXEN    = 0;     // Bit10 TX pins controlled by periph
    U1STAbits.URXISEL  = 0;     // Bits6,7 Int. on character received

    U1STAbits.ADDEN = 0;     // Bit5 Address Detect Disabled
    U1STAbits.OERR  = 0;     // Bit1 *Clear or Read Only Bit*

    IPC2bits.U1RXIP = 3;     // Priority of the U1RXInterrupt
    IPC3bits.U1TXIP = 3;

    IFS0bits.U1TXIF = 0;     // Clear the Transmit Interrupt Flag
    IEC0bits.U1TXIE = 1;     // Enable Transmit Interrupts
    IFS0bits.U1RXIF = 0;     // Clear the Receive Interrupt Flag
    IEC0bits.U1RXIE = 1;     // Enable Receive Interrupts
    IFS4bits.U1ERIF  = 0;     // Clear Error Interrupt Flag
    IEC4bits.U1ERIE  = 1;     // Enable Error Interrupts

    U1MODEbits.UARTEN = 1;     // And turn the peripheral on
    U1STAbits.UTXEN   = 1;     // Transmit is enabled, UxTX pin is controlled by UARTx
    
}

int uart_sync_write(uint8_t *data, int len) {
    int i = 0;
    
    IEC0bits.U1TXIE = 0;
    while(i < len) {
        if (!U1STAbits.UTXBF)
            U1TXREG = data[i++];
    }
    while (!U1STAbits.TRMT);
    IEC0bits.U1TXIE = 1;
    return len;
}

int uart_read_rx_buffer(uint8_t *buffer) {
    memcpy(buffer, uart_rx_buffer, index_rx);
    return index_rx;
}

void uart_clean_rx_buffer() {
    index_rx=0;
}

int uart_async_write(uint8_t *data, int len) {
    
    if (f_tx_on) return -1;
    else {
        f_tx_on=1;
        memcpy(uart_tx_buffer, data, len);
        len_tx=len;
        index_tx=0;
        U1TXREG=uart_tx_buffer[index_tx++];
    }
     return len;
}

void __attribute((interrupt, no_auto_psv)) _U1ErrInterrupt(void)
{
    // In caso di errore lo ignoriamo soltanto
    IFS4bits.U1ERIF = 0;

    if (U1STAbits.OERR)
        U1STAbits.OERR = 0;
}


/*----------------------------------------------------------------------------*/
/*  Interrupt UART 1 - RX                                                     */
/*----------------------------------------------------------------------------*/
void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void)
{
    if (is_expired(ts, get_millis(), 200)) {
        index_rx=0;
        ts=0;
    }
    if (index_rx<UART_BUFFER_RX) {
        ts=get_millis();
        // DIR_LAT=0;
        uart_rx_buffer[index_rx++]=U1RXREG;
    }
    
    IFS0bits.U1RXIF = 0;
   
}


/*----------------------------------------------------------------------------*/
/*  Interrupt UART 1 - TX                                                     */
/*----------------------------------------------------------------------------*/
void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void)
{
    
    if (index_tx<len_tx) {
        while(index_tx < len_tx && !U1STAbits.UTXBF)
            U1TXREG=uart_tx_buffer[index_tx++];
    }
    else {
        f_tx_on=0;
    }
    
    IFS0bits.U1TXIF = 0;
    
  
}
