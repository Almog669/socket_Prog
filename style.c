#include<stdio.h>

void changeTextblue(){
   printf("\033[0;34m");
}

void changeTextRed(){
    printf("\033[0;31m");
}

void changeTextItalic(){
    printf("\033[3m\033[38;5;208m");
    
}