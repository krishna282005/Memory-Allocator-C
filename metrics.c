#include <stdio.h>
#include <stdint.h>
#include "heap.h"
#include "metrics.h"

float frag_index[100];
int count = 0;

void reset_fragmentation_log(void)
{
    count = 0;
}


void fragment_analyzer()
{
    BlockHeader *current = (BlockHeader *)free_list_head;
    uint32_t total_free_memory = 0;
    uint32_t max_payload = 0;
    float fi=0;
    while(current != NULL)
    {
        max_payload = (current->size > max_payload)?(current->size):(max_payload);
        total_free_memory = total_free_memory + current->size;
        current = *(BlockHeader **)(current + 1);
    }
    if (total_free_memory == 0)
    {
        printf("\n NO FREE SPACE \n");
    }
    else
    {
        if (count<100)
        {
            frag_index[count]= 1 - (float)(max_payload)/total_free_memory;
            fi=frag_index[count];
        }
        else
        {
            int i=0;
            while(i<99)
            {
                frag_index[i]=frag_index[i+1];
                i++;
            }
            frag_index[99]=1 - (float)(max_payload)/total_free_memory;
            fi=frag_index[99];
        }
    }
    printf("\n Total free memory = %u",total_free_memory);
    printf("\n Largest free block size = %u",max_payload);
    printf("\n Fragmentation index = %0.3f and no. of iteration = %u\n", fi,count);
    count=count+1;
}

void export_fragmentation_csv(const char *filename)
{
    FILE *fp = fopen(filename, "w");

    if (fp == NULL)
    {
        printf("Unable to create CSV file.\n");
        return;
    }

    fprintf(fp, "Iteration,Fragmentation_Index\n");

    for (int i = 0; i < count && i < 100; i++)
    {
        fprintf(fp, "%d,%f\n", i, frag_index[i]);
    }

    fclose(fp);

    printf("\nfragmentation.csv generated successfully.\n");
}
