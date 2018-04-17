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

tSSIHandler ADC_Page_SSI_Handler;
uint32_t ADC_not_configured=1;

extern int index1, numparam;
extern char mass1[20], mass2[20];
extern int i;

char uri1[20];
char http_req[20];
int http_len;
int content_len;
char data[200];




/* we will use character "t" as tag for CGI */
char const* TAGCHAR="t";
char const** TAGS=&TAGCHAR;

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
		char mass[]=	"test data: %d\n"
							"Test2 data: %X\n"
							"Test3 data: %x\n";
		
		char c = i +0x30;

     /* prepare data to be inserted in html */
     //*pcInsert = (char)&"hello_there";
		//*pcInsert = c;
		//memset(pcInsert, c, 20);

		sprintf( pcInsert, mass, i,i,i);


		//snprintf(pcInsert, sizeof(pcInsert), "%s%s", f1, f2);
    
    /* 4 characters need to be inserted in html*/
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
			{mass1[0] = atoi(pcValue[i]); }

			if (strcmp(pcParam[i] , "Var2")==0)  
			{mass1[1] = atoi(pcValue[i]); }
			
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
	memcpy(&uri1, uri,20);
	memcpy(&http_req, http_request,20);
	http_len = http_request_len;
	content_len = content_len;
	

	
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
	
	pbuf_copy_partial(p, data, 200, 0);
	
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
	char mass[] ="/post_done.html";
	int size = sizeof(mass);
	response_uri = mass;
	response_uri_len = size;
	
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
  
  /* configure CGI handlers (LEDs control CGI) */
  CGI_TAB[0] = LEDS_CGI;
  http_set_cgi_handlers(CGI_TAB, 1);  
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
