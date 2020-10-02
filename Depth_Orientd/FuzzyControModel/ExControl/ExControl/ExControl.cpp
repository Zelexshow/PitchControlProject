// ExControl.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <iostream>
#include<cstdlib>
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <stdio.h>

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")


#define NINT(a) ((a) >= 0.0 ? (int)((a)+0.5) : (int)((a)-0.5))
float *SwapArray;
float GetSwapValue(int Index) { return(SwapArray[Index - 1]); }
void SetSwapValue(int Index, float Val) { SwapArray[Index - 1] = Val; }

extern "C" void __declspec(dllexport) __cdecl DISCON(float *avrSwap,
	int *aviFail, char *accInfile, char *avcOutname, char *avcMsg)
{
	int iStatus;
	static float rPitchDemand;
	//定义控制变量:转矩、三个桨距角、一个保留区other
	double torque, pitch1, pitch2, pitch3;// other//speed
	SwapArray = avrSwap;	//Store the pointer
							//Make sure there's a C string terminator
	accInfile[NINT(avrSwap[49])] = '\0';
	avcOutname[NINT(avrSwap[50])] = '\0';
	avcMsg[0] = '\0';
	iStatus = NINT(avrSwap[0]);


	//TCP通讯
	int iResult = 0;
	WSADATA wsaData;

	SOCKET ServeSocket;  // 服务端端口描述字
	SOCKET ClientSocket; // 客户端端口描述字
	sockaddr_in ServeAddr; // 服务端端口地址
	sockaddr_in ClientAddr;// 客户端端口地址

	unsigned short Port = 1102;// 服务端端口号

	char  ControlBuf[1024]; // 存放simulink发送过来的数据 
	char  WindBuf[1024];  // 存放发往simulink的风机数据 

	memset(WindBuf, '\0', sizeof(WindBuf));//初始化

	int ServeAddrSize = sizeof(ServeAddr);// 存放端口地址长度
	int ClientAddrSize = sizeof(ClientAddr);

	//-----------------------------------------------
	// Initialize Winsock 初始化
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		wprintf(L"WSAStartup failed with error %d\n", iResult);
	}

	//-----------------------------------------------
	// Create a receiver socket to receive datagrams 创建套接字
	ServeSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (ServeSocket == INVALID_SOCKET)
	{
		wprintf(L"socket failed with error %d\n", WSAGetLastError());

	}
	//-----------------------------------------------
	// Bind the socket to any address and the specified port.
	//blind之前要把const struct sockaddr FAR* 中的结构变量初始化
	ServeAddr.sin_family = AF_INET;
	ServeAddr.sin_port = htons(Port);
	ServeAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	//将上面的信息赋值给服务段套接字
	iResult = bind(ServeSocket, (SOCKADDR *)& ServeAddr, ServeAddrSize);
	if (iResult != 0)
	{
		wprintf(L"bind failed with error %d\n", WSAGetLastError());
	}

	//开始监听
	if (listen(ServeSocket, 10) == SOCKET_ERROR)
	{
		printf("监听失败:%d", WSAGetLastError());
	}

	//等待客户请求到来  		

	SOCKET	sockConn = accept(ServeSocket, (SOCKADDR *)& ClientAddr, &ClientAddrSize);
	//阻塞，直到有连接 sockConn为新建立的与客户端的套接字描述字
	if (sockConn == SOCKET_ERROR) {
		printf("等待请求失败:%d", WSAGetLastError());
	}
	printf("客户端的IP是:[%s]\n", inet_ntoa(ClientAddr.sin_addr));


	//-----------------------------------------------
	//将海流能信息放到WindBuf里

	//float information[5];
	float information[8];

	//转矩控制相关
	information[0] = GetSwapValue(20); //电机速度:rad/s
	information[1] = GetSwapValue(16);//最优模态增益:Nm/(rad/s)^2
	information[2] = GetSwapValue(23);//电机扭矩:Mm

	//功率控制相关参数
	information[3] = GetSwapValue(15);//电机功率:W

	//暂时用不到的变量
	
	information[4] = GetSwapValue(30);//叶根弯矩1
	information[5] = GetSwapValue(31);//叶根弯矩2
	information[6] = GetSwapValue(32);//叶根弯矩3
	information[7] = GetSwapValue(60); //叶片1方位角


	int ValidateBufLen = 0;
	//仅用到转矩控制和功率控制所需的模型参数
	for (int i = 0; i<8; i++)
	{
		ValidateBufLen = strlen(WindBuf);
		sprintf(WindBuf + ValidateBufLen, "%g;", information[i]);
	}

	//接收来自MyFunc传过来的控制信号
	memset(ControlBuf, '\0', sizeof(ControlBuf));//清零初始化
	int iRecv = recv(sockConn, ControlBuf, sizeof(ControlBuf), 0); //阻塞，直至收到信息			
	printf("recv dada: %s \n", ControlBuf);

	//从ControlBuf中取出数据放入临时变量
	sscanf(ControlBuf, "%lf;%lf;%lf;%lf;", &torque, &pitch1, &pitch2, &pitch3); //存收到的数据
	//sscanf(ControlBuf, "%g;%g;%g;%g;", &torque, &pitch1, &pitch2, &pitch3);
																							//将模型参数传送给MyFunc								 
	int iSend = send(sockConn, WindBuf, sizeof(WindBuf), 0);

	// 发送 没发送成功就一直发送
	while (iSend == 0 || iSend <0) {
		iSend = send(sockConn, WindBuf, sizeof(WindBuf), 0);
	}
	printf("send dada: %s \n", WindBuf);

	//-----------------------------------------------
	// Close the socket
	closesocket(sockConn);

	//-----------------------------------------------
	// Clean up and exit.
	wprintf(L"Exiting.\n");
	closesocket(ServeSocket);
	WSACleanup();

	//桨距覆盖开关打开
	SetSwapValue(55, 0);
	//扭矩覆盖开关打开
	SetSwapValue(56, 0);
	//torque control 	
	SetSwapValue(47, torque);
	//pitch control
	SetSwapValue(42, pitch1);
	SetSwapValue(43, pitch2);
	SetSwapValue(44, pitch3);
	//平均风速增量--暂时不考虑
	//SetSwapValue(92, speed);
	//9.24与大师兄的代码进行了对比，无bug
}




