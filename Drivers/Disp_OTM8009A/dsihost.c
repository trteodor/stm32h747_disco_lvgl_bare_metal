#include "GPIO_h7.h"
#include "System.h"
#include "dsihost.h"
#include "DLTuc.h"



#define DSI_TE_Pin GPIO_PIN_2
#define DSI_TE_GPIO_Port GPIOJ

static void DSI_ConfigVideoMode(void);
static void DSIHOST_MspInit(void);
// static void DSI_Host_PeriphInit(void);
// void DSI_ConfigAdaptedCommandMode(void);

/**************************************************************************************/

void DSIHOST_DSI_Init(void)
{
    DSIHOST_MspInit();
    DSI_Host_PeriphInit();

  DSI_LPCmdTypeDef LLLPCmd;
  LLLPCmd.LPGenShortWriteNoP    = DSI_LP_GSW0P_ENABLE;
  LLLPCmd.LPGenShortWriteOneP   = DSI_LP_GSW1P_ENABLE;
  LLLPCmd.LPGenShortWriteTwoP   = DSI_LP_GSW2P_ENABLE;
  LLLPCmd.LPGenShortReadNoP     = DSI_LP_GSR0P_ENABLE;
  LLLPCmd.LPGenShortReadOneP    = DSI_LP_GSR1P_ENABLE;
  LLLPCmd.LPGenShortReadTwoP    = DSI_LP_GSR2P_ENABLE;
  LLLPCmd.LPGenLongWrite        = DSI_LP_GLW_ENABLE;
  LLLPCmd.LPDcsShortWriteNoP    = DSI_LP_DSW0P_ENABLE;
  LLLPCmd.LPDcsShortWriteOneP   = DSI_LP_DSW1P_ENABLE;
  LLLPCmd.LPDcsShortReadNoP     = DSI_LP_DSR0P_ENABLE;
  LLLPCmd.LPDcsLongWrite        = DSI_LP_DLW_ENABLE;
    DSI_ConfigAdaptedCommandMode(&LLLPCmd);
    // DSI_ConfigVideoMode();
}
/**************************************************************************************/
void DSI_Host_PeriphInit(void)
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



  /*************************** Set the PHY parameters ***************************/
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
    /*TODO:*/
    DSI->WPCR[1U] |= ((uint32_t)0 << DSI_WPCR1_SDDC_Pos);
}



void DSI_ConfigAdaptedCommandMode(DSI_LPCmdTypeDef *LPCmd)
{
  DSI_CmdCfgTypeDef CmdCfg;

  /* Configure the DSI for Command mode */
  CmdCfg.VirtualChannelID      = 0;
  CmdCfg.HSPolarity            = DSI_HSYNC_ACTIVE_HIGH;
  CmdCfg.VSPolarity            = DSI_VSYNC_ACTIVE_HIGH;
  CmdCfg.DEPolarity            = DSI_DATA_ENABLE_ACTIVE_HIGH;
  CmdCfg.ColorCoding           = DSI_RGB888;
  CmdCfg.CommandSize           = DSI_HACT;
  CmdCfg.TearingEffectSource   = DSI_TE_EXTERNAL;
  CmdCfg.TearingEffectPolarity = DSI_TE_RISING_EDGE;
  CmdCfg.VSyncPol              = DSI_VSYNC_FALLING;
  CmdCfg.AutomaticRefresh      = DSI_AR_DISABLE;
  CmdCfg.TEAcknowledgeRequest  = DSI_TE_ACKNOWLEDGE_DISABLE;

/************************************************************************************/
  //ConfigAdaptedCommandMode
/************************************************************************************/
/* Select command mode by setting CMDM and DSIM bits */
  DSI->MCR |= DSI_MCR_CMDM;
  DSI->WCFGR &= ~DSI_WCFGR_DSIM;
  DSI->WCFGR |= DSI_WCFGR_DSIM;

  /* Select the virtual channel for the LTDC interface traffic */
  DSI->LVCIDR &= ~DSI_LVCIDR_VCID;
  DSI->LVCIDR |= CmdCfg.VirtualChannelID;

  /* Configure the polarity of control signals */
  DSI->LPCR &= ~(DSI_LPCR_DEP | DSI_LPCR_VSP | DSI_LPCR_HSP);
  DSI->LPCR |= (CmdCfg.DEPolarity | CmdCfg.VSPolarity | CmdCfg.HSPolarity);

  /* Select the color coding for the host */
  DSI->LCOLCR &= ~DSI_LCOLCR_COLC;
  DSI->LCOLCR |= CmdCfg.ColorCoding;

  /* Select the color coding for the wrapper */
  DSI->WCFGR &= ~DSI_WCFGR_COLMUX;
  DSI->WCFGR |= ((CmdCfg.ColorCoding) << 1U);

  /* Configure the maximum allowed size for write memory command */
  DSI->LCCR &= ~DSI_LCCR_CMDSIZE;
  DSI->LCCR |= CmdCfg.CommandSize;

  /* Configure the tearing effect source and polarity and select the refresh mode */
  DSI->WCFGR &= ~(DSI_WCFGR_TESRC | DSI_WCFGR_TEPOL | DSI_WCFGR_AR | DSI_WCFGR_VSPOL);
  DSI->WCFGR |= (CmdCfg.TearingEffectSource | CmdCfg.TearingEffectPolarity | CmdCfg.AutomaticRefresh |
                            CmdCfg.VSyncPol);

  /* Configure the tearing effect acknowledge request */
  DSI->CMCR &= ~DSI_CMCR_TEARE;
  DSI->CMCR |= CmdCfg.TEAcknowledgeRequest;

  /* Enable the Tearing Effect interrupt */
   /* Enable the End of Refresh interrupt */
  DSI->WIER |= (DSI_WIER_TEIE | DSI_WIER_ERIE);


/************************************************************************************/
/************************************************************************************/
  // ConfigCommand
/************************************************************************************/
  /* Select High-speed or Low-power for command transmission */
  DSI->CMCR &=            ~(DSI_CMCR_GSW0TX | \
                            DSI_CMCR_GSW1TX | \
                            DSI_CMCR_GSW2TX | \
                            DSI_CMCR_GSR0TX | \
                            DSI_CMCR_GSR1TX | \
                            DSI_CMCR_GSR2TX | \
                            DSI_CMCR_GLWTX  | \
                            DSI_CMCR_DSW0TX | \
                            DSI_CMCR_DSW1TX | \
                            DSI_CMCR_DSR0TX | \
                            DSI_CMCR_DLWTX  | \
                            DSI_CMCR_MRDPS);

  DSI->CMCR |= (LPCmd->LPGenShortWriteNoP  | \
                           LPCmd->LPGenShortWriteOneP | \
                           LPCmd->LPGenShortWriteTwoP | \
                           LPCmd->LPGenShortReadNoP   | \
                           LPCmd->LPGenShortReadOneP  | \
                           LPCmd->LPGenShortReadTwoP  | \
                           LPCmd->LPGenLongWrite      | \
                           LPCmd->LPDcsShortWriteNoP  | \
                           LPCmd->LPDcsShortWriteOneP | \
                           LPCmd->LPDcsShortReadNoP   | \
                           LPCmd->LPDcsLongWrite      | \
                           LPCmd->LPMaxReadPacket);

  /* Configure the acknowledge request after each packet transmission */
  DSI->CMCR &= ~DSI_CMCR_ARE;
  DSI->CMCR |= LPCmd->AcknowledgeRequest;

  DSI_ConfigFlowControl(DSI_FLOW_CONTROL_BTA);

  /*DSI_ForceRXLowPower*/
  DSI->WPCR[1U] &= ~DSI_WPCR1_FLPRXLPM;
  DSI->WPCR[1U] |= ((uint32_t)1 << 22U);

}

/**************************************************************************************/
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

/**************************************************************************************/
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
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF13_DSI;
    GPIO_Init(DSI_TE_GPIO_Port, &GPIO_InitStruct);

    /* DSI interrupt Init */
    NVIC_SetPriority (DSI_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
    NVIC_EnableIRQ(DSI_IRQn);
}

/**************************************************************************************/
void DSI_ConfigFlowControl(uint32_t FlowControl)
{
  DSI->PCR &= ~DSI_FLOW_CONTROL_ALL;
  DSI->PCR |= FlowControl;
}
/**************************************************************************************/
void (*TearingEffectCbPtr)(void) = NULL;
void (*EndOfRefreshCbPtr)(void) = NULL;

void DSI_RegisterIrqCallBacks(void(*TearingEffectCb)(void), void (*EndOfRefreshCb)(void))
{
TearingEffectCbPtr = TearingEffectCb;
EndOfRefreshCbPtr = EndOfRefreshCb;
}
/******************************************************************************************/
void DSI_IRQHandler(void)
{
  uint32_t ErrorStatus0;
  uint32_t ErrorStatus1;
  /* Tearing Effect Interrupt management ***************************************/
  if ( (DSI->WISR & DSI_WISR_TEIF) != 0U)
  {
      if ( (DSI->WIER & DSI_WIER_TEIE) != 0)
      {
        /* Clear the Tearing Effect Interrupt Flag */
        DSI->WIFCR = DSI_WISR_TEIF;
        /*Call legacy Tearing Effect callback*/
        if(TearingEffectCbPtr != NULL)
        {
          TearingEffectCbPtr();
        }
      }
  }

  /* End of Refresh Interrupt management ***************************************/
    if ( (DSI->WISR & DSI_WISR_ERIF) != 0U)
    {
      if ( (DSI->WIER & DSI_WIER_ERIE) != 0)
      {
        /* Clear the End of Refresh Interrupt Flag */
        DSI->WIFCR = DSI_WISR_ERIF;
        /*Call Legacy End of refresh callback */
        if(EndOfRefreshCbPtr != NULL)
        {
          EndOfRefreshCbPtr();
        }
      }
    }

}


/**************************************************************************************/
static void DSI_ConfigPacketHeader(
                                   uint32_t ChannelID,
                                   uint32_t DataType,
                                   uint32_t Data0,
                                   uint32_t Data1)
{
  /* Update the DSI packet header with new information */
  DSI->GHCR = (DataType | (ChannelID << 6U) | (Data0 << 8U) | (Data1 << 16U));
}

/**************************************************************************************/
void DSI_ShortWrite(
                        uint32_t ChannelID,
                        uint32_t Mode,
                        uint32_t Param1,
                        uint32_t Param2)
{


  /* Wait for Command FIFO Empty */
  while ((DSI->GPSR & DSI_GPSR_CMDFE) == 0U){}
  /* Configure the packet to send a short DCS command with 0 or 1 parameter */
  /* Update the DSI packet header with new information */
  DSI->GHCR = (Mode | (ChannelID << 6U) | (Param1 << 8U) | (Param2 << 16U));
}
/**************************************************************************************/
void DSI_LongWrite(
                        uint32_t ChannelID,
                        uint32_t Mode,
                        uint32_t NbParams,
                        uint32_t Param1,
                        uint8_t *ParametersTable)
{
  uint32_t uicounter;
  uint32_t nbBytes;
  uint32_t count;
  uint32_t fifoword;
  uint8_t *pparams = ParametersTable;

  /* Wait for Command FIFO Empty */
  while ((DSI->GPSR & DSI_GPSR_CMDFE) == 0U){}

  /* Set the DCS code on payload byte 1, and the other parameters on the write FIFO command*/
  fifoword = Param1;
  nbBytes = (NbParams < 3U) ? NbParams : 3U;

  for (count = 0U; count < nbBytes; count++)
  {
    fifoword |= (((uint32_t)(*(pparams + count))) << (8U + (8U * count)));
  }
  DSI->GPDR = fifoword;

  uicounter = NbParams - nbBytes;
  pparams += nbBytes;
  /* Set the Next parameters on the write FIFO command*/
  while (uicounter != 0U)
  {
    nbBytes = (uicounter < 4U) ? uicounter : 4U;
    fifoword = 0U;
    for (count = 0U; count < nbBytes; count++)
    {
      fifoword |= (((uint32_t)(*(pparams + count))) << (8U * count));
    }
    DSI->GPDR = fifoword;

    uicounter -= nbBytes;
    pparams += nbBytes;
  }

  DSI_ConfigPacketHeader(
                         ChannelID,
                         Mode,
                         ((NbParams + 1U) & 0x00FFU),
                         (((NbParams + 1U) & 0xFF00U) >> 8U));
}


/**************************************************************************************/
void DSI_Read(
                    uint32_t ChannelNbr,
                    uint8_t *Array,
                    uint32_t Size,
                    uint32_t Mode,
                    uint32_t DCSCmd,
                    uint8_t *ParametersTable)
{
  uint8_t *pdata = Array;
  uint32_t datasize = Size;
  uint32_t fifoword;
  uint32_t nbbytes;
  uint32_t count;

  if (datasize > 2U)
  {
    /* set max return packet size */
    DSI_ShortWrite(ChannelNbr, DSI_MAX_RETURN_PKT_SIZE, ((datasize) & 0xFFU),
                       (((datasize) >> 8U) & 0xFFU));


    DSI_ConfigPacketHeader(ChannelNbr, Mode, DCSCmd, 0U);
  }

  /* If DSI fifo is not empty, read requested bytes */
  while (((int32_t)(datasize)) > 0)
  {
    if ((DSI->GPSR & DSI_GPSR_PRDFE) == 0U)
    {
      fifoword = DSI->GPDR;
      nbbytes = (datasize < 4U) ? datasize : 4U;

      for (count = 0U; count < nbbytes; count++)
      {
        *pdata = (uint8_t)(fifoword >> (8U * count));
        pdata++;
        datasize--;
      }
    }

    /* Software workaround to avoid TIMEOUT when a DSI read command is   */
    /* issued to the panel and the read data is not captured by the DSI Host */
    /* which returns Packet Size Error.                                      */
    /* Need to ensure that the Read command has finished before checking PSE */
    if ((DSI->GPSR & DSI_GPSR_RCB) == 0U)
    {
      if ((DSI->ISR[1U] & DSI_ISR1_PSE) == DSI_ISR1_PSE)
      {

      }
    }
  }

}
/******************************************************************************************/
void DSI_Start(void)
{
  /* Enable the DSI host */
  SET_BIT(DSI->CR, DSI_CR_EN);

  /* Enable the DSI wrapper */
  SET_BIT(DSI->WCR, DSI_WCR_DSIEN);;
}
/******************************************************************************************/
void DSI_Refresh(void)
{
  /* Update the display */
  DSI->WCR |= DSI_WCR_LTDCEN;
}






