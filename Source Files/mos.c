
#include "../Header Files/mos.h"

#define PI_OPCODE_ERROR 1
#define PI_OPERAND_ERROR 2
#define PI_PAGEFAULT 3

#define SI_READ 1
#define SI_WRITE 2
#define SI_TERMINATE 3

#define TI_EXCEEDED 2

static FILE* input_path;
static FILE* output_path;
int lines = 0;



static int used_frames[NUM_FRAMES];

static void initialize_frames() {
    for (int i = 0; i < NUM_FRAMES; i++)
        used_frames[i] = 0;

    used_frames[29] = 1;
}

static int allocate_frame() {
    while (1) {
        int frame = rand() % 29;

        if (!used_frames[frame]) {
            used_frames[frame] = 1;
            return frame;
        }
    }
}


void init_mos(char* ip, char* op) {
    input_path = fopen(ip, "r");
    output_path = fopen(op, "w");

    if (input_path == NULL) {
        printf("Could Not Open JobCard!!");
        return;
    }

    if (output_path == NULL) {
        printf("Could Not Open Output File!!");
        return;
    }

    initialize_frames();
}

int mos_eof() {
    return feof(input_path);
}

int is_valid_instruction(char* str, int i, int len) {
    if (str[i] == 'H')
        return 1;

    if (i + 3 >= len)
        return 0;

    if (
        (str[i]=='G' && str[i+1]=='D') ||
        (str[i]=='P' && str[i+1]=='D') ||
        (str[i]=='L' && str[i+1]=='R') ||
        (str[i]=='S' && str[i+1]=='R') ||
        (str[i]=='C' && str[i+1]=='R') ||
        (str[i]=='B' && str[i+1]=='T')
    ) {
        if (
            str[i+2] >= '0' && str[i+2] <= '9' &&
            str[i+3] >= '0' && str[i+3] <= '9'
        ) {
            return 4;
        }
    }
    else {
        return -1;
    }

    return 0;
}

int readCard() {
    char line[256];

    if (fgets(line, sizeof(line), input_path) == NULL)
        return 0;

    line[strcspn(line, "\r\n")] = '\0';
    if (strlen(line) == 0) return readCard();

    if (strncmp(line, "$AMJ", 4) != 0)
        return 0;

    extract_params(line + 4,  0);
    extract_params(line + 8,  1);
    extract_params(line + 12, 2);
    initialize_frames();
    machine.ptr = 29;

    int current_page = 0;

    while (fgets(line, sizeof(line), input_path)) {

        line[strcspn(line, "\r\n")] = '\0';

        if (strcmp(line, "$DTA") == 0)
            break;

        if (strcmp(line, "$END") == 0)
            break;

        int frame = allocate_frame();

        machine.page_table[current_page].frame = frame;
        machine.page_table[current_page].valid = 1;

        int pt_ra = machine.ptr * FRAME_SIZE + current_page;

        machine.RAM[pt_ra][0] = '0' + frame / 10;
        machine.RAM[pt_ra][1] = '0' + frame % 10;
        machine.RAM[pt_ra][2] = '1';
        machine.RAM[pt_ra][3] = ' ';

        int ra_base = frame * FRAME_SIZE;

        int word_idx = 0;
        int k = 0;
        int len = (int)strlen(line);

        while (k < len && word_idx < FRAME_SIZE) {

            if (line[k] == ' ' || line[k] == '\t') {
                k++;
                continue;
            }

            int step = is_valid_instruction(line, k, len);

            if (step == 0) {
                machine.PI = PI_OPERAND_ERROR;
                while (fgets(line, sizeof(line), input_path)) {
                    line[strcspn(line, "\r\n")] = '\0';
                    if (strcmp(line, "$END") == 0) break;
                }
                return 1;  
            }

            if (step == -1) {
                machine.PI = PI_OPCODE_ERROR;
                while (fgets(line, sizeof(line), input_path)) {
                    line[strcspn(line, "\r\n")] = '\0';
                    if (strcmp(line, "$END") == 0) break;
                }
                return 1;
            }

            char word[4] = {' ', ' ', ' ', ' '};

            for (int i = 0; i < step; i++)
                word[i] = line[k + i];

            memcpy(machine.RAM[ra_base + word_idx], word, 4);

            word_idx++;
            k += step;
        }

        current_page++;
    }

    return 1;
}

void boot() {

    machine.IC = 0;

    while (1) {

        run();

        if (machine.TI == TI_EXCEEDED) {
            if (machine.SI == SI_WRITE) mos_write();
            printf("ERROR: Time Limit Exceeded\n");
            fprintf(output_path, "ERROR: Time Limit Exceeded\n");
            clear_memory();
            mos_terminate();

            char line[256];
            while (fgets(line, sizeof(line), input_path)) {
                line[strcspn(line, "\r\n")] = '\0';
                if (strcmp(line, "$END") == 0) break;
            }
            break;
        }
        if (machine.PI == PI_OPCODE_ERROR) {

            printf("ERROR: Operation Code Error\n");
            fprintf(output_path, "ERROR: Operation Code Error\n");

            clear_memory();
            mos_terminate();
            break;
        }

        if (machine.PI == PI_OPERAND_ERROR) {

            printf("ERROR: Operand Error\n");
            fprintf(output_path, "ERROR: Operand Error\n");

            clear_memory();
            mos_terminate();
            break;
        }

        if (machine.PI == PI_PAGEFAULT) {

            int page = machine.fault_ptr;

            if (page < 0 || page >= PAGE_TABLE_SIZE) {

                printf("ERROR: Invalid Page Fault\n");
                fprintf(output_path, "ERROR: Invalid Page Fault\n");

                mos_terminate();
                break;
            }

            int frame = allocate_frame();

            machine.page_table[page].frame = frame;
            machine.page_table[page].valid = 1;

            int pt_ra = machine.ptr * FRAME_SIZE + page;

            machine.RAM[pt_ra][0] = '0' + frame / 10;
            machine.RAM[pt_ra][1] = '0' + frame % 10;
            machine.RAM[pt_ra][2] = '1';
            machine.RAM[pt_ra][3] = ' ';

            machine.PI = 0;

            machine.IC--;

            continue;
        }

        if (machine.SI == SI_READ) {
            mos_read();
            if (machine.PI == 99) { machine.PI = 0; break; }
        }
        else if (machine.SI == SI_WRITE) {
            mos_write();
        }

        else if (machine.SI == SI_TERMINATE) {

            printf("Program Terminated Successfully\n");
            fprintf(output_path, "Program Terminated Successfully\n");

            char line[256];

            while (fgets(line, sizeof(line), input_path)) {

                line[strcspn(line, "\r\n")] = '\0';

                if (strcmp(line, "$END") == 0)
                    break;
            }

            mos_terminate();
            break;
        }



        machine.SI = 0;
    }
}

void mos_read() {
    int va = extract_address(machine.IR[2], machine.IR[3]);
    int ra = compute_address(va);

    if (machine.PI != 0) return;

    char line[256];

    if (fgets(line, sizeof(line), input_path) == NULL) {
        printf("ERROR: Out of Data\n");
        fprintf(output_path, "ERROR: Out of Data\n");
        clear_memory();
        mos_terminate();
        machine.SI = 0;
        machine.PI = 99; 
        return;
    }x

    line[strcspn(line, "\r\n")] = '\0';

    if (strcmp(line, "$END") == 0) {
        printf("ERROR: Out of Data\n");
        fprintf(output_path, "ERROR: Out of Data\n");
        clear_memory();
        mos_terminate();
        machine.SI = 0;
        machine.PI = 99;
        return;
    }

    int len = strlen(line);
    int k = 0, word = 0;
    while (k < len && word < FRAME_SIZE) {
        char w[4] = {' ', ' ', ' ', ' '};
        for (int b = 0; b < 4 && k + b < len; b++) w[b] = line[k + b];
        memcpy(machine.RAM[ra + word], w, 4);
        k += 4;
        word++;
    }
}
void mos_write() {

    if (lines >= get_line_limit()) {

        printf("ERROR: Line Limit Exceeded\n");
        fprintf(output_path, "ERROR: Line Limit Exceeded\n");

        clear_memory();
        mos_terminate();
        return;
    }

    int va = extract_address(machine.IR[2], machine.IR[3]);

    int ra = compute_address(va);

    if (machine.PI != 0)
        return;

    for (int i = 0; i < FRAME_SIZE; i++) {

        for (int b = 0; b < 4; b++) {

            char ch = machine.RAM[ra + i][b];

            if (ch != ' ')
                fprintf(output_path, "%c", ch);
        }
    }

    fprintf(output_path, "\n");

    lines++;
}

void mos_terminate() {
    fprintf(output_path, "\n\n");
}

void mos_close() {

    fclose(input_path);
    fclose(output_path);
}

