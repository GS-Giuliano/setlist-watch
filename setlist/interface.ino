const int maxApp = 5; 
const int maxComboItens = 70;

String appName[maxApp] = {"Relogio", "Repertorio", "Musicas", "Informacoes", "Configurar"}; 
String comboIds[maxComboItens];
String comboTexts[maxComboItens];
String sCombo[maxComboItens];

uint8_t menuSelecionar()
{
    int mSelect = 0; 

    boolean exitMenu = false; 

    menuDisplay(0); 
    
    timerEspera = millis() + 5000;
    while (!exitMenu) {
        if (millis() > timerEspera)
        {
            mSelect = 0;
            exitMenu = true;
        }
        if (ttgo->getTouch(x, y)) { 
            
            while (ttgo->getTouch(x, y)) {} 
            timerEspera = millis() + 5000;
            if (y >= 160) { 
                mSelect += 1;
                if (mSelect == maxApp) mSelect = 0;
                menuDisplay(mSelect);
            }

            if (y <= 80) { 
                mSelect -= 1;
                if (mSelect < 0) mSelect = maxApp - 1;
                menuDisplay(mSelect);
            }
            if (y > 80 && y < 160) { 
                bugTrace(true,"[INF] Menu selecionado: "+String(mSelect));
                ttgo->motor->onec(100);
                exitMenu = true;
            }
        }
    }
    //Return with mSelect containing the desired mode
    ttgo->tft->fillScreen(TFT_BLACK);
    return mSelect;
}

void menuDisplay(int mSel)
{

    int curSel = 0;
    // Display mode header
    ttgo->tft->fillScreen(TFT_BLACK);
    ttgo->tft->fillRect(0, 80, 239, 80, TFT_YELLOW);

    // Display apps
    if (mSel == 0) curSel = maxApp - 1;
    else curSel = mSel - 1;
    

    ttgo->tft->setTextSize(1);
    ttgo->tft->setTextColor(TFT_YELLOW);
    ttgo->tft->setCursor(50, 30);
    // ttgo->tft->println(appName[curSel]);
    ttgo->tft->drawString(appName[curSel], 30, 30, 4);

    ttgo->tft->setTextSize(1);
    ttgo->tft->setTextColor(TFT_BLACK);
    ttgo->tft->setCursor(40, 110);
    // ttgo->tft->println(appName[mSel]);
    ttgo->tft->drawString(appName[mSel], 30, 110, 4);

    if (mSel == maxApp - 1) curSel = 0;
    else curSel = mSel + 1;

    ttgo->tft->setTextSize(1);
    ttgo->tft->setTextColor(TFT_YELLOW);
    ttgo->tft->setCursor(50, 190);
    // ttgo->tft->print(appName[curSel]);
    ttgo->tft->drawString(appName[curSel], 30, 190, 4);
}

void LCDcls() {
    ttgo->tft->setTextFont(1);
    ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK); 
    ttgo->tft->fillRect(0,66,240,144, TFT_BLACK);    
}

void LCDtextoCentralizado(String txt, int8_t linha)
{
    ttgo->tft->setTextWrap(false);
    ttgo->tft->fillRect(10, 80+linha*30, 220, 40, TFT_BLACK);
    ttgo->tft->setTextFont(1);
    ttgo->tft->setTextColor(TFT_WHITE); // Orange
    ttgo->tft->drawCentreString(txt, 120, 80+linha*30, 4);
    
}

void drawCenterString(String msg, int linha, int fonte)
{
    ttgo->tft->drawString(msg, (120-ttgo->tft->textWidth(msg, 4)/2) , linha, fonte);
}

String identificaTeclaPressionada(int16_t x,int16_t y, bool enviarTecla)
{
    tecla = "";
    if (x > 0 && x < 60 && y > 40 && y < 106) {tecla = "0"; ttgo->tft->fillRoundRect(0,41,59,64, 4, TFT_YELLOW); }
    if (x > 60 && x < 120 && y > 40 && y < 106) {tecla = "1"; ttgo->tft->fillRoundRect(60,41,59,64, 4, TFT_YELLOW); }
    if (x > 120 && x < 180 && y > 40 && y < 106) {tecla = "2"; ttgo->tft->fillRoundRect(120,41,59,64, 4, TFT_YELLOW); }
    if (x > 180 && x < 240 && y > 40 && y < 106) {tecla = "3"; ttgo->tft->fillRoundRect(180,41,59,64, 4, TFT_YELLOW); }

    if (x > 0 && x < 60 && y > 106 && y < 171) {tecla = "4"; ttgo->tft->fillRoundRect(0,106,59,64, 4, TFT_YELLOW); }
    if (x > 60 && x < 120 && y > 106 && y < 171) {tecla = "5"; ttgo->tft->fillRoundRect(60,106,59,64, 4, TFT_YELLOW); }
    if (x > 120 && x < 180 && y > 106 && y < 171) {tecla = "6"; ttgo->tft->fillRoundRect(120,106,59,64, 4, TFT_YELLOW); }
    if (x > 180 && x < 240 && y > 106 && y < 171) {tecla = "7"; ttgo->tft->fillRoundRect(180,106,59,64, 4, TFT_YELLOW); }

    if (x > 0 && x < 60 && y > 171 && y < 240) {tecla = "8"; ttgo->tft->fillRoundRect(0,171,59,64, 4, TFT_YELLOW); }
    if (x > 60 && x < 120 && y > 171 && y < 240) {tecla = "9"; ttgo->tft->fillRoundRect(60,171,59,64, 4, TFT_YELLOW); }
    if (x > 120 && x < 180 && y > 171 && y < 240) {tecla = "O"; ttgo->tft->fillRoundRect(120,171,59,64, 4, TFT_YELLOW); }
    if (x > 180 && x < 240 && y > 171 && y < 240) {tecla = "C"; ttgo->tft->fillRoundRect(180,171,59,64, 4, TFT_YELLOW); }
    if (tecla!="")
    {
        ttgo->motor->onec(25);
        if (enviarTecla)
        {
            // enviaTecla(tecla);
        } else {
            if (tecla=="C")
            {
                sNumeros = sNumeros.substring(0,sNumeros.length()-1) ;
            } else if (tecla!="O") 
            {
                sNumeros = sNumeros + tecla;
            }
        }
        desenhaTeclas();
    }
    
    return(tecla);
}

void desenhaTeclas()
{
    ttgo->tft->fillRoundRect(0,41,59,64, 4, TFT_WHITE);
    ttgo->tft->fillRoundRect(60,41,59,64, 4, TFT_WHITE);
    ttgo->tft->fillRoundRect(120,41,59,64, 4, TFT_WHITE);
    ttgo->tft->fillRoundRect(180,41,59,64, 4, TFT_WHITE);

    ttgo->tft->fillRoundRect(0,106,59,64, 4, TFT_WHITE);
    ttgo->tft->fillRoundRect(60,106,59,64, 4, TFT_WHITE);
    ttgo->tft->fillRoundRect(120,106,59,64, 4, TFT_WHITE);
    ttgo->tft->fillRoundRect(180,106,59,64, 4, TFT_WHITE);

    ttgo->tft->fillRoundRect(0,171,59,64, 4, TFT_WHITE);
    ttgo->tft->fillRoundRect(60,171,59,64, 4, TFT_WHITE);
    ttgo->tft->fillRoundRect(120,171,59,64, 4, TFT_GREEN);
    ttgo->tft->fillRoundRect(180,171,59,64, 4, TFT_YELLOW);

    ttgo->tft->setTextColor(TFT_BLACK);
    ttgo->tft->setCursor(26,70);ttgo->tft->print("0");
    ttgo->tft->setCursor(86,70);ttgo->tft->print("1");
    ttgo->tft->setCursor(146,70);ttgo->tft->print("2");
    ttgo->tft->setCursor(206,70);ttgo->tft->print("3");

    ttgo->tft->setCursor(26,135);ttgo->tft->print("4");
    ttgo->tft->setCursor(86,135);ttgo->tft->print("5");
    ttgo->tft->setCursor(146,135);ttgo->tft->print("6");
    ttgo->tft->setCursor(206,135);ttgo->tft->print("7");

    ttgo->tft->setCursor(26,200);ttgo->tft->print("8");
    ttgo->tft->setCursor(86,200);ttgo->tft->print("9");
    ttgo->tft->setCursor(140,200);ttgo->tft->print("OK");
    ttgo->tft->setCursor(192,200);ttgo->tft->print("ESC");
    ttgo->tft->setTextColor(TFT_WHITE);
    ttgo->tft->setTextSize(2);
}


void mostrarCabecalho(boolean fullUpdate)
{
    byte xpos = 54; // Stating position for the display
    byte ypos = 10;

    // Get the current data
    RTC_Date tnow = ttgo->rtc->getDateTime();

    hh = tnow.hour;
    mm = tnow.minute;
    ss = tnow.second;
    dday = tnow.day;
    mmonth = tnow.month;
    yyear = tnow.year;

    if (fullUpdate) {

        obtemBateria();
        // Bateria
        ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK); // Orange
        ttgo->tft->fillRoundRect(10, 8, 20, 30, 0, 0x39C4);
        int8_t bat = (30 * battery)/100;

        if (battery>20)
        {
            ttgo->tft->fillRoundRect(10, 38-bat, 20, bat, 0, TFT_GREEN);
        } else {
            ttgo->tft->fillRoundRect(10, 38-bat, 20, bat, 0, TFT_RED);
        }
        for (int8_t a=0; a<30; a++)
        {
            a++;a++;a++;
            ttgo->tft->drawFastHLine(10,8+a,20,0x39C4);    
        }
        ttgo->tft->setTextSize(1);
        ttgo->tft->drawString(String(battery)+"%", 10, 44, 2);

        // WiFi e Internet
        if (bWIFI_pronto)
        {
            ttgo->tft->fillRoundRect(216, 10, 22, 21, 3, TFT_GREEN);
        } else {
            ttgo->tft->fillRoundRect(216, 10, 22, 21, 3, TFT_RED);
        }
        
        if (bWIFI_internetDisponivel)
        {
            ttgo->tft->fillRoundRect(216, 36, 22, 21, 3, TFT_GREEN);
        } else {
            ttgo->tft->fillRoundRect(216, 36, 22, 21, 3, TFT_RED);
        }
        
        ttgo->tft->setTextSize(2);
        ttgo->tft->setTextColor(TFT_BLACK); // Orange
        ttgo->tft->setCursor( 222, 14);
        ttgo->tft->print("W");
        ttgo->tft->setCursor( 222, 39);
        ttgo->tft->print("I");

        ttgo->tft->setTextColor(0x39C4, TFT_BLACK);
        ttgo->tft->setTextSize(1);

        // Font 7 is a 7-seg display but only contains
        // characters [space] 0 1 2 3 4 5 6 7 8 9 0 : .
        ttgo->tft->setTextColor(0x39C4, TFT_BLACK); // Set desired color
        ttgo->tft->drawString("88:88", xpos, ypos, 7);
        ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK); // Orange

        if (hh < 10) xpos += ttgo->tft->drawChar('0', xpos, ypos, 7);
        xpos += ttgo->tft->drawNumber(hh, xpos, ypos, 7);
        xcolon = xpos + 3;
        xpos += ttgo->tft->drawChar(':', xcolon, ypos, 7);
        if (mm < 10) xpos += ttgo->tft->drawChar('0', xpos, ypos, 7);
        ttgo->tft->drawNumber(mm, xpos, ypos, 7);

        // Data
        ttgo->tft->setTextColor(TFT_GREEN, TFT_BLACK);
        ttgo->tft->setTextSize(2);
        ttgo->tft->setCursor( 10, 215);
        ttgo->tft->print(dday);
        ttgo->tft->print("/");
        ttgo->tft->print(mmonth);
        ttgo->tft->print("/");
        ttgo->tft->print((yyear-2000));

    }

    if (ss % 2) { // Toggle the colon every second
        ttgo->tft->setTextSize(1);
        ttgo->tft->setTextColor(0x39C4, TFT_BLACK);
        xpos += ttgo->tft->drawChar(':', xcolon, ypos, 7);
    } else {
        
        ttgo->tft->setTextSize(1);
        ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK);
        ttgo->tft->drawChar(':', xcolon, ypos, 7);
    }
    ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK);    

}

void mostraItensASelecionar(int mSel, bool mostrarId)
{
    ttgo->tft->fillRect(0, 60, 240, 180, TFT_BLACK);

    for(int8_t i=0; i < maxItensEncontrados; i++)
    {
        line = sCombo[i];
        comboIds[i] = CSV_obtemCampoEmLinha(line,1);
        comboTexts[i] = CSV_obtemCampoEmLinha(line,2);
    }

    int curSel = 0;

    ttgo->tft->fillRect(0, 120, 239, 60, TFT_WHITE);

    if (mSel == 0) curSel = maxItensEncontrados - 1;
    else curSel = mSel - 1;
    
    ttgo->tft->setTextSize(2);
    ttgo->tft->setCursor( 8, 86);
    ttgo->tft->setTextColor(TFT_WHITE);
    // ttgo->tft->drawString(comboIds[curSel]+" "+comboTexts[curSel], 8, 86, 4);
    if (mostrarId)
    {
        ttgo->tft->print(comboIds[curSel]+" "+comboTexts[curSel]);
    } else {
        ttgo->tft->print(comboTexts[curSel]);
    }

    ttgo->tft->setCursor( 8, 146);
    ttgo->tft->setTextColor(TFT_BLACK);
    // ttgo->tft->drawString(comboIds[mSel]+" "+comboTexts[mSel], 8, 146, 4);
    if (mostrarId)
    {
        ttgo->tft->print(comboIds[mSel]+" "+comboTexts[mSel]);
    } else {
        ttgo->tft->print(comboTexts[mSel]);
    }
    

    if (mSel == maxItensEncontrados - 1) curSel = 0;
    else curSel = mSel + 1;

    ttgo->tft->setCursor( 8, 206);
    ttgo->tft->setTextColor(TFT_WHITE);
    if (mostrarId)
    {
        ttgo->tft->print(comboIds[curSel]+" "+comboTexts[curSel]);
    } else {
        ttgo->tft->print(comboTexts[curSel]);
    }
}

int8_t dialogoSelecionar(String titulo, bool mostrarId)
{
    ttgo->tft->fillScreen(TFT_BLACK);
    ttgo->tft->fillRect(0, 0, 240, 59, TFT_YELLOW);

    ttgo->tft->setTextSize(1);
    ttgo->tft->setTextColor(TFT_BLACK);
    drawCenterString(titulo, 18, 4);

    mostraItensASelecionar(0, mostrarId); // display the list of Apps

    int8_t mSelect = 0; // Seleção
    boolean exitMenu = false; // used to stay in the menu until user selects app
    while (!exitMenu) {
        if (ttgo->getTouch(x, y)) { 
            
            while (ttgo->getTouch(x, y)) {} 

            if (y < 60)
            {
                exitMenu = true;
                mSelect = -1;
            }
            if (y >= 180) { 
                mSelect += 1;
                if (mSelect == maxItensEncontrados) mSelect = 0;
                // bugTrace(true,"[INF] Combo avancar: "+String(mSelect));
                mostraItensASelecionar(mSelect, mostrarId);
            }
            if (y > 60 && y <= 120) { 
                mSelect -= 1;
                if (mSelect < 0) mSelect = maxItensEncontrados - 1;
                // bugTrace(true,"[INF] Combo voltar: "+String(mSelect));
                mostraItensASelecionar(mSelect, mostrarId);
            }
            if (y > 120 && y < 180) { 
                bugTrace(true,"[INF] Combo selecionado: "+String(mSelect));
                exitMenu = true;
            }
        }

    }
    //Return with mSelect containing the desired mode
    ttgo->tft->fillScreen(TFT_BLACK);
    return (mSelect);
}

int8_t dialogoConfirmar(String msg)
{
    ttgo->tft->fillRect(0, 0, 240, 59, TFT_YELLOW);
    ttgo->tft->fillRect(0, 120, 240, 180, TFT_BLACK);

    ttgo->tft->setTextSize(1);
    ttgo->tft->setTextColor(TFT_BLACK);
    drawCenterString("Confirme...", 18, 4);

    ttgo->tft->fillRect(0, 60, 239, 59, TFT_WHITE);
    drawCenterString(msg, 80, 4);

    // 146 - 206
    ttgo->tft->fillRect(0, 120, 120, 120, TFT_GREEN);
    ttgo->tft->drawString("Sim", 38, 170, 4);
    ttgo->tft->fillRect(120, 120, 120, 120, TFT_RED);
    ttgo->tft->setTextColor(TFT_WHITE);
    ttgo->tft->drawString("Nao", 160, 170, 4);


    int8_t mSelect = 0; // Seleção
    boolean exitMenu = false; // used to stay in the menu until user selects app
    
    timerEspera = millis() + 5000;
    while (!exitMenu) {
        if (millis() > timerEspera)
        {
            exitMenu = true;
        }

        if (ttgo->getTouch(x, y)) 
        { 
            while (ttgo->getTouch(x, y)) {} 

            if (y < 120)
            {
                exitMenu = true;
                mSelect = 0;
            }
            if (y >= 120 && x < 120) { 
                exitMenu = true;
                mSelect = 1;
            }
            if (y >=120 && x >=120) { 
                exitMenu = true;
                mSelect = 2;
            }
        }
    }
    ttgo->tft->fillScreen(TFT_BLACK);
    return (mSelect);    
}

void dialogoMensagem(String txt, bool ok, int esperar)
{
    if (ok)
    {
        ttgo->tft->fillScreen(TFT_GREEN);
    } else {
        ttgo->tft->fillScreen(TFT_RED);
    }    
    ttgo->tft->setTextSize(1);
    ttgo->tft->setTextWrap(false);
    ttgo->tft->setTextFont(1);
    if (ok)
    {
        ttgo->tft->setTextColor(TFT_BLACK);
    } else {
        ttgo->tft->setTextColor(TFT_WHITE);
    }    
    // ttgo->tft->drawString(txt, 10, 110, 4);
    drawCenterString(txt, 110, 4);
    esperaToqueNaTela(esperar);
}

void dialogoMensagemDupla(String txt1, String txt2, bool ok, int esperar)
{
    if (ok)
    {
        ttgo->tft->fillScreen(TFT_GREEN);
    } else {
        ttgo->tft->fillScreen(TFT_RED);
    }    
    ttgo->tft->setTextSize(1);
    ttgo->tft->setTextWrap(false);
    ttgo->tft->setTextFont(1);
    if (ok)
    {
        ttgo->tft->setTextColor(TFT_BLACK);
    } else {
        ttgo->tft->setTextColor(TFT_WHITE);
    }    
    drawCenterString(txt1, 90, 4);
    drawCenterString(txt2, 120, 4);

    esperaToqueNaTela(esperar);
}

void dialogoNumeros(String mensagem)
{
    bool mantemLoop = true;
    
    sNumeros = "";
    ttgo->tft->fillScreen(TFT_BLACK);

    ttgo->tft->fillRect(0,0,240,20, TFT_YELLOW);
    ttgo->tft->fillRect(0,20,240,20, TFT_BLUE);
    ttgo->tft->setTextSize(2);
    ttgo->tft->setCursor(10,3);
    ttgo->tft->setTextColor(TFT_BLACK);
    ttgo->tft->print(mensagem);

    ttgo->tft->setCursor(10,22);
    // ttgo->tft->print(sLCD[1]);

    desenhaTeclas();

    unsigned long expirar = millis()+20000;

    do
    {
        if (millis() > expirar)
        {
            mantemLoop = false;
        } else {
            if (ttgo->getTouch(x, y))
            {
                expirar = millis()+20000;
                identificaTeclaPressionada(x,y, false);
                while (ttgo->getTouch(x, y)) {} 
                if (tecla=="O" || sNumeros.length()> 11)
                {
                    mantemLoop = false;
                } else {
                    if (y < 40)
                    {
                        sNumeros = "";
                        mantemLoop = false;
                        delay(100);
                    } else {
                        ttgo->tft->fillRect(0,20,240,20, TFT_BLUE);
                        ttgo->tft->setCursor(10,22);
                        ttgo->tft->setTextColor(TFT_WHITE);
                        ttgo->tft->print(sNumeros);
                    }                    
                }
                
            }
        }
    } while (mantemLoop);

    ttgo->tft->fillScreen(TFT_BLACK);
    timerInatividade = millis();
}


// --------------------------------------------------------------

void telaInicio(boolean fullUpdate)
{
    mostrarCabecalho(fullUpdate);

    if (fullUpdate) {
        
        if (bSongMode)
        {
            // Período da aula
        } else {
            ttgo->tft->fillRoundRect(0, 74, 240, 31, 6, TFT_BLUE);
            ttgo->tft->setTextSize(1);
            ttgo->tft->setTextColor(TFT_WHITE);
            ttgo->tft->drawString("SETLIST WATCH", 26, 80, 4);
            ttgo->tft->setTextColor(0x03FF);
            ttgo->tft->drawCentreString(String(selectedSetlist), 120, 120, 2);
            ttgo->tft->setTextSize(1);
        }
    }

}

void telaInformacoes()
{
    m = millis();
    int min = int(m/60000);
    int hor = int(min/60);
    min = (min - hor*60);
    mostrarCabecalho(true);

    // Informações
    ttgo->tft->setTextColor(TFT_WHITE);
    ttgo->tft->setTextSize(2);
    ttgo->tft->setCursor( 48, 70);
    ttgo->tft->print(String(UUID).substring(6));

    ttgo->tft->setTextSize(1);
    ttgo->tft->drawString("Versao", 4, 92, 2);
    ttgo->tft->drawString(": "+String(VERSAO), 75, 92, 2);
    ttgo->tft->drawString("Bateria", 4, 110, 2);
    ttgo->tft->drawString(": "+String(battery)+"%", 75, 110, 2);
    ttgo->tft->drawString("Ligado", 4, 128, 2);
    ttgo->tft->drawString(": "+String(hor)+"horas "+String(min)+" minutos", 75, 128, 2);
    ttgo->tft->drawString("Rede", 4, 146, 2);
    ttgo->tft->drawString(": "+String(ssid)+" ("+String(canal)+")", 75, 146, 2);
    ttgo->tft->drawString("IP Local" , 4, 164, 2);
    ttgo->tft->drawString(": "+String(IP) , 75, 164, 2);

    esperaToqueNaTela(15000);
}

