/*
SETLIST WATCH

Date  : 03/2025
Author: Giuliano Nascimento

Usar a seguinte configuração no Arduino IDE:

Board: TTGO T-Watch
Flash Frequency: 80m
CPU Frequency: 240MHz
Flash Size: 16MB


Para saber mais:
https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library/tree/t-watch-s3
Biblioteca do relógio:
https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library
Biblioteca gráfica:
https://learn.adafruit.com/adafruit-gfx-graphics-library/

Links interessantes:

Dicas avançadas:
https://nick.zoic.org/art/lilygo-ttgo-t-watch-2020/

*/

#define LILYGO_WATCH_2020_V3         // Use T-Watch2020
#define LILYGO_WATCH_HAS_MOTOR       //Use Motor module 

#define APP_TIME_ZONE               0
#define TAM_BUFFER_DADOS            2048
#define INTERVALO_DOWNLOAD          120000
#define INTERVALO_TESTE_INTERNET    25000
#define INTERVALO_INATIVIDADE       20000
#define INTERVALO_TENTATIVA_CONEXAO 120000
#define INTERVALO_DORMIR            1800000
#define INTERVALO_AULA_ACABANDO     300000
#define TEMPO_ESPERA_CONEXAO        50
#define TFT_GREY                    0x5AEB
#define TFT_ORANGE                  0xFD20      /* 255, 165,   0 */
#define M_SIZE                      0.667
#define TOLERANCIA                  15

#include "SPIFFS.h"
#include <LilyGoWatch.h>
#include <soc/rtc.h>
#include <time.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "ping.h"

TTGOClass *ttgo;

uint32_t targetTime = 0;       // for next 1 second display update
// uint32_t clockUpTime = 0;      // track the time the clock is displayed
uint8_t hh, mm, ss, mmonth, dday; // H, M, S variables
uint16_t yyear; // Year is 16 bit int
AXP20X_Class *power;


// Painel
float ltx = 0;    // Saved x coord of bottom of needle
uint16_t osx = M_SIZE * 120, osy = M_SIZE * 120; // Saved x & y coords
uint32_t updateTime = 0;       // time for next update
int old_analog =  -999; // Value last displayed
int value[6] = {0, 0, 0, 0, 0, 0};
int old_value[6] = { -1, -1, -1, -1, -1, -1};
int d = 0;
int16_t x, y;

const char* VERSAO            = "1.0.0";
const char* www_username      = "admin";
const char* www_password      = "e5d4c3b2a1";

char host[30]                 = "temora.webcfc.com.br";
char ssid[50]                 = "";
char password[50]             = "";
char DADOS[TAM_BUFFER_DADOS]  = "";
char UUID[20]                 = ""; // Identificação única deste ESP32 (mac address)
char MADD[20]                 = ""; // MAC Address do ESP32
char IP[16]                   = ""; // Endereço de IP depois de conectado ao WiFi
char IP_REMOTO[16]            = ""; // Endereço de IP do dispositivo remoto
char IP_SERVIDOR[16]          = ""; // Endereço de IP do servidor

bool irq                      = false;
bool bWIFI_pronto             = false;
bool bWIFI_ativo              = false;
bool bWIFI_internetDisponivel = false;
bool bInternetEmUso           = false;
bool bEncontrouRede           = false;
bool bUltimoPingComSucesso    = false;
bool bPingando                = false;
bool bHoraDefinida            = false;
bool bPrimeiroAcessoAInternet = true;
bool bTelaLigada              = false;
bool bSongMode                = false;
bool sDormindo                = false;
bool ipVeiculoAcessivel       = false;
bool bAvisouQueAulaEstaAcabando = false;
bool bAcertouHoraQuandoLigou  = false;

unsigned long m                     = 0;
unsigned long timerURL              = 0;
unsigned long timerInatividade      = 0;
unsigned long timerUltimoTesteInternet = 0;
unsigned long timerEspera           = 0;
unsigned long timerSemInternet      = 0;
unsigned long timerPing             = 0;
unsigned long timerOn               = 0;
unsigned long timerUp               = 0;
unsigned long timerOff              = 0;
unsigned long timerPhoto            = 0;
unsigned long timerGenerico         = 0;

long timerUltimaBuscaWiFi           = -INTERVALO_TENTATIVA_CONEXAO;

int cntTimeout = 0;
int8_t battery    = 0;
int8_t canal      = 1;
int8_t maxItensEncontrados = 0;
int8_t maxPlacas = 0;
int8_t maxAulas = 0;
int8_t maxTipos = 0;
int8_t maxFaltas = 0;
int8_t placaSelecionada = 0;

int pCSV1      = 0;
int pCSV2      = -1;

String me;
String url;
String line;
String sResposta;
String sRedesEncontradas;
String redeEncontrada;
String sRotacao = "0";

String sSensores[40];
String sLCD[2];
String cfg_WIFI_SSID                = "";
String cfg_WIFI_PASSWORD            = "";
String cfg_TEM_SSID                 = "";
String cfg_TEM_PASSWORD             = "";
String sCSV                         = "";
String sNumeros                     = "";
String tecla                        = "";



String selectedSetlist              = "Setlist 1";


IPAddress ipzero(0, 0, 0, 0);

char agoraData[11]                  = "0000-00-00";	          // Data atual (AAAA-MM-DD)
char agoraHora[6]                   = "00:00";				  // hora atual (HH:MM)


// int getTnum();
// void prtTime(byte digit);
// void menuDisplay(int mSel);

byte xcolon = 0; // location of the colon



// Display Jupiters 4 moons
// It uses the current date time and corrects to get UTC time
// Make sure you set the correct Time Zone below


void setup()
{
    //initSetup();
    Serial.begin(115200);
    btStop();
    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    ttgo->tft->setTextFont(1);
    ttgo->tft->fillScreen(TFT_BLACK);
    ttgo->tft->setTextColor(TFT_YELLOW, TFT_BLACK); // Note: the new fonts do not draw the background colour
    //LVGL is not used, this line is not needed
    // ttgo->lvgl_begin();

    //Check if the RTC clock matches, if not, use compile time
    ttgo->rtc->check();

    //Synchronize time to system time
    ttgo->rtc->syncToSystem();
    ttgo->motor_begin();
    power = ttgo->power;

    pinMode(AXP202_INT, INPUT_PULLUP);
    attachInterrupt(AXP202_INT, [] {
        irq = true;
    }, FALLING);

    //!Clear IRQ unprocessed  first
    ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ |
                           AXP202_VBUS_REMOVED_IRQ |
                           AXP202_VBUS_CONNECT_IRQ |
                           AXP202_CHARGING_IRQ, true);
    ttgo->power->clearIRQ();

    inicializaSPIFFS();
    obtemMacAddress();
    ligaDesligaTela(true);
    if (sRotacao=="0")
    {
        sRotacao = "1";
        ttgo->tft->setRotation(0);
    } else {
        sRotacao = "0";
        ttgo->tft->setRotation(2);
    }           
    telaInicio(true);

}



void loop()
{
    m = millis();

    if (irq) {
        irq = false;
        ttgo->power->readIRQ();
        if (ttgo->power->isPEKShortPressIRQ()) {
            if (sRotacao=="0")
            {
                sRotacao = "1";
                ttgo->tft->setRotation(0);
            } else {
                sRotacao = "0";
                ttgo->tft->setRotation(2);
            }
            salvaRotacaoNoESP();
            ttgo->tft->fillRect(0, 0, 240, 240, TFT_BLACK);
            telaInicio(true); 
        }
        ttgo->power->clearIRQ();
    }

    if (targetTime < millis()) {
        // bugTrace(true, "Timer: "+String(m)+" - Prox foto: "+timerPhoto);
        targetTime = millis() + 5000;
        if (bTelaLigada)
        {
            if ((m > timerUltimaBuscaWiFi + INTERVALO_TENTATIVA_CONEXAO) && !conectadoAInternet())
            {
                // conectaRedeWifi();
            }
            telaInicio(ss == 0); 
        }
        
    }


    if (ttgo->getTouch(x, y)) {
        while (ttgo->getTouch(x, y)) {} // wait for user to release
        if (m > timerInatividade + INTERVALO_INATIVIDADE )
        {
            ligaDesligaTela(true);
            telaInicio(true);
        } else {
            if (y < 70)
            {
                ttgo->motor->onec();
                if (x < 120)
                {
                    LCDcls();
                    LCDtextoCentralizado(F("Desconectando..."),0);
                    WiFi.disconnect(false,true);
                    conectaRedeWifi();
                } else {
                    LCDcls();
                    LCDtextoCentralizado(F("Atualizando..."),0);
                    if (!conectadoAInternet())
                    {
                        conectaRedeWifi();
                    } else {
                        API_download();
                    }
                }
            } else {
                switch (menuSelecionar()) { 
                case 0: 
                    break;
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                    telaInformacoes();
                    break;
                }
                telaInicio(true);
            }
        }
        timerInatividade = millis();
    }

    if (bTelaLigada && m > timerInatividade + INTERVALO_INATIVIDADE)
    {
        ligaDesligaTela(false);
    }

    // Colocando nenê pra naná
    // if (!sDormindo && m > timerInatividade + INTERVALO_DORMIR)
    // {
    //     bugTrace(true,"[INF] Dormiu!");
    //     WiFi.disconnect(true);
    //     WiFi.mode(WIFI_OFF);        
	// 	bWIFI_pronto = false;
	// 	bWIFI_internetDisponivel = false;
    //     power->setPowerOutPut(AXP202_LDO3, false);
    //     power->setPowerOutPut(AXP202_LDO4, false);
    //     power->setPowerOutPut(AXP202_LDO2, false);
    //     // The following power channels are not used
    //     power->setPowerOutPut(AXP202_EXTEN, false);
    //     power->setPowerOutPut(AXP202_DCDC2, false);
    //     sDormindo = true;
    // }
}