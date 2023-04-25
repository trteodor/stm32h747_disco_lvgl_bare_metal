#include "GPIO_h7.h"
#include "System.h"
#include "dsihost.h"
#include "DLTuc.h"

#define DSI_TE_Pin GPIO_PIN_2
#define DSI_TE_GPIO_Port GPIOJ

static void DSI_ConfigVideoMode(void);
static void DSIHOST_MspInit(void);
static void DSI_Host_PeriphInit(void);


void DSIHOST_DSI_Init(void)
{
    DSIHOST_MspInit();
    DSI_Host_PeriphInit();
    DSI_ConfigVideoMode();


}

static void DSI_Host_PeriphInit(void)
{
    SET_BIT(DSI->WRPCR, DSI_WRPCR_REGEN); /*!< Regulator Enable */
    while ( (DSI->WISR & DSI_WISR_RRS) != DSI_WISR_RRS) {}

    #define HSE_VALUE    (25000000UL) /*TODO: take a look on it!!!*/
    /* Set the PLL division factors */
    DSI->WRPCR &= ~(DSI_WRPCR_PLL_NDIV | DSI_WRPCR_PLL_IDF | DSI_WRPCR_PLL_ODF);
    DSI->WRPCR |= (((__PLLNDIV) << DSI_WRPCR_PLL_NDIV_Pos) | \
                        ((DSI_PLL_IN_DIV1) << DSI_WRPCR_PLL_IDF_Pos) | \
                        ((DSI_PLL_OUT_DIV1) << DSI_WRPCR_PLL_ODF_Pos));

    /* Enable the DSI PLL */
    SET_BIT(DSI->WRPCR, DSI_WRPCR_PLLEN);

    /* Requires min of 400us delay before reading the PLLLS flag */
    /* 1ms delay is inserted that is the minimum HAL delay granularity */
    DelayMs(1);
    while ( (DSI->WISR & DSI_WISR_PLLLS) != DSI_WISR_PLLLS){}

    /* D-PHY clock and digital enable*/
    DSI->PCTLR |= (DSI_PCTLR_CKE | DSI_PCTLR_DEN);

    /* Clock lane configuration */
    DSI->CLCR &= ~(DSI_CLCR_DPCC | DSI_CLCR_ACR);
    DSI->CLCR |= (DSI_CLCR_DPCC | DSI_AUTO_CLK_LANE_CTRL_DISABLE);

    /* Configure the number of active data lanes */
    DSI->PCONFR &= ~DSI_PCONFR_NL;
    DSI->PCONFR |= DSI_TWO_DATA_LANES;

    /************************ Set the DSI clock parameters ************************/

    /* Set the TX escape clock division factor */
    DSI->CCR &= ~DSI_CCR_TXECKDIV;
    DSI->CCR |= 4;

    /* Calculate the bit period in high-speed mode in unit of 0.25 ns (UIX4) */
    /* The equation is : UIX4 = IntegerPart( (1000/F_PHY_Mhz) * 4 )          */
    /* Where : F_PHY_Mhz = (NDIV * HSE_Mhz) / (IDF * ODF)                    */
    uint32_t tempIDF;
    uint32_t unitIntervalx4;

    tempIDF = (DSI_PLL_IN_DIV1 > 0U) ? DSI_PLL_IN_DIV1 : 1U;
    unitIntervalx4 = (4000000U * tempIDF * ((1UL << (0x3U & DSI_PLL_OUT_DIV1)))) / ((HSE_VALUE / 1000U) * __PLLNDIV);

    /* Set the bit period in high-speed mode */
    DSI->WPCR[0U] &= ~DSI_WPCR0_UIX4;
    DSI->WPCR[0U] |= unitIntervalx4;


    /* Disable all error interrupts and reset the Error Mask */
    DSI->IER[0U] = 0U;
    DSI->IER[1U] = 0U;



    /* Set the timeout clock division factor */
    DSI->CCR &= ~DSI_CCR_TOCKDIV;
    DSI->CCR |= ((1) << 8U);

    /* High-speed transmission timeout */
    DSI->TCCR[0U] &= ~DSI_TCCR0_HSTX_TOCNT;
    DSI->TCCR[0U] |= ((10000) << 16U);

    /* Low-power reception timeout */
    DSI->TCCR[0U] &= ~DSI_TCCR0_LPRX_TOCNT;
    DSI->TCCR[0U] |= 5000;

    /* High-speed read timeout */
    DSI->TCCR[1U] &= ~DSI_TCCR1_HSRD_TOCNT;
    DSI->TCCR[1U] |= 0;

    /* Low-power read timeout */
    DSI->TCCR[2U] &= ~DSI_TCCR2_LPRD_TOCNT;
    DSI->TCCR[2U] |= 0;

    /* High-speed write timeout */
    DSI->TCCR[3U] &= ~DSI_TCCR3_HSWR_TOCNT;
    DSI->TCCR[3U] |= 0;

    /* High-speed write presp mode */
    DSI->TCCR[3U] &= ~DSI_TCCR3_PM;
    #define DSI_HS_PM_DISABLE        0x00000000U
    DSI->TCCR[3U] |= DSI_HS_PM_DISABLE;

    /* Low-speed write timeout */
    DSI->TCCR[4U] &= ~DSI_TCCR4_LPWR_TOCNT;
    DSI->TCCR[4U] |= 0;

    /* BTA timeout */
    DSI->TCCR[5U] &= ~DSI_TCCR5_BTA_TOCNT;
    DSI->TCCR[5U] |= 0;

    #define __DataLaneMaxReadTime 0
    #define __ClockLaneLP2HSTime 33
    #define __ClockLaneHS2LPTime 28
    #define __StopWaitTime 0

    uint32_t maxTime = (__ClockLaneLP2HSTime > __ClockLaneHS2LPTime) ? __ClockLaneLP2HSTime : __ClockLaneHS2LPTime;

    /* Clock lane timer configuration */

    /* In Automatic Clock Lane control mode, the DSI Host can turn off the clock lane between two
        High-Speed transmission.
        To do so, the DSI Host calculates the time required for the clock lane to change from HighSpeed
        to Low-Power and from Low-Power to High-Speed.
        This timings are configured by the HS2LP_TIME and LP2HS_TIME in the DSI Host Clock Lane Timer Configuration
        Register (DSI_CLTCR).
        But the DSI Host is not calculating LP2HS_TIME + HS2LP_TIME but 2 x HS2LP_TIME.

        Workaround : Configure HS2LP_TIME and LP2HS_TIME with the same value being the max of HS2LP_TIME or LP2HS_TIME.
    */
    DSI->CLTCR &= ~(DSI_CLTCR_LP2HS_TIME | DSI_CLTCR_HS2LP_TIME);
    DSI->CLTCR |= (maxTime | ((maxTime) << 16U));

    /* Data lane timer configuration */

    DSI->DLTCR &= ~(DSI_DLTCR_MRD_TIME | DSI_DLTCR_LP2HS_TIME | DSI_DLTCR_HS2LP_TIME);
    DSI->DLTCR |= (__DataLaneMaxReadTime | ((__ClockLaneLP2HSTime) << 16U) | ((
                                __ClockLaneHS2LPTime) << 24U));

    /* Configure the wait period to request HS transmission after a stop state */
    DSI->PCONFR &= ~DSI_PCONFR_SW_TIME;

    DSI->PCONFR |= ((__StopWaitTime) << 8U);

    /* Set the DSI Host Protocol Configuration Register */
    DSI->PCR &= ~DSI_FLOW_CONTROL_ALL;
    DSI->PCR |= DSI_PCR_BTAE;


    /* High-Speed Transmission Slew Rate Control on Clock Lane */
    DSI->WPCR[1U] &= ~DSI_WPCR1_HSTXSRCCL;
    DSI->WPCR[1U] |= 4 << DSI_WPCR1_HSTXSRCCL_Pos;

    /* High-Speed Transmission Delay on Clock Lane */
    DSI->WPCR[1U] &= ~DSI_WPCR1_HSTXDCL;
    DSI->WPCR[1U] |= 4;


    /* High-Speed Transmission Slew Rate Control on Data Lanes */
    DSI->WPCR[1U] &= ~DSI_WPCR1_HSTXSRCDL;
    DSI->WPCR[1U] |= 4 << DSI_WPCR1_HSTXSRCDL_Pos;

    /* High-Speed Transmission Delay on Data Lanes */
    DSI->WPCR[1U] &= ~DSI_WPCR1_HSTXDDL;
    DSI->WPCR[1U] |= 4 << DSI_WPCR1_HSTXDDL_Pos;


    /* Low-Power RX low-pass Filtering Tuning */
    DSI->WPCR[1U] &= ~DSI_WPCR1_LPRXFT;
    DSI->WPCR[1U] |= 0 << DSI_WPCR1_LPRXFT_Pos;


    DSI->IER[0U] = 0U;
    DSI->IER[1U] = 0U;

    DSI->IER[1U] |= (DSI_ISR1_TOHSTX | DSI_ISR1_TOLPRX);

    /* Activate/Disactivate additional current path on all lanes */
    DSI->WPCR[1U] &= ~DSI_WPCR1_SDDC;
    DSI->WPCR[1U] |= ((uint32_t)1 << DSI_WPCR1_SDDC_Pos);
}



static void DSI_ConfigVideoMode(void)
{
    DSI_VidCfgTypeDef VidCfg = {0};
    VidCfg.VirtualChannelID = 0;
    VidCfg.ColorCoding = DSI_RGB888;
    VidCfg.LooselyPacked = DSI_LOOSELY_PACKED_DISABLE;
    VidCfg.Mode = DSI_VID_MODE_BURST;
    VidCfg.PacketSize = 800;
    VidCfg.NumberOfChunks = 0;
    VidCfg.NullPacketSize = 0;
    VidCfg.HSPolarity = DSI_HSYNC_ACTIVE_LOW;
    VidCfg.VSPolarity = DSI_VSYNC_ACTIVE_LOW;
    VidCfg.DEPolarity = DSI_DATA_ENABLE_ACTIVE_HIGH;
    VidCfg.HorizontalSyncActive = 4;
    VidCfg.HorizontalBackPorch = 77;
    VidCfg.HorizontalLine = 1982;
    VidCfg.VerticalSyncActive = 1;
    VidCfg.VerticalBackPorch = 15;
    VidCfg.VerticalFrontPorch = 16;
    VidCfg.VerticalActive = 480;
    VidCfg.LPCommandEnable = DSI_LP_COMMAND_ENABLE;
    VidCfg.LPLargestPacketSize = 0;
    VidCfg.LPVACTLargestPacketSize = 0;
    VidCfg.LPHorizontalFrontPorchEnable = DSI_LP_HFP_ENABLE;
    VidCfg.LPHorizontalBackPorchEnable = DSI_LP_HBP_ENABLE;
    VidCfg.LPVerticalActiveEnable = DSI_LP_VACT_ENABLE;
    VidCfg.LPVerticalFrontPorchEnable = DSI_LP_VFP_ENABLE;
    VidCfg.LPVerticalBackPorchEnable = DSI_LP_VBP_ENABLE;
    VidCfg.LPVerticalSyncActiveEnable = DSI_LP_VSYNC_ENABLE;
    VidCfg.FrameBTAAcknowledgeEnable = DSI_FBTAA_DISABLE;
    /* Select video mode by resetting CMDM and DSIM bits */
    DSI->MCR &= ~DSI_MCR_CMDM;
    DSI->WCFGR &= ~DSI_WCFGR_DSIM;

    /* Configure the video mode transmission type */
    DSI->VMCR &= ~DSI_VMCR_VMT;
    DSI->VMCR |= VidCfg.Mode;

    /* Configure the video packet size */
    DSI->VPCR &= ~DSI_VPCR_VPSIZE;
    DSI->VPCR |= VidCfg.PacketSize;

    /* Set the chunks number to be transmitted through the DSI link */
    DSI->VCCR &= ~DSI_VCCR_NUMC;
    DSI->VCCR |= VidCfg.NumberOfChunks;

    /* Set the size of the null packet */
    DSI->VNPCR &= ~DSI_VNPCR_NPSIZE;
    DSI->VNPCR |= VidCfg.NullPacketSize;

    /* Select the virtual channel for the LTDC interface traffic */
    DSI->LVCIDR &= ~DSI_LVCIDR_VCID;
    DSI->LVCIDR |= VidCfg.VirtualChannelID;

    /* Configure the polarity of control signals */
    DSI->LPCR &= ~(DSI_LPCR_DEP | DSI_LPCR_VSP | DSI_LPCR_HSP);
    DSI->LPCR |= (VidCfg.DEPolarity | VidCfg.VSPolarity | VidCfg.HSPolarity);

    /* Select the color coding for the host */
    DSI->LCOLCR &= ~DSI_LCOLCR_COLC;
    DSI->LCOLCR |= VidCfg.ColorCoding;

    /* Select the color coding for the wrapper */
    DSI->WCFGR &= ~DSI_WCFGR_COLMUX;
    DSI->WCFGR |= ((VidCfg.ColorCoding) << 1U);

    /* Set the Horizontal Synchronization Active (HSA) in lane byte clock cycles */
    DSI->VHSACR &= ~DSI_VHSACR_HSA;
    DSI->VHSACR |= VidCfg.HorizontalSyncActive;

    /* Set the Horizontal Back Porch (HBP) in lane byte clock cycles */
    DSI->VHBPCR &= ~DSI_VHBPCR_HBP;
    DSI->VHBPCR |= VidCfg.HorizontalBackPorch;

    /* Set the total line time (HLINE=HSA+HBP+HACT+HFP) in lane byte clock cycles */
    DSI->VLCR &= ~DSI_VLCR_HLINE;
    DSI->VLCR |= VidCfg.HorizontalLine;

    /* Set the Vertical Synchronization Active (VSA) */
    DSI->VVSACR &= ~DSI_VVSACR_VSA;
    DSI->VVSACR |= VidCfg.VerticalSyncActive;

    /* Set the Vertical Back Porch (VBP)*/
    DSI->VVBPCR &= ~DSI_VVBPCR_VBP;
    DSI->VVBPCR |= VidCfg.VerticalBackPorch;

    /* Set the Vertical Front Porch (VFP)*/
    DSI->VVFPCR &= ~DSI_VVFPCR_VFP;
    DSI->VVFPCR |= VidCfg.VerticalFrontPorch;

    /* Set the Vertical Active period*/
    DSI->VVACR &= ~DSI_VVACR_VA;
    DSI->VVACR |= VidCfg.VerticalActive;

    /* Configure the command transmission mode */
    DSI->VMCR &= ~DSI_VMCR_LPCE;
    DSI->VMCR |= VidCfg.LPCommandEnable;

    /* Low power largest packet size */
    DSI->LPMCR &= ~DSI_LPMCR_LPSIZE;
    DSI->LPMCR |= ((VidCfg.LPLargestPacketSize) << 16U);

    /* Low power VACT largest packet size */
    DSI->LPMCR &= ~DSI_LPMCR_VLPSIZE;
    DSI->LPMCR |= VidCfg.LPVACTLargestPacketSize;

    /* Enable LP transition in HFP period */
    DSI->VMCR &= ~DSI_VMCR_LPHFPE;
    DSI->VMCR |= VidCfg.LPHorizontalFrontPorchEnable;

    /* Enable LP transition in HBP period */
    DSI->VMCR &= ~DSI_VMCR_LPHBPE;
    DSI->VMCR |= VidCfg.LPHorizontalBackPorchEnable;

    /* Enable LP transition in VACT period */
    DSI->VMCR &= ~DSI_VMCR_LPVAE;
    DSI->VMCR |= VidCfg.LPVerticalActiveEnable;

    /* Enable LP transition in VFP period */
    DSI->VMCR &= ~DSI_VMCR_LPVFPE;
    DSI->VMCR |= VidCfg.LPVerticalFrontPorchEnable;

    /* Enable LP transition in VBP period */
    DSI->VMCR &= ~DSI_VMCR_LPVBPE;
    DSI->VMCR |= VidCfg.LPVerticalBackPorchEnable;

    /* Enable LP transition in vertical sync period */
    DSI->VMCR &= ~DSI_VMCR_LPVSAE;
    DSI->VMCR |= VidCfg.LPVerticalSyncActiveEnable;

    /* Enable the request for an acknowledge response at the end of a frame */
    DSI->VMCR &= ~DSI_VMCR_FBTAAE;
    DSI->VMCR |= VidCfg.FrameBTAAcknowledgeEnable;




    /*Set generic VCID*/
    DSI->GVCIDR &= ~DSI_GVCIDR_VCID;
    DSI->GVCIDR |= 0;
}


static void DSIHOST_MspInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
   /*TODO: HAL_RCCEx_PeriphCLKConfig ?????*/

    /* DSI clock enable */
    SET_BIT(RCC->APB3ENR, RCC_APB3ENR_DSIEN);

    SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOJEN);
    /**DSIHOST GPIO Configuration
    DSI_D1P     ------> DSIHOST_D1P
    DSI_D1N     ------> DSIHOST_D1N
    DSI_CKP     ------> DSIHOST_CKP
    DSI_CKN     ------> DSIHOST_CKN
    DSI_D0P     ------> DSIHOST_D0P
    DSI_D0N     ------> DSIHOST_D0N
    PJ2     ------> DSIHOST_TE
    */
    GPIO_InitStruct.Pin = DSI_TE_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_DSI;
    GPIO_Init(DSI_TE_GPIO_Port, &GPIO_InitStruct);

    /* DSI interrupt Init */
    NVIC_SetPriority (FMC_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
    NVIC_EnableIRQ(DSI_IRQn);
}


void DSI_ConfigFlowControl(uint32_t FlowControl)
{
  DSI->PCR &= ~DSI_FLOW_CONTROL_ALL;
  DSI->PCR |= FlowControl;
}

void DSI_IRQHandler(void)
{
    LOG("DSI_IRQHandler");
}