// Standard library includes.
#include <stdint.h>
#include <stdlib.h>
// Vendor-provided device header file.
#include "stm32g0xx.h"

// 32-sample sine wave.
#define _AMP(x) ( x / 8 )
const size_t SINE_SAMPLES = 32;
const uint16_t SINE_WAVE[] = {
  _AMP(2048), _AMP(2447), _AMP(2831), _AMP(3185),
  _AMP(3495), _AMP(3750), _AMP(3939), _AMP(4056),
  _AMP(4095), _AMP(4056), _AMP(3939), _AMP(3750),
  _AMP(3495), _AMP(3185), _AMP(2831), _AMP(2447),
  _AMP(2048), _AMP(1649), _AMP(1265), _AMP(911),
  _AMP(601),  _AMP(346),  _AMP(157),  _AMP(40),
  _AMP(0),    _AMP(40),   _AMP(157),  _AMP(346),
  _AMP(601),  _AMP(911),  _AMP(1265), _AMP(1649)
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
  // Enable peripherals: GPIOA, DMA, DAC, TIM6.
  RCC->IOPENR   |= RCC_IOPENR_GPIOAEN;
  RCC->AHBENR   |= RCC_AHBENR_DMA1EN;
  RCC->APBENR1  |= ( RCC_APBENR1_DAC1EN |
                     RCC_APBENR1_TIM6EN );

  // Pin A4 output type: Analog.
  GPIOA->MODER    &= ~( 0x3 << ( 4 * 2 ) );
  GPIOA->MODER    |=  ( 0x3 << ( 4 * 2 ) );

  // DMA configuration (channel 1).
  // CCR register:
  // - Memory-to-peripheral
  // - Circular mode enabled.
  // - Increment memory ptr, don't increment periph ptr.
  // - 16-bit data size for both source and destination.
  // - High priority.
  uint32_t dma_ccr_clr = ~( DMA_CCR_MEM2MEM |
                            DMA_CCR_PL |
                            DMA_CCR_MSIZE |
                            DMA_CCR_PSIZE |
                            DMA_CCR_PINC |
                            DMA_CCR_EN );
  uint32_t dma_ccr_set =  ( ( 0x2 << DMA_CCR_PL_Pos ) |
                            ( 0x1 << DMA_CCR_MSIZE_Pos ) |
                            ( 0x1 << DMA_CCR_PSIZE_Pos ) |
                            DMA_CCR_MINC |
                            DMA_CCR_CIRC |
                            DMA_CCR_DIR );
  DMA1_Channel1->CCR &= dma_ccr_clr;
  DMA1_Channel1->CCR |= dma_ccr_set;
  // Select DAC Ch1 as DMA Ch1 request source in DMAMUX.
  // Note: DMAMUX channel numbers are slightly confusing in
  // the documentation. They aren't reliably 0- or 1-indexed.
  DMAMUX1_Channel0->CCR &= ~( DMAMUX_CxCR_DMAREQ_ID );
  DMAMUX1_Channel0->CCR |=  ( 0x8 << DMAMUX_CxCR_DMAREQ_ID_Pos );
  // Set DMA source and destination addresses.
  // Source: Address of the sine wave buffer in memory.
  DMA1_Channel1->CMAR  = ( uint32_t )&SINE_WAVE;
  // Dest.: DAC1 Ch1 '12-bit right-aligned data' register.
  DMA1_Channel1->CPAR  = ( uint32_t )&( DAC1->DHR12R1 );
  // Set DMA data transfer length (# of sine wave samples).
  DMA1_Channel1->CNDTR = ( uint16_t )SINE_SAMPLES;
  // Enable DMA1 Channels 1/2.
  DMA1_Channel1->CCR |= ( DMA_CCR_EN );

  // TIM6 configuration.
  // Set prescaler and autoreload for a 440Hz sine wave.
  TIM6->PSC  =  ( 0x0000 );
  TIM6->ARR  =  ( SystemCoreClock / ( 440 * SINE_SAMPLES ) );
  // Enable trigger output on timer update events.
  TIM6->CR2 &= ~( TIM_CR2_MMS );
  TIM6->CR2 |=  ( 0x2 << TIM_CR2_MMS_Pos );
  // Start the timer.
  TIM6->CR1 |=  ( TIM_CR1_CEN );

  // DAC configuration.
  // Set trigger source to TIM6 TRGO.
  DAC1->CR  &= ~( DAC_CR_TSEL1 );
  DAC1->CR  |=  ( 0x5 << DAC_CR_TSEL1_Pos );
  // Set outputs to buffered GPIO 'normal mode'.
  DAC1->MCR &= ~( DAC_MCR_MODE1 );
  // Enable DAC DMA requests.
  DAC1->CR  |=  ( DAC_CR_DMAEN1 );
  // Enable DAC Channels.
  DAC1->CR  |=  ( DAC_CR_EN1 );
  // Delay briefly to allow sampling to stabilize (?)
  delay_cycles( 1000 );
  // Enable DAC channel trigger.
  DAC1->CR  |=  ( DAC_CR_TEN1 );

  // Done; a low-res 440Hz sine wave should be playing on PA4.
  while (1) {}
}
