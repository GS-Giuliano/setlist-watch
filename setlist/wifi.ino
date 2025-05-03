/**

WIFI - Descoberta e Conexão

Company: GIUSOFT Tecnologia
Author : Giuliano Nascimento
E-mail : giuliano@giusoft.com.br
Date   : 2019-08-19

*/
void WiFi_Desativar()
{
	WiFi.disconnect();
}

void acertaDataHoraPeloWifi()
{
	struct tm timeinfo;
	if (bWIFI_internetDisponivel && !bAcertouHoraQuandoLigou)
	{
		bAcertouHoraQuandoLigou = true;
		LCDcls();
		LCDtextoCentralizado(F("Acertando data/hora"),0);
		LCDtextoCentralizado(F("Pelo servidor remoto"),1);

		bugTrace(true,F("."));
		bugTrace(true, F("[ INF ] Acertando hora pela API"));

		WiFiClient client;
		byte inicio = 0;
		bool configuracaoAlterada = false;
		line = "";
		const int httpPort = 80;
		bool ehResposta = false;

		if (!client.connect(host, httpPort)) {

			// Estava conectado e desconectou agora...
			if (bWIFI_internetDisponivel)
			{
				bugTrace(true, F("[ERR] Internet caiu!"));
				log("9002;0","Internet desconectada");
			}
			bWIFI_internetDisponivel = false;
			log("9014;0","Buscou hora pelo WiFi");
			return;
		}

		bWIFI_internetDisponivel = true;

		montaUrl("date");
		bugTrace(true,url);
		sResposta = "";

		client.print(String("GET ") + url + " HTTP/1.1\r\n" +
		"Host: " + host + "\r\n" +
		"Connection: close\r\n\r\n");
		unsigned long timeout = millis();
		while (client.available() == 0) {
			if (millis() - timeout > 5000) {
				bugTrace(true, F("[ERR] Timeout !"));
				cntTimeout++;
				client.stop();
				return;
			}
		}

		me = "";
		while(client.available()) {
			String line = client.readStringUntil('\n');
			if (inicio>0)
			{
				if (line.length() > 10)
				{
					me+=line;
				}
			}
			if (line.indexOf("-Type")>-1)
				inicio++;
		}
		client.stop();

		bugTrace(false,F("[INF] Data/hora no servidor: "));
		bugTrace(true,me);

		// 2022-01-30 16:50:04

		int year = String(me.substring(0,5)).toInt();
		int month = String(me.substring(5,7)).toInt();
		int day = String(me.substring(8,10)).toInt();
		int hour = String(me.substring(11,13)).toInt();
		int minute = String(me.substring(14,16)).toInt();
		int second = String(me.substring(17,19)).toInt();

		// Se vieram dados válidos, então conseguiu acertar pela API
		if (year >= 2022 && year< 2050 && month>=1 && month <= 12 && day >= 1 && day <= 31)
		{
			// setTime(hour, minute, second, day, month, year);
			ttgo->rtc->setDateTime(year, month, day, hour,minute, 0);
			log("9014;1","Buscou hora pelo WiFi");
		}
		cntTimeout = 0;
	}
}



void buscaRedes()
{
	//if (millis()-timerUltimaBuscaWiFi > 15000)
	String linha = "";
	redeEncontrada = "";
	bool encontrouTemoraPlaca = false;
  	bEncontrouRede = false;
  	LCDcls();
	LCDtextoCentralizado("Buscando",0);
	LCDtextoCentralizado("redes...",1);
	Serial.println(F("."));
	bugTrace(true,"[ NETWORK ]");

	int n = WiFi.scanNetworks();
	long f = -75;
	sRedesEncontradas="";

	strcpy(ssid,"");
	strcpy(password,"");

	if (n == 0) {
		LCDtextoCentralizado("Nao encontrada! ",0);
		bugTrace(true, F("[INF] Nenhuma rede encontrada."));
	} else {
		LCDtextoCentralizado("Encontrei "+String(n),0);
		LCDtextoCentralizado("redes proximas!",1);
		delay(500);
		for (int i = 0; i < n; ++i) {
			sRedesEncontradas+=String("<tr><td><a href='/saveWiFi?ssid=")+urlencode(String(WiFi.SSID(i)))+String("' class='button'>")+String(WiFi.SSID(i))+String("</a></td><td>")+String(WiFi.RSSI(i));
			redeEncontrada = WiFi.SSID(i) + " (" + String(WiFi.RSSI(i)) + ") " + "Canal " + String(WiFi.channel(i));
			redeEncontrada+=(WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*";
			delay(10);

			// Buscando por OUTRAS redes
			if (WiFi.SSID(i)==cfg_TEM_SSID)
			{
				// Se não encontrou rede outra ou o sinal desta for mais forte, usa ela
				WiFi.SSID(i).toCharArray(ssid, 40);
				canal = WiFi.channel(i);
				cfg_TEM_PASSWORD.toCharArray(password,40);
				f = WiFi.RSSI(i);
				bEncontrouRede = true;
				encontrouTemoraPlaca = true;
				redeEncontrada+=F(" <---");
				sRedesEncontradas+=String("•••");
			}

			// Temora de carro
			if (!bEncontrouRede && WiFi.SSID(i) == "temora2017")
			{
				if (!bEncontrouRede || WiFi.RSSI(i) > f)
				{
					WiFi.SSID(i).toCharArray(ssid, 40);
					canal = WiFi.channel(i);
					strcpy(password, "temora2017");
					f = WiFi.RSSI(i);
					bEncontrouRede = true;
				}
				redeEncontrada+=F(" <---");
				sRedesEncontradas+=String("•••");
			}

			// Temora de moto
			if (!encontrouTemoraPlaca && WiFi.SSID(i) == "temora2023")
			{
				WiFi.SSID(i).toCharArray(ssid, 40);
				canal = WiFi.channel(i);
				strcpy(password, "temor@Mobile");
				f = WiFi.RSSI(i);
				bEncontrouRede = true;
				redeEncontrada+=F(" <---");
				sRedesEncontradas+=String("•••");
			}
			// Temora 
			if (!bEncontrouRede && WiFi.SSID(i) == "temora")
			{
				if (!bEncontrouRede || WiFi.RSSI(i) > f)
				{
					WiFi.SSID(i).toCharArray(ssid, 40);
					canal = WiFi.channel(i);
					strcpy(password, "brasil2023");
					f = WiFi.RSSI(i);
					bEncontrouRede = true;
				}
				redeEncontrada+=F(" <---");
				sRedesEncontradas+=String("•••");
			}

			if (!bEncontrouRede && WiFi.SSID(i) == "iPhone de Giuliano")
			{
				if (!bEncontrouRede || WiFi.RSSI(i) > f)
				{
					WiFi.SSID(i).toCharArray(ssid, 40);
					canal = WiFi.channel(i);
					strcpy(password, "e5d4c3b2a1");
					f = WiFi.RSSI(i);
					bEncontrouRede = true;
				}
				redeEncontrada+=F(" <---");
				sRedesEncontradas+=String("•••");
			}

			if (!bEncontrouRede && WiFi.SSID(i) == "GIUSOFT 2,4GHZ")
			//if (!bEncontrouRede && WiFi.SSID(i) == "TEMORA GIUSOFT")
			{
				if (!bEncontrouRede || WiFi.RSSI(i) > f)
				{
				WiFi.SSID(i).toCharArray(ssid, 40);
				canal = WiFi.channel(i);
				strcpy(password, "giu2016soft");
				f = WiFi.RSSI(i);
				bEncontrouRede = true;
				}
				redeEncontrada+=F(" <---");
				sRedesEncontradas+=String("•••");
			}
     
			bugTrace(true, redeEncontrada);
		}

	}
}




// Conecta a uma rede que faz parte do equipamento
bool conectarRede()
{
    bInternetEmUso = true;
	if (strcmp(ssid,"")!=0)
	{
		timerUltimoTesteInternet = 0;
		WiFi.config(ipzero,ipzero,ipzero,ipzero);
		WiFi_Desativar();
		bugTrace(true,F("."));
		bugTrace(false, F("\n[INF] Rede: ["));
		bugTrace(false, ssid);
		bugTrace(true, "] ");
		LCDcls();
		LCDtextoCentralizado(F("Conectando..."),0);
		LCDtextoCentralizado(ssid,1);



		// PARA DEFINIR UM IP FIXO, DESCOMENTE AS LINHAS ABAIXO:
		// if (WiFi.config(TEMORA_staticIP, TEMORA_gateway, TEMORA_subnet, TEMORA_dns1, TEMORA_dns2) == false) {
		// 	bugTrace(true, F("Falhou ao tentar setar IP 192.168.1.63 para cliente rede TEMORA4"));
		// }
		WiFi.begin(ssid, password);
		int cnt = 0;
		while ((WiFi.status() != WL_CONNECTED) && cnt < TEMPO_ESPERA_CONEXAO) {
			cnt++;
			delay(250);
			bugTrace(false, "."+String(WiFi.status()));
		}
		bugTrace(true, "");
		if (WiFi.status() == WL_CONNECTED)
		{
			LCDcls();
			LCDtextoCentralizado(F("Conectado!"),0);
			Serial.print("[INF] IP local: ");
			Serial.println(WiFi.localIP());
			delay(500);
			ttgo->motor->onec();
			// Busca configuração
			API_download();

			bWIFI_pronto = true;
			bWIFI_ativo = true;
            bInternetEmUso = false;
			return(true);
		}
	} else {
		LCDcls();
		LCDtextoCentralizado(F("Falhou!"),0);
        bInternetEmUso = false;
		return(false);
	}
    bInternetEmUso = false;
}


bool pingOk(byte o1, byte o2, byte o3, byte o4)
{
    bInternetEmUso = true;
	IPAddress ipTeste(o1, o2, o3, o4);
	//bool pingResult = Ping.ping(ipTeste,3);
	bPingando = true;
	bool pingResult = ping_start(ipTeste, 1, 0, 0, 3);
	bugTrace(false, "[INF] Pingando: "+String(o1)+"."+String(o2)+"."+String(o3)+"."+String(o4)+" ");
	bPingando = false;
	if (pingResult)
	{
		bUltimoPingComSucesso = true;
		bugTrace(true, F(" -- Ok "));
		return(true);
	} else {
		bUltimoPingComSucesso = false;
		bugTrace(true, F(" -- falhou! "));
		return(false);
	}
    bInternetEmUso = false;
}


// Está conectado ao WiFi?
bool conectadoAoWiFi()
{
	bool ok = false;
	if (WiFi.status() == WL_CONNECTED)
	{
		IpAddress2String(WiFi.localIP()).toCharArray(IP, sizeof(IP));
		//bugTrace(true,F("[INF] WiFi acessível!"));
		bWIFI_ativo = true;
		return (true);
	} else {
		WiFi_Desativar();
		if (bWIFI_ativo)
		{
			bugTrace(true,F("[ERR] WiFi desconectado!"));
		}
		bWIFI_ativo = false;
		bWIFI_internetDisponivel = false;
		return (false);
	}
}

bool conectadoAInternet()
{
	bool ok = true;
	IPAddress addr;
	if (WiFi.status() == WL_CONNECTED)
	{

		//bugTrace(false, String(agora));
		// bugTrace(false, F("[INF] Teste de Internet "));
		// bugTrace(false, F("-- ["));
		// bugTrace(false, String(ssid));
		// bugTrace(false, F("] "));
		// bugTrace(false, F("- Canal "));
		// bugTrace(true, String(canal));
		if (millis() > timerUltimoTesteInternet + INTERVALO_TESTE_INTERNET)
		{
			ipVeiculoAcessivel = false;
			timerUltimoTesteInternet = millis();
			// Verifica se DHCP retornou IP válido
			if (WiFi.localIP()[0]==0)
			{

				WiFi_Desativar();
				bugTrace(true, F("[INF] IP obtido invalido!"));
				ok = false;
			} else {
				bugTrace(false, F("[INF] IP local: "));
				bugTrace(true, IpAddress2String(WiFi.localIP()));
				IpAddress2String(WiFi.localIP()).toCharArray(IP, sizeof(IP));
			}

			// Ip Obtido com sucesso?
			if (ok)
			{
				// Testa se está pingando...
				ok = pingOk(8,8,8,8); // Google
				if (!ok)
				{
					ok = pingOk(1,1,1,1); // Cloudflare
				}
				
				// if (CSV_obtemCampoEmLinha(sPlacas[placaSelecionada],5)!="")
				// {
				// 	if (addr.fromString(CSV_obtemCampoEmLinha(sPlacas[placaSelecionada],5))) {
				// 		ipVeiculoAcessivel = (pingOk(addr[0], addr[1], addr[2], addr[3]));
				// 		if(ipVeiculoAcessivel)
				// 		{
				// 			bugTrace(true, F("[INF] IP veiculo: Acessivel"));
				// 		}else{
				// 		bugTrace(true, F("[INF] IP veiculo: Inacessivel!"));							
				// 		}
				// 	} else {
				// 		bugTrace(true, F("[INF] IP veiculo: Falhou!"));
				// 	}
				// }

			}

			// if (completa)
			// {
			// 	if (ok)
			// 	{
			// 		// Testa DNS
			// 		int err = WiFi.hostByName(host, ipTeste) ;
			// 		if(err == 1){
			// 			ok = true;
			// 			bUltimoDNSComSucesso = true;
			// 			String ip = IpAddress2String(ipTeste);
			// 			ip.toCharArray(host, sizeof(host));
			// 			strcat(host,"/webcfc");
			// 			bugTrace(false, F("-- DNS funcionando. IP da API: "));
			// 			bugTrace(true,String(host));
			// 			// Serial.print("Ip address: ");
			// 			// Serial.println(ipServer);
			// 		} else {
			// 			ok = false;
			// 			bUltimoDNSComSucesso = false;
			// 			bugTrace(true, F("-- DNS falhou!"));
			// 			// Serial.print("Error code: ");
			// 			// Serial.println(err);
			// 		}
			// 	}
			// }
		} else {
			// bugTrace(true, F("[INF] Testado recentemente..."));
			ok = bWIFI_internetDisponivel;
		}

	} else {
		bWIFI_pronto = false;
		bWIFI_ativo = false;
		ok = false;
	}

	if (ok)
	{
		timerSemInternet = millis();
		if (!bWIFI_internetDisponivel)
		{
			bugTrace(true, F("[INF] Internet OK"));
		}
		bWIFI_pronto = true;
		bWIFI_ativo = true;
		bWIFI_internetDisponivel = true;
	} else {
		if (bWIFI_internetDisponivel)
		{
			bugTrace(true, F("[INF] Internet falhou!"));
		}
		bWIFI_internetDisponivel = false;
	}
	return(ok);
}


// Busca redes e conecta se encontrar alguma conhecida
void conectaRedeWifi()
{

	if (!conectadoAoWiFi() )// && (millis()-timerUltimaBuscaWiFi > 10000)
	{
		timerUltimaBuscaWiFi = millis();

		bool conectou = false;

		char srv_ssid[50]     = "";
		char srv_password[50] = "";
		String conectado = "";

		bWIFI_pronto = false;
		bWIFI_internetDisponivel = false;
		WiFi.disconnect(false,true);
		WiFi.mode(WIFI_MODE_STA);
        WiFi.setSleep(WIFI_PS_NONE);
        // esp_wifi_set_ps(WIFI_PS_NONE);
		buscaRedes();
		bugTrace(true, "");

		conectou = conectarRede();

		if (conectou)
		{
      		timerUltimoTesteInternet = 0;
			conectadoAInternet();
			if (bWIFI_internetDisponivel)
			{
				bHoraDefinida = false;
				bPrimeiroAcessoAInternet = false;

				// Atualiza data e hora pela Internet
				acertaDataHoraPeloWifi();
			}
		} else {
		}
		LCDcls();
	}
}
