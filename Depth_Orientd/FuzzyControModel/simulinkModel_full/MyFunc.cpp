/* Copyright 2003-2014 The MathWorks, Inc. */

// *******************************************************************
// **** To build this mex function use: mex MyFunc.cpp ****
// *******************************************************************

#include "MyFunc.h"

#define S_FUNCTION_LEVEL 2
#define S_FUNCTION_NAME  MyFunc

// Need to include simstruc.h for the definition of the SimStruct and
// its associated macro definitions.
#include "simstruc.h"
#include "mex.h"//新增

#include <winsock2.h>//新增
#include <Ws2tcpip.h>//新增
#include <stdio.h>//新增

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")//新增

#define IS_PARAM_DOUBLE(pVal) (mxIsNumeric(pVal) && !mxIsLogical(pVal) &&\
!mxIsEmpty(pVal) && !mxIsSparse(pVal) && !mxIsComplex(pVal) && mxIsDouble(pVal))

// Function: mdlInitializeSizes ===============================================
// Abstract:
//    The sizes information is used by Simulink to determine the S-function
//    block's characteristics (number of inputs, outputs, states, etc.).
static void mdlInitializeSizes(SimStruct *S)
{
    // No expected parameters
    ssSetNumSFcnParams(S, 0);

    // Parameter mismatch will be reported by Simulink
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        return;
    }

    // Specify I/O
    if (!ssSetNumInputPorts(S, 1)) return;
    //ssSetInputPortWidth(S, 0, DYNAMICALLY_SIZED);
    ssSetInputPortWidth(S, 0, 4);
    ssSetInputPortDirectFeedThrough(S, 0, 1);
    if (!ssSetNumOutputPorts(S,1)) return;
    //ssSetOutputPortWidth(S, 0, DYNAMICALLY_SIZED);
    ssSetOutputPortWidth(S, 0, 8);
    ssSetNumSampleTimes(S, 1);

    // Reserve place for C++ object
    //ssSetNumPWork(S, 1);原始文件
	ssSetNumPWork(S, 3);//两个版本不同，所以优先使用这个试试

    //ssSetSimStateCompliance(S, USE_CUSTOM_SIM_STATE);原始文件中带的

    //ssSetOptions(S,
    //             SS_OPTION_WORKS_WITH_CODE_REUSE |
    //             SS_OPTION_EXCEPTION_FREE_CODE |
    //             SS_OPTION_DISALLOW_CONSTANT_SAMPLE_TIME);原始文件版本
	ssSetOptions(S,
		SS_OPTION_WORKS_WITH_CODE_REUSE |
		SS_OPTION_EXCEPTION_FREE_CODE);//网上版本

}


//不用动，按照原始文件就好
// Function: mdlInitializeSampleTimes =========================================
// Abstract:
//   This function is used to specify the sample time(s) for your
//   S-function. You must register the same number of sample times as
//   specified in ssSetNumSampleTimes.
static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0.0);
    ssSetModelReferenceSampleTimeDefaultInheritance(S); 
}

//不用动
// Function: mdlStart =======================================================
// Abstract:
//   This function is called once at start of model execution. If you
//   have states that should be initialized once, this is the place
//   to do it.
#define MDL_START
static void mdlStart(SimStruct *S)
{
    // Store new C++ object in the pointers vector
    DoubleAdder *da  = new DoubleAdder();
    ssGetPWork(S)[0] = da;
}

// Function: mdlOutputs =======================================================
// Abstract:
//   In this function, you compute the outputs of your S-function
//   block.
static void mdlOutputs(SimStruct *S, int_T tid)
{
    // Retrieve C++ object from the pointers vector
    DoubleAdder *da = static_cast<DoubleAdder *>(ssGetPWork(S)[0]);
    
    // Get data addresses of I/O
			   InputRealPtrsType  u = ssGetInputPortRealSignalPtrs(S,0);
               real_T *y = ssGetOutputPortRealSignal(S, 0);

			   int InputNum = ssGetInputPortWidth(S, 0);//新增的

	//开始TCP通讯
    //初始化
	int iResult;
	WSADATA wsaData;
    
	SOCKET ServeSocket;  // 服务端端口名
	SOCKET ClientSocket; // 客户端端口名
	sockaddr_in ServeAddr; // 服务端端口地址
	sockaddr_in ClientAddr;// 客户端端口地址
	unsigned short Port = 1102;// 服务端端口号;//后期可能更改

	char  ControlBuf[1024]; // 存放simulink发送过去的数据 
	char  WindBuf[1024];  // 存放来自Bladed的风机数据 
	memset(WindBuf, '\0', sizeof(WindBuf));//初始化
	memset(ControlBuf, '\0', sizeof(ControlBuf));

	int ServeAddrSize = sizeof(ServeAddr);// 存放端口地址长度
	int ClientAddrSize = sizeof(ClientAddr);

	//----------------------
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return;
	}

	//服务端信息
	ServeAddr.sin_family = AF_INET;
	ServeAddr.sin_port = htons(Port);
	ServeAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//创建客户端套接字
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	//发送信息处理(来自simulink的控制信号)
	
	int ValidateBufLen = 0;
	for (int i = 0;i<InputNum;i++)
	{
		ValidateBufLen = strlen(ControlBuf);
		sprintf(ControlBuf+ValidateBufLen, "%g;", *u[i]);

	}
	//sprintf(ControlBuf + strlen(ControlBuf), "%d;", InputNum);用来记录输入个数的

	//向服务段发送连接请求
	if (connect(ClientSocket, (struct  sockaddr*)&ServeAddr, ServeAddrSize) == INVALID_SOCKET) {
		printf("连接失败:%d", WSAGetLastError());
	}
	//信息交互
	int iSend = send(ClientSocket, ControlBuf, sizeof(ControlBuf), 0);

	//接收
	int iRecv = recv(ClientSocket, WindBuf, sizeof(WindBuf), 0);


	//float fl[4];//分别对应电机转速、模态增益、电机扭矩、电机功率
	float fl[8];//分别对应电机转速、模态增益、电机扭矩、电机功率、叶根弯矩1、叶根弯矩2、叶根弯矩3、方位角

	sscanf(WindBuf, "%f;%f;%f;%f;%f;%f;%f;%f;", &fl[0], &fl[1], &fl[2], &fl[3], &fl[4], &fl[5], &fl[6], &fl[7]);

	for (int i = 0;i<8;i++)
	{
		y[i] = fl[i];
	}
	closesocket(ClientSocket);
	WSACleanup();
	Sleep(5);//建议休眠5ms，防止通讯报错
}

//不动
// Function: mdlTerminate =====================================================
// Abstract:
//   In this function, you should perform any actions that are necessary
//   at the termination of a simulation.  For example, if memory was
//   allocated in mdlStart, this is the place to free it.
static void mdlTerminate(SimStruct *S)
{
    // Retrieve and destroy C++ object
    DoubleAdder *da = static_cast<DoubleAdder *>(ssGetPWork(S)[0]);
    delete da;
}

// Required S-function trailer
#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
