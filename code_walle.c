/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c-lcd.h"
#include <stdio.h> // Dodano dla sprintf
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LEWY_SILNIK_PWM TIM_CHANNEL_2
#define PRAWY_SILNIK_PWM TIM_CHANNEL_3
#define CZUJNIK_PODCZERWIENI_Pin GPIO_PIN_2
#define CZUJNIK_PODCZERWIENI_GPIO_Port GPIOB

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM1_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM6_Init(void);
static void MX_RTC_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void delay (uint16_t time)
{
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while (__HAL_TIM_GET_COUNTER (&htim1) < time);

}

uint32_t IC_Val1 = 0;
uint32_t IC_Val2 = 0;
uint32_t Difference = 0;
uint8_t Is_First_Captured = 0;  // is the first value captured ?
volatile uint8_t Distance  = 0;
float licznik;
int _write(int file, char *ptr, int len)
{
	HAL_UART_Transmit(&huart2, (uint8_t*)ptr, len, 10);
}


// Let's write the callback function

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)  // if the interrupt source is channel1
	{
		if (Is_First_Captured==0) // if the first value is not captured
		{
			IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
			Is_First_Captured = 1;  // set the first captured as true
			// Now change the polarity to falling edge
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
		}

		else if (Is_First_Captured==1)   // if the first is already captured
		{
			IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);  // read second value
			__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter

			if (IC_Val2 > IC_Val1)
			{
				Difference = IC_Val2-IC_Val1;
			}

			else if (IC_Val1 > IC_Val2)
			{
				Difference = (0xffff - IC_Val1) + IC_Val2;
			}

			Distance = Difference * .034/2;
			Is_First_Captured = 0; // set it back to false

			// set polarity to rising edge
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(&htim1, TIM_IT_CC1);
		}
	}
}

void HCSR04_Read (void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);  // pull the TRIG pin HIGH
	delay(10);  // wait for 10 us
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);  // pull the TRIG pin low

	__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC1);
}


void StopMotors(void) //funkcja do zatrzymywania pojazdu
{
    // Zatrzymanie PWM przy jezdzie do przodu
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3); // Stop PWM for motor connected to pin D6
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2); // Stop PWM for motor connected to pin D3
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); // Turn off motor connected to pin D6
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET); // Turn off motor connected to pin D3

    // Zatrzymanie PWM przy jezdzie do tylu
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1); // Stop PWM for motor connected to pin D6
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2); // Stop PWM for motor connected to pin D3
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); // Turn off motor connected to pin D6
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET); // Turn off motor connected to pin D3
}

void GoForward(void) //jazda do przodu
{
	 HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // Start PWM for motor connected to pin D6
	 HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // Start PWM for motor connected to pin D3
	 uint32_t pulse_motor_D6 = 18; // D6 Right Motor
	 uint32_t pulse_motor_D3 = 11; // D3 Left Motor
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse_motor_D6); // Set duty cycle for motor connected to pin D6
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse_motor_D3); // Set duty cycle for motor connected to pin D3
}

void GoBack(void) //jazda do tylu
{
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); // Start PWM for motor connected to pin D6
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2); // Start PWM for motor connected to pin D3
	uint32_t pulse_motor_D12 = 15; // D5 Right Motor
	uint32_t pulse_motor_D4 = 11; //  D4 Left Motor
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pulse_motor_D12); // Set duty cycle for motor connected to pin D5
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, pulse_motor_D4); // Set duty cycle for motor connected to pin D4
}

void GoLeft(void) //jazda w lewo
{
	 StopMotors();
	 HAL_Delay(500);
	 HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // Start PWM for motor connected to pin D6
	 HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // Start PWM for motor connected to pin D3
	 uint32_t pulse_motor_D6 = 20; // D6 Right Motor prawa gąsienica jest znacznie wolniejsza, dlatego potrzebuje wiekszego przyspieszenia
	 uint32_t pulse_motor_D3 = 0; // D3 Left Motor
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse_motor_D6); // Set duty cycle for motor connected to pin D6
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse_motor_D3); // Set duty cycle for motor connected to pin D3
	 HAL_Delay(2500);
	 StopMotors();
	 HAL_Delay(500);
}

void GoRight(void) //jazda w prawo
{
	 StopMotors();
	 HAL_Delay(500);
	 HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // Start PWM for motor connected to pin D6
	 HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // Start PWM for motor connected to pin D3
	 uint32_t pulse_motor_D6 = 0; // D6 Right Motor
	 uint32_t pulse_motor_D3 = 8; // D3 Left Motor prawa gąsienica jest znacznie szybsza, dlatego potrzebuje mniejszego przyspieszenia
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse_motor_D6); // Set duty cycle for motor connected to pin D6
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse_motor_D3); // Set duty cycle for motor connected to pin D3
	 HAL_Delay(2000);
	 StopMotors();
	 HAL_Delay(500);
}

//// Funkcja konfiguracji kanału ADC
//void Configure_ADC_Channel(ADC_HandleTypeDef* hadc, uint32_t channel)
//{
//    ADC_ChannelConfTypeDef sConfig = {0};
//
//    sConfig.Channel = channel;
//    sConfig.Rank = ADC_REGULAR_RANK_1;
//    sConfig.SamplingTime = ADC_SAMPLETIME_12CYCLES_5; // Dostosuj czas próbkowania według potrzeb
//
//    if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK) {
//        // Obsługa błędu
//        Error_Handler();
//}
//}

//void lcd_display_distance(uint8_t distance) {
//    char buffer[16];
//    sprintf(buffer, "Distance: %d cm", distance);
//    lcd_clear();
//    lcd_put_cur(0, 0);
//    lcd_send_string(buffer);
//}

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
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM1_Init();
  MX_I2C1_Init();
  MX_TIM6_Init();
  MX_RTC_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
//  lcd_init();
  HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
//  lcd_send_string ("Dist= ");
  //lcd_init();

  // Struktury ADC
  //ADC_ChannelConfTypeDef sConfig = {0};
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //JESLI CHCESZ PRZESUNAC KOD W LEWO ZAZNACZ KOD I KLIKNIJ SHIFT + TAB
	  HCSR04_Read();
	  HAL_Delay(200);

	  //ANALOGOWE NIE DZIA�?AJĄ!!
	  //ODCZYT Z LEWEGO CZUJNIKA ANALOGOWEGO PA0
	  // Odczyt wartości z lewego czujnika analogowego (kanał 9)
//	  Configure_ADC_Channel(&hadc1, ADC_CHANNEL_9);
//	  HAL_ADC_Start(&hadc1);
//	  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
//	  uint32_t adcValue1 = HAL_ADC_GetValue(&hadc1);
//	  float voltage1 = (adcValue1 / 4095.0) * 3.3;
//	  printf("Voltage1: %.2f V\n", voltage1);

	  // Debug output for voltage1


	  // Odczyt wartości z prawego czujnika analogowego (kanał 2)
//	  Configure_ADC_Channel(&hadc3, ADC_CHANNEL_2);
//	  HAL_ADC_Start(&hadc3);
//	  HAL_ADC_PollForConversion(&hadc3, HAL_MAX_DELAY);
//	  uint32_t adcValue2 = HAL_ADC_GetValue(&hadc3);
//	  float voltage2 = (adcValue2 / 4095.0) * 3.3;
//	  printf("Voltage2: %.2f V\n", voltage2);

	  ///WYKRYWANIE PRZESZKOD///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	  //WYKRYWANIE PRZESZKOD PRZEZ TYLKO SAM CZUJNIK ULTRADZWIEKOWY, JESLI PRZEDNI PRAWY I PRZEDNI LEWY NIC NIE WYKRYWA
	  if ((Distance < 25) && HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_SET && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_SET)
	  {
		  StopMotors();
		  HAL_Delay(300);
		  GoBack();
		  HAL_Delay(1000);
		  GoLeft();
	  }

	  //WYKRYWANIE PRZESZKOD PRZEZ CZUJNIK ULTRADZWIEKOWY ORAZ PRZEDNI PRAWY, JESLI PRZEDNI LEWY NIC NIE WYKRYWA
	  if ((Distance < 25) && HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_SET)
	  {
		  StopMotors();
		  HAL_Delay(300);
		  GoBack();
		  HAL_Delay(1000);
		  GoLeft();
	  }

	  //WYKRYWANIE PRZESZKOD PRZEZ CZUJNIK ULTRADZWIEKOWY, PRZEDNI PRAWY ORAZ PRAWY, JESLI PRZEDNI LEWY NIC NIE WYKRYWA
	  if ((Distance < 25) && HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_SET)
	  {
		  StopMotors();
		  HAL_Delay(300);
		  GoBack();
		  HAL_Delay(500);
		  GoLeft();
	  }

	  //WYKRYWANIE PRZESZKOD PRZEZ CZUJNIK ULTRADZWIEKOWY ORAZ PRZEDNI LEWY, JESLI PRZEDNI PRAWY NIC NIE WYKRYWA
	  if ((Distance < 25) && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_SET)
	  {
		  StopMotors();
		  HAL_Delay(300);
		  GoBack();
		  HAL_Delay(1000);
		  GoRight();
	  }

	  //WYKRYWANIE PRZESZKOD PRZEZ CZUJNIK ULTRADZWIEKOWY, PRZEDNI LEWY ORAZ LEWY, JESLI PRZEDNI PRAWY NIC NIE WYKRYWA
	  if ((Distance < 25) && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_SET)
	  {
		  StopMotors();
		  HAL_Delay(300);
		  GoBack();
		  HAL_Delay(500);
		  GoRight();
	  }

	  //WYKRYWANIE PRZESZKOD PRZEZ CZUJNIK ULTRADZWIEKOWY, PRZEDNI LEWY ORAZ PRZEDNI PRAWY, JESLI PRAWY I LEWY NIC NIE WYKRYWA
	  if ((Distance < 25) && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) == GPIO_PIN_SET && HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2) == GPIO_PIN_SET)
	  {
		  StopMotors();
		  HAL_Delay(300);
		  GoBack();
		  HAL_Delay(700);
		  GoLeft();
	  }

	  //WYKRYWANIE PRZESZKOD PRZEZ CZUJNIK ULTRADZWIEKOWY, PRZEDNI LEWY, PRZEDNI PRAWY I PRAWY, JESLI LEWY NIC NIE WYKRYWA
	  if ((Distance < 25) && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2) == GPIO_PIN_SET)
	  {
		  StopMotors();
		  HAL_Delay(300);
		  GoBack();
		  HAL_Delay(700);
		  GoLeft();
	  }

	  //WYKRYWANIE PRZESZKOD PRZEZ CZUJNIK ULTRADZWIEKOWY, PRZEDNI LEWY, PRZEDNI PRAWY I LEWY, JESLI PRAWY NIC NIE WYKRYWA
	  if ((Distance < 25) && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) == GPIO_PIN_SET )
	  {
		  StopMotors();
		  HAL_Delay(300);
		  GoBack();
		  HAL_Delay(700);
		  GoRight();
	  }

	  //WYKRYWANIE PRZESZKOD PRZEZ CZUJNIK ULTRADZWIEKOWY, PRZEDNI LEWY, PRZEDNI PRAWY, LEWY I PRAWY - TUNEL
	  if ((Distance < 25) && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) == GPIO_PIN_RESET)
	  {
		  StopMotors();
		  HAL_Delay(300);
		  GoBack();
		  HAL_Delay(2000);
		  GoRight();
	  }

	  //WYKRYWANIE PRZESZKOD PRZEZ PRZEDNI PRAWY CZUJNIK ODBICIA, JESLI ULTRADZWIEKOWY I PRAWY CZUJNIK NIC NIE WYKRYWA
	  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_RESET && (Distance > 25) && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) == GPIO_PIN_SET)
	  {
		  StopMotors();
		  HAL_Delay(500);
		  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // Start PWM for motor connected to pin D6
		  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // Start PWM for motor connected to pin D3
		  uint32_t pulse_motor_D6 = 20; // D6 Right Motor prawa gąsienica jest znacznie wolniejsza, dlatego potrzebuje wiekszego przyspieszenia
		  uint32_t pulse_motor_D3 = 0; // D3 Left Motor
		  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse_motor_D6); // Set duty cycle for motor connected to pin D6
		  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse_motor_D3); // Set duty cycle for motor connected to pin D3
		  HAL_Delay(1000);
		  StopMotors();
		  HAL_Delay(500);
	  }
	  //WYKRYWANIE PRZESZKOD PRZEZ PRZEDNI LEWY CZUJNIK ODBICIA, JESLI ULTRADZWIEKOWY I LEWY CZUJNIK NIC NIE WYKRYWA
	  if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_RESET && (Distance > 25) && HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2) == GPIO_PIN_SET) //WYKRYWANIE PRZESZKOD PRZEZ PRZEDNI LEWY CZUJNIK ODBICIA
	  {
		  StopMotors();
		  HAL_Delay(500);
		  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // Start PWM for motor connected to pin D6
		  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // Start PWM for motor connected to pin D3
		  uint32_t pulse_motor_D6 = 0; // D6 Right Motor
		  uint32_t pulse_motor_D3 = 8; // D3 Left Motor prawa gąsienica jest znacznie szybsza, dlatego potrzebuje mniejszego przyspieszenia
		  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse_motor_D6); // Set duty cycle for motor connected to pin D6
		  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse_motor_D3); // Set duty cycle for motor connected to pin D3
		  HAL_Delay(1000);
		  StopMotors();
		  HAL_Delay(500);
	  }

	  //WYKRYWANIE PRZESZKOD PRZEZ PRAWY CZUJNIK ODBICIA, JEŚLI PRZEDNI PRAWY CZUJNIK NIC NIE WYKRYWA
	  if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_SET)
	  {
		  StopMotors();
		  HAL_Delay(250);
		  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // Start PWM for motor connected to pin D6
		  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // Start PWM for motor connected to pin D3
		  uint32_t pulse_motor_D6 = 20; // D6 Right Motor prawa gąsienica jest znacznie wolniejsza, dlatego potrzebuje wiekszego przyspieszenia
		  uint32_t pulse_motor_D3 = 8; // D3 Left Motor
		  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse_motor_D6); // Set duty cycle for motor connected to pin D6
		  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse_motor_D3); // Set duty cycle for motor connected to pin D3
		  HAL_Delay(1000);
		  StopMotors();
		  HAL_Delay(250);
	  }

	  //WYKRYWANIE PRZESZKOD PRZEZ LEWY CZUJNIK ODBICIA, JEŚLI PRZEDNI LEWY CZUJNIK NIC NIE WYKRYWA
	  if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_SET)
	  {
		  StopMotors();
		  HAL_Delay(250);
		  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // Start PWM for motor connected to pin D6
		  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // Start PWM for motor connected to pin D3
		  uint32_t pulse_motor_D6 = 16; // D6 Right Motor prawa gąsienica jest znacznie wolniejsza, dlatego potrzebuje wiekszego przyspieszenia
		  uint32_t pulse_motor_D3 = 11; // D3 Left Motor
		  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse_motor_D6); // Set duty cycle for motor connected to pin D6
		  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse_motor_D3); // Set duty cycle for motor connected to pin D3
		  HAL_Delay(1000);
		  StopMotors();
		  HAL_Delay(250);
	  }
//
//	  //KONIEC WARUNKOW WYKRYWANIA PRZESZKOD///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	  //WYKRYWANIE METY (TASMY MAGNETYCZNEJ) PRZEZ CZUJNIK HALLA
//	  if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2) == GPIO_PIN_SET)
//	  {
//		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, SET);
//		  StopMotors();
//		  HAL_Delay(3000);
//		  GoForward();
//		  HAL_Delay(1000);
//		  StopMotors();
//		  HAL_Delay(300);
//		  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // Start PWM for motor connected to pin D6
//		  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // Start PWM for motor connected to pin D3
//		  uint32_t pulse_motor_D6 = 0; // D6 Right Motor
//		  uint32_t pulse_motor_D3 = 11; // D3 Left Motor prawa gąsienica jest znacznie szybsza, dlatego potrzebuje mniejszego przyspieszenia
//		  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse_motor_D6); // Set duty cycle for motor connected to pin D6
//		  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse_motor_D3); // Set duty cycle for motor connected to pin D3
//		  HAL_Delay(5000);
//		  StopMotors();
//		  HAL_Delay(500);
//	  }

//	  //WYKRYWANIE TASMY ALUMINIOWEJ/////////////////////////////////////////////////////////////////////

	  //WYKRYWANIE TASMY PRZEZ CZUJNIK LEWY
	  if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET)
	  {
		  StopMotors();
		  HAL_Delay(300);
		  GoBack();
		  HAL_Delay(1500);
		  StopMotors();
		  HAL_Delay(300);
		  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // Start PWM for motor connected to pin D6
		  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // Start PWM for motor connected to pin D3
		  uint32_t pulse_motor_D6 = 0; // D6 Right Motor
		  uint32_t pulse_motor_D3 = 11; // D3 Left Motor prawa gąsienica jest znacznie szybsza, dlatego potrzebuje mniejszego przyspieszenia
		  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse_motor_D6); // Set duty cycle for motor connected to pin D6
		  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse_motor_D3); // Set duty cycle for motor connected to pin D3
		  HAL_Delay(2000);
		  StopMotors();
		  HAL_Delay(300);
	  }

	  //WYKRYWANIE TASMY PRZEZ CZUJNIK PRAWY
	  if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_RESET)
	  {
		  StopMotors();
		  HAL_Delay(300);
		  GoBack();
		  HAL_Delay(1500);
		  StopMotors();
		  HAL_Delay(300);
		  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // Start PWM for motor connected to pin D6
		  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // Start PWM for motor connected to pin D3
		  uint32_t pulse_motor_D6 = 20; // D6 Right Motor prawa gąsienica jest znacznie wolniejsza, dlatego potrzebuje wiekszego przyspieszenia
		  uint32_t pulse_motor_D3 = 0; // D3 Left Motor
		  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse_motor_D6); // Set duty cycle for motor connected to pin D6
		  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse_motor_D3); // Set duty cycle for motor connected to pin D3
		  HAL_Delay(2000);
		  StopMotors();
		  HAL_Delay(300);
	  }


	  //KONIEC WYKRYWANIA TASMY ALUMINIOWEJ//////////////////////////////////////////////////////////////

	  else
	  {
		  GoForward();
	  }

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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10909CEC;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 597;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_THURSDAY;
  sDate.Month = RTC_MONTH_JULY;
  sDate.Date = 0x4;
  sDate.Year = 0x24;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0x0;
  sAlarm.AlarmTime.Minutes = 0x0;
  sAlarm.AlarmTime.Seconds = 0x1;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_ALL;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 0x1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 80-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0xffff-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 500;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 20;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 500;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 20;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 7999;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 9999;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TRIG_ULTRA_GPIO_Port, TRIG_ULTRA_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : NOT_USED_BUTTON_Pin HALLA_Pin PRZEDNI_LEWY_ODBICIA_Pin PRAWY_ODBICIA_Pin */
  GPIO_InitStruct.Pin = NOT_USED_BUTTON_Pin|HALLA_Pin|PRZEDNI_LEWY_ODBICIA_Pin|PRAWY_ODBICIA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LEWY_LINIA_Pin PRAWY_LINIA_Pin */
  GPIO_InitStruct.Pin = LEWY_LINIA_Pin|PRAWY_LINIA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PRZEDNI_PRAWY_ODBICIA_Pin */
  GPIO_InitStruct.Pin = PRZEDNI_PRAWY_ODBICIA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PRZEDNI_PRAWY_ODBICIA_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TRIG_ULTRA_Pin */
  GPIO_InitStruct.Pin = TRIG_ULTRA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TRIG_ULTRA_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LEWY_ODBICIA_Pin */
  GPIO_InitStruct.Pin = LEWY_ODBICIA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LEWY_ODBICIA_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
//licznik+=0.1;
//printf("Czas przejazdu: %.2f s\r\n",licznik);
//}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
  RTC_AlarmTypeDef sAlarm;
  HAL_RTC_GetAlarm(hrtc,&sAlarm,RTC_ALARM_A,FORMAT_BIN);
  if(sAlarm.AlarmTime.Seconds>58) {
    sAlarm.AlarmTime.Seconds=0;
  }else{
    sAlarm.AlarmTime.Seconds=sAlarm.AlarmTime.Seconds+1;
  }
    while(HAL_RTC_SetAlarm_IT(hrtc, &sAlarm, FORMAT_BIN)!=HAL_OK){}
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
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
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
