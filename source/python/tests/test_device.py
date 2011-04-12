#!/usr/bin/env python
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

#
# see gsatst.c
#


import pact.pgs as pgs
import unittest
#import test_leak
import work_gsatst

import Numeric
#import math
#from math import pi, sin, cos
from math import *

pdb = pgs

use_pause = True
single_frame = False

gsatst_N = 100

def set_line_info(info, type, axis_type, style, scatter,
                  marker, color, start, width):

    if type == pgs.INSEL:
        axis_type = pgs.INSEL
    elif axis_type == pgs.INSEL:
        axis_type = type
    info['PLOT-TYPE'] = type
    info['AXIS-TYPE'] = axis_type
    
    info['HIST-START'] = start
    info['SCATTER'] = scatter
    info['MARKER-INDEX'] = marker
    info['LINE-COLOR'] = color
    info['LINE-STYLE'] = style
    info['LINE-WIDTH'] = width

############################################################

class Device(unittest.TestCase):
#class Device(test_leak.Leak):
        
    def test1(self):
        dev = pgs.device("foo", "bar", "blat")
        self.failUnlessEqual(dev.name, "foo")
        self.failUnlessEqual(dev.type, "BAR")
        self.failUnlessEqual(dev.title, "blat")

    def test2(self):
        dev = pgs.device("SCREEN", "COLOR", "blat")
        dev.open(0.01, 0.01, 0.3, 0.3)
        dev.close()

    def test_pal(self):
        """Test Palette"""
        dev = pgs.device("SCREEN", "COLOR", "blat")
        dev.open(0.01, 0.01, 0.3, 0.3)
        pal = dev.set_palette("rainbow")
        self.failUnless(isinstance(pal, pgs.palette))
        self.failUnlessEqual(pal.name, "rainbow")
        dev.close()

############################################################

class Graph(unittest.TestCase):

    def test1(self):
        x, y = self.gen1d()
        graph = pgs.make_graph_1d(65, "Test Data #1",
                                  False, gsatst_N,
                                  x, y,
                                  "X Values", "Y Values")
        
        members = dir(graph)
        self.failUnless('data' in members)
        self.failUnless('identifier' in members)
        self.failUnless('info_type' in members)
        self.failUnless('mesh' in members)
        self.failUnless('rendering' in members)
        self.failUnless('use' in members)

        self.failUnless(isinstance(graph.data, pgs.graph))
        self.failUnlessEqual(graph.identifier, 65)
        self.failUnlessEqual(graph.info_type, 'pcons *')
        self.failUnlessEqual(graph.mesh, 0)
#        self.failUnlessEqual(graph.rendering, 0)
        self.failUnlessEqual(graph.use, None)

        info = graph.info
        self.failUnless(isinstance(info, pgs.assoc))
        self.failUnlessEqual(len(info), 8)
        self.failUnlessEqual(info['HIST-START'], 0)
        self.failUnlessEqual(info['LINE-COLOR'], 1)
        self.failUnlessEqual(info['MARKER-INDEX'], 0)
        self.failUnlessEqual(info['AXIS-TYPE'], -1)
        self.failUnlessEqual(info['LINE-STYLE'], 1)
        self.failUnlessEqual(info['PLOT-TYPE'], -1)
        self.failUnlessEqual(info['SCATTER'], 0)
        self.failUnlessEqual(info['LINE-WIDTH'], 0)

        # test assignment
        graph.info = None
        # self.failUnlessEqual(graph.info, {})
        # should raise TypeError
        # del graph.info


############################################################

class Image(unittest.TestCase):
        
    def test1(self):
        X_max = 250
        Y_max = 250
        sf    = 255
        calc_im = pgs.make_image('Test Image', 'char', None,
                                 0.0, 0.0, 0.0, 0.0, 0.0, sf,
                                 X_max, Y_max, 4, None)
        self.failUnless(isinstance(calc_im, pgs.image))
        self.failUnlessEqual(calc_im.version_id, 0)
        self.failUnlessEqual(calc_im.label, 'Test Image')
        self.failUnlessEqual(calc_im.xmin, 0.0)
        self.failUnlessEqual(calc_im.xmax, 250.0)
        self.failUnlessEqual(calc_im.ymin, 0.0)
        self.failUnlessEqual(calc_im.ymax, 250.0)
        self.failUnlessEqual(calc_im.zmin, 0.0)
        self.failUnlessEqual(calc_im.zmax, 255.0)
        self.failUnlessEqual(calc_im.element_type, 'char *')

        im = calc_im.buffer
        self.failUnlessEqual(type(im), type(Numeric.array(1)))
        self.failUnlessEqual(im.shape, (250, 250))

        self.failUnlessEqual(calc_im.kmax, 250)
        self.failUnlessEqual(calc_im.lmax, 250)
        self.failUnlessEqual(calc_im.size, 62500)
        self.failUnlessEqual(calc_im.bits_pixel, 4)
        self.failUnless(isinstance(calc_im.palette, pgs.palette))

############################################################

palettes = ["rainbow", "bw", "iron", "contrast", "cym"]
clrs = ["BLACK", "WHITE", "GRAY", "DARK_GRAY",
        "BLUE", "GREEN", "CYAN", "RED", "MAGENTA", "BROWN",
        "DARK_BLUE", "DARK_GREEN", "DARK_CYAN", "DARK_RED",
        "YELLOW", "DARK_MAGENTA"]
        

def gen1d():
    x = []
    y = []
    for i in xrange(gsatst_N):
        temp = - (i + 1) / 10.0
        x.append(temp)
        y.append(6.022e23 / temp)
    return x, y

def gen_test_2():
    # prepare the data
    n = gsatst_N
    ip = pgs.get_processor_number();
    np = pgs.get_number_processors();
    n  /= np;
    imn = ip*n;
    imx = imn + n;

    dx = 0.1*(float(imx - imn))/(float(gsatst_N) - 1.0);

    x = []
    y = []
    for i in range(imn, imx):
        xv = i*dx
        x.append(xv)
        y.append(cos(xv))
    return (x,y)


def compute_2d_data(kmax, lmax):
    #   REAL *f, *x, *y;
    #   int kmax, lmax;
    #   {int k, l, i;
    #    REAL xmin, xmax, ymin, ymax;
    #    REAL dx, dy, tol, t, r, fv;

    xmin = -5.0
    xmax =  5.0
    ymin = -5.0
    ymax =  5.0

    dx = (xmax - xmin) / float(kmax - 1.0)
    dy = (xmax - xmin) / float(kmax - 1.0)
    
    tol = 1.0e-5

    x = Numeric.zeros((kmax, lmax), Numeric.Float)
    y = Numeric.zeros((kmax, lmax), Numeric.Float)
    f = Numeric.zeros((kmax, lmax), Numeric.Float)
    SMALL = .000001

    for k in range(kmax):
        for l in range(lmax):
             xvalue = k*dx + xmin
             yvalue = l*dy + ymin
             x[k,l] = xvalue
             y[k,l] = yvalue
             r    = xvalue*xvalue + yvalue*yvalue
             t    = 5.0*atan(yvalue/(xvalue + SMALL))
             r    = pow(r, 0.125)
             fv   = exp(-r)*(1.0 + 0.1*cos(t))
             f[k,l] = floor(fv/tol)*tol

    return f, x, y


class Gsatst(unittest.TestCase):
    def setUp(self):
        dev = pgs.device("SCREEN", "COLOR", "blat")
        dev.open(0.01, 0.01, 0.3, 0.3)
        self.dev = dev

    def tearDown(self):
        self.dev.close
        
    def test_1(self):
        """gsatst.c test_1"""
        dev = self.dev
        x,y = gen1d()
        data = pgs.make_graph_1d(65, "Test Data #1",
                                  False, gsatst_N,
                                  x, y,
                                  "X Values", "Y Values")

        # pgs.plot_labels = pdb.TRUE

        data.info = pgs.set_line_info(data.info, pgs.CARTESIAN, pgs.CARTESIAN,
                                      pgs.SOLID, pdb.FALSE, 0, dev.BLUE,
                                      0, 0.0)
        dev.draw_graph(data)
        if use_pause:
            pgs.pause()

        data.info = pgs.set_line_info(data.info, pgs.POLAR, pgs.POLAR,
                                      pgs.SOLID, pdb.FALSE, 0, dev.GREEN,
                                      0, 0.0)
        dev.draw_graph(data)
        if use_pause:
            pgs.pause()

#        set_line_info(info, pgs.INSEL, pgs.INSEL,
#                      pgs.SOLID, FALSE, 0, dev.RED,
#                      0, 0.0);
        data.info = pgs.set_line_info(data.info, pgs.INSEL, pgs.INSEL,
                                      pgs.SOLID, pdb.FALSE, 0, dev.RED,
                                      0, 0.0)
        dev.draw_graph(data)
        if use_pause:
            pgs.pause()

#        set_line_info(info, pgs.HISTOGRAM, pgs.CARTESIAN,
#                      pgs.SOLID, FALSE, 0, dev.WHITE,
#                      0, 0.0)
#        data.info = info
        data.info = pgs.set_line_info(data.info, pgs.HISTOGRAM, pgs.CARTESIAN,
                                      pgs.SOLID, pdb.FALSE, 0, dev.WHITE,
                                      0, 0.0)
        dev.draw_graph(data)
        if use_pause:
            pgs.pause()

    def test_2(self):
        dev = self.dev

        x,y = gen_test_2()

        data = pgs.make_graph_1d(66, "Test Data #2",
                                 False, gsatst_N,
                                 x, y,
                                 "X Values", "Y Values")
        #    PG_plot_labels = pdb.TRUE;

        # PG_turn_grid(dev, pdb.ON);

        data.info = pgs.set_line_info(data.info, pgs.HISTOGRAM, pgs.CARTESIAN,
                                      pgs.SOLID, pdb.FALSE, 0, dev.MAGENTA,
                                      1, 0.0);
        dev.draw_graph(data)
        if use_pause:
            pgs.pause()

        data.info = pgs.set_line_info(data.info, pgs.POLAR, pgs.POLAR,
                                      pgs.SOLID, pdb.FALSE, 0, dev.CYAN,
                                      0, 0.0)
        dev.draw_graph(data)
        if use_pause:
            pgs.pause()

        data.info = pgs.set_line_info(data.info, pgs.INSEL, pgs.INSEL,
                                      pgs.SOLID, pdb.FALSE, 0, dev.YELLOW,
                                      0, 0.0)
        dev.draw_graph(data)
        if use_pause:
            pgs.pause()

        data.info = pgs.set_line_info(data.info, pgs.CARTESIAN, pgs.CARTESIAN,
                                      pgs.SOLID, pdb.FALSE, 0, dev.DARK_GRAY,
                                      0, 0.0);

        dev.draw_graph(data)
        if use_pause:
            pgs.pause()

        #    PG_rl_graph(data, TRUE, TRUE);

        # PG_turn_grid(dev, pdb.OFF);

    def test_3(self):
        dev = self.dev

        # prepare the data
        X_max  = 250
        Y_max  = 250
        sf     = 255

        calc_im = pgs.make_image("Test Image", "char", None,
                                 0.0, 0.0, 0.0, 0.0, 0.0, sf,
                                 X_max, Y_max, 4, None)

        pl = dev.set_palette(palettes[1])

        # compute the first image
        bf = calc_im.buffer
        for l in range(Y_max):
            for k in range(X_max):
                bf[k,l] = sf*(float(k))*(float(l)) / \
                          (float((Y_max - 1)*(X_max - 1)))

        # draw the first image
        dev.draw_image(calc_im, "Test Data A", None)
        if use_pause:
            pgs.pause()

        # compute the second image
        pl = dev.set_palette(palettes[2])

        dx = 2.0*pi / float(X_max)
        dy = 2.0*pi / float(Y_max)
#        bf = calc_im.buffer
        for l in range(Y_max):
            for k in range(X_max):
                xv = k*dx;
                yv = ((Y_max - l - 1))*dy;
                bf[k,l] = sf*(0.5 + 0.5*sin(xv)*cos(yv))

#        pgs.plot_labels = pdb.FALSE;

        # draw the second image
        dev.draw_image(calc_im, "Test Data B", None);
        if use_pause:
            pgs.pause()

    def sf_dt(self, dev, x1, y1, face, style, size):
        """set the font and draw the text """
        
        dev.set_font(face, style, size)

        dx, dy = dev.get_text_ext("foo")

        # write some text and draw a box around it
        dev.write_WC(x1, y1, "%s", "foo")

        x2 = x1 + dx
        y2 = y1 + dy
        dev.draw_box(x1, x2, y1, y2)

    def aux_test_4(self, dev):

        # set_color_line
        dev.set_color_line(dev.BLACK, pdb.TRUE)
        dev.set_color_text(dev.BLACK, pdb.TRUE)

        self.sf_dt(dev, 0.1, 0.9, "helvetica", "medium", 12)
        self.sf_dt(dev, 0.1, 0.8, "helvetica", "italic", 12)
        self.sf_dt(dev, 0.1, 0.7, "helvetica", "bold", 12)
        self.sf_dt(dev, 0.1, 0.6, "helvetica", "bold-italic", 12)

        self.sf_dt(dev, 0.1, 0.50, "helvetica", "medium", 10)
        self.sf_dt(dev, 0.1, 0.45, "helvetica", "italic", 10)
        self.sf_dt(dev, 0.1, 0.40, "helvetica", "bold", 10)
        self.sf_dt(dev, 0.1, 0.35, "helvetica", "bold-italic", 10)

        self.sf_dt(dev, 0.1, 0.30, "helvetica", "medium", 8)
        self.sf_dt(dev, 0.1, 0.25, "helvetica", "italic", 8)
        self.sf_dt(dev, 0.1, 0.20, "helvetica", "bold", 8)
        self.sf_dt(dev, 0.1, 0.15, "helvetica", "bold-italic", 8)

        self.sf_dt(dev, 0.3, 0.9, "times", "medium", 12)
        self.sf_dt(dev, 0.3, 0.8, "times", "italic", 12)
        self.sf_dt(dev, 0.3, 0.7, "times", "bold", 12)
        self.sf_dt(dev, 0.3, 0.6, "times", "bold-italic", 12)
        
        self.sf_dt(dev, 0.3, 0.50, "times", "medium", 10)
        self.sf_dt(dev, 0.3, 0.45, "times", "italic", 10)
        self.sf_dt(dev, 0.3, 0.40, "times", "bold", 10)
        self.sf_dt(dev, 0.3, 0.35, "times", "bold-italic", 10)

        self.sf_dt(dev, 0.3, 0.30, "times", "medium", 8)
        self.sf_dt(dev, 0.3, 0.25, "times", "italic", 8)
        self.sf_dt(dev, 0.3, 0.20, "times", "bold", 8)
        self.sf_dt(dev, 0.3, 0.15, "times", "bold-italic", 8)

        self.sf_dt(dev, 0.5, 0.9, "courier", "medium", 12)
        self.sf_dt(dev, 0.5, 0.8, "courier", "italic", 12)
        self.sf_dt(dev, 0.5, 0.7, "courier", "bold", 12)
        self.sf_dt(dev, 0.5, 0.6, "courier", "bold-italic", 12)

        self.sf_dt(dev, 0.5, 0.50, "courier", "medium", 10)
        self.sf_dt(dev, 0.5, 0.45, "courier", "italic", 10)
        self.sf_dt(dev, 0.5, 0.40, "courier", "bold", 10)
        self.sf_dt(dev, 0.5, 0.35, "courier", "bold-italic", 10)

        self.sf_dt(dev, 0.5, 0.30, "courier", "medium", 8)
        self.sf_dt(dev, 0.5, 0.25, "courier", "italic", 8)
        self.sf_dt(dev, 0.5, 0.20, "courier", "bold", 8)
        self.sf_dt(dev, 0.5, 0.15, "courier", "bold-italic", 8)

        dev.update_vs()

    def test_4(self):
        dev = self.dev
        
        dev.clear_window()  # screen

        dev.set_viewport(0.1, 0.9, 0.2, 0.8)
        dev.set_window(0.0, 1.0, 0.0, 1.0)
        dev.draw_box(-0.02, 1.02, -0.02, 1.02)

        # /*    PG_expose_device(PG_console_device) */

        self.aux_test_4(dev)

        # reset for following tests
        dev.set_font("helvetica", "medium", 10)

        dev.finish_plot()
        if use_pause:
            pgs.pause()

        # do oriented text test
        dev.clear_window()   # screen
        dev.set_window(-3.0, 3.0, -3.0, 3.0)

        for i in range(8):
            an = i*pi / 4.0
            x0 = cos(an)
            y0 = sin(an)
            dev.set_char_path(x0, y0)
            # write_WC uses varargs
            dev.write_WC(x0, y0, "Sample", "")

        # reset for following tests
        dev.set_char_path(1.0, 0.0)

        dev.finish_plot()
        if use_pause:
            pgs.pause()

    def test_5(self):
        dev = self.dev
        kmax = 20
        lmax = 20
        kxl = kmax * lmax
        id = 65  # A
        lncolor = dev.BLUE
        lnwidth = 0.0
        lnstyle = pgs.SOLID
        centering = -2 # N_CENT

        f, x, y = compute_2d_data(kmax, lmax);

        fmin, fmax = pgs.iso_limit(f, kxl);

        data = pgs.make_graph_r2_r1(id, "{x, y}->f", False,
                                    kmax, lmax, centering, x, y, f,
                                    "xy", "f")

        data.info = pgs.set_tds_info(data.info, pgs.PLOT_CONTOUR, pgs.CARTESIAN,
                                     lnstyle, lncolor, 10, 1.0,
                                     lnwidth, 0.0, 0.0, 0.0, pdb.HUGE)

        # PG_plot_labels = pdb.FALSE;
        
        dev.turn_data_id(pdb.OFF)

        dev.contour_plot(data)
        if use_pause:
            pgs.pause()

        dev.turn_data_id(pdb.ON)

    def test_7(self):
        dev = self.dev
        kmax = 20
        lmax = 20
        kxl = kmax * lmax
        id = 65  # A
        centering = -2 # N_CENT

        f, x, y = compute_2d_data(kmax, lmax);

        data = pgs.make_graph_r2_r1(id, "{x, y}->f", False,
                                    kmax, lmax, centering, x, y, f,
                                    "xy", "f")

        # PG_plot_labels = pdb.FALSE;

        dev.set_palette(palettes[3])
        dev.poly_fill_plot(data)
        if use_pause:
            pgs.pause()

    def test_8(self):
        dev = self.dev
        kmax = 10
        lmax = 10
        kxl = kmax * lmax
#        id = 65  # A
        centering = -2 # N_CENT

        rz, rx, ry = compute_2d_data(kmax, lmax);

        rs = Numeric.exp(rz)

        maxes = Numeric.array([kmax, lmax], 'i')

        #emap = CMAKE_N(char, kxl);
        #memset(emap, 1, kxl);
        #alist = PG_set_attrs_alist(NULL,
        #                           "EXISTENCE", SC_CHAR_I, TRUE, emap,
        #                            "EXISTENCE", SC_CHAR_I, pdb.TRUE, emap,
        #                           NULL);

        #ext = CMAKE_N(REAL, 4);
        #ext[0] = -1;
        #ext[1] =  1;
        #ext[2] = -1;
        #ext[3] =  1;
        ext = Numeric.array([-1., 1., -1., 1.], 'f')

        if single_frame:
            dtheta = pdb.HUGE
            dphi   = pdb.HUGE
            dchi   = pdb.HUGE
        else:
            dtheta = 45.0
            dphi   = 45.0
            dchi   = 30.0

        #PG_plot_labels = pdb.FALSE;

        # draw the wire frames
        dev.set_palette("standard")

        alist = None
        PM_LR_S = "Logical-Rectangular"

        theta = 0.0
        while theta <= 90.0:
            phi = 0.0
            while phi <= 90.0:
                chi = 30.0
                while chi <= 60.0:
                    dev.draw_surface(rz, rz, ext, rx, ry, kxl,
                                     0.0, 2.0*pi, 0.0, 2.0*pi,
                                     theta, phi, chi,
                                     0.0, dev.BLUE, pgs.SOLID,
                                     pgs.PLOT_WIRE_MESH, "Test Data",
                                     PM_LR_S, maxes, alist)
                    if use_pause:
                        pgs.pause()
                    chi += dchi
                phi += dphi;
            theta += dtheta

    def test_10(self):
        dev = self.dev

        clrs = ["BLACK", "WHITE", "GRAY", "DARK_GRAY",
                "BLUE", "GREEN", "CYAN", "RED", "MAGENTA", "BROWN",
                "DARK_BLUE", "DARK_GREEN", "DARK_CYAN", "DARK_RED",
                "YELLOW", "DARK_MAGENTA"]
        sty = [pgs.LINE_SOLID, pgs.LINE_DOTTED,
               pgs.LINE_DASHED, pgs.LINE_DOTDASHED]

        dev.set_viewport(0.05, 0.95, 0.05, 0.95)
        dev.set_window(0.0, 1.0, 0.0, 1.0)
        dev.set_palette("standard");

        # colors
        dev.clear_window()  #  dev.clear_screen()

        n  = 16
        dy = 1.0 / float(n + 1.0)
        y  = 0.5*dy
        for i in range(n):
            dev.set_color_line(i, pgs.TRUE)
            dev.draw_line(0.0, y, 0.5, y)

            dev.set_color_text(i, pgs.TRUE)
            stmp = "%d %s" % (i, clrs[i])
            dev.write_WC(0.6, y, "%s", "%d %s" % (i, clrs[i]));
            # dev.write_WC(0.6, y, "%d %s", i, clrs[i]);

            y += dy

        dev.finish_plot()
        if use_pause:
            pgs.pause()

        #  widths and styles
        dev.set_line_color(dev.WHITE);
        dev.clear_window()  # dev.clear_screen()

        n  = 10
        dy = 1.0 /  float(n + 1.0)
        y  = 0.5*dy
        for i in range(n):
            w = 0.3*i;
            dev.set_line_width(w)
            for j in range(4):
                xmn = 0.25*j + 0.05
                xmx = 0.25*(j+1) - 0.05
                dev.set_line_style(sty[j])
                dev.draw_line(xmn, y, xmx, y);

            y += dy

        dev.finish_plot()
        if use_pause:
            pgs.pause()


    def xtest_11(self):
        """plotting AC data sets"""
        dev = self.dev

        #int kmax, lmax, kxl;
        #int centering;
        #REAL *x, *y;
        #PM_mapping *f;

        # set up data
        centering = pdb.Z_CENT
        kmax      = 11
        lmax      = 11
        # kxl       = kmax*lmax
        # x         = CMAKE_N(REAL, kxl)
        # y         = CMAKE_N(REAL, kxl)

        f = work_gsatst.compute_2d_ac_mapping_2(kmax, lmax)

        #pgs.set_attrs_mapping(f,
        #                      "PALETTE", SC_CHAR_I, TRUE, "rainbow",
        #                      NULL)

        # pgs.plot_labels = FALSE

        print f
        dev.draw_picture(f, pgs.PLOT_CONTOUR,
		    pdb.TRUE, dev.BLUE, pgs.LINE_SOLID,  0.4,
		    pdb.TRUE, dev.RED,  pgs.LINE_DOTTED, 0.1)
        if use_pause:
            pgs.pause()

        dev.draw_picture(f, pgs.PLOT_FILL_POLY,
                         pdb.TRUE, dev.BLUE, pgs.LINE_SOLID, 0.4,
                         pdb.TRUE, dev.RED, pgs.LINE_DOTTED, 0.1)
        if use_pause:
            pgs.pause()

        dev.draw_picture(f, pgs.PLOT_SURFACE,
                         pdb.TRUE, dev.BLUE, pgs.LINE_SOLID, 0.4,
                         pdb.TRUE, dev.RED, pgs.LINE_DOTTED, 0.1)
        if use_pause:
            pgs.pause()

        dev.draw_picture(f, pgs.PLOT_WIRE_MESH,
                         pdb.TRUE, dev.BLUE, pgs.LINE_SOLID, 0.4,
                         pdb.TRUE, dev.RED, pgs.LINE_DOTTED, 0.1)
        if use_pause:
            pgs.pause()

        dev.draw_picture(f, pgs.PLOT_MESH,
                         pdb.TRUE, dev.BLUE, pgs.LINE_SOLID, 0.4,
                         pdb.TRUE, dev.RED, pgs.LINE_DOTTED, 0.1)
        if use_pause:
            pgs.pause()

        # PM_rel_mapping(f, TRUE, TRUE)


if __name__ == '__main__':
    unittest.main()
        
