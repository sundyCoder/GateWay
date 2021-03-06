
#include <stdlib.h>
#include <iostream>
#include <queue>
#include <ctime>
#include "../inc/SerialClass.h"
#include "../inc/Robot.h"
using namespace std;

Serial::Serial(char *portName)
{
    //We're not yet connected
    this->connected = false;

    //Try to connect to the given port throuh CreateFile
    this->hSerial = CreateFileA(portName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

    //Check if the connection was successfull
    if(this->hSerial==INVALID_HANDLE_VALUE)
    {
        //If not success full display an Error
        if(GetLastError()==ERROR_FILE_NOT_FOUND){

            //Print Error if neccessary
            printf("ERROR: Handle was not attached. Reason: %s not available.\n", portName);

        }
        else
        {
            printf("Open Serial ERROR!!!");
        }
    }
    else
    {
        //If connected we try to set the comm parameters
        DCB dcbSerialParams = {0};

        //Try to get the current
        if (!GetCommState(this->hSerial, &dcbSerialParams))
        {
            //If impossible, show an error
            printf("failed to get current serial parameters!");
        }
        else
        {
            //Define serial connection parameters for the dMEMs board
            dcbSerialParams.BaudRate=CBR_115200;
            dcbSerialParams.ByteSize=8;
            dcbSerialParams.StopBits=ONESTOPBIT;
            dcbSerialParams.Parity=NOPARITY;
            //Setting the DTR to Control_Enable ensures that the dMEMs is properly
            //reset upon establishing a connection
            dcbSerialParams.fDtrControl = DTR_CONTROL_DISABLE;

             //Set the parameters and check for their proper application
             if(!SetCommState(hSerial, &dcbSerialParams))
             {
                printf("ALERT: Could not set Serial Port parameters");
             }
             else
             {
                 //If everything went fine we're connected
                 this->connected = true;
                 //Flush any remaining characters in the buffers
                 PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
                 //We wait 2s as the dMEMs board will be reseting
                 //Sleep(DMEMS_WAIT_TIME); //modified
             }
        }
    }

}

Serial::~Serial()
{
    //Check if we are connected before trying to disconnect
    if(this->connected)
    {
        //We're no longer connected
        this->connected = false;
        //Close the serial handler
        CloseHandle(this->hSerial);
    }
}

int Serial::ReadData(char *buffer, unsigned int nbChar)
{
    //Number of bytes we'll have read
    DWORD bytesRead;
    //Number of bytes we'll really ask to read
    unsigned int toRead;

    //Use the ClearCommError function to get status info on the Serial port
    ClearCommError(this->hSerial, &this->errors, &this->status);

    //Check if there is something to read
    if(this->status.cbInQue>0)
    {
        //If there is we check if there is enough data to read the required number
        //of characters, if not we'll read only the available characters to prevent
        //locking of the application.
        if(this->status.cbInQue>nbChar)
        {
            toRead = nbChar;
        }
        else
        {
            toRead = this->status.cbInQue;
        }

        //Try to read the require number of chars, and return the number of read bytes on success
        if(ReadFile(this->hSerial, buffer, toRead, &bytesRead, NULL) )
        {
            return bytesRead;
        }

    }

    //If nothing has been read, or that an error was detected return 0
    return 0;

}


bool Serial::WriteData(char *buffer, unsigned int nbChar)
{
    DWORD bytesSend;

    //Try to write the buffer on the Serial port
    if(!WriteFile(this->hSerial, (void *)buffer, nbChar, &bytesSend, 0))
    {
        //In case it don't work get comm error and return false
        ClearCommError(this->hSerial, &this->errors, &this->status);

        return false;
    }
    else
        return true;
}

bool Serial::IsConnected()
{
    //Simply return the connection status
    return this->connected;
}

rbNode bCastInfo[NROBOT];
dataPack dataInfo[NROBOT];

void SerialTest(int com_id){
	printf("########Gateway for Multi-robot System########\n");

    char comm[64];
    sprintf_s(comm, "\\\\.\\COM%d", com_id);
	Serial* SP = new Serial(comm);    // adjust as needed

	if (SP->IsConnected())
		fprintf(stdout, "COM%d is connected.\n", com_id);

    std::queue<char> dataBuff;
	unsigned char incomingData[512] = "";		// don't forget to pre-allocate memory	
	int dataLength = 37;
	int readResult = 0;
	char filename[100];
	sprintf_s(filename, "%d.log", time(0));
	FILE *fp = fopen(filename, "w");
    if(fp == NULL){
        printf("Open Filed!\n");
    }
	rbNode bInfo;
	
	while(SP->IsConnected())
	{
		readResult = SP->ReadData((char *)incomingData,dataLength);
        //printf("Bytes read: (0 means no data available) %i\n",readResult);
        incomingData[readResult] = 0;
		for (int i = 0; i < readResult; ++i) {
			dataBuff.push(incomingData[i]);
		}
		int sz = 37;
		if (dataBuff.size() >= sz) {
			int k = 0;
			for (int i = 0; i < sz; ++i) {
				//printf("%02X ", dataBuff.front());
				incomingData[k++] = dataBuff.front();
				dataBuff.pop();
			}
			//puts("");
/*##########################Received Data###############################*/
			memcpy(&bInfo, incomingData+4, sizeof bInfo);
			bCastInfo[bInfo.nodeID - 1] = bInfo;
			//if ((int)(bInfo->nodeID) == 8)
				DispPackInfo(bInfo.nodeID, fp);
			fflush(fp);
/*##########################Received Data###############################*/
		}
	}
}


