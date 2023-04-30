
#ifndef __DSI_HOST_H_
#define __DSI_HOST_H_

#include "stm32h7xx.h"


#define DSI_VACT            480
#define DSI_HACT            100      /* !!!! SCREEN DIVIDED INTO 4 AREAS !!!! */




void DSIHOST_DSI_Init(void);
void DSI_ConfigFlowControl(uint32_t FlowControl);
void DSI_Start(void);
void DSI_ConfigFlowControl(uint32_t FlowControl);
void DSI_Refresh(void);
void DSI_RegisterIrqCallBacks(void(*TearingEffectCb)(void), void (*EndOfRefreshCb)(void));

void DSI_Read(
                    uint32_t ChannelNbr,
                    uint8_t *Array,
                    uint32_t Size,
                    uint32_t Mode,
                    uint32_t DCSCmd,
                    uint8_t *ParametersTable);



void DSI_ShortWrite(
                        uint32_t ChannelID,
                        uint32_t Mode,
                        uint32_t Param1,
                        uint32_t Param2);

void DSI_LongWrite(
                        uint32_t ChannelID,
                        uint32_t Mode,
                        uint32_t NbParams,
                        uint32_t Param1,
                        uint8_t *ParametersTable);


/**
  * @brief  DSI Video mode configuration
  */
typedef struct
{
  uint32_t VirtualChannelID;             /*!< Virtual channel ID                                                 */

  uint32_t ColorCoding;                  /*!< Color coding for LTDC interface
                                              This parameter can be any value of @ref DSI_Color_Coding           */

  uint32_t LooselyPacked;                /*!< Enable or disable loosely packed stream (needed only when using
                                              18-bit configuration).
                                              This parameter can be any value of @ref DSI_LooselyPacked          */

  uint32_t Mode;                         /*!< Video mode type
                                              This parameter can be any value of @ref DSI_Video_Mode_Type        */

  uint32_t PacketSize;                   /*!< Video packet size                                                  */

  uint32_t NumberOfChunks;               /*!< Number of chunks                                                   */

  uint32_t NullPacketSize;               /*!< Null packet size                                                   */

  uint32_t HSPolarity;                   /*!< HSYNC pin polarity
                                              This parameter can be any value of @ref DSI_HSYNC_Polarity         */

  uint32_t VSPolarity;                   /*!< VSYNC pin polarity
                                              This parameter can be any value of @ref DSI_VSYNC_Active_Polarity  */

  uint32_t DEPolarity;                   /*!< Data Enable pin polarity
                                              This parameter can be any value of @ref DSI_DATA_ENABLE_Polarity   */

  uint32_t HorizontalSyncActive;         /*!< Horizontal synchronism active duration (in lane byte clock cycles) */

  uint32_t HorizontalBackPorch;          /*!< Horizontal back-porch duration (in lane byte clock cycles)         */

  uint32_t HorizontalLine;               /*!< Horizontal line duration (in lane byte clock cycles)               */

  uint32_t VerticalSyncActive;           /*!< Vertical synchronism active duration                               */

  uint32_t VerticalBackPorch;            /*!< Vertical back-porch duration                                       */

  uint32_t VerticalFrontPorch;           /*!< Vertical front-porch duration                                      */

  uint32_t VerticalActive;               /*!< Vertical active duration                                           */

  uint32_t LPCommandEnable;              /*!< Low-power command enable
                                              This parameter can be any value of @ref DSI_LP_Command             */

  uint32_t LPLargestPacketSize;          /*!< The size, in bytes, of the low power largest packet that
                                              can fit in a line during VSA, VBP and VFP regions                  */

  uint32_t LPVACTLargestPacketSize;      /*!< The size, in bytes, of the low power largest packet that
                                              can fit in a line during VACT region                               */

  uint32_t LPHorizontalFrontPorchEnable; /*!< Low-power horizontal front-porch enable
                                              This parameter can be any value of @ref DSI_LP_HFP                 */

  uint32_t LPHorizontalBackPorchEnable;  /*!< Low-power horizontal back-porch enable
                                              This parameter can be any value of @ref DSI_LP_HBP                 */

  uint32_t LPVerticalActiveEnable;       /*!< Low-power vertical active enable
                                              This parameter can be any value of @ref DSI_LP_VACT                */

  uint32_t LPVerticalFrontPorchEnable;   /*!< Low-power vertical front-porch enable
                                              This parameter can be any value of @ref DSI_LP_VFP                 */

  uint32_t LPVerticalBackPorchEnable;    /*!< Low-power vertical back-porch enable
                                              This parameter can be any value of @ref DSI_LP_VBP                 */

  uint32_t LPVerticalSyncActiveEnable;   /*!< Low-power vertical sync active enable
                                              This parameter can be any value of @ref DSI_LP_VSYNC               */

  uint32_t FrameBTAAcknowledgeEnable;    /*!< Frame bus-turn-around acknowledge enable
                                              This parameter can be any value of @ref DSI_FBTA_acknowledge       */

} DSI_VidCfgTypeDef;


/**
  * @brief  DSI Adapted command mode configuration
  */
typedef struct
{
  uint32_t VirtualChannelID;             /*!< Virtual channel ID                                                */

  uint32_t ColorCoding;                  /*!< Color coding for LTDC interface
                                              This parameter can be any value of @ref DSI_Color_Coding          */

  uint32_t CommandSize;                  /*!< Maximum allowed size for an LTDC write memory command, measured in
                                              pixels. This parameter can be any value between 0x00 and 0xFFFFU   */

  uint32_t TearingEffectSource;          /*!< Tearing effect source
                                              This parameter can be any value of @ref DSI_TearingEffectSource   */

  uint32_t TearingEffectPolarity;        /*!< Tearing effect pin polarity
                                              This parameter can be any value of @ref DSI_TearingEffectPolarity */

  uint32_t HSPolarity;                   /*!< HSYNC pin polarity
                                              This parameter can be any value of @ref DSI_HSYNC_Polarity        */

  uint32_t VSPolarity;                   /*!< VSYNC pin polarity
                                              This parameter can be any value of @ref DSI_VSYNC_Active_Polarity */

  uint32_t DEPolarity;                   /*!< Data Enable pin polarity
                                              This parameter can be any value of @ref DSI_DATA_ENABLE_Polarity  */

  uint32_t VSyncPol;                     /*!< VSync edge on which the LTDC is halted
                                              This parameter can be any value of @ref DSI_Vsync_Polarity        */

  uint32_t AutomaticRefresh;             /*!< Automatic refresh mode
                                              This parameter can be any value of @ref DSI_AutomaticRefresh      */

  uint32_t TEAcknowledgeRequest;         /*!< Tearing Effect Acknowledge Request Enable
                                              This parameter can be any value of @ref DSI_TE_AcknowledgeRequest */

} DSI_CmdCfgTypeDef;

/**
  * @brief  DSI command transmission mode configuration
  */
typedef struct
{
  uint32_t LPGenShortWriteNoP;           /*!< Generic Short Write Zero parameters Transmission
                                              This parameter can be any value of @ref DSI_LP_LPGenShortWriteNoP  */

  uint32_t LPGenShortWriteOneP;          /*!< Generic Short Write One parameter Transmission
                                              This parameter can be any value of @ref DSI_LP_LPGenShortWriteOneP */

  uint32_t LPGenShortWriteTwoP;          /*!< Generic Short Write Two parameters Transmission
                                              This parameter can be any value of @ref DSI_LP_LPGenShortWriteTwoP */

  uint32_t LPGenShortReadNoP;            /*!< Generic Short Read Zero parameters Transmission
                                              This parameter can be any value of @ref DSI_LP_LPGenShortReadNoP   */

  uint32_t LPGenShortReadOneP;           /*!< Generic Short Read One parameter Transmission
                                              This parameter can be any value of @ref DSI_LP_LPGenShortReadOneP  */

  uint32_t LPGenShortReadTwoP;           /*!< Generic Short Read Two parameters Transmission
                                              This parameter can be any value of @ref DSI_LP_LPGenShortReadTwoP  */

  uint32_t LPGenLongWrite;               /*!< Generic Long Write Transmission
                                              This parameter can be any value of @ref DSI_LP_LPGenLongWrite      */

  uint32_t LPDcsShortWriteNoP;           /*!< DCS Short Write Zero parameters Transmission
                                              This parameter can be any value of @ref DSI_LP_LPDcsShortWriteNoP  */

  uint32_t LPDcsShortWriteOneP;          /*!< DCS Short Write One parameter Transmission
                                              This parameter can be any value of @ref DSI_LP_LPDcsShortWriteOneP */

  uint32_t LPDcsShortReadNoP;            /*!< DCS Short Read Zero parameters Transmission
                                              This parameter can be any value of @ref DSI_LP_LPDcsShortReadNoP   */

  uint32_t LPDcsLongWrite;               /*!< DCS Long Write Transmission
                                              This parameter can be any value of @ref DSI_LP_LPDcsLongWrite      */

  uint32_t LPMaxReadPacket;              /*!< Maximum Read Packet Size Transmission
                                              This parameter can be any value of @ref DSI_LP_LPMaxReadPacket     */

  uint32_t AcknowledgeRequest;           /*!< Acknowledge Request Enable
                                              This parameter can be any value of @ref DSI_AcknowledgeRequest     */

} DSI_LPCmdTypeDef;



#define DSI_RGB565                 0x00000000U /*!< The values 0x00000001 and 0x00000002 can also be used for the RGB565 color mode configuration */
#define DSI_RGB666                 0x00000003U /*!< The value 0x00000004 can also be used for the RGB666 color mode configuration                 */
#define DSI_RGB888                 0x00000005U

#define DSI_LOOSELY_PACKED_ENABLE  DSI_LCOLCR_LPE
#define DSI_LOOSELY_PACKED_DISABLE 0x00000000U

#define DSI_VID_MODE_NB_PULSES    0U
#define DSI_VID_MODE_NB_EVENTS    1U
#define DSI_VID_MODE_BURST        2U

#define DSI_HSYNC_ACTIVE_HIGH       0x00000000U
#define DSI_HSYNC_ACTIVE_LOW        DSI_LPCR_HSP

#define DSI_VSYNC_ACTIVE_HIGH       0x00000000U
#define DSI_VSYNC_ACTIVE_LOW        DSI_LPCR_VSP

#define DSI_DATA_ENABLE_ACTIVE_HIGH 0x00000000U
#define DSI_DATA_ENABLE_ACTIVE_LOW  DSI_LPCR_DEP

#define DSI_LP_COMMAND_DISABLE    0x00000000U
#define DSI_LP_COMMAND_ENABLE     DSI_VMCR_LPCE

#define DSI_LP_HFP_DISABLE        0x00000000U
#define DSI_LP_HFP_ENABLE         DSI_VMCR_LPHFPE

#define DSI_LP_HBP_DISABLE        0x00000000U
#define DSI_LP_HBP_ENABLE         DSI_VMCR_LPHBPE

#define DSI_LP_VACT_DISABLE       0x00000000U
#define DSI_LP_VACT_ENABLE        DSI_VMCR_LPVAE

#define DSI_LP_VFP_DISABLE       0x00000000U
#define DSI_LP_VFP_ENABLE        DSI_VMCR_LPVFPE

#define DSI_LP_VBP_DISABLE       0x00000000U
#define DSI_LP_VBP_ENABLE        DSI_VMCR_LPVBPE

#define DSI_LP_VSYNC_DISABLE     0x00000000U
#define DSI_LP_VSYNC_ENABLE      DSI_VMCR_LPVSAE

#define DSI_FBTAA_DISABLE        0x00000000U
#define DSI_FBTAA_ENABLE         DSI_VMCR_FBTAAE

#define DSI_TE_DSILINK           0x00000000U
#define DSI_TE_EXTERNAL          DSI_WCFGR_TESRC

#define DSI_TE_RISING_EDGE       0x00000000U
#define DSI_TE_FALLING_EDGE      DSI_WCFGR_TEPOL

#define DSI_VSYNC_FALLING        0x00000000U
#define DSI_VSYNC_RISING         DSI_WCFGR_VSPOL

#define DSI_AR_DISABLE           0x00000000U
#define DSI_AR_ENABLE            DSI_WCFGR_AR

#define DSI_TE_ACKNOWLEDGE_DISABLE 0x00000000U
#define DSI_TE_ACKNOWLEDGE_ENABLE  DSI_CMCR_TEARE

#define DSI_LP_GSW0P_DISABLE     0x00000000U
#define DSI_LP_GSW0P_ENABLE      DSI_CMCR_GSW0TX

#define DSI_LP_GSW1P_DISABLE     0x00000000U
#define DSI_LP_GSW1P_ENABLE      DSI_CMCR_GSW1TX

#define DSI_LP_GSW2P_DISABLE     0x00000000U
#define DSI_LP_GSW2P_ENABLE      DSI_CMCR_GSW2TX

#define DSI_LP_GSR0P_DISABLE     0x00000000U
#define DSI_LP_GSR0P_ENABLE      DSI_CMCR_GSR0TX

#define DSI_LP_GSR1P_DISABLE     0x00000000U
#define DSI_LP_GSR1P_ENABLE      DSI_CMCR_GSR1TX

#define DSI_LP_GSR2P_DISABLE     0x00000000U
#define DSI_LP_GSR2P_ENABLE      DSI_CMCR_GSR2TX

#define DSI_LP_GLW_DISABLE       0x00000000U
#define DSI_LP_GLW_ENABLE        DSI_CMCR_GLWTX

#define DSI_LP_DSW0P_DISABLE     0x00000000U
#define DSI_LP_DSW0P_ENABLE      DSI_CMCR_DSW0TX

#define DSI_LP_DSW1P_DISABLE     0x00000000U
#define DSI_LP_DSW1P_ENABLE      DSI_CMCR_DSW1TX

#define DSI_LP_DSR0P_DISABLE     0x00000000U
#define DSI_LP_DSR0P_ENABLE      DSI_CMCR_DSR0TX

#define DSI_LP_DLW_DISABLE       0x00000000U
#define DSI_LP_DLW_ENABLE        DSI_CMCR_DLWTX














#define DSI_ONE_DATA_LANE          0U
#define DSI_TWO_DATA_LANES         1U

#define DSI_AUTO_CLK_LANE_CTRL_DISABLE 0x00000000U
#define DSI_AUTO_CLK_LANE_CTRL_ENABLE  DSI_CLCR_ACR


#define __PLLNDIV 20

#define DSI_PLL_IN_DIV1             0x00000001U
#define DSI_PLL_IN_DIV2             0x00000002U
#define DSI_PLL_IN_DIV3             0x00000003U
#define DSI_PLL_IN_DIV4             0x00000004U
#define DSI_PLL_IN_DIV5             0x00000005U
#define DSI_PLL_IN_DIV6             0x00000006U
#define DSI_PLL_IN_DIV7             0x00000007U

#define DSI_PLL_OUT_DIV1            0x00000000U
#define DSI_PLL_OUT_DIV2            0x00000001U
#define DSI_PLL_OUT_DIV4            0x00000002U
#define DSI_PLL_OUT_DIV8            0x00000003U


#define DSI_DCS_SHORT_PKT_WRITE_P0  0x00000005U /*!< DCS short write, no parameters      */
#define DSI_DCS_SHORT_PKT_WRITE_P1  0x00000015U /*!< DCS short write, one parameter      */
#define DSI_GEN_SHORT_PKT_WRITE_P0  0x00000003U /*!< Generic short write, no parameters  */
#define DSI_GEN_SHORT_PKT_WRITE_P1  0x00000013U /*!< Generic short write, one parameter  */
#define DSI_GEN_SHORT_PKT_WRITE_P2  0x00000023U /*!< Generic short write, two parameters */


#define DSI_DCS_LONG_PKT_WRITE      0x00000039U /*!< DCS long write     */
#define DSI_GEN_LONG_PKT_WRITE      0x00000029U /*!< Generic long write */

#define DSI_MAX_RETURN_PKT_SIZE (0x00000037U) /*!< Maximum return packet configuration */

#define DSI_DCS_SHORT_PKT_READ      0x00000006U /*!< DCS short read                     */
#define DSI_GEN_SHORT_PKT_READ_P0   0x00000004U /*!< Generic short read, no parameters  */
#define DSI_GEN_SHORT_PKT_READ_P1   0x00000014U /*!< Generic short read, one parameter  */
#define DSI_GEN_SHORT_PKT_READ_P2   0x00000024U /*!< Generic short read, two parameters */


#define DSI_FLOW_CONTROL_CRC_RX    DSI_PCR_CRCRXE
#define DSI_FLOW_CONTROL_ECC_RX    DSI_PCR_ECCRXE
#define DSI_FLOW_CONTROL_BTA       DSI_PCR_BTAE
#define DSI_FLOW_CONTROL_EOTP_RX   DSI_PCR_ETRXE
#define DSI_FLOW_CONTROL_EOTP_TX   DSI_PCR_ETTXE
#define DSI_FLOW_CONTROL_ALL       (DSI_FLOW_CONTROL_CRC_RX | DSI_FLOW_CONTROL_ECC_RX  | \
                                    DSI_FLOW_CONTROL_BTA    | DSI_FLOW_CONTROL_EOTP_RX | \
                                    DSI_FLOW_CONTROL_EOTP_TX)


#define GPIO_AF13_DSI           ((uint8_t)0x0D)   /* DSI Alternate Function mapping   */

#define __DSI_MASK_TE()   (GPIOJ->AFR[0] &= (0xFFFFF0FFU))   /* Mask DSI TearingEffect Pin*/
#define __DSI_UNMASK_TE() (GPIOJ->AFR[0] |= ((uint32_t)(GPIO_AF13_DSI) << 8)) /* UnMask DSI TearingEffect Pin*/


#define __DSI_WRAPPER_ENABLE() do { \
                                                  __IO uint32_t tmpreg = 0x00U; \
                                                  SET_BIT(DSI->WCR, DSI_WCR_DSIEN);\
                                                  /* Delay after an DSI warpper enabling */ \
                                                  tmpreg = READ_BIT(DSI->WCR, DSI_WCR_DSIEN);\
                                                  (void)(tmpreg); \
                                                } while(0U)

#define __DSI_WRAPPER_DISABLE() do { \
                                                   __IO uint32_t tmpreg = 0x00U; \
                                                   CLEAR_BIT(DSI->WCR, DSI_WCR_DSIEN);\
                                                   /* Delay after an DSI warpper disabling*/ \
                                                   tmpreg = READ_BIT(DSI->WCR, DSI_WCR_DSIEN);\
                                                   (void)(tmpreg); \
                                                 } while(0U)













void DSI_Host_PeriphInit(void);
void DSI_ConfigAdaptedCommandMode(DSI_LPCmdTypeDef *LPCmd);






#endif /*__DSI_HOST_H_*/