#pragma once
class zmirror
{
public:
	zmirror();
	~zmirror();
private:
	void*	mirrorSocket;			//req,¾µÏñsocket
	char*	mirrorServerAddr;		//¾µÏñ·þÎñµØÖ·
public:
	int		InitialConnect();
	int		MirrorService();
};

