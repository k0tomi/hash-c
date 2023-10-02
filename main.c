#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_FILE_NAME 100

// OpenSSL engine implementation
#define OPENSSL_ENGINE NULL

static const unsigned char *getShaSum(const unsigned char *string)
{
    EVP_MD_CTX *mdCtx = EVP_MD_CTX_new();
    unsigned char mdVal[EVP_MAX_MD_SIZE];
    unsigned int mdLen;

    if (!EVP_DigestInit_ex(mdCtx, EVP_sha256(), OPENSSL_ENGINE))
    {
        printf("Message digest initialization failed.\n");
        EVP_MD_CTX_free(mdCtx);
        exit(EXIT_FAILURE);
    }

    // Hashes cnt bytes of data at d into the digest context mdCtx
    if (!EVP_DigestUpdate(mdCtx, string, strlen((const char *)string)))
    {
        printf("Message digest update failed.\n");
        EVP_MD_CTX_free(mdCtx);
        exit(EXIT_FAILURE);
    }

    if (!EVP_DigestFinal_ex(mdCtx, mdVal, &mdLen))
    {
        printf("Message digest finalization failed.\n");
        EVP_MD_CTX_free(mdCtx);
        exit(EXIT_FAILURE);
    }
    EVP_MD_CTX_free(mdCtx);

    // Convert the hash to a string representation
    char *hex = malloc(mdLen * 2 + 1);
    for (int i = 0; i < mdLen; i++)
    {
        sprintf(hex + i * 2, "%02x", mdVal[i]);
    }
    hex[mdLen * 2] = '\0';

    const unsigned char *result = hex;

    // Free the memory allocated for hex before returning
    free(hex);

    return result;
}

int main(void)
{

    FILE *fp;
    char filename[MAX_FILE_NAME];
    char c, c2, c3; 
    int fd;
    struct stat st;
    int wl_wordcount = 0;
    int wl_charcount = 0;
    char **buf;
    char target_hash[65];

    fflush(stdin);
    printf("Enter hash: ");
    scanf("%s", target_hash);

    printf("Enter wordlist name: ");
    scanf("%s", filename);


    // Open the file and check pre-requirements
    fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        printf("fd value failed");
        exit(EXIT_FAILURE);
    }
    
    fp = fdopen(fd, "r");

    if ((fstat(fd, &st) != 0) || (!S_ISREG(st.st_mode)))
    {
        printf("Irregular file");
        exit(EXIT_FAILURE);
    }
    
    // Get current wordcount for memory allocation and reset filepointer
        for (c = getc(fp); c != EOF; c = getc(fp))
    {
        if (c == '\n')
            wl_wordcount = wl_wordcount + 1;
    }
    printf("WC = [%d]\n", wl_wordcount);
    fseek(fp, 0, SEEK_SET);

    // allocate memory for the required wordcount into first dimension
    int total_wc = wl_wordcount;
    buf = malloc(wl_wordcount * (sizeof(char *)));
    
    // reset wordcount counter
    wl_wordcount = 0;

    // iterate file looking for individual wordsize
    for (c2 = getc(fp); c2 != EOF; c2 = getc(fp))
    {
        if (c2 == '\n')
        {
            // allocate memory equal to worsize without a newline or string terminator
            buf[wl_wordcount] = malloc(wl_charcount * (sizeof(char)));
            if (buf[wl_wordcount] == NULL){
                printf("malloc failue detected\n");
                for (int i = 0; i < total_wc; i++)
                    free(buf[i]);
                free(buf);
                exit(EXIT_FAILURE);
            }
            wl_charcount = 0;
            wl_wordcount = wl_wordcount + 1;
        }
        else
        {
            wl_charcount = wl_charcount + 1;
        }
    }
    printf("Finished memory allocation succesfully!\n");
    
    // Reset filepointer and wordcount counters
    fseek(fp, 0, SEEK_SET);
    wl_wordcount = 0;

    // Iterate file adding words to their allocated space
    for (c3 = getc(fp); wl_wordcount < total_wc; c3 = getc(fp))
    {
        if (c3 == '\n')
        {
            // At the start of a new word, attempt hash decryption with the previous word
            if (strcmp(getShaSum(buf[wl_wordcount]), target_hash) == 0)
            {
                // End loop on successful decryption and print plaintext equivalent
                printf("Decryption successful! Plaintext: [%s]\n", buf[wl_wordcount]);
                break;
            }
            wl_wordcount = wl_wordcount + 1;
        }
        else
        {
            strncat(buf[wl_wordcount], &c3, 1);
        }
    }
    printf("Finished iterating file\n");

    // Extract characters from file and store in character c
    printf("Number of attempted words before ending: [%d]\n", wl_wordcount);

    // Close the file
    fclose(fp);
    close(fd);

    for (int i = 0; i < total_wc; i++)
        free(buf[i]);
    free(buf);
    printf("Press any key to exit");
    fflush(stdout);
    getchar(); getchar();
    exit(EXIT_SUCCESS);
}
