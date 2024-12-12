#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <lzma.h>

unsigned long sizediffaddr(FILE *fpatch)
{
    int cp;
    unsigned long size = 0;
    while ((cp = fgetc(fpatch)) != '\n')
    {
        if (isdigit(cp))
        {
            size = size * 10 + (cp - '0');
        }
    }
    // rewind(fpatch);
    return size;
}

unsigned long *allocdiffaddr(FILE *fpatch, unsigned long size)
{
    int cp;
    unsigned long *maddr = malloc(sizeof(unsigned long) * size);
    if (!maddr)
    {
        perror("malloc failed");
        return NULL;
    }

    unsigned long current_number = 0;
    unsigned long index = 0;
    unsigned long nparsed = size;

    while (nparsed)
    {
        cp = fgetc(fpatch);
        if (isdigit(cp))
        {
            current_number = current_number * 10 + (cp - '0');
        }
        else if (cp == '\n')
        {
            maddr[index++] = current_number;
            current_number = 0;
            nparsed--;
        }
    }

    return maddr;
}

void writediff(FILE *fori, FILE *fpatch, FILE *fout, unsigned long *ptradd)
{
    int co, cm;
    bool map = false;
    unsigned long index = 0;
    unsigned long start = 0;
    unsigned long end = 1;
    while ((co = fgetc(fori)) != EOF)
    {
        if (index == ptradd[start])
        {
            map = true;
            start += 2;
        }
        if (index == ptradd[end])
        {
            map = false;
            end += 2;
        }
        if (!map)
        {
            fwrite(&co, sizeof(char), 1, fout);
        }
        if (map)
        {
            cm = fgetc(fpatch);
            fwrite(&cm, sizeof(char), 1, fout);
        }

        index++;
    }
}

void patchdecompress(char *in, char *out)
{
    FILE *fin = fopen(in, "rb");
    FILE *fout = fopen(out, "wb");
    if (!fin || !fout)
    {
        perror("File open error");
        if (fin)
            fclose(fin);
        if (fout)
            fclose(fout);
        return;
    }

    lzma_stream strm = LZMA_STREAM_INIT;
    if (lzma_stream_decoder(&strm, UINT64_MAX, LZMA_CONCATENATED) != LZMA_OK)
    {
        fprintf(stderr, "Error initializing LZMA stream decoder.\n");
        fclose(fin);
        fclose(fout);
        return;
    }

    uint8_t inbuf[BUFSIZ];
    uint8_t outbuf[BUFSIZ];
    lzma_action action = LZMA_RUN;

    int success = 1;
    while (1)
    {
        strm.next_in = inbuf;
        strm.avail_in = fread(inbuf, 1, BUFSIZ, fin);
        if (ferror(fin))
        {
            fprintf(stderr, "File read error.\n");
            success = 0;
            break;
        }
        if (feof(fin))
            action = LZMA_FINISH;

        do
        {
            strm.next_out = outbuf;
            strm.avail_out = BUFSIZ;

            lzma_ret ret = lzma_code(&strm, action);
            if (ret == LZMA_STREAM_END)
            {
                fwrite(outbuf, 1, BUFSIZ - strm.avail_out, fout);
                break;
            }
            if (ret != LZMA_OK)
            {
                fprintf(stderr, "Decompression error: %d\n", ret);
                success = 0;
                break;
            }

            if (fwrite(outbuf, 1, BUFSIZ - strm.avail_out, fout) < BUFSIZ - strm.avail_out)
            {
                fprintf(stderr, "File write error.\n");
                success = 0;
                break;
            }
        } while (strm.avail_out == 0);

        if (action == LZMA_FINISH || !success)
            break;
    }

    lzma_end(&strm);
    fclose(fin);
    fclose(fout);

    if (!success)
    {
        remove(out);
    }
}

void applypatch(char *ori, char *patch, char *out)
{
    FILE *fori = NULL;
    fori = fopen(ori, "rb");
    if (fori == NULL)
    {
        fprintf(stderr, "fail to read %s\n", ori);
        exit(1);
    }
    fseek(fori, 0L, SEEK_END);
    long orisz = ftell(fori);
    fseek(fori, 0L, SEEK_SET);
    FILE *fpatch = NULL;
    fpatch = fopen(patch, "rb");
    if (fpatch == NULL)
    {
        fprintf(stderr, "fail to read %s\n", patch);
        exit(1);
    }
    fseek(fpatch, 0L, SEEK_END);
    long patchsz = ftell(fpatch);
    fseek(fpatch, 0L, SEEK_SET);
    if (patchsz >= orisz)
    {
        fprintf(stderr, "invalid size\n");
        fclose(fori);
        fclose(fpatch);
        exit(1);
    }
    FILE *fout = NULL;
    fout = fopen(out, "wb");
    if (fori == NULL)
    {
        fprintf(stderr, "fail to write %s\n", out);
        fclose(fori);
        fclose(fpatch);
        exit(1);
    }
    char validsig[2] = {0xFF, 0x98};
    char findsig[2];
    fread(&findsig, sizeof(findsig), 1, fpatch);
    if (memcmp(validsig, findsig, sizeof(validsig)) != 0)
    {
        fprintf(stderr, "invalid signature %02x %02x\n",
                (unsigned char)findsig[0], (unsigned char)findsig[1]);
        fclose(fori);
        fclose(fpatch);
        fclose(fout);
        exit(1);
    }

    unsigned long addsize = sizediffaddr(fpatch);
    unsigned long *allocaddr = allocdiffaddr(fpatch, addsize);

    writediff(fori, fpatch, fout, allocaddr);
    free(allocaddr);
    fclose(fori);
    fclose(fpatch);
    fclose(fout);
}

int main()
{
    patchdecompress("patchout.gmap", "tmp/tmppatchoutd.bin");
    applypatch("blob.bin", "tmp/tmppatchoutd.bin", "patched.bin");
    return 0;
}