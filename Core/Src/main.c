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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LiquidCrystal.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
RTC_TimeTypeDef myTime;
RTC_DateTypeDef myDate;

typedef unsigned char byte;

typedef struct {
    uint16_t frequency;
    uint16_t duration;
} Tone;

typedef struct {
    char symbol; //T tank, B bullet, H health, R rocket, M manee
    char direction; // U, D, L, R
    int x, y;
    int health;
    int bullet;
    int score;
} Element;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
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
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim8;

UART_HandleTypeDef huart1;

PCD_HandleTypeDef hpcd_USB_FS;

/* USER CODE BEGIN PV */
int mode = 0;
int health = 2;
int bullet = 8;
int sounds = 1;
int music = 1;
int clear_flag = 0;
int flag = 0;
int select_line = 1;
int select_item = 1;
char name1[20];
char name2[20];
int x1 = 2, y1 = 0;
int x2 = 1, y2 = 19;
char str[100];

int sounds_play = 0;
int sounds_time;

char msg[100];


byte tankU[] = {0x00, 0x0E, 0x0E, 0x0E, 0x1F, 0x1F, 0x00, 0x00};

byte tankR[] = {0x1C, 0x1C, 0x1C, 0x1F, 0x1F, 0x1C, 0x1C, 0x1C};
byte tankD[] = {0x00, 0x00, 0x1F, 0x1F, 0x0E, 0x0E, 0x0E, 0x00};
byte tankL[] = {0x07, 0x07, 0x07, 0x1F, 0x1F, 0x07, 0x07, 0x07};

byte wall[] = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};

byte manee[] = {0x1F, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x1F};

byte heart[] = {0x00, 0x00, 0x0A, 0x1F, 0x1F, 0x0E, 0x04, 0x00};

byte rocket[] = {0x08, 0x1C, 0x08, 0x03, 0x07, 0x0E, 0x1C, 0x08};

byte box[] = {0x1F, 0x11, 0x15, 0x1D, 0x19, 0x1B, 0x1F, 0x1B};

Element board[4][20];

GPIO_TypeDef *const Row_ports[] = {GPIOD, GPIOD, GPIOD, GPIOB};
const uint16_t Row_pins[] = {GPIO_PIN_3, GPIO_PIN_5, GPIO_PIN_7, GPIO_PIN_4};
GPIO_TypeDef *const Column_ports[] = {GPIOD, GPIOD, GPIOB, GPIOB};
const uint16_t Column_pins[] =
        {GPIO_PIN_4, GPIO_PIN_6, GPIO_PIN_3, GPIO_PIN_5};
volatile uint32_t last_gpio_exti;

const Tone *volatile melody_ptr;
volatile uint16_t melody_tone_count;
volatile uint16_t current_tone_number;
volatile uint32_t current_tone_end;
volatile uint16_t volume = 100;
const Tone one[] = {{NOTE_B4,  128},
                    {NOTE_B5,  128},
                    {NOTE_FS5, 128},
                    {
                     NOTE_DS5, 128},
                    {NOTE_B5,  64},
                    {NOTE_FS5, 192},
                    {NOTE_DS5, 256},
                    {NOTE_C5,  128},
                    {NOTE_C6,  128},
                    {NOTE_G6,  128},
                    {NOTE_E6,  128},
                    {NOTE_C6,  64},
                    {NOTE_G6,  192},
                    {NOTE_E6,  256},
                    {NOTE_B4,  128},
                    {
                     NOTE_B5,  128},
                    {NOTE_FS5, 128},
                    {NOTE_DS5, 128},
                    {NOTE_B5,
                               64},
                    {NOTE_FS5, 192},
                    {NOTE_DS5, 256},
                    {NOTE_DS5, 64},
                    {
                     NOTE_E5,  64},
                    {NOTE_F5,  64},
                    {NOTE_F5,  64},
                    {NOTE_FS5, 64},
                    {NOTE_G5,  64},
                    {NOTE_G5,  64},
                    {NOTE_GS5, 64},
                    {
                     NOTE_A5,  128},
                    {NOTE_B5,  256},
                    {0,        0}};

const Tone two[] = {{REST,     750},
                    {NOTE_D4,  374},
                    {NOTE_G4,  561},
                    {
                     NOTE_AS4, 187},
                    {NOTE_A4,  374},
                    {NOTE_G4,  750},
                    {NOTE_D5,  374},
                    {
                     NOTE_C5,  1124},
                    {NOTE_A4,  1124},
                    {NOTE_G4,  561},
                    {NOTE_AS4, 187},
                    {NOTE_A4,  374},
                    {NOTE_F4,  750},
                    {NOTE_GS4, 374},
                    {NOTE_D4,  2249},
                    {NOTE_D4,  374},
                    {NOTE_G4,  561},
                    {NOTE_AS4, 187},
                    {NOTE_A4,  374},
                    {NOTE_G4,  750},
                    {NOTE_D5,  374},
                    {NOTE_F5,  750},
                    {NOTE_E5,  374},
                    {NOTE_DS5, 750},
                    {NOTE_B4,  374},
                    {NOTE_DS5, 561},
                    {NOTE_D5,  187},
                    {NOTE_CS5, 374},
                    {NOTE_CS4, 750},
                    {NOTE_B4,  374},
                    {NOTE_G4,  2249},
                    {NOTE_AS4, 374},
                    {NOTE_D5,  750},
                    {NOTE_AS4, 374},
                    {NOTE_D5,  750},
                    {NOTE_AS4, 374},
                    {NOTE_DS5, 750},
                    {NOTE_D5,  374},
                    {NOTE_CS5, 750},
                    {NOTE_A4,  374},
                    {NOTE_AS4, 561},
                    {NOTE_D5,  187},
                    {NOTE_CS5, 374},
                    {NOTE_CS4, 750},
                    {NOTE_D4,  374},
                    {NOTE_D5,  1700},
                    {REST,     150},
                    {
                     NOTE_AS4, 374},
                    {NOTE_D5,  750},
                    {NOTE_AS4, 374},
                    {NOTE_D5,
                               750},
                    {NOTE_AS4, 374},
                    {NOTE_F5,  750},
                    {NOTE_E5,  374},
                    {
                     NOTE_DS5, 750},
                    {NOTE_B4,  374},
                    {NOTE_DS5, 561},
                    {NOTE_D5,
                               187},
                    {NOTE_CS5, 374},
                    {NOTE_CS4, 750},
                    {NOTE_AS4, 374},
                    {NOTE_G4,  2249},
                    {0,        0}};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

static void MX_GPIO_Init(void);

static void MX_I2C1_Init(void);

static void MX_SPI1_Init(void);

static void MX_USB_PCD_Init(void);

static void MX_TIM2_Init(void);

static void MX_USART1_UART_Init(void);

static void MX_RTC_Init(void);

static void MX_TIM8_Init(void);

static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} pin_type;

pin_type pins[9] = {{},
                    {.port = GPIOE, .pin = GPIO_PIN_15},
                    {
                            .port = GPIOE, .pin = GPIO_PIN_8},
                    {.port = GPIOE, .pin = GPIO_PIN_9},
                    {.port = GPIOE, .pin =
                    GPIO_PIN_10},
                    {.port = GPIOE, .pin = GPIO_PIN_11},
                    {.port =
                    GPIOE, .pin = GPIO_PIN_12},
                    {.port = GPIOE, .pin = GPIO_PIN_13},
                    {.port = GPIOE, .pin =
                    GPIO_PIN_14}};

typedef struct {
    pin_type digit_activators[4];
    pin_type BCD_input[4];
    uint32_t digits[4];
    uint32_t number;
    uint32_t selected;
} seven_segment_type;

seven_segment_type seven_segment = {.digit_activators = {{.port = GPIOC,
                                                                 .pin = GPIO_PIN_0},
                                                         {.port = GPIOC, .pin = GPIO_PIN_1},
                                                         {.port =
                                                         GPIOC, .pin = GPIO_PIN_2},
                                                         {.port = GPIOC, .pin = GPIO_PIN_3}},
        .BCD_input = {{.port = GPIOF, .pin = GPIO_PIN_2},
                      {.port = GPIOA,
                              .pin = GPIO_PIN_1},
                      {.port = GPIOA, .pin = GPIO_PIN_0},
                      {
                              .port = GPIOA, .pin = GPIO_PIN_3}}, .digits = {1, 1, 0, 0},
        .number = 0};

void seven_segment_display_decimal(uint32_t n) {
    if (n < 10) {
        HAL_GPIO_WritePin(seven_segment.BCD_input[0].port,
                          seven_segment.BCD_input[0].pin,
                          (n & 1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(seven_segment.BCD_input[1].port,
                          seven_segment.BCD_input[1].pin,
                          (n & 2) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(seven_segment.BCD_input[2].port,
                          seven_segment.BCD_input[2].pin,
                          (n & 4) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(seven_segment.BCD_input[3].port,
                          seven_segment.BCD_input[3].pin,
                          (n & 8) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
}

void seven_segment_deactivate_digits(void) {
    for (int i = 0; i < 4; ++i) {
        HAL_GPIO_WritePin(seven_segment.digit_activators[i].port,
                          seven_segment.digit_activators[i].pin, GPIO_PIN_RESET);
    }
}

uint32_t select_time;

void seven_segment_activate_digit(uint32_t d) {
    if (d < 4) {
        if ((HAL_GetTick() - select_time > 150 && board[x1][y1].health <= 1
             && d == 3)
            || (HAL_GetTick() - select_time > 150
                && board[x2][y2].health <= 1 && d == 1)) {
            HAL_GPIO_TogglePin(seven_segment.digit_activators[d].port,
                               seven_segment.digit_activators[d].pin);
            select_time = HAL_GetTick();
        } else if ((board[x1][y1].health == 1 && d == 3)
                   || (board[x2][y2].health == 1 && d == 1)) {
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, 0);
        } else {
            HAL_GPIO_WritePin(seven_segment.digit_activators[d].port,
                              seven_segment.digit_activators[d].pin, GPIO_PIN_SET);
        }
    }

}

void seven_segment_set_num(uint32_t n) {
    if (n < 10000) {
        seven_segment.number = n;
        for (uint32_t i = 0; i < 4; ++i) {
            seven_segment.digits[3 - i] = n % 10;
            n /= 10;
        }
    }
}

void seven_segment_refresh(void) {
    static uint32_t state = 0;
    static uint32_t last_time = 0;
    int x = 5;
    if (HAL_GetTick() - last_time > x) {
        seven_segment_deactivate_digits();
        seven_segment_activate_digit(state);
        seven_segment_display_decimal(seven_segment.digits[state]);
        state = (state + 1) % 4;
        last_time = HAL_GetTick();
    }
}

TIM_HandleTypeDef *pwm_timer = &htim8; // Point to PWM timer configured in CubeMX
uint32_t pwm_channel = TIM_CHANNEL_1;  // Specify configured PWM channel

void PWM_Start() {
    HAL_TIM_PWM_Start(pwm_timer, pwm_channel);
}

void PWM_Change_Tone(uint16_t pwm_freq, uint16_t volume) {
    if (pwm_freq == 0 || pwm_freq > 20000) {
        __HAL_TIM_SET_COMPARE(pwm_timer, pwm_channel, 0);
    } else {
        const uint32_t internal_clock_freq = HAL_RCC_GetSysClockFreq();
        const uint16_t prescaler = 1 + internal_clock_freq / pwm_freq / 60000;
        const uint32_t timer_clock = internal_clock_freq / prescaler;
        const uint32_t period_cycles = timer_clock / pwm_freq;
        const uint32_t pulse_width = volume * period_cycles / 1000 / 2;

        pwm_timer->Instance->PSC = prescaler - 1;
        pwm_timer->Instance->ARR = period_cycles - 1;
        pwm_timer->Instance->EGR = TIM_EGR_UG;
        __HAL_TIM_SET_COMPARE(pwm_timer, pwm_channel, pulse_width);
    }
}

void Change_Melody(const Tone *melody, uint16_t tone_count) {
    melody_ptr = melody;
    melody_tone_count = tone_count;
    current_tone_number = 0;
}

void Update_Melody() {
    if ((HAL_GetTick() > current_tone_end)
        && (current_tone_number < melody_tone_count) && mode != 2) {
        const Tone active_tone = *(melody_ptr + current_tone_number);
        PWM_Change_Tone(active_tone.frequency, volume);
        current_tone_end = HAL_GetTick() + active_tone.duration;
        current_tone_number++;
        if (current_tone_number == melody_tone_count)
            current_tone_number = 0;
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
    /* USER CODE BEGIN 1 */
    sprintf(name1, "SAJJAD");
    sprintf(name2, "TAHOORA");
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
    MX_I2C1_Init();
    MX_SPI1_Init();
    MX_USB_PCD_Init();
    MX_TIM2_Init();
    MX_USART1_UART_Init();
    MX_RTC_Init();
    MX_TIM8_Init();
    MX_TIM3_Init();
    /* USER CODE BEGIN 2 */
    LiquidCrystal(GPIOD, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11,
                  GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14);
    begin(20, 4);
    HAL_GPIO_WritePin(Column_ports[0], Column_pins[0], 1);
    HAL_GPIO_WritePin(Column_ports[1], Column_pins[1], 1);
    HAL_GPIO_WritePin(Column_ports[2], Column_pins[2], 1);
    HAL_GPIO_WritePin(Column_ports[3], Column_pins[3], 1);
//  myTime.Hours = 0;
//  myTime.Minutes = 0;
//  myTime.Seconds = 0;
//  HAL_RTC_SetTime(&hrtc, &myTime, RTC_FORMAT_BIN);
//  myDate.Year = 0;
//  myDate.Month = 0;
//  myDate.Date = 0;
//  HAL_RTC_SetDate(&hrtc, &myDate, RTC_FORMAT_BIN);
    createChar(0, tankU);
    createChar(1, tankR);
    createChar(2, tankD);
    createChar(3, tankL);
    createChar(4, wall);
    createChar(5, manee);
    createChar(6, heart);
    createChar(7, rocket);
    HAL_TIM_Base_Start_IT(&htim2);
    HAL_TIM_Base_Start_IT(&htim3);
    HAL_TIM_Base_Start_IT(&htim8);
    PWM_Start();
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        if (mode == 2) {
            static uint32_t last_time = 0;
//            if (HAL_GetTick() - last_time > 30000) {
//                generate_random_items();
//                last_time = HAL_GetTick();
//            }
            print_board();
        } else if (mode == 5) {
            sprintf(msg, "END\n");
            HAL_UART_Transmit(&huart1, msg, strlen(msg), 100);
            sprintf(msg, "Score First: %d\n", board[x1][y1].score);
            HAL_UART_Transmit(&huart1, msg, strlen(msg), 100);
            sprintf(msg, "Score Second: %d\n", board[x2][y2].score);
            HAL_UART_Transmit(&huart1, msg, strlen(msg), 100);
            sprintf(msg, "**********\n");
            HAL_UART_Transmit(&huart1, msg, strlen(msg), 100);
            print_end();
            break;
        }

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI
                                       | RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB | RCC_PERIPHCLK_USART1
                                         | RCC_PERIPHCLK_I2C1 | RCC_PERIPHCLK_RTC
                                         | RCC_PERIPHCLK_TIM8;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    PeriphClkInit.USBClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
    PeriphClkInit.Tim8ClockSelection = RCC_TIM8CLK_HCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void) {

    /* USER CODE BEGIN I2C1_Init 0 */

    /* USER CODE END I2C1_Init 0 */

    /* USER CODE BEGIN I2C1_Init 1 */

    /* USER CODE END I2C1_Init 1 */
    hi2c1.Instance = I2C1;
    hi2c1.Init.Timing = 0x2000090E;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Analogue filter
    */
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Digital filter
    */
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK) {
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
static void MX_RTC_Init(void) {

    /* USER CODE BEGIN RTC_Init 0 */

    /* USER CODE END RTC_Init 0 */

    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    /* USER CODE BEGIN RTC_Init 1 */

    /* USER CODE END RTC_Init 1 */

    /** Initialize RTC Only
    */
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 39;
    hrtc.Init.SynchPrediv = 999;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if (HAL_RTC_Init(&hrtc) != HAL_OK) {
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
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK) {
        Error_Handler();
    }
    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Month = RTC_MONTH_JANUARY;
    sDate.Date = 0x2;
    sDate.Year = 0x0;

    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN RTC_Init 2 */

    /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void) {

    /* USER CODE BEGIN SPI1_Init 0 */

    /* USER CODE END SPI1_Init 0 */

    /* USER CODE BEGIN SPI1_Init 1 */

    /* USER CODE END SPI1_Init 1 */
    /* SPI1 parameter configuration*/
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 7;
    hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
    if (HAL_SPI_Init(&hspi1) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN SPI1_Init 2 */

    /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void) {

    /* USER CODE BEGIN TIM2_Init 0 */

    /* USER CODE END TIM2_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    /* USER CODE BEGIN TIM2_Init 1 */

    /* USER CODE END TIM2_Init 1 */
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 143;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 999;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM2_Init 2 */

    /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void) {

    /* USER CODE BEGIN TIM3_Init 0 */

    /* USER CODE END TIM3_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    /* USER CODE BEGIN TIM3_Init 1 */

    /* USER CODE END TIM3_Init 1 */
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 17;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 999;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM3_Init 2 */

    /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void) {

    /* USER CODE BEGIN TIM8_Init 0 */

    /* USER CODE END TIM8_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    /* USER CODE BEGIN TIM8_Init 1 */

    /* USER CODE END TIM8_Init 1 */
    htim8.Instance = TIM8;
    htim8.Init.Prescaler = 71;
    htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim8.Init.Period = 65535;
    htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim8.Init.RepetitionCounter = 0;
    htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim8) != HAL_OK) {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(&htim8) != HAL_OK) {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.BreakFilter = 0;
    sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
    sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
    sBreakDeadTimeConfig.Break2Filter = 0;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM8_Init 2 */

    /* USER CODE END TIM8_Init 2 */
    HAL_TIM_MspPostInit(&htim8);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void) {

    /* USER CODE BEGIN USART1_Init 0 */

    /* USER CODE END USART1_Init 0 */

    /* USER CODE BEGIN USART1_Init 1 */

    /* USER CODE END USART1_Init 1 */
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 38400;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN USART1_Init 2 */

    /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USB Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_PCD_Init(void) {

    /* USER CODE BEGIN USB_Init 0 */

    /* USER CODE END USB_Init 0 */

    /* USER CODE BEGIN USB_Init 1 */

    /* USER CODE END USB_Init 1 */
    hpcd_USB_FS.Instance = USB;
    hpcd_USB_FS.Init.dev_endpoints = 8;
    hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
    hpcd_USB_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
    hpcd_USB_FS.Init.low_power_enable = DISABLE;
    hpcd_USB_FS.Init.battery_charging_enable = DISABLE;
    if (HAL_PCD_Init(&hpcd_USB_FS) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN USB_Init 2 */

    /* USER CODE END USB_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOE, CS_I2C_SPI_Pin | LD4_Pin | LD3_Pin | LD5_Pin
                             | LD7_Pin | LD9_Pin | LD10_Pin | LD8_Pin
                             | LD6_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4 | GPIO_PIN_6, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3 | GPIO_PIN_5, GPIO_PIN_RESET);

    /*Configure GPIO pins : CS_I2C_SPI_Pin LD4_Pin LD3_Pin LD5_Pin
                             LD7_Pin LD9_Pin LD10_Pin LD8_Pin
                             LD6_Pin */
    GPIO_InitStruct.Pin = CS_I2C_SPI_Pin | LD4_Pin | LD3_Pin | LD5_Pin
                          | LD7_Pin | LD9_Pin | LD10_Pin | LD8_Pin
                          | LD6_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /*Configure GPIO pins : PC0 PC1 PC2 PC3 */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pin : PF2 */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    /*Configure GPIO pins : PA0 PA1 PA3 */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : PD3 PD5 PD7 */
    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /*Configure GPIO pins : PD4 PD6 */
    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /*Configure GPIO pins : PB3 PB5 */
    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pin : PB4 */
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);

    HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);

    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */

void generate_random_items() {

    int x;
    int y;
    int counter = 0;

    while (counter != 1) {
        x = HAL_GetTick() % 4;
        y = HAL_GetTick() % 21;
        if (board[x][y].symbol == 'E') {
            board[x][y].symbol = 'H';
            counter++;
        }
    }
    counter = 0;
    while (counter != 1) {
        x = HAL_GetTick() % 4;
        y = HAL_GetTick() % 21;
        if (board[x][y].symbol == 'E') {
            board[x][y].symbol = 'R';
            counter++;
        }
    }
}

void initial_board() {
    sprintf(msg, "**********\n");
    HAL_UART_Transmit(&huart1, msg, strlen(msg), 100);
    sprintf(msg, "First Player: %s\n", name1);
    HAL_UART_Transmit(&huart1, msg, strlen(msg), 100);
    sprintf(msg, "Second Player: %s\n", name2);
    HAL_UART_Transmit(&huart1, msg, strlen(msg), 100);
    sprintf(msg, "START");
    HAL_UART_Transmit(&huart1, msg, strlen(msg), 100);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 20; j++) {
            board[i][j].symbol = 'E';
        }
    }

    board[2][0].symbol = '1';
    board[2][0].direction = 'R';
    board[2][0].x = 2;
    board[2][0].y = 0;
    board[2][0].bullet = bullet;
    board[2][0].health = health;
    board[2][0].score = 0;

    board[1][19].symbol = '2';
    board[1][19].direction = 'L';
    board[1][19].x = 1;
    board[1][19].y = 19;
    board[1][19].bullet = bullet;
    board[1][19].health = health;
    board[1][19].score = 0;

    board[1][1].symbol = 'W';
    board[2][1].symbol = 'W';
    board[1][18].symbol = 'W';
    board[2][18].symbol = 'W';

    int x;
    int y;
    int counter = 0;

    while (counter != 1) {
//        x = rand() % 4;
//        y = rand() % 21;
        x = HAL_GetTick() % 4;
        y = HAL_GetTick() % 21;
        if (board[x][y].symbol == 'E') {
            board[x][y].symbol = 'H';
            counter++;
        }
    }
    counter = 0;
    while (counter != 1) {
        x = HAL_GetTick() % 4;
        y = HAL_GetTick() % 21;
        if (board[x][y].symbol == 'E') {
            board[x][y].symbol = 'R';
            counter++;
        }
    }
    counter = 0;
    while (counter != 1) {
        x = HAL_GetTick() % 4;
        y = HAL_GetTick() % 21;
        if (board[x][y].symbol == 'E') {
            board[x][y].symbol = 'M';
            counter++;
        }
    }
}

int check_next_bullet(int x, int y) {

    if (x == -1 || x == 4 || y == -1 || y == 20) {
        return 0;
    }

    switch (board[x][y].symbol) {
        case '*':
            board[x][y].symbol = 'E';
            return 0;
        case 'E':
            return 1;
        case 'H':
        case 'R':
            return 2;
        case 'M':
            board[x][y].symbol = 'E';
            return 1;
        case 'W':
            return 0;
        case '1':
            board[x1][y1].health--;
            board[x2][y2].score++;
            if (sounds)
                PWM_Change_Tone(300, 100);
            sounds_play = 1;
            sounds_time = HAL_GetTick();
            return 0;
        case '2':
            board[x2][y2].health--;
            board[x1][y1].score++;
            if (sounds)
                PWM_Change_Tone(300, 100);
            sounds_play = 1;
            sounds_time = HAL_GetTick();
            return 0;

    }
    return 0;
}

void print_end() {
    clear();
    setCursor(0, 0);
    print("END");
    if (board[x1][y1].health > board[x2][y2].health) {
        setCursor(0, 1);
        sprintf(str, "Winner: %s", name1);
        print(str);
        setCursor(0, 2);
        sprintf(str, "Score: %d", board[x1][y1].score);
        print(str);
    } else if (board[x1][y1].health < board[x2][y2].health) {
        setCursor(0, 1);
        sprintf(str, "Winner: %s", name2);
        print(str);
        setCursor(0, 2);
        sprintf(str, "Score: %d", board[x2][y2].score);
        print(str);
    } else {
        setCursor(0, 1);
        sprintf(str, "Winner: %s", name1);
        print(str);
        setCursor(0, 2);
        sprintf(str, "Winner: %s", name2);
        print(str);
        setCursor(0, 3);
        sprintf(str, "Score: %d", board[x2][y2].score);
        print(str);
    }
}

void print_board() {
    static uint32_t last_time = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 20; j++) {
            switch (board[i][j].symbol) {
                //T tank, B bullet, H health, R rocket, M manee
                case '1':
                case '2':
                    switch (board[i][j].direction) {
                        case 'U':
                            setCursor(j, i);
                            write(0);
                            break;
                        case 'R':
                            setCursor(j, i);
                            write(1);
                            break;
                        case 'D':
                            setCursor(j, i);
                            write(2);
                            break;
                        case 'L':
                            setCursor(j, i);
                            write(3);
                            break;
                    }
                    break;
                case 'H':
                    setCursor(j, i);
                    write(6);
                    break;
                case 'R':
                    setCursor(j, i);
                    write(7);
                    break;
                case 'M':
                    setCursor(j, i);
                    write(5);
                    break;
                case 'W':
                    setCursor(j, i);
                    write(4);
                    break;
                case 'E':
                    setCursor(j, i);
                    print(" ");
                    break;
                case '*':
                    if (HAL_GetTick() - last_time > 30) {
                        int result;
                        switch (board[i][j].direction) {
                            case 'U':
                                result = check_next_bullet(i - 1, j);
                                if (result == 0) {
                                    setCursor(j, i);
                                    print(" ");
                                    board[i][j].symbol = 'E';
                                } else if (result == 1) {
                                    setCursor(j, i - 1);
                                    print("*");
                                    board[i - 1][j] = board[i][j];

                                    setCursor(j, i);
                                    write(' ');
                                    board[i][j].symbol = 'E';

                                } else if (result == 2 && i - 2 >= 0) {
                                    setCursor(j, i - 2);
                                    print("*");
                                    board[i - 2][j] = board[i][j];

                                    setCursor(j, i);
                                    write(' ');
                                    board[i][j].symbol = 'E';
                                }
                                break;
                            case 'R':
                                result = check_next_bullet(i, j + 1);
                                if (result == 0) {
                                    setCursor(j, i);
                                    print(" ");
                                    board[i][j].symbol = 'E';
                                } else if (result == 1) {
                                    setCursor(j + 1, i);
                                    print("*");
                                    board[i][j + 1] = board[i][j];

                                    setCursor(j, i);
                                    print(" ");
                                    board[i][j].symbol = 'E';

                                } else if (result == 2 && j + 2 <= 19) {
                                    setCursor(j + 2, i);
                                    print("*");
                                    board[i][j + 2] = board[i][j];

                                    setCursor(j, i);
                                    print(" ");
                                    board[i][j].symbol = 'E';
                                }
                                break;
                            case 'D':
                                result = check_next_bullet(i + 1, j);
                                if (result == 0) {
                                    setCursor(j, i);
                                    print(" ");
                                    board[i][j].symbol = 'E';
                                } else if (result == 1) {
                                    setCursor(j, i + 1);
                                    print("*");
                                    board[i + 1][j] = board[i][j];

                                    setCursor(j, i);
                                    print(" ");
                                    board[i][j].symbol = 'E';

                                } else if (result == 2 && i + 2 <= 3) {
                                    setCursor(j, i + 2);
                                    print("*");
                                    board[i + 2][j] = board[i][j];

                                    setCursor(j, i);
                                    print(" ");
                                    board[i][j].symbol = 'E';
                                }
                                break;
                            case 'L':
                                result = check_next_bullet(i, j - 1);
                                if (result == 0) {
                                    setCursor(j, i);
                                    print(" ");
                                    board[i][j].symbol = 'E';
                                } else if (result == 1) {
                                    setCursor(j - 1, i);
                                    print("*");
                                    board[i][j - 1] = board[i][j];

                                    setCursor(j, i);
                                    print(" ");
                                    board[i][j].symbol = 'E';

                                } else if (result == 2 && j - 2 >= 0) {
                                    setCursor(j - 2, i);
                                    print("*");
                                    board[i][j - 2] = board[i][j];

                                    setCursor(j, i);
                                    print(" ");
                                    board[i][j].symbol = 'E';
                                }
//                                else if (j - 2 < 0) {
//                                    setCursor(j, i);
//                                    print(" ");
//                                    board[i][j].symbol = 'E';
//                                }
                                break;
                        }
                        last_time = HAL_GetTick();
                    }
                    break;
            }
        }
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

    if (htim->Instance == TIM3 && mode == 2) {
        seven_segment_refresh();

        if (board[x1][y1].health > 9) {
            board[x1][y1].health = 9;
        }
        if (board[x2][y2].health > 9) {
            board[x2][y2].health = 9;
        }
        if (board[x1][y1].bullet > 9) {
            board[x1][y1].bullet = 9;
        }
        if (board[x2][y2].bullet > 9) {
            board[x2][y2].bullet = 9;
        }

        if (board[x1][y1].bullet < 0) {
            board[x1][y1].bullet = 0;
        }
        if (board[x2][y2].bullet < 0) {
            board[x2][y2].bullet = 0;
        }
    }

    //LCD
    if (htim->Instance == TIM2) {

        if (sounds_play == 1 && HAL_GetTick() - sounds_time > 500) {
            PWM_Change_Tone(0, 0);
        }

        int remain = 0;
        int col = 0;
        if (mode == 0 && music == 1) {
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
            Change_Melody(two, ARRAY_LENGTH(two));
            music = 0;
        }
        switch (mode) {

            //Initial
            case (0):

                if (clear_flag) {
                    clear_flag = 0;
                    clear();
                }
                setCursor(8, 1);
                print("TANK");
                setCursor(7, 2);
                print("BATTLE");
                setCursor(5, 3);
                write(0);
                setCursor(14, 3);
                write(0);
                setCursor(5, 2);
                print("*");
                setCursor(14, 2);
                print("*");
                break;

                //Menu
            case (1):
                if (clear_flag) {
                    clear_flag = 0;
                    clear();
                }
                setCursor(1, 0);
                print("Start Game");
                setCursor(1, 1);
                print("Settings");
                setCursor(1, 2);
                print("About");
                switch (select_line) {
                    case (1):
                        setCursor(0, 1);
                        print(" ");
                        setCursor(0, 2);
                        print(" ");
                        setCursor(0, 0);
                        write(1);
                        break;
                    case (2):
                        setCursor(0, 0);
                        print(" ");
                        setCursor(0, 2);
                        print(" ");
                        setCursor(0, 1);
                        write(1);
                        break;
                    case (3):
                        setCursor(0, 0);
                        print(" ");
                        setCursor(0, 1);
                        print(" ");
                        setCursor(0, 2);
                        write(1);
                        break;
                }
                break;

                //Start
            case (2):
                if (clear_flag) {
                    clear_flag = 0;
                    clear();
                    initial_board();
                    print_board();
                }
                seven_segment.digits[1] = board[x2][y2].health;
                seven_segment.digits[0] = board[x2][y2].bullet;
                seven_segment.digits[3] = board[x1][y1].health;
                seven_segment.digits[2] = board[x1][y1].bullet;
                if (board[x1][y1].health == 0 || board[x2][y2].health == 0) {
                    mode = 5;
                    clear_flag = 1;
                    flag = 1;
                    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
                }

                break;

                //Settings
            case (3):
                if (clear_flag) {
                    clear_flag = 0;
                    clear();
                }
                setCursor(0, 0);
                sprintf(str, "1) %s", name1);
                print(str);
                setCursor(0, 1);
                sprintf(str, "2) %s", name2);
                print(str);
                setCursor(1, 2);
                sprintf(str, "Health:%d", health);
                print(str);
                setCursor(11, 2);
                sprintf(str, "Bullet:%d", bullet);
                print(str);
                setCursor(1, 3);
                sprintf(str, "Sounds:%d", sounds);
                print(str);
                switch (select_item) {
                    case (1):
                        print(" ");
                        setCursor(0, 3);
                        print(" ");
                        setCursor(10, 2);
                        print(" ");
                        setCursor(0, 2);
                        write(1);
                        break;
                    case (2):
                        setCursor(0, 2);
                        print(" ");
                        setCursor(0, 3);
                        print(" ");
                        setCursor(10, 2);
                        write(1);
                        break;
                    case (3):
                        setCursor(0, 2);
                        print(" ");
                        setCursor(10, 2);
                        print(" ");
                        setCursor(0, 3);
                        write(1);
                        break;
                }
                break;

                //About
            case (4):
                if (clear_flag) {
                    clear_flag = 0;
                    clear();
                }
                setCursor(0, 0);
                write(1);
                setCursor(19, 0);
                write(3);
                setCursor(1, 0);
                print("TAHOORA and SAJJAD");
                HAL_RTC_GetTime(&hrtc, &myTime, RTC_FORMAT_BIN);
                HAL_RTC_GetDate(&hrtc, &myDate, RTC_FORMAT_BIN);
                setCursor(0, 2);
                sprintf(str, "TIME= %2d:%2d:%2d", myTime.Hours, myTime.Minutes,
                        myTime.Seconds);
                print(str);
                setCursor(0, 3);
                sprintf(str, "DATE= %2d/%2d/%2d", myDate.Year, myDate.Month,
                        myDate.Date);
                print(str);
                break;
        }
    }
}

void change_direction_tank(char tank) {
    int x, y;
    x = tank == '1' ? x1 : x2;
    y = tank == '1' ? y1 : y2;

    switch (board[x][y].direction) {
        case 'U':
            board[x][y].direction = 'R';
            break;
        case 'R':
            board[x][y].direction = 'D';
            break;
        case 'D':
            board[x][y].direction = 'L';
            break;
        case 'L':
            board[x][y].direction = 'U';
            break;
    }
}

int check_next(int x, int y, int tank) {
    Element next_element = board[x][y];

    switch (next_element.symbol) {
        case 'E':
            return 1;
        case 'H':
            next_element.symbol = 'E';
            if (tank == 1)
                board[x1][y1].health++;
            else
                board[x2][y2].health++;
            return 1;
        case 'R':
            next_element.symbol = 'E';
            if (tank == 1)
                board[x1][y1].bullet++;
            else
                board[x2][y2].bullet++;
            return 1;
        case 'M':
        case 'W':
        case '1':
        case '2':
            return 0;
    }
    return 0;
}

void move(char tank) {
    int x, y;
    x = tank == '1' ? x1 : x2;
    y = tank == '1' ? y1 : y2;

    switch (board[x][y].direction) {
        case 'U':
            if (x != 0) {
                int result =
                        tank == '1' ?
                        check_next(x - 1, y, 1) : check_next(x - 1, y, 2);
                if (result) {
                    if (tank == '1') {
                        x1 = x1 - 1;
                    } else {
                        x2 = x2 - 1;
                    }
                    board[x - 1][y] = board[x][y];
                    board[x][y].symbol = 'E';
                }
            }
            break;
        case 'R':
            if (y != 19) {
                int result =
                        tank == '1' ?
                        check_next(x, y + 1, 1) : check_next(x, y + 1, 2);
                if (result) {
                    if (tank == '1') {
                        y1 = y1 + 1;
                    } else {
                        y2 = y2 + 1;
                    }
                    board[x][y + 1] = board[x][y];
                    board[x][y].symbol = 'E';
                }
            }
            break;
        case 'D':
            if (x != 3) {
                int result =
                        tank == '1' ?
                        check_next(x + 1, y, 1) : check_next(x + 1, y, 2);

                if (result) {
                    board[x + 1][y] = board[x][y];
                    board[x][y].symbol = 'E';
                    if (tank == '1') {
                        x1 = x1 + 1;
                    } else {
                        x2 = x2 + 1;
                    }
                }
            }
            break;
        case 'L':
            if (y != 0) {
                int result =
                        tank == '1' ?
                        check_next(x, y - 1, 1) : check_next(x, y - 1, 2);

                if (result) {
                    if (tank == '1') {
                        y1 = y1 - 1;
                    } else {
                        y2 = y2 - 1;
                    }
                    board[x][y - 1] = board[x][y];
                    board[x][y].symbol = 'E';
                }
            }
            break;
    }
}

void fire(char tank) {
    int x, y;
    x = tank == '1' ? x1 : x2;
    y = tank == '1' ? y1 : y2;
    switch (board[x][y].direction) {
        case 'U':
            if (x != 0 && board[x - 1][y].symbol != 'W') {
                board[x - 1][y].symbol = '*';
                board[x - 1][y].direction = 'U';
            }
            break;
        case 'R':
            if (y != 19 && board[x][y + 1].symbol != 'W') {
                board[x][y + 1].symbol = '*';
                board[x][y + 1].direction = 'R';
            }
            break;
        case 'D':
            if (x != 3 && board[x + 1][y].symbol != 'W') {
                board[x + 1][y].symbol = '*';
                board[x + 1][y].direction = 'D';
            }
            break;
        case 'L':
            if (y != 0 && board[x][y - 1].symbol != 'W') {
                board[x][y - 1].symbol = '*';
                board[x][y - 1].direction = 'L';
            }
            break;
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (last_gpio_exti + 150 > HAL_GetTick()) // Simple button debouncing
    {
        return;
    }
    last_gpio_exti = HAL_GetTick();

    int8_t row_number = -1;
    int8_t column_number = -1;

    if (GPIO_Pin == GPIO_PIN_0) {
        // blue_button_pressed = 1;
        // return;
    }

    for (uint8_t row = 0; row < 4; row++) // Loop through Rows
    {
        if (GPIO_Pin == Row_pins[row]) {
            row_number = row;
        }
    }

    HAL_GPIO_WritePin(Column_ports[0], Column_pins[0], 0);
    HAL_GPIO_WritePin(Column_ports[1], Column_pins[1], 0);
    HAL_GPIO_WritePin(Column_ports[2], Column_pins[2], 0);
    HAL_GPIO_WritePin(Column_ports[3], Column_pins[3], 0);

    for (uint8_t col = 0; col < 4; col++) // Loop through Columns
    {
        HAL_GPIO_WritePin(Column_ports[col], Column_pins[col], 1);
        if (HAL_GPIO_ReadPin(Row_ports[row_number], Row_pins[row_number])) {
            column_number = col;
        }
        HAL_GPIO_WritePin(Column_ports[col], Column_pins[col], 0);
    }

    HAL_GPIO_WritePin(Column_ports[0], Column_pins[0], 1);
    HAL_GPIO_WritePin(Column_ports[1], Column_pins[1], 1);
    HAL_GPIO_WritePin(Column_ports[2], Column_pins[2], 1);
    HAL_GPIO_WritePin(Column_ports[3], Column_pins[3], 1);

    if (row_number == -1 || column_number == -1) {
        return; // Reject invalid scan
    }
    //   C0   C1   C2   C3
    // +----+----+----+----+
    // | 1  | 2  | 3  | 4  |  R0
    // +----+----+----+----+
    // | 5  | 6  | 7  | 8  |  R1
    // +----+----+----+----+
    // | 9  | 10 | 11 | 12 |  R2
    // +----+----+----+----+
    // | 13 | 14 | 15 | 16 |  R3
    // +----+----+----+----+
    const uint8_t button_number = row_number * 4 + column_number + 1;
    char str[10];
    switch (button_number) {
        case 5: //s3
            if (mode == 2)
                change_direction_tank('2');
            break;
        case 6: //s7
            if (mode == 2) {
                move('2');
            }


            break;
        case 7: //s11
            if (mode == 2) {
                if (board[x2][y2].bullet != 0) {
                    fire('2');
                    if (sounds)
                        PWM_Change_Tone(150, 100);
                    sounds_play = 1;
                    sounds_time = HAL_GetTick();
                    board[x2][y2].bullet--;
                }

            }
            break;
        case 8: //s15

            break;
        case 13: //s1
            if (mode == 0) {
                mode = 1;
                clear_flag = 1;
            } else if (mode == 1) {
                select_line = (select_line % 3) + 1;
            } else if (mode == 2) {
                change_direction_tank('1');
            } else if (mode == 3) {
                select_item = (select_item % 3) + 1;
            }
            break;
        case 14: //s5
            if (mode == 1) {
                mode = select_line + 1;
                if (mode == 2)
                    PWM_Change_Tone(0, 0);
                clear_flag = 1;
            } else if (mode == 2) {
                move('1');
            } else if (mode == 3) {
                switch (select_item) {
                    case 1:
                        health = (health % 9) + 1;
                        break;
                    case 2:
                        bullet = (bullet % 9) + 1;
                        break;

                    case 3:
                        sounds = (sounds + 1) % 2;
                        break;

                }
            }
            break;
        case 15: //s9
            if (mode == 2) {
                if (board[x1][y1].bullet != 0) {
                    fire('1');
                    if (sounds)
                        PWM_Change_Tone(150, 100);
                    sounds_play = 1;
                    sounds_time = HAL_GetTick();
                    board[x1][y1].bullet--;
                }
            }
            break;
        case 16: //s13
            if (mode == 3 || mode == 4) {
                mode = 1;
                clear_flag = 1;
            }
            break;

        default:
            break;
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
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
