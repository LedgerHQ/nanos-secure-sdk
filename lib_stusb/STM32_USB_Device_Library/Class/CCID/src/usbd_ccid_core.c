/**
  ******************************************************************************
  * @file    usbd_ccid_core.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    31-January-2014
  * @brief   This file provides all the CCID core functions.
  *
  * @verbatim
  *      
  *          ===================================================================      
  *                                CCID Class  Description
  *          =================================================================== 
  *           This module manages the Specification for Integrated Circuit(s) 
  *             Cards Interface Revision 1.1
  *           This driver implements the following aspects of the specification:
  *             - Bulk Transfers 
  *      
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbd_ccid_core.h"

#ifdef HAVE_USB_CLASS_CCID

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/ 



/* Private function ----------------------------------------------------------*/ 

/**
  * @brief  USBD_CCID_Init
  *         Initialize  the USB CCID Interface 
  * @param  pdev: device instance
  * @param  cfgidx: configuration index
  * @retval status
  */
uint8_t  USBD_CCID_Init (USBD_HandleTypeDef  *pdev, 
                            uint8_t cfgidx)
{
  UNUSED(cfgidx);
  /* Open EP IN */
  USBD_LL_OpenEP(pdev,
              CCID_BULK_IN_EP,
              USBD_EP_TYPE_BULK,
              CCID_BULK_EPIN_SIZE);
  
  /* Open EP OUT */
  USBD_LL_OpenEP(pdev,
              CCID_BULK_OUT_EP,
              USBD_EP_TYPE_BULK,
              CCID_BULK_EPOUT_SIZE);

#ifdef HAVE_CCID_INTERRUPT
    /* Open INTR EP IN */
  USBD_LL_OpenEP(pdev,
              CCID_INTR_IN_EP,
              USBD_EP_TYPE_INTR,
              CCID_INTR_EPIN_SIZE);
#endif // HAVE_CCID_INTERRUPT

  /* Init the CCID  layer */
  CCID_Init(pdev); 
  
  return USBD_OK;
}

/**
  * @brief  USBD_CCID_DeInit
  *         DeInitilaize the usb ccid configuration
  * @param  pdev: device instance
  * @param  cfgidx: configuration index
  * @retval status
  */
uint8_t  USBD_CCID_DeInit (USBD_HandleTypeDef  *pdev, 
                              uint8_t cfgidx)
{
  UNUSED(cfgidx);
  /* Close CCID EPs */
  USBD_LL_CloseEP (pdev , CCID_BULK_IN_EP);
  USBD_LL_CloseEP (pdev , CCID_BULK_OUT_EP);
#ifdef HAVE_CCID_INTERRUPT
  USBD_LL_CloseEP (pdev , CCID_INTR_IN_EP);
#endif // HAVE_CCID_INTERRUPT  
    
  /* Un Init the CCID layer */
  CCID_DeInit(pdev);   
  return USBD_OK;
}

/**
  * @brief  USBD_CCID_Setup
  *         Handle the CCID specific requests
  * @param  pdev: device instance
  * @param  req: USB request
  * @retval status
  */
uint8_t  USBD_CCID_Setup (USBD_HandleTypeDef  *pdev, USBD_SetupReqTypedef *req)
{
  uint8_t slot_nb;
  uint8_t seq_nb;
  
  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  /* Class request */
  case USB_REQ_TYPE_CLASS :
    switch (req->bRequest)
    {
    case REQUEST_ABORT :

        if ((req->wLength == 0) &&
         ((req->bmRequest & 0x80) != 0x80))
        { /* Check bmRequest : No Data-In stage. 0x80 is Data Direction */
          
          /* The wValue field contains the slot number (bSlot) in the low byte 
          and the sequence number (bSeq) in the high byte.*/
          slot_nb = (uint8_t) ((req->wValue) & 0x00ff);
          seq_nb = (uint8_t) (((req->wValue) & 0xff00)>>8);
          
          if ( CCID_CmdAbort(slot_nb, seq_nb) != 0 )
          { /* If error is returned by lower layer : 
                 Generally Slot# may not have matched */
            USBD_CtlError(pdev , req);
            return USBD_FAIL; 
          }
        }
      else
      {
         USBD_CtlError(pdev , req);
         return USBD_FAIL; 
      }
      break;
      
    case REQUEST_GET_CLOCK_FREQUENCIES :
      if((req->wValue  == 0) && 
         (req->wLength != 0) &&
        ((req->bmRequest & 0x80) == 0x80))
        {   /* Check bmRequest : Data-In stage. 0x80 is Data Direction */
            USBD_CtlError(pdev , req);
            return USBD_FAIL; 
      }
      else
      {
         USBD_CtlError(pdev , req);
         return USBD_FAIL; 
      }
      break;

      case REQUEST_GET_DATA_RATES :
      if((req->wValue  == 0) && 
         (req->wLength != 0) &&
        ((req->bmRequest & 0x80) == 0x80))
       {  /* Check bmRequest : Data-In stage. 0x80 is Data Direction */
            USBD_CtlError(pdev , req);
            return USBD_FAIL; 
      }
      else
      {
         USBD_CtlError(pdev , req);
         return USBD_FAIL; 
      }
      break;
      
    default:
       USBD_CtlError(pdev , req);
       return USBD_FAIL; 
    }
    break;
  /* Interface & Endpoint request */
  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQ_GET_INTERFACE :
      break;
      
    case USB_REQ_SET_INTERFACE :
      break;
    
    case USB_REQ_CLEAR_FEATURE:  

      /* Re-activate the EP */      
      USBD_LL_CloseEP (pdev , (uint8_t)req->wIndex);
      if((((uint8_t)req->wIndex) & 0x80) == 0x80)
      {
        USBD_LL_OpenEP(pdev,
                    ((uint8_t)req->wIndex),
                    USBD_EP_TYPE_BULK,
                    CCID_BULK_EPIN_SIZE);
      }
      else
      {
        USBD_LL_OpenEP(pdev,
                    ((uint8_t)req->wIndex),
                    USBD_EP_TYPE_BULK,
                    CCID_BULK_EPOUT_SIZE);
      }
      
      break;
      
    }  
    break;
   
  default:
    break;
  }
  return USBD_OK;
}

/**
  * @brief  USBD_CCID_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
uint8_t  USBD_CCID_DataIn (USBD_HandleTypeDef  *pdev, 
                              uint8_t epnum)
{
  CCID_BulkMessage_In(pdev , epnum);
  return USBD_OK;
}

/**
  * @brief  USBD_CCID_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
uint8_t  USBD_CCID_DataOut (USBD_HandleTypeDef  *pdev, 
                               uint8_t epnum, uint8_t* buffer)
{
  CCID_BulkMessage_Out(pdev , epnum, buffer, io_seproxyhal_get_ep_rx_size(CCID_BULK_OUT_EP));
  return USBD_OK;
}

#endif // HAVE_USB_CLASS_CCID

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


