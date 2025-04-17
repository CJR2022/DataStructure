//2022041015 최정륜

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <windows.h>

#define COMPARE(x, y) ((x) > (y) ? 1 : ((x) < (y) ? -1 : 0))

typedef struct // 초기버전 다항식
{
    int degree;
    int *coef;
} Polynomial;

typedef struct { // 개선된 버전 다항식
    int coef;
    int expon;
} Term;
Term *terms; // 모든 다항식을 저장하는 전역 배열
int avail = 0; // 다음 사용 가능한 위치

typedef struct polyNode *polyPointer; 
typedef struct polyNode { // 다항식 노드
int coef;
int expon;
polyPointer link;
};


Polynomial padd_ver1(Polynomial a, Polynomial b); //다항식 덧셈(초기버전)

void WritePolynomialToFile_v1(FILE *file, Polynomial poly); //다항식 출력(초기버전)

void padd_ver2(int startA, int finishA, int startB, int finishB, int *startD, int *finishD); //다항식 덧셈(개선된 버전)

void WritePolynomialToFile_v2(FILE *file, int start, int finish); //다항식 출력(개선된 버전)

void attach(int coefficient, int exponent); //다항식 항 추가(개선된 버전)

int compare(const void *a, const void *b); // 내림차순 비교 

polyPointer createLinkedListFromTerms(Term *terms, int size); //다항식 연결리스트 생성

polyPointer padd_ver3(polyPointer a, polyPointer b); //다항식 덧셈(연결리스트 버전)

void WritePolynomialToFile_v3(FILE *file, polyPointer poly); //다항식 출력(연결리스트 버전)

void attach_link(int coefficient, int exponent, polyPointer *p); //다항식 항 추가(연결리스트 버전)

void freePoly(polyPointer poly); //연결리스트 메모리 해제

// 수행 시간 측정 함수
double getElapsedTime(LARGE_INTEGER start, LARGE_INTEGER end, LARGE_INTEGER frequency) {
    return (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
}


int main() 
{
    LARGE_INTEGER frequency, start1, end1, start2, end2, start3, end3; // 고해상도 타이머 변수
    QueryPerformanceFrequency(&frequency);
    int size_FirstPoly = 0; // 첫 번째 다항식의 항 개수
    int size_SecondPoly = 0; // 두 번째 다항식의 항 개수
    FILE *file = fopen("input.txt", "r"); // 파일 열기
    if (file == NULL) {
        perror("파일을 열 수 없습니다.");
        return 1;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 초기버전
    QueryPerformanceCounter(&start1); // 시작 시간 측정
    Polynomial FirstPoly = {0, NULL};
    Polynomial SecondPoly = {0, NULL};
    fscanf(file, "%d %d", &size_FirstPoly, &size_SecondPoly); //두 다항식의 항 개수 읽기
    for(int i = 0, tmp = 0; i < size_FirstPoly; i++) { // 첫 번째 다항식 최대 차수 읽기
        fscanf(file, "%*d %d", &tmp);
        if (tmp > FirstPoly.degree)
            FirstPoly.degree = tmp;
    }
    for(int i = 0, tmp = 0; i < size_SecondPoly; i++) { // 두 번째 다항식 최대 차수 읽기
        fscanf(file, "%*d %d", &tmp);
        if (tmp > SecondPoly.degree)
            SecondPoly.degree = tmp;
    }
    FirstPoly.coef = (int *)calloc((FirstPoly.degree + 1), sizeof(int)); // 첫 번째 다항식 동적 메모리 할당
    SecondPoly.coef = (int *)calloc((SecondPoly.degree + 1), sizeof(int)); // 두 번째 다항식 동적 메모리 할당
    rewind(file); // 파일 포인터를 처음으로 되돌리기
    fscanf(file, "%*d %*d"); 
    for(int coef, expon, i = 0; i < size_FirstPoly; i++) {
        fscanf(file, "%d %d", &coef, &expon); // 첫 번째 다항식의 차수별 계수 읽기
        FirstPoly.coef[FirstPoly.degree-expon] = coef; // 해당 차수에 계수 저장
    }
    for(int coef, expon, i = 0; i < size_SecondPoly; i++) {
        fscanf(file, "%d %d", &coef, &expon); // 두 번째 다항식의 차수별 계수 읽기
        SecondPoly.coef[SecondPoly.degree-expon] = coef; // 해당 차수에 계수 저장
    }
    Polynomial result = padd_ver1(FirstPoly, SecondPoly); // 다항식 덧셈 수행
    QueryPerformanceCounter(&end1); // 종료 시간 측정
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
    rewind(file); // 파일 포인터를 처음으로 되돌리기

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 개선된 버전
    QueryPerformanceCounter(&start2); // 시작 시간 측정
    terms = (Term *)malloc(2 * (size_FirstPoly + size_SecondPoly) * sizeof(Term)); // 최악의 경우를 가정한 동적 메모리 할당
    avail = size_FirstPoly + size_SecondPoly; // 사용 가능한 위치 초기화
    fscanf(file, "%*d %*d"); 

    // 두 다항식의 계수, 차수 입력 
    for(int i=0; i < size_FirstPoly; i++) {
        fscanf(file, "%d %d", &terms[i].coef, &terms[i].expon);
    }
    for(int i=0; i < size_SecondPoly; i++) {
        fscanf(file, "%d %d", &terms[i + size_FirstPoly].coef, &terms[i + size_FirstPoly].expon);
    }
    qsort(terms, size_FirstPoly, sizeof(Term), compare); // 첫 번째 다항식 정렬
    qsort(terms + size_FirstPoly, size_SecondPoly, sizeof(Term), compare); // 두 번째 다항식 정렬
    int startResult, finishResult; // 결과 다항식의 시작과 끝 위치
    padd_ver2(0, size_FirstPoly - 1, size_FirstPoly, size_FirstPoly + size_SecondPoly - 1, &startResult, &finishResult); // 개선된 다항식 덧셈 수행
    QueryPerformanceCounter(&end2); // 종료 시간 측정
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    rewind(file); // 파일 포인터를 처음으로 되돌리기

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 연결리스트 버전
    QueryPerformanceCounter(&start3); // 시작 시간 측정
    polyPointer FirstPoly_v3, SecondPoly_v3, result_v3;
    FirstPoly_v3 = (polyPointer)malloc(sizeof(struct polyNode)); 
    SecondPoly_v3 = (polyPointer)malloc(sizeof(struct polyNode)); 
    FirstPoly_v3->link = NULL; 
    SecondPoly_v3->link = NULL; 
    fscanf(file, "%*d %*d");
    // 첫 번째 다항식 읽기 및 정렬
    Term *firstTerms = (Term *)malloc(size_FirstPoly * sizeof(Term));
    for (int i = 0; i < size_FirstPoly; i++) {
        fscanf(file, "%d %d", &firstTerms[i].coef, &firstTerms[i].expon);
    }
    qsort(firstTerms, size_FirstPoly, sizeof(Term), compare); // 첫 번째 다항식 정렬
    FirstPoly_v3 = createLinkedListFromTerms(firstTerms, size_FirstPoly);
    free(firstTerms); // 배열 메모리 해제

    // 두 번째 다항식 읽기 및 정렬
    Term *secondTerms = (Term *)malloc(size_SecondPoly * sizeof(Term));
    for (int i = 0; i < size_SecondPoly; i++) {
        fscanf(file, "%d %d", &secondTerms[i].coef, &secondTerms[i].expon);
    }
    qsort(secondTerms, size_SecondPoly, sizeof(Term), compare); // 두 번째 다항식 정렬
    SecondPoly_v3 = createLinkedListFromTerms(secondTerms, size_SecondPoly);
    free(secondTerms); // 배열 메모리 해제
    result_v3 = padd_ver3(FirstPoly_v3, SecondPoly_v3); // 연결리스트 다항식 덧셈 수행
    QueryPerformanceCounter(&end3); // 종료 시간 측정
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    fclose(file); // 파일 닫기

    FILE *outputFile = fopen("output.txt", "w");
    if (outputFile == NULL) {
        perror("결과 파일을 열 수 없습니다.");
        return 1;
    }

    // 초기 버전 다항식 출력
    WritePolynomialToFile_v1(outputFile, FirstPoly);
    WritePolynomialToFile_v1(outputFile, SecondPoly);
    WritePolynomialToFile_v1(outputFile, result);

    // 개선된 버전 다항식 출력
    WritePolynomialToFile_v2(outputFile, 0, size_FirstPoly-1);
    WritePolynomialToFile_v2(outputFile, size_FirstPoly, size_FirstPoly + size_SecondPoly - 1);
    WritePolynomialToFile_v2(outputFile, startResult, finishResult);

    // 연결리스트 버전 다항식 출력
    WritePolynomialToFile_v3(outputFile, FirstPoly_v3);
    WritePolynomialToFile_v3(outputFile, SecondPoly_v3);
    WritePolynomialToFile_v3(outputFile, result_v3);
    

    // 수행 시간 출력
    fprintf(outputFile, "%lf\t%lf\t%lf\n", 
        getElapsedTime(start1, end1, frequency), getElapsedTime(start2, end2, frequency), getElapsedTime(start3, end3, frequency)); // 수행 시간 출력

    fclose(outputFile);

    // 동적 메모리 해제
    free(FirstPoly.coef);
    free(SecondPoly.coef);
    free(result.coef);
    free(terms);
    freePoly(FirstPoly_v3); // 연결리스트 메모리 해제
    freePoly(SecondPoly_v3); // 연결리스트 메모리 해제
    freePoly(result_v3); // 연결리스트 메모리 해제
    result.coef = NULL;
    FirstPoly.coef = NULL;
    SecondPoly.coef = NULL;
    FirstPoly_v3 = NULL;
    SecondPoly_v3 = NULL;
    result_v3 = NULL;


    return 0;
}

//다항식 a, b 각각의 최대 차수만큼 반복하여 d에 계수 덧셈
Polynomial padd_ver1(Polynomial a, Polynomial b) {
    int maxDegree = (a.degree > b.degree) ? a.degree : b.degree; // 결과 다항식의 최대 차수
    Polynomial d = {maxDegree, NULL};
    d.coef = (int *)calloc((maxDegree + 1), sizeof(int));

    // a의 항 추가
    for (int i = 0; i <= a.degree; i++) {
        if (a.coef[i] != 0) { // 0 계수는 무시
            d.coef[maxDegree - (a.degree - i)] += a.coef[i];
        }
    }

    // b의 항 추가
    for (int i = 0; i <= b.degree; i++) {
        if (b.coef[i] != 0) { // 0 계수는 무시
            d.coef[maxDegree - (b.degree - i)] += b.coef[i];
        }
    }
    return d;
}

void WritePolynomialToFile_v1(FILE *file, Polynomial poly) {
    bool isFirstTerm = true;
    for (int i = 0; i <= poly.degree; i++) {
        if (poly.coef[i] != 0) {
            if (!isFirstTerm) {
                fprintf(file, " + ");
            }
            isFirstTerm = false;
            fprintf(file, "%dx^%d", poly.coef[i], poly.degree - i);
        }
    }
    fprintf(file, "\n");
}

void padd_ver2(int startA, int finishA, int startB, int finishB, int *startD, int *finishD) {
    *startD = avail; // 결과 다항식의 시작 위치
    while (startA <= finishA && startB <= finishB) {
        switch (COMPARE(terms[startA].expon, terms[startB].expon)) {
            case -1: // B의 차수가 더 클 때
                attach(terms[startB].coef, terms[startB].expon);
                startB++;
                break;
            case 0: // A와 B의 차수가 같을 때
                if (terms[startA].coef + terms[startB].coef != 0) { // 계수 합이 0이 아닐 때만 추가
                    attach(terms[startA].coef + terms[startB].coef, terms[startA].expon);
                }
                startA++;
                startB++;
                break;
            case 1: // A의 차수가 더 클 때
                attach(terms[startA].coef, terms[startA].expon);
                startA++;
                break;
        }
    }

    // 남아 있는 항 추가
    while (startA <= finishA) {
        attach(terms[startA].coef, terms[startA].expon);
        startA++;
    }
    while (startB <= finishB) {
        attach(terms[startB].coef, terms[startB].expon);
        startB++;
    }

    *finishD = avail - 1; // 결과 다항식의 끝 위치
}

void attach(int coefficient, int exponent) {
    terms[avail].coef = coefficient;
    terms[avail].expon = exponent;
    avail++;
}

void WritePolynomialToFile_v2(FILE *file, int start, int finish) {
    bool isFirstTerm = true;
    for (int i = start; i <= finish; i++) {
        if (terms[i].coef != 0) {
            if (!isFirstTerm) {
                fprintf(file, " + ");
            }
            isFirstTerm = false;
            fprintf(file, "%dx^%d", terms[i].coef, terms[i].expon);
        }
    }
    fprintf(file, "\n");
}

int compare(const void *a, const void *b) {
    Term *termA = (Term *)a;
    Term *termB = (Term *)b;
    return COMPARE(termB->expon, termA->expon); // 내림차순 비교
}

polyPointer createLinkedListFromTerms(Term *terms, int size) {
    polyPointer head = NULL, rear = NULL;

    for (int i = 0; i < size; i++) {
        polyPointer temp = (polyPointer)malloc(sizeof(struct polyNode));
        temp->coef = terms[i].coef;
        temp->expon = terms[i].expon;
        temp->link = NULL;

        if (head == NULL) {
            head = temp;
            rear = temp;
        } else {
            rear->link = temp;
            rear = temp;
        }
    }
    return head;
}

polyPointer padd_ver3(polyPointer a, polyPointer b){
    polyPointer c, rear, temp;
    rear = (polyPointer)malloc(sizeof(struct polyNode));
    c = rear; // 결과 다항식의 헤드
    int sum = 0;
    while(a != NULL && b != NULL){
        switch(COMPARE(a->expon, b->expon)){
            case -1: // B의 차수가 더 클 때
                attach_link(b->coef, b->expon, &rear); // 새로운 항 추가
                b = b->link;
                break;
            case 0: // A와 B의 차수가 같을 때
                sum = a->coef + b->coef; // 계수 합
                if(sum != 0){ // 계수 합이 0이 아닐 때만 추가
                    attach_link(sum, a->expon, &rear); // 새로운 항 추가
                }
                a = a->link;
                b = b->link;
                break;
            case 1: // A의 차수가 더 클 때
                attach_link(a->coef, a->expon, &rear); // 새로운 항 추가
                a = a->link;
                break;
        }
    }
    while(a){ // A에 남아 있는 항 추가
        attach_link(a->coef, a->expon, &rear); // 새로운 항 추가
        a = a->link;
    }
    while(b){ // B에 남아 있는 항 추가
        attach_link(b->coef, b->expon, &rear); // 새로운 항 추가
        b = b->link;
    }
    rear->link = NULL;
    temp = c;
    c = c->link;
    free(temp);
    return c; // 결과 다항식 반환
}

void attach_link(int coefficient, int exponent, polyPointer *p) {
    polyPointer temp = malloc(sizeof(struct polyNode)); // 새로운 노드 생성
    temp->coef = coefficient; // 계수 저장
    temp->expon = exponent; // 차수 저장
    temp->link = NULL; // 링크 초기화
    (*p)->link = temp; // 새로운 항 연결
    *p = temp; // 포인터 이동
}

void WritePolynomialToFile_v3(FILE *file, polyPointer poly) {
    bool isFirstTerm = true;
    while (poly != NULL) {
        if (poly->coef != 0) {
            if (!isFirstTerm) {
                fprintf(file, " + ");
            }
            isFirstTerm = false;
            fprintf(file, "%dx^%d", poly->coef, poly->expon);
        }
        poly = poly->link; // 다음 항으로 이동
    }
    fprintf(file, "\n");
}

void freePoly(polyPointer poly) {
    polyPointer temp;
    while (poly != NULL) {
        temp = poly;
        poly = poly->link; // 다음 노드로 이동
        free(temp); // 현재 노드 메모리 해제
    }
}
