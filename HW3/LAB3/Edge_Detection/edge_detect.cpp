#include "edge_detect.h"
#include "simple_bus_types.h"
#include "math.h"

void edge_detect::main_action()
{
    int mydata[9];
    for(int j=0;j<height;j++) {
        for(int i=0;i<width;i++) {
            /* Boundary condition */ 
			if(i == 0 || i == (width-1) || j == 0 || j == (height-1) ) {
                int a = 0;
                bus_port->direct_write(&a,0x100000);
            }
			/* Normal cases */
            else {
                //=========================== write your code here ============================
                // Read pixel values from frame_buffer & fill it into mydata[0]~mydata[8]
				bus_port->direct_read(&mydata[0], ((j - 1) * 512 + i - 1) * 4);
				bus_port->direct_read(&mydata[1], ((j - 1) * 512 + i) * 4);
				bus_port->direct_read(&mydata[2], ((j - 1) * 512 + i + 1) * 4);
				bus_port->direct_read(&mydata[3], (j * 512 + i - 1) * 4);
				bus_port->direct_read(&mydata[4], (j * 512 + i) * 4);
				bus_port->direct_read(&mydata[5], (j * 512 + i + 1) * 4);
				bus_port->direct_read(&mydata[6], ((j + 1) * 512 + i - 1) * 4);
				bus_port->direct_read(&mydata[7], ((j + 1) * 512 + i) * 4);
				bus_port->direct_read(&mydata[8], ((j + 1) * 512 + i + 1) * 4);

                // Edge Detection
                int edge = edge_filtering(mydata);

                // write to display_buffer
				bus_port->direct_write(&edge, 0x100000);

                //=============================================================================
            }
        }
    }
}

int edge_detect::edge_filtering(int* mydata)
{

    int hor_coef[3][3] = {
        { -1,   0,  1},
        { -2,   0,  2},
        { -1,   0,  1}
    };
    int ver_coef[3][3] = {
        {  1,  2,  1},
        {  0,  0,  0},
        { -1, -2, -1}
    };
    int ver = 0;
    int hor = 0;
    int edge = 0;
    for(int j=0;j<3;j++) {
        for(int i=0;i<3;i++) {
            hor += hor_coef[j][i]*mydata[3*j+i];
            ver += ver_coef[j][i]*mydata[3*j+i];
        }
    }
    double amp = sqrt((double)(hor*hor+ver*ver));
    edge = (amp>50) ? 0xFF : 0x00;
    return edge;
}