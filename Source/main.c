#include "main.h"

UART_HandleTypeDef UartHandle;
__IO ITStatus UartReady = RESET;

uint8_t aTxBuffer[] = "Greetings from STM32L432! 115200 8N1\r\n";

uint8_t aRxBuffer[RXBUFFERSIZE];

void SystemClock_Config(void);
static void Error_Handler(void);
static uint16_t Buffercmp(uint8_t * pBuffer1, uint8_t * pBuffer2, uint16_t BufferLength);

int main(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    HAL_Init();

    SystemClock_Config();

    BSP_LED_Init(LED3);
    UartHandle.Instance = USARTx;
    UartHandle.Init.BaudRate = 115200;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits = UART_STOPBITS_1;
    UartHandle.Init.Parity = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode = UART_MODE_TX_RX;
    if (HAL_UART_DeInit(&UartHandle) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_UART_Init(&UartHandle) != HAL_OK)
    {
        Error_Handler();
    }

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;


    __HAL_RCC_GPIOA_CLK_ENABLE();

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    NVIC_SetPriority((IRQn_Type)(EXTI15_10_IRQn), 0x03);
    HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI15_10_IRQn));

    BSP_LED_Off(LED3);
    while (1)
    {
        BSP_LED_On(LED3);
        if (HAL_UART_Transmit_IT(&UartHandle, (uint8_t *)aTxBuffer, TXBUFFERSIZE) != HAL_OK)
        {
            Error_Handler();
        }

        while (UartReady != SET)
        {
        }

        UartReady = RESET;
        BSP_LED_Off(LED3);
        
        HAL_Delay(1000);
    }
}

void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};

    /* MSI is enabled after System reset, activate PLL with MSI as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
    RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 40;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLP = 7;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        /* Initialization Error */
        while (1)
            ;
    }

    RCC_ClkInitStruct.ClockType =
        (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        /* Initialization Error */
        while (1)
            ;
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef * UartHandle)
{
    UartReady = SET;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * UartHandle)
{
    UartReady = SET;
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef * UartHandle) { Error_Handler(); }

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_12)
    {
        //
    }
}

static void Error_Handler(void)
{
    BSP_LED_On(LED3);
    while (1)
    {
        BSP_LED_Toggle(LED3);
        HAL_Delay(1000);
    }
}