// ExControl.cpp : ���� DLL Ӧ�ó���ĵ���������
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
	//������Ʊ���:ת�ء���������ǡ�һ��������other
	double torque, pitch1, pitch2, pitch3;// other//speed
	SwapArray = avrSwap;	//Store the pointer
							//Make sure there's a C string terminator
	accInfile[NINT(avrSwap[49])] = '\0';
	avcOutname[NINT(avrSwap[50])] = '\0';
	avcMsg[0] = '\0';
	iStatus = NINT(avrSwap[0]);


	//TCPͨѶ
	int iResult = 0;
	WSADATA wsaData;

	SOCKET ServeSocket;  // ����˶˿�������
	SOCKET ClientSocket; // �ͻ��˶˿�������
	sockaddr_in ServeAddr; // ����˶˿ڵ�ַ
	sockaddr_in ClientAddr;// �ͻ��˶˿ڵ�ַ

	unsigned short Port = 1102;// ����˶˿ں�

	char  ControlBuf[1024]; // ���simulink���͹��������� 
	char  WindBuf[1024];  // ��ŷ���simulink�ķ������ 

	memset(WindBuf, '\0', sizeof(WindBuf));//��ʼ��

	int ServeAddrSize = sizeof(ServeAddr);// ��Ŷ˿ڵ�ַ����
	int ClientAddrSize = sizeof(ClientAddr);

	//-----------------------------------------------
	// Initialize Winsock ��ʼ��
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		wprintf(L"WSAStartup failed with error %d\n", iResult);
	}

	//-----------------------------------------------
	// Create a receiver socket to receive datagrams �����׽���
	ServeSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (ServeSocket == INVALID_SOCKET)
	{
		wprintf(L"socket failed with error %d\n", WSAGetLastError());

	}
	//-----------------------------------------------
	// Bind the socket to any address and the specified port.
	//blind֮ǰҪ��const struct sockaddr FAR* �еĽṹ������ʼ��
	ServeAddr.sin_family = AF_INET;
	ServeAddr.sin_port = htons(Port);
	ServeAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	//���������Ϣ��ֵ��������׽���
	iResult = bind(ServeSocket, (SOCKADDR *)& ServeAddr, ServeAddrSize);
	if (iResult != 0)
	{
		wprintf(L"bind failed with error %d\n", WSAGetLastError());
	}

	//��ʼ����
	if (listen(ServeSocket, 10) == SOCKET_ERROR)
	{
		printf("����ʧ��:%d", WSAGetLastError());
	}

	//�ȴ��ͻ�������  		

	SOCKET	sockConn = accept(ServeSocket, (SOCKADDR *)& ClientAddr, &ClientAddrSize);
	//������ֱ�������� sockConnΪ�½�������ͻ��˵��׽���������
	if (sockConn == SOCKET_ERROR) {
		printf("�ȴ�����ʧ��:%d", WSAGetLastError());
	}
	printf("�ͻ��˵�IP��:[%s]\n", inet_ntoa(ClientAddr.sin_addr));


	//-----------------------------------------------
	//����������Ϣ�ŵ�WindBuf��

	//float information[5];
	float information[8];

	//ת�ؿ������
	information[0] = GetSwapValue(20); //����ٶ�:rad/s
	information[1] = GetSwapValue(16);//����ģ̬����:Nm/(rad/s)^2
	information[2] = GetSwapValue(23);//���Ť��:Mm

	//���ʿ�����ز���
	information[3] = GetSwapValue(15);//�������:W

	//��ʱ�ò����ı���
	
	information[4] = GetSwapValue(30);//Ҷ�����1
	information[5] = GetSwapValue(31);//Ҷ�����2
	information[6] = GetSwapValue(32);//Ҷ�����3
	information[7] = GetSwapValue(60); //ҶƬ1��λ��


	int ValidateBufLen = 0;
	//���õ�ת�ؿ��ƺ͹��ʿ��������ģ�Ͳ���
	for (int i = 0; i<8; i++)
	{
		ValidateBufLen = strlen(WindBuf);
		sprintf(WindBuf + ValidateBufLen, "%g;", information[i]);
	}

	//��������MyFunc�������Ŀ����ź�
	memset(ControlBuf, '\0', sizeof(ControlBuf));//�����ʼ��
	int iRecv = recv(sockConn, ControlBuf, sizeof(ControlBuf), 0); //������ֱ���յ���Ϣ			
	printf("recv dada: %s \n", ControlBuf);

	//��ControlBuf��ȡ�����ݷ�����ʱ����
	sscanf(ControlBuf, "%lf;%lf;%lf;%lf;", &torque, &pitch1, &pitch2, &pitch3); //���յ�������
	//sscanf(ControlBuf, "%g;%g;%g;%g;", &torque, &pitch1, &pitch2, &pitch3);
																							//��ģ�Ͳ������͸�MyFunc								 
	int iSend = send(sockConn, WindBuf, sizeof(WindBuf), 0);

	// ���� û���ͳɹ���һֱ����
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

	//���า�ǿ��ش�
	SetSwapValue(55, 0);
	//Ť�ظ��ǿ��ش�
	SetSwapValue(56, 0);
	//torque control 	
	SetSwapValue(47, torque);
	//pitch control
	SetSwapValue(42, pitch1);
	SetSwapValue(43, pitch2);
	SetSwapValue(44, pitch3);
	//ƽ����������--��ʱ������
	//SetSwapValue(92, speed);
	//9.24���ʦ�ֵĴ�������˶Աȣ���bug
}




