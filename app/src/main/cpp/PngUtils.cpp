//
// Created by zkp on 2017/8/11.
//
#include "include/com_zkp_com_myapplication_PngUtils.h"
#include <stdio.h>
#include <png.h>

#define PNG_BYTES_TO_CHECK 7

JNIEXPORT jboolean JNICALL Java_com_zkp_com_myapplication_PngUtils_checkIsPng
        (JNIEnv *env, jclass cls, jstring fileName) {
    FILE *in;
    png_byte buff[PNG_BYTES_TO_CHECK];
    jboolean iscopy;

    const char *filename = env->GetStringUTFChars(fileName, &iscopy);
    in = fopen(filename, "r");
    if (!in) {
        return false;
    }
    size_t size = fread(buff, sizeof(*buff), sizeof(buff) / sizeof(*buff), in);
    fclose(in);
    env->ReleaseStringUTFChars(fileName, filename);
    if (size != PNG_BYTES_TO_CHECK)
        return false;
    else
        return (!png_sig_cmp(buff, (png_size_t) 0, PNG_BYTES_TO_CHECK));
}


JNIEXPORT jstring JNICALL Java_com_zkp_com_myapplication_PngUtils_readPngHighLevel(JNIEnv *env,
                                                                                   jclass type,
                                                                                   jstring fileName) {
    png_structp png_ptr_r;
    png_infop info_ptr_r;
    FILE *fp;
    jboolean iscopy;

    const char *filename = env->GetStringUTFChars(fileName, &iscopy);
    fp = fopen(filename, "rb");
    if (!fp) {
        return env->NewStringUTF("high level read failed");
    }
    png_ptr_r = png_create_read_struct
            (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr_r)
        return env->NewStringUTF("high level read failed");
    info_ptr_r = png_create_info_struct(png_ptr_r);
    if (!info_ptr_r) {
        png_destroy_read_struct(&png_ptr_r,
                                (png_infopp) NULL, (png_infopp) NULL);
        return env->NewStringUTF("high level read failed");
    }
    if (setjmp(png_jmpbuf(png_ptr_r))) {
        png_destroy_read_struct(&png_ptr_r, &info_ptr_r,
                                NULL);
        fclose(fp);
        return env->NewStringUTF("high level read failed");
    }
    png_init_io(png_ptr_r, fp);
    png_read_png(png_ptr_r, info_ptr_r, PNG_TRANSFORM_BGR, NULL);
    png_textp pngTextp;
    int num;
    int result = png_get_text(png_ptr_r,info_ptr_r,&pngTextp,&num);
    if (num > 0) {
        png_charp key = pngTextp[0].key;
        png_charp value = pngTextp[0].text;
    }
    png_uint_32 picHeight = png_get_image_height(png_ptr_r, info_ptr_r);
    png_bytepp png_rows = new png_bytep[picHeight];
    for (int i = 0; i < picHeight; ++i) {
        png_rows[i] = NULL;
    }
    png_rows = png_get_rows(png_ptr_r, info_ptr_r);
    fclose(fp);
    return env->NewStringUTF("high level read success");
}

JNIEXPORT jstring JNICALL
Java_com_zkp_com_myapplication_PngUtils_lowLevelReadLowLevelWrite(JNIEnv *env, jclass type,
                                                                   jstring fileName_,
                                                                   jstring outfileName) {
    png_structp png_ptr_r;
    png_infop info_ptr_r;
    FILE *infp, *outfp;
    jboolean iscopy;

    png_structp png_ptr_w;
    png_infop info_ptr_w;

    const char *filename = env->GetStringUTFChars(fileName_, &iscopy);
    infp = fopen(filename, "rb");
    if (!infp) {
        return env->NewStringUTF("low level read failed");
    }
    png_ptr_r = png_create_read_struct
            (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr_r)
        return env->NewStringUTF("low level read failed");
    info_ptr_r = png_create_info_struct(png_ptr_r);
    if (!info_ptr_r) {
        png_destroy_read_struct(&png_ptr_r,
                                (png_infopp) NULL, (png_infopp) NULL);
        return env->NewStringUTF("low level read failed");
    }
    if (setjmp(png_jmpbuf(png_ptr_r))) {
        png_destroy_read_struct(&png_ptr_r, &info_ptr_r,
                                NULL);
        fclose(infp);
        return env->NewStringUTF("low level read failed");
    }
    png_init_io(png_ptr_r, infp);

    png_read_info(png_ptr_r, info_ptr_r);
    png_uint_32 picHeight = png_get_image_height(png_ptr_r, info_ptr_r);
    png_uint_32 picWidth = png_get_image_width(png_ptr_r, info_ptr_r);
    png_byte bitDepth = png_get_bit_depth(png_ptr_r, info_ptr_r);
    png_byte colorType = png_get_color_type(png_ptr_r, info_ptr_r);
    png_byte interlaceType = png_get_interlace_type(png_ptr_r, info_ptr_r);
    png_byte compressionType = png_get_compression_type(png_ptr_r, info_ptr_r);
    png_byte filterType = png_get_filter_type(png_ptr_r, info_ptr_r);
    png_byte passNum = png_set_interlace_handling(png_ptr_r);
    png_bytep row_pointers[picHeight];

    /* Clear the pointer array */
    for (int row = 0; row < picHeight; row++)
        row_pointers[row] = NULL;

    for (int row = 0; row < picHeight; row++)
        row_pointers[row] = (png_bytep) png_malloc(png_ptr_r, png_get_rowbytes(png_ptr_r,
                                                                               info_ptr_r));
//
//    for (int pass = 0; pass < passNum; pass++) {
//        for (int y = 0; y < picHeight; y++) {
//            png_read_rows(png_ptr_r, &row_pointers[y], NULL, 1);
//        }
//    }
    png_read_image(png_ptr_r, row_pointers);
    /* Read rest of file, and get additional chunks in info_ptr - REQUIRED */
    png_read_end(png_ptr_r, info_ptr_r);


    const char *outfilename = env->GetStringUTFChars(outfileName, &iscopy);
    outfp = fopen(outfilename, "wb");
    png_ptr_w = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                        NULL, NULL, NULL);
    if (png_ptr_w == NULL) {
        fclose(outfp);
        return env->NewStringUTF("");
    }

    info_ptr_w = png_create_info_struct(png_ptr_w);
    if (info_ptr_w == NULL) {
        fclose(outfp);
        png_destroy_write_struct(&png_ptr_w, NULL);
        return env->NewStringUTF("");
    }

    if (setjmp(png_jmpbuf(png_ptr_w))) {
        fclose(outfp);
        png_destroy_write_struct(&png_ptr_w, &info_ptr_w);
        return env->NewStringUTF("error");
    }
    png_init_io(png_ptr_w, outfp);
//    png_set_rows(png_ptr_w, info_ptr_w, (png_bytepp) &row_pointers);
//    png_write_png(png_ptr_w, info_ptr_w, NULL, NULL);
    png_set_IHDR(png_ptr_w, info_ptr_w, picWidth, picHeight, bitDepth, colorType, interlaceType,
                 compressionType, filterType);
    png_text text_ptr[1];

    char key0[] = "Title";
    char text0[] = "ZKP";
    text_ptr[0].key = key0;
    text_ptr[0].text = text0;
    text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
    text_ptr[0].itxt_length = 0;
    text_ptr[0].lang = NULL;
    text_ptr[0].lang_key = NULL;
    png_set_text(png_ptr_w, info_ptr_w, text_ptr, 1);
    png_write_info(png_ptr_w, info_ptr_w);
    png_byte image[picHeight * picWidth * bitDepth];
    if (picHeight > PNG_UINT_32_MAX / (sizeof(png_bytep)))
        png_error(png_ptr_w, "Image is too tall to process in memory");

    /* Set up pointers into your "image" byte array */
//    for (int k = 0; k < picHeight; k++)
//        row_pointers[k] = image + k * picWidth * bitDepth;
    png_write_image(png_ptr_w, row_pointers);
    png_write_end(png_ptr_w, info_ptr_w);
    png_destroy_write_struct(&png_ptr_w, &info_ptr_w);

    fflush(outfp);
    fclose(outfp);

    env->ReleaseStringUTFChars(fileName_, filename);
    return env->NewStringUTF("");
}

JNIEXPORT jstring JNICALL Java_com_zkp_com_myapplication_PngUtils_highLevelReadHighLevelWrite(
        JNIEnv *env,
        jclass type,
        jstring infileName,
        jstring outfileName) {

    png_structp png_ptr_r;
    png_infop info_ptr_r;
    FILE *infp, *outfp;
    jboolean iscopy;

    png_structp png_ptr_w;
    png_infop info_ptr_w;

    const char *infilename = env->GetStringUTFChars(infileName, &iscopy);
    infp = fopen(infilename, "rb");

    ///////////read//////////
    if (infp == NULL)
        return env->NewStringUTF("open file failed");
    png_ptr_r = png_create_read_struct
            (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr_r)
        return env->NewStringUTF("high level read failed");
    info_ptr_r = png_create_info_struct(png_ptr_r);
    if (!info_ptr_r) {
        png_destroy_read_struct(&png_ptr_r,
                                (png_infopp) NULL, (png_infopp) NULL);
        return env->NewStringUTF("high level read failed");
    }
    if (setjmp(png_jmpbuf(png_ptr_r))) {
        png_destroy_read_struct(&png_ptr_r, &info_ptr_r,
                                NULL);
        fclose(infp);
        return env->NewStringUTF("high level read failed");
    }
    png_init_io(png_ptr_r, infp);
    png_read_png(png_ptr_r, info_ptr_r, PNG_TRANSFORM_EXPAND, NULL);
    png_uint_32 picHeight = png_get_image_height(png_ptr_r, info_ptr_r);
    png_uint_32 picWidth = png_get_image_width(png_ptr_r, info_ptr_r);
    png_byte bitDepth = png_get_bit_depth(png_ptr_r, info_ptr_r);
    png_byte colorType = png_get_color_type(png_ptr_r, info_ptr_r);
    png_byte interlaceType = png_get_interlace_type(png_ptr_r, info_ptr_r);
    png_byte compressionType = png_get_compression_type(png_ptr_r, info_ptr_r);
    png_byte filterType = png_get_filter_type(png_ptr_r, info_ptr_r);
    png_bytepp png_rows = png_get_rows(png_ptr_r, info_ptr_r);
    ///////////read end//////////

    const char *outfilename = env->GetStringUTFChars(outfileName, &iscopy);
    outfp = fopen(outfilename, "wb");
    png_ptr_w = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                        NULL, NULL, NULL);
    if (png_ptr_w == NULL) {
        fclose(outfp);
        return env->NewStringUTF("");
    }

    info_ptr_w = png_create_info_struct(png_ptr_w);
    if (info_ptr_w == NULL) {
        fclose(outfp);
        png_destroy_write_struct(&png_ptr_w, NULL);
        return env->NewStringUTF("");
    }

    if (setjmp(png_jmpbuf(png_ptr_w))) {
        fclose(outfp);
        png_destroy_write_struct(&png_ptr_w, &info_ptr_w);
        return env->NewStringUTF("");
    }
    png_set_IHDR(png_ptr_w, info_ptr_w, picWidth, picHeight, bitDepth, colorType, interlaceType,
                 compressionType, filterType);
    png_init_io(png_ptr_w, outfp);
    png_set_rows(png_ptr_w, info_ptr_w, png_rows);
    png_write_png(png_ptr_w, info_ptr_w, PNG_TRANSFORM_SHIFT
                                         | PNG_TRANSFORM_SWAP_ALPHA, NULL);
    fflush(outfp);
    fclose(outfp);
    return env->NewStringUTF("");
}