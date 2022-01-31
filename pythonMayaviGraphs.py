from mayavi import mlab
from requests import get
import numpy as np

[phi_s, theta_s] = np.mgrid[0:2 * np.pi:36j, 0:np.pi / 2:9j]
x = np.cos(phi_s) * np.sin(theta_s)
y = np.sin(phi_s) * np.sin(theta_s)
z = np.cos(theta_s)

def drawAP(r, pos, color):
    mlab.points3d([pos[0]], [pos[1]], [pos[2]], color=(1, 1, 1), scale_factor=2) #Punto central
    return mlab.mesh(r * x + pos[0], r * y + pos[1], r * z + pos[2], color=color, representation="fancymesh", tube_radius=0.1)

def updateAP(r, pos, mesh):
    mesh.mlab_source.set(x= r * x + pos[0], y= r * y + pos[1], z= r * z + pos[2])

def updatePos(x, y, z, point, text):
    point.mlab_source.set(x=x, y=y, z=z)
    text.position = (x + 0.75, y + 0.75, z + 1)
    text.text = "({:.2f}, {:.2f}, {:.2f})".format(x, y, z)

def rssiToDistance(rssi0, rssi1, rssi2): #Esta función debe ser reemplazada por una red neuronal entrenada
    r0 = 30
    r1 = 30
    r2 = 30
    return (r0, r1, r2)

mlab.figure('Posición (x, y, z)')
pos0 = (0, 0, 0)
pos1 = (30, 0, 0)
pos2 = (30, 30, 0)
mesh0 = drawAP(30, pos0, (1, 0.2, 0.2))
mesh1 = drawAP(30, pos1, (0.2, 1, 0.2))
mesh2 = drawAP(30, pos2, (0.2, 0.2, 1))
pointC = mlab.points3d([0], [0], [0], color=(1, 1, 1), scale_factor=0.7)
pointCText = mlab.text3d(1, 1, 1, "({}, {}, {})".format(0, 0, 0), scale=2)
pointC.visible = False

mlab.outline(color=(0.7, 0.7, 0.7), extent=[-30, 60, -30, 60, 0, 120])
mlab.axes(xlabel='x', ylabel='y', zlabel='z')
mlab.view(270, 55, 255, (15, 15, 15))

f2 = mlab.figure("Posición (r0, r1, r2)")

Rx, Ry, Rz = np.mgrid[0:80:50j, 0:80:50j, 0:80:50j]
F = -Rx**4 + 2*Rx**2*Ry**2 + 1800*Rx**2 - 2*Ry**4 + 2*Ry**2*Rz**2 - Rz**4 + 1800*Rz**2 - 1620000
iso = mlab.contour3d(F, contours=[0], color=(0, 0.7, 0.7), extent=[0, 80, 0, 80, 0, 80])
iso.actor.property.representation = 'wireframe'
iso.actor.property.line_width = 1

pointR = mlab.points3d([0], [0], [0], color=(0, 1, 0), scale_factor=1.5)
pointR.glyph.scale_mode = 'data_scaling_off'

pointR.mlab_source.dataset.point_data.scalars = [[0, 255, 0, 255]]

pointRText = mlab.text3d(1, 1, 1, "({}, {}, {})".format(0, 0, 0), scale=2)

mlab.outline(color=(0.7, 0.7, 0.7), extent=[0, 80, 0, 80, 0, 80])
mlab.axes(xlabel='r0', ylabel='r1', zlabel='r2')
mlab.view(250, 80, 170, (30, 10, 35))

@mlab.animate(delay=10, ui=False)
def anim():
    while True:
        #print("update")
        rssi0, rssi1, rssi2 = np.array(get("http://192.168.100.150").text.split(','), int)
        r0, r1, r2 = rssiToDistance(rssi0, rssi1, rssi2)
        updateAP(r0, pos0, mesh0)
        updateAP(r1, pos1, mesh1)
        updateAP(r2, pos2, mesh2)
        updatePos(r0, r1, r2, pointR, pointRText)

        disc = -r0**4 + 2 * r0**2 * r1**2 + 1800 * r0**2 - 2 * r1**4 + 2 * r1**2 * r2**2 - r2**4 + 1800 * r2**2 - 1620000
        if disc > 0:
            print("green")
            #pointR.mlab_source.dataset.point_data.scalars = [0]
            pointC.visible = True
            pointCText.visible = True
            xp = r0**2/60 - r1**2/60 + 15
            yp = r1**2/60 - r2**2/60 + 15
            zp = N(5/300*sqrt(disc))
            updatePos(xp, yp, zp, pointC, pointCText)
        else:
            print("red")
            #pointR.mlab_source.dataset.point_data.scalars = [1]
            pointC.visible = False
            pointCText.visible = False

        yield

a = anim()

mlab.show()
