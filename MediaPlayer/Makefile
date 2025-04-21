CXX = g++ 
CXXFLAGS = -std=c++17 -Wall -I./ -Icontroller -Imodel -Iservice -Iview -Iutils -Ienum -Imain `pkg-config --cflags sdl2 SDL2_ttf`
LDFLAGS = `pkg-config --libs sdl2 SDL2_ttf SDL2_mixer` -ltag -lstdc++fs -lpthread

# Thư mục chứa mã nguồn
SRC_DIRS = controller model service utils view main

# Tự động tìm tất cả các file .cpp trong thư mục nguồn
SRCS = $(wildcard $(SRC_DIRS:%=%/*.cpp))
OBJS = $(SRCS:.cpp=.o)
TARGET = MediaPlayer

# Mục mặc định
all: $(TARGET)

# Liên kết file .o thành executable
$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

# Compile mỗi file .cpp thành .o tương ứng
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Xoá file build
clean:
	rm -f $(OBJS) $(TARGET)
