

#ifndef __SD_RAM_H
#define __SD_RAM_H

typedef struct
{
    uint32_t LoadToActiveDelay;
    uint32_t ExitSelfRefreshDelay;
    uint32_t SelfRefreshTime;
    uint32_t RowCycleDelay;
    uint32_t WriteRecoveryTime;
    uint32_t RPDelay;
    uint32_t RCDDelay;
} FMC_SDRAM_TimingTypeDef;

void SDRAM_FMC_Init(void);
void SDRAM_test (void);


#define GPIO_AF12_FMC           ((uint8_t)0x0C)  /* FMC Alternate Function mapping     */
#define RCC_PERIPHCLK_FMC              (0x01000000U)
#define FMC_SDRAM_DEVICE               FMC_Bank5_6_R


#define FMC_D28_Pin GPIO_PIN_6
#define FMC_D28_GPIO_Port GPIOI
#define FMC_NBL3_Pin GPIO_PIN_5
#define FMC_NBL3_GPIO_Port GPIOI
#define FMC_NBL2_Pin GPIO_PIN_4
#define FMC_NBL2_GPIO_Port GPIOI
#define FMC_D25_Pin GPIO_PIN_1
#define FMC_D25_GPIO_Port GPIOI
#define FMC_D24_Pin GPIO_PIN_0
#define FMC_D24_GPIO_Port GPIOI
#define FMC_D29_Pin GPIO_PIN_7
#define FMC_D29_GPIO_Port GPIOI
#define FMC_NBL1_Pin GPIO_PIN_1
#define FMC_NBL1_GPIO_Port GPIOE
#define FMC_D26_Pin GPIO_PIN_2
#define FMC_D26_GPIO_Port GPIOI
#define FMC_D23_Pin GPIO_PIN_15
#define FMC_D23_GPIO_Port GPIOH
#define FMC_D22_Pin GPIO_PIN_14
#define FMC_D22_GPIO_Port GPIOH
#define FMC_NBL0_Pin GPIO_PIN_0
#define FMC_NBL0_GPIO_Port GPIOE
#define FMC_D27_Pin GPIO_PIN_3
#define FMC_D27_GPIO_Port GPIOI
#define FMC_SDCAS_Pin GPIO_PIN_15
#define FMC_SDCAS_GPIO_Port GPIOG
#define FMC_D2_Pin GPIO_PIN_0
#define FMC_D2_GPIO_Port GPIOD
#define FMC_D21_Pin GPIO_PIN_13
#define FMC_D21_GPIO_Port GPIOH
#define FMC_D30_Pin GPIO_PIN_9
#define FMC_D30_GPIO_Port GPIOI
#define FMC_D3_Pin GPIO_PIN_1
#define FMC_D3_GPIO_Port GPIOD
#define FMC_D31_Pin GPIO_PIN_10
#define FMC_D31_GPIO_Port GPIOI
#define FMC_SDCLK_Pin GPIO_PIN_8
#define FMC_SDCLK_GPIO_Port GPIOG
#define FMC_A2_Pin GPIO_PIN_2
#define FMC_A2_GPIO_Port GPIOF
#define FMC_A1_Pin GPIO_PIN_1
#define FMC_A1_GPIO_Port GPIOF
#define FMC_A0_Pin GPIO_PIN_0
#define FMC_A0_GPIO_Port GPIOF
#define FMC_A3_Pin GPIO_PIN_3
#define FMC_A3_GPIO_Port GPIOF
#define FMC_BA0_Pin GPIO_PIN_4
#define FMC_BA0_GPIO_Port GPIOG
#define FMC_A12_Pin GPIO_PIN_2
#define FMC_A12_GPIO_Port GPIOG
#define FMC_A5_Pin GPIO_PIN_5
#define FMC_A5_GPIO_Port GPIOF
#define FMC_A4_Pin GPIO_PIN_4
#define FMC_A4_GPIO_Port GPIOF
#define FMC_D7_Pin GPIO_PIN_10
#define FMC_D7_GPIO_Port GPIOE
#define FMC_SDNWE_Pin GPIO_PIN_5
#define FMC_SDNWE_GPIO_Port GPIOH
#define FMC_A7_Pin GPIO_PIN_13
#define FMC_A7_GPIO_Port GPIOF
#define FMC_A8_Pin GPIO_PIN_14
#define FMC_A8_GPIO_Port GPIOF
#define FMC_D6_Pin GPIO_PIN_9
#define FMC_D6_GPIO_Port GPIOE
#define FMC_D8_Pin GPIO_PIN_11
#define FMC_D8_GPIO_Port GPIOE
#define FMC_D18_Pin GPIO_PIN_10
#define FMC_D18_GPIO_Port GPIOH
#define FMC_D19_Pin GPIO_PIN_11
#define FMC_D19_GPIO_Port GPIOH
#define FMC_D1_Pin GPIO_PIN_15
#define FMC_D1_GPIO_Port GPIOD
#define FMC_D0_Pin GPIO_PIN_14
#define FMC_D0_GPIO_Port GPIOD
#define FMC_A6_Pin GPIO_PIN_12
#define FMC_A6_GPIO_Port GPIOF
#define FMC_A9_Pin GPIO_PIN_15
#define FMC_A9_GPIO_Port GPIOF
#define FMC_D9_Pin GPIO_PIN_12
#define FMC_D9_GPIO_Port GPIOE
#define FMC_D12_Pin GPIO_PIN_15
#define FMC_D12_GPIO_Port GPIOE
#define FMC_D17_Pin GPIO_PIN_9
#define FMC_D17_GPIO_Port GPIOH
#define FMC_D20_Pin GPIO_PIN_12
#define FMC_D20_GPIO_Port GPIOH
#define FMC_SDRAS_Pin GPIO_PIN_11
#define FMC_SDRAS_GPIO_Port GPIOF
#define FMC_A10_Pin GPIO_PIN_0
#define FMC_A10_GPIO_Port GPIOG
#define FMC_D5_Pin GPIO_PIN_8
#define FMC_D5_GPIO_Port GPIOE
#define FMC_D10_Pin GPIO_PIN_13
#define FMC_D10_GPIO_Port GPIOE
#define FMC_SDNE1_Pin GPIO_PIN_6
#define FMC_SDNE1_GPIO_Port GPIOH
#define FMC_D16_Pin GPIO_PIN_8
#define FMC_D16_GPIO_Port GPIOH
#define FMC_D15_Pin GPIO_PIN_10
#define FMC_D15_GPIO_Port GPIOD
#define FMC_D14_Pin GPIO_PIN_9
#define FMC_D14_GPIO_Port GPIOD
#define FMC_A11_Pin GPIO_PIN_1
#define FMC_A11_GPIO_Port GPIOG
#define FMC_D4_Pin GPIO_PIN_7
#define FMC_D4_GPIO_Port GPIOE
#define FMC_D11_Pin GPIO_PIN_14
#define FMC_D11_GPIO_Port GPIOE
#define FMC_SDCKE1_Pin GPIO_PIN_7
#define FMC_SDCKE1_GPIO_Port GPIOH
#define FMC_D13_Pin GPIO_PIN_8
#define FMC_D13_GPIO_Port GPIOD

#define FMC_SDRAM_BANK1                         (0x00000000U)
#define FMC_SDRAM_BANK2                         (0x00000001U)
#define FMC_SDRAM_COLUMN_BITS_NUM_9             (0x00000001U)
#define FMC_SDRAM_ROW_BITS_NUM_12               (0x00000004U)
#define FMC_SDRAM_MEM_BUS_WIDTH_32              (0x00000020U)
#define FMC_SDRAM_INTERN_BANKS_NUM_4            (0x00000040U)
#define FMC_SDRAM_CAS_LATENCY_3                 (0x00000180U)
#define FMC_SDRAM_WRITE_PROTECTION_DISABLE      (0x00000000U)
#define FMC_SDRAM_CLOCK_PERIOD_2                (0x00000800U)
#define FMC_SDRAM_RBURST_ENABLE                 (0x00001000U)
#define FMC_SDRAM_RPIPE_DELAY_0                 (0x00000000U)
#define FMC_SDRAM_CMD_TARGET_BANK2              FMC_SDCMR_CTB2
#define REFRESH_COUNT                    ((uint32_t)0x0603)   /* SDRAM refresh counter (100Mhz SD clock) */


/* SDCR register clear mask */
#define SDCR_CLEAR_MASK   ((uint32_t)(FMC_SDCRx_NC    | FMC_SDCRx_NR     | \
                                    FMC_SDCRx_MWID  | FMC_SDCRx_NB     | \
                                    FMC_SDCRx_CAS   | FMC_SDCRx_WP     | \
                                    FMC_SDCRx_SDCLK | FMC_SDCRx_RBURST | \
                                    FMC_SDCRx_RPIPE))

#define SDTR_CLEAR_MASK   ((uint32_t)(FMC_SDTRx_TMRD  | FMC_SDTRx_TXSR   | \
                                      FMC_SDTRx_TRAS  | FMC_SDTRx_TRC    | \
                                      FMC_SDTRx_TWR   | FMC_SDTRx_TRP    | \
                                      FMC_SDTRx_TRCD))


#endif /*__SD_RAM_H*/
