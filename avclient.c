#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUF_LEN 8192

int main(int argc, char **argv)
{
        int servfd, rv;
        ssize_t n;
        size_t nf;
        unsigned char buf[BUF_LEN] = "LOLOL\n";
        FILE *img_file;
        struct sockaddr_in serv;

        img_file = fopen("image.jpeg", "w");
        if (!img_file) {
                perror("fopen");
                exit(EXIT_FAILURE);
        }

        if (argc < 2) {
                fprintf(stderr, "%s: not enough params (2)\n", argv[0]);
                exit(EXIT_FAILURE);
        }

        memset(&serv, 0, sizeof(serv));
        serv.sin_family = AF_INET;
        serv.sin_port = htons(9999);
        serv.sin_addr.s_addr = inet_addr(argv[1]);

        servfd = socket(AF_INET, SOCK_STREAM, 0);
        if (servfd == -1) {
                perror("socket");
                exit(EXIT_FAILURE);
        }

        rv = connect(servfd, (struct sockaddr *)&serv, sizeof(serv));
        if (rv == -1) {
                perror("connect");
                exit(EXIT_FAILURE);
        }

        n = write(servfd, (void *)buf, strlen(buf));
        if (n == -1) {
                perror("write");
                exit(EXIT_FAILURE);
        }

        for (;;) {
                n = read(servfd, (void *)buf, BUF_LEN);
                if (n == 0) {
                        break;
                } else if (n == -1) {
                        perror("read");
                        exit(EXIT_FAILURE);
                }
                nf = fwrite((void *)buf, 1, n, img_file);
                if (nf != n) {
                        perror("fwrite");
                        exit(EXIT_FAILURE);
                }
        }

        rv = fclose(img_file);
        if (rv == -1) {
                perror("fclose");
                exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);
}

