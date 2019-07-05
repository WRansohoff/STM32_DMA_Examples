// Standard library includes.
#include <stdint.h>
#include <stdlib.h>
// Vendor-provided device header file.
#include "stm32f3xx.h"

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
uint32_t SystemCoreClock = 8000000;

// Simple imprecise delay method.
void __attribute__( ( optimize( "O0" ) ) )
delay_cycles( uint32_t cyc ) {
  for ( uint32_t d_i = 0; d_i < cyc; ++d_i ) { asm( "NOP" ); }
}

/**
 * Main program.
 */
int main(void) {
  // Enable peripherals: GPIOA, DMA, DAC, TIM6, SYSCFG.
  RCC->AHBENR   |= ( RCC_AHBENR_GPIOAEN |
                     RCC_AHBENR_DMA1EN );
  RCC->APB1ENR  |= ( RCC_APB1ENR_DAC1EN |
                     RCC_APB1ENR_TIM6EN );
  RCC->APB2ENR  |= RCC_APB2ENR_SYSCFGEN;

  // Pin A4: analog mode. (PA4 = DAC1, Channel 1)
  GPIOA->MODER    &= ~( 0x3 << ( 4 * 2 ) );
  GPIOA->MODER    |=  ( 0x3 << ( 4 * 2 ) );

  // Set the 'TIM6/DAC1 remap' bit in SYSCFG_CFGR1,
  // so that DAC1_Ch1 maps to DMA1_Ch3 instead of DMA2_Ch3.
  // (Not all STM32F303 chips have a DMA2 peripheral)
  SYSCFG->CFGR1 |=  ( SYSCFG_CFGR1_TIM6DAC1Ch1_DMA_RMP );

  // DMA configuration (DMA1, channel 3).
  // CCR register:
  // - Memory-to-peripheral
  // - Circular mode enabled.
  // - Increment memory ptr, don't increment periph ptr.
  // - 16-bit data size for both source and destination.
  // - High priority (2/3).
  DMA1_Channel3->CCR &= ~( DMA_CCR_MEM2MEM |
                          DMA_CCR_PL |
                          DMA_CCR_MSIZE |
                          DMA_CCR_PSIZE |
                          DMA_CCR_PINC |
                          DMA_CCR_EN );
  DMA1_Channel3->CCR |=  ( ( 0x2 << DMA_CCR_PL_Pos ) |
                           ( 0x1 << DMA_CCR_MSIZE_Pos ) |
                           ( 0x1 << DMA_CCR_PSIZE_Pos ) |
                           DMA_CCR_MINC |
                           DMA_CCR_CIRC |
                           DMA_CCR_DIR );
  // Set DMA source and destination addresses.
  // Source: Address of the sine wave buffer in memory.
  DMA1_Channel3->CMAR  = ( uint32_t )&SINE_WAVE;
  // Dest.: DAC1 Ch1 '12-bit right-aligned data' register.
  DMA1_Channel3->CPAR  = ( uint32_t )&( DAC1->DHR12R1 );
  // Set DMA data transfer length (# of sine wave samples).
  DMA1_Channel3->CNDTR = ( uint16_t )SINE_SAMPLES;
  // Enable DMA1 Channel 1.
  // Note: the transfer won't actually start here, because
  // the DAC peripheral is not sending DMA requests yet.
  DMA1_Channel3->CCR |= ( DMA_CCR_EN );

  // TIM6 configuration. This timer will set the frequency
  // at which the DAC peripheral requests DMA transfers.
  // Set prescaler and autoreload for a 440Hz sine wave.
  TIM6->PSC  =  ( 0x0000 );
  TIM6->ARR  =  ( SystemCoreClock / ( 440 * SINE_SAMPLES ) );
  // Enable trigger output on timer update events.
  TIM6->CR2 &= ~( TIM_CR2_MMS );
  TIM6->CR2 |=  ( 0x2 << TIM_CR2_MMS_Pos );
  // Start the timer.
  TIM6->CR1 |=  ( TIM_CR1_CEN );

  // DAC configuration.
  // Set trigger sources to TIM6 TRGO (TRiGger Output).
  DAC1->CR  &= ~( DAC_CR_TSEL1 );
  // Enable DAC DMA requests for channel 1.
  DAC1->CR  |=  ( DAC_CR_DMAEN1 );
  // Enable DAC channel 1.
  DAC1->CR  |=  ( DAC_CR_EN1 );
  // Delay briefly to allow sampling to stabilize.
  delay_cycles( 1000 );
  // Enable DAC channel trigger.
  // The DMA channel and timer are both already on, so the
  // DMA transfer will start as soon as the DAC peripheral
  // starts making requests. The DAC peripheral will make a
  // request every time that TIM6 ticks over, but only after
  // this 'trigger enable' bit is set.
  DAC1->CR  |=  ( DAC_CR_TEN1 );

  // Done; a low-res 440Hz sine wave should be playing on PA4.
  while (1) {}
}
