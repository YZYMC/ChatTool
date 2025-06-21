# ChatTool

**ChatTool** 是一个跨平台的命令行聊天工具，使用 C++ 编写，基于 TCP 通信，支持客户端/服务端结构、群聊与私聊、多语言显示等功能。

## 功能特性

- 支持 **群聊与私聊**
- 支持 **在线用户查询**
- 支持 **多语言**（自动或手动切换）
- **跨平台**支持

---

## 如何编译
**如果您没有编译条件，请前往`Release`下载预编译版本。**  
  
### 推荐的Linux编译方法

```bash
# 安装 CMake 和 g++
sudo apt install cmake g++ -y
mkdir build && cd build
cmake ..
make
```

### 推荐的 Windows 编译方法（使用 Visual Studio）

直接打开`ChatTool.sln`文件，然后生成解决方案即可

---

## 配置说明
**配置文件在编译时已经自带，您只需要修改即可**  
  
### 客户端配置文件 `client.ini`

```ini
[network]
host = 127.0.0.1
port = 29564

[user]
name = Alice
lang = zh
```

### 服务端配置文件 `server.ini`

```ini
[network]
port = 29564
```

---

## 使用方式

直接运行编译出来的可执行文件即可。  

---

## 多语言支持

ChatTool 支持多语言，当前内置语言包括：

* 中文（简体）: `zh`
* 英文: `en`

程序会根据系统语言自动选择语言，或通过 `-lang` 参数指定语言。

---

## License

本项目使用 MIT License。你可以自由修改、分发、使用 ChatTool。
