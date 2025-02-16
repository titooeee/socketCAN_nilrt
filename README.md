# Using SocketCAN (PCAN USB) in NI Real-Time Linux

By default, **SocketCAN** is **not enabled** in NI RT Linux. This guide covers how to **enable SocketCAN in the RT kernel** and configure **PCAN USB** for CAN communication.

## 1. Setting Up NI RT Linux
I installed NI RT Linux on an **old desktop** following [this guide](https://forums.ni.com/t5/NI-Linux-Real-Time-Discussions/Running-NI-Linux-RT-on-a-desktop-PC/td-p/3845207), but you can also use **Windows (cross-compilation required), Linux, PXI, or cRIO**.

Once installed:
- **Remote into RT Linux** using **VS Code + SSH** ([Setup Guide](https://code.visualstudio.com/docs/remote/ssh)).
- Install missing dependencies:
  ```bash
  opkg install git
  ```
- Verify necessary tools:
  ```bash
  make –version
  gcc –version
  bc –version
  flex –version
  bison –version
  depmod –version
  ls /usr/include/openssl/ssl.h
  ls /usr/include/elf.h
  ```
- Check the kernel version:
  ```bash
  cat /etc/os-release
  ```
  Example output:
  ```
  NAME="NI Linux Real-Time"
  VERSION="10.3 (kirkstone)"
  VERSION_ID=10.3
  ```

## 2. Building & Configuring the Kernel
Follow NI’s official documentation:
- 📺 [YouTube - Linux Kernel on NI Linux RT](https://www.youtube.com/watch?v=xxxxx)
- 📂 [GitHub - NI Linux Kernel README](https://github.com/ni/linux)

### Clone the kernel source and configure it:
```bash
git clone -b nilrt/24.5/kirkstone https://github.com/ni/linux.git  
cd linux  
export ARCH=x86_64  
make nati_x86_64_defconfig  
make menuconfig  
```

### Enable **SocketCAN & PCAN USB**:
1. Navigate to **CAN Bus Subsystem Support**
2. Go to **Device Drivers → Network Device Support → CAN Device Drivers → CAN USB Interfaces**
3. Enable **PEAK PCAN-USB**

### Compile & install:
```bash
make -j$(nproc) bzImage modules  
make modules_install  
```

### Update bootloader:
```bash
scp bzImage /boot/runmode/bzImage-6.1.92-rt32  
ln -sf bzImage-6.1.92-rt32 /boot/runmode/bzImage  
```

### Reboot and confirm installation:
```bash
lsmod | grep can  
ip link show | grep can  
```

## 3. Sending CAN Messages
Set up and send messages over CAN:
```bash
sudo ip link set can0 type can bitrate 500000  
sudo ip link set can0 up  

while true; do  
    cansend can0 123#11223344AABBCCDD  
    sleep 0.1  
done  
```
If a second CAN transceiver is connected, it should receive these messages.

## 4. Calling a Shared Library from LabVIEW
- Follow NI’s guide: [Using LabVIEW to Call .so Files in NI Linux RT](https://www.ni.com/en-us/support/documentation/supplemental/21/using-shared-libraries-on-ni-linux-real-time.html).
- Copy the shared library:
  ```bash
  scp libsocketcan_fd.so /home/lvuser/natinst/bin/libsocketcan_fd.so
  ```
- Add **Desktop RT Target** to a LabVIEW project.
- Use **Call Library Function Node** to interface with the shared library.
- Run `test.vi` to send CAN frames using **PCAN USB**.
