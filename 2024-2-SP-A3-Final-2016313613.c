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


unsigned int hash_words(const char *word1, const char *word2) {
    unsigned int hashValue = 0;

    // 첫 번째 단어를 해싱
    while (*word1) {
        hashValue = (hashValue * 31 + *word1) % TABLE_SIZE;
        word1++;
    }

    // 두 번째 단어를 해싱
    while (*word2) {
        hashValue = (hashValue * 31 + *word2) % TABLE_SIZE;
        word2++;
    }

    return hashValue;
}

typedef struct Node {
    char *word1;
    char *word2;
    int count;
    struct Node *next;
} Node;

void initialize_bucket(Node *bucket[], int size) {
    for (int i = 0; i < size; i++) {
        bucket[i] = NULL;
    }
}

void insert_bigram(Node *bucket[], const char *word1, const char *word2) {
    unsigned int hashIndex = hash_words(word1, word2);
    // 해시 테이블에서 해당 인덱스의 연결 리스트 검색
    Node *current = bucket[hashIndex];
    while (current) {
        if (strcmp(current->word1, word1) == 0 && strcmp(current->word2, word2) == 0) {
            current->count++;
            return;
        }
        current = current->next;
    }
    // 새로운 노드 추가
    Node *newNode = malloc(sizeof(Node));
    newNode->word1 = strdup(word1);
    newNode->word2 = strdup(word2);
    newNode->count = 1;
    newNode->next = bucket[hashIndex];
    bucket[hashIndex] = newNode;
}

void free_bucket(Node *bucket[], int size) {
    for (int i = 0; i < size; i++) {
        Node *current = bucket[i];
        while (current) {
            Node *temp = current;
            current = current->next;
            free(temp->word1);
            free(temp->word2);
            free(temp);
        }
    }
}
int flat_bucket(Node *bucket[], int size, Node **nodeArray) {
    int index = 0;
    for (int i = 0; i < size; i++) {
        Node *current = bucket[i];
        while (current) {
            nodeArray[index++] = current;
            current = current->next;
        }
    }
    return index; // 배열에 추가된 노드의 총 개수 반환
}

int compare_by_count(const void *a, const void *b) {
    Node *nodeA = *(Node **) a;
    Node *nodeB = *(Node **) b;

    // count 내림차순 정렬
    return nodeB->count - nodeA->count;
}
void insertion_sort(Node **nodeArray, int nodeCount) {
    for (int i = 1; i < nodeCount; i++) {
        Node *key = nodeArray[i];
        int j = i - 1;

        // count 내림차순 기준으로 정렬
        while (j >= 0 && nodeArray[j]->count < key->count) {
            nodeArray[j + 1] = nodeArray[j];
            j--;
        }
        nodeArray[j + 1] = key;
    }
}
void main() {
    int wordCount;
    char **words = read_txt("shakespeare.txt", &wordCount);

    Node *bucket[TABLE_SIZE];
    initialize_bucket(bucket, TABLE_SIZE);
    for (int i = 0; i < wordCount - 1; i++) {
        // wordCount - 1: 마지막 단어는 다음 단어가 없음
        insert_bigram(bucket, words[i], words[i + 1]);
    }

    Node **nodeArray = malloc(MAX_WORDS_CAPACITY * sizeof(Node *));
    int nodeCount = flat_bucket(bucket, TABLE_SIZE, nodeArray);

    // count 내림차순 정렬
    qsort(nodeArray, nodeCount, sizeof(Node *), compare_by_count);

    // 정렬된 결과 출력 (상위 10개만 출력)
    printf("Top 10 Bigrams:\n");
    for (int i = 0; i < (nodeCount < 10 ? nodeCount : 10); i++) {
        printf("Bigram: (%s, %s), Count: %d\n",
               nodeArray[i]->word1, nodeArray[i]->word2, nodeArray[i]->count);
    }

    // 메모리 해제
    for (int i = 0; i < wordCount; i++) {
        free(words[i]);
    }
    free(words);
    free(nodeArray);
    free_bucket(bucket, TABLE_SIZE);
}
