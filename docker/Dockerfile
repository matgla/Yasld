FROM archlinux:base-20230709.0.163418

RUN pacman --noconfirm -Syu
RUN pacman --noconfirm -Sy gcc clang cmake
RUN pacman -S --needed --noconfirm git base-devel
RUN mkdir -p /tmp/yay-build
RUN useradd -m -G wheel builder && passwd -d builder && chown -R builder:builder /tmp/yay-build
RUN echo 'builder ALL=(ALL) NOPASSWD: ALL' >> /etc/sudoers
RUN su - builder -c "git clone https://aur.archlinux.org/yay.git /tmp/yay-build/yay"
RUN su - builder -c "cd /tmp/yay-build/yay && makepkg -si --noconfirm"
RUN rm -rf /tmp/yay-build
RUN su - builder -c "yay --noconfirm -Syu && yay --noconfirm -Sy arm-none-eabi-gcc arm-none-eabi-gdb arm-none-eabi-newlib arm-none-eabi-binutils"
RUN pacman -Sy --noconfirm python-virtualenv
