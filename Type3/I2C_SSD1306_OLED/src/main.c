// Standard library includes.
#include <stdint.h>
#include <stdlib.h>
// Vendor-provided device header file.
#include "stm32g0xx.h"

// 128x64-pixel monochrome framebuffer.
#define SSD1306_W 128
#define SSD1306_H 64
#define SSD1306_A ( SSD1306_W * SSD1306_H ) / 8
uint8_t FRAMEBUFFER[ SSD1306_A ];
// Initialization commands for the SSD1306 display.
#define NUM_INIT_CMDS 25
const uint8_t INIT_CMDS[ NUM_INIT_CMDS ] = {
  // 0x00, to indicate command bytes.
  0x00,
  // Display clock division, multiplex (# rows)
  0xD5, 0x80, 0xA8, 0x3F,
  // Display offset, start line, charge pump on.
  0xD3, 0x00, 0x40, 0x8D, 0x14,
  // Memory mode, segment remap, desc. column scan.
  0x20, 0x00, 0xA1, 0xC8,
  // 'COMPINS', contrast.
  0xDA, 0x12, 0x81, 0x0A,
  // precharge, VCOM detection level.
  0xD9, 0xF1, 0xDB, 0x40,
  // Output follows RAM, normal mode, display on.
  0xA4, 0xA6, 0xAF
};

// Global variable to hold the core clock speed in Hertz.
uint32_t SystemCoreClock = 16000000;

// Simple imprecise delay method.
void __attribute__( ( optimize( "O0" ) ) )
delay_cycles( uint32_t cyc ) {
  for ( uint32_t d_i = 0; d_i < cyc; ++d_i ) { asm( "NOP" ); }
}

/**
 * Main program.
 */
int main(void) {
  // Enable peripherals: GPIOA, DMA, I2C2.
  RCC->IOPENR   |= RCC_IOPENR_GPIOAEN;
  RCC->AHBENR   |= RCC_AHBENR_DMA1EN;
  RCC->APBENR1  |= RCC_APBENR1_I2C2EN;

  // Pin A11/12 output type: Alt. Func. #6.
  GPIOA->MODER    &= ~( 0x3 << ( 11 * 2 ) |
                        0x3 << ( 12 * 2 ) );
  GPIOA->MODER    |=  ( 0x2 << ( 11 * 2 ) |
                        0x2 << ( 12 * 2 ) );
  GPIOA->AFR[ 1 ] &= ~( GPIO_AFRH_AFSEL11 |
                        GPIO_AFRH_AFSEL12 );
  GPIOA->AFR[ 1 ] |=  ( 0x6 << GPIO_AFRH_AFSEL11_Pos |
                        0x6 << GPIO_AFRH_AFSEL12_Pos );

  // DMA configuration (channel 1).
  // CCR register:
  // - Memory-to-peripheral
  // - Circular mode disabled.
  // - Increment memory ptr, don't increment periph ptr.
  // - 8-bit data size for both source and destination.
  // - High priority.
  DMA1_Channel1->CCR &= ~( DMA_CCR_MEM2MEM |
                           DMA_CCR_PL |
                           DMA_CCR_MSIZE |
                           DMA_CCR_PSIZE |
                           DMA_CCR_PINC |
                           DMA_CCR_CIRC |
                           DMA_CCR_EN );
  DMA1_Channel1->CCR |=  ( ( 0x2 << DMA_CCR_PL_Pos ) |
                           DMA_CCR_MINC |
                           DMA_CCR_DIR );
  // Route DMA channel 0 to I2C2 transmit.
  DMAMUX1_Channel0->CCR &= ~( DMAMUX_CxCR_DMAREQ_ID );
  DMAMUX1_Channel0->CCR |=  ( 13 << DMAMUX_CxCR_DMAREQ_ID_Pos );
  // Set DMA source and destination addresses.
  // Source: Address of the initialization commands.
  DMA1_Channel1->CMAR  = ( uint32_t )&INIT_CMDS;
  // Dest.: 'I2C2 transmit' register.
  DMA1_Channel1->CPAR  = ( uint32_t )&( I2C2->TXDR );
  // Set DMA data transfer length (# of init commands).
  DMA1_Channel1->CNDTR = ( uint16_t )NUM_INIT_CMDS;
  // Enable DMA1 Channel 1.
  DMA1_Channel1->CCR |= ( DMA_CCR_EN );

  // I2C2 configuration:
  // Timing register. For "Fast-Mode+" (1MHz), the RM says:
  // (@16MHz) presc=0, SCLL=4, SCLH=2, SDADEL=0, SCLDEL=2.
  I2C2->TIMINGR  = 0x00200204;
  // Enable the peripheral.
  I2C2->CR1     |= I2C_CR1_PE;
  // Set the device address. Usually 0x78, can be 0x7A.
  // The I2C peripheral also needs to know how many bytes
  // to send before it starts transmitting.
  I2C2->CR2     &= ~( I2C_CR2_SADD |
                      I2C_CR2_NBYTES );
  I2C2->CR2     |=  ( 0x7A << I2C_CR2_SADD_Pos |
                      NUM_INIT_CMDS << I2C_CR2_NBYTES_Pos );
  // Enable I2C DMA requests.
  I2C2->CR1     |=  ( I2C_CR1_TXDMAEN );
  // Send a start signal.
  I2C2->CR2     |=  ( I2C_CR2_START );
  // (DMA is now running.)
  // Wait for DMA to finish.
  while ( !( DMA1->ISR & DMA_ISR_TCIF1 ) ) {};
  DMA1->IFCR |= DMA_IFCR_CTCIF1;
  // Stop the I2C transmission.
  while ( !( I2C2->ISR & I2C_ISR_TC ) ) {};
  I2C2->CR2  |=  ( I2C_CR2_STOP );
  while ( I2C2->ISR & I2C_ISR_BUSY ) {};

  // Reconfigure DMA and I2C for sending the framebuffer.
  // Disable the DMA channel.
  DMA1_Channel1->CCR &= ~( DMA_CCR_EN );
  // Set DMA circular mode.
  DMA1_Channel1->CCR |=  ( DMA_CCR_CIRC );
  // Set I2C autoreload and the maximum 255 byte length.
  I2C2->CR2      &= ~( I2C_CR2_NBYTES );
  I2C2->CR2      |=  ( I2C_CR2_RELOAD |
                       255 << I2C_CR2_NBYTES_Pos );
  // Enable the I2C2 interrupt.
  NVIC_SetPriority( I2C2_IRQn, 0x03 );
  NVIC_EnableIRQ( I2C2_IRQn );
  // Enable the 'transfer complete' I2C interrupt.
  I2C2->CR1      |= I2C_CR1_TCIE;
  // Update DMA source/destination/size registers.
  // Source: Address of the framebuffer.
  DMA1_Channel1->CMAR  = ( uint32_t )&FRAMEBUFFER;
  // Dest.: 'I2C2 transmit' register.
  DMA1_Channel1->CPAR  = ( uint32_t )&( I2C2->TXDR );
  // Set DMA data transfer length (framebuffer length).
  DMA1_Channel1->CNDTR = ( uint16_t )SSD1306_A;
  // Send a start signal.
  I2C2->CR2     |=  ( I2C_CR2_START );
  while ( !( I2C2->CR2 & I2C_CR2_START ) ) {};
  // Send '0x40' to indicate display data.
  I2C2->TXDR = 0x40;
  // Re-enable DMA1 Channel 1.
  DMA1_Channel1->CCR |= ( DMA_CCR_EN );

  // Done; now draw patterns to the framebuffer.
  // The display is configured to hold 8 vertical pixels in
  // each byte, with the first 128 bytes representing
  // y-coordinates [0:7], the next 128 bytes [8:15], and so on.
  // So if we set each byte to the same value, it will look
  // like a pattern of horizontal lines of varying thickness.
  uint8_t val = 0x00;
  while (1) {
    // Draw the new pattern to the framebuffer.
    for ( size_t i = 0; i < SSD1306_A; ++i ) {
      FRAMEBUFFER[ i ] = val;
    }
    // Update the pattern.
    ++val;
    // Delay briefly.
    delay_cycles( 200000 );
  }
}

// I2C2 interrupt handler.
void I2C2_IRQ_handler( void ) {
  if ( I2C2->ISR & I2C_ISR_TCR ) {
    I2C2->CR2 &= ~( I2C_CR2_NBYTES );
    I2C2->CR2 |=  ( 255 << I2C_CR2_NBYTES_Pos );
  }
}
