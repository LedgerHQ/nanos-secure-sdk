
/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2019 Ledger
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/

#pragma once

#define __SC000_REV 0x0001
#define __SC300_REV 0x0001
#define __NVIC_PRIO_BITS 2
#define __Vendor_SysTickConfig 0
typedef enum {
  /******  Cortex-M Processor Exceptions Numbers
     ****************************************************************/
  NonMaskableInt_IRQn = -14,   /*!< 2 Non Maskable Interrupt   */
  HardFault_IRQn = -13,        /*!< 4 Cortex-M Memory Management Interrupt        */
  MemoryManagement_IRQn = -12, /*!< 4 Cortex-M Memory Management Interrupt */
  BusFault_IRQn = -11,         /*!< 5 Cortex-M Bus Fault Interrupt         */
  UsageFault_IRQn = -10,       /*!< 6 Cortex-M Usage Fault Interrupt       */
  SVC_IRQn = -5,               /*!< 11 Cortex-M SV Call Interrupt               */
  DebugMonitor_IRQn = -4,      /*!< 12 Cortex-M Debug Monitor Interrupt      */
  PendSV_IRQn = -2,            /*!< 14 Cortex-M Pend SV Interrupt            */
  SysTick_IRQn = -1,           /*!< 15 Cortex-M System Tick Interrupt           */
} IRQn_Type;
