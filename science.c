/***************************************************************************************************/
//
//      science.c
//
//      Opis:  Obsługa fotorezystora, karty pamięci, CAN, LED-ów i czujnika MICS-6814
//
//      Rzeszow 12.12.2024
//      Rev:    0.3
//      Autor:  Paweł Marszałek
/****************************************************************************************************/

/********************************** Pliki naglowkowe ************************************************/
#include "main.h"
#include "fatfs.h"
#include "can.h"
#include "spi.h"

/********************************** Zmienne globalne ************************************************/
ADC_HandleTypeDef adc1; // Zmienna dla ADC
CAN_HandleTypeDef hcan1; // Zmienna dla CAN
FATFS FatFs; // Zmienna dla systemu plików FAT
FIL file; // Zmienna dla plików
SPI_HandleTypeDef hspi1; // Zmienna dla SPI
I2C_HandleTypeDef hi2c2;      // Zmienna dla I2C (BMP390)
I2C_HandleTypeDef hi2c1; // Zmienna dla I2C (SGP40)

/********************************** Funkcje *********************************************************/
void Inicjalizuj_Fotorezystory(void);
uint32_t Odczytaj_Fotorezystor(uint32_t kanal);
void Inicjalizuj_CAN(void);
void Konfiguruj_LED(void);
void Ustaw_LED(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState stan);
void Inicjalizuj_MICS6814(void);
uint32_t Odczytaj_MICS6814(uint32_t kanal);
void Inicjalizuj_BMP390(void) {
    hi2c2.Instance = I2C2;
    hi2c2.Init.ClockSpeed = 400000;
    hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    /* Aktywacja zegara dla portów PB10 i PB11 */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Konfiguracja PB10 - I2C_C4_SCL */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2; // Ustaw alternatywne funkcje dla I2C
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Konfiguracja PB11 - I2C_C4_SDA */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    if (HAL_I2C_Init(&hi2c2) != HAL_OK)


    	void Inicjalizuj_SGP40(void) {
    	    hi2c1.Instance = I2C1;
    	    hi2c1.Init.ClockSpeed = 400000;
    	    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    	    hi2c1.Init.OwnAddress1 = 0;
    	    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    	    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    	    hi2c1.Init.OwnAddress2 = 0;
    	    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    	    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    	    /* Aktywacja zegara dla portów PC0 i PC1 */
    	    __HAL_RCC_GPIOC_CLK_ENABLE();

    	    GPIO_InitTypeDef GPIO_InitStruct = {0};

    	    /* Konfiguracja PC0 - I2C_C1_SCL */
    	    GPIO_InitStruct.Pin = GPIO_PIN_0;
    	    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    	    GPIO_InitStruct.Pull = GPIO_PULLUP;
    	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    	    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1; // Ustaw alternatywne funkcje dla I2C
    	    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    	    /* Konfiguracja PC1 - I2C_C1_SDA */
    	    GPIO_InitStruct.Pin = GPIO_PIN_1;
    	    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    	    if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
    	        // Obsługa błędu
    	        Error_Handler();
    	    }
    	}

/********************************** Inicjalizacja zmiennych globalnych ******************************/

/********************************** Blok inicjalizacji **********************************************/


    uint16_t Odczytaj_SGP40(void) {
        uint8_t command[2] = {0x26, 0x0F}; // Komenda pomiaru wilgotności (raw signal)
        uint8_t response[3]; // Odczyt danych z czujnika

        // Wyślij komendę pomiaru
        if (HAL_I2C_Master_Transmit(&hi2c1, (0x59 << 1), command, 2, HAL_MAX_DELAY) != HAL_OK) {
            Error_Handler(); // Obsługa błędu
        }

        HAL_Delay(50); // Oczekiwanie na wynik

        // Odczytaj dane
        if (HAL_I2C_Master_Receive(&hi2c1, (0x59 << 1), response, 3, HAL_MAX_DELAY) != HAL_OK) {
            Error_Handler(); // Obsługa błędu
        }

        // Połączenie bajtów w wartość 16-bitową
        return (response[0] << 8) | response[1];
    }


    /**

 *
 *  Inicjalizacja fotorezystorów
 */void Odczytaj_BMP390(float *cisnienie, float *temperatura) {
        uint8_t data[6];
        HAL_I2C_Master_Transmit(&hi2c2, (0x76 << 1), (uint8_t[]){0xF7}, 1, HAL_MAX_DELAY);
        HAL_I2C_Master_Receive(&hi2c2, (0x76 << 1) | 0x01, data, 6, HAL_MAX_DELAY);

        uint32_t adc_p = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
        uint32_t adc_t = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);

        // Proste skalowanie (docelowo kalibracja BMP390)
        *cisnienie = adc_p / 100.0f;
        *temperatura = adc_t / 100.0f;
    }

void Inicjalizuj_Fotorezystory(void) {
    ADC_ChannelConfTypeDef konfiguracja_kanalu = {0};

    /* Konfiguracja ADC */
    adc1.Instance = ADC1;
    adc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    adc1.Init.Resolution = ADC_RESOLUTION_12B;
    adc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    adc1.Init.ContinuousConvMode = DISABLE;
    adc1.Init.DiscontinuousConvMode = DISABLE;
    adc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    adc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc1.Init.NbrOfConversion = 1;

    if (HAL_ADC_Init(&adc1) != HAL_OK) {
        // Obsługa błędu
        Error_Handler();
    }

    /* Konfiguracja kanałów dla fotorezystora */
    konfiguracja_kanalu.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    konfiguracja_kanalu.SingleDiff = ADC_SINGLE_ENDED;
    konfiguracja_kanalu.OffsetNumber = ADC_OFFSET_NONE;
    konfiguracja_kanalu.Offset = 0;

    konfiguracja_kanalu.Channel = ADC_CHANNEL_14; // PC4
    if (HAL_ADC_ConfigChannel(&adc1, &konfiguracja_kanalu) != HAL_OK) {
        Error_Handler();
    }
    konfiguracja_kanalu.Channel = ADC_CHANNEL_15; // PC5
    if (HAL_ADC_ConfigChannel(&adc1, &konfiguracja_kanalu) != HAL_OK) {
        Error_Handler();
    }
    konfiguracja_kanalu.Channel = ADC_CHANNEL_8; // PB0
    if (HAL_ADC_ConfigChannel(&adc1, &konfiguracja_kanalu) != HAL_OK) {
        Error_Handler();
    }
    konfiguracja_kanalu.Channel = ADC_CHANNEL_9; // PB1
    if (HAL_ADC_ConfigChannel(&adc1, &konfiguracja_kanalu) != HAL_OK) {
        Error_Handler();
    }
}

/**
 *  Inicjalizacja czujnika MICS-6814
 */
void Inicjalizuj_MICS6814(void) {
    // W tej chwili konfiguracja ADC jest współdzielona z fotorezystorami.
    // Dalsza konfiguracja może być potrzebna w zależności od potrzeb.
}

/**
 *  Odczyt danych z MICS-6814
 */
uint32_t Odczytaj_MICS6814(uint32_t kanal) {
    ADC_ChannelConfTypeDef konfiguracja_kanalu = {0};

    konfiguracja_kanalu.Channel = kanal;
    konfiguracja_kanalu.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    konfiguracja_kanalu.SingleDiff = ADC_SINGLE_ENDED;
    konfiguracja_kanalu.OffsetNumber = ADC_OFFSET_NONE;
    konfiguracja_kanalu.Offset = 0;

    if (HAL_ADC_ConfigChannel(&adc1, &konfiguracja_kanalu) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_ADC_Start(&adc1) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_ADC_PollForConversion(&adc1, 100) != HAL_OK) {
        Error_Handler();
    }

    uint32_t wartosc = HAL_ADC_GetValue(&adc1);

    HAL_ADC_Stop(&adc1);

    return wartosc;
}
float cisnienie, temperatura;
Odczytaj_BMP390(&cisnienie, &temperatura);

/* CAN Transmisja */
CAN_TxHeaderTypeDef TxHeader;
uint8_t data[8] = {
    (uint8_t)(cisnienie), (uint8_t)(temperatura),
    wartosc_nh3 >> 8, wartosc_nh3 & 0xFF,
    wartosc_no2 >> 8, wartosc_no2 & 0xFF
};
uint32_t TxMailbox;

TxHeader.DLC = 6;
TxHeader.IDE = CAN_ID_STD;
TxHeader.RTR = CAN_RTR_DATA;
TxHeader.StdId = 0x321;

if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, data, &TxMailbox) == HAL_OK) {
    Ustaw_LED(GPIOC, GPIO_PIN_11, GPIO_PIN_SET); // LED L3 - CAN OK
}

/**
 *  Inicjalizacja magistrali CAN
 */
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

/**
 *  Konfiguracja LED
 */
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

/**
 *  Ustawienie stanu LED
 */
void Ustaw_LED(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState stan) {
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, stan);
}

/********************************** Petla glowna programu *******************************************/
int main(void) {
    HAL_Init();

    /* Inicjalizacja */
    Inicjalizuj_Fotorezystory();
    Inicjalizuj_MICS6814();
    Inicjalizuj_CAN();
    Konfiguruj_LED();

    /* Inicjalizacja systemu plików */
    if (f_mount(&FatFs, "", 1) == FR_OK) {
        Ustaw_LED(GPIOD, GPIO_PIN_10, GPIO_PIN_SET); // LED L2 - karta działa
    }

    while (1) {
        uint32_t wartosc_nh3 = Odczytaj_MICS6814(ADC_CHANNEL_1); // PA1 (NH3)
        uint32_t wartosc_no2 = Odczytaj_MICS6814(ADC_CHANNEL_2); // PA2 (NO2)
        uint32_t wartosc_co = Odczytaj_MICS6814(ADC_CHANNEL_3);  // PA3 (CO)

        /* CAN Transmisja */
        CAN_TxHeaderTypeDef TxHeader;
        uint8_t data[8] = {wartosc_nh3 >> 8, wartosc_nh3 & 0xFF, wartosc_no2 >> 8, wartosc_no2 & 0xFF};
        uint32_t TxMailbox;

        TxHeader.DLC = 4;
        TxHeader.IDE = CAN_ID_STD;
        TxHeader.RTR = CAN_RTR_DATA;
        TxHeader.StdId = 0x321;

        if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, data, &TxMailbox) == HAL_OK) {
            Ustaw_LED(GPIOC, GPIO_PIN_11, GPIO_PIN_SET); // LED L3 - CAN OK
        }

        HAL_Delay(1000); // Opóźnienie
    }
}




uint16_t wartosc_sgp40 = Odczytaj_SGP40();

/* CAN Transmisja */
CAN_TxHeaderTypeDef TxHeader;
uint8_t data[8] = {
    (wartosc_sgp40 >> 8) & 0xFF, wartosc_sgp40 & 0xFF,
    (uint8_t)(cisnienie), (uint8_t)(temperatura),
    wartosc_nh3 >> 8, wartosc_nh3 & 0xFF
};
uint32_t TxMailbox;

TxHeader.DLC = 8;
TxHeader.IDE = CAN_ID_STD;
TxHeader.RTR = CAN_RTR_DATA;
TxHeader.StdId = 0x321;

if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, data, &TxMailbox) == HAL_OK) {
    Ustaw_LED(GPIOC, GPIO_PIN_11, GPIO_PIN_SET); // LED L3 - CAN OK
}
