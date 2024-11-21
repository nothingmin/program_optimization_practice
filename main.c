#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORDS_CAPACITY 1000000  // 초기 단어 배열 크기
#define MAX_WORD_LENGTH 100         // 단어의 최대 길이

// 특수문자를 제거한 새로운 문자열 생성
char *remove_special_characters(const char *input) {
    int length = strlen(input);
    char *output = malloc((length + 1) * sizeof(char)); // +1은 널 문자 공간

    int j = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        if (isalnum(input[i])) {
            // 알파벳(a-z, A-Z) 또는 숫자(0-9)인지 확인
            output[j++] = input[i];
        }
    }
    output[j] = '\0'; // 문자열 종료 문자 추가

    return output;
}

// 파일을 읽어 단어를 동적 배열에 저장하는 함수
char **read_txt(const char *filename, int *wordCount) {
    // 파일 열기
    FILE *file = fopen(filename, "r");

    char **words = malloc(MAX_WORDS_CAPACITY * sizeof(char *));
    char word[MAX_WORD_LENGTH];
    *wordCount = 0;

    while (fscanf(file, "%99s", word) == 1) {
        // 특수문자 제거
        char *clean_word = remove_special_characters(word);

        // 빈 단어는 무시
        if (strlen(clean_word) == 0) {
            free(clean_word);
            continue;
        }


        // 단어를 동적으로 메모리 할당하여 저장
        words[*wordCount] = malloc(strlen(clean_word) + 1);
        strcpy(words[*wordCount], clean_word);
        free(clean_word); // `clean_word`는 복사 후 해제
        (*wordCount)++;
    }

    fclose(file); // 파일 닫기
    return words; // 읽은 단어 배열 반환
}

void main() {
    int wordCount;
    char **words = read_txt("example.txt", &wordCount);
    printf("%d\n",  wordCount );
    for (int i = 0; i < wordCount; i++) {
        printf("%s\n", words[i]);
    }
}
