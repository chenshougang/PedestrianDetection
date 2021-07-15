sudo ip link set down can0
sudo ip link set down can1

# 检查
ifconfig	# 关闭的话里面就没有can0, can1了
