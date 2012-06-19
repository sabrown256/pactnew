;
; IMPORT.SCM - test importing libraries
;            - which means generating and compiling bindings for them
;

(cond ((file? "/usr/lib/x86_64-linux-gnu/libjpeg.so")
       (import "/usr/include/jpeglib.h" "/usr/lib/x86_64-linux-gnu/libjpeg.so"))
      ((file? "/usr/lib64/libjpeg.so")
       (import "/usr/include/jpeglib.h" "/usr/lib64/libjpeg.so")))

;(define fname "pict-0001-001.jpg")
(define fname "import-image.jpg")
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

