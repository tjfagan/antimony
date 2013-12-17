import wx

import editor
import point

class Canvas(wx.Panel):
    def __init__(self, parent, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)
        self.Bind(wx.EVT_PAINT, self.paint)

        pt = point.Point('asdf',0, 0)
        ctrl = point.PointControl(self, pt)
        ed = editor.Editor(self, pt)
        ed.MoveXY(20, 20)

        self.center = wx.RealPoint(0, 0)
        self.scale = 10.0 # scale is measured in pixels/mm
        self.mouse_pos = wx.Point(0, 0)
        self.dragging = False

        self.Bind(wx.EVT_LEFT_DOWN, self.start_drag)
        self.Bind(wx.EVT_LEFT_UP, self.stop_drag)
        self.Bind(wx.EVT_MOTION, self.on_motion)

    def paint(self, event=None):
        for c in self.Children: c.reposition()

        dc = wx.PaintDC(self)
        dc.SetBackground(wx.Brush((20, 20, 20)))
        dc.Clear()

        x, y = self.mm_to_pixel(0, 0)
        dc.SetPen(wx.Pen((255, 0, 0), 2))
        dc.DrawLine(x, y, x + 50, y)
        dc.SetPen(wx.Pen((0, 255, 0), 2))
        dc.DrawLine(x, y, x, y - 50)

    def mm_to_pixel(self, x, y):
        """ Converts an x,y position in mm into an i,j coordinate.
        """
        return (int((x - self.center.x) * self.scale + self.Size.x/2),
                int((self.Size.y/2) - (y - self.center.y) * self.scale))

    def start_drag(self, event):    self.dragging = True
    def stop_drag(self, event):     self.dragging = False

    def on_motion(self, event):
        delta = event.GetPosition() - self.mouse_pos
        if self.dragging:
            self.center += wx.RealPoint(-delta.x / self.scale,
                                        delta.y / self.scale)
            self.Refresh()
        self.mouse_pos = event.GetPosition()


