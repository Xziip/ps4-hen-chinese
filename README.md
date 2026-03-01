# PS4HEN v2.2.0 BETA (中文版)

## 功能特性
- 当前支持 5.05 - 12.02 固件版本
- 自制程序启用器
- 插件系统
  - 插件已集成到 HEN 中，此仓库中的 `hen.bin` 包含 [最新](https://github.com/Scene-Collective/ps4-hen-plugins/releases/latest) 标签的插件。
  - 更多信息请访问 [ps4-hen-plugins](https://github.com/Scene-Collective/ps4-hen-plugins) 页面。
- 越狱
- 沙盒逃逸
- 调试设置
- 外置硬盘支持
- VR 支持
- 远程软件包安装
- 休眠模式支持
- 外置硬盘格式化支持
- 绕过固件检查
- 调试奖杯支持
- sys_dynlib_dlsym 补丁
- UART 启用器
- 永不禁用截图
- 远程游玩启用器
- 禁用 ASLR

## 编译

以下说明适用于基于 Debian 的系统。（在 Ubuntu 上测试通过）

安装 [ps4-payload-sdk](https://github.com/Scene-Collective/ps4-payload-sdk)：

```sh
git clone https://github.com/Scene-Collective/ps4-payload-sdk.git
sudo ./install.sh
```

克隆此仓库：

```sh
git clone https://github.com/Scene-Collective/ps4-hen.git
```

编译载荷：

```sh
./build.sh
```

## 贡献者
特别感谢以下人员：
- [qwertyoruiopz](https://twitter.com/qwertyoruiopz)
- [Specter](https://twitter.com/SpecterDev) 
- [flat_z](https://twitter.com/flat_z)
- [idc](https://twitter.com/3226_2143)
- [Joonie](https://github.com/Joonie86/)
- [Vortex](https://github.com/xvortex)
- [zecoxao](https://twitter.com/notnotzecoxao)
- [SiSTRo](https://github.com/SiSTR0)
- [SocraticBliss](https://twitter.com/SocraticBliss)
- [ChendoChap](https://github.com/ChendoChap)
- [Biorn1950](https://github.com/Biorn1950)
- [Al-Azif](https://github.com/Al-Azif)
- Anonymous
- illusiony

## 移植协助
特别感谢以下人员：
- [BestPig](https://twitter.com/BestPig)
- [LM](https://twitter.com/LightningMods)
- [Al-Azif](https://twitter.com/_AlAzif)
- [zecoxao](https://twitter.com/notnotzecoxao)
