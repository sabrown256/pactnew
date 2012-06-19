;
; IMPORT.SCM - test importing libraries
;            - which means generating and compiling bindings for them
;

(import "/usr/include/jpeglib.h" "/usr/lib/x86_64-linux-gnu/libjpeg.so")

(define fname "pict-0001-001.jpg")
(define infile (fopen fname "rb"))

(jpeg-create-decompress &cinfo)

; specify data source (eg, a file)
(jpeg-stdio-src(&cinfo infile)

; read file parameters with jpeg_read_header()
(jpeg-read-header &cinfo 1)

; set parameters for decompression

; start decompressor
(jpeg-start-decompress &cinfo)

(quit)

