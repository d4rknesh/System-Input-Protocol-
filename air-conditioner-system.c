#include <stdio.h>

int Decoder(int *RX_Data,
            void (*powerOn)(),
            void (*powerOff)(),
            int (*checkPacket)(int*, int), 
            int* temperature, 
            int *sleepTimer);

void powerOn();
void powerOff();
int checkPacket(int *RX_Data, int size);



int main() {
   
    int RX_Data[] = {0x4A, 0x0F, 0x33, 0x44 , 0x1a , 0x22 ,0x33 , 0x01 , 0x44 , 0x1b , 0x55 , 0x01 , 0xaa ,0x44 ,0x1D, 0x22  , 0xd9} ;
    
    int Temperature = 0 ;
    int sleepTimer = 0 ;
    Decoder(RX_Data, powerOn, powerOff, checkPacket, &Temperature, &sleepTimer);
    
    return 1;
}

int Decoder(int *RX_Data,
            void (*powerOn)(),
            void (*powerOff)(),
            int (*checkPacket)(int*, int), 
            int *temperature, 
            int *sleepTimer) 
{
    
    if (RX_Data[0] != 0x3A) {
        printf("\n  !! Invalid Packet Header !!\n");
        return 0;
    }else 
    {
        printf("\n    ~ Header is Valid ~ --> byte[0] = 0x3A\n");
    }

    int dataPacketSize = RX_Data[1];
    printf("    ~ Data size: %d ~ --> byte[1] = 0x%x \n\n", dataPacketSize , RX_Data[1]);



    int checkFlag = checkPacket(RX_Data, dataPacketSize);
    if (!checkFlag) {
        printf("\n!! Invalid Packet !!\n");
        return 0;
    }


    int flag = 0 ;
    for (int i = 2; i <= dataPacketSize ; i++)
    {
        // printf("iterate on index %d \n" , i) ;
        if (flag > 0 ) {
            // printf("    pass index %d\n\n" , i ) ;
            flag -- ;
            continue; 
        }
        switch (RX_Data[i]) {
            case 0x22: 
                // printf("\n  ----PowerOff ------ :RX_Data[%d] --> 0x%x", i , RX_Data[i] );
                powerOff();
                break;
            case 0x33: 
                // printf("\n  ----PowerOn ------  :RX_Data[%d] --> 0x%x \n", i , RX_Data[i] );
                powerOn();
            break;
            case 0x44: 
                // printf("\n  ----Set Temp ------ :RX_Data[%d] --> 0x%x ", i , RX_Data[i] );
                *temperature = RX_Data[i];
                printf("\n-Temperature Set to: %d\n\n", RX_Data[i+1]);
                flag = 1 ;
                break;
            case 0x55: 
                // printf("\n  ----Set Timer ------:RX_Data[%d] --> 0x%x ", i , RX_Data[i] );
                *sleepTimer = (RX_Data[i+1] << 8) + RX_Data[i+2];
                printf("\n-Sleep Timer Set to: %d\n\n", *sleepTimer);
                flag = 2 ;
                break;
            default:
                printf("\n  !! Unknown Command !!\n");
                break;
        }
    }
    
    return 1;
}

void powerOn() {
    printf("~ System Powered On ~\n\n");
}

void powerOff() {
    printf("\n~ System Powered Off ~\n\n");
}


int checkPacket(int *RX_Data, int size) {
    
    int LSB = RX_Data[size + 1] ;                                           // printf("\nlsb is 0x%x\n" , LSB) ;   
    int sumVal = 0;
    for (int i = 1; i <= size; i++)
    {
                                                                        // printf("\nRX_Data[%d] = 0x%x " , i , RX_Data[i]) ;
        sumVal = sumVal + RX_Data[i] ;  
    }                                                                   // printf("\nSumVal = %d = 0x%x " ,  sumVal , sumVal) ;
    sumVal = sumVal & 0xFF ;
                                                                        // printf("\nSumVal = %d = 0x%x " ,  sumVal , sumVal) ;
    
    if (sumVal == LSB)
    {
        printf("\n No Packet Loss\n\n") ;
        return 1 ;
    }
    else  {
        printf("\n Noise Detected \n") ;
        return 0 ;
    }

}
