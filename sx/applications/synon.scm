;
; SYNON.SCM - define the standard synonyms for ULTRA II
;
; Source Version: 4.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

(synonym powr pow)
(synonym powrx powx)
(synonym log10x logx)
(synonym derivative der)
(synonym integrate int)

(synonym line-color default-color)

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; ULTRA specific synonyms

(if (defined? ultra)
    (begin
      (synonym erase era)
      (synonym domain dom)
      (synonym range ran)
      (synonym kill expunge)
      (synonym select # cur pl)
      (synonym compose comp)
      (synonym smooth3 smooth)
      (synonym hardcopy hc)
      (synonym show unhide)

      (synonym + sum)
      (synonym - dif)
      (synonym * prod)
      (synonym / quot ratio div)
      (synonym load ld*)

; Any time after 2/93, promote rd*, and save* to C and eliminate open, and wrt.
      (synonym open rd*)
      (synonym wrt save*)

      (synonym end quit)
      (synonym break stop)))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; PDBView specific synonyms

(if (defined? pdbview)
    (begin
      (synonym array-nd-set nds)
      (synonym change-dimension chdim)
      (synonym close clf)
      (synonym close-textfile clt)
      (synonym close-viewport clv)
      (synonym close-window clw)
      (synonym change-file cf)
      (synonym change-palette palette)
      (synonym change-viewport cv)
      (synonym change-window cw)
      (synonym copy-mapping cm)
      (synonym data-reference dref)
      (synonym default-viewport-rendering vr)
      (synonym define-file-variable defv)
      (synonym define-file-variable* defv*)
      (synonym delete-mapping dl)
      (synonym display-domain pld)
      (synonym display-mapping pl)
      (synonym drawable-rendering dr)
      (synonym domv fix-domain)
      (synonym ranm fix-range)
      (synonym hardcopy-viewport hcv)
      (synonym hardcopy-window hc)
      (synonym list-files lf)
      (synonym list-mappings-in-viewport lsv)
      (synonym list-windows lw)
      (synonym make-ac-mapping-direct ac)
      (synonym make-lr-mapping-direct lr)
      (synonym make-lr-mapping-synth lrs)
      (synonym make-cp-set cps)
      (synonym print-mapping pm)
      (synonym save-mapping sm)
      (synonym update-window wu)
      (synonym view-angle va)
      (synonym open-textfile ot)
      (synonym close-textfile ct)
      (synonym table tab)))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; C syntax mode synonyms
; NOTE: these won't hurt even if C mode is not present

(if (defined? pdbview)
    (begin  (synonym m+                          mplus)
	    (synonym m-                          mminus)
	    (synonym m*                          mtimes)
	    (synonym m/                          mquotient)
	    (synonym m==                         meq)
	    (synonym m!=                         mneq)
	    (synonym m<                          mlt)
	    (synonym m<=                         mle)
	    (synonym m>                          mgt)
	    (synonym m>=                         mge)
	    (synonym curve?                      curvep)
	    (synonym ultra-file?                 ultra_filep)
	    (synonym pg-interface-object?        pg_interface_objectp)
            (if (defined? describe*)
		(synonym describe*               describe_))
	    (synonym pa-iv-specification?        pa_iv_specificationp)
	    (synonym pa-package?                 pa_packagep)
	    (synonym pa-src-variable?            pa_src_variablep)
	    (synonym pa-variable?                pa_variablep)
	    (synonym pa-variable->c-array        pa_variable_to_c_array)
	    (synonym defstr?                     defstrp)
	    (synonym file-variable?              file_variablep)
	    (synonym hash->pdbdata               hash_to_pdbdata)
	    (synonym index->tuple-string         index_to_tuple_string)
	    (synonym make-cast*                  make_cast_)
	    (synonym read-defstr*                read_defstr_)
	    (synonym write-defstr*               write_defstr_)
	    (synonym make-defstr*                make_defstr_)
	    (synonym make-typedef*               make_typedef_)
	    (synonym pdb->list                   pdb_to_list)
	    (synonym pdbdata?                    pdbdatap)
	    (synonym pdbdata->hash               pdbdata_to_hash)
	    (synonym pdbfile?                    pdbfilep)
	    (synonym reset-pointer-list!         reset_pointer_listb)
	    (synonym pd-set-buffer-size!         pd_set_buffer_sizeb)
	    (synonym set-maximum-file-size!      set_maximum_file_sizeb)
	    (synonym set-track-pointers!         set_track_pointersb)
	    (synonym syment?                     symentp)
	    (synonym set-attribute-value!        set_attribute_valueb)
	    (synonym pdbdata->pg-graph           pdbdata_to_pg_graph)
	    (synonym pdbdata->pg-image           pdbdata_to_pg_image)
	    (synonym pg-device?                  pg_devicep)
	    (synonym pg-device-attributes?       pg_device_attributesp)
	    (synonym pg-graph?                   pg_graphp)
	    (synonym pg-graph->pdbcurve          pg_graph_to_pdbcurve)
	    (synonym pg-graph->pdbdata           pg_graph_to_pdbdata)
	    (synonym pg-grotrian-graph?          pg_grotrian_graphp)
	    (synonym pg-image?                   pg_imagep)
	    (synonym pg-image->pdbdata           pg_image_to_pdbdata)
	    (synonym pg-set-domain-limits!       pg_set_domain_limitsb)
	    (synonym pg-set-graph-attribute!     pg_set_graph_attributeb)
	    (synonym pg-set-label!               pg_set_labelb)
	    (synonym pg-set-range-limits!        pg_set_range_limitsb)
	    (synonym pg-set-view-angle!          pg_set_view_angleb)
	    (synonym pg-clipping?                pg_clippingp)
	    (synonym list->pg-palette            list_to_pg_palette)
	    (synonym pg-normalized->pixel        pg_normalized_to_pixel)
	    (synonym pg-normalized->world        pg_normalized_to_world)
	    (synonym pg-palette->list            pg_palette_to_list)
	    (synonym pg-pixel->normalized        pg_pixel_to_normalized)
	    (synonym pg-set-autodomain!          pg_set_autodomainb)
	    (synonym pg-set-autoplot!            pg_set_autoplotb)
	    (synonym pg-set-autorange!           pg_set_autorangeb)
	    (synonym pg-set-border-width!        pg_set_border_widthb)
	    (synonym pg-set-char-path!           pg_set_char_pathb)
	    (synonym pg-set-char-up!             pg_set_char_upb)
	    (synonym pg-set-clear-mode!          pg_set_clear_modeb)
	    (synonym pg-set-clipping!            pg-set_clippingb)
	    (synonym pg-set-data-id-flag!        pg_set_data_id_flagb)
	    (synonym pg-set-fill-color!          pg_set_fill_colorb)
	    (synonym pg-set-finish-state!        pg_set_finish_stateb)
	    (synonym pg-set-grid-flag!           pg_set_grid_flagb)
	    (synonym pg-set-line-color!          pg_set_line_colorb)
	    (synonym pg-set-logical-op!          pg_set_logical_opb)
	    (synonym pg-set-line-style!          pg_set_line_styleb)
	    (synonym pg-set-line-width!          pg_set_line_widthb)
	    (synonym pg-set-marker-orientation!  pg_set_marker_orientationb)
	    (synonym pg-set-marker-scale!        pg_set_marker_scaleb)
	    (synonym pg-set-maximum-intensity!   pg_set_maximum_intensityb)
	    (synonym pg-set-palette!             pg_set_paletteb)
	    (synonym pg-set-resolution-scale-factor!    pg_set_resolution_scale_factorb)
	    (synonym pg-set-scatter-flag!        pg_set_scatter_flagb)
	    (synonym pg-set-text-color!          pg_set_text_colorb)
	    (synonym pg-set-text-font!           pg_set_text_fontb)
	    (synonym pg-set-white-background!    pg_set_white_backgroundb)
	    (synonym pg-set-frame!               pg_set_frameb)
	    (synonym pg-set-vector-attributes!   pg_set_vector_attributesb)
	    (synonym pg-set-viewport!            pg_set_viewportb)
	    (synonym pg-set-world-coordinate-system!    pg_set_world_coordinate_systemb)
	    (synonym pg-world->normalized        pg_world_to_normalized)
	    (synonym pg-set-color-type!          pg_set_color_typeb)
	    (synonym c-array?                    c_arrayp)
	    (synonym list->c-array               list_to_c_array)
	    (synonym c-array->list               c_array_to_list)
	    (synonym c-array-set!                c_array_setb)
	    (synonym pm-set?                     pm_setp)
	    (synonym pm-mapping?                 pm_mappingp)
	    (synonym pm-grotrian-mapping?        pm_grotrian_mappingp)
	    (synonym pm-mapping->pdbdata         pm_mapping_to_pdbdata)
	    (synonym pdbdata->pm-mapping         pdbdata_to_pm_mapping)
	    (synonym pm-set->pdbdata             pm_set_to_pdbdata)
	    (synonym pdbdata->pm-set             pdbdata_to_pm_set)
	    (synonym pdbdata->c-array            pdbdata_to_c_array)
	    (synonym c-array->pdbdata            c_array_to_pdbdata)
	    (synonym c-array->pdbdata-i          c_array_to_pdbdata_i)
	    (synonym pm-lr->ac                   pm_lr_to_ac)
	    (synonym c-arrays->set               c_arrays_to_set)
	    (synonym pm-set-set-attribute!       pm_set_set_attributeb)
	    (synonym pm-connection->ac-domain    pm_connection_to_ac_domain)
	    (synonym have-spoke?                 have_spokep)))
	    
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; backward compatibility synonyms
; NOTE: remember the sense - (synonym select # cur pl)

; replaced pm-array with c-array to more closely mirror the C code
(if (defined? pdbview)
    (begin (synonym list->c-array         list->pm-array)
	   (synonym pa-variable->c-array  pa-variable->pm-array)
	   (synonym pdbdata->c-array      pdbdata->pm-array)
	   (synonym c-array?              pm-array?)
	   (synonym c-array-extrema       pm-array-extrema)
	   (synonym c-array-length        pm-array-length)
	   (synonym c-array-ref           pm-array-ref)
	   (synonym c-array-set!          pm-array-set!)
	   (synonym c-array->list         pm-array->list)
	   (synonym c-array->pdbdata      pm-array->pdbdata)
	   (synonym c-array->pdbdata-i    pm-array->pdbdata-i)
	   (synonym c-arrays->set         pm-arrays->set)))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; LETREC - temporarily reassign letrec to let* to ease the transition from
;        - the original incorrect definition of letrec

(define-macro (letrec . args)
   (printf nil "\nUse let* instead of letrec. It is the correct function.\n")
   (printf nil "In the future letrec will be defined to its standard meaning.\n\n")
   (set! letrec let*)
   (apply let* args))

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
