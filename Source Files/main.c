#include "../Header Files/main.h"

int main() {
    printf("Initializing...\n");
    initialize();
    printf("\nInit MOS...\n");
    init_mos("D:/Data/SY/Operating Systems/AMOS/Data Files/jobcard.txt",
         "D:/Data/SY/Operating Systems/AMOS/Data Files/output.txt");
    srand(time(NULL));
    while (1) {
        printf("\n--- Reading Job Card ---\n");
        if (!readCard()) break;
        printf("\nJob ID: %d | Time Limit: %d | Line Limit: %d\n",
               job.jobId, job.time_limit, job.line_limit);
        printf("\nPage Table after LOAD:\n");
        for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
            if (machine.page_table[i].valid)
                printf("  Page %d -> Frame %d (RA %d-%d)\n",
                       i, machine.page_table[i].frame,
                       machine.page_table[i].frame * FRAME_SIZE,
                       machine.page_table[i].frame * FRAME_SIZE + FRAME_SIZE - 1);
        }
        printf("  Page Table stored in Frame %d (RA %d-%d)\n",
               machine.ptr, machine.ptr * FRAME_SIZE,
               machine.ptr * FRAME_SIZE + FRAME_SIZE - 1);
        printf("\nRAM after LOAD:\n");
        print_memory();
        printf("\nBooting...\n");
        boot();
        printf("\nRAM after EXECUTION:\n");
        print_memory();
        printf("\n--- Job Done, Clearing ---\n");
        clear_memory();
        for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
            machine.page_table[i].frame = -1;
            machine.page_table[i].valid = 0;
        }
        machine.SI = 0;
        machine.PI = 0;
        machine.TI = 0;
        machine.IC = 0;
        lines = 0;
    }
    mos_close();
    printf("\nAll jobs done.\n");
    return 0;
}
