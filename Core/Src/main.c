/***************************************************************************************************/
//      science.c
//
//      Opis: Obsługa fotorezystora, CAN, LED-ów i czujnika MICS-6814
//
//      Rzeszow 12.12.2024
//      Rev:    0.3
//      Autor:  Paweł Marszałek
/***************************************************************************************************/

/********************************** Pliki nagłówkowe ************************************************/
#include "main.h"

/********************************** Zmienne globalne ************************************************/
ADC_HandleTypeDef hadc1; // Zmienna dla ADC
CAN_HandleTypeDef hcan1; // Zmienna dla CAN
SPI_HandleTypeDef hspi1; // Zmienna dla SPI
I2C_HandleTypeDef hi2c2; // Zmienna dla I2C (BMP390)
I2C_HandleTypeDef hi2c1; // Zmienna dla I2C (SGP40)

/********************************** Funkcje *********************************************************/
void Inicjalizuj_Fotorezystory(void);
uint32_t Odczytaj_Fotorezystor(uint32_t kanal);
void Inicjalizuj_CAN(void);
void Konfiguruj_LED(void);
void Ustaw_LED(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState stan);
void Inicjalizuj_MICS6814(void);
uint32_t Odczytaj_MICS6814(uint32_t kanal);
void Inicjalizuj_BMP390(void);
void Inicjalizuj_SGP40(void);
uint16_t Odczytaj_SGP40(void);
void Odczytaj_BMP390(float *cisnienie, float *temperatura);
void Inicjalizuj_ADC(void);
void Inicjalizuj_I2C(void);
void Error_Handler(void);

/********************************** Funkcja main ******************************************************/
int main(void) {
    // Inicjalizacje systemu HAL
    HAL_Init();

    // Inicjalizacja wszystkich modułów
    Inicjalizuj_ADC();
    Inicjalizuj_CAN();
    Konfiguruj_LED();
    Inicjalizuj_MICS6814();
    Inicjalizuj_BMP390();
    Inicjalizuj_SGP40();
    Inicjalizuj_I2C();

    // Zapalenie wszystkich LEDów
    Ustaw_LED(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    Ustaw_LED(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);
    Ustaw_LED(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);
    Ustaw_LED(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);

    // Główna pętla programu
    while (1) {
        uint32_t wartosc_foto1 = Odczytaj_Fotorezystor(ADC_CHANNEL_14);  // PC4

        float cisnienie, temperatura;
        Odczytaj_BMP390(&cisnienie, &temperatura);

        // Logika sterowania LED
        if (wartosc_foto1 > 2000) {
            Ustaw_LED(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
        } else {
            Ustaw_LED(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
        }

        HAL_Delay(1000); // Przerwa 1s
    }
}

/********************************** Inicjalizacja funkcji **********************************************/

void Inicjalizuj_Fotorezystory(void) {
    ADC_ChannelConfTypeDef konfiguracja_kanalu = {0};

    /* Konfiguracja ADC */
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;

    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        Error_Handler();
    }

    konfiguracja_kanalu.SamplingTime = ADC_SAMPLETIME_2CYCLE_5;
    konfiguracja_kanalu.SingleDiff = ADC_SINGLE_ENDED;
    konfiguracja_kanalu.OffsetNumber = ADC_OFFSET_NONE;
    konfiguracja_kanalu.Offset = 0;

    konfiguracja_kanalu.Channel = ADC_CHANNEL_14; // PC4
    if (HAL_ADC_ConfigChannel(&hadc1, &konfiguracja_kanalu) != HAL_OK) {
        Error_Handler();
    }
}

uint32_t Odczytaj_Fotorezystor(uint32_t kanal) {
    ADC_ChannelConfTypeDef konfiguracja_kanalu = {0};

    konfiguracja_kanalu.Channel = kanal;
    konfiguracja_kanalu.SamplingTime = ADC_SAMPLETIME_2CYCLE_5;
    konfiguracja_kanalu.SingleDiff = ADC_SINGLE_ENDED;
    konfiguracja_kanalu.OffsetNumber = ADC_OFFSET_NONE;
    konfiguracja_kanalu.Offset = 0;

    if (HAL_ADC_ConfigChannel(&hadc1, &konfiguracja_kanalu) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_ADC_Start(&hadc1) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_ADC_PollForConversion(&hadc1, 100) != HAL_OK) {
        Error_Handler();
    }

    uint32_t wartosc = HAL_ADC_GetValue(&hadc1);

    HAL_ADC_Stop(&hadc1);

    return wartosc;
}

void Inicjalizuj_CAN(void) {
    hcan1.Instance = CAN1;
    hcan1.Init.Prescaler = 16; // Dostosuj do prędkości CAN
    hcan1.Init.Mode = CAN_MODE_NORMAL;
    hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan1.Init.TimeSeg1 = CAN_BS1_13TQ;
    hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
    hcan1.Init.AutoBusOff = DISABLE;
    hcan1.Init.AutoWakeUp = DISABLE;
    hcan1.Init.AutoRetransmission = ENABLE;
    hcan1.Init.ReceiveFifoLocked = DISABLE;
    hcan1.Init.TransmitFifoPriority = DISABLE;

    if (HAL_CAN_Init(&hcan1) != HAL_OK) {
        Error_Handler();
    }
}

void Konfiguruj_LED(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Konfiguracja pinów LED */
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void Ustaw_LED(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState stan) {
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, stan);
}

void Inicjalizuj_MICS6814(void) {
    // W tej chwili konfiguracja ADC jest współdzielona z fotorezystorami.
    // Dalsza konfiguracja może być potrzebna w zależności od potrzeb.
}

uint32_t Odczytaj_MICS6814(uint32_t kanal) {
    return Odczytaj_Fotorezystor(kanal);  // Można tu dodać specyficzną obsługę MICS6814
}

void Inicjalizuj_BMP390(void) {
    hi2c2.Instance = I2C2;
    hi2c2.Init.Timing = 0x00707CBB;
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    if (HAL_I2C_Init(&hi2c2) != HAL_OK) {
        Error_Handler();
    }
}

void Inicjalizuj_SGP40(void) {
    hi2c1.Instance = I2C1;
    hi2c1.Init.Timing = 0x00707CBB;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void Error_Handler(void) {
    while (1) {
        // Tutaj można dodać np. miganie LED w przypadku błędu
    }
}

void Inicjalizuj_ADC(void) {
    // Tymczasowa implementacja inicjalizacji ADC
}

void Inicjalizuj_I2C(void) {
    // Tymczasowa implementacja inicjalizacji I2C
}

void Odczytaj_BMP390(float *cisnienie, float *temperatura) {
    *cisnienie = 1013.25;  // Przykładowa wartość ciśnienia
    *temperatura = 25.0;   // Przykładowa wartość temperatury
}

uint16_t Odczytaj_SGP40(void) {
    return 500;  // Przykładowa wartość odczytu
}
