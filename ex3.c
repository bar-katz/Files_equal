#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>

#define ERROR "Error in system call"
#define SPECIALS " \n"

int isCharIn(char c, const char *specials, size_t size) {
  for (int i = 0; i < size; i++) {
    if (c == specials[i]) {
      return 1;
    }
  }
  return 0;
}

int max(int x, int y) {
  if (x > y) {
    return x;
  }
  return y;
}

int numOfLetters(char *buffer, int size) {
  int count = 0;
  for (int i = 0; i < size; i++) {
    if (!isCharIn(buffer[i], SPECIALS, strlen(SPECIALS))) {
      count++;
    }
  }
  return count;
}

int checkEqual(char *buffer1,
               int buffer1_size,
               char *buffer2,
               int buffer2_size) {

  int buffer1_offset = 0; // used to skip special letters for buffer1
  int buffer2_offset = 0; // used to skip special letters for buffer2
  int similar_flag = 0;   // flag for similar buffers

  // get number of letters different than space and '\n' in each buffer
  int buffer1_letters = numOfLetters(buffer1, buffer1_size);
  int buffer2_letters = numOfLetters(buffer2, buffer2_size);

  if (buffer1_letters != buffer2_letters) {
    // number of letters differ
    return 1;
  }

  int numOfIter = max(buffer1_letters, buffer2_letters);

  for (int i = 0; i < numOfIter; i++) {
    int if1 = i + buffer1_offset;
    int if2 = i + buffer2_offset;

    if (if1 >= buffer1_size || if2 >= buffer2_size) {
      // one of the buffer got to the end of its content
      return 1;
    }

    if (buffer1[if1] != buffer2[if2]) {
      // if chars differ check if they are the same letter
      if (((buffer1[if1] >= 'A' && buffer1[if1] <= 'Z') &&
          (buffer1[if1] + 32 == buffer2[if2]))
          ||
              ((buffer1[if1] >= 'a' && buffer1[if1] <= 'z') &&
                  (buffer1[if1] - 32 == buffer2[if2]))) {
        // if chars are the same letter turn on flag
        similar_flag = 1;
        continue;

      } else if (isCharIn(buffer1[if1], SPECIALS, strlen(SPECIALS))) {
        // if file1_buffer[if1] is space or '\n'
        buffer1_offset++;
        i--;
        similar_flag = 1;
        continue;

      } else if (isCharIn(buffer2[if2], SPECIALS, strlen(SPECIALS))) {
        // if file2_buffer[if2] is space or '\n'
        buffer2_offset++;
        i--;
        similar_flag = 1;
        continue;

      } else {
        // content of files differ -- return 1
        return 1;
      }
    }
  }

  // content of files is similar -- return 2
  if (similar_flag) {
    return 2;
  }

  // content of files is the same -- return 3
  return 3;
}

int main(int argc, char *argv[]) {

  char *file1 = argv[1];
  char *file2 = argv[2];

  // files descriptors
  int file1_fd;
  int file2_fd;

  struct stat file1_stat;
  struct stat file2_stat;

  // get files descriptors
  if ((file1_fd = open(file1, O_RDONLY)) == -1 ||
      (file2_fd = open(file2, O_RDONLY)) == -1) {
    write(2, ERROR, strlen(ERROR));
    return 4;
  }

  // get stat of files
  if (stat(file1, &file1_stat) == -1 ||
      stat(file2, &file2_stat) == -1) {
    write(2, ERROR, strlen(ERROR));
    return 4;
  }

  // buffer for content of files
  char file1_buffer[file1_stat.st_size];
  char file2_buffer[file2_stat.st_size];

  // read files content
  if (read(file1_fd, file1_buffer, file1_stat.st_size) == -1 ||
      read(file2_fd, file2_buffer, file2_stat.st_size) == -1) {
    write(2, ERROR, strlen(ERROR));
    return 4;
  }

  // close files
  close(file1_fd);
  close(file2_fd);

  return checkEqual(file1_buffer, file1_stat.st_size,
                    file2_buffer, file2_stat.st_size);
}
