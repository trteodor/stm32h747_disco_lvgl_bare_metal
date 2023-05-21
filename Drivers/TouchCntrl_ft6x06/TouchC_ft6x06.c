/*
 * TouchC_ft6x06.c
 *
 *  Created on: May 16, 2022
 *      Author: teodor
 */
#include "TouchC_ft6x06.h"
#include "stm32h7xx.h"
#include "i2c4.h"
#include "GPIO_h7.h"
#include  "System.h"

TS_Init_t hTS;
__IO uint32_t InterruptTsFlag = 0;
TS_Gesture_Config_t GestureConf;
TS_State_t  TS_State = {0};

typedef struct
{
  uint32_t freq;       /* Frequency in Hz */
  uint32_t freq_min;   /* Minimum frequency in Hz */
  uint32_t freq_max;   /* Maximum frequency in Hz */
  uint32_t hddat_min;  /* Minimum data hold time in ns */
  uint32_t vddat_max;  /* Maximum data valid time in ns */
  uint32_t sudat_min;  /* Minimum data setup time in ns */
  uint32_t lscl_min;   /* Minimum low period of the SCL clock in ns */
  uint32_t hscl_min;   /* Minimum high period of SCL clock in ns */
  uint32_t trise;      /* Rise time in ns */
  uint32_t tfall;      /* Fall time in ns */
  uint32_t dnf;        /* Digital noise filter coefficient */
} I2C_Charac_t;

typedef struct
{
  uint32_t presc;      /* Timing prescaler */
  uint32_t tscldel;    /* SCL delay */
  uint32_t tsdadel;    /* SDA delay */
  uint32_t sclh;       /* SCL high period */
  uint32_t scll;       /* SCL low period */
} I2C_Timings_t;

TS_Ctx_t           Ts_Ctx[TS_INSTANCES_NBR] = {0};
void               *Ts_CompObj[TS_INSTANCES_NBR] = {0};


int32_t TS_GetTick(void);

static int32_t FT6X06_Probe(uint32_t Instance);
TouchStateFt6x06_t TS_GetTouchPointAndState(int16_t *Xread, int16_t *Yread);
TouchIRQ_StateFt6x06_t TS_GetIT_State_OTM8009a(void);
void TS_InitIT_OTM8009a(void);

static volatile TS_Drv_t           *Ts_Drv = NULL;

/******************************************************************************************************/
void TS_InitIT_OTM8009a(void) /*Gestures are not supported yet*/
{
    uint32_t x_size, y_size;

  x_size = 800;
  y_size = 480;

  hTS.Width = x_size;
  hTS.Height = y_size;
  hTS.Orientation = TS_SWAP_XY | TS_SWAP_Y;
  hTS.Accuracy = 5;

//  GestureConf.Radian = 0x0A;
//  GestureConf.OffsetLeftRight = 0x19;
//  GestureConf.OffsetUpDown = 0x19;
//  GestureConf.DistanceLeftRight = 0x19;
//  GestureConf.DistanceUpDown = 0x19;
//  GestureConf.DistanceZoom = 0x32;

  /* Touchscreen initialization */
//  TS_GestureConfig(0, &GestureConf);

  TS_Init(0, &hTS);
  TS_EnableIT(0);
}
/******************************************************************************************************/
/*
 * @brief TS_GetIT_State_OTM8009a(void)
 * IRQ TouchCntrl flag read and reset function
 */
TouchIRQ_StateFt6x06_t TS_GetIT_State_OTM8009a(void)
{
	if(InterruptTsFlag == Touch_IRQ_FlagSet_ft6x06 )
	{
		InterruptTsFlag = Touch_IRQ_FlagReset_ft6x06; /*After read reset the flag...*/
		return Touch_Touched_ft6x06;
	}
	return Touch_IRQ_FlagReset_ft6x06;
}

/******************************************************************************************************/
TouchStateFt6x06_t TS_GetTouchPointAndState(int16_t *Xread, int16_t *Yread)
{
	  TS_GetState(0, &TS_State);

	  if(TS_State.TouchDetected)
	  {
		*Xread = TS_State.TouchX;
		*Yread = TS_State.TouchY;
		return Touch_Touched_ft6x06;
	  }
	  return Touch_Released_ft6x06;
}


/******************************************************************************************************/
int32_t TS_Init(uint32_t Instance, TS_Init_t *TS_Init)
{

    FT6X06_Probe(Instance);

      TS_Capabilities_t Capabilities;
      Ts_Ctx[Instance].Width             = TS_Init->Width;
      Ts_Ctx[Instance].Height            = TS_Init->Height;
      Ts_Ctx[Instance].Orientation       = TS_Init->Orientation;
      Ts_Ctx[Instance].Accuracy          = TS_Init->Accuracy;

      Ts_Drv->GetCapabilities(Ts_CompObj[Instance], &Capabilities);


        /* Store maximum X and Y on context */
        Ts_Ctx[Instance].MaxX = Capabilities.MaxXl;
        Ts_Ctx[Instance].MaxY = Capabilities.MaxYl;
        /* Initialize previous position in order to always detect first touch */
        for(int i = 0; i < TS_TOUCH_NBR; i++)
        {
          Ts_Ctx[Instance].PreviousX[i] = TS_Init->Width + TS_Init->Accuracy + 1U;
          Ts_Ctx[Instance].PreviousY[i] = TS_Init->Height + TS_Init->Accuracy + 1U;
        }

 return 0;
}


int32_t TS_GetCapabilities(uint32_t Instance, TS_Capabilities_t *Capabilities)
{

 (void)Ts_Drv->GetCapabilities(Ts_CompObj[Instance], Capabilities);

  return 0;
}
/******************************************************************************************************/
int32_t TS_EnableIT(uint32_t Instance)
{
  // int32_t ret = DRV_ERR_NONE;
  GPIO_InitTypeDef gpio_init_structure;
//  static const uint32_t TS_EXTI_LINE[TS_INSTANCES_NBR] = {TS_INT_LINE};
  if(Instance >=TS_INSTANCES_NBR)
  {
    // ret = DRV_ERROR_WRONG_PARAM;
  }
  else
  {

    TS_INT_GPIO_CLK_ENABLE();
    /* Configure Interrupt mode for TS_INT pin falling edge : when a new touch is available */
    /* TS_INT pin is active on low level on new touch available */
    gpio_init_structure.Pin = TS_INT_PIN;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
    /*TODO: !!!!!*/
    gpio_init_structure.Mode = GPIO_MODE_IT_FALLING;
    GPIO_Init(TS_INT_GPIO_PORT, &gpio_init_structure);
    Ts_Drv->EnableIT(Ts_CompObj[Instance]);
    NVIC_SetPriority ((IRQn_Type)(TS_INT_EXTI_IRQn), (1UL << __NVIC_PRIO_BITS) - 1UL);
    NVIC_EnableIRQ((IRQn_Type)(TS_INT_EXTI_IRQn));
  }
  return 0;
}

int32_t TS_GetState(uint32_t Instance, TS_State_t *TS_State)
{
  // int32_t ret = DRV_ERR_NONE;
  // UNUSED(ret);
  uint32_t x_oriented, y_oriented;
  uint32_t x_diff, y_diff;


    FT6X06_State_t state;

    /* Get each touch coordinates */
    if(Ts_Drv->GetState(Ts_CompObj[Instance], &state) < 0)
    {
      // ret = DRV_ERROR_COMPONENT_FAILURE;
    }/* Check and update the number of touches active detected */
    else if(state.TouchDetected != 0U)
    {
      x_oriented = state.TouchX;
      y_oriented = state.TouchY;

      if((Ts_Ctx[Instance].Orientation & TS_SWAP_XY) == TS_SWAP_XY)
      {
        x_oriented = state.TouchY;
        y_oriented = state.TouchX;
      }

      if((Ts_Ctx[Instance].Orientation & TS_SWAP_X) == TS_SWAP_X)
      {
        x_oriented = Ts_Ctx[Instance].MaxX - x_oriented - 1UL;
      }

      if((Ts_Ctx[Instance].Orientation & TS_SWAP_Y) == TS_SWAP_Y)
      {
        y_oriented = Ts_Ctx[Instance].MaxY - y_oriented - 1UL;
      }

      /* Apply boundary */
      TS_State->TouchX = (x_oriented * Ts_Ctx[Instance].Width) / Ts_Ctx[Instance].MaxX;
      TS_State->TouchY = (y_oriented * Ts_Ctx[Instance].Height) / Ts_Ctx[Instance].MaxY;
      /* Store Current TS state */
      TS_State->TouchDetected = state.TouchDetected;

      /* Check accuracy */
      x_diff = (TS_State->TouchX > Ts_Ctx[Instance].PreviousX[0])?
               (TS_State->TouchX - Ts_Ctx[Instance].PreviousX[0]):
               (Ts_Ctx[Instance].PreviousX[0] - TS_State->TouchX);

      y_diff = (TS_State->TouchY > Ts_Ctx[Instance].PreviousY[0])?
               (TS_State->TouchY - Ts_Ctx[Instance].PreviousY[0]):
               (Ts_Ctx[Instance].PreviousY[0] - TS_State->TouchY);


      if ((x_diff > Ts_Ctx[Instance].Accuracy) || (y_diff > Ts_Ctx[Instance].Accuracy))
      {
        /* New touch detected */
        Ts_Ctx[Instance].PreviousX[0] = TS_State->TouchX;
        Ts_Ctx[Instance].PreviousY[0] = TS_State->TouchY;
      }
      else
      {
        TS_State->TouchX = Ts_Ctx[Instance].PreviousX[0];
        TS_State->TouchY = Ts_Ctx[Instance].PreviousY[0];
      }
    }
    else
    {
      TS_State->TouchDetected = 0U;
      TS_State->TouchX = Ts_Ctx[Instance].PreviousX[0];
      TS_State->TouchY = Ts_Ctx[Instance].PreviousY[0];
    }

  return 0;
}

#if (USE_TS_MULTI_TOUCH > 0)
int32_t TS_Get_MultiTouchState(uint32_t Instance, TS_MultiTouch_State_t *TS_State)
{
  uint32_t x_oriented[2], y_oriented[2];
  uint32_t x_diff, y_diff;
  uint32_t index;

    TS_MultiTouch_State_t state;
    /* Get each touch coordinates */
    if(Ts_Drv->GetMultiTouchState(Ts_CompObj[Instance], &state) < 0)
    {

    }/* Check and update the number of touches active detected */
    else if(state.TouchDetected != 0U)
    {
      for(index = 0; index < state.TouchDetected; index++)
      {
        x_oriented[index] = state.TouchX[index];
        y_oriented[index] = state.TouchY[index];

        if((Ts_Ctx[Instance].Orientation & TS_SWAP_XY) == TS_SWAP_XY)
        {
          x_oriented[index] = state.TouchY[index];
          y_oriented[index] = state.TouchX[index];
        }

        if((Ts_Ctx[Instance].Orientation & TS_SWAP_X) == TS_SWAP_X)
        {
          x_oriented[index] = Ts_Ctx[Instance].MaxX - x_oriented[index] - 1UL;
        }

        if((Ts_Ctx[Instance].Orientation & TS_SWAP_Y) == TS_SWAP_Y)
        {
          y_oriented[index] = Ts_Ctx[Instance].MaxY - y_oriented[index] - 1UL;
        }

       /* Apply boundary */
       TS_State->TouchX[index] = (x_oriented[index] * Ts_Ctx[Instance].Width) / Ts_Ctx[Instance].MaxX;
       TS_State->TouchY[index] = (y_oriented[index] * Ts_Ctx[Instance].Height) / Ts_Ctx[Instance].MaxY;
       /* Store Current TS state */
       TS_State->TouchDetected = state.TouchDetected;

        /* Check accuracy */
        x_diff = (TS_State->TouchX[index] > Ts_Ctx[Instance].PreviousX[0])?
          (TS_State->TouchX[index] - Ts_Ctx[Instance].PreviousX[0]):
          (Ts_Ctx[Instance].PreviousX[0] - TS_State->TouchX[index]);

        y_diff = (TS_State->TouchY[index] > Ts_Ctx[Instance].PreviousY[0])?
          (TS_State->TouchY[index] - Ts_Ctx[Instance].PreviousY[0]):
          (Ts_Ctx[Instance].PreviousY[0] - TS_State->TouchY[index]);

        if ((x_diff > Ts_Ctx[Instance].Accuracy) || (y_diff > Ts_Ctx[Instance].Accuracy))
        {
          /* New touch detected */
          Ts_Ctx[Instance].PreviousX[index] = TS_State->TouchX[index];
          Ts_Ctx[Instance].PreviousY[index] = TS_State->TouchY[index];
        }
        else
        {
          TS_State->TouchX[index] = Ts_Ctx[Instance].PreviousX[index];
          TS_State->TouchY[index] = Ts_Ctx[Instance].PreviousY[index];
        }
      }
    }
    else
    {
      TS_State->TouchDetected = 0U;
      for(index = 0; index < TS_TOUCH_NBR; index++)
      {
        TS_State->TouchX[index] = Ts_Ctx[Instance].PreviousX[index];
        TS_State->TouchY[index] = Ts_Ctx[Instance].PreviousY[index];
      }
    }

  return 0;
}
#endif /* USE_TS_MULTI_TOUCH > 1 */

#if (USE_TS_GESTURE > 0)

int32_t TS_GestureConfig(uint32_t Instance, TS_Gesture_Config_t *GestureConfig)
{
	Ts_Drv->GestureConfig(Ts_CompObj[Instance], GestureConfig);
	return 0;
}

int32_t TS_GetGestureId(uint32_t Instance, uint32_t *GestureId)
{

  uint8_t tmp = 0;

  if(Instance >=TS_INSTANCES_NBR)
  {

  }/* Get gesture Id */
  else if(Ts_Drv->GetGesture(Ts_CompObj[Instance], &tmp)  < 0)
  {

  }
  else
  {
    /* Remap gesture Id to a TS_Gesture_Id_t value */
    switch(tmp)
    {
    case FT6X06_GEST_ID_NO_GESTURE :
      *GestureId = GESTURE_ID_NO_GESTURE;
      break;
    case FT6X06_GEST_ID_MOVE_UP :
      *GestureId = GESTURE_ID_MOVE_UP;
      break;
    case FT6X06_GEST_ID_MOVE_RIGHT :
      *GestureId = GESTURE_ID_MOVE_RIGHT;
      break;
    case FT6X06_GEST_ID_MOVE_DOWN :
      *GestureId = GESTURE_ID_MOVE_DOWN;
      break;
    case FT6X06_GEST_ID_MOVE_LEFT :
      *GestureId = GESTURE_ID_MOVE_LEFT;
      break;
    case FT6X06_GEST_ID_ZOOM_IN :
      *GestureId = GESTURE_ID_ZOOM_IN;
      break;
    case FT6X06_GEST_ID_ZOOM_OUT :
      *GestureId = GESTURE_ID_ZOOM_OUT;
      break;
    default :
      *GestureId = GESTURE_ID_NO_GESTURE;
      break;
    }

    return 0;
  }

  return 0;
}
#endif /* USE_TS_GESTURE > 0 */

int32_t TS_Set_Orientation(uint32_t Instance, uint32_t Orientation)
{
  Ts_Ctx[Instance].Orientation = Orientation;
  return 0;
}

int32_t TS_Get_Orientation(uint32_t Instance, uint32_t *Orientation)
{
  *Orientation = Ts_Ctx[Instance].Orientation;
  return 0;
}

static int32_t FT6X06_Probe(uint32_t Instance)
{
  int32_t ret;
  FT6X06_IO_t              IOCtx;
  static FT6X06_Object_t   FT6X06Obj;
  FT6X06_Capabilities_t    Cap;
  uint32_t id, i;
  uint32_t const i2c_address[] = {TS_I2C_ADDRESS, TS_I2C_ADDRESS_A02};

  /* Configure the touch screen driver */
  IOCtx.Init        = Touch_I2C4_Init;
  IOCtx.DeInit      = I2C4_DeInit;
  IOCtx.ReadReg     = I2C4_ReadReg;
  IOCtx.WriteReg    = I2C4_WriteReg;
  IOCtx.GetTick     = TS_GetTick;

  for(i = 1; i < 2UL; i++)
  {
    IOCtx.Address     = (uint16_t)i2c_address[i];

    if(FT6X06_RegisterBusIO (&FT6X06Obj, &IOCtx) != FT6X06_OK)
    {
      ret = -1;
    }
    else if(FT6X06_ReadID(&FT6X06Obj, &id) != FT6X06_OK)
    {
      ret = -1;
    }
    else if(id != FT6X06_ID)
    {
      ret = -1;
    }
    else
    {

      (void)FT6X06_GetCapabilities(&FT6X06Obj, &Cap);
      Ts_CompObj[Instance] = &FT6X06Obj;
      Ts_Drv = (TS_Drv_t *) &FT6X06_TS_Driver;

      if(Ts_Drv->Init(Ts_CompObj[Instance]) != FT6X06_OK)
      {
      ret = -1;
      }
      else
      {
        ret = -0;
        break;
      }
    }
  }

  return ret;
}

int32_t TS_GetTick(void)
{
  return (int32_t)GetSysTime();
}

/*
 ****************************************************
 ****************************************************
 ****************************************************
 ****************************************************
 I2C4 Section
 */

int32_t Touch_I2C4_Init(void)
{
    I2C4_Init();
    return 0;
}

int32_t I2C4_DeInit(void)
{

  return 0;
}

int32_t I2C4_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
    I2C4_Mem_Write(DevAddr, Reg,I2C_MEMADD_SIZE_8BIT, pData,Length);
    return 0;
}

int32_t I2C4_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
    I2C4_Mem_Read(DevAddr,Reg,I2C_MEMADD_SIZE_8BIT,pData,Length);
    return 0;
}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI->PR1 && 0x80)
	{
    InterruptTsFlag = 1;
		EXTI->PR1 |= 0x80; /*clear the IRQ flag by writing 1*/
	}
}




