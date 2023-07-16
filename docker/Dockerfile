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
RUN pacman -Sy --noconfirm wget
RUN cd /opt && wget https://github.com/matgla/YasldToolchain/releases/download/release_0.1/yasld_toolchain_gcc-13.1.tar.xz && tar -xJvf yasld_toolchain_gcc-13.1.tar.xz && rm yasld_toolchain_gcc-13.1.tar.xz
ENV YASLD_TOOLCHAIN_PATH=/opt/gcc-13.1.0-with-pic
RUN su - builder -c "yay --noconfirm -Syu && yay --noconfirm -Sy renode"
RUN pip install --break-system-packages -r /opt/renode/tests/requirements.txt



