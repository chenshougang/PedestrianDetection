# 安装busybox, 需要里面的devmem工具
# sudo apt install busybox

# pinmux.0x0c303000 and pinmux.0x0c303008 are for CAN1
# pinmux.0x0c303010 and pinmux.0x0c303018 are for CAN0
# 检查当前的寄存器值
sudo busybox devmem 0x0c303000	# 0x0000C055
sudo busybox devmem 0x0c303008	# 0x0000C055
sudo busybox devmem 0x0c303010	# 0x0000C059
sudo busybox devmem 0x0c303018	# 0x0000C059

# 用devmem修改寄存器
sudo busybox devmem 0x0c303000 32 0x0000C400
sudo busybox devmem 0x0c303008 32 0x0000C458
sudo busybox devmem 0x0c303010 32 0x0000C400
sudo busybox devmem 0x0c303018 32 0x0000C458

# 改完后检查
sudo busybox devmem 0x0c303000	# 0x0000C400
sudo busybox devmem 0x0c303008	# 0x0000C458
sudo busybox devmem 0x0c303010	# 0x0000C400
sudo busybox devmem 0x0c303018	# 0x0000C458


sudo modprobe can
sudo modprobe can-raw
sudo modprobe can-dev
sudo modprobe mttcan

# 检查挂载
lsmod


# sudo ip link set can0 type can bitrate 250000
# sudo ip link set can1 type can bitrate 250000

# sudo ip link set can0 type can bitrate 500000
# sudo ip link set can1 type can bitrate 500000

# sudo ip link set can0 type can bitrate 1000000
# sudo ip link set can1 type can bitrate 1000000


sudo ip link set can0 type can bitrate 500000 dbitrate 2000000 berr-reporting on fd on
sudo ip link set can1 type can bitrate 500000 dbitrate 2000000 berr-reporting on fd on


# sudo ip link set can0 type can bitrate 1000000 loopback on
# sudo ip link set can1 type can bitrate 1000000 loopback on



sudo ip link set up can0
sudo ip link set up can1

# 检查
ifconfig

# 或者静态检查
# ip -details -statistics link show can0 
# ip -details -statistics link show can1

# 或者简写版的
# ip -s -d link show can0
# ip -s -d link show can1
