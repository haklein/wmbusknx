#include <string>
#include <chrono>
#include <stdio.h>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include <jsoncpp/json/value.h>
#include <iostream>

#include <eibclient.h>

#include "Telegram.h"

EIBConnection *eibcon;

bool sendTelegram(EIBConnection *eibcon, std::string ga, uint32_t value) {
	Telegram *tg = new Telegram();
	tg->setGroupAddress(Telegram::stringtogaddr(ga));
	uint32_t converted = __builtin_bswap32(value);
	tg->setUserData((const unsigned char *)&converted, 4);
	return tg->sendTo(eibcon);
}


int main(int argc, char **argv) {
	if (argc != 4) {
		std::cerr << "need 4 argments: <knxd url> <ga> <json>, got: " << argc << std::endl;
		exit(-1);
	}
	std::string strJson(argv[3]);
	std::cout << "parsing: " << strJson << std::endl;

	Json::Value root;   
	Json::Reader reader;
	bool parsingSuccessful = reader.parse( strJson.c_str(), root );     //parse process
	if ( !parsingSuccessful )
	{
		std::cout  << "Failed to parse" << reader.getFormattedErrorMessages();
		return 0;
	}
	float total_m3 = std::stof(root.get("total_m3","").asString());
	uint32_t total_l = total_m3 * 1000;
	std::cout << "Total m3:" << total_m3 << std::endl;
	std::cout << "Total l:" << total_l << std::endl;

	eibcon = EIBSocketURL(argv[1]);
       if (!eibcon) {
            std::cerr << "Can't connect to knxd socket" << std::endl;
            return -1;
        }

        if (EIBOpen_GroupSocket (eibcon, 0) == -1) {
            std::cerr << "Can't open knxd group socket" << std::endl;
            return -1;
        }

	sendTelegram(eibcon, argv[2], total_l);

	EIBClose(eibcon);


	return 0;

}

