/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "crc.h"
#include "fatfs.h"
#include "i2c.h"
#include "rtc.h"
#include "sdio.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"
#include "iic.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "key.h"
#include "dht11.h"
#include "stdio.h"
#include "rtc.h"
#include "BH1750.h"
#include "i2c.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

  uint8_t s1 = 1;			//主页面功能号
  uint8_t s2 = 0;			//主次页面判断标志位
  uint8_t s3 = 1;		//次页面功能号 
  uint8_t s4 = 1;
  uint8_t s5 = 0;
  uint8_t flag;
  uint8_t flag_storage = 0;
  uint8_t flag_tem = 1;
  uint8_t flag_hum = 2;
  uint8_t flag_con = 3;
  uint8_t flag_light = 4;
	uint16_t temperature;
	uint16_t humidity;
  uint16_t temp2;
  uint16_t AD_Value;
  uint32_t byteswritten;              
  uint8_t lx_Integer;
  uint8_t lx_Decimal;
  uint8_t ppm_Integer;
  uint8_t ppm_Decimal;
  uint16_t flag_conup = 1000;
  uint8_t flag_conlow = 0;
  uint16_t flag_lightup = 5000;
  uint8_t flag_lightlow = 0;
  uint8_t flag_humup = 95;
  uint8_t flag_humlow = 5;

  float ppm;
  float lx;
  float test_tem;
  float flag_temup = 34.0;
  float flag_temlow = 10.0;
	float Vol_Value;
  
  char SensorBuff[100];
  char tem_show[50];
  char hum_show[50];
  char ppm_show[50];
  char lux_show[50];
  char date_show[50];
  char time_show[50];
  RTC_DateTypeDef GetDate;
  RTC_TimeTypeDef GetTime;
  RTC_AlarmTypeDef sAlarm;  
  
  extern uint8_t r_s;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void SD_Open(void);
void Function_Primary(void);
void Key_function(void);
void Date_Show(void);
void SD_Show(void);
void function(void);
void key_scan_up(void);
void key_scan_down(void);
void test(void);
void Real_Time_Storage(void);
void Threshold_setting(void);
void Threshold_Temperature(void);
void Threshold_Humidity(void);
void Device_init(void);
void Alarm_Display(void);
void Threshold_Concentration(void);
void Collection_Path(void);
void Set_Tem_Path(void);
void Set_Hum_Path(void);
void Set_Con_Path(void);
void Set_Light_Path(void);
void Collection_Path_Page(void);
void Threshold_Lgiht(void);
void CD4051(uint8_t n);
void Polling_CD4051(void);


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SDIO_SD_Init();
  MX_USART1_UART_Init();
  MX_FATFS_Init();
  MX_CRC_Init();
  MX_FSMC_Init();
  MX_TIM3_Init();
  MX_TIM7_Init();
  MX_RTC_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init();
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,0);//inh

  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,0);
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_TIM_Base_Start_IT(&htim4);
  rtc_init_user();
  IIC_Init();
  SD_Open();
  DHT11_Read_Data(&temperature,&humidity);
  // CD4051(0);
  // HAL_ADC_Start_DMA(&hadc1, Ad_temp, 100);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    // Polling_CD4051();
    Device_init();
    function();
    key_scan_down();
    key_scan_up();
    test();
    printf("%d",flag);
    printf("Temperature = %d.%d degree\r\n",temperature>>8,temperature&0xff);

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim == &htim3)
  {
    RTC_Get();
       /* 打开文件 */
    f_open(&SDFile,"0:/Date.CSV",FA_OPEN_EXISTING | FA_WRITE);
 
    /* 查找文件的结尾 */
    f_lseek(&SDFile,f_size(&SDFile));
    /* 写入数据 */
    sprintf(SensorBuff, "%d.%d,%d.%d,%d.%d,%d.%d,%02d/%02d/%02d,%02d:%02d:%02d\r\n",temperature>>8,temperature&0xff,
    humidity>>8,humidity&0xff,ppm_Integer,ppm_Decimal,lx_Integer,lx_Decimal,calendar.w_year, calendar.w_month, calendar.w_date,calendar.hour, 
    calendar.min, calendar.sec);
    f_write(&SDFile,SensorBuff,strlen(SensorBuff),&byteswritten);
    printf("\r\nsuccess\r\n");
 
    /* 关闭文件 */
    f_close(&SDFile);
  }

  if(htim == &htim4)
  {

    RTC_Get();
    flag = DHT11_Read_Data(&temperature,&humidity);

  }
}

//监控数据
void Date_Show(void)
{
  sprintf((char*)tem_show,"Temperature = %d.%d degree",temperature>>8,temperature&0xff);
  sprintf((char*)hum_show,"Humidity = %d.%d%%",humidity>>8,humidity&0xff);
  sprintf((char*)ppm_show,"Concentration = %d.%d ppm",ppm_Integer,ppm_Decimal);
  sprintf((char*)lux_show,"Light intensity = %d.%d lx",lx_Integer,lx_Decimal);
  POINT_COLOR = BLUE;
  LCD_ShowString(50,20,240,200,16,"Monitoring Data");
  POINT_COLOR = BLACK;
  LCD_ShowString(10,60,240,200,16,tem_show);
  LCD_ShowString(10,100,240,200,16,hum_show);
  LCD_ShowString(10,140,240,200,16,ppm_show);
  LCD_ShowString(10,180,240,200,16,lux_show);
}

//显示SD卡信息函数
void SD_Show(void){
  POINT_COLOR = BLUE;
  LCD_ShowString(50,20,200,240,16,"Detection device");
  POINT_COLOR = BLACK;
  LCD_ShowString(10,60,200,240,16,"Temperature equipment");
  LCD_ShowString(10,100,200,240,16,"Humidity equipment");
  LCD_ShowString(10,140,200,240,16,"Concentration equipment");
  LCD_ShowString(10,180,200,240,16,"Lighting equipment");

  HAL_SD_CardCIDTypeDef SDCard_CID;
  HAL_SD_CardInfoTypeDef SDCard_INFO;
  uint64_t CardCap;			//SD卡容量

  // LCD_DrawRectangle(25,25,215,135);
  // POINT_COLOR=RED;					
  // LCD_ShowString(30,30,200,16,16,(uint8_t *)"ManufacturerID:%d\r\n",SDCard_CID.ManufacturerID);	
  // LCD_ShowString(30,50,200,16,16,(uint8_t *)"Card RCA:%d\r\n",SDCard_INFO.RelCardAdd);	
  // LCD_ShowString(30,70,200,16,16,(uint8_t *)"LogBlockNbr:%d\r\n",SDCard_INFO.LogBlockNbr);
  // LCD_ShowString(30,90,200,16,16, (uint8_t *)"LogBlockSize:%d\r\n",SDCard_INFO.LogBlockSize);

  LCD_ShowString(30,230,200,16,16,(uint8_t *)"SD Card Information");
  LCD_ShowString(30,250,80,16,16,(uint8_t *)"Card Type:     ");
  LCD_ShowString(30,270,80,16,16,(uint8_t *)"Card Capa:");


  HAL_SD_GetCardCID(&hsd,&SDCard_CID);	//获取CID
  HAL_SD_GetCardInfo(&hsd,&SDCard_INFO);//获取SD卡信息
  switch(SDCard_INFO.CardType)		//SD卡类型
  {
    case CARD_SDSC:
      if(SDCard_INFO.CardVersion == CARD_V1_X){
        LCD_ShowString(120,250,50,16,16,(uint8_t *)"SDSC V1");
        // printf("Card Type: SDSC V1\r\n");
      }
      else if(SDCard_INFO.CardVersion == CARD_V2_X){
        LCD_ShowString(120,250,50,16,16,(uint8_t *)"SDSC V2");
        // printf("Card Type: SDSC V2\r\n");
      }
      break;
    case CARD_SDHC_SDXC:
      LCD_ShowString(120,250,50,16,16,(uint8_t *)"SDHC");
      // printf("Card Type: SDHC\r\n");
      break;
  }
	
  CardCap = (uint64_t)(SDCard_INFO.LogBlockNbr)*(uint64_t)(SDCard_INFO.LogBlockSize)/1024/1024;	//计算SD卡容量
  // printf("Card ManufacturerID:%d\r\n",SDCard_CID.ManufacturerID);//制造商ID
  // printf("Card RCA:%d\r\n",SDCard_INFO.RelCardAdd);	//卡相对地址 
  // printf("LogBlockNbr:%d\r\n",SDCard_INFO.LogBlockNbr);	//逻辑块数量
  // printf("LogBlockSize:%d\r\n",SDCard_INFO.LogBlockSize);//逻辑块大小
  // printf("Card Capacity:%d MB\r\n",(uint32_t)CardCap);	//显示容量
  // printf("Card BlockSize:%d\r\n",SDCard_INFO.BlockSize);//块大小
  LCD_ShowNum(120,270,(uint32_t)CardCap,5,16);
  LCD_ShowString(160,270,50,16,16,(uint8_t *)"MB");
  if(flag == 0)
  {
    LCD_Fill(200,60,220,80,GREEN);
    LCD_Fill(200,100,220,120,GREEN);
  }else{
    LCD_Fill(200,60,220,80,RED);
    LCD_Fill(200,100,220,120,RED);
  }
  
  if((AD_Value > 3000)&(AD_Value < 3400))
  {
    LCD_Fill(200,140,220,160,RED);
  }else{
    LCD_Fill(200,140,220,160,GREEN);
  }

  if(r_s == 0)
  {
    LCD_Fill(200,180,220,200,GREEN);
  }else{
    LCD_Fill(200,180,220,200,RED);
  }
  // HAL_Delay(2000);
}

//开启SD卡
void SD_Open(void)
{
  /*注册一个工作区域*/
    f_mount(&SDFatFS,SDPath,1);			   
    
    /* 打开创建一个新文件 */
    f_open(&SDFile,(const char*)"0:/Date.CSV",FA_CREATE_NEW | FA_WRITE);  
 
    /* 写入标题数据 */
    sprintf(SensorBuff, "Temperature,Humidity,ppm,Light,Date,Time\r\n");
    f_write(&SDFile,SensorBuff,strlen(SensorBuff),&byteswritten);
    printf("\r\n!!!\r\n");
 
    /* 关闭文件 */
    f_close(&SDFile);
}

//主页面
void Function_Primary(void)
{
  RTC_Get();
  sprintf((char*)date_show,"%02d/%02d/%02d",calendar.w_year, calendar.w_month, calendar.w_date);
  sprintf((char*)time_show,"%02d:%02d:%02d",calendar.hour, calendar.min, calendar.sec);
  POINT_COLOR = BLUE;
  LCD_ShowString(25,10,240,200,16,"Data Collection Systems");
  POINT_COLOR = BLACK;
  LCD_ShowString(20,50,240,200,16,"Real-time Monitoring");
  LCD_ShowString(20,90,240,200,16,"Device inspection");
  LCD_ShowString(20,130,240,200,16,"Real-time storage");
  LCD_ShowString(20,170,240,200,16,"Threshold setting");
  LCD_ShowString(70,220,240,200,16,date_show);
  LCD_ShowString(70,240,240,200,16,time_show);
  switch (s1)
      {
        case 1:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,60,5);
          
         break;

        case 2:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,100,5);
         break;
         
        case 3:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,140,5);
          break;

        case 4:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,180,5);
          break;

        default:
         break;
      }
  
}

//确定键与返回键
void function(void)			
{	
	if(HAL_GPIO_ReadPin(Key3_GPIO_Port,Key3_Pin)==0)    //确定键
	{
		HAL_Delay(50);
	  if(HAL_GPIO_ReadPin(Key3_GPIO_Port,Key3_Pin) == 0)
		{	
      if(s2 == 13)   //光照路径选择  
      {
        switch (s4)
        {
        case 1:
          flag_light = 1;
          if(flag_light == flag_tem)
            flag_tem = 0;
          else if(flag_light == flag_hum)
            flag_hum = 0;
          else if(flag_light == flag_con)
            flag_con = 0;
          break;

        case 2:
          flag_light = 2;
          if(flag_light == flag_tem)
            flag_tem = 0;
          else if(flag_light == flag_hum)
            flag_hum = 0;
          else if(flag_light == flag_con)
            flag_con = 0;
          break;
        
        case 3:
          flag_light = 3;
          if(flag_light == flag_tem)
            flag_tem = 0;
          else if(flag_light == flag_hum)
            flag_hum = 0;
          else if(flag_light == flag_con)
            flag_con = 0;
          break; 

        case 4:
          flag_light = 4;
          if(flag_light == flag_tem)
            flag_tem = 0;
          else if(flag_light == flag_hum)
            flag_hum = 0;
          else if(flag_light == flag_con)
            flag_con = 0;
          break;       
        
        default:
          break;
        }
      }

      if(s2 == 12)   //浓度路径选择  
      {
        switch (s4)
        {
        case 1:
          flag_con = 1;
          if(flag_con == flag_tem)
            flag_tem = 0;
          else if(flag_con == flag_hum)
            flag_hum = 0;
          else if(flag_con == flag_light)
            flag_light = 0;
          break;

        case 2:
          flag_con = 2;
          if(flag_con == flag_tem)
            flag_tem = 0;
          else if(flag_con == flag_hum)
            flag_hum = 0;
          else if(flag_con == flag_light)
            flag_light = 0;
          break;
        
        case 3:
          flag_con = 3;
          if(flag_con == flag_tem)
            flag_tem = 0;
          else if(flag_con == flag_hum)
            flag_hum = 0;
          else if(flag_con == flag_light)
            flag_light = 0;
          break; 

        case 4:
          flag_con = 4;
          if(flag_con == flag_tem)
            flag_tem = 0;
          else if(flag_con == flag_hum)
            flag_hum = 0;
          else if(flag_con == flag_light)
            flag_light = 0;
          break;       
        
        default:
          break;
        }
      }
      
      if(s2 == 11)   //湿度路径选择  
      {
        switch (s4)
        {
        case 1:
          flag_hum = 1;
          if(flag_hum == flag_tem)
            flag_tem = 0;
          else if(flag_hum == flag_con)
            flag_con = 0;
          else if(flag_hum == flag_light)
            flag_light = 0;
          break;

        case 2:
          flag_hum = 2;
          if(flag_hum == flag_tem)
            flag_tem = 0;
          else if(flag_hum == flag_con)
            flag_con = 0;
          else if(flag_hum == flag_light)
            flag_light = 0;
          break;
        
        case 3:
          flag_hum = 3;
          if(flag_hum == flag_tem)
            flag_tem = 0;
          else if(flag_hum == flag_con)
            flag_con = 0;
          else if(flag_hum == flag_light)
            flag_light = 0;
          break; 

        case 4:
          flag_hum = 4;
          if(flag_hum == flag_tem)
            flag_tem = 0;
          else if(flag_hum == flag_con)
            flag_con = 0;
          else if(flag_hum == flag_light)
            flag_light = 0;
          break;       
        
        default:
          break;
        }
      }

      if(s2 == 10)   //温度路径选择  
      {
        switch (s4)
        {
        case 1:
          flag_tem = 1;
          if(flag_tem == flag_hum)
            flag_hum = 0;
          else if(flag_tem == flag_con)
            flag_con = 0;
          else if(flag_tem == flag_light)
            flag_light = 0;
          break;

        case 2:
          flag_tem = 2;
          if(flag_tem == flag_hum)
            flag_hum = 0;
          else if(flag_tem == flag_con)
            flag_con = 0;
          else if(flag_tem == flag_light)
            flag_light = 0;
          break;
        
        case 3:
          flag_tem = 3;
          if(flag_tem == flag_hum)
            flag_hum = 0;
          else if(flag_tem == flag_con)
            flag_con = 0;
          else if(flag_tem == flag_light)
            flag_light = 0;
          break; 

        case 4:
          flag_tem = 4;
          if(flag_tem == flag_hum)
            flag_hum = 0;
          else if(flag_tem == flag_con)
            flag_con = 0;
          else if(flag_tem == flag_light)
            flag_light = 0;
          break;       
        
        default:
          break;
        }
      }

      if(s2 == 9)   //路径设置
      {
        switch (s4)
        {
        case 1:
          LCD_Clear(WHITE);
          s2 = 10;
          s4 = 1;
          break;

        case 2:
          LCD_Clear(WHITE);
          s2 = 11;
          s4 = 1;
          break;
        
        case 3:
          LCD_Clear(WHITE);
          s2 = 12;
          s4 = 1;
          break; 

        case 4:
          LCD_Clear(WHITE);
          s2 = 13;
          s4 = 1;
          break;       
        
        default:
          break;
        }
      }

      if(s2 == 7)   //浓度阈值设置
      {
        switch(s4)
        {
          case 1:
            flag_conup += 5;
            break;
          case 2:
            flag_conup -= 5;
            break;

          case 3:
            flag_conlow += 5;
            break;

          case 4:
            flag_conlow -= 5;
            if(flag_conlow < 0)
            flag_conlow = 0;
            break;

            default:
								break;
        }
      }

      if(s2 == 5)   //温度阈值设置
      {
        switch(s4)
        {
          case 1:
            flag_temup += 0.1;
            break;
          case 2:
            flag_temup -= 0.1;
            break;

          case 3:
            flag_temlow += 0.1;
            break;

          case 4:
            flag_temlow -= 0.1;
            if(flag_temlow < 0)
            flag_temlow = 0;
            break;

            default:
								break;
        }
      }

      if(s2 == 6)   //湿度阈值设置
      {
        switch(s4)
        {
          case 1:
            flag_humup += 5;
            break;
            if(flag_temlow > 100)
            flag_temlow = 100;
          case 2:
            flag_humup -= 5;
            break;

          case 3:
            flag_humlow += 5;
            break;

          case 4:
            flag_humlow -= 5;
            if(flag_temlow < 0)
            flag_temlow = 0;
            break;

            default:
								break;
        }
      }

      if(s2 == 4)   //阈值设置选择
      {
        switch(s4)
        {
          case 1:
            LCD_Clear(WHITE);
            s2 = 5;
            s4 = 1;
            // Threshold_Temperature();
            break;

          case 2:
            LCD_Clear(WHITE);
            s2 = 6;
            s4 = 1;
            // Threshold_Humidity();
            break;

          case 3:
            LCD_Clear(WHITE);
            s2 = 7;
            s4 = 1;
            // Threshold_Humidity();
            break;

          case 4:
            LCD_Clear(WHITE);
            s2 = 14;
            s4 = 1;
            break;

          default:
								break;
        }
      }

      if(s2 == 3)   //SD装配卸载功能选择
      {
        switch (s3)
        {
          case 1:
            flag_storage = 0;
          break;

          case 2:
            flag_storage = 1;
            break;

          case 3:
            LCD_Clear(WHITE);
            s2 = 9;
            s4 = 1;   
          default:
            break;
        }
      }

			if(s2 == 0)		//主界面功能选择
			{
				switch(s1)		
					{
						case 1: LCD_Clear(WHITE);
								    s2 = 1;
                    // Date_Show();
								    break;
						case 2: LCD_Clear(WHITE);
							    	s2 = 2;
                    flag = 0;
                    // SD_Show();
								    break;
						case 3: LCD_Clear(WHITE);
                    s3 = 1;
								    s2 = 3;
                    // Real_Time_Storage();
                    break;
            case 4: LCD_Clear(WHITE);
                    s2 = 4;
                    // Threshold_setting();
								    break;
						
						default:
								break;
					}
      }
    }
  }

  if(HAL_GPIO_ReadPin(key4_GPIO_Port,key4_Pin)==1)    //返回键
	{
		HAL_Delay(50);
		if(HAL_GPIO_ReadPin(key4_GPIO_Port,key4_Pin) == 1)
		{	
      switch(s2)
      {
        case 1:
          LCD_Clear(WHITE);
          s2 = 0;
          // Function_Primary();
          break;
        case 2:
          LCD_Clear(WHITE);
          s2 = 0;
          // Function_Primary();
          break;
        case 3:
          LCD_Clear(WHITE);
          s2 = 0;
          // Function_Primary();
          break;
        case 4:
          LCD_Clear(WHITE);
          s2 = 0;
          // Function_Primary();
          break;
        case 5:
          LCD_Clear(WHITE);
          s2 = 4;
          s4 = 1;
          // Threshold_setting();
          break;
        case 6:
          LCD_Clear(WHITE);
          s2 = 4;
          s4 = 1;
          // Threshold_setting();
          break;
        case 7:
          LCD_Clear(WHITE);
          s2 = 4;
          s4 = 1;
          // Threshold_setting();
          break;

        case 9:
          LCD_Clear(WHITE);
          s2 = 3;
          s4 = 1;
          break;

        case 10:
          LCD_Clear(WHITE);
          s2 = 9;
          s4 = 1;
          break;

        case 11:
          LCD_Clear(WHITE);
          s2 = 9;
          s4 = 1;
          break;

        case 12:
          LCD_Clear(WHITE);
          s2 = 9;
          s4 = 1;
          break;

        case 13:
          LCD_Clear(WHITE);
          s2 = 9;
          s4 = 1;
          break;

        case 14:
          LCD_Clear(WHITE);
          s2 = 4;
          s4 = 1;
          break;
  
          default:
            break;
      }
     
    }
  }
}
  
//功能切换上建
void key_scan_up(void)		
{
	if(HAL_GPIO_ReadPin(Key2_GPIO_Port,Key2_Pin) == 0)
	{
		switch(s2)		//判断是主界面中哪个功能的次界面
		{
			case 0:
			HAL_Delay(50);
			if(HAL_GPIO_ReadPin(Key2_GPIO_Port,Key2_Pin) == 0)
			{
				
				s1+=1;
			}
			if(s1 > 4)
				s1 = 1;
			else
				s1 = s1;
      // Function_Primary();
			break;
			
			case 3:
			HAL_Delay(50);
			if(HAL_GPIO_ReadPin(Key2_GPIO_Port,Key2_Pin) == 0)
			{
				
				s3+=1;
			}
			if(s3 > 3)
				s3 = 1;
			else
				s3 = s3;
      // Real_Time_Storage();

			break;
      
      case 4:
        HAL_Delay(50);
			  if(HAL_GPIO_ReadPin(Key2_GPIO_Port,Key2_Pin) == 0)
		  	{
				  s4 += 1;
			  }
			  if(s4 > 4)
				  s4 = 1;
			  else
				  s4 = s4;
        // Threshold_setting();
		  	break;

      case 5:
			HAL_Delay(50);
			if(HAL_GPIO_ReadPin(Key2_GPIO_Port,Key2_Pin) == 0)
			{
				s4 += 1;
			}
			if(s4 > 4)
				s4 = 1;
			else
				s4 = s4;
      // Threshold_Temperature();
			break;

      case 6:
			HAL_Delay(50);
			if(HAL_GPIO_ReadPin(Key2_GPIO_Port,Key2_Pin) == 0)
			{
				s4 += 1;
			}
			if(s4 > 4)
				s4 = 1;
			else
				s4 = s4;
			break;

      case 9:
			HAL_Delay(50);
			if(HAL_GPIO_ReadPin(Key2_GPIO_Port,Key2_Pin) == 0)
			{
				s4 += 1;
			}
			if(s4 > 4)
				s4 = 1;
			else
				s4 = s4;
			break;

      case 10:
			HAL_Delay(50);
			if(HAL_GPIO_ReadPin(Key2_GPIO_Port,Key2_Pin) == 0)
			{
				s4 += 1;
			}
			if(s4 > 4)
				s4 = 1;
			else
				s4 = s4;
			break;

      case 11:
			HAL_Delay(50);
			if(HAL_GPIO_ReadPin(Key2_GPIO_Port,Key2_Pin) == 0)
			{
				s4 += 1;
			}
			if(s4 > 4)
				s4 = 1;
			else
				s4 = s4;
			break;

      case 12:
			HAL_Delay(50);
			if(HAL_GPIO_ReadPin(Key2_GPIO_Port,Key2_Pin) == 0)
			{
				s4 += 1;
			}
			if(s4 > 4)
				s4 = 1;
			else
				s4 = s4;
			break;

      case 13:
			HAL_Delay(50);
			if(HAL_GPIO_ReadPin(Key2_GPIO_Port,Key2_Pin) == 0)
			{
				s4 += 1;
			}
			if(s4 > 4)
				s4 = 1;
			else
				s4 = s4;
			break;

		default:
		break;
			}
	}
}	

//功能切换下键
void key_scan_down(void)		
{
	if(HAL_GPIO_ReadPin(Key1_GPIO_Port,Key1_Pin) == 0)
	{
		switch(s2)		//判断是主界面中哪个功能的次界面
		{
			case 0:
			HAL_Delay(50);
			if(HAL_GPIO_ReadPin(Key1_GPIO_Port,Key1_Pin) == 0)
			{
				
				s1 -= 1;
			}
			if(s1 < 1)
				s1 = 4;
			else
				s1 = s1;
      // Function_Primary();
			break;
			
			case 3:
			HAL_Delay(50);
			if(HAL_GPIO_ReadPin(Key1_GPIO_Port,Key1_Pin) == 0)
			{
				
				s3 -= 1;
			}
			if(s3 < 1)
				s3 = 3;
			else
				s3 = s3;
      // Real_Time_Storage();
			break;

      case 4:
        HAL_Delay(50);
			  if(HAL_GPIO_ReadPin(Key1_GPIO_Port,Key1_Pin) == 0)
		  	{
				  s4 -= 1;
			  }
			  if(s4 < 1)
				  s4 = 4;
			  else
				  s4 = s4;
        // Threshold_setting();
		  	break;

      case 5:
		  	HAL_Delay(50);
		  	if(HAL_GPIO_ReadPin(Key1_GPIO_Port,Key1_Pin) == 0)
			  {
			  	s4 -= 1;
		    }
			  if(s4 < 1)
			  	s4 = 4;
		  	else
			  	s4 = s4;
        // Threshold_Temperature();
		  	break;

        case 6:
		  	HAL_Delay(50);
		  	if(HAL_GPIO_ReadPin(Key1_GPIO_Port,Key1_Pin) == 0)
			  {
			  	s4 -= 1;
		    }
			  if(s4 < 1)
			  	s4 = 4;
		  	else
			  	s4 = s4;
		  	break;

        case 9:
		  	HAL_Delay(50);
		  	if(HAL_GPIO_ReadPin(Key1_GPIO_Port,Key1_Pin) == 0)
			  {
			  	s4 -= 1;
		    }
			  if(s4 < 1)
			  	s4 = 4;
		  	else
			  	s4 = s4;
		  	break;

        case 10:
		  	HAL_Delay(50);
		  	if(HAL_GPIO_ReadPin(Key1_GPIO_Port,Key1_Pin) == 0)
			  {
			  	s4 -= 1;
		    }
			  if(s4 < 1)
			  	s4 = 4;
		  	else
			  	s4 = s4;
		  	break;

        case 11:
		  	HAL_Delay(50);
		  	if(HAL_GPIO_ReadPin(Key1_GPIO_Port,Key1_Pin) == 0)
			  {
			  	s4 -= 1;
		    }
			  if(s4 < 1)
			  	s4 = 4;
		  	else
			  	s4 = s4;
		  	break;

        case 12:
		  	HAL_Delay(50);
		  	if(HAL_GPIO_ReadPin(Key1_GPIO_Port,Key1_Pin) == 0)
			  {
			  	s4 -= 1;
		    }
			  if(s4 < 1)
			  	s4 = 4;
		  	else
			  	s4 = s4;
		  	break;

        case 13:
		  	HAL_Delay(50);
		  	if(HAL_GPIO_ReadPin(Key1_GPIO_Port,Key1_Pin) == 0)
			  {
			  	s4 -= 1;
		    }
			  if(s4 < 1)
			  	s4 = 4;
		  	else
			  	s4 = s4;
		  	break;

		default:
		break;
			}
	}
}	

//标志位判断
void test(void)
{
  switch (s2)
  {
  case 0:
    Function_Primary();
    break;
  
  case 1:
    Date_Show();
    break;

  case 2:
    SD_Show();
    break;

  case 3:
    Real_Time_Storage();
    break;

  case 4:
    Threshold_setting();
    break;

  case 5:
    Threshold_Temperature();
    break;

  case 6:
    Threshold_Humidity();
    break;

  case 7:
    Threshold_Concentration();
    break;

  case 8:
    Alarm_Display();
    break;

  case 9:
    Collection_Path_Page();
    break;

  case 10:
    Set_Tem_Path();
    break;

  case 11:
    Set_Hum_Path();
    break;

  case 12:
    Set_Con_Path();
    break;

  case 13:
    Set_Light_Path();
    break;

  case 14:
    Threshold_Lgiht();
    break;

  default:
    break;
  }

  test_tem = temperature&0xff;
  if((((temperature>>8)+test_tem*0.1) > flag_temup)|(((temperature>>8)+test_tem*0.1) < flag_temlow))
  {
    s2 = 8;
    s5 = 1;
  }

  if(((int)(humidity>>8) > flag_humup)|((int)(humidity>>8) < flag_humlow))
  {
    s2 = 8;
    s5 = 2;
  }

  if((ppm_Integer > flag_conup)|(ppm_Integer < flag_conlow))
  {
    s2 = 8;
    s5 = 3;
  }

    if((lx_Integer > flag_lightup)|(lx_Integer < flag_lightlow))
  {
    s2 = 8;
    s5 = 4;
  }

  if(s2 != 2)
  Polling_CD4051();
  else
  CD4051(0);
}

//实时存储
void Real_Time_Storage(void){
  POINT_COLOR = BLUE;
  LCD_ShowString(50,20,240,200,16,"Collection Point");
  POINT_COLOR = BLACK;
  LCD_ShowString(20,60,240,200,16,"Turn on storage");
  LCD_ShowString(20,100,240,200,16,"Turn off storage");
  LCD_ShowString(20,140,240,200,16,"Set collection path");

  switch (s3)
      {
        case 1:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,70,5);
         break;

        case 2:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,110,5);
         break;

        case 3:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,150,5);
         break;

        default:
         break;
      }

  switch (flag_storage)
  {
    case 0:
      HAL_TIM_Base_Start_IT(&htim3);    
      LCD_Fill(100,180,120,200,GREEN); 
      break;

    case 1:
      HAL_TIM_Base_Stop_IT(&htim3);    
      LCD_Fill(100,180,120,200,RED); 
    
    default:
      break;
  }
}

//阈值设置
void Threshold_setting(void)
{
  char tem_road[20];
  char con_road[20];
  char hum_road[20];
  char light_road[20];
  sprintf((char*)tem_road,"Set Temperatuer R%d",flag_tem);
  sprintf((char*)con_road,"Set Density R%d",flag_con);
  sprintf((char*)hum_road,"Set Humdity R%d",flag_hum); 
  sprintf((char*)light_road,"Set Light R%d",flag_light);
  POINT_COLOR = BLUE;
  LCD_ShowString(60,20,240,200,16,"Set Interface");
  POINT_COLOR = BLACK;
  LCD_ShowString(20,60,240,200,16,tem_road);
  LCD_ShowString(20,100,240,200,16,hum_road);
  LCD_ShowString(20,140,240,200,16,con_road);
  LCD_ShowString(20,180,240,200,16,light_road);

  switch (s4)
      {
        case 1:
          LCD_Fill(0,0,20,200,WHITE);
          Draw_Circle(5,70,5);
          
         break;

        case 2:
          LCD_Fill(0,0,20,200,WHITE);
          Draw_Circle(5,110,5);
         break;

        case 3:
          LCD_Fill(0,0,20,200,WHITE);
          Draw_Circle(5,150,5);
         break;

        case 4:
          LCD_Fill(0,0,20,200,WHITE);
          Draw_Circle(5,190,5);
          break;

        default:
         break;
      }
}

//温度阈值设置
void Threshold_Temperature(void)
{
  char Threshold_temup[20];
  char Threshold_temlow[20];
  sprintf((char*)Threshold_temup,"%.1f degree",flag_temup);
  sprintf((char*)Threshold_temlow,"%.1f degree",flag_temlow);
  POINT_COLOR = BLUE;
  LCD_ShowString(30,20,240,200,16,"Temperatuer threshold");
  POINT_COLOR = BLACK;
  LCD_ShowString(20,50,240,200,16,"Turn up the upper limit");
  LCD_ShowString(20,80,240,200,16,"Turn down the upper limit");
  LCD_ShowString(20,150,240,200,16,"Turn up the lower limit");
  LCD_ShowString(20,180,240,200,16,"Turn down the lower limit");
  POINT_COLOR = GREEN;
  LCD_ShowString(100,110,240,200,16,Threshold_temup);
  LCD_ShowString(100,210,240,200,16,Threshold_temlow);
  POINT_COLOR = BLACK;

  switch (s4)
      {
        case 1:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,60,5);
         break;

        case 2:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,90,5);
         break;

        case 3:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,160,5);
         break;

        case 4:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,190,5);
         break;

        default:
         break;
      }
}

//湿度阈值设置
void Threshold_Humidity(void)
{
  char Threshold_humup[20];
  char Threshold_humlow[20];
  sprintf((char*)Threshold_humup,"%d%%",flag_humup);
  sprintf((char*)Threshold_humlow,"%d%%",flag_humlow);
  POINT_COLOR = BLUE;
  LCD_ShowString(50,20,240,200,16,"Humidity threshold");
  POINT_COLOR = BLACK;
  LCD_ShowString(20,50,240,200,16,"Turn up the upper limit");
  LCD_ShowString(20,80,240,200,16,"Turn down the upper limit");
  LCD_ShowString(20,150,240,200,16,"Turn up the lower limit");
  LCD_ShowString(20,180,240,200,16,"Turn down the lower limit");
  POINT_COLOR = GREEN;
  LCD_ShowString(100,110,240,200,16,Threshold_humup);
  LCD_ShowString(100,210,240,200,16,Threshold_humlow);
  POINT_COLOR = BLACK;

  switch (s4)
      {
        case 1:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,60,5);
         break;

        case 2:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,90,5);
         break;

        case 3:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,160,5);
         break;

        case 4:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,190,5);
         break;

        default:
         break;
      }
}

void Threshold_Concentration(void)
{
  char Threshold_conup[20];
  char Threshold_conlow[20];
  sprintf((char*)Threshold_conup,"%d ppm",flag_conup);
  sprintf((char*)Threshold_conlow,"%d ppm",flag_conlow);
  POINT_COLOR = BLUE;
  LCD_ShowString(30,20,240,200,16,"Concentration threshold");
  POINT_COLOR = BLACK;
  LCD_ShowString(20,50,240,200,16,"Turn up the upper limit");
  LCD_ShowString(20,80,240,200,16,"Turn down the upper limit");
  LCD_ShowString(20,150,240,200,16,"Turn up the lower limit");
  LCD_ShowString(20,180,240,200,16,"Turn down the lower limit");
  POINT_COLOR = GREEN;
  LCD_ShowString(100,110,240,200,16,Threshold_conup);
  LCD_ShowString(100,210,240,200,16,Threshold_conlow);
  POINT_COLOR = BLACK;

  switch (s4)
      {
        case 1:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,60,5);
         break;

        case 2:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,90,5);
         break;

        case 3:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,160,5);
         break;

        case 4:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,190,5);
         break;

        default:
         break;
      }
}

void Threshold_Lgiht(void)
{
  char Threshold_lxup[20];
  char Threshold_lxlow[20];
  sprintf((char*)Threshold_lxup,"%d lx",flag_lightup);
  sprintf((char*)Threshold_lxlow,"%d lx",flag_lightlow);
  POINT_COLOR = BLUE;
  LCD_ShowString(60,20,240,200,16,"Light threshold");
  POINT_COLOR = BLACK;
  LCD_ShowString(20,50,240,200,16,"Turn up the upper limit");
  LCD_ShowString(20,80,240,200,16,"Turn down the upper limit");
  LCD_ShowString(20,150,240,200,16,"Turn up the lower limit");
  LCD_ShowString(20,180,240,200,16,"Turn down the lower limit");
  POINT_COLOR = GREEN;
  LCD_ShowString(100,110,240,200,16,Threshold_lxup);
  LCD_ShowString(100,210,240,200,16,Threshold_lxlow);
  POINT_COLOR = BLACK;

  switch (s4)
      {
        case 1:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,60,5);
         break;

        case 2:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,90,5);
         break;

        case 3:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,160,5);
         break;

        case 4:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,190,5);
         break;

        default:
         break;
      }
}

void Device_init(void)
{
    HAL_ADC_Start(&hadc1);     //启动ADC转换
    // HAL_ADC_PollForConversion(&hadc1, 50);   //等待转换完成，50为最大等待时间，单位为ms
    // DHT11_Read_Data(&temperature,&humidity);
 
    // if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
    // {
      AD_Value = HAL_ADC_GetValue(&hadc1);   //获取AD值
    // }
    if((AD_Value > 3000)&(AD_Value < 3400))
      Vol_Value = Vol_Value;
      else
      Vol_Value = AD_Value*3.3f/4096;
    ppm = pow((3.4880*10*Vol_Value)/(5 -Vol_Value),(1.0/0.3203));
    ppm_Integer = (int)ppm;
    ppm_Decimal = (int)(10*ppm)%10; 
    lx = Read_BH1750_Data();
    lx_Integer = (int)lx;
    lx_Decimal = (int)(10*lx)%10;
}

void Alarm_Display(void)
{
  switch (s5)
  {
  case 1:
    LCD_Clear(YELLOW);
    sprintf((char*)tem_show,"Road %d = %d.%d degree",flag_tem,temperature>>8,temperature&0xff);
    LCD_ShowString(40,140,200,240,16,"TEMPERATURE ALARM!!!");
    LCD_ShowString(50,170,200,240,16,tem_show);
    // HAL_GPIO_WritePin(Beep_GPIO_Port,Beep_Pin,1);
    if((((temperature>>8)+test_tem*0.1) < flag_temup)&(((temperature>>8)+test_tem*0.1) > flag_temlow))
    {
      LCD_Clear(WHITE);
      s2 = 0;
      s5 = 0;
    }
    break;

  case 2:
    LCD_Clear(YELLOW);
    sprintf((char*)hum_show,"Road %d = %d.%d%%",flag_hum,humidity>>8,humidity&0xff);
    LCD_ShowString(40,140,200,240,16,"HUMIDITY ALARM!!!");
    LCD_ShowString(50,170,200,240,16,hum_show);
    // HAL_GPIO_WritePin(Beep_GPIO_Port,Beep_Pin,1);
    break;

  case 3:
    LCD_Clear(YELLOW);
    sprintf((char*)ppm_show,"Road %d = %.2f ppm",flag_con,ppm);
    LCD_ShowString(40,140,200,240,16,"CONCENTRATION ALARM!!!");
    LCD_ShowString(50,170,200,240,16,ppm_show);
    // HAL_GPIO_WritePin(Beep_GPIO_Port,Beep_Pin,1);
    if((ppm_Integer < flag_conup)&(ppm_Integer > flag_conlow))
    {
      LCD_Clear(WHITE);
      s2 = 0;
      s5 = 0;
    }
    break;

  case 4:
    LCD_Clear(YELLOW);
    sprintf((char*)lux_show,"Road %d intensity = %d.%d lx",flag_light,lx,temp2);
    LCD_ShowString(40,140,200,240,16,"LIGHT ALARM!!!");
    LCD_ShowString(50,170,200,240,16,lux_show);
    // HAL_GPIO_WritePin(Beep_GPIO_Port,Beep_Pin,1);
    break;

  default:
    break;
  }
}

void Collection_Path_Page(void)
{
  POINT_COLOR = BLUE;
  LCD_ShowString(50,20,200,240,16,"Collection Path");
  POINT_COLOR = BLACK;
  LCD_ShowString(20,60,200,240,16,"Set temperature equipment");
  LCD_ShowString(20,100,200,240,16,"Set humidity equipment");
  LCD_ShowString(20,140,200,240,16,"Set density equipment");
  LCD_ShowString(20,180,200,240,16,"Set lighting equipment");

  switch (s4)
      {
        case 1:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,70,5);
          
         break;

        case 2:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,110,5);
         break;
         
        case 3:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,150,5);
          break;

        case 4:
          LCD_Fill(0,0,18,200,WHITE);
          Draw_Circle(5,190,5);
          break;

        default:
         break;
      }

    if(flag_tem == 0)
  {
    POINT_COLOR = RED;
    LCD_ShowString(10,220,240,200,16,"Temperature device is not set!");
    POINT_COLOR = BLACK;
  }else if(flag_con == 0)
  {
    POINT_COLOR = RED;
    LCD_ShowString(10,220,240,200,16,"Density device is not set!");
    POINT_COLOR = BLACK;
  }else if(flag_hum == 0)
  {
    POINT_COLOR = RED;
    LCD_ShowString(10,220,240,200,16,"Humidity device is not set!");
    POINT_COLOR = BLACK;
  }else if(flag_light == 0)
  {
    POINT_COLOR = RED;
    LCD_ShowString(10,220,240,200,16,"Light device is not set!");
    POINT_COLOR = BLACK;
  }
}

void Set_Tem_Path(void)
{
  POINT_COLOR = BLUE;
  LCD_ShowString(50,20,200,240,16,"Temperature Path");
  POINT_COLOR = BLACK;
  LCD_ShowString(20,60,200,240,16,"Set to road 1");
  LCD_ShowString(20,100,200,240,16,"Set to road 2");
  LCD_ShowString(20,140,200,240,16,"Set to road 3");
  LCD_ShowString(20,180,200,240,16,"Set to road 4");
  switch (s4)
    {
      case 1:
      LCD_Fill(0,0,18,200,WHITE);
      Draw_Circle(5,70,5);
      break;

      case 2:
      LCD_Fill(0,0,18,200,WHITE);
      Draw_Circle(5,110,5);
      break;
         
      case 3:
      LCD_Fill(0,0,18,200,WHITE);
      Draw_Circle(5,150,5);
      break;

      case 4:
      LCD_Fill(0,0,18,200,WHITE);
      Draw_Circle(5,190,5);
      break;

      default:
      break;
    }

  if(flag_tem == 1)
    LCD_Fill(150,60,170,80,GREEN);
  else
    LCD_Fill(150,60,170,80,RED);

  if(flag_tem == 2)
    LCD_Fill(150,100,170,120,GREEN);
  else
    LCD_Fill(150,100,170,120,RED);

  if(flag_tem == 3)
    LCD_Fill(150,140,170,160,GREEN);
  else
    LCD_Fill(150,140,170,160,RED);
  
  if(flag_tem == 4)
    LCD_Fill(150,180,170,200,GREEN);
  else
    LCD_Fill(150,180,170,200,RED);

}

void Set_Hum_Path(void)
{
  POINT_COLOR = BLUE;
  LCD_ShowString(65,20,200,240,16,"Humidity Path");
  POINT_COLOR = BLACK;
  LCD_ShowString(20,60,200,240,16,"Set to road 1");
  LCD_ShowString(20,100,200,240,16,"Set to road 2");
  LCD_ShowString(20,140,200,240,16,"Set to road 3");
  LCD_ShowString(20,180,200,240,16,"Set to road 4");
  switch (s4)
    {
      case 1:
      LCD_Fill(0,0,18,200,WHITE);
      Draw_Circle(5,70,5);
      break;

      case 2:
      LCD_Fill(0,0,18,200,WHITE);
      Draw_Circle(5,110,5);
      break;
         
      case 3:
      LCD_Fill(0,0,18,200,WHITE);
      Draw_Circle(5,150,5);
      break;

      case 4:
      LCD_Fill(0,0,18,200,WHITE);
      Draw_Circle(5,190,5);
      break;

      default:
      break;
      }

  if(flag_hum == 1)
    LCD_Fill(150,60,170,80,GREEN);
  else
    LCD_Fill(150,60,170,80,RED);

  if(flag_hum == 2)
    LCD_Fill(150,100,170,120,GREEN);
  else
    LCD_Fill(150,100,170,120,RED);

  if(flag_hum == 3)
    LCD_Fill(150,140,170,160,GREEN);
  else
    LCD_Fill(150,140,170,160,RED);
  
  if(flag_hum == 4)
    LCD_Fill(150,180,170,200,GREEN);
  else
    LCD_Fill(150,180,170,200,RED);


}

void Set_Con_Path(void)
{
  POINT_COLOR = BLUE;
  LCD_ShowString(50,20,200,240,16,"Concentration Path");
  POINT_COLOR = BLACK;
  LCD_ShowString(20,60,200,240,16,"Set to road 1");
  LCD_ShowString(20,100,200,240,16,"Set to road 2");
  LCD_ShowString(20,140,200,240,16,"Set to road 3");
  LCD_ShowString(20,180,200,240,16,"Set to road 4");
  switch (s4)
    {
      case 1:
      LCD_Fill(0,0,18,200,WHITE);
      Draw_Circle(5,70,5);
      break;

      case 2:
      LCD_Fill(0,0,18,200,WHITE);
      Draw_Circle(5,110,5);
      break;
         
      case 3:
      LCD_Fill(0,0,18,200,WHITE);
      Draw_Circle(5,150,5);
      break;

      case 4:
      LCD_Fill(0,0,18,200,WHITE);
      Draw_Circle(5,190,5);
      break;

      default:
      break;
      }

  if(flag_con == 1)
    LCD_Fill(150,60,170,80,GREEN);
  else
    LCD_Fill(150,60,170,80,RED);

  if(flag_con == 2)
    LCD_Fill(150,100,170,120,GREEN);
  else
    LCD_Fill(150,100,170,120,RED);

  if(flag_con == 3)
    LCD_Fill(150,140,170,160,GREEN);
  else
    LCD_Fill(150,140,170,160,RED);
  
  if(flag_con == 4)
    LCD_Fill(150,180,170,200,GREEN);
  else
    LCD_Fill(150,180,170,200,RED);
}

void Set_Light_Path(void)
{
  POINT_COLOR = BLUE;
  LCD_ShowString(75,20,200,240,16,"Light Path");
  POINT_COLOR = BLACK;
  LCD_ShowString(20,60,200,240,16,"Set to road 1");
  LCD_ShowString(20,100,200,240,16,"Set to road 2");
  LCD_ShowString(20,140,200,240,16,"Set to road 3");
  LCD_ShowString(20,180,200,240,16,"Set to road 4");
  switch (s4)
    {
      case 1:
      LCD_Fill(0,0,18,200,WHITE);
      Draw_Circle(5,70,5);
      break;

      case 2:
      LCD_Fill(0,0,18,200,WHITE);
      Draw_Circle(5,110,5);
      break;
         
      case 3:
      LCD_Fill(0,0,18,200,WHITE);
      Draw_Circle(5,150,5);
      break;

      case 4:
      LCD_Fill(0,0,18,200,WHITE);
      Draw_Circle(5,190,5);
      break;

      default:
      break;
      }

  if(flag_light == 1)
    LCD_Fill(150,60,170,80,GREEN);
  else
    LCD_Fill(150,60,170,80,RED);

  if(flag_light == 2)
    LCD_Fill(150,100,170,120,GREEN);
  else
    LCD_Fill(150,100,170,120,RED);

  if(flag_light == 3)
    LCD_Fill(150,140,170,160,GREEN);
  else
    LCD_Fill(150,140,170,160,RED);
  
  if(flag_light == 4)
    LCD_Fill(150,180,170,200,GREEN);
  else
    LCD_Fill(150,180,170,200,RED);
}

void CD4051(uint8_t n)
{
  switch (n)
  {
  case 0:
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,0);//c
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_14,0);//b
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_15,0);//a
    break;
  case 1:
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,0);//c
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_14,0);//b
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_15,1);//a
    break;

  case 2:
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,0);//c
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_14,1);//b
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_15,0);//a
    break;
  case 3:
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,0);//c
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_14,1);//b
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_15,1);//a
    break;
  case 4:
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,1);//c
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_14,0);//b
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_15,0);//a
    break;
  case 5:
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,1);//c
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_14,0);//b
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_15,1);//a
    break;
  case 6:
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,1);//c
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_14,1);//b
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_15,0);//a
    break;
  case 7:
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,1);//c
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_14,1);//b
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_15,1);//a
    break;
  
  default:
    break;
  }
}

void Polling_CD4051(void)
{
  CD4051(0);
  flag = DHT11_Read_Data(&temperature,&humidity);
  CD4051(50);
  HAL_Delay(50);
  CD4051(2);
  HAL_Delay(50);
  CD4051(3);
  HAL_Delay(50);
  CD4051(4);
  HAL_Delay(50);
  CD4051(5);
  HAL_Delay(50);
  CD4051(6);
  HAL_Delay(50);
  CD4051(7);
  HAL_Delay(50);
}





/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
