//==============================================================================
// Copyright (C) John-Philip Taylor
// tyljoh010@myuct.ac.za
//
// This file is part of the EEE4084F Course
//
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
//
// This is an adaptition of The "Hello World" example avaiable from
// https://en.wikipedia.org/wiki/Message_Passing_Interface#Example_program
//==============================================================================


/** \mainpage Prac3 Main Page
 *
 * \section intro_sec Introduction
 *
 * The purpose of Prac3 is to learn some basics of MPI coding.
 *
 * Look under the Files tab above to see documentation for particular files
 * in this project that have Doxygen comments.
 */



//---------- STUDENT NUMBERS --------------------------------------------------
//
// Please note:  put your student numbers here !!  <<<< NB!  NB!
//
//-----------------------------------------------------------------------------

/* Note that Doxygen comments are used in this file. */
/** \file Prac3
 *  Prac3 - MPI Main Module
 *  The purpose of this prac is to get a basic introduction to using
 *  the MPI libraries for prallel or cluster-based programming.
 */

// Includes needed for the program
#include "Prac3.h"

/** This is the master node function, describing the operations
    that the master will be doing */
void Master () {
    //! <h3>Local vars</h3>
    // The above outputs a heading to doxygen function entry
    //int  j;             //! j: Loop counter


    // Start of "Hello World" example..............................................
    printf("0: We have %d processors\n", numprocs);
    /*
    for(j = 1; j < numprocs; j++) {
    sprintf(buff, "Hello %d! ", j);
    MPI_Send(buff, BUFSIZE, MPI_CHAR, j, TAG, MPI_COMM_WORLD);
    }
    for(j = 1; j < numprocs; j++) {
    // This is blocking: normally one would use MPI_Iprobe, with MPI_ANY_SOURCE,
    // to check for messages, and only when there is a message, receive it
    // with MPI_Recv.  This would let the master receive messages from any
    // slave, instead of a specific one only.
    MPI_Recv(buff, BUFSIZE, MPI_CHAR, j, TAG, MPI_COMM_WORLD, &stat);
    printf("0: %s\n", buff);
    }
    */
    // End of "Hello World" example................................................


    // Read the input image
    if(!Input.Read("Data/greatwall.jpg")){
    printf("Cannot read image\n");
    return;
    }

    printf("test: before output allocate %d %d %d \n", Input.Width, Input.Height, Input.Components);

    // Allocated RAM for the output image
    if(!Output.Allocate(Input.Width, Input.Height, Input.Components)) return;

    printf("test: before partition\n");


    //partition image into blocks to go to each process
    JPEG input_image_blocks [numprocs];
    for(int j=0; j<numprocs; j++){
        //printf("test: outer loop %d \n", j);
        input_image_blocks[j].Allocate(Input.Width, (int)(Input.Height/numprocs), Input.Components);
        for(int r=Input.Height*j/numprocs; r<(Input.Height*(j+1)/numprocs); r++){
            for(int c=0; c<Input.Width*Input.Components; c++){
                input_image_blocks[j].Rows[r - Input.Height*j/numprocs][c] = Input.Rows[r][c];
                //printf("test: inner loop %d \n", Input.Rows[r][c]);
            }        
        }
        printf("test: outer loop %d, %d %d %d \n", j, input_image_blocks[j].Width, input_image_blocks[j].Height, input_image_blocks[j].Components);
    }

    int buff[4]; //! buff: Buffer for transferring message data
    MPI_Status stat;    //! stat: Status of the MPI application

    //send config message to slaves
    /**MESSAGE STRUCTURE:
     *   --------------------------------------------------------
     *  ||  BUFFER_SIZE  |   HEIGHT  |   WIDTH   |   COMPONENTS ||
     *   --------------------------------------------------------  
     *  ||  4 bytes      |   4 bytes |   4 bytes |   4 bytes    ||  =16bytes
    */

    int BUFFER_SIZE = sizeof(**input_image_blocks[0].Rows);
    buff[0] = BUFFER_SIZE;
    buff[1] = input_image_blocks[0].Height;
    buff[2] = input_image_blocks[0].Width;
    buff[3] = input_image_blocks[0].Components;

    for(int j=1; j<numprocs; j++){
        MPI_Send(buff, 4, MPI_INT, j, TAG, MPI_COMM_WORLD);  
    }

    //send data to the slaves
    for (int j=1; j < numprocs; j++){
        //@TODO MPI_Send for 2d array?
        MPI_Send(input_image_blocks[j].Rows, BUFFER_SIZE, MPI_BYTE, j, TAG, MPI_COMM_WORLD);  
    }

    
    //receive the processed data back from slaves
    JPEG output_image_blocks[numprocs];
    for(int j=0; j<numprocs; j++){
        output_image_blocks[j].Allocate(input_image_blocks[j].Width, input_image_blocks[j].Height, input_image_blocks[j].Components);
    }

    char receive_buffer [BUFFER_SIZE];
    for(int j=1; j<numprocs; j++){
        MPI_Recv(receive_buffer, BUFFER_SIZE, MPI_CHAR, j, TAG, MPI_COMM_WORLD, &stat);
    }
    


    //put the blocks back together
    for(int j=0; j<numprocs; j++){
        printf("test: re assemble, outer loop %d \n", j);
        for(int r=Input.Height*j/numprocs; r<(Input.Height*(j+1)/numprocs); r++){
            for(int c=0; c<Input.Width*Input.Components; c++){
                Output.Rows[r][c] = input_image_blocks[j].Rows[r - Input.Height*j/numprocs][c];
            }
        }
    }



    // Write the output image
    if(!Output.Write("Data/Output.jpg")){
    printf("Cannot write image\n");
    return;
    }

    printf("test: output %d %d %d \n", Output.Height, Output.Width, Output.Components);
    //! <h3>Output</h3> The file Output.jpg will be created on success to save
    //! the processed output.
}
//------------------------------------------------------------------------------

/** This is the Slave function, the workers of this MPI application. */
void Slave(int ID){
// Start of "Hello World" example..............................................
    char idstr[32];
    int buff [4];

    MPI_Status stat;

    // receive from rank 0 (master):
    // This is a blocking receive, which is typical for slaves.

    //receive config message
    MPI_Recv(buff, 4, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
    char receive_buffer[buff[0]];
    printf("SLAVE: message received, %d %d %d %d \n", buff[0], buff[1], buff[2], buff[3]);

    //receive data
    MPI_Recv(receive_buffer, buff[0], MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &stat);


    //perform work operation
    //@TODO: medianFilter();


    // send result to rank 0 (master):
    MPI_Send(receive_buffer, buff[0], MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
    // End of "Hello World" example................................................
}
//------------------------------------------------------------------------------

/** This is the entry point to the program. */
int main(int argc, char** argv){
 int myid;

 // MPI programs start with MPI_Init
 MPI_Init(&argc, &argv);

 // find out how big the world is
 MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

 // and this processes' rank is
 MPI_Comm_rank(MPI_COMM_WORLD, &myid);

 // At this point, all programs are running equivalently, the rank
 // distinguishes the roles of the programs, with
 // rank 0 often used as the "master".
 if(myid == 0) Master();
 else          Slave (myid);

 // MPI programs end with MPI_Finalize
 MPI_Finalize();
 return 0;
}
//------------------------------------------------------------------------------




//Median Filter
JPEG medianFilter(JPEG input_image_block, int window_width, int window_height){
    JPEG output_image_block = JPEG();
    output_image_block.Allocate(input_image_block.Width, input_image_block.Height, input_image_block.Components);
    JSAMPLE** window;

    //dummy copy code
    tic();
    int x, y;
    for(y = 0; y < input_image_block.Height; y++){
        for(x = 0; x < input_image_block.Width*input_image_block.Components; x++){
            output_image_block.Rows[y][x] = input_image_block.Rows[y][x];
        }
    }
    printf("Time = %lg ms\n", (double)toc()/1e-3);


}

















