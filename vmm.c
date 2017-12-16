#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>



const int PAGE_SIZE = 256;
const int VM_SIZE = 256;
const int MM_SIZE = 128;
const int ATC_SIZE = 16;


int main(int argc, char *argv[]){

    int physical_memory[MM_SIZE];
    int virtual_memory[VM_SIZE][2];
    int atc[ATC_SIZE][2];
    int i = 0;


    int page_number = 0;
    int physicalAddress = 0;
    int pageFaultCount = 0;

    double pageFaultRate = 0.0;

    char *line = NULL;
    size_t len = 0;


    int largest = 0;
    int evict = 0;

    

    FILE *pFile;
    pFile = fopen(argv[1], "r");

    // INITIAL PAGE TABLE FILLING
    // [0] Physical Address [1] Age
    for(i = 0; i < VM_SIZE; i++){
        virtual_memory[i][0] = (i > MM_SIZE - 1)? -1 : i;
        virtual_memory[i][1] = (i > MM_SIZE - 1)? -1 : MM_SIZE - i;
    }
    for(i = 0; i < MM_SIZE; i++){
        physical_memory[i] = i;
    }

    for(i = 0; i < ATC_SIZE; i++){
      atc[i][0] = (i > ATC_SIZE - 1)? -1 : i;
      atc[i][1] = (i > ATC_SIZE - 1)? -1 : ATC_SIZE - i;
    }

    // Check to see if correct arguments exist
    if(argc != 2){
        printf("Incorrect Number of Arguments.\n");
        return 1;
    }

    //checks to see if the .txt file supplied is empty
    if(pFile == NULL){
        printf("Error opening a file %s: %s\n", argv[1], strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("==================================\n" );
    printf("||    Virtual Memory Manager    ||\n" );
    printf("==================================\n" );

    printf("Translating 1000 Logical Addresses: \n\n");

    ssize_t read;
    while((read = getline(&line, &len, pFile)) != -1){
        int offset = atoi(line) & 255;
        int page = atoi(line) & 65280;
        int page_table_number = page >> 8;
        int atc_hit = 0;
	int atc_miss = 0;

        for(i = 0; i < ATC_SIZE; i++){
         if(atc[i][0] == page_table_number){
           atc_hit = 1;
           printf("Address Translation Cache HIT\n");
           break;
         }
	 else if(atc[i][0] != page_table_number){
	   atc_miss = 1;
	   printf("Address Translation cache MISS\n");
	   break;
	 }
       }

        if(virtual_memory[page_table_number][0] < 0 && !atc_hit){
            pageFaultCount++;
            printf("Page not found in page table. Page Fault\n");
            // EVICT SOMEONE
            for(i = 0; i < VM_SIZE; i++){
                if(virtual_memory[i][1] > largest){
                    largest = virtual_memory[i][1];
                    evict = i;
                }
            }

	    printf("\nEvict: %d",evict);
	    int r = rand();
	    printf("r: %d",r);
            int atc_replacement = r % 15;
	    printf("\natc_replacement: %d",atc_replacement);
            atc[atc_replacement][0] = page_table_number;
            atc[atc_replacement][1] = virtual_memory[evict][0];
            virtual_memory[page_table_number][0] = virtual_memory[evict][0];
            virtual_memory[page_table_number][1] = 0;
            virtual_memory[evict][0] = -1;
            virtual_memory[evict][1] = 0;
        }

        //printing formatting for Virtual Address
        printf("\nVirtual Address = %d     \t", page);

        //calculates the physical address
        physicalAddress = (physical_memory[virtual_memory[page_table_number][0]] * PAGE_SIZE) + offset;


        printf("Physical Address: %d\n", physicalAddress);


        page_number++;

        for(i = 0; i < VM_SIZE; i++){
            virtual_memory[i][1]++;
        }
    }

    //calculates the % of page faults
    printf("No. Of Page Faults = %d",pageFaultCount);
    pageFaultRate = (double) pageFaultCount / 1000 * 100;
    printf("\nPage Fault Rate: %.2f%% \n", pageFaultRate);

    free(line);
    fclose(pFile);

    exit(EXIT_SUCCESS);

}
