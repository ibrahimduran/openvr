import struct
import bpy
import math

packer = struct.Struct('IfffffffIfff')

class Data:
    connected = False

    index = 0
    x = 0.0
    y = 0.0
    z = 0.0
    Rx = 0.0
    Ry = 0.0
    Rz = 0.0
    Rw = 0.0

    actions = []

    def pack(self):
        buffer = struct.pack('B', self.index)
        buffer += struct.pack('f', self.x)
        buffer += struct.pack('f', self.y)
        buffer += struct.pack('f', self.z)
        buffer += struct.pack('f', self.Rx)
        buffer += struct.pack('f', self.Ry)
        buffer += struct.pack('f', self.Rz)
        buffer += struct.pack('f', self.Rw)
        buffer += struct.pack('I', 0)
        buffer += struct.pack('f', 0.0)
        buffer += struct.pack('f', 0.0)
        buffer += struct.pack('f', 0.0)


        for i in range(0, 48 - len(buffer)): 
            buffer += struct.pack('B', 0)

        return buffer

    def unpack(self, buffer):
        # self.index = struct.unpack('I', bytes[0])
        # self.x = struct.unpack('f', buffer[1:5])[0]
        # self.y = struct.unpack('f', buffer[5:9])[0]
        # self.z = struct.unpack('f', buffer[9:13])[0]
        self.Rx = struct.unpack('f', buffer[13:17])[0]
        self.Ry = struct.unpack('f', buffer[17:21])[0]
        self.Rz = struct.unpack('f', buffer[21:25])[0]
        self.Rw = struct.unpack('f', buffer[25:29])[0]

        self.Rw = abs(self.Rw)

    def from_obj(self, obj):
        self.x = obj.location[0] * 2
        self.y = obj.location[2] * 5
        self.z = obj.location[1] * 2 * -1

        self.Rx = obj.rotation_quaternion[1]
        self.Ry = obj.rotation_quaternion[3]
        self.Rz = obj.rotation_quaternion[2]
        self.Rw = obj.rotation_quaternion[0]

        # if not is_touching_trackpad and obj.vr.trackpad.v > 0.95:
        #     is_touching_trackpad = True

        #     trackpadX = -1 * math.sin(obj.vr.trackpad.h * 360 * math.pi / 180) * obj.vr.trackpad.s
        #     trackpadY = -1 * math.cos(obj.vr.trackpad.h * 360 * math.pi / 180) * obj.vr.trackpad.s

        #     self.actions.append((0, bytes(0)))
        #     self.actions.append((1, bytes(0)))

        # elif is_touching_trackpad and obj.vr.trackpad.v < 0.95:
        #     is_touching_trackpad = False                

        # is_touching_trackpad = obj.vr.trackpad.v > 0.95

        # if is_touching_trackpad:
        #     trackpadX = -1 * math.sin(obj.vr.trackpad.h * 360 * math.pi / 180) * obj.vr.trackpad.s
        #     trackpadY = -1 * math.cos(obj.vr.trackpad.h * 360 * math.pi / 180) * obj.vr.trackpad.s
        # else:
        #     trackpadX = 0.0
        #     trackpadY = 0.0

        # trigger = obj.vr.trigger

        # gripBtn = '1' if obj.vr.grip_btn else '0'
        # sysBtn = '1' if obj.vr.sys_btn else '0'
        # appBtn = '1' if obj.vr.app_btn else '0'

        # buttons = int(gripBtn + sysBtn + appBtn, 2)

    def to_obj(self, obj):
        obj.location[0] = self.x / 2
        obj.location[2] = self.y / 5
        obj.location[1] = self.z / 2 * -1
        # self.x = obj.location[0] * 2
        # self.y = obj.location[2] * 5
        # self.z = obj.location[1] * 2 * -1

        obj.rotation_quaternion[1] = self.Rx
        obj.rotation_quaternion[3] = self.Ry
        obj.rotation_quaternion[2] = self.Rz
        obj.rotation_quaternion[0] = self.Rw
