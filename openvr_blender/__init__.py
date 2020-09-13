# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTIBILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

bl_info = {
    "name" : "vr_platform",
    "author" : "Ibrahim Duran",
    "description" : "",
    "blender" : (2, 80, 0),
    "version" : (0, 0, 1),
    "location" : "",
    "warning" : "",
    "category" : "Generic"
}

import bpy
import _thread
import time
import socket
import struct
import math
import asyncore

from .data import Data

from bpy.props import (
    StringProperty,
    BoolProperty,
    IntProperty,
    FloatVectorProperty,
    FloatProperty,
    FloatVectorProperty,
    CollectionProperty,
    EnumProperty,
    PointerProperty,
)

from bpy.types import (
    Panel,
    Menu,
    Operator,
    PropertyGroup,
)

packer = struct.Struct('cfffffffIfff')

class DebuggerHandler(asyncore.dispatcher_with_send):

    obj = None
    buffer = bytes()

    def handle_close(self):
        if self.obj != None:
            self.obj.vr.is_debugging = False
        print("obj disc!")


    def handle_read(self):
        self.buffer += self.recv(30 - len(self.buffer))

        if len(self.buffer) != 30:
            return

        index = struct.unpack('B', self.buffer[0:1])[0]

        if self.obj == None:
            found = [ o for o in bpy.context.scene.objects if o.vr.device_index == index ]
            self.obj = found[0] if len(found) > 0 else None

        if self.obj != None:
            self.obj.vr.is_debugging = True

            # self.index = struct.unpack('I', bytes[0])
            # self.x = struct.unpack('f', buffer[1:5])[0]
            # self.y = struct.unpack('f', buffer[5:9])[0]
            # self.z = struct.unpack('f', buffer[9:13])[0]
            self.obj.vr.rx = struct.unpack('f', self.buffer[13:17])[0]
            self.obj.vr.ry = struct.unpack('f', self.buffer[17:21])[0]
            self.obj.vr.rz = struct.unpack('f', self.buffer[21:25])[0]
            self.obj.vr.rw = struct.unpack('f', self.buffer[25:29])[0]

            # self.obj.location[0] = self.obj.vr.x / 2
            # self.obj.location[2] = self.obj.vr.y / 5
            # self.obj.location[1] = self.obj.vr.z / 2 * -1
            # self.obj.rotation_quaternion = (self.obj.vr.rw, self.obj.vr.rx, -1 * self.obj.vr.rz, self.obj.vr.ry)

        self.buffer = bytes()

class DebuggerServer(asyncore.dispatcher):

    def __init__(self, host, port):
        asyncore.dispatcher.__init__(self)
        self.create_socket()
        self.set_reuse_addr()
        self.bind((host, port))
        self.listen(5)

    def handle_accepted(self, sock, addr):
        print('Incoming connection from %s' % repr(addr))
        handler = DebuggerHandler(sock)

server = DebuggerServer('0.0.0.0', 8889)
_thread.start_new_thread(lambda: asyncore.loop(), ())

def send_data(context, obj):
    scene = bpy.context.scene
    objname = obj.name

    host = scene.vr.ip
    port = scene.vr.port

    try:
        obj.vr.is_connected = True
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((host, port))
    except Exception as e:
        obj.vr.is_connected = False
        return

    is_touching_trackpad = False

    app_btn_was_pressed = False
    sys_btn_was_pressed = False
    grip_btn_was_pressed = False

    while (obj != None and obj.vr.is_connected):
        scene = bpy.context.scene

        try:
            time.sleep(scene.vr.delay)

            if not obj.vr.is_debugging:
                obj.vr.x = obj.location[0] * 2
                obj.vr.y = obj.location[2] * 5
                obj.vr.z = obj.location[1] * 2 * -1

                obj.vr.rx = obj.rotation_quaternion[1]
                obj.vr.ry = obj.rotation_quaternion[3]
                obj.vr.rz = -1 * obj.rotation_quaternion[2]
                obj.vr.rw = obj.rotation_quaternion[0]
            else:
                obj.location[0] = obj.vr.x / 2
                obj.location[2] = obj.vr.y / 5
                obj.location[1] = obj.vr.z / 2 * -1
                obj.rotation_quaternion = (obj.vr.rw, obj.vr.rx, -1 * obj.vr.rz, obj.vr.ry)

            buffer = struct.pack('B', obj.vr.device_index)
            buffer += struct.pack('f', obj.vr.x)
            buffer += struct.pack('f', obj.vr.y)
            buffer += struct.pack('f', obj.vr.z)
            buffer += struct.pack('f', obj.vr.rx)
            buffer += struct.pack('f', obj.vr.ry)
            buffer += struct.pack('f', obj.vr.rz)
            buffer += struct.pack('f', obj.vr.rw)

            if obj.vr.grip_btn:
                if not grip_btn_was_pressed:
                    buffer += struct.pack('B', 1 | (3 << 4))
                    buffer += struct.pack('B', 1)
                    grip_btn_was_pressed = True
            else:
                if grip_btn_was_pressed:
                    buffer += struct.pack('B', 1 | (3 << 4))
                    buffer += struct.pack('B', 0)
                    grip_btn_was_pressed = False

            if obj.vr.app_btn:
                if not app_btn_was_pressed:
                    buffer += struct.pack('B', 1 | (2 << 4))
                    buffer += struct.pack('B', 1)
                    app_btn_was_pressed = True
            else:
                if app_btn_was_pressed:
                    buffer += struct.pack('B', 1 | (2 << 4))
                    buffer += struct.pack('B', 0)
                    app_btn_was_pressed = False

            if obj.vr.sys_btn:
                if not sys_btn_was_pressed:
                    buffer += struct.pack('B', 1 | (1 << 4))
                    buffer += struct.pack('B', 1)
                    sys_btn_was_pressed = True
            else:
                if sys_btn_was_pressed:
                    buffer += struct.pack('B', 1 | (1 << 4))
                    buffer += struct.pack('B', 0)
                    sys_btn_was_pressed = False

            buffer += struct.pack('B', 0)
            # buffer += struct.pack('I', 0)
            # buffer += struct.pack('f', 0.0)
            # buffer += struct.pack('f', 0.0)
            # buffer += struct.pack('f', 0.0)

            if scene.vr.enabled:
                try:
                    s.send(buffer)
                    # s.send(('\n' + str(len(packet)) + '\n').encode())
                except Exception as e:
                    obj.vr.is_connected = False
                    break
        except ReferenceError:
            obj = [ o for o in bpy.context.scene.objects if o.name == objname ]
            obj = obj[0] if len(obj) > 0 else None

    s.close()


# ------------------------------------------------------------------------
#    Helper Functions
# ------------------------------------------------------------------------

def ShowMessageBox(message = "", title = "Message Box", icon = 'INFO'):

    def draw(self, context):
        self.layout.label(message)

    bpy.context.window_manager.popup_menu(draw, title = title, icon = icon)

# ------------------------------------------------------------------------
#    Scene Properties
# ------------------------------------------------------------------------

class VRSceneProperties(PropertyGroup):

    enabled: BoolProperty(
        name="Enable emulation",
        description="Enable emulation by connecting to the server.",
        default = False
    )

    delay: FloatProperty(
        name="Delay data transfer",
        description="Set custom delay value to slow down data transfer",
        default = 0.0,
        min = 0.0
    )

    ip: StringProperty(
        name="IP",
        description="Target IP address of the server",
        default="127.0.0.1",
        maxlen=1024,
    )

    port: IntProperty(
        name="Port",
        description="Target port of the server",
        default=8888,
        min=1,
        max=65535
    )

    debugger_host: StringProperty(
        name="Host",
        description="Select a host for the debug server",
        default="127.0.0.1"
    )

    debugger_port: IntProperty(
        name="Port",
        description="Select a port for the debug server",
        default=8889,
        min=1,
        max=65535
    )

    debugger_enabled: BoolProperty(
        name="Enable debugger",
        description="Enable debugger to simulate VR environment for devices",
        default=False
    )

# ------------------------------------------------------------------------
#    Object Properties
# ------------------------------------------------------------------------

class VRObjectProperties(PropertyGroup):

    data = Data()

    is_connected: BoolProperty(
        default=False
    )

    is_debugging: BoolProperty(
        name="Emulate debug values"
    )

    device_index: IntProperty(
        name="Device Index",
        description="Device index on the server",
        default=0,
        min=0
    )

    device_type: EnumProperty(
        name="Type",
        description="Select device type same as on the server",
        items=[
            ('HMD', 'HMD', "HMD device"),
            ('CONTROLLER', 'Controller', "Controller device")
        ]
    )

    trackpad: FloatVectorProperty(
        name="Trackpad",
        subtype="COLOR_GAMMA",
        min=0.0,
        max=1.0
    )

    trackpad_x: FloatProperty(
        name="Trackpad X",
        min=0.0,
        max=1.0
    )

    trackpad_y: FloatProperty(
        name="Trackpad Y",
        min=0.0,
        max=1.0
    )

    trigger: FloatProperty(
        name="Trigger",
        min=0.0,
        max=1.0
    )

    grip_btn: BoolProperty(
        name="Toggle grip"
    )

    sys_btn: BoolProperty(
        name="Toggle system"
    )

    app_btn: BoolProperty(
        name="Toggle application"
    )

    x: FloatProperty(name="X")
    y: FloatProperty(name="Y")
    z: FloatProperty(name="Z")
    rx: FloatProperty(name="Rx")
    ry: FloatProperty(name="Ry")
    rz: FloatProperty(name="Rz")
    rw: FloatProperty(name="Rw")


# ------------------------------------------------------------------------
#    Operators
# ------------------------------------------------------------------------

class WM_OT_Connect(Operator):
    bl_label = "Connect"
    bl_idname = "wm.connect"

    def execute(self, context):
        scene = context.scene
        mytool = scene.vr

        obj = context.selected_objects[0]

        _thread.start_new_thread(send_data, (context, obj))

        return {'FINISHED'}

class WM_OT_Disconnect(Operator):
    bl_label = "Disconnect"
    bl_idname = "wm.disconnect"

    def execute(self, context):
        scene = context.scene
        mytool = scene.vr

        obj = context.selected_objects[0]

        obj.vr['is_connected'] = False

        return {'FINISHED'}

class WM_OT_TriggerButton(Operator):
    bl_label = "Trigger"
    bl_idname = "wm.trigger_button"

    def execute(self, context):
        scene = context.scene
        mytool = scene.vr

        obj = context.selected_objects[0]

        obj.vr['is_connected'] = False

        return {'FINISHED'}

class WM_OT_GripButton(Operator):
    bl_label = "Grip"
    bl_idname = "wm.grip_button"

    def execute(self, context):
        scene = context.scene
        mytool = scene.vr

        obj = context.selected_objects[0]

        obj.vr['is_connected'] = False

        return {'FINISHED'}

class WM_OT_SystemButton(Operator):
    bl_label = "System"
    bl_idname = "wm.system_button"

    def execute(self, context):
        scene = context.scene
        mytool = scene.vr

        obj = context.selected_objects[0]

        del obj.vr.data
        obj.vr.data = None
        obj.vr['is_connected'] = False

        return {'FINISHED'}

class WM_OT_ApplicationButton(Operator):
    bl_label = "Application"
    bl_idname = "wm.application_button"

    def execute(self, context):
        scene = context.scene
        mytool = scene.vr

        obj = context.selected_objects[0]

        obj.vr['is_connected'] = False

        return {'FINISHED'}

class WM_OT_StartDebugger(Operator):
    bl_label = "Start Debugger"
    bl_idname = "wm.start_debugger"

    def execute(self, context):
        debugger_thread = _thread.start_new_thread(debugger_server, (context,))

        return {'FINISHED'}

class WM_OT_StopDebugger(Operator):
    bl_label = "Stop Debugger"
    bl_idname = "wm.stop_debugger"

    def execute(self, context):
        debugger_socket = None

        return {'FINISHED'}

# ------------------------------------------------------------------------
#    Panel in Object Mode
# ------------------------------------------------------------------------

class OBJECT_PT_VRSettingsPanel(Panel):
    bl_label = "VR Settings"
    bl_idname = "OBJECT_PT_vr_settings_panel"
    bl_space_type = "VIEW_3D"
    bl_region_type = "UI"
    bl_category = "Item"
    bl_context = "objectmode"


    @classmethod
    def poll(self,context):
        return context.object is not None

    def draw(self, context):
        layout = self.layout
        scene = context.scene

        # if scene.vr.debugger_socket == None:
        #     _thread.start_new_thread(debugger_server, (context))

        layout.label(text="Emulation Settings", icon="TOOL_SETTINGS")

        row = layout.row()
        row.prop(scene.vr, "ip") 
        row.prop(scene.vr, "port")
        layout.prop(scene.vr, "delay")
        layout.prop(scene.vr, "enabled")

        layout.separator()

        layout.label(text="Debugger Settings", icon="PLUGIN")
        row = layout.row()
        row.prop(scene.vr, "debugger_host")
        row.prop(scene.vr, "debugger_port")

        row = layout.row()
        row.prop(scene.vr, "debugger_enabled")

        # if debugger_socket == None:
        #     row.operator("wm.start_debugger")
        # else:
        #     row.operator("wm.stop_debugger")


class OBJECT_PT_VRMainPanel(Panel):
    bl_label = "VR Properties"
    bl_idname = "OBJECT_PT_vr_main_panel"
    bl_space_type = "VIEW_3D"
    bl_region_type = "UI"
    bl_category = "Item"
    bl_context = "objectmode"


    @classmethod
    def poll(self,context):
        return context.object is not None

    def draw(self, context):
        layout = self.layout
        scene = context.scene

        if context.selected_objects == None or len(context.selected_objects) != 1:
            layout.label(text="Select an object to view VR properties.")
            return

        obj = context.selected_objects[0]

        layout.prop(obj.vr, "device_index")
        layout.prop(obj.vr, "device_type")

        if obj.vr.device_type == 'CONTROLLER':
            y = -1 * math.cos(obj.vr.trackpad.h * 360 * math.pi / 180) * obj.vr.trackpad.s
            x = -1 * math.sin(obj.vr.trackpad.h * 360 * math.pi / 180) * obj.vr.trackpad.s

            layout.prop(obj.vr, "trackpad")
            layout.label(text='X: %.2f Y: %.2f' % (x, y))

            row = layout.row()
            row.operator("wm.trigger_button")
            row.prop(obj.vr, "trigger")

            row = layout.row()
            row.operator("wm.grip_button")
            row.prop(obj.vr, "grip_btn")
            
            row = layout.row()
            row.operator("wm.system_button")
            row.prop(obj.vr, "sys_btn")
            
            row = layout.row()
            row.operator("wm.application_button")
            row.prop(obj.vr, "app_btn")

        layout.separator()

        if obj.vr.is_connected == False:
            layout.operator("wm.connect")
        else:
            layout.operator("wm.disconnect")


class OBJECT_PT_VRDebugPanel(Panel):
    bl_label = "VR Debug"
    bl_idname = "OBJECT_PT_vr_debug_panel"
    bl_space_type = "VIEW_3D"
    bl_region_type = "UI"
    bl_category = "Item"
    bl_context = "objectmode"


    @classmethod
    def poll(self,context):
        return context.object is not None

    def draw(self, context):
        layout = self.layout
        scene = context.scene

        if context.selected_objects == None or len(context.selected_objects) != 1:
            layout.label(text="Select an object to view VR properties.")
            return

        obj = context.selected_objects[0]

        row = layout.row()
        row.label(text="Status: ")
        row.label(text="Remote" if obj.vr.is_debugging else "Local")

        row = layout.row()
        row.label(text="X:   %.02f" % obj.vr.x)
        row.label(text="Y:   %.02f" % obj.vr.y)
        row.label(text="Z:   %.02f" % obj.vr.z)
        row.label(text="")

        row = layout.row()
        row.label(text="Rx: %.04f" % obj.vr.rx)
        row.label(text="Ry: %.04f" % obj.vr.ry)
        row.label(text="Rz: %.04f" % obj.vr.rz)
        row.label(text="Rw: %.04f" % obj.vr.rw)


# ------------------------------------------------------------------------
#    Registration
# ------------------------------------------------------------------------

classes = (
    VRSceneProperties,
    VRObjectProperties,

    WM_OT_Connect,
    WM_OT_Disconnect,
    WM_OT_TriggerButton,
    WM_OT_GripButton,
    WM_OT_SystemButton,
    WM_OT_ApplicationButton,

    WM_OT_StartDebugger,
    WM_OT_StopDebugger,

    OBJECT_PT_VRSettingsPanel,
    OBJECT_PT_VRMainPanel,
    OBJECT_PT_VRDebugPanel,
)

def register():
    from bpy.utils import register_class
    for cls in classes:
        register_class(cls)

    bpy.types.Object.vr = PointerProperty(type=VRObjectProperties)
    bpy.types.Scene.vr = PointerProperty(type=VRSceneProperties)

def unregister():
    from bpy.utils import unregister_class
    for cls in reversed(classes):
        unregister_class(cls)

    del bpy.types.Object.vr
    del bpy.types.Scene.vr


if __name__ == "__main__":
    register()
