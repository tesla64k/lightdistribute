#pragma once
class zmirror
{
public:
	zmirror();
	~zmirror();
private:
	void*	mirrorSocket;			//req,����socket
	char*	mirrorServerAddr;		//��������ַ
public:
	int		InitialConnect();
	int		MirrorService();
};

