/******************************************************************************
*
*    NTU  GIEE   MSOC   Final Project   2016/05
*
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "xtime_l.h"
#include "xparameters.h"	/* SDK generated parameters */
#include "xsdps.h"		/* SD device driver */
#include "xil_printf.h"
#include "ff.h"
#include "xil_cache.h"
#include "xplatform_info.h"

#include "xuartps.h"	// if PS uart is used
#include "xscutimer.h"  // if PS Timer is used
#include "xdmaps.h"		// if PS DMA is used
#include "xscugic.h" 	// if PS GIC is used

#undef XPAR_AXI_BRAM_CTRL_0_BASEADDR
#undef XPAR_AXI_BRAM_CTRL_0_HIGHADDR
#define XPAR_AXI_BRAM_CTRL_0_BASEADDR 0x40000000
#define XPAR_AXI_BRAM_CTRL_0_HIGHADDR 0x40001FFF

#define XPAR_AXI_BRAM_CTRL_1_BASEADDR 0x42000000
#define XPAR_AXI_BRAM_CTRL_1_HIGHADDR 0x42001FFF

//#define BRAM_MEMORY XPAR_AXI_BRAM_CTRL_0_BASEADDR
#define BRAM_MEMORY XPAR_AXI_BRAM_CTRL_1_BASEADDR
#define MED_BASE_ADDR 0x43C00000
#define blk_fld0 0
#define blk_fld1 8
#define blk_fld2 16
#define blk_fld3 0
#define blk_fld4 8
#define blk_fld5 16
#define blk_fld6 0
#define blk_fld7 8
#define blk_fld8 16
#define blk_rdy 31
//#define DENOISE_BASE_ADDR 0x43C00000
//#define SD_MEMORY 0xE0100000
//#define SD_MEMORY 0x00000000

#define RESET_LOOP_COUNT	10	// Number of times to check reset is done
#define LENGTH 8192 // source and destination buffers lengths in number of words
#define DDR_MEMORY XPAR_PS7_DDR_0_S_AXI_BASEADDR+0x00020000 // pass all code and data sections
#define TIMER_DEVICE_ID	XPAR_SCUTIMER_DEVICE_ID
#define TIMER_LOAD_VALUE 0xFFFFFFFF
#define DMA0_ID XPAR_XDMAPS_1_DEVICE_ID
#define INTC_DEVICE_INT_ID XPAR_SCUGIC_SINGLE_DEVICE_ID


volatile static int Done = 0;	/* Dma transfer is done */
volatile static int Error = 0;	/* Dma Bus Error occurs */

XUartPs Uart_PS;		/* Instance of the UART Device */
XScuTimer Timer;		/* Cortex A9 SCU Private Timer Instance */
XDmaPs Dma;				/* PS DMA */
XScuGic Gic;			/* PS GIC */

/************************** Constant Definitions *****************************/
int FfsSdPolledRead(void);
int FfsSdPolledOpen(void);
int FfsSdPolledClose(void);
int FfsSdPolledWrite(void);
u8 mf(u8*);

/************************** Variable Definitions *****************************/
static FIL fil;		/* File object */
static FATFS fatfs;
static char FileName[32] = "STEFAN.Y";
static char *SD_File;
u32 Platform;

#ifdef __ICCARM__
#pragma data_alignment = 32
//u8 InputFrame [352*288];
u8 InputFrameAll [352*288*100];
u8 OutputFrame [352*288*100];
#pragma data_alignment = 4
#else
//u8 InputFrame [352*288] __attribute__ ((aligned(32)));
u8 InputFrameAll [352*288*100] __attribute__ ((aligned(32)));
u8 OutputFrame [352*288*100] __attribute__ ((aligned(32)));
#endif

int getNumber (){

	u8 byte;
	u8 uartBuffer[16];
	u8 validNumber;
	int digitIndex;
	int digit, number, sign;
	int c;

	while(1){
		byte = 0x00;
		digit = 0;
		digitIndex = 0;
		number = 0;
		validNumber = TRUE;

		//get bytes from uart until RETURN is entered
		while(byte != 0x0d){
			while (!XUartPs_IsReceiveData(STDIN_BASEADDRESS));
			byte = XUartPs_ReadReg(STDIN_BASEADDRESS,
								    XUARTPS_FIFO_OFFSET);
			uartBuffer[digitIndex] = byte;
			XUartPs_Send(&Uart_PS, &byte, 1);
			digitIndex++;
		}

		//calculate number from string of digits

		for(c = 0; c < (digitIndex - 1); c++){
			if(c == 0){
				//check if first byte is a "-"
				if(uartBuffer[c] == 0x2D){
					sign = -1;
					digit = 0;
				}
				//check if first byte is a digit
				else if((uartBuffer[c] >> 4) == 0x03){
					sign = 1;
					digit = (uartBuffer[c] & 0x0F);
				}
				else
					validNumber = FALSE;
			}
			else{
				//check byte is a digit
				if((uartBuffer[c] >> 4) == 0x03){
					digit = (uartBuffer[c] & 0x0F);
				}
				else
					validNumber = FALSE;
			}
			number = (number * 10) + digit;
		}
		number *= sign;
		if(validNumber == TRUE){
			print("\r\n");
			return number;
		}
		print("This is not a valid number.\n\r");
	}
}

void DmaDoneHandler(unsigned int Channel,
		    XDmaPs_Cmd *DmaCmd,
		    void *CallbackRef)
{
	/* done handler */
  	Done = 1;
}

void DmaFaultHandler(unsigned int Channel,
		     XDmaPs_Cmd *DmaCmd,
		     void *CallbackRef)
{
	/* fault handler */

	Error = 1;
}

int SetupIntrSystem(XScuGic *GicPtr, XDmaPs *DmaPtr)
{
	int Status;

	Xil_ExceptionInit();

	// Connect the interrupt controller interrupt handler to the hardware
	// interrupt handling logic in the processor.
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
			     (Xil_ExceptionHandler)XScuGic_InterruptHandler,
			     GicPtr);

	// Connect a device driver handler that will be called when an interrupt
	// for the device occurs, the device driver handler performs the specific
	// interrupt processing for the device

	// Connect the Fault ISR
	Status = XScuGic_Connect(GicPtr,
				 XPAR_XDMAPS_0_FAULT_INTR,
				 (Xil_InterruptHandler)XDmaPs_FaultISR,
				 (void *)DmaPtr);
	if (Status != XST_SUCCESS)
		return XST_FAILURE;

	// Connect the Done ISR for channel 0 of DMA 0
	Status = XScuGic_Connect(GicPtr,
				 XPAR_XDMAPS_0_DONE_INTR_0,
				 (Xil_InterruptHandler)XDmaPs_DoneISR_0,
				 (void *)DmaPtr);

	if (Status != XST_SUCCESS)
		return XST_FAILURE;

	// Enable the interrupt for the device
	XScuGic_Enable(GicPtr, XPAR_XDMAPS_0_DONE_INTR_0);

	return XST_SUCCESS;
}

u8 menu(void)
{
	u8 byte;

	print("Enter 1 for Denoise Process\r\n");
	print("Enter 2 for VGA Color Bar\r\n");
	print("Enter 3 for exit\r\n");
	while (!XUartPs_IsReceiveData(STDIN_BASEADDRESS));
	byte = XUartPs_ReadReg(STDIN_BASEADDRESS,
						    XUARTPS_FIFO_OFFSET);
	return(byte);

}

u8 byte_sel(void)
{
	u8 byte;

   	print("Enter number of words you want to transfer:\r\n");
   	print("1=256; 2=512; 3=1024; 4=2048; 5=4096; 6=8192;\r\n");
 	while (!XUartPs_IsReceiveData(STDIN_BASEADDRESS));
	byte = XUartPs_ReadReg(STDIN_BASEADDRESS,
						    XUARTPS_FIFO_OFFSET);
	return(byte);
}

/*****************************************************************************/
/**
*
* Main function to call the SD example.
*
* @param	None
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None
*
******************************************************************************/
int main(void)
{
	//  NOTE! The cache should not be enable!!!
	Xil_DCacheDisable();

	int Status, f, i, j, k, idy, idx, index_y, index_x;
	u8  blk[9];
	u8* bp;
	u8* fp=OutputFrame;
	XTime tCur,tEnd;
	XTime_GetTime(&tCur);

	u8 select;
    int num;
    int InFrameIdx;
    int InFrameIdx3;
    u8 num_in;
    int dma_improvement;
    int wordIdx;
    u8 byteSel;
    u32 * source, * destination;
    int software_cycles, interrupt_cycles;
    int test_done = 0;
    u32 InputFrame32;
	u32 slv_data_status0;
	u32 slv_data_status1;
	u32 slv_data_status2;
	u32 slv_data_status3;
	u32 slv_data_status5;
	u32 rd_data;
	u8 med_blk;

	u8 *InputFrame;

	// UART related definitions
    // int Status;
	XUartPs_Config *Config;

	// PS Timer related definitions
	volatile u32 CntValue1;
	XScuTimer_Config *ConfigPtr;
	XScuTimer *TimerInstancePtr = &Timer;

	xil_printf("SD Polled File System Read \r\n");

	Status = FfsSdPolledOpen();
	//xil_printf("%x \r\n", fil.fptr);
	if (Status != XST_SUCCESS) {
		xil_printf("SD Polled File System Open failed \r\n");
		return XST_FAILURE;
	}

//	xil_printf("Successfully ran SD Polled File System Open \r\n");

	// PS DMA related definitions
	XDmaPs_Config *DmaCfg;
	XDmaPs_Cmd DmaCmd = {
		.ChanCtrl = {
			.SrcBurstSize = 4,
			.SrcBurstLen = 4,
			.SrcInc = 1,		// increment source address
			.DstBurstSize = 4,
			.DstBurstLen = 4,
			.DstInc = 1,		// increment destination address
		},
	};
	unsigned int Channel = 0;

	// PS Interrupt related definitions
	XScuGic_Config *GicConfig;

	// Initialize UART
	// Look up the configuration in the config table, then initialize it.
	Config = XUartPs_LookupConfig(XPAR_XUARTPS_0_DEVICE_ID);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	Status = XUartPs_CfgInitialize(&Uart_PS, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Initialize timer counter
	ConfigPtr = XScuTimer_LookupConfig(TIMER_DEVICE_ID);

	Status = XScuTimer_CfgInitialize(TimerInstancePtr, ConfigPtr,
				 ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Initialize GIC
	GicConfig = XScuGic_LookupConfig(INTC_DEVICE_INT_ID);
	if (NULL == GicConfig) {
		xil_printf("XScuGic_LookupConfig(%d) failed\r\n",
				INTC_DEVICE_INT_ID);
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(&Gic, GicConfig,
				       GicConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		xil_printf("XScuGic_CfgInitialize failed\r\n");
		return XST_FAILURE;
	}

	// Set options for timer/counter 0
	// Load the timer counter register.
	XScuTimer_LoadTimer(TimerInstancePtr, TIMER_LOAD_VALUE);

	// Start the Scu Private Timer device.
	XScuTimer_Start(TimerInstancePtr);

///    print("-- Simple DMA Design Example --\r\n");
	// Get a snapshot of the timer counter value before it's started
	CntValue1 = XScuTimer_GetCounterValue(TimerInstancePtr);

///	xil_printf("Above message printing took %d clock cycles\r\n", TIMER_LOAD_VALUE-CntValue1);

	// Setup DMA Controller
	DmaCfg = XDmaPs_LookupConfig(DMA0_ID);
	if (!DmaCfg) {
		xil_printf("Lookup DMAC %d failed\r\n", DMA0_ID);
		return XST_FAILURE;
	}
	Status = XDmaPs_CfgInitialize(&Dma,DmaCfg,DmaCfg->BaseAddress);

	if (Status) {
		xil_printf("XDmaPs_CfgInitialize failed\r\n");
		return XST_FAILURE;
	}

    // Setup Interrupt system here even we don't use it for the poll-DMA mode
	// as required by the PS DMA driver
	Status = SetupIntrSystem(&Gic, &Dma);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);

		// Initialize src memory
		/// for (i=0; i<num; i++)
		///	*(source+i) = num-i;
		int row_pointer;
//////		DmaCmd.BD.DstAddr = (u32)destination;


	XTime_GetTime(&tCur);
	Status=FfsSdPolledRead();
	if (Status != XST_SUCCESS) {
		xil_printf("SD Polled File System Read failed \r\n");
		return XST_FAILURE;
	}

	for(f=0; f<100;f++)
	{
		xil_printf("Reading frame %d \r\n", f);
		InputFrame = InputFrameAll + 352*288*f;
		for (k=0;k<288;k=k+22) {
			row_pointer = (k == 0) ? 0 : 1;
			num = (k == 286) ? 264 : 1936;
			InFrameIdx = k*352 - (k/22)*352;
			//xil_printf("Sending %d bytes\r\n",num*4);
			//xil_printf("Index %d\r\n",InFrameIdx);
			source = (u32 *)DDR_MEMORY;
			destination = (u32 *)BRAM_MEMORY;
			//print("DDR to BRAM transfer\r\n");
			DmaCmd.BD.SrcAddr = (u32)&InputFrame[InFrameIdx];
			DmaCmd.BD.DstAddr = (u32)destination;
			DmaCmd.BD.Length = num * sizeof(int);
			//print("Setting up interrupt system\r\n");
			Status = SetupIntrSystem(&Gic, &Dma);
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}

			Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);

			XDmaPs_SetDoneHandler(&Dma,0,DmaDoneHandler,0);
			Status = XDmaPs_Start(&Dma, Channel, &DmaCmd, 0);	// release DMA buffer as we are done
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			// reset timer
		//////		XScuTimer_RestartTimer(TimerInstancePtr);

			// Disable the interrupt for the device
			XScuGic_Disable(&Gic, XPAR_XDMAPS_0_DONE_INTR_0);

			
			for(i=row_pointer;i<22;++i) {
				for(j=0;j<352;++j){
					bp=blk;
					for(idy=-1;idy<=1;++idy) {
						for(idx=-1;idx<=1;++idx){
							index_y=(i+idy <0)?0:(i+idy>287)?287:i+idy;
							index_x=(j+idx <0)?0:(j+idx>351)?351:j+idx;
							wordIdx = (index_y*352+index_x) >> 2;
							byteSel = (index_y*352+index_x) % 4;
							*bp++ = destination[wordIdx] >> byteSel*8;
						//	xil_printf("wordIdx = %d \r\n",wordIdx);
						}
					}
					slv_data_status2 = 0x00000000;
					*(source+2) = slv_data_status2;
					slv_data_status0 = slv_data_status0 | blk[0] << blk_fld0 | blk[1] << blk_fld1 | blk[2] << blk_fld2;
					*(source+0) = slv_data_status0;
					slv_data_status1 = slv_data_status1 | blk[3] << blk_fld3 | blk[4] << blk_fld4 | blk[5] << blk_fld5;
					*(source+1) = slv_data_status1;
					slv_data_status2 = slv_data_status2 | blk[6] << blk_fld6 | blk[7] << blk_fld7 | blk[8] << blk_fld8 | 0x1 << blk_rdy;
					*(source+2) = slv_data_status2;
					med_blk = source[5];

				//*fp++=mf(blk);
			   }
			}
		}
		
		// CPU version 
		/* for(i=0;i<288;++i)
			for(j=0;j<352;++j){
				bp=blk;
				for(idy=-1;idy<=1;++idy)
					for(idx=-1;idx<=1;++idx){
						index_y=(i+idy <0)?0:(i+idy>287)?287:i+idy;
						index_x=(j+idx <0)?0:(j+idx>351)?351:j+idx;
						*bp++=InputFrame[index_y*352+index_x];
					}
				*fp++=mf(blk);
		} */
	}

	XTime_GetTime(&tEnd);
	float FPS = 10000.0/(tEnd-tCur)*COUNTS_PER_SECOND;
	xil_printf("FPS = %d/100 \r\n",(int)FPS);



	FfsSdPolledClose();
	if (Status != XST_SUCCESS) {
			xil_printf("SD Polled File System Close failed \r\n");
			return XST_FAILURE;
		}
	Status=FfsSdPolledWrite();
	if(Status!= XST_SUCCESS){
		xil_printf("Successfully ran SD Polled File System failed \r\n");
		return XST_FAILURE;
	}
	xil_printf("Successfully ran SD Polled File System Write \r\n");
	return XST_SUCCESS;

}

/*****************************************************************************/
/**
*
* File system example using SD driver to write to and read from an SD card
* in polled mode. This example creates a new file on an
* SD card (which is previously formatted with FATFS), write data to the file
* and reads the same data back to verify.
*
* @param	None
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None
*
******************************************************************************/

int FfsSdPolledWrite(void)
{
	FRESULT Res;
	UINT NumBytesRead;
	u32 FileSize = 352*288*100;
	//TCHAR *Path = "0:/";

	Platform = XGetPlatform_Info();

	/*
	 * Register volume work area, initialize device
	 */
	//Res = f_mount(&fatfs, Path, 0);

	/*if (Res != FR_OK) {
		return XST_FAILURE;
	}*/

	/*
	 * Open file with required permissions.
	 * Here - Creating new file with read/write permissions. .
	 * To open file with write permissions, file system should not
	 * be in Read Only mode.
	 */

	Res = f_open(&fil, "DENOISE.Y",  FA_CREATE_ALWAYS | FA_WRITE);
	if (Res) {
		return XST_FAILURE;
	}

	Res = f_lseek(&fil, 0);
	if (Res) {
		return XST_FAILURE;
	}

	Res = f_write(&fil, (void*)OutputFrame, FileSize,
				&NumBytesRead);
	if (Res) {
		return XST_FAILURE;
	}

	/*
	 * Pointer to beginning of file .
	 */
	Res=f_close(&fil);
	if (Res) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}


int FfsSdPolledOpen(void){
	FRESULT Res;

	TCHAR *Path = "0:/";

	Platform = XGetPlatform_Info();


	/*
	 * Register volume work area, initialize device
	 */
	Res = f_mount(&fatfs, Path, 0);

	if (Res != FR_OK) {
		return XST_FAILURE;
	}

	/*
	 * Open file with required permissions.
	 * Here - Creating new file with read/write permissions. .
	 * To open file with write permissions, file system should not
	 * be in Read Only mode.
	 */

	SD_File = (char *)FileName;

	Res = f_open(&fil, SD_File,  FA_OPEN_EXISTING|FA_READ);
	if (Res) {
		return XST_FAILURE;
	}

	Res = f_lseek(&fil, 0);
	if (Res) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int FfsSdPolledClose(void){
	FRESULT Res;
	Res=f_close(&fil);
		if (Res) {
				return XST_FAILURE;
			}
	return XST_SUCCESS;

}


int FfsSdPolledRead(void)
{
	FRESULT Res;
	UINT NumBytesRead;
	u32 FileSize = 352*288*100;

	Res = f_read(&fil, (void*)InputFrameAll, FileSize,
				&NumBytesRead);
		if (Res) {
			return XST_FAILURE;
		}

	/*
	 * Pointer to beginning of file .
	 */
	return XST_SUCCESS;
}


u8 mf(u8* blk){
	// 3x3 median filter by bubble sort
	int i, j , Temp,sp;
    for (i=8; i>0; i--)
       {
        sp=1;
       for (j =0; j <=i; j++)
          if (blk[j] > blk[j+1])
             {
               Temp = blk[j];
               blk[j] = blk[j+1];
               blk[j+1] = Temp;
               sp=0;
             }
             if (sp==1) break;
       }
///    xil_printf("%x \r\n",blk[4]);
	return blk[4];
}
