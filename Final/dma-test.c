/******************************************************************************
*    NTU  GIEE   MSOC   Final Project   2016/05
******************************************************************************/

#include "xtime_l.h"
#include "xparameters.h"	// SDK generated parameters
#include "xsdps.h"			// SD device driver
#include "xil_printf.h"
#include "ff.h"
#include "xil_cache.h"
#include "xplatform_info.h"
#include "xuartps.h"	// if PS uart is used
#include "xscutimer.h"  // if PS Timer is used
#include "xdmaps.h"		// if PS DMA is used
#include "xscugic.h" 	// if PS GIC is used

//#undef XPAR_AXI_BRAM_CTRL_0_BASEADDR
//#undef XPAR_AXI_BRAM_CTRL_0_HIGHADDR
#define XPAR_AXI_BRAM_CTRL_0_BASEADDR 0x40000000
#define XPAR_AXI_BRAM_CTRL_0_HIGHADDR 0x400FFFFF

#define XPAR_AXI_BRAM_CTRL_1_BASEADDR 0x42000000
#define XPAR_AXI_BRAM_CTRL_1_HIGHADDR 0x420FFFFF

#define BRAM_MEMORY_0 XPAR_AXI_BRAM_CTRL_0_BASEADDR
#define BRAM_MEMORY_1 XPAR_AXI_BRAM_CTRL_1_BASEADDR
#define DENOISE_BASE_ADDR 0x43C00000
//#define SD_MEMORY 0xE0100000
//#define SD_MEMORY 0x00000000

#define RESET_LOOP_COUNT	10	// Number of times to check reset is done
#define LENGTH 8192 			// source and destination buffers lengths in number of words
#define DDR_MEMORY XPAR_PS7_DDR_0_S_AXI_BASEADDR+0x00020000 // pass all code and data sections
#define TIMER_DEVICE_ID	XPAR_SCUTIMER_DEVICE_ID
#define TIMER_LOAD_VALUE 0xFFFFFFFF
#define DMA0_ID XPAR_XDMAPS_1_DEVICE_ID
#define INTC_DEVICE_INT_ID XPAR_SCUGIC_SINGLE_DEVICE_ID

/************************** Functions *****************************/
// File operations
int FfsSdPolledRead(void);
int FfsSdPolledOpen(void);
int FfsSdPolledClose(void);
int FfsSdPolledWrite(void);
// DMA & interrupt handling
void DmaDoneHandler(unsigned int Channel, XDmaPs_Cmd *DmaCmd, void *CallbackRef);
void DmaFaultHandler(unsigned int Channel, XDmaPs_Cmd *DmaCmd, void *CallbackRef);
int SetupIntrSystem(XScuGic *GicPtr, XDmaPs *DmaPtr);
// Interactive functions
u8 menu(void);

/************************** Global Variables *****************************/
static FIL fil;		/* File object */
static FATFS fatfs;
static char FileName[32] = "STEFAN.Y";
static char *SD_File;
u32 Platform;

volatile static int Done = 0;	/* Dma transfer is done */
volatile static int Error = 0;	/* Dma Bus Error occurs */

XUartPs Uart_PS;		/* Instance of the UART Device */
XScuTimer Timer;		/* Cortex A9 SCU Private Timer Instance */
XDmaPs Dma;				/* PS DMA */
XScuGic Gic;			/* PS GIC */

#ifdef __ICCARM__
#pragma data_alignment = 32
u8 InputFrame [352*288];
u8 OutputFrame [352*288*100];
#pragma data_alignment = 4
#else
u8 InputFrame [352*288] __attribute__ ((aligned(32)));
u8 OutputFrame [352*288*100] __attribute__ ((aligned(32)));
#endif

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

	/****************************** Variables ***********************************/
	int Status, f, i, j, idy, idx, index_y, index_x;
	u8  blk[9];
	u8* bp;
	u8* fp=OutputFrame;
	XTime tCur,tEnd;

	u8 select;
    int num;
    u8 num_in;
    int dma_improvement;

    u32 * source, * destination;
    int software_cycles, interrupt_cycles;
    int test_done = 0;
    u32 InputFrame32;

	/*****************************************************************************/


	/****************************** Configurations ***********************************/

	XUartPs_Config *Config;		// UART related definitions
	XScuGic_Config *GicConfig;	// PS Interrupt related definitions
	XDmaPs_Config *DmaCfg;		// PS DMA related definitions
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
	// PS Timer related definitions
	volatile u32 CntValue1;
	XScuTimer_Config *ConfigPtr;
	XScuTimer *TimerInstancePtr = &Timer;


	// Initialize UART
	Config = XUartPs_LookupConfig(XPAR_XUARTPS_0_DEVICE_ID);
	if (NULL == Config) {
		return XST_FAILURE;
	}
	Status = XUartPs_CfgInitialize(&Uart_PS, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	/* UART DONE */

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
	/* GIC DONE */

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
	/* DMA DONE */

	// Initialize timer counter
	ConfigPtr = XScuTimer_LookupConfig(TIMER_DEVICE_ID);

	Status = XScuTimer_CfgInitialize(TimerInstancePtr, ConfigPtr,
				 ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	// Set options for timer/counter 0
	// Load the timer counter register.
	XScuTimer_LoadTimer(TimerInstancePtr, TIMER_LOAD_VALUE);
	/* Time-ciounter DONE */


	// Setup Interrupt system here even we don't use it for the poll-DMA mode
	// as required by the PS DMA driver
	Status = SetupIntrSystem(&Gic, &Dma);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Open SD card file
	Status = FfsSdPolledOpen();
	if (Status != XST_SUCCESS) {
		xil_printf("SD Polled File System Open failed \r\n");
		return XST_FAILURE;
	}




	unsigned int Channel = 0;





	// Start the Scu Private Timer device.
	XScuTimer_Start(TimerInstancePtr);

	// Get a snapshot of the timer counter value before it's started
	CntValue1 = XScuTimer_GetCounterValue(TimerInstancePtr);


	/*************************** configuration done *********************************/


    /*****************************************************************************/

//	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);
    test_done = 0;
    num = 256;
    int count = 0;
	destination = (u32 *)BRAM_MEMORY_1;

	XTime_GetTime(&tCur);
	while(test_done==0)
    {
		if(test_done)
			break;

		// Read a frame from SD card into InputFrame
		for(f=0;f<1;++f){
			Status=FfsSdPolledRead();
			if (Status != XST_SUCCESS) {
				xil_printf("SD Polled File System Read failed \r\n");
				return XST_FAILURE;
			}
		}


		// DMA in polling mode
		DmaCmd.BD.SrcAddr = (u32)&InputFrame[0];
		DmaCmd.BD.DstAddr = (u32)destination;
		DmaCmd.BD.Length = num*sizeof(u8);
		
		print("Setting up interrupt system\r\n");
		Status = SetupIntrSystem(&Gic, &Dma);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		
		xil_printf("Exception mask \r\n");
		Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);

		XDmaPs_SetDoneHandler(&Dma,0,DmaDoneHandler,0);
		xil_printf("Start DMA \r\n");
		// Start DMA
		Status = XDmaPs_Start(&Dma, Channel, &DmaCmd, 0);	// release DMA buffer as we are done

		// reset timer
		XScuTimer_RestartTimer(TimerInstancePtr);

		// Wait for DMA to be done
		while ((Done==0) & (Error==0));
		if (Error)
			print("Error occurred during DMA transfer\r\n");

		CntValue1 = XScuTimer_GetCounterValue(TimerInstancePtr);

		/* Wait for median filter to initiate another DMA */

		xil_printf("Moving data through DMA in Interrupt mode took %d clock cycles\r\n", TIMER_LOAD_VALUE-CntValue1);
		interrupt_cycles = TIMER_LOAD_VALUE - CntValue1;

		xil_printf("Transfer complete\r\n");
		// Disable the interrupt for the device
		XScuGic_Disable(&Gic, XPAR_XDMAPS_0_DONE_INTR_0);


		/* Verification */

		for (i = 0; i < num/4; i++) {
			InputFrame32 = InputFrame[i*4+3]<<24 | InputFrame[i*4+2]<<16 | InputFrame[i*4+1]<<8 | InputFrame[i*4];
			//xil_printf("InputFrame32 : %x %x \r\n", InputFrame32,destination[i]);
			if ( destination[i] != InputFrame32) {
				xil_printf("Data match failed at = %d, source data = %d, destination data = %d\n\r",i,InputFrame32,destination[i]);
				print("-- Exiting main() --");
				return XST_FAILURE;
			}
		}
		xil_printf("Transfered data verified\r\n");

		dma_improvement = software_cycles/interrupt_cycles;
		/*xil_printf("Improvement using Interrupt DMA = %2d", dma_improvement);
		xil_printf("x improvement \r\n");
		xil_printf("------------------------------------------------------------------- \r\n\r\n");*/
		Error = 0;
		Done = 0;
		source = (u32 *)DENOISE_BASE_ADDR;
  	    u32 slv_data_status;
  	    u8 med_blk;
  	    slv_data_status = 0x00000001;
  	    *(source+0) = slv_data_status;
  	    //xil_printf("source data = %x\n\r",source[0]);
  	    //test_done = 1;
  	    //xil_printf("source data = %x\n\r",source);
  	    /*if(count >= 88*72/4)
  	    {
  	    	//xil_printf("source data = %x\n\r",source);
  	    	test_done = 1;
  	    }
  	    else
  	    {
  	    	source += 1;
			destination += 1;
			count += 1;
			test_done = 0;
			//xil_printf("count = %d\n\r",count);
			//xil_printf("source data = %x\n\r",source);
			//xil_printf("des data = %x\n\r",destination);
  	    }*/
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

int FfsSdPolledOpen(void)
{
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

	Res = f_open(&fil, SD_File, FA_OPEN_EXISTING|FA_READ);
	if (Res) {
		return XST_FAILURE;
	}
	Res = f_lseek(&fil, 0);
	if (Res) {
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

int FfsSdPolledClose(void)
{
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
	u32 FileSize = 352*288;

	Res = f_read(&fil, (void*)InputFrame, FileSize,
				&NumBytesRead);
	if (Res) {
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

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

	Res = f_write(&fil, (void*)OutputFrame, FileSize, &NumBytesRead);
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
