/**
  ******************************************************************************
  * @file    LwIP/LwIP_HTTP_Server_Raw/Src/httpd_cg_ssi.c
  * @author  MCD Application Team
  * @brief   Webserver SSI and CGI handlers
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lwip/debug.h"
#include "lwip/tcp.h"
#include "lwip/apps/httpd.h"
#include "http_server.h"


#include <string.h>
#include <stdlib.h>

#include "flash_if.h"

tSSIHandler ADC_Page_SSI_Handler;
uint32_t ADC_not_configured=1;

extern int index1, numparam;
extern char mass1[20], mass2[20];

uint32_t temp [5];


char uri1[20];
char http_req[20];
int http_len;
int size;



	typedef struct
			{
		uint32_t EndOffset;
		uint32_t DataOffset;
		uint32_t TotalContent;
		uint32_t FirstPacketFlag;
		uint32_t TotalData;
		uint32_t WriteOffset;
		uint8_t buffer;
	}BldrPostTypeDef;
		
	BldrPostTypeDef Bldr_post = {0};
	
	uint32_t firmware_size =0;
	

	
enum
{
	Not_bin_file,
	Size_too_big,
	Success,
} POST_status;

int post_size = 0;

static const char http_crnl_2[4] =
/* "\r\n--" */
{0xd, 0xa,0x2d,0x2d};
static const char octet_stream[14] =
/* "octet-stream" */
{0x6f, 0x63, 0x74, 0x65, 0x74, 0x2d, 0x73, 0x74, 0x72, 0x65, 0x61, 0x6d,0x0d, };




/* we will use character "t" as tag for CGI */
char const* TAGCHAR="t";
char const** TAGS=&TAGCHAR;

char const* POST_tag="r";
char const** TAGS2=&POST_tag;
/* CGI handler for LED control */ 
const char * LEDS_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
u16_t ADC_Handler(int iIndex, char *pcInsert, int iInsertLen);
void httpd_ssi_init(void);
void httpd_cgi_init(void);

/* Html request for "/leds.cgi" will start LEDS_CGI_Handler */
const tCGI LEDS_CGI={"/leds.cgi", LEDS_CGI_Handler};

/* Cgi call table, only one CGI used */
tCGI CGI_TAB[1];

/**
  * @brief  Configures the ADC.
  * @param  None
  * @retval None
  */


/**
  * @brief  ADC_Handler : SSI handler for ADC page 
  */
u16_t ADC_Handler(int iIndex, char *pcInsert, int iInsertLen)
{
  /* We have only one SSI handler iIndex = 0 */
  if (iIndex ==0)
  {  
     /* get digits to display */

//		char c = i +0x30;

//     /* prepare data to be inserted in html */
//     //*pcInsert = (char)&"hello_there";
//		//*pcInsert = c;
//		//memset(pcInsert, c, 20);

//		sprintf( pcInsert, mass, i,i,i);


		//snprintf(pcInsert, sizeof(pcInsert), "%s%s", f1, f2);
    
    /* 4 characters need to be inserted in html*/
    return strlen(pcInsert);
  }
  return 0;
}


u16_t Post_result(int iIndex, char *pcInsert, int iInsertLen)
{
	  if (iIndex ==0)
  {  
		if ( POST_status == Not_bin_file)
		{
			sprintf( pcInsert, "NOT BIN FILE");
			return strlen(pcInsert);
		}
		else if ( POST_status == Success)
		{
			sprintf( pcInsert, "Successfully downloaded %d bytes", firmware_size);
			firmware_size = 0;
			return strlen(pcInsert);
		}
		

		
		char data[]=	"status";
		sprintf( pcInsert, data);
		
    return strlen(pcInsert);
  }
  return 0;
	
}

/**
  * @brief  CGI handler for LEDs control 
  */
const char * LEDS_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
  uint32_t i=0;
	
	index1 = iIndex;
	numparam = iNumParams;
	
	memset(mass1, 0, 20);
	memset(mass2, 0, 20);

	for (i=0; i<iNumParams; i++)
	{

			if (strcmp(pcParam[i] , "Var1")==0)
			{temp[0] = atoi(pcValue[i]); }

			if (strcmp(pcParam[i] , "Var2")==0)  
			{temp[1] = atoi(pcValue[i]); }
			
			      if (strcmp(pcParam[i] , "led")==0)
						{
							mass2[i] = atoi(pcValue[i]);
						}
			
		}


  
  /* We have only one SSI handler iIndex = 0 */
  if (iIndex==0)
  {

		
  }
  /* uri to send after cgi call*/
  return "/tx_data.html";  
}

/* These functions must be implemented by the application */

/** Called when a POST request has been received. The application can decide
 * whether to accept it or not.
 *
 * @param connection Unique connection identifier, valid until httpd_post_end
 *        is called.
 * @param uri The HTTP header URI receiving the POST request.
 * @param http_request The raw HTTP request (the first packet, normally).
 * @param http_request_len Size of 'http_request'.
 * @param content_len Content-Length from HTTP header.
 * @param response_uri Filename of response file, to be filled when denying the
 *        request
 * @param response_uri_len Size of the 'response_uri' buffer.
 * @param post_auto_wnd Set this to 0 to let the callback code handle window
 *        updates by calling 'httpd_post_data_recved' (to throttle rx speed)
 *        default is 1 (httpd handles window updates automatically)
 * @return ERR_OK: Accept the POST request, data may be passed in
 *         another err_t: Deny the POST request, send back 'bad request'.
 */
err_t httpd_post_begin(void *connection, const char *uri, const char *http_request,
                       u16_t http_request_len, int content_len, char *response_uri,
                       u16_t response_uri_len, u8_t *post_auto_wnd)

{
	Bldr_post.TotalContent = content_len;
	//if too big then ssi it
	
	return ERR_OK;
}

/** Called for each pbuf of data that has been received for a POST.
 * ATTENTION: The application is responsible for freeing the pbufs passed in!
 *
 * @param connection Unique connection identifier.
 * @param p Received data.
 * @return ERR_OK: Data accepted.
 *         another err_t: Data denied, http_post_get_response_uri will be called.
 */
err_t httpd_post_receive_data(void *connection, struct pbuf *p)
{
	
	
	//parse for DataOffset and EndOffset if packet_num = 0
	if ( Bldr_post.FirstPacketFlag == 0)
	{
    /* parse packet for the octet-stream field */
    for (int i=0; i < p->tot_len; i++)
    {
      if (strncmp ((char*)(p->payload) +i, octet_stream, 13)==0)
      {
        Bldr_post.DataOffset = i+16; //16 cause of /r/n
        break;
      }
    }
    
    //parse for end (1 pack only)
    for (int i=4; i < p->tot_len; i++)
    {
      if (strncmp ((char*)(p->payload) + p->tot_len -i,http_crnl_2 , 4)==0)
      {
        Bldr_post.EndOffset = p->len - i;
        Bldr_post.TotalData += Bldr_post.EndOffset - Bldr_post.DataOffset;
        break;
      }
    }
    
    //first pack with no end
    if (Bldr_post.EndOffset == 0  && Bldr_post.DataOffset != 0)
    {
      Bldr_post.FirstPacketFlag = 1;
      Bldr_post.TotalData = p->tot_len - Bldr_post.DataOffset;
      
      FLASH_If_Init();
      FLASH_If_Erase(FLASH_USER_START_ADDR, FLASH_USER_END_ADDR);
      
      while (Bldr_post.WriteOffset != p->tot_len - Bldr_post.DataOffset)
      {
        pbuf_copy_partial(p, &Bldr_post.buffer, 1 ,Bldr_post.WriteOffset + Bldr_post.DataOffset);
        FLASH_If_Write(FLASH_USER_START_ADDR + Bldr_post.WriteOffset, &Bldr_post.buffer ,1);
        Bldr_post.WriteOffset++;
      }
      
      
    }
    //octet stream not found, not bin file
    else if ( Bldr_post.DataOffset == 0)
    {
      memset(&Bldr_post, 0, sizeof(Bldr_post));
      POST_status = Not_bin_file;
      
      //return status page
      pbuf_free(p);
      return 1;
      
    }
    
    //end was found, all data in 1 pack
    else if (Bldr_post.EndOffset != 0  && Bldr_post.DataOffset != 0)
    {
      
      FLASH_If_Init();
      FLASH_If_Erase(FLASH_USER_START_ADDR, FLASH_USER_END_ADDR);
      
      while (Bldr_post.WriteOffset != Bldr_post.EndOffset - Bldr_post.DataOffset)
      {
        pbuf_copy_partial(p, &Bldr_post.buffer, 1 ,Bldr_post.WriteOffset + Bldr_post.DataOffset);
        FLASH_If_Write(FLASH_USER_START_ADDR + Bldr_post.WriteOffset, &Bldr_post.buffer ,1);
        Bldr_post.WriteOffset++;
      }
      
      POST_status = Success;
      firmware_size = Bldr_post.TotalData;
      memset(&Bldr_post, 0, sizeof(Bldr_post));
      
      pbuf_free(p);
      return ERR_OK;
    }
	}
	
  
  // >1 packs
  else if ( Bldr_post.FirstPacketFlag == 1)
  {
    
    for (int i=4; i < p->tot_len; i++)
    {
      //looking for end
      if (strncmp ((char*)(p->payload) + p->tot_len -i,http_crnl_2 , 4)==0)
      {
        Bldr_post.EndOffset = p->tot_len - i;
        
        while (Bldr_post.WriteOffset != Bldr_post.TotalData + Bldr_post.EndOffset)
        {
          pbuf_copy_partial(p, &Bldr_post.buffer, 1 ,Bldr_post.WriteOffset - Bldr_post.TotalData);
          FLASH_If_Write(FLASH_USER_START_ADDR + Bldr_post.WriteOffset, &Bldr_post.buffer ,1);
          Bldr_post.WriteOffset++;
        }
        
        Bldr_post.TotalData = Bldr_post.WriteOffset;
        
        
        POST_status = Success;
        firmware_size = Bldr_post.TotalData;
        memset(&Bldr_post, 0, sizeof(Bldr_post));
        break;
      }
    }
    
    //just copy full pack
    if (Bldr_post.EndOffset == 0  && Bldr_post.DataOffset != 0)
    {
      while (Bldr_post.WriteOffset != Bldr_post.TotalData + p->tot_len)
      {
        pbuf_copy_partial(p, &Bldr_post.buffer, 1 ,Bldr_post.WriteOffset - Bldr_post.TotalData);
        FLASH_If_Write(FLASH_USER_START_ADDR + Bldr_post.WriteOffset, &Bldr_post.buffer ,1);
        Bldr_post.WriteOffset++;
      }
      Bldr_post.TotalData += p->tot_len;
    }
  }
  
  
	pbuf_free(p);
	return ERR_OK;
	
}


/** Called when all data is received or when the connection is closed.
 * The application must return the filename/URI of a file to send in response
 * to this POST request. If the response_uri buffer is untouched, a 404
 * response is returned.
 *
 * @param connection Unique connection identifier.
 * @param response_uri Filename of response file, to be filled when denying the request
 * @param response_uri_len Size of the 'response_uri' buffer.
 */
void httpd_post_finished(void *connection, char *response_uri, u16_t response_uri_len)
{
	char mass[] ="/post_done.shtml";
	
	response_uri_len = sizeof(mass);
        strncpy(response_uri, mass, response_uri_len);


	
}





/**
  * @brief  Http webserver Init
  */
void http_server_init(void)
{
  /* Httpd Init */
  httpd_init();
  
  /* configure SSI handlers (ADC page SSI) */
  http_set_ssi_handler(ADC_Handler, (char const **)TAGS, 1);
	http_set_ssi_handler(Post_result, (char const **)TAGS2, 1);
  
  /* configure CGI handlers (LEDs control CGI) */
  CGI_TAB[0] = LEDS_CGI;
  http_set_cgi_handlers(CGI_TAB, 1);  
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
