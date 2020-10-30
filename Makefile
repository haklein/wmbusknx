all:
	g++ wmbusknx.cpp Telegram.cpp -ljsoncpp -leibclient -o wmbusknx


