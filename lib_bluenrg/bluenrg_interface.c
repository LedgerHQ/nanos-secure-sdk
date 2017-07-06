/**
  ******************************************************************************
  * @file    bluenrg_interface.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    04-July-2014
  * @brief   This file provides code for the BlueNRG Shield driver
  *          based on STM32Cube HAL for STM32 Nucleo boards.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "hal.h"
#include "hci.h"

#include "ble_status.h"
#include "hci.h"
#include "gp_timer.h"
#include "os_io_seproxyhal.h"

/**
 * @brief  Writes data to a serial interface.
 * @param  data1   :  1st buffer
 * @param  data2   :  2nd buffer
 * @param  n_bytes1: number of bytes in 1st buffer
 * @param  n_bytes2: number of bytes in 2nd buffer
 * @retval None
 */
void Hal_Write_Serial(const void* data1, const void* data2, uint16_t n_bytes1,
                      uint16_t n_bytes2)
{
  G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_BLUENRG_SEND;
  G_io_seproxyhal_spi_buffer[1] = (1+n_bytes1+n_bytes2)>>8;
  G_io_seproxyhal_spi_buffer[2] = (1+n_bytes1+n_bytes2);
  G_io_seproxyhal_spi_buffer[3] = 0x3; // start before and stop after
  io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 3+1);
  if (data1 && n_bytes1) {
    io_seproxyhal_spi_send(data1, n_bytes1);
  }
  if (data2 && n_bytes2) {
    io_seproxyhal_spi_send(data2, n_bytes2);
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
