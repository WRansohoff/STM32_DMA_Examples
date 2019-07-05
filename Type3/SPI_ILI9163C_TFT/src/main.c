// Standard library includes.
#include <stdint.h>
#include <stdlib.h>
// Vendor-provided device header file.
#include "stm32g0xx.h"

// 128x128-pixel 16-bit (RGB-565) framebuffer.
#define ILI9163C_W ( 128 )
#define ILI9163C_H ( 128 )
#define ILI9163C_A ( ILI9163C_W * ILI9163C_H )
uint16_t FRAMEBUFFER[ ILI9163C_A ];
// Macro definitions for 'command' (0) and 'data' (1) modes.
#define ILI9163C_CMD ( 0 )
#define ILI9163C_DAT ( 1 )
// Software-controlled pin macros, for convenience.
// B4 = CS, B6 = Reset, B7 = Data/Command.
#define TFT_CS  ( GPIO_ODR_OD4 )
#define TFT_RST ( GPIO_ODR_OD6 )
#define TFT_DC  ( GPIO_ODR_OD7 )

// Global variable to hold the core clock speed in Hertz.
uint32_t SystemCoreClock = 16000000;

// Simple imprecise delay method.
void __attribute__( ( optimize( "O0" ) ) )
delay_cycles( uint32_t cyc ) {
  for ( uint32_t d_i = 0; d_i < cyc; ++d_i ) { asm( "NOP" ); }
}

// Write a byte to the SPI peripheral.
void spi_w8( SPI_TypeDef *SPIx, uint8_t dat ) {
  // Wait for TXE 'transmit buffer empty' bit to be set.
  while ( !( SPIx->SR & SPI_SR_TXE ) ) {};
  // Send the byte.
  *( uint8_t* )&( SPIx->DR ) = dat;
}

// Write two bytes to the SPI peripheral. Note that they
// send in the order of 0x2211. (1 = first, 2 = second)
void spi_w16( SPI_TypeDef *SPIx, uint16_t dat ) {
  // Wait for TXE 'transmit buffer empty' bit to be set.
  while ( !( SPIx->SR & SPI_SR_TXE ) ) {};
  // Send the bytes.
  *( uint16_t* )&( SPIx->DR ) = dat;
}

// Method to set the 'data / command' pin.
void dat_cmd( SPI_TypeDef *SPIx, uint8_t dc ) {
  // Wait for the BSY 'busy' bit to be cleared.
  while ( SPIx->SR & SPI_SR_BSY ) {};
  // Set the D/C pin appropriately.
  if ( dc ) { GPIOB->ODR |=  ( TFT_DC ); }
  else      { GPIOB->ODR &= ~( TFT_DC ); }
}

/**
 * Main program.
 */
int main(void) {
  // Enable peripherals: GPIOB, DMA, SPI1.
  RCC->IOPENR   |= RCC_IOPENR_GPIOBEN;
  RCC->AHBENR   |= RCC_AHBENR_DMA1EN;
  RCC->APBENR2  |= RCC_APBENR2_SPI1EN;

  // Setup core clock to 64MHz.
  // Set 2 wait states in Flash.
  FLASH->ACR &= ~( FLASH_ACR_LATENCY );
  FLASH->ACR |=  ( 2 << FLASH_ACR_LATENCY_Pos );
  // Configure PLL; R = 2, M = 1, N = 8.
  // freq = ( 16MHz * ( N / M ) ) / R
  RCC->PLLCFGR &= ~( RCC_PLLCFGR_PLLR |
                     RCC_PLLCFGR_PLLREN |
                     RCC_PLLCFGR_PLLN |
                     RCC_PLLCFGR_PLLM |
                     RCC_PLLCFGR_PLLSRC );
  RCC->PLLCFGR |=  ( 1 << RCC_PLLCFGR_PLLR_Pos |
                     8 << RCC_PLLCFGR_PLLN_Pos |
                     RCC_PLLCFGR_PLLREN |
                     2 << RCC_PLLCFGR_PLLSRC_Pos );
  // Enable and select the PLL.
  RCC->CR   |= RCC_CR_PLLON;
  while ( !( RCC->CR & RCC_CR_PLLRDY ) ) {};
  RCC->CFGR &= ~( RCC_CFGR_SW );
  RCC->CFGR |=  ( 2 << RCC_CFGR_SW_Pos );
  while ( ( RCC->CFGR & RCC_CFGR_SWS ) >> RCC_CFGR_SWS_Pos != 2 ) {};
  // System clock is now 64MHz.
  SystemCoreClock = 64000000;

  // Setup pins: B3/5 are AF#0 (SPI1) (SCK/SDO),
  // B4 = CS, B6 = Reset, B7 = D/C.
  GPIOB->MODER    &= ~( 0x3 << ( 3 * 2 ) |
                        0x3 << ( 4 * 2 ) |
                        0x3 << ( 5 * 2 ) |
                        0x3 << ( 6 * 2 ) |
                        0x3 << ( 7 * 2 ) );
  GPIOB->MODER    |=  ( 0x2 << ( 3 * 2 ) |
                        0x1 << ( 4 * 2 ) |
                        0x2 << ( 5 * 2 ) |
                        0x1 << ( 6 * 2 ) |
                        0x1 << ( 7 * 2 ) );
  GPIOB->AFR[ 0 ] &= ~( GPIO_AFRL_AFSEL3 |
                        GPIO_AFRL_AFSEL5 );
  // Initial pin states: DC low, CS/Reset high.
  GPIOB->ODR      &= ~( TFT_DC );
  GPIOB->ODR      |=  ( TFT_CS | TFT_RST );

  // DMA configuration (channel 1).
  // CCR register:
  // - Memory-to-peripheral
  // - Circular mode enabled.
  // - Increment memory ptr, don't increment periph ptr.
  // - 16-bit data size for both source and destination.
  // - High priority.
  DMA1_Channel1->CCR &= ~( DMA_CCR_MEM2MEM |
                           DMA_CCR_PL |
                           DMA_CCR_MSIZE |
                           DMA_CCR_PSIZE |
                           DMA_CCR_PINC |
                           DMA_CCR_EN );
  DMA1_Channel1->CCR |=  ( ( 0x2 << DMA_CCR_PL_Pos ) |
                           ( 0x1 << DMA_CCR_MSIZE_Pos ) |
                           ( 0x1 << DMA_CCR_PSIZE_Pos ) |
                           DMA_CCR_MINC |
                           DMA_CCR_CIRC |
                           DMA_CCR_DIR );
  // Route DMA channel 0 to SPI1 transmit.
  DMAMUX1_Channel0->CCR &= ~( DMAMUX_CxCR_DMAREQ_ID );
  DMAMUX1_Channel0->CCR |=  ( 17 << DMAMUX_CxCR_DMAREQ_ID_Pos );
  // Set DMA source and destination addresses.
  // Source: Address of the framebuffer.
  DMA1_Channel1->CMAR  = ( uint32_t )&FRAMEBUFFER;
  // Destination: SPI1 data register.
  DMA1_Channel1->CPAR  = ( uint32_t )&( SPI1->DR );
  // Set DMA data transfer length (framebuffer length).
  DMA1_Channel1->CNDTR = ( uint16_t )ILI9163C_A;

  // Toggle pin B6 to reset the display.
  GPIOB->ODR &= ~( TFT_RST );
  delay_cycles( 200000 );
  GPIOB->ODR |=  ( TFT_RST );

  // SPI1 configuration:
  // - Clock phase/polarity: 1/1
  // - Assert internal CS signal (software CS pin control)
  // - MSB-first
  // - 8-bit frames
  // - Baud rate prescaler of 4 (or 128 for debugging)
  // - TX DMA requests enabled.
  SPI1->CR1 &= ~( SPI_CR1_LSBFIRST |
                  SPI_CR1_BR );
  SPI1->CR1 |=  ( SPI_CR1_SSM |
                  SPI_CR1_SSI |
                  0x1 << SPI_CR1_BR_Pos |
                  SPI_CR1_MSTR |
                  SPI_CR1_CPOL |
                  SPI_CR1_CPHA );
  SPI1->CR2 &= ~( SPI_CR2_DS );
  SPI1->CR2 |=  ( 0x7 << SPI_CR2_DS_Pos |
                  SPI_CR2_TXDMAEN );
  // Enable the SPI peripheral.
  SPI1->CR1 |=  ( SPI_CR1_SPE );

  // Send initialization commands.
  // Pull CS pin low.
  GPIOB->ODR &= ~( TFT_CS );
  // Software reset.
  dat_cmd( SPI1, ILI9163C_CMD );
  spi_w8( SPI1, 0x01 );
  delay_cycles( 200000 );
  // Display off.
  spi_w8( SPI1, 0x28 );
  // Color mode: 16bpp.
  spi_w8( SPI1, 0x3A );
  dat_cmd( SPI1, ILI9163C_DAT );
  spi_w8( SPI1, 0x55 );
  // Exit sleep mode.
  dat_cmd( SPI1, ILI9163C_CMD );
  spi_w8( SPI1, 0x11 );
  delay_cycles( 200000 );
  // Display on.
  spi_w8( SPI1, 0x29 );
  delay_cycles( 200000 );
  // Set drawing window.
  // The displays I got are offset by a few pixels.
  // So instead of setting X/Y ranges of [0:127]...
  // Column set: [2:129]
  spi_w8( SPI1, 0x2A );
  dat_cmd( SPI1, ILI9163C_DAT );
  spi_w16( SPI1, 0x0200 );
  spi_w16( SPI1, 0x8100 );
  dat_cmd( SPI1, ILI9163C_CMD );
  // Row set: [1:128]
  spi_w8( SPI1, 0x2B );
  dat_cmd( SPI1, ILI9163C_DAT );
  spi_w16( SPI1, 0x0100 );
  spi_w16( SPI1, 0x8000 );
  dat_cmd( SPI1, ILI9163C_CMD );
  // Set 'write to RAM' mode.
  spi_w8( SPI1, 0x2C );
  // From now on, we'll only be sending pixel data.
  dat_cmd( SPI1, ILI9163C_DAT );

  // Enable DMA1 Channel 1 to start sending the framebuffer.
  DMA1_Channel1->CCR |= ( DMA_CCR_EN );

  // Done; now just alternate between solid colors to get
  // a feel for the refresh speed.
  uint16_t color = 0x1984;
  while (1) {
    // Draw the new color to the framebuffer.
    for ( size_t i = 0; i < ILI9163C_A; ++i ) {
      FRAMEBUFFER[ i ] = color;
    }
    // Invert the color.
    color = color ^ 0xFFFF;
    // Delay briefly.
    delay_cycles( 2500000 );
  }
}
