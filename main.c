#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096
#define SO_EOF -1

typedef struct
{
    int fd;
    char buffer[BUFFER_SIZE];
    int buf_pos;
    int buf_end;
    char mode[3];
    int eof;
    int error;
} SO_FILE;

int so_fflush(SO_FILE *stream)
{
    if (!stream)
    {
        stream->eof = 1;
        return SO_EOF; // Invalid arg
    }

    // exista date in buffer
    int bytesToWrite = stream->buf_pos;
    
    if (bytesToWrite > 0)
    {
        int written = write(stream->fd, stream->buffer, bytesToWrite);
        
        if (written < bytesToWrite)
        {
            // nu au fost scrise toate date;e
            stream->error = 1;
            return SO_EOF;
        }
        stream->buf_pos = 0; // reset
    }

    return 0; // ok
}

SO_FILE *so_fopen(const char *pathname, const char *mode)
{
    int fd;
    int flags = 0;

    if (strcmp(mode, "r") == 0)
    {
        flags = O_RDONLY;
    }
    else if (strcmp(mode, "r+") == 0)
    {
        flags = O_RDWR;
    }
    else if (strcmp(mode, "w") == 0)
    {
        flags = O_WRONLY | O_CREAT | O_TRUNC;
    }
    else if (strcmp(mode, "w+") == 0)
    {
        flags = O_RDWR | O_CREAT | O_TRUNC;
    }
    else if (strcmp(mode, "a") == 0)
    {
        flags = O_WRONLY | O_CREAT | O_APPEND;
    }
    else if (strcmp(mode, "a+") == 0)
    {
        flags = O_RDWR | O_CREAT | O_APPEND;
    }
    else
    {
        return NULL;
    }

    fd = open(pathname, flags, 0777);
    
    if (fd == -1)
    {
        perror("Eroare la deschiderea fisierului");
        return NULL;
    }

    SO_FILE *soFile = (SO_FILE *)malloc(sizeof(SO_FILE));
    
    if (!soFile)
    {
        close(fd);
        return NULL;
    }

    soFile->fd = fd;
    soFile->buf_pos = 0;
    soFile->buf_end = 0;
    strncpy(soFile->mode, mode, sizeof(soFile->mode) - 1);
    soFile->mode[sizeof(soFile->mode) - 1] = '\0';
    soFile->eof = 0;
    soFile->error = 0;

    return soFile;
}

int so_fclose(SO_FILE *stream)
{
    if (!stream)
    {
        return SO_EOF; // Invalid arg
    }

    so_fflush(stream);
    close(stream->fd);
    free(stream);

    return 0;
}

int so_fgetc(SO_FILE *stream)
{
    // 1. verif buffer date
    if (stream->buf_pos >= stream->buf_end)
    {
        // 2. nu, incarcam date
        int bytesRead = read(stream->fd, stream->buffer, BUFFER_SIZE);
        
        if (bytesRead == 0)
        {
            // sf fisier
            stream->eof = 1;
            return SO_EOF;
        }
        else if (bytesRead < 0)
        {
            // err
            stream->error = 1;
            return SO_EOF;
        }

        stream->buf_pos = 0;
        stream->buf_end = bytesRead;
    }

    return (unsigned char)stream->buffer[stream->buf_pos++];
}

int so_fputc(int c, SO_FILE *stream)
{
    if (!stream)
    {
        return SO_EOF;
    }

    stream->buffer[stream->buf_pos++] = (char)c;

    if (stream->buf_pos >= BUFFER_SIZE)
    {
        int bytesWritten = write(stream->fd, stream->buffer, BUFFER_SIZE);
        
        if (bytesWritten != BUFFER_SIZE)
        {
            stream->error = 1;
            return SO_EOF;
        }
        stream->buf_pos = 0; // reset
    }
    return (unsigned char)c;
}

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
    char *char_ptr = (char *)ptr;
    size_t totalBytesToRead = size * nmemb;
    size_t bytesRead = 0;

    for (size_t i = 0; i < totalBytesToRead; i++)
    {
        int ch = so_fgetc(stream);
        
        if (ch == SO_EOF)
        {
            return bytesRead / size;
            break;
        }
        char_ptr[i] = ch;
        bytesRead++;
    }

    return bytesRead / size;
}

size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
    const char *char_ptr = (const char *)ptr;
    size_t totalBytesToWrite = size * nmemb;
    size_t bytesWritten = 0;

    for (size_t i = 0; i < totalBytesToWrite; i++)
    {
        if (so_fputc(char_ptr[i], stream) == SO_EOF)
        {
            return bytesWritten / size;
            break;
        }
        bytesWritten++;
    }

    return bytesWritten / size;
}

int so_fseek(SO_FILE *stream, long offset, int whence)
{
    if (!stream)
    {
        stream->eof = 1;
        return SO_EOF;
    }

    if (stream->buf_end - stream->buf_pos > 0)
    {
        so_fflush(stream);
    }

    stream->buf_pos = 0;
    stream->buf_end = 0;

    if (lseek(stream->fd, offset, whence) == -1)
    {
        stream->error = 1;
        return SO_EOF;
    }

    return 0;
}

long so_ftell(SO_FILE *stream)
{
    if (!stream)
    {
        stream->eof = 1;
        return SO_EOF;
    }

    long position = lseek(stream->fd, 0, SEEK_CUR);
    
    if (position == -1)
    {
        stream->error = 1;
        return SO_EOF;
    }

    position += (stream->buf_pos - stream->buf_end);

    return position;
}

int so_fileno(SO_FILE *stream)
{
    if (!stream)
    {
        return -1;
    }
    return stream->fd;
}

int so_feof(SO_FILE *stream)
{
    if (!stream)
    {
        return 0;
    }
    return stream->eof;
}

int so_ferror(SO_FILE *stream)
{
    if (!stream)
    {
        return 0;
    }
    return stream->error;
}

SO_FILE *so_popen(const char *command, const char *type)
{
    int pfd[2];
    pid_t pid;

    if (pipe(pfd) == -1)
    {
        return NULL;
    }

    pid = fork();
    
    if (pid == -1)
    {
        close(pfd[0]);
        close(pfd[1]);
        return NULL;
    }

    if (pid == 0)
    { // copil
        if (strcmp(type, "r") == 0)
        {
            close(pfd[0]);
            dup2(pfd[1], STDOUT_FILENO);
        }
        else if (strcmp(type, "w") == 0)
        {
            close(pfd[1]);
            dup2(pfd[0], STDIN_FILENO);
        }

        execl("/bin/sh", "sh", "-c", command, (char *)NULL);
        exit(1);
    }

    // parinte
    SO_FILE *soFile = (SO_FILE *)malloc(sizeof(SO_FILE));
    
    if (!soFile)
    {
        close(pfd[0]);
        close(pfd[1]);
        return NULL;
    }

    if (strcmp(type, "r") == 0)
    {
        close(pfd[1]);
        soFile->fd = pfd[0];
    }
    else if (strcmp(type, "w") == 0)
    {
        close(pfd[0]);
        soFile->fd = pfd[1];
    }
    soFile->buf_pos = 0;
    soFile->buf_end = 0;

    return soFile;
}

int so_pclose(SO_FILE *stream)
{
    int status;
    
    if (!stream)
    {
        return -1;
    }

    close(stream->fd);
    free(stream);

    if (wait(&status) == -1)
    {
        return -1;
    }

    return WEXITSTATUS(status);
}
