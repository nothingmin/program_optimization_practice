#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORDS_CAPACITY 1000000  // 초기 단어 배열 크기
#define MAX_WORD_LENGTH 100         // 단어의 최대 길이
#define TABLE_SIZE 10007 // 해시 테이블 크기 (소수 사용 권장)

// 특수문자를 제거, 소문자 변환
char *remove_special_characters_and_lower(const char *input) {
    size_t length = strlen(input);
    char *output = malloc((length + 1) * sizeof(char)); // +1은 널 문자 공간

    int j = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        if (isalnum(input[i])) {
            // 알파벳(a-z, A-Z) 또는 숫자(0-9)인지 확인
            output[j++] = tolower(input[i]);
        }
    }
    output[j] = '\0';

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
        char *clean_word = remove_special_characters_and_lower(word);

        // 빈 단어는 무시
        if (strlen(clean_word) == 0) {
            free(clean_word);
            continue;
        }

        words[*wordCount] = malloc(strlen(clean_word) + 1);
        strcpy(words[*wordCount], clean_word);
        free(clean_word);
        (*wordCount)++;
    }

    fclose(file);
    return words;
}


// 해시 함수: 단어를 정수로 변환
unsigned int hash_word(const char *word) {
    unsigned int hashValue = 0;
    while (*word) {
        hashValue = (hashValue * 31 + *word) % TABLE_SIZE; // 31은 일반적으로 사용되는 해싱 상수
        word++;
    }
    return hashValue;
}

struct Node {
    char *word;
    int count;
    struct Node *next;
};

void main() {
    int wordCount;
    char **words = read_txt("shakespeare.txt", &wordCount);
    for (int i = 0; i < wordCount; i++) {
        printf("%s\n", words[i]);
        free(words[i]);
    }
    free(words);
}
