print("running write_some_data...")
camera = bpy.data.objects['Camera']
mw = camera.matrix_world
scene = bpy.context.scene
frame = scene.frame_start
path = '/home/pawelburzynski/Cambridge/PartII project/neural_rendering/main/Data/marble1/training/'
f = open(path+"data.txt", 'w', encoding='utf-8')
while frame <= scene.frame_end:
    scene.frame_set(frame)
    bpy.context.scene.render.filepath = path + str(frame) + ".png"
    bpy.ops.render.render(write_still=True)
    x, y, z = mw.to_translation()
    rx, ry, rz = mw.to_euler('XYZ')
    f.write("%d" % frame)
    f.write(", ")
    f.write("%5.3f, %5.3f, %5.3f" % (x, y, z))
    f.write("\n")
    frame += 1
f.close()

print("running write_some_data...")
camera = bpy.data.objects['Camera']
mw = camera.matrix_world
scene = bpy.context.scene
frame = scene.frame_start
path = '/home/pawelburzynski/Cambridge/PartII project/neural_rendering/main/Data/marble1/training/'
f = open(path+"data.txt", 'w', encoding='utf-8')
while frame <= scene.frame_end:
    scene.frame_set(frame)
    x, y, z = mw.to_translation()
    rx, ry, rz = mw.to_euler('XYZ')
    f.write("%d" % frame)
    f.write(", ")
    f.write("%5.3f, %5.3f, %5.3f" % (x, y, z))
    f.write("\n")
    frame += 1
f.close()



print("running write_some_data...")
camera = bpy.data.objects['Camera']
mw = camera.matrix_world
scene = bpy.context.scene
frame = scene.frame_start
f = open("/home/pawelburzynski/Cambridge/PartII project/marble1/data.txt", 'w', encoding='utf-8')
while frame <= scene.frame_end:
    scene.frame_set(frame)
    offset = bpy.context.object.constraints["Follow Path"].offset
    f.write("%d" % frame)
    f.write(", ")
    f.write("%.5f" % offset);
    f.write("\n")
    frame += 1
f.close()

seed(1)
camera = bpy.data.objects['Camera']
mw = camera.matrix_world
scene = bpy.context.scene
path = '/home/pawelburzynski/Cambridge/PartII project/neural_rendering/main/Data/marble1/eval/'
frame = 1
f = open(path+"data.txt", 'w', encoding='utf-8')
while frame <= 20:
    offset = uniform(0,100)
    bpy.context.object.constraints['Follow Path'].offset = offset
    bpy.context.scene.render.filepath = path + str(frame) + ".png"
    bpy.ops.render.render(write_still=True)
    x, y, z = mw.to_translation()
    f.write("%d" % frame)
    f.write(", ")
    f.write("%5.3f, %5.3f, %5.3f" % (x, y, z))
    f.write("\n")
    frame += 1
f.close()
