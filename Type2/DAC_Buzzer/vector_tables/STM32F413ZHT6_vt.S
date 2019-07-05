/*
 * Minimal vector table for an STM32F303K8.
 */

.syntax unified
.cpu cortex-m4
.fpu softvfp /* TODO: Hard floats. */
.thumb

// Global values.
.global vtable
.global default_interrupt_handler

.type vtable, %object
.section .vector_table,"a",%progbits
vtable:
    // 0-15
    .word _estack
    .word reset_handler
    .word NMI_handler
    .word hard_fault_handler
    .word mem_manage_handler
    .word bus_fault_handler
    .word usage_fault_handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVC_handler
    .word debug_monitor_handler
    .word 0
    .word pending_SV_handler
    .word SysTick_handler
    .word window_watchdog_IRQ_handler
    .word PVD_IRQ_handler
    .word tamper_IRQ_handler
    .word RTC_wakeup_IRQ_handler
    .word flash_IRQ_handler
    .word RCC_IRQ_handler
    .word EXTI0_IRQ_handler
    .word EXTI1_IRQ_handler
    .word EXTI2_IRQ_handler
    .word EXTI3_IRQ_handler
    .word EXTI4_IRQ_handler
    .word DMA_Stream0_IRQ_handler
    .word DMA_Stream1_IRQ_handler
    .word DMA_Stream2_IRQ_handler
    .word DMA_Stream3_IRQ_handler
    .word DMA_Stream4_IRQ_handler
    .word DMA_Stream5_IRQ_handler
    .word DMA_Stream6_IRQ_handler
    .word ADC_IRQ_handler
    .word CAN1_TX_IRQ_handler
    .word CAN1_RX0_IRQ_handler
    .word CAN1_RX1_IRQ_handler
    .word CAN1_SCE_IRQ_handler
    .word EXTI5_9_IRQ_handler
    .word TIM1_BRK_TIM9_IRQ_handler
    .word TIM1_UP_TIM10_IRQ_handler
    .word TIM1_TRG_COM_TIM11_IRQ_handler
    .word TIM1_CC_IRQ_handler
    .word TIM2_IRQ_handler
    .word TIM3_IRQ_handler
    .word TIM4_IRQ_handler
    .word I2C1_EV_IRQ_handler
    .word I2C1_ER_IRQ_handler
    .word I2C2_EV_IRQ_handler
    .word I2C2_ER_IRQ_handler
    .word SPI1_IRQ_handler
    .word SPI2_IRQ_handler
    .word USART1_IRQ_handler
    .word USART2_IRQ_handler
    .word USART3_IRQ_handler
    .word EXTI10_15_IRQ_handler
    .word RTC_alarm_IRQ_handler
    .word OTG_WKUP_IRQ_handler
    .word TIM8_BRK_TIM12_IRQ_handler
    .word TIM8_UP_TIM13_IRQ_handler
    .word TIM8_TRG_COM_TIM14_IRQ_handler
    .word TIM8_CC_IRQ_handler
    .word DMA1_Stream7_IRQ_handler
    .word FSMC_IRQ_handler
    .word SDIO_IRQ_handler
    .word TIM5_IRQ_handler
    .word SPI3_IRQ_handler
    .word UART4_IRQ_handler
    .word UART5_IRQ_handler
    .word TIM6_DAC_IRQ_handler
    .word TIM7_IRQ_handler
    .word DMA2_Stream0_IRQ_handler
    .word DMA2_Stream1_IRQ_handler
    .word DMA2_Stream2_IRQ_handler
    .word DMA2_Stream3_IRQ_handler
    .word DMA2_Stream4_IRQ_handler
    .word DFSDM1_FLT0_IRQ_handler
    .word DFSDM1_FLT1_IRQ_handler
    .word CAN2_TX_IRQ_handler
    .word CAN2_RX0_IRQ_handler
    .word CAN2_RX1_IRQ_handler
    .word CAN2_SCE_IRQ_handler
    .word OTG_FS_IRQ_handler
    .word DMA2_Stream5_IRQ_handler
    .word DMA2_Stream6_IRQ_handler
    .word DMA2_Stream7_IRQ_handler
    .word USART6_IRQ_handler
    .word I2C3_EV_IRQ_handler
    .word I2C3_ER_IRQ_handler
    .word CAN3_TX_IRQ_handler
    .word CAN3_RX0_IRQ_handler
    .word CAN3_RX1_IRQ_handler
    .word CAN3_SCE_IRQ_handler
    .word 0
    .word 0
    .word RNG_IRQ_handler
    .word FPU_IRQ_handler
    .word UART7_IRQ_handler
    .word UART8_IRQ_handler
    .word SPI4_IRQ_handler
    .word SPI5_IRQ_handler
    .word 0
    .word SAI1_IRQ_handler
    .word UART9_IRQ_handler
    .word UART10_IRQ_handler
    .word 0
    .word 0
    .word QSPI_IRQ_handler
    .word 0
    .word 0
    .word FMPI2C1_EV_IRQ_handler
    .word FMPI2C1_ER_IRQ_handler
    .word LPTIM1_IRQ_handler
    .word DFSDM2_FLT0_IRQ_handler
    .word DFSDM2_FLT1_IRQ_handler
    .word DFSDM2_FLT2_IRQ_handler
    .word DFSDM2_FLT3_IRQ_handler

    /*
     * Setup weak aliases for each exception handler to the
     * default one. These can be updated later, or just
     * overridden since they're weak refs.
     * The reset_handler is set up separately.
     */
    .weak NMI_handler
    .thumb_set NMI_handler, default_interrupt_handler
    .weak hard_fault_handler
    .thumb_set hard_fault_handler, default_interrupt_handler
    .weak mem_manage_handler
    .thumb_set mem_manage_handler, default_interrupt_handler
    .weak bus_fault_handler
    .thumb_set bus_fault_handler, default_interrupt_handler
    .weak usage_fault_handler
    .thumb_set usage_fault_handler, default_interrupt_handler
    .weak SVC_handler
    .thumb_set SVC_handler, default_interrupt_handler
    .weak debug_monitor_handler
    .thumb_set debug_monitor_handler, default_interrupt_handler
    .weak pending_SV_handler
    .thumb_set pending_SV_handler, default_interrupt_handler
    .weak SysTick_handler
    .thumb_set SysTick_handler, default_interrupt_handler
    .weak window_watchdog_IRQ_handler
    .thumb_set window_watchdog_IRQ_handler, default_interrupt_handler
    .weak PVD_IRQ_handler
    .thumb_set PVD_IRQ_handler, default_interrupt_handler
    .weak tamper_IRQ_handler
    .thumb_set tamper_IRQ_handler, default_interrupt_handler
    .weak RTC_wakeup_IRQ_handler
    .thumb_set RTC_wakeup_IRQ_handler, default_interrupt_handler
    .weak flash_IRQ_handler
    .thumb_set flash_IRQ_handler, default_interrupt_handler
    .weak RCC_IRQ_handler
    .thumb_set RCC_IRQ_handler, default_interrupt_handler
    .weak EXTI0_IRQ_handler
    .thumb_set EXTI0_IRQ_handler, default_interrupt_handler
    .weak EXTI1_IRQ_handler
    .thumb_set EXTI1_IRQ_handler, default_interrupt_handler
    .weak EXTI2_IRQ_handler
    .thumb_set EXTI2_IRQ_handler, default_interrupt_handler
    .weak EXTI3_IRQ_handler
    .thumb_set EXTI3_IRQ_handler, default_interrupt_handler
    .weak EXTI4_IRQ_handler
    .thumb_set EXTI4_IRQ_handler, default_interrupt_handler
    .weak DMA_Stream0_IRQ_handler
    .thumb_set DMA_Stream0_IRQ_handler, default_interrupt_handler
    .weak DMA_Stream1_IRQ_handler
    .thumb_set DMA_Stream1_IRQ_handler, default_interrupt_handler
    .weak DMA_Stream2_IRQ_handler
    .thumb_set DMA_Stream2_IRQ_handler, default_interrupt_handler
    .weak DMA_Stream3_IRQ_handler
    .thumb_set DMA_Stream3_IRQ_handler, default_interrupt_handler
    .weak DMA_Stream4_IRQ_handler
    .thumb_set DMA_Stream4_IRQ_handler, default_interrupt_handler
    .weak DMA_Stream5_IRQ_handler
    .thumb_set DMA_Stream5_IRQ_handler, default_interrupt_handler
    .weak DMA_Stream6_IRQ_handler
    .thumb_set DMA_Stream6_IRQ_handler, default_interrupt_handler
    .weak ADC_IRQ_handler
    .thumb_set ADC_IRQ_handler, default_interrupt_handler
    .weak CAN1_TX_IRQ_handler
    .thumb_set CAN1_TX_IRQ_handler, default_interrupt_handler
    .weak CAN1_RX0_IRQ_handler
    .thumb_set CAN1_RX0_IRQ_handler, default_interrupt_handler
    .weak CAN1_RX1_IRQ_handler
    .thumb_set CAN1_RX1_IRQ_handler, default_interrupt_handler
    .weak CAN1_SCE_IRQ_handler
    .thumb_set CAN1_SCE_IRQ_handler, default_interrupt_handler
    .weak EXTI5_9_IRQ_handler
    .thumb_set EXTI5_9_IRQ_handler, default_interrupt_handler
    .weak TIM1_BRK_TIM9_IRQ_handler
    .thumb_set TIM1_BRK_TIM9_IRQ_handler, default_interrupt_handler
    .weak TIM1_UP_TIM10_IRQ_handler
    .thumb_set TIM1_UP_TIM10_IRQ_handler, default_interrupt_handler
    .weak TIM1_TRG_COM_TIM11_IRQ_handler
    .thumb_set TIM1_TRG_COM_TIM11_IRQ_handler, default_interrupt_handler
    .weak TIM1_CC_IRQ_handler
    .thumb_set TIM1_CC_IRQ_handler, default_interrupt_handler
    .weak TIM2_IRQ_handler
    .thumb_set TIM2_IRQ_handler, default_interrupt_handler
    .weak TIM3_IRQ_handler
    .thumb_set TIM3_IRQ_handler, default_interrupt_handler
    .weak TIM4_IRQ_handler
    .thumb_set TIM4_IRQ_handler, default_interrupt_handler
    .weak I2C1_EV_IRQ_handler
    .thumb_set I2C1_EV_IRQ_handler, default_interrupt_handler
    .weak I2C1_ER_IRQ_handler
    .thumb_set I2C1_ER_IRQ_handler, default_interrupt_handler
    .weak I2C2_EV_IRQ_handler
    .thumb_set I2C2_EV_IRQ_handler, default_interrupt_handler
    .weak I2C2_ER_IRQ_handler
    .thumb_set I2C2_ER_IRQ_handler, default_interrupt_handler
    .weak SPI1_IRQ_handler
    .thumb_set SPI1_IRQ_handler, default_interrupt_handler
    .weak SPI2_IRQ_handler
    .thumb_set SPI2_IRQ_handler, default_interrupt_handler
    .weak USART1_IRQ_handler
    .thumb_set USART1_IRQ_handler, default_interrupt_handler
    .weak USART2_IRQ_handler
    .thumb_set USART2_IRQ_handler, default_interrupt_handler
    .weak USART3_IRQ_handler
    .thumb_set USART3_IRQ_handler, default_interrupt_handler
    .weak EXTI10_15_IRQ_handler
    .thumb_set EXTI10_15_IRQ_handler, default_interrupt_handler
    .weak RTC_alarm_IRQ_handler
    .thumb_set RTC_alarm_IRQ_handler, default_interrupt_handler
    .weak OTG_WKUP_IRQ_handler
    .thumb_set OTG_WKUP_IRQ_handler, default_interrupt_handler
    .weak TIM8_BRK_TIM12_IRQ_handler
    .thumb_set TIM8_BRK_TIM12_IRQ_handler, default_interrupt_handler
    .weak TIM8_UP_TIM13_IRQ_handler
    .thumb_set TIM8_UP_TIM13_IRQ_handler, default_interrupt_handler
    .weak TIM8_TRG_COM_TIM14_IRQ_handler
    .thumb_set TIM8_TRG_COM_TIM14_IRQ_handler, default_interrupt_handler
    .weak TIM8_CC_IRQ_handler
    .thumb_set TIM8_CC_IRQ_handler, default_interrupt_handler
    .weak DMA1_Stream7_IRQ_handler
    .thumb_set DMA1_Stream7_IRQ_handler, default_interrupt_handler
    .weak FSMC_IRQ_handler
    .thumb_set FSMC_IRQ_handler, default_interrupt_handler
    .weak SDIO_IRQ_handler
    .thumb_set SDIO_IRQ_handler, default_interrupt_handler
    .weak TIM5_IRQ_handler
    .thumb_set TIM5_IRQ_handler, default_interrupt_handler
    .weak SPI3_IRQ_handler
    .thumb_set SPI3_IRQ_handler, default_interrupt_handler
    .weak UART4_IRQ_handler
    .thumb_set UART4_IRQ_handler, default_interrupt_handler
    .weak UART5_IRQ_handler
    .thumb_set UART5_IRQ_handler, default_interrupt_handler
    .weak TIM6_DAC_IRQ_handler
    .thumb_set TIM6_DAC_IRQ_handler, default_interrupt_handler
    .weak TIM7_IRQ_handler
    .thumb_set TIM7_IRQ_handler, default_interrupt_handler
    .weak DMA2_Stream0_IRQ_handler
    .thumb_set DMA2_Stream0_IRQ_handler, default_interrupt_handler
    .weak DMA2_Stream1_IRQ_handler
    .thumb_set DMA2_Stream1_IRQ_handler, default_interrupt_handler
    .weak DMA2_Stream2_IRQ_handler
    .thumb_set DMA2_Stream2_IRQ_handler, default_interrupt_handler
    .weak DMA2_Stream3_IRQ_handler
    .thumb_set DMA2_Stream3_IRQ_handler, default_interrupt_handler
    .weak DMA2_Stream4_IRQ_handler
    .thumb_set DMA2_Stream4_IRQ_handler, default_interrupt_handler
    .weak DFSDM1_FLT0_IRQ_handler
    .thumb_set DFSDM1_FLT0_IRQ_handler, default_interrupt_handler
    .weak DFSDM1_FLT1_IRQ_handler
    .thumb_set DFSDM1_FLT1_IRQ_handler, default_interrupt_handler
    .weak CAN2_TX_IRQ_handler
    .thumb_set CAN2_TX_IRQ_handler, default_interrupt_handler
    .weak CAN2_RX0_IRQ_handler
    .thumb_set CAN2_RX0_IRQ_handler, default_interrupt_handler
    .weak CAN2_RX1_IRQ_handler
    .thumb_set CAN2_RX1_IRQ_handler, default_interrupt_handler
    .weak CAN2_SCE_IRQ_handler
    .thumb_set CAN2_SCE_IRQ_handler, default_interrupt_handler
    .weak OTG_FS_IRQ_handler
    .thumb_set OTG_FS_IRQ_handler, default_interrupt_handler
    .weak DMA2_Stream5_IRQ_handler
    .thumb_set DMA2_Stream5_IRQ_handler, default_interrupt_handler
    .weak DMA2_Stream6_IRQ_handler
    .thumb_set DMA2_Stream6_IRQ_handler, default_interrupt_handler
    .weak DMA2_Stream7_IRQ_handler
    .thumb_set DMA2_Stream7_IRQ_handler, default_interrupt_handler
    .weak USART6_IRQ_handler
    .thumb_set USART6_IRQ_handler, default_interrupt_handler
    .weak I2C3_EV_IRQ_handler
    .thumb_set I2C3_EV_IRQ_handler, default_interrupt_handler
    .weak I2C3_ER_IRQ_handler
    .thumb_set I2C3_ER_IRQ_handler, default_interrupt_handler
    .weak CAN3_TX_IRQ_handler
    .thumb_set CAN3_TX_IRQ_handler, default_interrupt_handler
    .weak CAN3_RX0_IRQ_handler
    .thumb_set CAN3_RX0_IRQ_handler, default_interrupt_handler
    .weak CAN3_RX1_IRQ_handler
    .thumb_set CAN3_RX1_IRQ_handler, default_interrupt_handler
    .weak CAN3_SCE_IRQ_handler
    .thumb_set CAN3_SCE_IRQ_handler, default_interrupt_handler
    .weak RNG_IRQ_handler
    .thumb_set RNG_IRQ_handler, default_interrupt_handler
    .weak FPU_IRQ_handler
    .thumb_set FPU_IRQ_handler, default_interrupt_handler
    .weak UART7_IRQ_handler
    .thumb_set UART7_IRQ_handler, default_interrupt_handler
    .weak UART8_IRQ_handler
    .thumb_set UART8_IRQ_handler, default_interrupt_handler
    .weak SPI4_IRQ_handler
    .thumb_set SPI4_IRQ_handler, default_interrupt_handler
    .weak SPI5_IRQ_handler
    .thumb_set SPI5_IRQ_handler, default_interrupt_handler
    .weak SAI1_IRQ_handler
    .thumb_set SAI1_IRQ_handler, default_interrupt_handler
    .weak UART9_IRQ_handler
    .thumb_set UART9_IRQ_handler, default_interrupt_handler
    .weak UART10_IRQ_handler
    .thumb_set UART10_IRQ_handler, default_interrupt_handler
    .weak QSPI_IRQ_handler
    .thumb_set QSPI_IRQ_handler, default_interrupt_handler
    .weak FMPI2C1_EV_IRQ_handler
    .thumb_set FMPI2C1_EV_IRQ_handler, default_interrupt_handler
    .weak FMPI2C1_ER_IRQ_handler
    .thumb_set FMPI2C1_ER_IRQ_handler, default_interrupt_handler
    .weak LPTIM1_IRQ_handler
    .thumb_set LPTIM1_IRQ_handler, default_interrupt_handler
    .weak DFSDM2_FLT0_IRQ_handler
    .thumb_set DFSDM2_FLT0_IRQ_handler, default_interrupt_handler
    .weak DFSDM2_FLT1_IRQ_handler
    .thumb_set DFSDM2_FLT1_IRQ_handler, default_interrupt_handler
    .weak DFSDM2_FLT2_IRQ_handler
    .thumb_set DFSDM2_FLT2_IRQ_handler, default_interrupt_handler
    .weak DFSDM2_FLT3_IRQ_handler
    .thumb_set DFSDM2_FLT3_IRQ_handler, default_interrupt_handler
.size vtable, .-vtable

/*
 * A 'Default' interrupt handler. This is where interrupts
 * which are not otherwise configured will go.
 * It is an infinite loop, because...well, clearly we weren't
 * expecting the interrupt, so what can we do?
 */
.section .text.default_interrupt_handler,"ax",%progbits
default_interrupt_handler:
    default_interrupt_loop:
      B default_interrupt_loop
.size default_interrupt_handler, .-default_interrupt_handler
