# ĐỒ ÁN 2 HỆ ĐIỀU HÀNH: KERNEL LINUX

## Thông tin nhóm

- 1712273 Lâm Đức Anh
- 1712304 Hoàng Đức Công

# Hướng dẫn sử dụng:

## Cài đặt và biên dịch file thực thi 

- Mở terminal: Ctrl + Alt + T

- Cài đặt GCC (trên Ubuntu): sudo apt install gcc

- Chuyển đến thư mục chứa mã nguồn: cd /path/to/folder_open_or_folder_write

- Biên dịch file build kernel object: make

- Thêm kernel object vào kernel: sudo insmod <kernel_object_name>.ko với kernel_object_name = {open; write}

- Tháo kernel object: sudo rmmod <module_name> với module_name = {open; write}

