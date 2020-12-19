
/**
 * Intellivision rom file converter for the NT Mini Noir intellivision rom format
 * Copyright (c) 2020 Robert Mendon (https://github.com/dot-bob)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <unistd.h>
using namespace std;

#define MAXBUFFER 256

// header details
unsigned int MapAddress[4] = {0,0,0,0};
unsigned int DataSize[4] = {0,0,0,0};

void PrintHelp(void)
{
	cout	<< "\nNT Mini Noir intellivision rom formater v1.2\nCommand Use:  int2intv -m X <input file> <output file>\n"
			<< "\nSwitches:\n-m X, where X = rom memory map to encode\n";
			
	exit(1);
}

void PopulateHeader(int Format)
{
    switch(Format)
    {
        case 0:
        MapAddress[0] = 0x5000;
        DataSize[0] = 0x2000;

        MapAddress[1] = 0xD000;
        DataSize[1] = 0x1000;

        MapAddress[2] = 0xF000;
        DataSize[2] = 0x1000;
        break;

        case 1:
        MapAddress[0] = 0x5000;
        DataSize[0] = 0x2000;

        MapAddress[1] = 0xD000;
        DataSize[1] = 0x2FFF;
        break;
        
        case 2:
        MapAddress[0] = 0x5000;
        DataSize[0] = 0x2000;

        MapAddress[1] = 0x9000;
        DataSize[1] = 0x2FFF;

        MapAddress[2] = 0xD000;
        DataSize[2] = 0x1000;
        break;

        case 3:
        case 9:
        MapAddress[0] = 0x5000;
        DataSize[0] = 0x2000;

        MapAddress[1] = 0x9000;
        DataSize[1] = 0x2000;

        MapAddress[2] = 0xD000;
        DataSize[2] = 0x1000;

        MapAddress[3] = 0xF000;
        DataSize[3] = 0x1000;
        break;
        
        case 4:
        MapAddress[0] = 0x5000;
        DataSize[0] = 0x2000;
        break;

        case 5:
        MapAddress[0] = 0x5000;
        DataSize[0] = 0x2FFF;

        MapAddress[1] = 0x9000;
        DataSize[1] = 0x2FFF;
        break;

        case 6:
        MapAddress[0] = 0x6000;
        DataSize[0] = 0x2000;
        break;

        case 7:
        MapAddress[0] = 0x4800;
        DataSize[0] = 0x2000;
        break;

        case 8:
        MapAddress[0] = 0x5000;
        DataSize[0] = 0x1000;

        MapAddress[1] = 0x7000;
        DataSize[1] = 0x1000;
        break;

        case 99:
        MapAddress[0] = 0x1000;
        DataSize[0] = 0x1000;
        break;

        default:
        cout << "Error: invalid rom map." << endl;
        PrintHelp();
        break;
    }
}

int ProcessArg(const char *value, char *buffer, int argc, char* argv[])
{
	int i;
	for(i = 1;i< argc;i++)
	{
		if(strcmp(value,argv[i]) == 0)
		{
			i++;
			if(i < argc)
			{
				if(strlen(argv[i]) < MAXBUFFER) // Prevent buffer overflow.
				{
					strcpy(buffer, argv[i]);
					return(1);
				}
				else
				{
					cout << "Error: Argument too long.  Buffer overflow.";
					exit(3);
					return(-1);
				}
			}
			else
				return(-1);
		}
	}
	return(0);
}

bool CheckforFile(const char *path)
{
	
	if(access(path, 0) == 0)
	{
		return(true);
	}
	else return(false);
}

const int wordsize = 2;
const char tempFileName[] = "tempfile.tmp";

int main(int argc, char* argv[])
{
    ifstream inputFile;
    ofstream outputFile;

    char inputBuffer[wordsize];
    char outputBuffer[wordsize];
    char stringBuffer[MAXBUFFER];
    char intputFileName[MAXBUFFER];
    char outputFileName[MAXBUFFER];
    unsigned int readSize = 0;
    unsigned int headerSize;

    if(argc < 4)
	{
        PrintHelp();
	}

    if(ProcessArg("-m",stringBuffer,argc,argv) != 1)
		PrintHelp();

    PopulateHeader(atoi(stringBuffer));
    
    strcpy(intputFileName, argv[3]);
    strcpy(outputFileName, argv[4]);

    if(!CheckforFile(intputFileName))
    {
        cout << "Error opening input file: "<< intputFileName << endl;
        exit(1);
    }

    inputFile.open(intputFileName, ios::binary);
    outputFile.open(tempFileName, ios::binary);

    // swap data order
    while(inputFile.read(inputBuffer, wordsize))
    {   
        readSize++; 
        for(int i=0;i<wordsize;i++)
            outputBuffer[i] = inputBuffer[(wordsize-1)-i];

        outputFile.write(outputBuffer, wordsize);
    }
    inputFile.close();
    outputFile.close();
    
    inputFile.open(tempFileName, ios::binary);
    outputFile.open(outputFileName, ios::binary);

// first block
    if(readSize > DataSize[0])
        headerSize = DataSize[0];
    else
        headerSize = readSize;

    readSize -= headerSize;
    
    //write header
    outputFile.write((const char *) &MapAddress[0], sizeof(unsigned int)); // write address
    outputFile.write((const char *) &headerSize, sizeof(unsigned int)); // write address

    //write data
    while(inputFile.read(inputBuffer, wordsize))
    {
        outputFile.write(inputBuffer, wordsize);
        if(--headerSize == 0)
            break;
    }

    if(readSize == 0)
        goto endblock;

// second block
    if(readSize > DataSize[1])
        headerSize = DataSize[1];
    else
        headerSize = readSize;

    readSize -= headerSize;

    //write header
    outputFile.write((const char *) &MapAddress[1], sizeof(unsigned int)); // write address
    outputFile.write((const char *) &headerSize, sizeof(unsigned int)); // write size
 
    //write data
    while(inputFile.read(inputBuffer, wordsize))
    {
        outputFile.write(inputBuffer, wordsize);
        if(--headerSize == 0)
            break;
    }

    if(readSize == 0)
        goto endblock;

// third block
    if(readSize > DataSize[2])
        headerSize = DataSize[2];
    else
        headerSize = readSize;

     readSize -= headerSize;

    //write header
    outputFile.write((const char *) &MapAddress[2], sizeof(unsigned int)); // write address
    outputFile.write((const char *) &headerSize, sizeof(unsigned int)); // write size

    //write data
    while(inputFile.read(inputBuffer, wordsize))
    {
        outputFile.write(inputBuffer, wordsize);
        if(--headerSize == 0)
            break;
    }

    if(readSize == 0)
        goto endblock;

// fourth block
    headerSize = readSize;

    //write header
    outputFile.write((const char *) &MapAddress[3], sizeof(unsigned int)); // write address
    outputFile.write((const char *) &headerSize, sizeof(unsigned int)); // write size

    //write data
    while(inputFile.read(inputBuffer, wordsize))
    {
        outputFile.write(inputBuffer, wordsize);
        if(--headerSize == 0)
            break;
    }

endblock:
// end block
    headerSize = 0;
    // write footer: write the address and size as zero
    outputFile.write((const char *) &headerSize, sizeof(unsigned int)); // write address
    outputFile.write((const char *) &headerSize, sizeof(unsigned int)); // write size

    inputFile.close();
    outputFile.close();

    unlink(tempFileName);
    return 0;
}
