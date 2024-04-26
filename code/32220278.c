#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUM_CREG 10 // 계산 레지스터 수
#define NUM_LREG 8 // 로드 레지스터 수
#define SIZE_LINE 100 // 파일 한 줄 크기

FILE* fp; // 파일 포인터
void exception(char* line, int line_number); // 예외를 처리하는 함수
int hex2dec(char* hex, unsigned int dec, int length, int line_number); // 16진수를 10진수로 변환하여 int형으로 반환하는 함수

char* opcode, * operand1, * operand2, * operand3; // 명령 코드, 연산자 1~3

int main(int argc, char* argv[]){
    char line[SIZE_LINE]; // 파일 한 줄
    int reg_cal[NUM_CREG]; // 계산 레지스터
    int reg_load[NUM_LREG]; // 로드 레지스터
    int reg_val = 0, reg_zero = 0; // 반환 레지스터, 제로 레지스터
    int line_number = 0, length = 0; // 줄 숫자, 문자열 길이

    // 레지스터, 한 줄 배열 초기화
    memset(reg_cal, 0, NUM_CREG);
    memset(reg_load, 0, NUM_LREG);
    memset(line, 0, SIZE_LINE);

    // 명령행 인수의 개수를 출력
    printf("the number of command-line arguments: %d\n", argc);
    // 명령행 인수가 2개가 아닐 경우 오류
    if (argc != 2){
        printf("Error: the number of command-line arguments is not correct.\n");
        return 0;
    }
    // 실행 파일과 입력 파일을 출력
    printf("executable file: %s\n", argv[0]);
    printf("input file: %s\n", argv[1]);

    // 입력 파일을 읽기 모드로 열기
    fp = fopen(argv[1], "r");
    // 해당하는 파일이 없을 경우 오류
    if (fp == NULL) {
        printf("Error: invalid file name.\n");
        return 0;
    }

    // 파일 한 줄씩 읽으면서 예외 처리
    while (fgets(line, SIZE_LINE, fp)) {
        line_number++; // 읽을 때마다 줄 숫자 +1씩 증가
        exception(line, line_number); // 예외를 처리하는 함수
    }
    fclose(fp); // 파일 닫기

    line_number = 0; // 줄 숫자 초기화

    fp = fopen(argv[1], "r");

    // 파일 한 줄씩 읽으면서 계산 실행
    while (fgets(line, SIZE_LINE, fp)) {
        // 띄어쓰기를 분리자로 사용하여 문자열 분리 -> 명령 코드와 연산자 추출
        opcode = strtok(line, " ");
        operand1 = strtok(NULL, " ");
        operand2 = strtok(NULL, " ");
        operand3 = strtok(NULL, " ");

        int temp = 0, t1 = 0, t2 = 0; // 임시 레지스터 초기화
        line_number++; // 반복할 때마다 줄 숫자 +1씩 증가

        // 명령어가 LW일 때 계산
        if (strcmp(opcode, "LW") == 0) {
            for (int i = 0; i < NUM_LREG; i++) {
                if (operand1[1] == '0' + i) {
                    length = strlen(operand2);
                    temp = hex2dec(operand2, temp, length, line_number);
                    reg_load[i] = temp;
                }
            }
            printf("Loaded %d to %s\n", temp, operand1);
        }
    
        // 명령어가 ADD, SUB, MUL, DIV일 때 계산
        else if (strcmp(opcode, "ADD") == 0 || strcmp(opcode, "SUB") == 0 || strcmp(opcode, "MUL") == 0 || strcmp(opcode, "DIV") == 0) {
            // 연산자2와 연산자3을 임시 레지스터에 저장
            for (int i = 0; i < NUM_CREG; i++) {
                if (operand2[1] == '0' + i) {
                    if (operand2[0] == 's') {
                        t1 = reg_load[i];
                    }
                    else if (operand2[0] == 't') {
                        t1 = reg_cal[i];
                    }
                }
                else if (strcmp(operand2, "zero") == 0) {
                    t1 = reg_zero;
                }
                if (operand3[1] == '0' + i) {
                    if (operand3[0] == 's') {
                        t2 = reg_load[i];
                    }
                    else if (operand3[0] == 't') {
                        t2 = reg_load[i];
                    }
                }
                else if (strcmp(operand3, "zero") == 0) {
                    t2 = reg_zero;
                }
            }
            // 연산 후 연산자1 레지스터에 결과 저장
            for (int i = 0; i < NUM_CREG; i++) {
                if (operand1[1] == '0' + i) {
                    // 명령어가 ADD일 때
                    if (strcmp(opcode, "ADD") == 0) {
                        temp = t1 + t2;
                        if (operand1[0] == 't') {
                            reg_cal[i] = temp;
                        }
                        else if (operand1[0] == 'v') {
                            reg_val = temp;
                        }
                        printf("Added %s(%d) to %s(%d) and changed %s to %d\n", operand2, t1, operand3, t2, operand1, temp);
                    }
                    // 명령어가 SUB일 때
                    else if (strcmp(opcode, "SUB") == 0) {
                        temp = t1 - t2;
                        if (operand1[0] == 't') {
                            reg_cal[i] = temp;
                        }
                        else if (operand1[0] == 'v') {
                            reg_val = temp;
                        }
                        printf("Subtracted %s(%d) from %s(%d) and changed %s to %d\n", operand2, t1, operand3, t2, operand1, temp);
                    }
                    // 명령어가 MUL일 때
                    else if (strcmp(opcode, "MUL") == 0) {
                        temp = t1 * t2;
                        if (operand1[0] == 't') {
                            reg_cal[i] = temp;
                        }
                        else if (operand1[0] == 'v') {
                            reg_val = temp;
                        }
                        printf("Multiplied %s(%d) by %s(%d) and changed %s to %d\n", operand2, t1, operand3, t2, operand1, temp);
                    }
                    // 명령어가 DIV일 때
                    else if (strcmp(opcode, "DIV") == 0) {
                        // 나누는 수가 0일 경우 오류
                        if (t2 == 0){
                            printf("Error: indivisible value in line %d\n", line_number);
                        }
                        else{
                            temp = t1 / t2;
                            if (operand1[0] == 't') {
                                reg_cal[i] = temp;
                            }
                            else if (operand1[0] == 'v') {
                                reg_val = temp;
                            }
                            printf("Divided %s(%d) by %s(%d) and changed %s to %d\n", operand2, t1, operand3, t2, operand1, temp);
                        }
                    }
                }
            }
        }

        // 명령어가 NOP일 때 계산
        else if (strcmp(opcode, "NOP") == 0) {
            printf("No operation\n");
            printf("%d\n", reg_val);
        }

        // 명령어가 JMP일 때 계산
        else if (strcmp(opcode, "JMP") == 0) {
            // 연산자1(16진수)을 10진수로 변환한 후 임시 레지스터에 저장
            length = strlen(operand1);
            temp = hex2dec(operand1, temp, length, line_number);

            printf("Jumpped to line %d\n", temp);
            line_number = temp; // 줄 숫자를 건너뛰어야 하는 줄로 변경            

            fseek(fp, 0, SEEK_SET); // 파일 포인터를 처음으로 재설정
            
            // 건너뛸 줄이 파일에 존재하지 않으면 오류
            for (int i = 1; i < temp; i++) {
                if (fgets(line, SIZE_LINE, fp) == NULL) {
                    fprintf(stderr, "Error: invalid jump target in line %d\n", line_number);
                    return 1;
                }
            }
        }

        // 명령어가 BEQ, BNE일 때 계산
        else if (strcmp(opcode, "BEQ") == 0 || strcmp(opcode, "BNE") == 0) {
            // 연산자1과 연산자2를 임시 레지스터에 저장
            for (int i = 0; i < NUM_CREG; i++) {
                if (operand1[1] == '0' + i) {
                    if (operand1[0] == 's') {
                        t1 = reg_load[i];
                    }
                    else if (operand1[0] == 't') {
                        t1 = reg_cal[i];
                    }
                    else if (strcmp(operand1, "zero") == 0) {
                        t1 = reg_zero;
                    }
                }
                if (operand2[1] == '0' + i) {
                    if (operand2[0] == 's') {
                        t2 = reg_load[i];
                    }
                    else if (operand2[0] == 't') {
                        t2 = reg_cal[i];
                    }
                }
                else if (strcmp(operand2, "zero") == 0) {
                    t2 = reg_zero;
                }
                else if (operand2[0] == '0' && operand2[1] == 'x') {
                    length = strlen(operand2);
                    t2 = hex2dec(operand2, t2, length, line_number);
                }
            }
            // 연산자3(16진수)을 10진수로 변환하여 임시 레지스터에 저장
            length = strlen(operand3);
            temp = hex2dec(operand3, temp, length, line_number);

            // 명령어가 BEQ일 때 비교 후 건너뛰기
            if (strcmp(opcode, "BEQ") == 0) {
                if (t1 == t2) {
                    printf("Checked if %s(%d) is equal to %s(%d) and jumped to line %d\n", operand1, t1, operand2, t2, temp);
                    line_number = temp;

                    fseek(fp, 0, SEEK_SET);
                    for (int i = 1; i < temp; i++) {
                        if (fgets(line, SIZE_LINE, fp) == NULL) {
                            fprintf(stderr, "Error: invalid jump target in line %d\n", line_number);
                            return 1;
                        }
                    }
                }
                else {
                    printf("Checked if %s(%d) is equal to %s(%d) and didn't jump to line %d\n", operand1, t1, operand2, t2, temp);
                }
            }
            // 명령어가 BNE일 때 비교 후 건너뛰기
            else if (strcmp(opcode, "BNE") == 0) {
                if (t1 != t2) {
                    printf("Checked if %s(%d) is not equal to %s(%d) and jumped to line %d\n", operand1, t1, operand2, t2, temp);
                    line_number = temp;

                    fseek(fp, 0, SEEK_SET);
                    for (int i = 1; i < temp; i++) {
                        if (fgets(line, SIZE_LINE, fp) == NULL) {
                            fprintf(stderr, "Error: invalid jump target in line %d\n", line_number);
                            return 1;
                        }
                    }
                }
                else {
                    printf("Checked if %s(%d) is not equal to %s(%d) and didn't jump to line %d\n", operand1, t1, operand2, t2, temp);
                }
            }
        }
    
        // 명령어가 SLT일 때 계산
        else if (strcmp(opcode, "SLT") == 0) {
            // 연산자2와 연산자3을 임시 레지스터에 저장
            for (int i = 0; i < NUM_CREG; i++) {
                if (operand2[1] == '0' + i) {
                    if (operand2[0] == 's') {
                        t1 = reg_load[i];
                    }
                    else if (operand2[0] == 't') {
                        t1 = reg_cal[i];
                    }
                }
                else if (strcmp(operand2, "zero") == 0) {
                    t1 = reg_zero;
                }
                if (operand3[1] == '0' + i) {
                    if (operand3[0] == 's') {
                        t2 = reg_load[i];
                    }
                    else if (operand3[0] == 't') {
                        t2 = reg_cal[i];
                    }
                }
                else if (operand3[0] == '0' && operand3[1] == 'x') {
                    length = strlen(operand2);
                    t2 = hex2dec(operand2, t2, length, line_number);
                }
            }
            // 값 비교 후 연산자1 레지스터에 결과 저장
            if (t1 < t2) {
                temp = 1;
                printf("Checked if %s(%d) is less than %s(%d) and set 1 to %s\n", operand2, t1, operand3, t2, operand1);
                if (operand1[0] == 'v') {
                    reg_val = temp;
                }
                else if (operand1[0] == 't') {
		            for (int i=0; i<NUM_CREG; i++){
             		    if (operand1[1] == '0'+i) {
                       		reg_cal[i] = temp;
			            }
		            }
                }
            }
            else if (t1 > t2) {
                temp = 0;
                printf("Checked if %s(%d) is less than %s(%d) and set 0 to %s\n", operand2, t1, operand3, t2, operand1);
                    if (operand1[0] == 'v') {
                        reg_val = temp;
                }
                else if (operand1[0] == 't') {
		            for (int i=0; i<NUM_CREG; i++){
             		    if (operand1[1] == '0'+i) {
                       		reg_cal[i] = temp;
			            }
		            }
                }
            }
        }
    }
    fclose(fp);
    return 0;
}

// 예외를 처리하는 함수
void exception(char* line, int line_number){
    opcode = strtok(line, " ");
    operand1 = strtok(NULL, " ");
    operand2 = strtok(NULL, " ");
    operand3 = strtok(NULL, " ");

        // 명령어가 ADD, SUB, MUL, DIV일 때 예외 처리
        if (strcmp(opcode, "ADD") == 0 || strcmp(opcode, "SUB") == 0 || strcmp(opcode, "MUL") == 0 || strcmp(opcode, "DIV") == 0) {
            // 연산자1 검사
            if (!(operand1[0] == 'v' || operand1[0] == 't')) {
                printf("Error: invalid operand1 in line %d\n", line_number);
                exit(EXIT_FAILURE);
            }
            // 연산자2 검사
            if (!(operand2[0] == 's' || operand2[0] == 't' || strcmp(operand2, "zero") == 0)) {
                printf("Error: invalid operand2 in line %d\n", line_number);
                exit(EXIT_FAILURE);
            }
            // 연산자3 검사
            if (!(operand3[0] == 's' || operand3[0] == 't' || strcmp(operand3, "zero") == 0)) {
                printf("Error: invalid operand3 in line %d\n", line_number);
                exit(EXIT_FAILURE);
            }
        }

        // 명령어가 LW일 때 예외 처리
        else if (strcmp(opcode, "LW") == 0) {
            // 연산자1 검사
            if (operand1[0] != 's') {
                printf("Error: invalid operand1 in line %d\n", line_number);
                exit(EXIT_FAILURE);
            }
            // 연산자2 검사
            if (operand2[0] != '0' && operand2[1] != 'x') {
                printf("Error: invalid operand2 in line %d\n", line_number);
                exit(EXIT_FAILURE);
            }
            // 연산자3 검사
            if (operand3[0] != '\n') {
                printf("Error: invalid operand3 in line %d\n", line_number);
                exit(EXIT_FAILURE);
            }
        }

        // 명령어가 NOP일 때 예외 처리
        else if (strcmp(opcode, "NOP") == 0) {
            if (!(operand1 == NULL && operand2 == NULL && operand3 == NULL)) {
                printf("Error: invalid operand in line %d\n", line_number);
                exit(EXIT_FAILURE);
            }
        }

        // 명령어가 JMP일 때 예외 처리
        else if (strcmp(opcode, "JMP") == 0) {
            // 연산자1 검사
            if (operand1[0] != '0' && operand1[1] != 'x'){
                printf("Error: invalid operand1 in line %d\n", line_number);
                exit(EXIT_FAILURE);
            }
            // 연산자2와 연산자3 검사
            if (operand2 != NULL && operand3 != NULL){
                printf("Error: invalid operand in line %d\n", line_number);
                exit(EXIT_FAILURE);
            }
        }

        // 명령어가 BEQ, BNE일 때 예외 처리
        else if (strcmp(opcode, "BEQ") == 0 || strcmp(opcode, "BNE") == 0) {
            // 연산자1 검사
            if (!(operand1[0] == 's' || operand1[0] == 't' || strcmp(operand1, "zero") != 0)) {
                printf("Error: invalid operand1 in line %d\n", line_number);
                exit(EXIT_FAILURE);
            }
            // 연산자2 검사
            if (!(operand2[0] == 's' || operand2[0] == 't' || strcmp(operand2, "zero") == 0 || (operand2[0] == '0' && operand2[0] == 'x'))) {
                printf("Error: invalid operand2 in line %d\n", line_number);
                exit(EXIT_FAILURE);
            }
            // 연산자3 검사
            if (!(operand3[0] == '0' && operand3[1] == 'x')) {
                printf("Error: invalid operand3 in line %d\n", line_number);
                exit(EXIT_FAILURE);
            }
        }

        // 명령어가 SLT일 때 예외 처리
        else if (strcmp(opcode, "SLT") == 0) {
            // 연산자1 검사
            if (!(operand1[0] == 't' || operand1[0] == 'v')) {
                printf("Error: invalid operand1 in line %d\n", line_number);
                exit(EXIT_FAILURE);
            }
            // 연산자2 검사
            if (!(operand2[0] == 's' || operand2[0] == 't' || strcmp(operand2, "zero") == 0)) {
                printf("Error: invalid operand2 in line %d\n", line_number);
                exit(EXIT_FAILURE);
            }
            // 연산자3 검사
            if (!(operand3[0] == 's' || operand3[0] == 't' || (operand3[0] == '0' && operand3[0] == 'x'))) {
                printf("Error: invalid operand3 in line %d\n", line_number);
                exit(EXIT_FAILURE);
            }
        } 

        // 일치하는 명령 코드가 없을 경우 오류
        else{
            printf("Error: invalid opcode in line %d\n", line_number);
            exit(EXIT_FAILURE);
        }
}

// 16진수를 10진수로 변환하여 정수형으로 반환하는 함수
int hex2dec(char* hex, unsigned int dec, int length, int line_number){
    // 각 자리수의 값에 16의 거듭제곱을 곱하여 10진수로 변환
    for (int i = 2; i < length; ++i) {
        dec *= 16;

        if (hex[i] >= '0' && hex[i] <= '9') {
            dec += hex[i] - '0';
        }
        else if (hex[i] >= 'a' && hex[i] <= 'f') {
            dec += hex[i] - 'a' + 10;
        }
        else if (hex[i] >= 'A' && hex[i] <= 'F') {
            dec += hex[i] - 'A' + 10;
        }
        // '0x'뒤에 오는 숫자가 문자가 올바르지 않을 경우
        else {
            printf("Error: invalid hexadecimal value in line %d\n", line_number);
            exit(EXIT_FAILURE);
        }
        // int형으로 변환하기에 너무 큰 숫자가 올 경우 
        if (dec > 2147483647) {
            printf("Error: hexadecimal value too large in line %d\n", line_number);
            exit(EXIT_FAILURE);
        }
    }
    return dec;
}