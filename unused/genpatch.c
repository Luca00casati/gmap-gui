#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <lzma.h>

unsigned long sizediffaddr(FILE* fori, FILE* fmod){
    bool newadress = true;
    unsigned long naddress = 0;
    int co, cm;
    while ((co = fgetc(fori))!=EOF)
    {
        cm = fgetc(fmod);
        if (co != cm && newadress){
            naddress++;
            newadress = false;
        }
        if (co == cm && !newadress){
            naddress++;
            newadress = true;
        }
        
    }
    rewind(fmod);
    rewind(fori);
    return naddress;
}

void setdiffaddr(FILE* fori, FILE* fmod, FILE* out){
    bool newadress = true;
    long findex = 0;
    int co, cm;
    while ((co = fgetc(fori))!=EOF)
    {
        cm = fgetc(fmod);
        if (co != cm && newadress){
            fprintf(out,"%ld\n", findex);
            newadress = false;
        }
        if (co == cm && !newadress){
            fprintf(out,"%ld\n", findex);
            newadress = true;
        }
        findex++;
        
    }
    rewind(fmod);
    rewind(fori);
}

void setdiffraw(FILE* fori, FILE* fmod, FILE* out){
    int co, cm;
    while ((co = fgetc(fori))!=EOF)
    {
        cm = fgetc(fmod);
        if (co != cm){
            fwrite(&cm, sizeof(char), 1, out);
        }
        
    }
    rewind(fmod);
    rewind(fori);
}

void genpatch(char* ori, char* mod, char* out){
    FILE* fori = NULL;
    fori = fopen(ori, "rb");
    if (fori == NULL){
        fprintf(stderr,"fail to read %s\n", ori);
        exit(1);
    }
    fseek(fori, 0L, SEEK_END);
    long orisz = ftell(fori);
    fseek(fori, 0L, SEEK_SET);
    FILE* fmod = NULL;
    fmod = fopen(mod, "rb");
    if (fmod == NULL){
        fprintf(stderr,"fail to read %s\n", mod);
        exit(1);
    }
    fseek(fmod, 0L, SEEK_END);
    long modsz = ftell(fmod);
    fseek(fmod, 0L, SEEK_SET);
    if (modsz != orisz){
        fprintf(stderr,"invalid size\n");
        fclose(fori);
        fclose(fmod);
        exit(1);
    }
    FILE* fout = NULL;
    fout = fopen(out, "wb");
    if (fori == NULL){
        fprintf(stderr,"fail to write %s\n", out);
        fclose(fori);
        fclose(fmod);
        exit(1);
    }
    unsigned long addsize = sizediffaddr(fori, fmod);
    char sig[2] = {0xFF, 0x98};
    fwrite(&sig, sizeof(sig), 1, fout);
    fprintf(fout, "%ld\n", addsize);
    setdiffaddr(fori, fmod, fout);
    setdiffraw(fori, fmod, fout);
    fclose(fori);
    fclose(fmod);
    fclose(fout);
}

void patchcompress(char* in, char* out) {
    FILE* fin = fopen(in, "rb");
    if (fin == NULL) {
        fprintf(stderr, "Fail to read %s\n", in);
        exit(1);
    }

    FILE* fout = fopen(out, "wb");
    if (fout == NULL) {
        fprintf(stderr, "Fail to write %s\n", out);
        fclose(fin);
        exit(1);
    }

    fseek(fin, 0, SEEK_END);
    long in_size = ftell(fin);
    fseek(fin, 0, SEEK_SET);

    unsigned char* in_buffer = malloc(in_size);
    if (!in_buffer) {
        fprintf(stderr, "Failed to allocate memory for input buffer\n");
        fclose(fin);
        fclose(fout);
        exit(1);
    }

    fread(in_buffer, 1, in_size, fin);

    lzma_stream strm = LZMA_STREAM_INIT;
    if (lzma_easy_encoder(&strm, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64) != LZMA_OK) {
        fprintf(stderr, "Failed to initialize LZMA encoder\n");
        free(in_buffer);
        fclose(fin);
        fclose(fout);
        exit(1);
    }

    size_t out_buffer_size = in_size + in_size / 3 + 128;
    unsigned char* out_buffer = malloc(out_buffer_size);
    if (!out_buffer) {
        fprintf(stderr, "Failed to allocate memory for output buffer\n");
        free(in_buffer);
        lzma_end(&strm);
        fclose(fin);
        fclose(fout);
        exit(1);
    }

    strm.next_in = in_buffer;
    strm.avail_in = in_size;
    strm.next_out = out_buffer;
    strm.avail_out = out_buffer_size;

    lzma_ret ret = lzma_code(&strm, LZMA_FINISH);
    if (ret != LZMA_STREAM_END) {
        fprintf(stderr, "Failed to compress data\n");
        free(in_buffer);
        free(out_buffer);
        lzma_end(&strm);
        fclose(fin);
        fclose(fout);
        exit(1);
    }

    fwrite(out_buffer, 1, strm.total_out, fout);

    free(in_buffer);
    free(out_buffer);
    lzma_end(&strm);
    fclose(fin);
    fclose(fout);
}

int main(){
    genpatch("blob.bin", "blob_mod.bin", "tmp/tmppatchoutc.bin");
    patchcompress("tmp/tmppatchoutc.bin", "patchout.gmap");
    return 0;
}