#generate training images
print("running write_some_data...")
camera = bpy.data.objects['Camera']
mw = camera.matrix_world
scene = bpy.context.scene
frame = scene.frame_start
path = '/home/pawelburzynski/Cambridge/PartII project/neural_rendering/main/Data/chair/training/'
while frame <= scene.frame_end:
    scene.frame_set(frame)
    bpy.context.scene.render.filepath = path + str(frame) + ".png"
    bpy.ops.render.render(write_still=True)
    frame += 1

#generate training metadata
print("running write_some_data...")
camera = bpy.data.objects['Camera']
mw = camera.matrix_world
scene = bpy.context.scene
frame = scene.frame_start
path = '/home/pawelburzynski/Cambridge/PartII project/neural_rendering/main/Data/chair/training/'
f = open(path+"data.txt", 'w', encoding='utf-8')
while frame <= scene.frame_end:
    scene.frame_set(frame)
    P, K, RT = get_3x4_P_matrix_from_blender(camera)
    x, y, z = mw.to_translation()
    rx, ry, rz = mw.to_euler('XYZ')
    f.write("%d" % frame)
    f.write(", ")
    f.write("%.4f, %.4f, %.4f" % (x, y, z))
    f.write("\n")
    f.write("%.4f, %.4f, %.4f, %.4f" % (P[0][0], P[0][1], P[0][2], P[0][3]))
    f.write("\n")
    f.write("%.4f, %.4f, %.4f, %.4f" % (P[1][0], P[1][1], P[1][2], P[1][3]))
    f.write("\n")
    f.write("%.4f, %.4f, %.4f, %.4f" % (P[2][0], P[2][1], P[2][2], P[2][3]))
    f.write("\n")
    frame += 1
f.close()

#generate eval metadata
import random
from random import uniform
random.seed(1)
camera = bpy.data.objects['Camera']
mw = camera.matrix_world
scene = bpy.context.scene
path = '/home/pawelburzynski/Cambridge/PartII project/neural_rendering/main/Data/chair/eval/'
frame = 1
f = open(path+"data.txt", 'w', encoding='utf-8')
while frame <= 20:
    offset = uniform(0,100)
    bpy.context.object.constraints['Follow Path'].offset = offset
    print(offset)
    P, K, RT = get_3x4_P_matrix_from_blender(camera)
    x, y, z = mw.to_translation()
    f.write("%d" % frame)
    f.write(", ")
    f.write("%.4f, %.4f, %.4f" % (x, y, z))
    f.write("\n")
    f.write("%.4f, %.4f, %.4f, %.4f" % (P[0][0], P[0][1], P[0][2], P[0][3]))
    f.write("\n")
    f.write("%.4f, %.4f, %.4f, %.4f" % (P[1][0], P[1][1], P[1][2], P[1][3]))
    f.write("\n")
    f.write("%.4f, %.4f, %.4f, %.4f" % (P[2][0], P[2][1], P[2][2], P[2][3]))
    f.write("\n")
    frame += 1
f.close()

#generate eval images
import random
from random import uniform
random.seed(1)
camera = bpy.data.objects['Camera']
mw = camera.matrix_world
scene = bpy.context.scene
path = '/home/pawelburzynski/Cambridge/PartII project/neural_rendering/main/Data/chair/eval/'
frame = 1
while frame <= 20:
    offset = uniform(0,100)
    bpy.context.object.constraints['Follow Path'].offset = offset
    bpy.context.scene.render.filepath = path + str(frame) + ".png"
    bpy.ops.render.render(write_still=True)
    frame += 1
