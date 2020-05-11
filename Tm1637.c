#include "Tm1637.h"

void _tm1637Start(_Bool ifRed);
void _tm1637Stop(_Bool ifRed);
void _tm1637ReadResult(_Bool ifRed);
void _tm1637WriteByte(unsigned char b, _Bool ifRed);
void _tm1637DelayUsec(unsigned int i);

void _tm1637ClkHigh(_Bool ifRed);
void _tm1637ClkLow(_Bool ifRed);
void _tm1637DioHigh(_Bool ifRed);
void _tm1637DioLow(_Bool ifRed);

/***************************************************************************************/

const char segmentMap[] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, // 0-7
    0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, // 8-9, A-F
    0x00
};

/***************************************************************************************/

/*!
 * \brief Funkcja dispBlink służy do migania wyświetlaczem
 *
 * \param ifRed - czy funkcja ma dotyczyć wyświetlacza czerwonego, czy może niebieskiego.
 *
 * Funkcja nie zwraca żadnych wartości.
 */

void dispBlink(_Bool ifRed) {
	tm1637SetBriOne(0, ifRed);
	HAL_Delay(250);
	tm1637SetBriOne(asgClock.bright, ifRed);
	HAL_Delay(250);
}

/***************************************************************************************/

/*!
 * \brief Funkcja int2Time służy do konwersji liczby sekund na liczbę w formacie MMSS
 *
 * \param integer - liczba sekund.
 *
 * \return min*100+sec - czas w formacie MMSS.
 */

uint16_t int2Time(uint16_t integer) {

	uint16_t min, sec;
	min = integer / 60;
	sec = integer % 60;

	return min*100 + sec;
}

/***************************************************************************************/

/*!
 * \brief Funkcja mt1637DisplayDecimal służy do ustawienia liczby na wyświetlaczu
 *
 * \param v - wartość wyświetlana,
 * \param displaySeparator - dwukropek,
 * \param ifRed - czy funkcja ma dotyczyć wyświetlacza czerwonego, czy może niebieskiego.
 *
 * Funkcja nie zwraca żadnych wartości.
 */

void tm1637DisplayDecimal(uint16_t v, _Bool displaySeparator, _Bool ifRed)
{
    unsigned char digitArr[4];
    for (int i = 0; i < 4; ++i) {
        digitArr[i] = segmentMap[v % 10];
        if (i == 2 && displaySeparator) {
            digitArr[i] |= 1 << 7;
        }
        v /= 10;
    }

    _tm1637Start(ifRed);
    _tm1637WriteByte(0x40, ifRed);
    _tm1637Stop(ifRed);

    _tm1637Start(ifRed);
    _tm1637WriteByte(0xc0, ifRed);

    for (int i = 0; i < 4; ++i) {
        _tm1637WriteByte(digitArr[3 - i], ifRed);
    }

    _tm1637Stop(ifRed);
}

/***************************************************************************************/

/*!
 * \brief Funkcja tm1637SetBrightness służy do ustawienia jasności obu wyświetlaczy (poniżej wersja dotycząca jednego wyświetlacza)
 *
 * \param brightness - poziom jasności.
 *
 * Funkcja nie zwraca żadnych wartości.
 */

// Dozwolone wartości 0-8, gdzie 0 to wyświetlacze zgaszone
void tm1637SetBrightness(char brightness) {

	/*
    Sposób zapisu:
    1000 0BBB - wyświetlacz zgaszony,
    1000 1BBB - wyświetlacz zapalony, jasności 0-7,
    BBB - jasność (binarnie).
	*/

    _tm1637Start(RED);
    _tm1637WriteByte(0x87 + brightness, RED);
    _tm1637Stop(RED);

    _tm1637Start(BLUE);
    _tm1637WriteByte(0x87 + brightness, BLUE);
    _tm1637Stop(BLUE);
}

// Wersja do ustawienia jasności pojedynczego wyświetlacza
void tm1637SetBriOne(char brightness, _Bool ifRed) {

    _tm1637Start(ifRed);
    _tm1637WriteByte(0x87 + brightness, ifRed);
    _tm1637Stop(ifRed);
}

/***************************************************************************************/

/*!
 * \brief Funkcja _tm1637Start_ służy do rozpoczęcia transmisji
 *
 * \param ifRed - czy funkcja ma dotyczyć wyświetlacza czerwonego, czy może niebieskiego.
 *
 * Funkcja nie zwraca żadnych wartości.
 */

void _tm1637Start(_Bool ifRed) {

    _tm1637ClkHigh(ifRed);
    _tm1637DioHigh(ifRed);
    _tm1637DelayUsec(2);
    _tm1637DioLow(ifRed);
}

/***************************************************************************************/

/*!
 * \brief Funkcja _tm1637Stop_ służy do zakończenia transmisji
 *
 * \param ifRed - czy funkcja ma dotyczyć wyświetlacza czerwonego, czy może niebieskiego.
 *
 * Funkcja nie zwraca żadnych wartości.
 */

void _tm1637Stop(_Bool ifRed) {

    _tm1637ClkLow(ifRed);
    _tm1637DelayUsec(2);
    _tm1637DioLow(ifRed);
    _tm1637DelayUsec(2);
    _tm1637ClkHigh(ifRed);
    _tm1637DelayUsec(2);
    _tm1637DioHigh(ifRed);
}

/***************************************************************************************/

/*!
 * \brief Funkcja _tm1637ReadResult_ służy do wczytania wysłanych wartości przez mcu wyświetlacza
 *
 * \param ifRed - czy funkcja ma dotyczyć wyświetlacza czerwonego, czy może niebieskiego.
 *
 * Funkcja nie zwraca żadnych wartości.
 */

void _tm1637ReadResult(_Bool ifRed) {

    _tm1637ClkLow(ifRed);
    _tm1637DelayUsec(5);

    _tm1637ClkHigh(ifRed);
    _tm1637DelayUsec(2);
    _tm1637ClkLow(ifRed);
}

/***************************************************************************************/

/*!
 * \brief Funkcja _tm1637WriteByte_ służy do przesłania bajtu danych do mcu wyświetlacza
 *
 * \param b - bajt do wysłania,
 * \param ifRed - czy funkcja ma dotyczyć wyświetlacza czerwonego, czy może niebieskiego.
 *
 * Funkcja nie zwraca żadnych wartości.
 */

void _tm1637WriteByte(unsigned char b, _Bool ifRed) {

    for (int i = 0; i < 8; ++i) {
        _tm1637ClkLow(ifRed);
        if (b & 0x01) {
            _tm1637DioHigh(ifRed);
        }
        else {
            _tm1637DioLow(ifRed);
        }
        _tm1637DelayUsec(3);
        b >>= 1;
        _tm1637ClkHigh(ifRed);
        _tm1637DelayUsec(3);
    }

    _tm1637ReadResult(ifRed);
}

/***************************************************************************************/

void _tm1637DelayUsec(unsigned int i) {

    for (; i>0; i--) {
        for (int j = 0; j < 10; ++j) {
            __asm__ __volatile__("nop\n\t":::"memory");
        }
    }
}

/***************************************************************************************/

void _tm1637ClkHigh(_Bool ifRed) {

	if(ifRed) HAL_GPIO_WritePin(CLKRED_GPIO_Port, CLKRED_Pin, 1);
	else HAL_GPIO_WritePin(CLKBLUE_GPIO_Port, CLKBLUE_Pin, 1);
}

void _tm1637ClkLow(_Bool ifRed) {

	if(ifRed) HAL_GPIO_WritePin(CLKRED_GPIO_Port, CLKRED_Pin, 0);
	else HAL_GPIO_WritePin(CLKBLUE_GPIO_Port, CLKBLUE_Pin, 0);
}

void _tm1637DioHigh(_Bool ifRed) {

	if(ifRed) HAL_GPIO_WritePin(DIORED_GPIO_Port, DIORED_Pin, 1);
	else HAL_GPIO_WritePin(DIOBLUE_GPIO_Port, DIOBLUE_Pin, 1);
}

void _tm1637DioLow(_Bool ifRed) {

	if(ifRed) HAL_GPIO_WritePin(DIORED_GPIO_Port, DIORED_Pin, 0);
	else HAL_GPIO_WritePin(DIOBLUE_GPIO_Port, DIOBLUE_Pin, 0);
}
