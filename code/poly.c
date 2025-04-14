#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct 
{
    int coef;
    int *expon;
} Polynomial;

Polynomial Zero();
bool IsZero(Polynomial poly);
int Coef(Polynomial poly, int expon);
int Lead_Exp(Polynomial poly);
Polynomial Add(Polynomial poly1, Polynomial poly2);
Polynomial Attatch(Polynomial poly, int coef, int expon);
Polynomial Remove(Polynomial poly, int expon);

int main() 
{
    int size_FirstPoly = 0;
    int size_SecondPoly = 0;
    Polynomial FirstPoly;
    Polynomial SecondPoly;
    FILE *file = fopen("input.txt", "r"); // 파일 열기
    if (file == NULL) {
        perror("파일을 열 수 없습니다.");
        return 1;
    }
    fscanf(file, "%d %d", &size_FirstPoly, &size_SecondPoly); //두 다항식의 항 개수 읽기

    FirstPoly.expon = malloc(size_FirstPoly * sizeof(int)); // 동적 메모리 할당
    SecondPoly.expon = malloc(size_FirstPoly * sizeof(int)); // 동적 메모리 할당
    
    for(int i = 0; i < size_FirstPoly; i++) {
        fscanf(file, "%d %d", &FirstPoly.coef, &FirstPoly.expon[i]); // 첫 번째 다항식의 차수별 계수 읽기
    }
    for(int i = 0; i < size_SecondPoly; i++) {
        fscanf(file, "%d %d", &SecondPoly.coef, &SecondPoly.expon[i]); // 두 번째 다항식의 차수별 계수 읽기
    }
    fclose(file); // 파일 닫기

    Polynomial result = Zero(); // 결과 다항식 초기화

    // 동적 메모리 해제
    free(FirstPoly.expon);
    free(SecondPoly.expon);

    return 0;
}

Polynomial Zero() 
{
    Polynomial poly = {0, 0};
    return poly;
}
bool IsZero(Polynomial poly) 
{
    return (poly.coef == 0 && poly.expon == 0);
}
int Coef(Polynomial poly, int expon) 
{
    if (poly.expon == expon) 
        return poly.coef;
    else 
        return 0;
}
int Lead_Exp(Polynomial poly) 
{
    return poly.expon;
}
Polynomial Add(Polynomial poly1, Polynomial poly2) 
{
    Polynomial result = {0, 0};
    if (poly1.expon == poly2.expon) 
    {
        result.coef = poly1.coef + poly2.coef;
        result.expon = poly1.expon;
    } 
    else if (poly1.expon > poly2.expon) 
    {
        result.coef = poly1.coef;
        result.expon = poly1.expon;
    } 
    else 
    {
        result.coef = poly2.coef;
        result.expon = poly2.expon;
    }
    return result;
}
Polynomial Attatch(Polynomial poly, int coef, int expon) 
{
    Polynomial newPoly = {coef, expon};
    return newPoly;
}
Polynomial Remove(Polynomial poly, int expon) 
{
    Polynomial newPoly = {0, 0};
    if (poly.expon == expon) 
    {
        newPoly.coef = 0;
        newPoly.expon = 0;
    } 
    else 
    {
        newPoly.coef = poly.coef;
        newPoly.expon = poly.expon;
    }
    return newPoly;
}