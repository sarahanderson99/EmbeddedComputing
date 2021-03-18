/*
NAME: Sarah Anderson
CLASS: ECE 4680-Embedded Computing
LAB 3: LZW Codec
DESCRIPTION: The purpose of this file is to compress and decomress a file called golfcore.ppm. To check to make sure that this works, check the abount of bytes each file has and use the command diff to make sure that the original is the same as the new.
NOTES: .lzw - compressed file and the .new - decompressed file
       ls -nl : to  get info for the byte size
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv){
    
	FILE *fpt_in, *fpt_out;
	int i, j, k, found, end, dcomp_size = 256, psize;
	unsigned char *dict_length, **dict, *prev, current, *pc, *X, Y, Z, *XYZ;
	unsigned short pIndex, P, C;

	if (argc != 4){
			printf("Usage: ./myprog (-c or -d) <input file> <output file>\n");
			exit(1);
	}

	fpt_in = fopen(argv[2], "rb"); //input file
	fpt_out = fopen(argv[3], "wb"); //output file

	if (fpt_in == NULL){
		printf("Error Reading File\n");
		exit(1);
	}

	//Dictionary inital
	dict = (unsigned char **)calloc(105536,sizeof(unsigned short));
	dict_length = (unsigned char *)calloc(105536,sizeof(unsigned short));
    
	for (i = 0; i <= 255; i++){
		dict[i] = malloc(1);
		dict[i][0] = i;
		dict_length[i] = 1;
	}
		
	//Compression algorithim
    if (strcmp(argv[1], "-c") == 0){
		printf("Compressing %s\n", argv[2]);

		prev = malloc(1);
		fread(prev, 1, 1, fpt_in);
		pIndex = (unsigned short) prev[0];
		psize = 1;

		end = 1;
		while(end == 1){
			end = fread(&current, 1, 1, fpt_in);

			pc = malloc(psize + 1);

			memcpy(pc, prev, psize);
			memcpy((pc + psize), &current, 1);

			//see if P+C is in the dict
			found = 0;
			for (j = 256; j < dcomp_size; j++){
				if ((psize + 1) == dict_length[j]){
					for (k = 0; k < psize + 1; k++){
						if (pc[k] == dict[j][k]){
							if (k == psize){
								found = 1;
								pIndex = (unsigned short) j; 
							}
						}
						else 
							break;	
					}

				}
				if (found == 1)
					break;
			}

			//if P+C is in the dictionary (finds it in the dictionary)
			if(found == 1){
				free(prev);
                psize++;
				prev = calloc(psize, 1);
				memcpy(prev, pc, psize);
			}

			// P+C not in Dictionary
			else{
				fwrite(&pIndex, 2, 1, fpt_out);
				dict[dcomp_size] = malloc(psize+1);
				memcpy( dict[dcomp_size], pc, psize + 1);
				dict_length[dcomp_size] = psize+1;
				psize = 1;
				free(prev);
				prev = calloc(1, 1);
				prev[0] = current;
				pIndex = (unsigned short) current;
				dcomp_size++;
			}
			free(pc);
		}

		fwrite(&pIndex, 2, 1, fpt_out);
        printf("Done completing the compressing \n");
	}

	//Decompression algorthim
    else if(strcmp(argv[1], "-d") == 0){

		printf("Decompressing of %s in process\n",argv[2]);
		C = 0; //starts at 0
		fread(&C, 2, 1,fpt_in);
		fwrite(dict[C],1,1,fpt_out);

		end = 1;

		while(end == 1){
			P = C;
			psize = dict_length[P];

			end = fread(&C,2,1,fpt_in);
			if (end == 0)
				break;

			X = (unsigned char *)calloc(dict_length[P],sizeof(unsigned char));
			XYZ = (unsigned char *) calloc(dict_length[P] + 1,sizeof(unsigned char));

			memcpy(X,dict[P],dict_length[P]);
			
			//if c is in the dictionary
			if (C < dcomp_size){
				fwrite(dict[C], 1, dict_length[C], fpt_out);
				Y = dict[C][0];
				memcpy(XYZ, X, dict_length[P]);
				memcpy(XYZ + dict_length[P],&Y,1);
		
			}
			
			//does this if c isnt found in the dictionary
			else {
				Z = dict[P][0];
				memcpy(XYZ, X, dict_length[P]);
				memcpy(XYZ + dict_length[P], &Z, 1);
				fwrite(XYZ, 1, psize + 1, fpt_out);
			}
			
			//Adding value to the dictionary since it isnt in the dictionary
			dict[dcomp_size] = calloc(psize+1, sizeof(unsigned char));
			memcpy(dict[dcomp_size], XYZ, psize + 1);
			dict_length[dcomp_size] = psize + 1;
			dcomp_size++;
			free(X);
			free(XYZ);
		}
        printf("Done completing the decompressing \n");
	}

	fclose(fpt_in);
	fclose(fpt_out);
	return 0;
}
