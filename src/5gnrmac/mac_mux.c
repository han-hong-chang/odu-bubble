/*******************************************************************************
################################################################################
#   Copyright (c) [2017-2019] [Radisys]                                        #
#                                                                              #
#   Licensed under the Apache License, Version 2.0 (the "License");            #
#   you may not use this file except in compliance with the License.           #
#   You may obtain a copy of the License at                                    #
#                                                                              #
#       http://www.apache.org/licenses/LICENSE-2.0                             #
#                                                                              #
#   Unless required by applicable law or agreed to in writing, software        #
#   distributed under the License is distributed on an "AS IS" BASIS,          #
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   #
#   See the License for the specific language governing permissions and        #
#   limitations under the License.                                             #
################################################################################
 *******************************************************************************/

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* header include files -- defines (.h) */
#include "envopt.h"        /* environment options */
#include "envdep.h"        /* environment dependent */
#include "envind.h"        /* environment independent */
#include "gen.h"           /* general layer */
#include "ssi.h"           /* system service interface */
#include "cm_hash.h"       /* common hash list */
#include "cm_mblk.h"       /* common memory link list library */
#include "cm_llist.h"      /* common linked list library */
#include "cm_err.h"        /* common error */
#include "cm_lte.h"        /* common LTE */
#include "lrg.h"           /* Layer manager interface includes*/
#include "crg.h"           /* CRG interface includes*/
#include "rgu.h"           /* RGU interface includes*/
#include "tfu.h"           /* TFU interface includes */
#include "rg_sch_inf.h"    /* SCH interface includes */
#include "rg_prg.h"       /* PRG (MAC-MAC) interface includes*/
#include "rg_env.h"       /* MAC environmental includes*/
#include "rg.h"           /* MAC includes*/
#include "rg_err.h"       /* MAC error includes*/
#include "du_log.h"

/* header/extern include files (.x) */
#include "gen.x"           /* general layer typedefs */
#include "ssi.x"           /* system services typedefs */
#include "cm5.x"           /* common timers */
#include "cm_hash.x"       /* common hash list */
#include "cm_lib.x"        /* common library */
#include "cm_llist.x"      /* common linked list */
#include "cm_mblk.x"       /* memory management */
#include "cm_tkns.x"       /* common tokens */
#include "cm_lte.x"       /* common tokens */
#include "rgu.x"           /* RGU types */
#include "tfu.x"           /* RGU types */
#include "lrg.x"           /* layer management typedefs for MAC */
#include "crg.x"           /* CRG interface includes */
#include "rg_sch_inf.x"    /* SCH interface typedefs */
#include "rg_prg.x"        /* PRG (MAC-MAC) Interface typedefs */
#include "du_app_mac_inf.h"
#include "mac.h"
#include "rg.x"            /* typedefs for MAC */

/*******************************************************************
 *
 * @brief pack the bits
 *
 * @details
 *
 *    Function : packBytes
 *
 *    Functionality:
 *     pack the bits in the corresponding byte
 *
 * @params[in] buffer pointer, byte and bit position, value and its size
 * @return void
 *
 * ****************************************************************/
void packBytes(uint8_t *buf, uint8_t *bytePos, uint8_t *bitPos, uint32_t val, uint8_t valSize)
{
   uint32_t  temp;
   uint8_t   bytePart1;
   uint32_t  bytePart2;
   uint8_t   bytePart1Size;
   uint32_t  bytePart2Size;
  
   if(*bitPos - valSize + 1 >= 0)
   {
      bytePart1 = (uint8_t)val;
      bytePart1 = (bytePart1 << (*bitPos -valSize +1));
      buf[*bytePos] |= bytePart1;
      if(*bitPos - valSize < 0)
      {
         *bitPos = 7;
         (*bytePos)++;
      }
      else
         *bitPos -= valSize;
   }
   else
   {
      temp = 0;
      bytePart1Size = *bitPos +1;
      bytePart2Size = valSize - bytePart1Size;

      bytePart1 = (val >> bytePart2Size) << (*bitPos -bytePart1Size +1);
      bytePart2 =  (~((~temp) << bytePart2Size)) & val;
 
      buf[*bytePos] |= bytePart1;
      (*bytePos)++;
      *bitPos = 7;
      packBytes(buf, bytePos, bitPos, bytePart2, bytePart2Size);
   }  
}

/*******************************************************************
 *
 * @brief fill the RAR PDU
 *
 * @details
 *
 *    Function : fillRarPdu
 *
 *    Functionality:
 *     The RAR PDU will be MUXed and formed
 *
 * @params[in] RAR info
 * @return void
 *
 * ****************************************************************/
void fillRarPdu(RarInfo *rarInfo)
{
   uint8_t   *rarPdu = rarInfo->rarPdu;
   uint16_t  totalBits = 0;
   uint8_t   numBytes = 0;
   uint8_t   bytePos= 0;
   uint8_t   bitPos = 0;

   /* RAR subheader fields */
   uint8_t   EBit = 0;
   uint8_t   TBit = 0;
   uint8_t   rapId = 0;

   /* RAR payload fields */
   uint8_t   RBit = 0;
   uint16_t  timeAdv = 0;
   uint32_t  ulGrant = 0;
   uint16_t  tmpCrnti = 0; 
	uint8_t   paddingLcid = 63;

   /* Size(in bits) of RAR subheader files */
   uint8_t   EBitSize = 1;
   uint8_t   TBitSize = 1;
   uint8_t   rapidSize = 6;
	uint8_t   paddingLcidSize = 6;
	uint8_t   paddingSize = 8;


   /* Size(in bits) of RAR payload fields */
   uint8_t   RBitSize = 1;
   uint8_t   timeAdvSize = 12;
   uint8_t   ulGrantSize = 27;
   uint8_t   tmpCrntiSize = 16;

   /* Fill RAR pdu fields */
   EBit = 0;
   TBit = 1;
   rapId = rarInfo->RAPID;
   
   RBit = 0;
   timeAdv = rarInfo->ta;
   ulGrant = 0; /* this will be done when implementing msg3 */ 
   tmpCrnti = rarInfo->tcrnti;

   /* Calulating total number of bytes in buffer */
   totalBits = EBitSize + TBitSize + rapidSize + RBitSize + timeAdvSize \
     + ulGrantSize + tmpCrntiSize;

   /* add padding size */
	totalBits += RBitSize*2 + paddingLcidSize + paddingSize;
   
   /* Calulating total number of bytes in buffer */
   numBytes = totalBits/8;
   if(totalBits % 8)
      numBytes += 1;
    
	rarInfo->rarPduLen = numBytes;

   /* Initialize buffer */
   for(bytePos = 0; bytePos < numBytes; bytePos++)
      rarPdu[bytePos] = 0;
   
   bytePos = 0;
   bitPos = 7;

   /* Packing fields into RAR PDU */
   packBytes(rarPdu, &bytePos, &bitPos, EBit, EBitSize); 
   packBytes(rarPdu, &bytePos, &bitPos, TBit, TBitSize);
   packBytes(rarPdu, &bytePos, &bitPos, rapId, rapidSize);
   packBytes(rarPdu, &bytePos, &bitPos, RBit, RBitSize);
   packBytes(rarPdu, &bytePos, &bitPos, timeAdv, timeAdvSize);
   packBytes(rarPdu, &bytePos, &bitPos, ulGrant, ulGrantSize);
   packBytes(rarPdu, &bytePos, &bitPos, tmpCrnti, tmpCrntiSize);

	/* padding of 2 bytes */
   packBytes(rarPdu, &bytePos, &bitPos, RBit, RBitSize*2);
   packBytes(rarPdu, &bytePos, &bitPos, paddingLcid, paddingLcidSize);
   packBytes(rarPdu, &bytePos, &bitPos, 0, paddingSize);
	
}

/*******************************************************************
 *
 * @brief Database required to form MAC PDU
 *
 * @details
 *
 *    Function : createMacRaCb
 *
 *    Functionality:
 *     stores the required params for muxing
 *
 * @params[in] Pointer to cellId,
 *                        crnti
 * @return void
 *
 * ****************************************************************/
void createMacRaCb(uint16_t cellId, uint16_t crnti)
{
   uint8_t idx = 0; /* supporting 1 UE */
   macCb.macCell->macRaCb[idx].cellId = cellId;
   macCb.macCell->macRaCb[idx].crnti = crnti;
}

/*************************************************
 * @brief fill RLC DL Data
 *
 * @details
 *
 * Function : fillMsg4DlData
 *      This function is a stub which sends Dl Data
 *      to form MAC SDUs
 *           
 * @param[in]  MacDlData *dlData
 ************************************************/

void fillMsg4DlData(MacDlData *dlData)
{
   uint8_t idx = 0;
   dlData->numPdu = 1;
   dlData->pduInfo[idx].lcId = MAC_LCID_CCCH;
   dlData->pduInfo[idx].pduLen = macCb.macCell->macRaCb[0].msg4PduLen;
   memcpy(dlData->pduInfo[idx].dlPdu, macCb.macCell->macRaCb[0].msg4Pdu,\
    macCb.macCell->macRaCb[0].msg4PduLen);
}

/*************************************************
 * @brief fill Mac Ce Info
 *
 * @details
 *
 * Function : fillMacCe
 *      This function fills Mac ce identities
 *           
 * @param[in]  RlcMacData *dlData
 ************************************************/

void fillMacCe(MacCeInfo *macCeInfo)
{
   uint8_t idx;
   macCeInfo->numCes = 1;
   for(idx = 0; idx < macCeInfo->numCes; idx++)
   {
      macCeInfo->macCe[idx].macCeLcid = MAC_LCID_CRI;
      memcpy(&macCeInfo->macCe[idx].macCeValue, \
         &macCb.macCell->macRaCb[idx].msg3Pdu, MAX_CRI_SIZE);
   }
}

/*******************************************************************
 *
 * @brief Forms MAC PDU
 *
 * @details
 *
 *    Function : buildMacPdu
 *
 *    Functionality:
 *     The MAC PDU will be MUXed and formed
 *
 * @params[in] MacDlData *, MacCeInfo *, tbSize
 * @return void
 *
 * ****************************************************************/

void macMuxPdu(MacDlData *dlData, MacCeInfo *macCeData, uint16_t tbSize)
{
   uint8_t bytePos = 0;
   uint8_t bitPos = 7;
   uint8_t idx = 0;
   uint8_t macPdu[tbSize];
   memset(macPdu, 0, (tbSize * sizeof(uint8_t)));

   /* subheader fields */
   uint8_t RBit = 0;              /* Reserved bit */
   uint8_t FBit;                  /* Format Indicator */
   uint8_t lcid;                  /* LCID */
   uint8_t lenField = 0;          /* Length field */

   /* subheader field size (in bits) */
   uint8_t RBitSize = 1;
   uint8_t FBitSize = 1;
   uint8_t lcidSize = 6;
   uint8_t lenFieldSize = 0;          /* 8-bit or 16-bit L field  */
   uint8_t criSize = 8;

   /* PACK ALL MAC CE */
   for(idx = 0; idx < macCeData->numCes; idx++)
   {
      lcid = macCeData->macCe[idx].macCeLcid;
      switch(lcid)
      {
         case MAC_LCID_CRI:
	 {
            /* Packing fields into MAC PDU R/R/LCID */
            packBytes(macPdu, &bytePos, &bitPos, RBit, RBitSize);
            packBytes(macPdu, &bytePos, &bitPos, RBit, RBitSize);
            packBytes(macPdu, &bytePos, &bitPos, lcid, lcidSize);
            memcpy(&macPdu[bytePos], macCeData->macCe[idx].macCeValue,\
            MAX_CRI_SIZE);
            break;
         }
         default:
            DU_LOG("\n MAC: Invalid LCID %d in mac pdu",lcid);
            break;
      }
   }

   /* PACK ALL MAC SDUs */
   for(idx = 0; idx < dlData->numPdu; idx++)
   {
      lcid = dlData->pduInfo[idx].lcId;
      lenField = dlData->pduInfo[idx].pduLen;
      switch(lcid)
      {
         case MAC_LCID_CCCH:
	 {
            if(dlData->pduInfo[idx].pduLen > 255)
            {
               FBit = 1;
               lenFieldSize = 16;

            }
            else
            {
               FBit = 0;
               lenFieldSize = 8;
            }
            /* Packing fields into MAC PDU R/F/LCID/L */
            packBytes(macPdu, &bytePos, &bitPos, RBit, RBitSize);
            packBytes(macPdu, &bytePos, &bitPos, FBit, FBitSize);
            packBytes(macPdu, &bytePos, &bitPos, lcid, lcidSize);
            packBytes(macPdu, &bytePos, &bitPos, lenField, lenFieldSize);
            memcpy(&macPdu[bytePos], dlData->pduInfo[idx].dlPdu, lenField);
            break;
	 }

         default:
            DU_LOG("\n MAC: Invalid LCID %d in mac pdu",lcid);
            break;
      }

   }
   if(bytePos < tbSize && (tbSize-bytePos >= 1))
   {
      /* padding remaining bytes */
      RBitSize = 2;
      lcid = MAC_LCID_PADDING;
      packBytes(macPdu, &bytePos, &bitPos, RBit, RBitSize);
      packBytes(macPdu, &bytePos, &bitPos, lcid, lcidSize);
   }

   MAC_ALLOC(macCb.macCell->macRaCb[0].msg4TxPdu, macCb.macCell->macRaCb[0].msg4TbSize);
   if(macCb.macCell->macRaCb[0].msg4TxPdu != NULLP)
   {
      memcpy(macCb.macCell->macRaCb[0].msg4TxPdu, macPdu,\
         macCb.macCell->macRaCb[0].msg4TbSize);
   }
}

/**********************************************************************
  End of file
 **********************************************************************/
