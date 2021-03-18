/*
NAME: Sarah Anderson
CLASS: ECE 4680-Embedded Computing
LAB 2: RLE Codex
DESCRIPTION: The purpose of this file is to compress and decomress a file called golfcore.ppm. To check to make sure that this works, check the abount of bytes each file has.
NOTES: .rle - compressed file that this program makes and the .new - decompressed file that this program makes
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){
	int 	            i,run_count = 0;
	FILE 	            *fpt,*decfpt,*compfpt;
    unsigned char       data[2], A, B;
	
    //check to make sure that the correct num of command line args are there
	if(argc != 3){
		printf("./exec filename (-c or -d)");
		exit(0);
	}
	
    //open up file from command line
	fpt = fopen(argv[1], "rb");
	
	//Compressing the file
	if (strcmp(argv[2], "-c") == 0){
        //open uo new file for compressed image to be stored
		compfpt = fopen("golfcore.rle", "wb");
		
        //read in onne value from the image
		fread(&A, sizeof(A), 1, fpt);
		run_count++;
		
        //Do the RLE Compression Algorithim that we studied in class
		while(fread(&B, sizeof(B), 1, fpt) == 1){
			if(A == B){
				if (run_count == 255){
					fwrite(&run_count, sizeof(unsigned char), 1, compfpt);
					fwrite(&A, sizeof(A), 1, compfpt);
					run_count = 1;
                    A = B;
				}
				else
					run_count++;
			}
			else{
				fwrite(&run_count, sizeof(unsigned char), 1, compfpt);
				fwrite(&A, sizeof(A), 1, compfpt);
				run_count = 1;
                A = B;
			}
		}
		if (feof(fpt)){
			fwrite(&run_count, sizeof(unsigned char), 1, compfpt);
			fwrite(&A, sizeof(A), 1, compfpt);
            fclose(compfpt);
            printf("Done completeing the compression \n");
		}
		else{
			printf("Error...exiting \n");
			fclose(fpt);
			exit(0);
		}
	}
    //Decompressing the file
	else if (strcmp(argv[2], "-d") == 0){
        //create new file for decompressed image to go into
		decfpt = fopen("golfcore.new", "wb");
		
        //gets two points and does the decompression algorithim
		while(fread(&data, sizeof(data), 1, fpt) == 1){
			run_count = (int)data[0];
			A = data[1];
			
			for(i = 0; i < run_count; i++){
				fwrite(&A, sizeof(unsigned char), 1, decfpt);
			}
		}
		if (feof(fpt)){
			fclose(decfpt);
            printf("Done completeting the decompression \n");
		}
		else{
			printf("Error...exiting \n");
			fclose(fpt);
			exit(0);
		}
	}
	
	fclose(fpt);
	return 0;
}
