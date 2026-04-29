#include "../Header Files/mmu.h"

int compute_address(int va) {
    int page_number = va / PAGE_SIZE;
    int offset      = va % PAGE_SIZE;
    printf("compute_address: va=%d page=%d valid=%d\n",
           va, page_number, machine.page_table[page_number].valid);
    if (!machine.page_table[page_number].valid) {
        machine.PI = 3;
        machine.fault_ptr = page_number;
        return page_number;
    }
    int frame = machine.page_table[page_number].frame;
    return frame * FRAME_SIZE + offset;
}
