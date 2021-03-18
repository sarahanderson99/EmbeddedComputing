/*
NAME: Sarah Anderson and Denise Linn
CLASS: ECE 4680-Embedded Computing
LAB 4: Huffman Codec
DESCRIPTION: The purpose of this file is to compress and decomress a file called golfcore.ppm. To check to make sure that this works, check the abount of bytes each file has and use the command diff to make sure that the original is the same as the new.
NOTES: .huf - compressed file and the .new - decompressed file
       ls -nl : to  get info for the byte size
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct node {
    int nfreq;
    int symbol; //0-255 or -1 for a non-leaf
    struct node* left;
    struct node* right;
    struct node* parent;
};

struct node *combine (struct node * array, int n, int m){
    struct node *new;
    
    new = (struct node *)calloc (1, sizeof (struct node));
    
    new->nfreq = array[n].nfreq + array[m].nfreq;
    new->left = &array[n];
    new->right = &array[m];
    new->parent = NULL;
    new->symbol = -1;
    
    return(new);
}

struct node* binary_tree(int freq[256]){
    int i = 0, j = 0, n = 0, m = 0, num_node = 0;
    struct node *array, temp, *arr_temp, *new_node;
    
    array = (struct node *)calloc (511, sizeof(struct node));
    arr_temp = (struct node *)calloc (256, sizeof(struct node));
    
    //get data into an array of structs
    for (i = 0; i < 256; i++){
        array[i].nfreq = freq[i];
        array[i].symbol = i;
        array[i].left = NULL;
        array[i].right = NULL;
        array[i].parent = NULL;
        num_node++;
    }
    
    while (num_node < 511){
        n = 0;
        //sorts
        for (i = 0; i < num_node-1; i++){
            for (j = 0; j < num_node-i-1; j++){
                if (array[j].nfreq > array[j+1].nfreq){
                    if(array[j].right != NULL){
                        array[j].right->parent = &array[j+1];
                        array[j].left->parent = &array[j+1];
                    }
                    
                    if(array[j+1].right != NULL){
                        array[j+1].right->parent = &array[j];
                        array[j+1].left->parent = &array[j];
                    }
                    
                    if(array[j].parent != NULL){
                        array[j].parent->right = &array[j+1];
                        array[j].parent->left = &array[j+1];
                    }
                    if(array[j+1].parent != NULL){
                        array[j+1].parent->right = &array[j];
                        array[j+1].parent->left = &array[j];
                    }
                    
                    temp = array[j];
                    array[j] = array[j+1];
                    array[j+1] = temp;
                }
            }
        }
        
        while(array[n].parent != NULL){
               n++;
        }
        m = n + 1;
           
        while(array[m].parent != NULL){
               m++;
        }
        
        //combine nodes
        new_node = combine(array, n, m);
        
        array[n].parent = &array[num_node];
        array[m].parent = &array[num_node];
 
        array[num_node] = *new_node;
        
        num_node++;
    }
    return (array);
}

int main(int argc, char **argv){
    
    FILE *fpt_in, *fpt_out;
    int freq[256], i = 0, j = 0, count[256], code[256], buff_count = 0, total_freq = 0, found;
    unsigned char temp, input, itemp, s;
    struct node *array, *temp_node;
    unsigned int buffer = 0;
    
    if (argc != 4){
            printf("Usage: ./myprog (-c or -d) <input file> <output file>\n");
            exit(1);
    }

    fpt_in = fopen(argv[2], "rb"); //input file
    
    //compression
    if (strcmp(argv[1], "-c") == 0){
        while (i < 256){
            freq[i] = 0;
            i++;
        }
    
        while(fread(&temp, 1, 1, fpt_in)){
            freq[(int)temp] += 1;
            total_freq++;
        }
    
        fpt_out = fopen(argv[3], "wb"); //output file
        fwrite(freq, 4, 256, fpt_out);
        fwrite(&total_freq, 4, 1, fpt_out);
        
        //build binary tree
        array = binary_tree(freq);
        
        //finds the  varying bit pattern
        for(i = 0; i < 256; i++){
            code[i] = 0x0000;
            count[i] = 0;
            j = 0;
            
            while (array[j].symbol != i){
                j++;
            }
            
            temp_node = &array[j];
            
            while (temp_node->parent != NULL){
                if(temp_node->parent->right == temp_node){
                    //1
                    code[i] = code[i] | (0x1 << count[i]);
                    count[i]++;
                }
                else{
                    //0
                    count[i]++;
                }
                temp_node = temp_node->parent;
            }
        }
        
        fseek(fpt_in, 0, SEEK_SET);
        
        while(fread(&input, 1, 1, fpt_in)){
            buffer = buffer << count[(int)input];
            buffer =  buffer | code[(int)input];
            buff_count += count[(int)input];
            while(buff_count >= 8){
                itemp = (buffer >> (buff_count - 8)) & 0xFF;
                buff_count -= 8;
                fwrite(&itemp, 1, 1, fpt_out);
                if(buff_count != 0){
                    buffer = buffer << (32 - buff_count);
                    buffer = buffer >> (32 - buff_count);
                }
                else{
                    buffer = 0;
                }
            }
        }
        if(buff_count > 0){
            itemp = buffer & 0xFF;
            itemp = itemp << (8 - buff_count);
            fwrite(&itemp, 1, 1, fpt_out);
        }
        
       fclose(fpt_in);
       fclose (fpt_out);
        
        printf("Compression is complete\n");
    }
    //decompression
    else if(strcmp(argv[1], "-d") == 0){
        
        fpt_in = fopen(argv[2], "rb"); //input file
        fread(freq, 4, 256, fpt_in);
        fread (&total_freq, 4, 1, fpt_in);
        
        fpt_out = fopen(argv[3], "wb"); //output file
        
        array = binary_tree(freq);
        
        //finds the  varying bit pattern
        for(i = 0; i < 256; i++){
            code[i] = 0x0000;
            count[i] = 0;
            j = 0;
            
            while (array[j].symbol != i){
                j++;
            }
            
            temp_node = &array[j];
            
            while (temp_node->parent != NULL){
                if(temp_node->parent->right == temp_node){
                    //1
                    code[i] = code[i] | (0x1 << count[i]);
                    count[i]++;
                }
                else{
                    //0
                    count[i]++;
                }
                temp_node = temp_node->parent;
            }
        }
        
        fread(&itemp, 1, 1, fpt_in);
        buffer = (itemp << 8) ;
        fread(&itemp, 1, 1, fpt_in);
        buffer = buffer | itemp;
        buffer = buffer << 8;
        fread(&itemp, 1, 1, fpt_in);
        buffer = buffer | itemp;
        buffer = buffer << 8;
        fread(&itemp, 1, 1, fpt_in);
        buffer = buffer | itemp;
        buff_count = 32;
        
        while(total_freq > 0){
            s=0;
            j = 0;
            found = 0;
            while(j < 256){
                i = 1;
                if((count[(int)s] < buff_count)){
                    while((buffer >> (32-i)) == (code[j] >> (count[j] -i))){
                        if((count[(int)s] == i) && (count[(int)s] < buff_count)){
                            fwrite(&s, 1,1, fpt_out);
                            buffer = (buffer << count[(int)s]) ;
                            total_freq--;
                            buff_count -= count[(int)s];
                            found = 1;
                            break;
                        }
                        i++;
                    }
                }
                s++;
                j++;
                if(total_freq == 0){
                    break;
                }
            }
            if(found == 0){
                fread(&itemp, 1, 1, fpt_in);
                buffer = ((int)itemp << (24 - buff_count)) | buffer;
                buff_count += 8;
            }
        }
        
        fclose(fpt_in);
        fclose(fpt_out);
        printf("Decompression is complete\n");
    }
    return 0;
}
