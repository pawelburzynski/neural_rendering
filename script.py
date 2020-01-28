print("running write_some_data...")
camera = bpy.data.objects['Camera']
mw = camera.matrix_world
scene = bpy.context.scene
frame = scene.frame_start
f = open("/home/pawelburzynski/Cambridge/PartII project/marble1/data.txt", 'w', encoding='utf-8')
while frame <= scene.frame_end:
    scene.frame_set(frame)
    x, y, z = mw.to_translation()
    rx, ry, rz = mw.to_euler('XYZ')
    f.write("%d" % frame)
    f.write(", ")
    f.write("%5.3f, %5.3f, %5.3f" % (x, y, z))
    f.write(", ")
    f.write("%5.3f, %5.3f, %5.3f" % (rx, ry, rz))
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