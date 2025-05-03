
void bugTrace(bool nl, String txt)
{
	if (nl)
	{
		Serial.println(txt);
	} else {
		Serial.print(txt);
	}
}

void log(String data, String txt)
{

}

// Um arquivo CSV é assim:
// campo0;campo1;campo2;campo3\n     (linha 0)
// campo0;campo1;campo2;campo3\n     (linha 1)
// campo0;campo1;campo2;campo3\n     (linha 2)
void CSV_abrir(String csv)
{
	sCSV = csv;
	pCSV1 = 0;
	pCSV2 = csv.indexOf("\n");
	if (pCSV2==-1)
	{
		pCSV2 = csv.length()-1;
	}
}

// Lê e retorna a próxima linha no arquivo CSV aberto com o comando CSV_abrir
// Uma linha CSV é assim:
// campo0;campo1;campo2;campo3\n
String CSV_obtemLinha()
{
	String par = "";
	String val = "";
	String sai = "";

	if (pCSV2<=pCSV1)
	{
		sai = "";
	} else {
		sai = sCSV.substring(pCSV1,pCSV2);
		pCSV1 = pCSV2+1;
		pCSV2 = sCSV.indexOf("\n", pCSV1);
		if (pCSV2==-1)
		{
			pCSV2 = sCSV.length()-1;
		}
	}
	return(sai);
}

// Informando uma linha de texto em "line", retorna a coluna "index"
String CSV_obtemCampoEmLinha(String line,int index)
{
	char separator = ';';
	int found = 0;
	int strIndex[] = {0, -1};
	int maxIndex = line.length()-1;

	for(int i=0; i<=maxIndex && found<=index; i++){
		if(line.charAt(i)==separator || i==maxIndex){
			found++;
			strIndex[0] = strIndex[1]+1;
			strIndex[1] = (i == maxIndex) ? i+1 : i;
		}
	}

	return found>index ? line.substring(strIndex[0], strIndex[1]) : "";
}

// Le um arquivo CSV e busca uma linha que comece com "nome"
// quando encontra, busca coluna com o índice "indice"
String CSV_obtemValorDoCampo(String csv, String nome, byte indice)
{
	int p = 0;
	int p1 = 0;
	int p2 = csv.indexOf("\n");
	String line;
	String par = "";
	String val = "";
	String sai = "";

	if (p2>-1)
	{
		while (p2>-1 && p2<csv.length())
		{
			line = csv.substring(p1,p2);
			//p = line.indexOf(";");
			// p = line.indexOf(";");
			// if (p>-1)
			// {
			//    par = line.substring(0,p);
			//    //val = line.substring(p+1);
			//    if (par == nome)
			//    {
			//       sai = CSV_obtemCampoEmLinha(line, indice);
			//    }
			// }
			if (line.indexOf(nome)>=0)
			{
				sai = CSV_obtemCampoEmLinha(line, indice);
			}
			p1 = p2+1;
			p2 = csv.indexOf("\n", p1);
		}
	}
	return(sai);
}

// Formata data e hora em string para uso rápido em outras rotinas
void atualizaDataHora()
{
    RTC_Date tnow = ttgo->rtc->getDateTime();
	sprintf(agoraData, "%04d-%02d-%02d", tnow.year, tnow.month, tnow.day);
	sprintf(agoraHora, "%02d:%02d", tnow.hour, tnow.minute);
}
// Calcula horário para a próxima foto com base no parametro TIMER_PHOTO
// (menos um minuto e adicionando randomicamente de 0 a 2 minutos)
void calculaNovaFotoRandomica()
{
	//
	int8_t fotosNaAula = 8;
	fotosNaAula = 5;
	timerPhoto = millis()+(fotosNaAula)*60000+random(0,120)*1000;
	bugTrace(false, "[INF] Próxima foto - Tempo atual: "+String( (millis()/60000) )+" ");
	bugTrace(true, "Tempo foto: "+String(timerPhoto));
}

unsigned long converteHoraInteiro(unsigned long hora, unsigned long minutos, unsigned long segundos)
{
	unsigned long inteiro_hora=hora*3600000;
	unsigned long inteiro_minuto=minutos*60000;
	unsigned long inteiro_segundo=segundos*1000;
	unsigned long total = (inteiro_hora+inteiro_minuto+inteiro_segundo);
	return (total);
}

void obtemRedeTemora()
{
	// cfg_TEM_SSID = CSV_obtemCampoEmLinha(sPlacas[placaSelecionada],6);
	// cfg_TEM_PASSWORD = CSV_obtemCampoEmLinha(sPlacas[placaSelecionada],7);
	// bugTrace(true,"SSID : "+cfg_TEM_SSID);
	// bugTrace(true,"Senha: "+cfg_TEM_PASSWORD);
}

void API_download() 
{
    bInternetEmUso = true;
	bugTrace(true,F("."));
	bugTrace(true, F("[ DOWNLOAD ]"));
	conectadoAInternet();
	if (bWIFI_internetDisponivel)
	{
   		bugTrace(true, F("[INF] Internet disponivel"));
        LCDcls();
        LCDtextoCentralizado("Baixando dados",0);
        LCDtextoCentralizado("espere um pouco... ",1);

        WiFiClient client;
        byte inicio = 0;
        bool configuracaoAlterada = false;
        line = "";
        const int httpPort = 80;
        bool ehResposta = false;
        bugTrace(true,"[INF] Conectando: "+String(host));

        if (!client.connect(host, httpPort)) {
            LCDcls();

            // Estava conectado e desconectou agora...
            if (bWIFI_internetDisponivel)
            {
                bugTrace(true, F("[ERR] Internet caiu!"));
            }
            bWIFI_internetDisponivel = false;
            bugTrace(false, F("[ERR] URL: "));
            return;
        }

        bWIFI_internetDisponivel = true;

        montaUrl("esp_uuids");


        if (sResposta!="")
        {
            ehResposta = true;
            url+= "&resposta=" + urlencode(sResposta);
            sResposta = "";
        }

        bugTrace(true, url);

        // This will send the request to the server
        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
        "Host: " + host + "\r\n" +
        "Connection: close\r\n\r\n");
        unsigned long timeout = millis();
        while (client.available() == 0) {
            if (millis() - timeout > 5000) {
                bugTrace(true, F("[ERR] Timeout !"));
                cntTimeout++;
                client.stop();
                // Se já deu timeout várias vezes seguidas, a rede pode estar ruim
                // ou desconectada, portanto é melhor reconectar
                if (cntTimeout>3)
                {
                    conectaRedeWifi();
                }
                return;
            }
        }

        // Read all the lines of the reply from server and print them to Serial
        maxPlacas = 0;
        maxAulas = 0;
        maxTipos = 0;
        maxFaltas = 0;
		bool novasPlacas = false;
		bool novasAulas = false;
        while(client.available()) {
            String line = client.readStringUntil('\n');
            // if (line.indexOf("placa;")>-1)
            // {
            //     bugTrace(true,"[PLACA] "+line);
            //     sPlacas[maxPlacas] = line;
			// 	if (sPlacas[maxPlacas]!=sPlacasSPIFFS[maxPlacas])
			// 	{
			// 		novasPlacas = true;
			// 	}
            //     maxPlacas++;
            // }
            // if (line.indexOf("aula;")>-1)
            // {
            //     bugTrace(true,"[AULA] "+line);
            //     sAulas[maxAulas] = line;
			// 	if (sAulas[maxAulas]!=sAulasSPIFFS[maxAulas])
			// 	{
			// 		novasAulas = true;
			// 	}
            //     maxAulas++;
            // }
            // if (line.indexOf("tipo;")>-1)
            // {
            //     bugTrace(true,"[TIPO] "+line);
            //     sTipos[maxTipos] = line;
            //     maxTipos++;
            // }
            // if (line.indexOf("falta;")>-1)
            // {
            //     bugTrace(true,"[FALTA] "+line);
            //     sFaltas[maxFaltas] = line;
            //     maxFaltas++;
            // }
        }
        client.stop();
        cntTimeout = 0;
		obtemRedeTemora();
		LCDcls();
        // LCDtextoCentralizado("Veiculos : "+String(maxPlacas),0);
        // LCDtextoCentralizado("Aulas    : "+String(maxAulas),1);
        
		// if (novasPlacas || novasAulas)
		// {
		// 	if (novasPlacas)
		// 	{
		// 		LCDtextoCentralizado("Dados atualizados!",2);
		// 	} else {
		// 		LCDtextoCentralizado("Aulas atualizadas!",2);
		// 	}
		// } else {
		// 	LCDtextoCentralizado("Sem novidades!",2);
		// }
		// if (novasPlacas)
		// {
		// 	bugTrace(true,"Novas placas encontradas! Salvando no SPIFFS...");
		// 	salvaPlacasNoESP();
		// } else {
		// 	bugTrace(true,"Nenhuma placa nova....");
		// }
		// if (novasAulas)
		// {
		// 	bugTrace(true,"Novas aulas encontradas! Salvando no SPIFFS...");
		// 	salvaAulasNoESP();
		// } else {
		// 	bugTrace(true,"Nenhuma aula nova....");
		// }
        delay(2000);
		LCDcls();
	}
    bInternetEmUso = false;    
}

void esperaToqueNaTela(int tempoDeEspera)
{
    int16_t x, y;
	int8_t cnt = 0;

	bool esperar = true;
	timerEspera = millis() + tempoDeEspera;
	timerGenerico = millis()+1000;
    ttgo->tft->setTextSize(1);
	ttgo->tft->setTextColor(TFT_BLACK);
	cnt = tempoDeEspera / 1000;
	ttgo->tft->fillRect(105, 210, 30, 30, TFT_YELLOW);
	ttgo->tft->drawCentreString(String(cnt),120,214,4);
    while (esperar) {
        if (millis() > timerGenerico)
        {
            timerGenerico = millis()+1000;
            cnt--;
            ttgo->tft->fillRect(105, 210, 30, 30, TFT_YELLOW);
			ttgo->tft->drawCentreString(String(cnt),120,214,4);
		}
		if (ttgo->getTouch(x, y) || millis() > timerEspera)
		{
			esperar = false;
		}
	}
    while (ttgo->getTouch(x, y)) {} 
    ttgo->tft->fillScreen(TFT_BLACK);
    timerInatividade = millis();
}

void obtemMacAddress() {
	uint8_t baseMac[6];
	esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
	char baseMacChr[18] = {0};
	sprintf(UUID, "ESP32_%02X%02X%02X%02X%02X%02X\0", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
	sprintf(MADD, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
}

void montaUrl(String cmd)
{
	url = "http://"+String(host)+"/admin/api.php";
	//surl = "/pub/api.php";

	url += "?uuid=";
	url += UUID;
	url += "&madd=";
	url += MADD;
	if (cmd != "")
	{
		url += "&cmd="+cmd;
	}
	url += "&v=";
	url += String(VERSAO);
	url += "&type=11"; // temora de moto
	url += "&ip_f=";
	url += IP;	//bugTrace(true,url);
}

String urlencode(String str)
{
	String encodedString="";
	char c;
	char code0;
	char code1;
	char code2;
	for (int i =0; i < str.length(); i++){
		c=str.charAt(i);
		if (c == ' '){
			encodedString+= '+';
		} else if (isalnum(c)){
			encodedString+=c;
		} else{
			code1=(c & 0xf)+'0';
			if ((c & 0xf) >9){
				code1=(c & 0xf) - 10 + 'A';
			}
			c=(c>>4)&0xf;
			code0=c+'0';
			if (c > 9){
				code0=c - 10 + 'A';
			}
			code2='\0';
			encodedString+='%';
			encodedString+=code0;
			encodedString+=code1;
			//encodedString+=code2;
		}
		// yield();
	}
	return encodedString;

}

String IpAddress2String(const IPAddress& ipAddress)
{
	return String(ipAddress[0]) + String(".") +\
	String(ipAddress[1]) + String(".") +\
	String(ipAddress[2]) + String(".") +\
	String(ipAddress[3])  ;
}


void obtemBateria()
{
    // Turn on the battery adc to read the values
    ttgo->power->adc1Enable(AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_BATT_VOL_ADC1, true);
    // get the values
    // float vbus_v = ttgo->power->getVbusVoltage();
    // float vbus_c = ttgo->power->getVbusCurrent();
    // float batt_v = ttgo->power->getBattVoltage();
    battery = ttgo->power->getBattPercentage();
}


void inicializaSPIFFS()
{
	String FILE_CONTENT;
	// Antes de qualquer coisa, busca a configuração salva na
	// memória não volátil interna do ESP32
	if (!SPIFFS.begin(true))
	{
		// Se não foi possível montar, é porque foi formatado agora
		// e precisa ser reiniciado
		bugTrace(true, F("SPIFFS não está pronto... reiniciando!"));
		ESP.restart();
	}
	else
	{
        listaArquivosMemoria(SPIFFS, "/", 0);
		//SPIFFS.remove("/config");
		if (SPIFFS.exists("/cfg_WIFI_SSID"))
		{
            bugTrace(true,"[SPIFF] Restaurando rede WiFi salva");
			File rFile = SPIFFS.open("/cfg_WIFI_SSID", "r");
			FILE_CONTENT = rFile.readString();
			rFile.close();
			cfg_WIFI_SSID = FILE_CONTENT.substring(0, FILE_CONTENT.indexOf("\n"));
			cfg_WIFI_PASSWORD = FILE_CONTENT.substring(FILE_CONTENT.indexOf("\n") + 1);
			bugTrace(false, F("[X] WIFI Outro: "));
			bugTrace(false, cfg_WIFI_SSID);
			bugTrace(false, F(" - "));
			bugTrace(true, cfg_WIFI_PASSWORD);
		}
		if (SPIFFS.exists("/cfg_TEM_SSID"))
		{
            bugTrace(true,"[SPIFF] Restaurando rede Temora salva");
			File rFile = SPIFFS.open("/cfg_TEM_SSID", "r");
			FILE_CONTENT = rFile.readString();
			rFile.close();
			cfg_TEM_SSID = FILE_CONTENT.substring(0, FILE_CONTENT.indexOf("\n"));
			cfg_TEM_PASSWORD = FILE_CONTENT.substring(FILE_CONTENT.indexOf("\n") + 1);
			bugTrace(false, F("[X] Rede Temora: "));
			bugTrace(false, cfg_TEM_SSID);
			bugTrace(false, F(" - "));
			bugTrace(true, cfg_TEM_PASSWORD);
		}

	}
}

void listaArquivosMemoria(fs::FS &fs, const char * dirname, uint8_t levels){
	//Serial.printf("SPIFFS: %s\r\n", dirname);
	bugTrace(true, F("SPIFFS:"));

	File root = fs.open(dirname);
	if(!root){
		return;
	}
	if(!root.isDirectory()){
		return;
	}

	File file = root.openNextFile();
	while(file){
		if(file.isDirectory()){
			bugTrace(false, F("DIR : "));
			bugTrace(true, file.name());
			if(levels){
				listaArquivosMemoria(fs, file.name(), levels -1);
			}
		} else {
			bugTrace(false, F("... "));
			bugTrace(false, file.name());
			bugTrace(false, F("\tTAM: "));
			bugTrace(true, String(file.size()));
		}
		file = root.openNextFile();
	}
	file.close();
	root.close();
}

void salvaRotacaoNoESP()
{
	File inputsWrite = SPIFFS.open("/cfg_Rotacao", "w+");
	if(!inputsWrite) {bugTrace(true, "ERROR ! Couldn't open file to write !");}
	inputsWrite.print(sRotacao);
	inputsWrite.print("\n");
	inputsWrite.close();
	delay(500);
}



void salvaWiFiNoESP()
{
	File inputsWrite = SPIFFS.open("/cfg_WIFI_SSID", "w+");
	if(!inputsWrite) {bugTrace(true, "ERROR ! Couldn't open file to write !");}
	inputsWrite.print(cfg_WIFI_SSID);
	inputsWrite.print("\n");
	inputsWrite.print(cfg_WIFI_PASSWORD);
	inputsWrite.close();
	delay(500);
}

void salvaTemoraNoESP()
{
	File inputsWrite = SPIFFS.open("/cfg_TEM_SSID", "w+");
	if(!inputsWrite) {bugTrace(true, "ERROR ! Couldn't open file to write !");}
	inputsWrite.print(cfg_TEM_SSID);
	inputsWrite.print("\n");
	inputsWrite.print(cfg_TEM_PASSWORD);
	inputsWrite.close();
	delay(500);
}


float truncate(float val, byte dec) 
{
    float x = val * pow(10, dec);
    float y = round(x);
    float z = x - y;
    if ((int)z == 5)
    {
        y++;
    } else {}
    x = y / pow(10, dec);
    return x;
}


void apagarAcelerometro()
{
    ttgo->bma->begin();
    ttgo->bma->enableAccel();
    ttgo->tft->fillScreen(TFT_BLACK);
    int16_t x, y;
    int16_t xpos, ypos;

    Accel acc;

    while (!ttgo->getTouch(x, y)) { // Wait for touch to exit

        ttgo->bma->getAccel(acc);
        xpos = acc.x;
        ypos = acc.y;
        ttgo->tft->fillCircle(xpos / 10 + 119, ypos / 10 + 119, 10, TFT_RED); // draw dot
        delay(100);
        ttgo->tft->fillCircle(xpos / 10 + 119, ypos / 10 + 119, 10, TFT_BLACK); // erase previous dot
    }

    while (ttgo->getTouch(x, y)) {}  // Wait for release to return to the clock

    ttgo->tft->fillScreen(TFT_BLACK); // Clear screen
}

void apagarTouch()
{
    uint32_t endTime = millis() + 10000; // Timeout at 10 seconds
    int16_t x, y;
    ttgo->tft->fillScreen(TFT_BLACK);

    while (endTime > millis()) {
        ttgo->getTouch(x, y);
        ttgo->tft->fillRect(98, 100, 70, 85, TFT_BLACK);
        ttgo->tft->setCursor(80, 100);
        ttgo->tft->print("X:");
        ttgo->tft->println(x);
        ttgo->tft->setCursor(80, 130);
        ttgo->tft->print("Y:");
        ttgo->tft->println(y);
        delay(25);
    }

    while (ttgo->getTouch(x, y)) {}  // Wait for release to exit
    ttgo->tft->fillScreen(TFT_BLACK);
}

void apagarMudarHora()
{
    // Get the current info
    RTC_Date tnow = ttgo->rtc->getDateTime();

    hh = tnow.hour;
    mm = tnow.minute;
    ss = tnow.second;
    dday = tnow.day;
    mmonth = tnow.month;
    yyear = tnow.year;

//Set up the interface buttons

    ttgo->tft->fillScreen(TFT_BLACK);
    ttgo->tft->fillRect(0, 35, 80, 50, TFT_BLUE);
    ttgo->tft->fillRect(161, 35, 78, 50, TFT_BLUE);
    ttgo->tft->fillRect(81, 85, 80, 50, TFT_BLUE);
    ttgo->tft->fillRect(0, 135, 80, 50, TFT_BLUE);
    ttgo->tft->fillRect(161, 135, 78, 50, TFT_BLUE);
    ttgo->tft->fillRect(0, 185, 80, 50, TFT_BLUE);
    ttgo->tft->setTextColor(TFT_GREEN);
    ttgo->tft->drawNumber(1, 30, 40, 2);
    ttgo->tft->drawNumber(2, 110, 40, 2);
    ttgo->tft->drawNumber(3, 190, 40, 2);
    ttgo->tft->drawNumber(4, 30, 90, 2);
    ttgo->tft->drawNumber(5, 110, 90, 2);
    ttgo->tft->drawNumber(6, 190, 90, 2);
    ttgo->tft->drawNumber(7, 30, 140, 2);
    ttgo->tft->drawNumber(8, 110, 140, 2);
    ttgo->tft->drawNumber(9, 190, 140, 2);
    ttgo->tft->drawNumber(0, 30, 190, 2);
    ttgo->tft->fillRoundRect(120, 200, 119, 39, 6, TFT_WHITE);
    ttgo->tft->setTextSize(2);
    ttgo->tft->setCursor(0, 0);

    ttgo->tft->setCursor(155, 210);
    ttgo->tft->setTextColor(TFT_BLACK);
    ttgo->tft->print("DONE");
    ttgo->tft->setTextColor(TFT_WHITE);
    int wl = 0; // Track the current number selected
    byte curnum = 1;  // Track which digit we are on

    prtTime(curnum); // Display the time for the current digit

    while (wl != 13) {
        wl = getTnum();
        if (wl != -1 && wl != 13)

            switch (curnum) {
            case 1:
                hh = wl * 10 + hh % 10;
                break;
            case 2:
                hh = int(hh / 10) * 10 + wl;
                break;
            case 3:
                mm = wl * 10 + mm % 10;
                break;
            case 4:
                mm = int(mm / 10) * 10 + wl;
                break;
            }
        while (getTnum() != -1) {}
        curnum += 1;
        if (curnum > 4) curnum = 1;
        prtTime(curnum);
    }
    while (getTnum() != -1)
    {}
    ttgo->rtc->setDateTime(yyear, mmonth, dday, hh, mm, 0);
    ttgo->tft->fillScreen(TFT_BLACK);
}

// prtTime will display the current selected time and highlight
// the current digit to be updated in yellow

void prtTime(byte digit)
{
    ttgo->tft->fillRect(0, 0, 100, 34, TFT_BLACK);
    if (digit == 1)   ttgo->tft->setTextColor(TFT_YELLOW);
    else   ttgo->tft->setTextColor(TFT_WHITE);
    ttgo->tft->drawNumber(int(hh / 10), 5, 5, 2);
    if (digit == 2)   ttgo->tft->setTextColor(TFT_YELLOW);
    else   ttgo->tft->setTextColor(TFT_WHITE);
    ttgo->tft->drawNumber(hh % 10, 25, 5, 2);
    ttgo->tft->setTextColor(TFT_WHITE);
    ttgo->tft->drawString(":",  45, 5, 2);
    if (digit == 3)   ttgo->tft->setTextColor(TFT_YELLOW);
    else   ttgo->tft->setTextColor(TFT_WHITE);
    ttgo->tft->drawNumber(int(mm / 10), 65, 5, 2);
    if (digit == 4)   ttgo->tft->setTextColor(TFT_YELLOW);
    else   ttgo->tft->setTextColor(TFT_WHITE);
    ttgo->tft->drawNumber(mm % 10, 85, 5, 2);
}

// getTnum takes care of translating where we pressed into
// a number that was pressed. Returns -1 for no press
// and 13 for DONE

int getTnum()
{
    int16_t x, y;
    if (!ttgo->getTouch(x, y)) return - 1;
    if (y < 85) {
        if (x < 80) return 1;
        else if (x > 160) return 3;
        else return 2;
    } else if (y < 135) {
        if (x < 80) return 4;
        else if (x > 160) return 6;
        else return 5;
    } else if (y < 185) {
        if (x < 80) return 7;
        else if (x > 160) return 9;
        else return 8;
    } else if (x < 80) return 0;
    else return 13;
}

void ligaDesligaTela(bool ligar)
{
    if (ligar != bTelaLigada)
    {
        if (ligar)
        {
            bugTrace(true,"[INF] Ligou display");
            bTelaLigada = true;
            ttgo->displayWakeup();
            ttgo->openBL();
            if (sDormindo)
            {
                power->setPowerOutPut(AXP202_LDO3, true);
                power->setPowerOutPut(AXP202_LDO4, true);
                power->setPowerOutPut(AXP202_LDO2, true);
                // The following power channels are not used
                power->setPowerOutPut(AXP202_EXTEN, true);
                power->setPowerOutPut(AXP202_DCDC2, true);
            }
            sDormindo = false;
        } else {
            bugTrace(true,"[INF] Desligou display");
            bTelaLigada = false;
            ttgo->displaySleep();
            ttgo->closeBL();
        }
    }
}