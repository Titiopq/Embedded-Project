/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>  // Needed for sprintf
#include <string.h> // Needed for strlen
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// --- High Octave Frequencies (loudest for small buzzers) ---
// --- Octave 5 (Mid-High) ---
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_A5  880
#define NOTE_B5  988

// --- Octave 6 (High - Best for resonance) ---
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
// --- Sound Structure ---
typedef struct {
    uint16_t frequency; // Frequency in Hz
    uint16_t duration;  // Duration in milliseconds
} Tone;

const Tone snd_power_up[] = {
    {NOTE_C5, 100}, // 523 Hz
    {NOTE_E5, 100}, // 659 Hz
    {NOTE_G5, 100}, // 784 Hz
    {NOTE_C6, 100}, // 1047 Hz
    {NOTE_E6, 100}, // 1319 Hz
    {NOTE_G6, 250}, // 1568 Hz (Holds the high note)
    {0, 0}
};

const Tone snd_next_level[] = {
    {NOTE_E6, 100}, // 1319 Hz
    {NOTE_G6, 250}, // 1568 Hz (Holds the high note)
    {0, 0}
};

const Tone snd_game_start[] = {
    {NOTE_G5, 100}, // Ready...
    {0,       50},
    {NOTE_G5, 100}, // Set...
    {0,       50},
    {NOTE_C6, 400}, // GO! (High C)
    {0, 0}
};

const Tone snd_game_loss[] = {
    {NOTE_C6, 200}, // 1047 Hz
    {NOTE_B5, 200}, // 988 Hz
    {NOTE_AS6,200}, // (A#) using higher octave equivalent or A5 sharp
    // Actually let's use the explicit chromatic step down:
    {NOTE_A5, 200}, // 880 Hz
    {NOTE_GS6, 0},  // Skip (Logic fix below)

    // Simpler "Sad" Sequence:
    {NOTE_E6, 250},
    {NOTE_C6, 250},
    {NOTE_G5, 250},
    {NOTE_C5, 600}, // Lands on low C5
    {0, 0}
};
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
#include <stdlib.h> // For rand()

// Game Constants
#define MAX_LEVEL 100     // Maximum sequence length
#define START_SPEED 1000 // Initial LED speed in ms
#define SLOW_FACTOR 0.93f

// Game State
typedef enum {
    STATE_IDLE,
    STATE_SHOW_SEQUENCE,
    STATE_WAIT_FOR_INPUT,
    STATE_GAME_OVER
} GameState_t;

GameState_t currentState = STATE_IDLE;

// Sequence Memory
int simonSequence[MAX_LEVEL]; // Stores the pattern (e.g., {1, 3, 2, 1...})
int currentLevel = 1;         // How many steps to show
int playerIndex = 0;          // Which step the player is currently guessing
char msg[30];

const int startMaxTime = 30000;
volatile int currentMaxTime = startMaxTime; 	  // Max in timer (ms)
volatile int currentTime = startMaxTime;		  // Current time (ms)

volatile int decreateTimerActive = 0;	  // if set, the timer will go down at constant rate

// Your Button Struct (Keep this from before)
typedef struct {
    GPIO_TypeDef* BtnPort; uint16_t BtnPin;
    GPIO_TypeDef* LedPort; uint16_t LedPin;
    uint8_t ID;
    GPIO_PinState LastState; uint32_t LastDebounceTime;
} Button_t;

Button_t btn1 = {GPIOB, GPIO_PIN_4, GPIOC, GPIO_PIN_7, 1, GPIO_PIN_SET, 0};
Button_t btn2 = {GPIOB, GPIO_PIN_3, GPIOC, GPIO_PIN_6, 2, GPIO_PIN_SET, 0};
Button_t btn3 = {GPIOB, GPIO_PIN_5, GPIOA, GPIO_PIN_7, 3, GPIO_PIN_SET, 0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void buzzer_set_tone(uint32_t frequency);
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
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  HAL_TIM_Base_Start_IT(&htim2);

  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

  Play_Melody(snd_power_up);

  sprintf(msg, "idle,0");
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 100);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  switch (currentState) {

		case STATE_IDLE:

			// Wait for ANY button to start
			if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == 0 ||
				HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == 0 ||
				HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 0)
			{
				HAL_Delay(500); // Debounce start
				Play_Melody(snd_game_start);
				currentMaxTime = startMaxTime;
				SetTime(currentMaxTime);
				Start_New_Game();
			}
			break;

		case STATE_SHOW_SEQUENCE:
		    // Send Message to ESP32
		    sprintf(msg, "playing,%d", currentLevel - 1);
		    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 100);

			decreateTimerActive = 0;
			Play_Melody(snd_next_level);

			SetTime(currentTime + currentMaxTime/3);
			UpdateCurrentMaxTime();

			HAL_Delay(500);
			RandomNewSequence();
			// Play the pattern up to the current level
			for (int i = 0; i < currentLevel; i++) {
				Flash_LED(simonSequence[i], 300); // 600ms speed
			}

			decreateTimerActive = 1;
			currentState = STATE_WAIT_FOR_INPUT;
			break;

		case STATE_WAIT_FOR_INPUT:

		  if (currentTime == 0) {
			  currentState = STATE_GAME_OVER;
			  break;
		  }
		  // --- Button 1 Check ---
		  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == 0) {
			  HAL_Delay(50); // WAIT 50ms for noise to settle

			  // Double check: Is it STILL pressed?
			  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == 0) {
				  Check_Player_Input(1); // Register the move

				  // Critical: Wait until user RELEASES the button before continuing
				  // Otherwise, the loop runs again immediately
				  while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == 0);
			  }
		  }

		  // --- Button 2 Check ---
		  else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == 0) {
			  HAL_Delay(50);
			  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == 0) {
				  Check_Player_Input(2);
				  while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == 0);
			  }
		  }

		  // --- Button 3 Check ---
		  else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 0) {
			  HAL_Delay(50);
			  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 0) {
				  Check_Player_Input(3);
				  while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 0);
			  }
		  }
		  break;

		case STATE_GAME_OVER:
			decreateTimerActive = 0;
			Play_GameOver_Anim();
			Play_Melody(snd_game_loss);
			break;
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
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
  htim2.Init.Prescaler = 83;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 19999;
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
  htim3.Init.Prescaler = 83;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 999;
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
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin PA7 */
  GPIO_InitStruct.Pin = LD2_Pin|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB3 PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
// Turn a specific LED ON for a short time
void Flash_LED(int id, int speed) {
    GPIO_TypeDef* port;
    uint16_t pin;

    // Map ID to Pin
    if (id == 1) { port = GPIOC; pin = GPIO_PIN_7; }
    else if (id == 2) { port = GPIOB; pin = GPIO_PIN_6; }
    else { port = GPIOA; pin = GPIO_PIN_7; }

    HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);   // ON
    HAL_Delay(speed);
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET); // OFF
    HAL_Delay(speed / 2); // Gap between flashes
}

// Generate a new random sequence
void Start_New_Game() {
    currentLevel = 1;
    playerIndex = 0;
    srand(HAL_GetTick()); // Seed random number generator with time

    currentState = STATE_SHOW_SEQUENCE;
}

// Generate a new random sequence
void RandomNewSequence() {
    // Fill the array with random numbers (1, 2, or 3)
    for(int i=0; i<currentLevel; i++) {
        simonSequence[i] = (rand() % 3) + 1;
    }
}

// Flash all LEDs to show Game Over
void Play_GameOver_Anim() {
    for(int i=0; i<3; i++) {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
        HAL_Delay(200);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
        HAL_Delay(200);
    }
    // Send Message to ESP32
    sprintf(msg, "end,%d", currentLevel);
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 100);

    currentState = STATE_IDLE; // Reset to start
}

void Check_Player_Input(int btnID) {
    // 1. Flash the LED briefly so user knows they pressed it
    Flash_LED(btnID, 200);

    // 2. Check logic
    if (btnID == simonSequence[playerIndex]) {
        // CORRECT!
        playerIndex++; // Move to next expected button

        // Did they finish the whole sequence?
        if (playerIndex >= currentLevel) {
            HAL_Delay(500); // Small pause before next level
            currentLevel++;
            playerIndex = 0; // Reset player cursor

            currentState = STATE_SHOW_SEQUENCE; // Show next pattern
        }
    }
    else {
        // WRONG!
        currentState = STATE_GAME_OVER;
    }
}

void buzzer_set_tone(uint32_t frequency) {
    if (frequency == 0) {
        // If freq is 0, set duty cycle to 0 to stop sound
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
    } else {
        // 1. Calculate the new Period (ARR)
        // Formula: 1,000,000 / Frequency - 1 (because counter starts at 0)
        uint32_t arr_value = (1000000 / frequency) - 1;

        // 2. Set the new Period
        __HAL_TIM_SET_AUTORELOAD(&htim3, arr_value);

        // 3. Set Duty Cycle to 50% (Volume/Resonance is best at 50%)
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, arr_value / 2);

        // 4. Reset counter to ensure smooth transition
        __HAL_TIM_SET_COUNTER(&htim3, 0);
    }
}

void Play_Melody(const Tone *melody) {
    int i = 0;
    while (melody[i].duration != 0) {
        buzzer_set_tone(melody[i].frequency);
        HAL_Delay(melody[i].duration);
        buzzer_set_tone(0); // Stop sound briefly
        HAL_Delay(20);      // Articulation gap
        i++;
    }
    buzzer_set_tone(0); // Ensure silence at end
}

void SetTime(int time) {
	currentTime = time;
    if (currentTime > currentMaxTime) {
        currentTime = currentMaxTime; // Decrement time
    }
    if (currentTime < 0)  {
    	currentTime = 0;
    }
}

void UpdateCurrentMaxTime() {
	currentMaxTime = currentMaxTime * 85 / 100;
	if(currentMaxTime < 300) {
		currentMaxTime = 300;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    // Check if the interrupt comes from TIM2 (Servo Timer)
    if (htim->Instance == TIM2) {

        // --- OPTION 1: Auto-Countdown (Smoothest) ---
		// Since this runs at 50Hz (every 0.02 seconds),
		// we can subtract 0.02 from the time automatically.
		if (decreateTimerActive) {
			SetTime(currentTime - 20); // Decrement time
		}

        // 1. Calculate Ratio
        float ratio = (float)currentTime / (float)currentMaxTime;
        if (ratio > 1)  ratio = 1;
        else if(ratio < 0) ratio = 0;

        // 2. Map to Range (600 - 2500)
        // Range = 1900
        uint32_t pulse_width = 600 + (uint32_t)(1900 * ratio);

        // 3. Update PWM
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse_width);
    }
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
#ifdef USE_FULL_ASSERT
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
