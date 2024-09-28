#include <md5.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MD5_LEN 16
#define MAX_LINE_LENGTH 1024
#define MAX_THREADS 4
#define INITIAL_CAPACITY 1024

typedef struct {
    const char *target_hash;
    char **lines;
    int start_idx;
    int end_idx;
    bool *found;
    pthread_mutex_t *mutex;
} ThreadData;

void md5_to_string(uint8_t *md5_hash, char *output) {
    for (int i = 0; i < MD5_LEN; ++i) {
        sprintf(&output[i * 2], "%02x", md5_hash[i]);
    }
    output[32] = '\0';
}

void *thread_brute_md5(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    uint8_t md5_result[16];
    char md5_string[33];
    for (int i = data->start_idx; i < data->end_idx; ++i) {
        pthread_mutex_lock(data->mutex);
        if (*data->found) {
            pthread_mutex_unlock(data->mutex);
            return NULL;
        }
        pthread_mutex_unlock(data->mutex);

        md5((uint8_t *)data->lines[i], strlen(data->lines[i]), md5_result);
        md5_to_string(md5_result, md5_string);

        if (strcmp(md5_string, data->target_hash) == 0) {
            pthread_mutex_lock(data->mutex);
            *data->found = true;
            pthread_mutex_unlock(data->mutex);
            return strdup(data->lines[i]);
        }
    }
    return NULL;
}

char **load_dict(const char *dict_file, int *line_count) {
    FILE *file = fopen(dict_file, "r");
    if (!file) {
        perror("Could not open file");
        return NULL;
    }

    char **lines = NULL;
    int capacity = INITIAL_CAPACITY;
    int count = 0;
    char line[MAX_LINE_LENGTH];

    lines = malloc(sizeof(char *) * capacity);
    if (!lines) {
        perror("Memory allocation error for strings");
        fclose(file);
        return NULL;
    }

    while (fgets(line, sizeof(line), file)) {
        if (count >= capacity) {
            capacity *= 2;
            char **new_lines = realloc(lines, sizeof(char *) * capacity);
            if (!new_lines) {
                perror("Memory reallocation error");
                for (int j = 0; j < count; ++j) {
                    free(lines[j]);
                }
                free(lines);
                fclose(file);
                return NULL;
            }
            lines = new_lines;
        }

        line[strcspn(line, "\n")] = '\0';
        lines[count] = strdup(line);
        if (!lines[count]) {
            perror("Memory allocation error for string");
            for (int j = 0; j < count; ++j) {
                free(lines[j]);
            }
            free(lines);
            fclose(file);
            return NULL;
        }
        count++;
    }

    fclose(file);
    *line_count = count;

    return lines;
}

void cleanup(char **lines, int line_count, pthread_t *threads, ThreadData *thread_data) {
    if (lines) {
        for (int i = 0; i < line_count; i++) {
            free(lines[i]);
        }
        free(lines);
    }
    free(threads);
    free(thread_data);
}

char *brute_force_md5(const char *target_hash, const char *dict_file) {
    int line_count;
    char **lines = load_dict(dict_file, &line_count);
    if (!lines) {
        fprintf(stderr, "Could not load dictionary\n");
        return NULL;
    }

    pthread_t *threads = malloc(sizeof(pthread_t) * MAX_THREADS);
    ThreadData *thread_data = malloc(sizeof(ThreadData) * MAX_THREADS);
    if (!threads || !thread_data) {
        perror("Memory allocation error for threads");
        cleanup(lines, line_count, threads, thread_data);
        return NULL;
    }

    char *result = NULL;
    bool found = false;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    int lines_per_thread = line_count / MAX_THREADS;

    for (int i = 0; i < MAX_THREADS; i++) {
        thread_data[i].target_hash = target_hash;
        thread_data[i].lines = lines;
        thread_data[i].start_idx = i * lines_per_thread;
        thread_data[i].end_idx = (i == MAX_THREADS - 1) ? line_count : (i + 1) * lines_per_thread;
        thread_data[i].found = &found;
        thread_data[i].mutex = &mutex;

        if (pthread_create(&threads[i], NULL, thread_brute_md5, &thread_data[i]) != 0) {
            perror("Failed to create thread\n");
            cleanup(lines, line_count, threads, thread_data);
            return NULL;
        }
    }

    for (int i = 0; i < MAX_THREADS; i++) {
        void *thread_result;
        pthread_join(threads[i], &thread_result);

        if (thread_result != NULL) {
            result = (char *)thread_result;
        }
    }

    cleanup(lines, line_count, threads, thread_data);
    pthread_mutex_destroy(&mutex);

    return result;
}
